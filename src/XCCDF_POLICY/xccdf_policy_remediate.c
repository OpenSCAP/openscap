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

#ifdef OSCAP_UNIX
#include <sys/wait.h>
#endif

#include <unistd.h>

#include <libxml/tree.h>
#include <pcre.h>

#include "XCCDF/item.h"
#include "common/_error.h"
#include "common/assume.h"
#include "common/debug_priv.h"
#include "common/oscap_acquire.h"
#include "xccdf_policy_priv.h"
#include "xccdf_policy_model_priv.h"
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
	free(text);
	assume_ex(xccdf_message_set_severity(msg, XCCDF_MSG_INFO), 1);
	assume_ex(xccdf_rule_result_add_message(rr, msg), 1);
	return 0;
}

static inline bool _file_exists(const char *file)
{
	struct stat sb;
	return file != NULL && stat(file, &sb) == 0;
}

static int _write_text_to_fd(int output_fd, const char* text) {

	ssize_t written = 0;
	const ssize_t length = strlen(text);

	while (written < length) {
		ssize_t w = write(output_fd, text + written, length - written);
		if (w < 0)
			break;
		written += w;
	}

	return written != length;

}

static int _write_text_to_fd_and_free(int output_fd, char *text)
{
	const int ret = _write_text_to_fd(output_fd, text);
	free(text);
	return ret;
}

static int _write_remediation_to_fd_and_free(int output_fd, const char* template, char* text)
{
	if (oscap_streq(template, "urn:xccdf:fix:script:ansible")) {
		// Add required indentation in front of every single line

		const char delim = '\n';
		const char *indentation = "    ";

		char *current = text;
		char *next_delim = NULL;

		do {
			next_delim = strchr(current, delim);
			if (next_delim != NULL) {
				*next_delim = '\0';
			}

			// write indentation
			if (_write_text_to_fd(output_fd, indentation) != 0) {
				free(text);
				return 1;
			}

			// write rest of line
			if (_write_text_to_fd(output_fd, current) != 0) {
				free(text);
				return 1;
			}
			if (_write_text_to_fd(output_fd, "\n") != 0) {
				free(text);
				return 1;
			}

			if (next_delim != NULL) {
				// text is NULL terminated to this is guaranteed to point to valid memory
				current = next_delim + 1;
			}
		}
		while (next_delim != NULL);

		free(text);
		return 0;

	} else {
		// no extra processing is needed
		return _write_text_to_fd_and_free(output_fd, text);
	}
}
struct _interpret_map {
	const char *sys;
	const char *interpret;
};

typedef const char * (*_search_interpret_map_fn) (const char *, const struct _interpret_map *);

static const char *_search_interpret_map(const char *sys, const struct _interpret_map *map)
{
	const struct _interpret_map *mapptr;
	for (mapptr = map; mapptr->sys != NULL; ++mapptr)
		if (oscap_streq(mapptr->sys, sys))
			return mapptr->interpret;
	return NULL;
}

static const char *_get_supported_interpret(const char *sys, const struct _interpret_map *unused)
{
	static const struct _interpret_map _openscap_supported_interprets[] = {
		{"urn:xccdf:fix:commands",		"/bin/bash"},
		{"urn:xccdf:fix:script:sh",		"/bin/bash"},
		{"urn:xccdf:fix:script:perl",		"/usr/bin/perl"},
		{"urn:xccdf:fix:script:python",		"/usr/bin/python"},
		{"urn:xccdf:fix:script:csh",		"/bin/csh"},
		{"urn:xccdf:fix:script:tclsh",		"/usr/bin/tclsh"},
		{"urn:xccdf:fix:script:javascript",	"/usr/bin/js"},

		// Current Ansible remediations are only Ansible snippets and are
		// not runnable without header.
		// {"urn:xccdf:fix:script:ansible",	"/usr/bin/ansible-playbook"},
		{NULL,					NULL}
	};
	const char *interpret = _search_interpret_map(sys, _openscap_supported_interprets);
	return _file_exists(interpret) ? interpret : NULL;
}

static inline struct xccdf_rule *_lookup_rule_by_rule_result(const struct xccdf_policy *policy, const struct xccdf_rule_result *rr)
{
	const struct xccdf_benchmark *benchmark = xccdf_policy_get_benchmark(policy);
	if (benchmark == NULL)
		return NULL;
	return (struct xccdf_rule *) xccdf_benchmark_get_item(benchmark, xccdf_rule_result_get_idref(rr));
}

