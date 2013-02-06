/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common/assume.h"
#include "common/debug_priv.h"
#include "common/oscap_acquire.h"
#include "xccdf_policy_priv.h"
#include "public/xccdf_policy.h"

static int _rule_add_info_message(struct xccdf_rule_result *rr, ...)
{
	va_list ap;
	const char *fmt;
	char *text;
	struct xccdf_message *msg;

	va_start(ap, rr);
	fmt = va_arg(ap, const char *);
	text = oscap_vsprintf(fmt, ap);
	va_end(ap);

	msg = xccdf_message_new();
	assume_ex(xccdf_message_set_content(msg, text), 1);
	oscap_free(text);
	assume_ex(xccdf_message_set_severity(msg, XCCDF_MSG_INFO), 1);
	assume_ex(xccdf_rule_result_add_message(rr, msg), 1);
	return 0;
}

static inline bool _file_exists(const char *file)
{
	struct stat sb;
	return file != NULL && stat(file, &sb) == 0;
}

static const char *_get_interpret(const char *sys)
{
	const char *interpret = NULL;
	if (oscap_streq(sys, NULL) ||
			oscap_streq(sys, "urn:xccdf:fix:commands") ||
			oscap_streq(sys, "urn:xccdf:fix:script:sh")) {
		interpret = "/bin/bash";
	} else if (oscap_streq(sys, "urn:xccdf:fix:script:csh")) {
		interpret = "/bin/csh";
	} else if (oscap_streq(sys, "urn:xccdf:fix:script:perl")) {
		interpret = "/usr/bin/perl";
	} else if (oscap_streq(sys, "urn:xccdf:fix:script:python")) {
		interpret = "/usr/bin/python";
	} else if (oscap_streq(sys, "urn:xccdf:fix:script:tclsh")) {
		interpret = "/usr/bin/tclsh";
	} else if (oscap_streq(sys, "urn:xccdf:fix:script:javascript")) {
		interpret = "/usr/bin/js";
	}

	return _file_exists(interpret) ? interpret : NULL;
}

static inline struct xccdf_fix *_find_suitable_fix(struct xccdf_policy *policy, struct xccdf_rule_result *rr)
{
	/* In XCCDF 1.2, there is nothing like a default fix. However we use
	 * the following heuristics to find out some suitable fix:
	 * 	- remove fixes which are not appplicable (CPE)
	 * 	- remove fixes we cannot execute
	 * 	- choose fixes with the least disruption
	 * 	- choose fixes which do not require reboot
	 * 	- choose the first fix
	 */
	struct xccdf_fix *fix = NULL;
	const struct xccdf_policy_model *model = xccdf_policy_get_model(policy);
	assume_ex(model != NULL, NULL);
	const struct xccdf_benchmark *benchmark = xccdf_policy_model_get_benchmark(model);
	assume_ex(model != NULL, NULL);
	const struct xccdf_item *rule = xccdf_benchmark_get_item(benchmark, xccdf_rule_result_get_idref(rr));
	assume_ex(rule != NULL, NULL);
	struct xccdf_fix_iterator *fix_it = xccdf_rule_get_fixes((const struct xccdf_rule *) rule);
	if (xccdf_fix_iterator_has_more(fix_it))
		fix = xccdf_fix_iterator_next(fix_it);
	xccdf_fix_iterator_free(fix_it);
	return fix; // TODO: implement the procedure described above
}

