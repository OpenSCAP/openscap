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

#include <libxml/tree.h>

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

static int _write_text_to_fd_and_free(int output_fd, const char *text)
{
	ssize_t len = strlen(text);
	ssize_t written = 0;
	while (written < len) {
		ssize_t w = write(output_fd, text + written, len - written);
		if (w < 0)
			break;
		written += w;
	}
	oscap_free(text);
	return written != len;
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
	dI("Following script will be executed: '''%s'''\n", str);
	oscap_free(str);

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

static inline int _xccdf_fix_execute(struct xccdf_rule_result *rr, struct xccdf_fix *fix)
{
	const char *interpret = NULL;
	char *temp_dir = NULL;
	char *temp_file = NULL;
	char *fix_text = NULL;
	int fd;
	int result = 1;
	if (fix == NULL || rr == NULL || oscap_streq(xccdf_fix_get_content(fix), NULL))
		return 1;

	if ((interpret = _get_supported_interpret(xccdf_fix_get_system(fix), NULL)) == NULL) {
		_rule_add_info_message(rr, "Not supported xccdf:fix/@system='%s' or missing interpreter.",
				xccdf_fix_get_system(fix) == NULL ? "" : xccdf_fix_get_system(fix));
		return 1;
	}

	if (_xccdf_fix_decode_xml(fix, &fix_text) != 0) {
		_rule_add_info_message(rr, "Fix element contains unresolved child elements.");
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
			_rule_add_info_message(rr, "Fix execution completed and returned: %d", WEXITSTATUS(wstatus));
			if (stdout_buff != NULL && stdout_buff[0] != '\0')
				_rule_add_info_message(rr, stdout_buff);
			oscap_free(stdout_buff);
			/* We return zero to indicate success. Rather than returning the exit code. */
			result = 0;
		}
	} else {
		_rule_add_info_message(rr, "Failed to fork. %s", strerror(errno));
		oscap_free(temp_file);
	}

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
	struct oscap_list *fixes = _filter_fixes_by_applicability(policy, rule);
	const struct _interpret_map map[] = {
		{template, "Cloud!"},
		{NULL, NULL}
	};
	fixes = _filter_fixes_by_system(fixes, _search_interpret_map, map);
	fixes = _filter_fixes_by_distruption_and_reboot(fixes);
	struct xccdf_fix_iterator *fix_it = oscap_iterator_new(fixes);
	if (xccdf_fix_iterator_has_more(fix_it))
		fix = xccdf_fix_iterator_next(fix_it);
	xccdf_fix_iterator_free(fix_it);
	oscap_list_free0(fixes);
	return fix;
}

static inline int _xccdf_policy_rule_generate_fix(struct xccdf_policy *policy, struct xccdf_rule *rule, const char *template, int output_fd)
{
	// Ensure that given Rule is selected and applicable (CPE).
	const bool is_selected = xccdf_policy_is_item_selected(policy, xccdf_rule_get_id(rule));
	if (!is_selected) {
		dI("Skipping unselected Rule/@id=\"%s\"\n", xccdf_rule_get_id(rule));
		return 0;
	}
	const bool is_applicable = xccdf_policy_model_item_is_applicable(xccdf_policy_get_model(policy), (struct xccdf_item*)rule);
	if (!is_applicable) {
		dI("Skipping notapplicable Rule/@id\"%s\"\n", xccdf_rule_get_id(rule));
		return 0;
	}
	// Find the most suitable fix.
	const struct xccdf_fix *fix = _find_fix_for_template(policy, rule, template);
	if (fix == NULL) {
		dI("No fix element was found for Rule/@id=\"%s\"\n", xccdf_rule_get_id(rule));
		return 0;
	}
	dI("Processing a fix for Rule/@id=\"%s\"\n", xccdf_rule_get_id(rule));

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

	// Print-out the fix to the output_fd
	if (_write_text_to_fd_and_free(output_fd, fix_text) != 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "write of the fix to fd=%d failed: %s", output_fd, strerror(errno));
		return 1;
	}
	return 0;
}

static int _xccdf_policy_item_generate_fix(struct xccdf_policy *policy, struct xccdf_item *item, const char *template, int output_fd)
{
	int ret = 0;
	switch (xccdf_item_get_type(item)) {
	case XCCDF_GROUP:{
		struct xccdf_item_iterator *child_it = xccdf_group_get_content((struct xccdf_group *) item);
		while (xccdf_item_iterator_has_more(child_it)) {
			struct xccdf_item *child = xccdf_item_iterator_next(child_it);
			ret = _xccdf_policy_item_generate_fix(policy, child, template, output_fd);
			if (ret != 0)
				break;
		}
		xccdf_item_iterator_free(child_it);
		} break;
	case XCCDF_RULE:{
		ret = _xccdf_policy_rule_generate_fix(policy, (struct xccdf_rule *) item, template, output_fd);
		} break;
	default:
		assert(false);
		break;
	}
	return ret;
}

int xccdf_policy_generate_fix(struct xccdf_policy *policy, struct xccdf_result *result, const char *sys, int output_fd)
{
	__attribute__nonnull__(policy);

	if (result == NULL) {
		// No TestResult is available. Generate fix from the stock profile.
		dI("Generating fixes for policy(profile/@id=%s)\n", xccdf_policy_get_id(policy));
		int ret = 0;
		struct xccdf_benchmark *benchmark = xccdf_policy_get_benchmark(policy);
		struct xccdf_item_iterator *item_it = xccdf_benchmark_get_content(benchmark);
		while (xccdf_item_iterator_has_more(item_it)) {
			struct xccdf_item *item = xccdf_item_iterator_next(item_it);
			ret = _xccdf_policy_item_generate_fix(policy, item, sys, output_fd);
			if (ret != 0)
				break;
		}
		xccdf_item_iterator_free(item_it);
		return ret;
	}
	else {
		// TODO.
		return 1;
	}
}