static inline bool _is_platform_applicable(struct xccdf_policy *policy, const char *platform)
{
	if (oscap_streq("", platform))
		return true;
	struct oscap_stringlist *platform_list = oscap_stringlist_new();
	assume_ex(oscap_stringlist_add_string(platform_list, platform), false);
	struct oscap_string_iterator *platform_it = oscap_stringlist_get_strings(platform_list);
	bool ret = xccdf_policy_model_platforms_are_applicable(xccdf_policy_get_model(policy), platform_it);
	oscap_string_iterator_free(platform_it);
	oscap_stringlist_free(platform_list);
	return ret;
}

static struct oscap_list *_get_fixes(struct xccdf_policy *policy, const struct xccdf_rule *rule)
{
	struct oscap_list *result = oscap_list_new();

	struct xccdf_fix_iterator *fix_it = xccdf_rule_get_fixes(rule);
	while (xccdf_fix_iterator_has_more(fix_it)) {
		struct xccdf_fix *fix = xccdf_fix_iterator_next(fix_it);
		oscap_list_add(result, fix);
	}
	xccdf_fix_iterator_free(fix_it);
	return result;
}

static struct oscap_list *_filter_fixes_by_applicability(struct xccdf_policy *policy, const struct xccdf_rule *rule)
{
	/* Filters out the fixes which are not applicable */
	struct oscap_list *result = oscap_list_new();
	if (!xccdf_policy_model_item_is_applicable(xccdf_policy_get_model(policy), (struct xccdf_item *) rule))
		/* The fix element is applicable only when the all the parent elements are. */
		return result;
	struct xccdf_fix_iterator *fix_it = xccdf_rule_get_fixes(rule);
	while (xccdf_fix_iterator_has_more(fix_it)) {
		struct xccdf_fix *fix = xccdf_fix_iterator_next(fix_it);
		const char *platform = xccdf_fix_get_platform(fix);
		if (_is_platform_applicable(policy, platform))
			oscap_list_add(result, fix);
	}
	xccdf_fix_iterator_free(fix_it);
	return result;
}

static struct oscap_list *_filter_fixes_by_system(struct oscap_list *fixes, _search_interpret_map_fn filter, const struct _interpret_map *allowed_systems)
{
	struct oscap_iterator *fix_it = oscap_iterator_new(fixes);
	while (oscap_iterator_has_more(fix_it)) {
		struct xccdf_fix *fix = (struct xccdf_fix *) oscap_iterator_next(fix_it);
		const char *sys = xccdf_fix_get_system(fix);
		if (sys == NULL)
			sys = "";
		if (filter(sys, allowed_systems) == NULL)
			oscap_iterator_detach(fix_it);
	}
	oscap_iterator_free(fix_it);
	return fixes;
}

static struct oscap_list *_filter_fixes_by_distruption_and_reboot(struct oscap_list *fixes)
{
	bool reboot = true;	// Let's assuming worse case and flip when fix/@rebot=false is found
	xccdf_level_t disruption = XCCDF_HIGH;

	struct oscap_iterator *fix_it = oscap_iterator_new(fixes);
	while (oscap_iterator_has_more(fix_it)) {
		struct xccdf_fix *fix = (struct xccdf_fix *) oscap_iterator_next(fix_it);
		if (!xccdf_fix_get_reboot(fix))
			reboot = false;
	}
	oscap_iterator_reset(fix_it);
	while (oscap_iterator_has_more(fix_it)) {
		struct xccdf_fix *fix = (struct xccdf_fix *) oscap_iterator_next(fix_it);
		if (reboot == false && xccdf_fix_get_reboot(fix)) {
			oscap_iterator_detach(fix_it);
		} else {
			xccdf_level_t dis = xccdf_fix_get_disruption(fix);
			if (dis == XCCDF_MEDIUM || dis == XCCDF_LOW)
				// Preferring "medium" and "low" over any other
				disruption = dis;
		}
	}
	if (disruption == XCCDF_MEDIUM || disruption == XCCDF_LOW) {
		oscap_iterator_reset(fix_it);
		while (oscap_iterator_has_more(fix_it)) {
			struct xccdf_fix *fix = (struct xccdf_fix *) oscap_iterator_next(fix_it);
			if (disruption != xccdf_fix_get_disruption(fix))
				oscap_iterator_detach(fix_it);
		}
	}
	oscap_iterator_free(fix_it);
	return fixes;
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
	const struct xccdf_rule *rule = _lookup_rule_by_rule_result(policy, rr);
	if (rule == NULL)
		return NULL;
	struct oscap_list *fixes = _filter_fixes_by_applicability(policy, rule);
	fixes = _filter_fixes_by_system(fixes, _get_supported_interpret, NULL);
	fixes = _filter_fixes_by_distruption_and_reboot(fixes);
	struct xccdf_fix_iterator *fix_it = oscap_iterator_new(fixes);
	if (xccdf_fix_iterator_has_more(fix_it))
		fix = xccdf_fix_iterator_next(fix_it);
	xccdf_fix_iterator_free(fix_it);
	oscap_list_free0(fixes);
	return fix;
}