static inline int _xccdf_fix_execute(struct xccdf_rule_result *rr, struct xccdf_fix *fix)
{
	const char *interpret = NULL;
	char *temp_dir = NULL;
	char *temp_file = NULL;
	int fd;
	int result = 1;
	if (fix == NULL || rr == NULL || oscap_streq(xccdf_fix_get_content(fix), NULL))
		return 1;

	if ((interpret = _get_interpret(xccdf_fix_get_system(fix))) == NULL) {
		_rule_add_info_message(rr, "Not supported xccdf:fix/@system='%s' or missing interpreter.",
				xccdf_fix_get_system(fix) == NULL ? "" : xccdf_fix_get_system(fix));
		return 1;
	}

	temp_dir = oscap_acquire_temp_dir();
	if (temp_dir == NULL)
		goto cleanup;
	// TODO: Directory and files shall be labeled with SELinux to prevent
	// confined processes with less priviledges to transit to oscap domain
	// and become basically unconfined.
	fd = oscap_acquire_temp_file(temp_dir, "fix-XXXXXXXX", &temp_file);
	if (fd == -1) {
		_rule_add_info_message(rr, "mkstemp failed: %s", strerror(errno));
		goto cleanup;
	}

	int err = write(fd, xccdf_fix_get_content(fix), strlen(xccdf_fix_get_content(fix)));
	if (err < 1) {
		_rule_add_info_message(rr, "Could not write to the temp file: %s", strerror(errno));
		(void) close(fd);
		goto cleanup;
	}

	if (close(fd) != 0)
		_rule_add_info_message(rr, "Could not close temp file: %s", strerror(errno));

	int pipefd[1];
	if (pipe(pipefd) == -1) {
		_rule_add_info_message(rr, "Could not create pipe: %s", strerror(errno));
		goto cleanup;
	}

	int fork_result = fork();
	if (fork_result >= 0) {
		/* fork succeded */
		if (fork_result == 0) {
			/* Execute fix and forward output to the parrent. */
			close(pipefd[0]);
			dup2(pipefd[1], fileno(stdout));
			dup2(pipefd[1], fileno(stderr));
			close(pipefd[1]);

			char *const argvp[3] = {
				(char *)interpret,
				temp_file,
				NULL
			};

			execve(interpret, argvp, NULL);
			/* Wow, execve returned. In this special case, we failed to execute the fix
			 * and we return 0 from function. At least the following error message will
			 * indicate the problem in xccdf:message. */
			printf("Error while executing fix script: execve returned: %s\n", strerror(errno));
			exit(42);
		} else {
			oscap_free(temp_file);
			close(pipefd[1]);
			const char *stdout_buff = oscap_acquire_pipe_to_string(pipefd[0]);
			int wstatus;
			waitpid(fork_result, &wstatus, 0);
			_rule_add_info_message(rr, "Fix execution comleted and returned: %d", WEXITSTATUS(wstatus));
			_rule_add_info_message(rr, stdout_buff);
			oscap_free(stdout_buff);
			/* We return zero to indicate success. Rather than returning the exit code. */
			result = 0;
		}
	}
	else
		_rule_add_info_message(rr, "Failed to fork. %s", strerror(errno));

cleanup:
	oscap_acquire_cleanup_dir(&temp_dir);
	return result;
}

int xccdf_policy_rule_result_remediate(struct xccdf_policy *policy, struct xccdf_rule_result *rr, struct xccdf_fix *fix, struct xccdf_result *test_result)
{
	if (policy == NULL || rr == NULL)
		return 1;
	if (xccdf_rule_result_get_result(rr) != XCCDF_RESULT_FAIL)
		return 0;

	if (fix == NULL) {
		fix = _find_suitable_fix(policy, rr);
		if (fix == NULL)
			// We may want to append xccdf:message about missing fix.
			return 0;
	}

	/* Initialize the fix. */
	struct xccdf_fix *cfix = xccdf_fix_clone(fix);
	int res = xccdf_policy_resolve_fix_substitution(policy, cfix, test_result);
	xccdf_rule_result_add_fix(rr, cfix);
	if (res != 0) {
		_rule_add_info_message(rr, "Fix execution was aborted: Text substitution failed.");
		return res;
	}

	/* Execute the fix. */
	res = _xccdf_fix_execute(rr, cfix);
	if (res != 0) {
		_rule_add_info_message(rr, "Fix execution was aborted.");
		return res;
	}

	/* Verify applied fix by calling OVAL again */
	// TODO.
	return -666;
}