static inline int _xccdf_fix_decode_xml(struct xccdf_fix *fix, char **result)
{
	/* We need to decode &amp; and similar sequences. That is a process reverse
	 * to the xmlEncodeSpecialChars()). Further we need to drop XML commentaries
	 * and expand CDATA blobs.
	 */
	*result = NULL;
	char *str = oscap_sprintf("<x xmlns:xhtml='http://www.w3.org/1999/xhtml'>%s</x>",
		xccdf_fix_get_content(fix));
        xmlDoc *doc = xmlReadMemory(str, strlen(str), NULL, NULL, XML_PARSE_RECOVER |
		XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET | XML_PARSE_NSCLEAN);
	dI("Following script will be executed: '''%s'''", str);
	free(str);

        xmlBuffer *buff = xmlBufferCreate();
	xmlNodePtr child = xmlDocGetRootElement(doc)->children;
	while (child != NULL) {
		switch (child->type) {
		case XML_ELEMENT_NODE:{
			/* Remaining child elements are suspicious. Perhaps it is an unresolved
			 * substitution element The execution would be dangerous, i.e. bash could
			 * interpret < and > characters of the element as pipe commands. */
			xmlFreeDoc(doc);
			xmlBufferFree(buff);
			return 1;
			}; break;
		case XML_TEXT_NODE:
		case XML_CDATA_SECTION_NODE:{
			xmlNodeBufGetContent(buff, child);
			}; break;
		default:
			break;
		}
		child = child->next;
        }
	xmlFreeDoc(doc);
	*result = oscap_strdup((char *)xmlBufferContent(buff));
	xmlBufferFree(buff);
	return 0;
}

#if defined(unix) || defined(__unix__) || defined(__unix)
static inline int _xccdf_fix_execute(struct xccdf_rule_result *rr, struct xccdf_fix *fix)
{
	if (fix == NULL || rr == NULL || oscap_streq(xccdf_fix_get_content(fix), NULL))
		return 1;

	const char *interpret = NULL;
	if ((interpret = _get_supported_interpret(xccdf_fix_get_system(fix), NULL)) == NULL) {
		_rule_add_info_message(rr, "Not supported xccdf:fix/@system='%s' or missing interpreter.",
				xccdf_fix_get_system(fix) == NULL ? "" : xccdf_fix_get_system(fix));
		return 1;
	}

	char *fix_text = NULL;
	if (_xccdf_fix_decode_xml(fix, &fix_text) != 0) {
		_rule_add_info_message(rr, "Fix element contains unresolved child elements.");
		return 1;
	}

	int result = 1;

	char *temp_dir = oscap_acquire_temp_dir();
	if (temp_dir == NULL)
		goto cleanup;
	// TODO: Directory and files shall be labeled with SELinux to prevent
	// confined processes with less priviledges to transit to oscap domain
	// and become basically unconfined.
	char *temp_file = NULL;
	int fd = oscap_acquire_temp_file(temp_dir, "fix-XXXXXXXX", &temp_file);
	if (fd == -1) {
		_rule_add_info_message(rr, "mkstemp failed: %s", strerror(errno));
		goto cleanup;
	}

	if (_write_text_to_fd_and_free(fd, fix_text) != 0) {
		_rule_add_info_message(rr, "Could not write to the temp file: %s", strerror(errno));
		(void) close(fd);
		goto cleanup;
	}

	if (close(fd) != 0)
		_rule_add_info_message(rr, "Could not close temp file: %s", strerror(errno));

	int pipefd[2];
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

			char *const envp[2] = {
				"PATH=/bin:/sbin:/usr/bin:/usr/sbin",
				NULL
			};

			execve(interpret, argvp, envp);
			/* Wow, execve returned. In this special case, we failed to execute the fix
			 * and we return 0 from function. At least the following error message will
			 * indicate the problem in xccdf:message. */
			printf("Error while executing fix script: execve returned: %s\n", strerror(errno));
			exit(42);
		} else {
			free(temp_file);
			close(pipefd[1]);
			char *stdout_buff = oscap_acquire_pipe_to_string(pipefd[0]);
			int wstatus;
			waitpid(fork_result, &wstatus, 0);
			_rule_add_info_message(rr, "Fix execution completed and returned: %d", WEXITSTATUS(wstatus));
			if (stdout_buff != NULL && stdout_buff[0] != '\0')
				_rule_add_info_message(rr, stdout_buff);
			free(stdout_buff);
			/* We return zero to indicate success. Rather than returning the exit code. */
			result = 0;
		}
	} else {
		_rule_add_info_message(rr, "Failed to fork. %s", strerror(errno));
		free(temp_file);
	}

cleanup:
	oscap_acquire_cleanup_dir(&temp_dir);
	return result;
}
#else
static inline int _xccdf_fix_execute(struct xccdf_rule_result *rr, struct xccdf_fix *fix)
{
	if (fix == NULL || rr == NULL || oscap_streq(xccdf_fix_get_content(fix), NULL))
		return 1;
	else
		_rule_add_info_message(rr, "Cannot execute the fix script: not implemented");
	return 1;
}
#endif

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

	struct xccdf_check *check = NULL;
	struct xccdf_check_iterator *check_it = xccdf_rule_result_get_checks(rr);
	while (xccdf_check_iterator_has_more(check_it))
		check = xccdf_check_iterator_next(check_it);
	xccdf_check_iterator_free(check_it);
	if (check != NULL && xccdf_check_get_multicheck(check))
		// Do not try to apply fix for multi-check.
		return 0;

	/* Initialize the fix. */
	struct xccdf_fix *cfix = xccdf_fix_clone(fix);
	int res = xccdf_policy_resolve_fix_substitution(policy, cfix, rr, test_result);
	xccdf_rule_result_add_fix(rr, cfix);
	if (res != 0) {
		_rule_add_info_message(rr, "Fix execution was aborted: Text substitution failed.");
		return res;
	}

	/* Execute the fix. */
	res = _xccdf_fix_execute(rr, cfix);
	if (res != 0) {
		_rule_add_info_message(rr, "Fix was not executed. Execution was aborted.");
		return res;
	}

	/* We report rule during remediation only when the fix was actually executed */
	int report = 0;
	struct xccdf_rule *rule = _lookup_rule_by_rule_result(policy, rr);
	if (rule == NULL) {
		// Sadly, we cannot handle this since b9d123d53140c6e369b7f2206e4e3e63dc556fd1.
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not find xccdf:Rule/@id=%s.", xccdf_rule_result_get_idref(rr));
	}
	else {
		report = xccdf_policy_report_cb(policy, XCCDF_POLICY_OUTCB_START, (void *) rule);
		if (report != 0)
			return report;
	}

	/* Verify applied fix by calling OVAL again */
	if (check == NULL) {
		xccdf_rule_result_set_result(rr, XCCDF_RESULT_ERROR);
		_rule_add_info_message(rr, "Failed to verify applied fix: Missing xccdf:check.");
	} else {
		int new_result = xccdf_policy_check_evaluate(policy, check);
		if (new_result == XCCDF_RESULT_PASS)
			xccdf_rule_result_set_result(rr, XCCDF_RESULT_FIXED);
		else {
			xccdf_rule_result_set_result(rr, XCCDF_RESULT_ERROR);
			_rule_add_info_message(rr, "Failed to verify applied fix: Checking engine returns: %s",
				new_result <= 0 ? "internal error" : xccdf_test_result_type_get_text(new_result));
		}
	}

	xccdf_rule_result_set_time_current(rr);
	return rule == NULL ? 0 : xccdf_policy_report_cb(policy, XCCDF_POLICY_OUTCB_END, (void *) rr);
}

int xccdf_policy_remediate(struct xccdf_policy *policy, struct xccdf_result *result)
{
	__attribute__nonnull__(result);
	struct xccdf_rule_result_iterator *rr_it = xccdf_result_get_rule_results(result);
	while (xccdf_rule_result_iterator_has_more(rr_it)) {
		struct xccdf_rule_result *rr = xccdf_rule_result_iterator_next(rr_it);
		xccdf_policy_rule_result_remediate(policy, rr, NULL, result);
	}
	xccdf_rule_result_iterator_free(rr_it);
	xccdf_result_set_end_time_current(result);
	return 0;
}

/* --- Follows functions for generating XCCDF:Fix script --- */
static const struct xccdf_fix *_find_fix_for_template(struct xccdf_policy *policy, struct xccdf_rule *rule, const char *template)
{
	struct xccdf_fix *fix = NULL;
	struct oscap_list *fixes = _get_fixes(policy, rule);

	if (template) {
		const struct _interpret_map map[] = {	{template, "Cloud!"},
							{NULL, NULL}};
		fixes = _filter_fixes_by_system(fixes, _search_interpret_map, map);
	}
	fixes = _filter_fixes_by_distruption_and_reboot(fixes);
	struct xccdf_fix_iterator *fix_it = oscap_iterator_new(fixes);
	if (xccdf_fix_iterator_has_more(fix_it))
		fix = xccdf_fix_iterator_next(fix_it);
	xccdf_fix_iterator_free(fix_it);
	oscap_list_free0(fixes);
	return fix;
}

static int _write_fix_header_to_fd(const char *sys, int output_fd, struct xccdf_rule *rule, unsigned int current, unsigned int total)
{
	if (oscap_streq(sys, "") || oscap_streq(sys, "urn:xccdf:fix:script:sh") || oscap_streq(sys, "urn:xccdf:fix:commands")) {
		char *fix_header = oscap_sprintf(
				"###############################################################################\n"
				"# BEGIN fix (%i / %i) for '%s'\n"
				"###############################################################################\n"
				"(>&2 echo \"Remediating rule %i/%i: '%s'\")\n",
				current, total, xccdf_rule_get_id(rule), current, total, xccdf_rule_get_id(rule));
		return _write_text_to_fd_and_free(output_fd, fix_header);
	} else {
		return 0;
	}
}

static int _write_fix_footer_to_fd(const char *sys, int output_fd, struct xccdf_rule *rule)
{
	if (oscap_streq(sys, "") || oscap_streq(sys, "urn:xccdf:fix:script:sh") || oscap_streq(sys, "urn:xccdf:fix:commands")) {
		char *fix_footer = oscap_sprintf("# END fix for '%s'\n\n", xccdf_rule_get_id(rule));
		return _write_text_to_fd_and_free(output_fd, fix_footer);
	} else {
		return 0;
	}
}

static int _write_fix_missing_warning_to_fd(const char *sys, int output_fd, struct xccdf_rule *rule)
{
	if (oscap_streq(sys, "") || oscap_streq(sys, "urn:xccdf:fix:script:sh") || oscap_streq(sys, "urn:xccdf:fix:commands")) {
		char *fix_footer = oscap_sprintf("# FIX FOR THIS RULE IS MISSING\n");
		return _write_text_to_fd_and_free(output_fd, fix_footer);
	} else {
		return 0;
	}
}

static inline int _xccdf_policy_rule_generate_fix_ansible(const char *template, int output_fd, const char *fix_text, bool ansible_variable_mode)
{
	// TODO: Tolerate different indentation styles in this regex
	const char *pattern =
		"- name: XCCDF Value [^ ]+ # promote to variable\n  set_fact:\n"
		"    ([^:]+): (.+)\n  tags:\n    - always\n";
	const char *err;
	int errofs;

	pcre *re = pcre_compile(pattern, PCRE_UTF8, &err, &errofs, NULL);
	if (re == NULL) {
		dE("Unable to compile regex pattern, "
				"pcre_compile() returned error (offset: %d): '%s'.\n", errofs, err);
		return 1;
	}

	// ovector sizing:
	// 2 elements are used for the whole needle,
	// 4 elements are used for the 2 capture groups
	// pcre documentation says we should allocate a third extra for additional
	// workspace.
	// (2 + 4) * (3 / 2) = 9
	int ovector[9];

	const size_t fix_text_len = strlen(fix_text);
	int start_offset = 0;
	while (true) {
		const int match = pcre_exec(re, NULL, fix_text, fix_text_len, start_offset,
				0, ovector, sizeof(ovector) / sizeof(ovector[0]));
		if (match == -1)
			break;
		if (match != 3) {
			dE("Expected 2 capture group matches per XCCDF variable. Found %i!",
				match - 1);
			pcre_free(re);
			return 1;
		}

		if (ansible_variable_mode) {
			// ovector[0] and [1] hold the start and end of the whole needle match
			// ovector[2] and [3] hold the start and end of the first capture group
			// ovector[4] and [5] hold the start and end of the second capture group
			char *variable_name = malloc((ovector[3] - ovector[2] + 1) * sizeof(char));
			memcpy(variable_name, &fix_text[ovector[2]], ovector[3] - ovector[2]);
			variable_name[ovector[3] - ovector[2]] = '\0';

			char *variable_value = malloc((ovector[5] - ovector[4] + 1) * sizeof(char));
			memcpy(variable_value, &fix_text[ovector[4]], ovector[5] - ovector[4]);
			variable_value[ovector[5] - ovector[4]] = '\0';

			char *var_line = oscap_sprintf("  %s: %s", variable_name, variable_value);

			free(variable_name);
			free(variable_value);

			if (_write_remediation_to_fd_and_free(output_fd, template, var_line) != 0) {
				pcre_free(re);
				return 1;
			}
		}
		else {
			char *remediation_part = malloc((ovector[0] + 1) * sizeof(char));
			memcpy(remediation_part, &fix_text[start_offset], ovector[0]);
			remediation_part[ovector[0]] = '\0';
			if (_write_remediation_to_fd_and_free(output_fd, template, remediation_part) != 0) {
				pcre_free(re);
				return 1;
			}
		}

		start_offset = ovector[1]; // next time start after the entire pattern
	}

	if (!ansible_variable_mode && fix_text_len - start_offset > 0) {
		char *remediation_part = malloc((fix_text_len - start_offset + 1) * sizeof(char));
		memcpy(remediation_part, &fix_text[start_offset], fix_text_len - start_offset);
		remediation_part[fix_text_len - start_offset] = '\0';

		if (_write_remediation_to_fd_and_free(output_fd, template, remediation_part) != 0) {
			pcre_free(re);
			return 1;
		}
	}

	pcre_free(re);
	return 0;
}

static inline int _xccdf_policy_rule_generate_fix(struct xccdf_policy *policy, struct xccdf_rule *rule, const char *template, int output_fd, unsigned int current, unsigned int total, bool ansible_variable_mode)
{
	// Ensure that given Rule is selected and applicable (CPE).
	const bool is_selected = xccdf_policy_is_item_selected(policy, xccdf_rule_get_id(rule));
	if (!is_selected) {
		dI("Skipping unselected Rule/@id=\"%s\"", xccdf_rule_get_id(rule));
		return 0;
	}
	// Find the most suitable fix.
	const struct xccdf_fix *fix = _find_fix_for_template(policy, rule, template);
	if (fix == NULL) {
		int ret = _write_fix_header_to_fd(template, output_fd, rule, current, total);
		if (ret != 0)
			return ret;
		ret = _write_fix_missing_warning_to_fd(template, output_fd, rule);
		if (ret != 0)
			return ret;
		ret = _write_fix_footer_to_fd(template, output_fd, rule);
		dI("No fix element was found for Rule/@id=\"%s\"", xccdf_rule_get_id(rule));
		return ret;
	}
	dI("Processing a fix for Rule/@id=\"%s\"", xccdf_rule_get_id(rule));

	// Process Text Substitute within the fix
	struct xccdf_fix *cfix = xccdf_fix_clone(fix);
	int res = xccdf_policy_resolve_fix_substitution(policy, cfix, NULL, NULL);
	if (res != 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "A fix for Rule/@id=\"%s\" was skipped: Text substitution failed.",
				xccdf_rule_get_id(rule));
		xccdf_fix_free(cfix);
		return res == 1; // Value 2 indicates warning.
	}
	// Refine. Resolve XML comments, CDATA and remaining elements
	char *fix_text = NULL;
	if (_xccdf_fix_decode_xml(cfix, &fix_text) != 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "A fix element for Rule/@id=\"%s\" contains unresolved child elements.",
				xccdf_rule_get_id(rule));
		xccdf_fix_free(cfix);
		return 1;
	}
	xccdf_fix_free(cfix);

	int ret = _write_fix_header_to_fd(template, output_fd, rule, current, total);
	if (ret != 0) {
		free(fix_text);
		return ret;
	}

	if (oscap_streq(template, "urn:xccdf:fix:script:ansible")) {
		// Ansible is special because we have two output modes, variable and
		// task mode. In both cases we have to do post processing. 

		ret = _xccdf_policy_rule_generate_fix_ansible(template, output_fd, fix_text, ansible_variable_mode);
		free(fix_text);
	}
	else {
		ret = _write_remediation_to_fd_and_free(output_fd, template, fix_text);
		if (ret != 0) {
			return ret;
		}
	}

	ret = _write_fix_footer_to_fd(template, output_fd, rule);
	return ret;
}

static int _xccdf_item_recursive_gather_selected_rules(struct xccdf_policy *policy, struct xccdf_item *item, struct oscap_list *rule_list)
{
	int ret = 0;
	const bool is_selected = xccdf_policy_is_item_selected(policy, xccdf_item_get_id(item));
	if (!is_selected) {
		return ret;
	}
	switch (xccdf_item_get_type(item)) {
	case XCCDF_GROUP:{
		struct xccdf_item_iterator *child_it = xccdf_group_get_content((struct xccdf_group *) item);
		while (xccdf_item_iterator_has_more(child_it)) {
			struct xccdf_item *child = xccdf_item_iterator_next(child_it);
			ret = _xccdf_item_recursive_gather_selected_rules(policy, child, rule_list);
			if (ret != 0)
				break;
		}
		xccdf_item_iterator_free(child_it);
		} break;
	case XCCDF_RULE:{
		oscap_list_add(rule_list, (struct xccdf_rule *) item);
		} break;
	default:
		assert(false);
		break;
	}
	return ret;
}

static int _write_script_header_to_fd(struct xccdf_policy *policy, struct xccdf_result *result, const char *sys, int output_fd)
{
	if (!(oscap_streq(sys, "") || oscap_streq(sys, "urn:xccdf:fix:script:sh") || oscap_streq(sys, "urn:xccdf:fix:commands") ||
		oscap_streq(sys, "urn:xccdf:fix:script:ansible")))
		return 0; // no header required

	const bool ansible_script = strcmp(sys, "urn:xccdf:fix:script:ansible") == 0;
	const char *how_to_apply = ansible_script ?
		"# $ ansible-playbook -i \"192.168.1.155,\" playbook.yml\n"
		"# $ ansible-playbook -i inventory.ini playbook.yml" :
		"# $ sudo ./remediation-role.sh";
	const char *oscap_version = oscap_get_version();
	const char *format = sys != NULL ? sys : "";
	const char *template = sys != NULL ? " --template " : "";

	char *fix_header;
	if (result == NULL) {
		// Profile-based remediation fix
		struct xccdf_profile *profile = xccdf_policy_get_profile(policy);
		const char *profile_id = xccdf_profile_get_id(profile);
		struct xccdf_benchmark *benchmark = xccdf_policy_get_benchmark(policy);
		if (benchmark == NULL) {
			return 1;
		}
		// Title
		struct oscap_text_iterator *title_iterator = xccdf_profile_get_title(profile);
		char *profile_title = oscap_textlist_get_preferred_plaintext(title_iterator, NULL);
		oscap_text_iterator_free(title_iterator);
		// Description
		struct oscap_text_iterator *description_iterator = xccdf_profile_get_description(profile);
		char *profile_description = description_iterator != NULL ?
				oscap_textlist_get_preferred_plaintext(description_iterator, NULL) : NULL;
		oscap_text_iterator_free(description_iterator);

		const char *benchmark_version_info = xccdf_benchmark_get_version(benchmark);
		const char *benchmark_id = xccdf_benchmark_get_id(benchmark);
		const struct xccdf_version_info *xccdf_version = xccdf_benchmark_get_schema_version(benchmark);
		const char *xccdf_version_name = xccdf_version_info_get_version(xccdf_version);

		if (NULL != profile_description) {
			size_t new_lines = 0;
			size_t description_length = 1;
			for (const char *c = profile_description; *c != '\0'; ++c, ++description_length)
				if (*c == '\n')
					++new_lines;

			if (new_lines > 0) {
				const char filler[] = "# ";
				char *commented_description = malloc(description_length + new_lines * (sizeof filler - 1));
				for (size_t i = 0, j = 0; j < description_length; ++i, ++j) {
					commented_description[i] = profile_description[j];
					if (profile_description[j] == '\n') {
						for (size_t k = 0; k < (sizeof filler - 1); ++k)
							commented_description[++i] = filler[k];
					}
				}
				free(profile_description);
				profile_description = commented_description;
			}
		}

		fix_header = oscap_sprintf(
			"###############################################################################\n#\n"
			"# %s remediation role for profile %s\n"
			"# Profile Title:  %s\n"
			"# Profile Description:\n"
			"# %s\n"
			"#\n"
			"# Benchmark ID:  %s\n"
			"# Benchmark Version:  %s\n#\n"
			"# XCCDF Version:  %s\n#\n"
			"# This file was generated by OpenSCAP %s using:\n"
			"# 	$ oscap xccdf generate fix --profile %s%s%s sds.xml \n#\n"
			"# This script is generated from an OpenSCAP profile without preliminary evaluation.\n"
			"# It attempts to fix every selected rule, even if the system is already compliant.\n"
			"#\n"
			"# How to apply this remediation role:\n"
			"%s\n"
			"#\n"
			"###############################################################################\n\n",
				ansible_script ? "Ansible" : "Bash", profile_id, profile_title, profile_description != NULL ? profile_description : "Not available", benchmark_id,
				benchmark_version_info, xccdf_version_name, oscap_version, profile_id, template, format, how_to_apply);

		free(profile_title);
		free(profile_description);

	} else {
		// Results-based remediation fix
		const char *start_time = xccdf_result_get_start_time(result);
		const char *end_time = xccdf_result_get_end_time(result);
		const char *result_id = xccdf_result_get_id(result);
		const struct xccdf_version_info *xccdf_version = xccdf_result_get_schema_version(result);
		const char *xccdf_version_name = xccdf_version_info_get_version(xccdf_version);

		fix_header = oscap_sprintf(
			"###############################################################################\n#\n"
			"# %s remediation role for the results of evaluation of profile %s \n"
			"# XCCDF Version:  %s\n#\n"
			"# Evaluation Start Time:  %s\n"
			"# Evaluation End Time:  %s\n#\n"
			"# This file was generated by OpenSCAP %s using:\n"
			"# 	$ oscap xccdf generate fix --result-id %s%s%s xccdf-results.xml \n#\n"
			"# This script is generated from the results of a profile evaluation.\n"
			"# It attempts to remediate all issues from the selected rules that failed the test.\n"
			"#\n"
			"# How to apply this remediation role:\n"
			"%s\n"
			"#\n"
			"###############################################################################\n\n",
				ansible_script ? "Ansible" : "Bash", xccdf_result_get_profile(result), xccdf_version_name,
				start_time != NULL ? start_time : "Unknown",
				end_time, oscap_version, result_id, template, format, how_to_apply);
	}

	if (ansible_script) {
		char *ansible_fix_header = oscap_sprintf(
			"---\n"
			"%s\n"
			" - hosts: all\n",
			fix_header);
		free(fix_header);
		return _write_text_to_fd_and_free(output_fd, ansible_fix_header);
	} else {
		return _write_text_to_fd_and_free(output_fd, fix_header);
	}
}

int xccdf_policy_generate_fix(struct xccdf_policy *policy, struct xccdf_result *result, const char *sys, int output_fd)
{
	__attribute__nonnull__(policy);
	int ret = 0;

	struct oscap_list *rules_to_fix = oscap_list_new();
	if (result == NULL) {
		// No TestResult is available. Generate fix from the stock profile.
		dI("Generating profile-oriented fixes for policy(profile/@id=%s)", xccdf_policy_get_id(policy));
		struct xccdf_benchmark *benchmark = xccdf_policy_get_benchmark(policy);

		if (benchmark == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not find benchmark model for policy id='%s' when generating fixes.", xccdf_policy_get_id(policy));
			oscap_list_free(rules_to_fix, NULL);
			return 1;
		}

		if (_write_script_header_to_fd(policy, result, sys, output_fd) != 0) {
			oscap_list_free(rules_to_fix, NULL);
			return 1;
		}

		struct xccdf_item_iterator *item_it = xccdf_benchmark_get_content(benchmark);
		while (xccdf_item_iterator_has_more(item_it)) {
			struct xccdf_item *item = xccdf_item_iterator_next(item_it);
			ret = _xccdf_item_recursive_gather_selected_rules(policy, item, rules_to_fix);
			if (ret != 0)
				break;
		}
		xccdf_item_iterator_free(item_it);
	}
	else {
		dI("Generating result-oriented fixes for policy(result/@id=%s)", xccdf_result_get_id(result));

		if (_write_script_header_to_fd(policy, result, sys, output_fd) != 0) {
			oscap_list_free(rules_to_fix, NULL);
			return 1;
		}

		struct xccdf_rule_result_iterator *rr_it = xccdf_result_get_rule_results(result);
		while (xccdf_rule_result_iterator_has_more(rr_it)) {
			struct xccdf_rule_result *rr = xccdf_rule_result_iterator_next(rr_it);
			if (xccdf_rule_result_get_result(rr) != XCCDF_RESULT_FAIL)
				continue;
			struct xccdf_rule *rule = _lookup_rule_by_rule_result(policy, rr);
			oscap_list_add(rules_to_fix, rule);
		}
		xccdf_rule_result_iterator_free(rr_it);
	}

	const unsigned int total = oscap_list_get_itemcount(rules_to_fix);

	// In Ansible we have to generate variables first and then tasks
	if (strcmp(sys, "urn:xccdf:fix:script:ansible") == 0) {
		_write_text_to_fd(output_fd, "   vars:\n");

		unsigned int current = 1;
		struct oscap_iterator *rules_to_fix_it = oscap_iterator_new(rules_to_fix);
		while (oscap_iterator_has_more(rules_to_fix_it)) {
			struct xccdf_rule *rule = (struct xccdf_rule*)oscap_iterator_next(rules_to_fix_it);
			ret = _xccdf_policy_rule_generate_fix(policy, rule, sys, output_fd, current++, total, true);
			if (ret != 0)
				break;
		}
		oscap_iterator_free(rules_to_fix_it);

		_write_text_to_fd(output_fd, "   tasks:\n");
	}

	unsigned int current = 1;
	struct oscap_iterator *rules_to_fix_it = oscap_iterator_new(rules_to_fix);
	while (oscap_iterator_has_more(rules_to_fix_it)) {
		struct xccdf_rule *rule = (struct xccdf_rule*)oscap_iterator_next(rules_to_fix_it);
		ret = _xccdf_policy_rule_generate_fix(policy, rule, sys, output_fd, current++, total, false);
		if (ret != 0)
			break;
	}
	oscap_iterator_free(rules_to_fix_it);

	oscap_list_free(rules_to_fix, NULL);

	return ret;
}
