/*
 * Copyright 2010--2014 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      Peter Vrabec   <pvrabec@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(OVAL_PROBES_ENABLED)
# include <oval_probe.h>
#endif
#include <oval_agent_api.h>
#include <oval_agent_xccdf_api.h>
#include <oval_results.h>
#include <oval_variables.h>

#include <scap_ds.h>
#include <xccdf_benchmark.h>
#include <xccdf_policy.h>
#include <xccdf_session.h>
#include <ds_rds_session.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#ifdef OS_WINDOWS
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif
#if defined(HAVE_SYSLOG_H)
#include <syslog.h>
#endif

#include "oscap-tool.h"
#include "oscap.h"
#include "oscap_source.h"
#include <oscap_debug.h>
#include "oscap_helpers.h"

#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif

static int app_evaluate_xccdf(const struct oscap_action *action);
static int app_xccdf_validate(const struct oscap_action *action);
static int app_xccdf_resolve(const struct oscap_action *action);
static int app_xccdf_export_oval_variables(const struct oscap_action *action);
static int app_xccdf_remediate(const struct oscap_action *action);
static bool getopt_xccdf(int argc, char **argv, struct oscap_action *action);
static bool getopt_generate(int argc, char **argv, struct oscap_action *action);
static int app_xccdf_xslt(const struct oscap_action *action);
static int app_generate_fix(const struct oscap_action *action);
static int app_generate_guide(const struct oscap_action *action);

#define XCCDF_SUBMODULES_NUM		7
#define XCCDF_GEN_SUBMODULES_NUM	5 /* See actual arrays
						initialization below. */
static struct oscap_module* XCCDF_SUBMODULES[XCCDF_SUBMODULES_NUM];
static struct oscap_module* XCCDF_GEN_SUBMODULES[XCCDF_GEN_SUBMODULES_NUM];

struct oscap_module OSCAP_XCCDF_MODULE = {
    .name = "xccdf",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "eXtensible Configuration Checklist Description Format",
    .usage_extra = "command [command-specific-options]",
    .submodules = XCCDF_SUBMODULES
};

static struct oscap_module XCCDF_RESOLVE = {
    .name = "resolve",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Resolve an XCCDF document",
    .usage = "[options] -o output-xccdf.xml input-xccdf.xml",
    .help =
		"Options:\n"
		"   --force                       - Force resolving XCCDF document even if it is aleready marked as resolved.",
    .opt_parser = getopt_xccdf,
    .func = app_xccdf_resolve
};

static struct oscap_module XCCDF_VALIDATE = {
    .name = "validate",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Validate XCCDF XML content",
    .usage = "xccdf-file.xml",
    .opt_parser = getopt_xccdf,
	.func = app_xccdf_validate,
	.help = "Options:\n"
		"   --skip-schematron             - Do not use schematron-based validation in addition to XML Schema\n"
	,
};

static struct oscap_module XCCDF_EXPORT_OVAL_VARIABLES = {
    .name = "export-oval-variables",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Export XCCDF values as OVAL external-variables document(s)",
    .usage = "[options] <xccdf benchmark file> [oval definitions files]",
    .opt_parser = getopt_xccdf,
    .func = app_xccdf_export_oval_variables,
	.help =	"Options:\n"
		"   --profile <name>              - The name of Profile to be evaluated.\n"
		"   --skip-validation             - Skip validation.\n"
		"   --fetch-remote-resources      - Download remote content referenced by XCCDF.\n"
		"   --local-files <dir>           - Use locally downloaded copies of remote resources stored in the given directory.\n"
		"   --datastream-id <id>          - ID of the data stream in the collection to use.\n"
		"                                   (only applicable for source data streams)\n"
		"   --xccdf-id <id>               - ID of component-ref with XCCDF in the data stream that should be evaluated.\n"
		"                                   (only applicable for source data streams)\n"
		"   --benchmark-id <id>           - ID of XCCDF Benchmark in some component in the data stream that should be evaluated.\n"
		"                                   (only applicable for source data streams)\n"
		"                                   (only applicable when datastream-id AND xccdf-id are not specified)\n"
		"   --cpe <name>                  - Use given CPE dictionary or language (autodetected)\n"
		"                                   for applicability checks.\n"
	,
};

static struct oscap_module XCCDF_EVAL = {
    .name = "eval",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Perform evaluation driven by XCCDF file and use OVAL as checking engine",
    .usage = "[options] INPUT_FILE [oval-definitions-files]",
    .help =
		"INPUT_FILE - XCCDF file or a source data stream file\n\n"
		"Options:\n"
		"   --profile <name>              - The name of Profile to be evaluated.\n"
		"   --rule <name>                 - The name of a single rule to be evaluated.\n"
		"   --skip-rule <name>            - The name of the rule to be skipped.\n"
		"   --reference <NAME:ID>         - Evaluate only rules that have the given reference.\n"
		"   --tailoring-file <file>       - Use given XCCDF Tailoring file.\n"
		"   --tailoring-id <component-id> - Use given DS component as XCCDF Tailoring file.\n"
		"   --cpe <name>                  - Use given CPE dictionary or language (autodetected)\n"
		"                                   for applicability checks.\n"
		"   --oval-results                - Save OVAL results as well.\n"
		"   --check-engine-results        - Save results from check engines loaded from plugins as well.\n"
		"   --export-variables            - Export OVAL external variables provided by XCCDF.\n"
		"   --results <file>              - Write XCCDF Results into file.\n"
		"   --results-arf <file>          - Write ARF (result data stream) into file.\n"
		"   --stig-viewer <file>          - Writes XCCDF results into FILE in a format readable by DISA STIG Viewer\n"
		"   --thin-results                - Thin Results provides only minimal amount of information in OVAL/ARF results.\n"
		"                                   The option --without-syschar is automatically enabled when you use Thin Results.\n"
		"   --without-syschar             - Don't provide system characteristic in OVAL/ARF result files.\n"
		"   --report <file>               - Write HTML report into file.\n"
		"   --skip-validation             - Skip validation.\n"
		"   --skip-signature-validation   - Skip data stream signature validation.\n"
		"                                   (only applicable for source data streams)\n"
		"   --enforce-signature           - Process only signed data streams.\n"
		"   --fetch-remote-resources      - Download remote content referenced by XCCDF.\n"
		"   --local-files <dir>           - Use locally downloaded copies of remote resources stored in the given directory.\n"
		"   --progress                    - Switch to sparse output suitable for progress reporting.\n"
		"                                   Format is \"$rule_id:$result\\n\".\n"
		"   --progress-full               - Switch to sparse but a bit more saturated output also suitable for progress reporting.\n"
		"                                   Format is \"$rule_id|$rule_title|$result\\n\".\n"
		"   --datastream-id <id>          - ID of the data stream in the collection to use.\n"
		"                                   (only applicable for source data streams)\n"
		"   --xccdf-id <id>               - ID of component-ref with XCCDF in the data stream that should be evaluated.\n"
		"                                   (only applicable for source data streams)\n"
		"   --benchmark-id <id>           - ID of XCCDF Benchmark in some component in the data stream that should be evaluated.\n"
		"                                   (only applicable for source data streams)\n"
		"                                   (only applicable when datastream-id AND xccdf-id are not specified)\n"
		"   --remediate                   - Automatically execute XCCDF fix elements for failed rules.\n"
		"                                   Use of this option is always at your own risk.\n",
    .opt_parser = getopt_xccdf,
    .func = app_evaluate_xccdf
};

static struct oscap_module XCCDF_REMEDIATE = {
	.name =		"remediate",
	.parent =	&OSCAP_XCCDF_MODULE,
	.summary =	"Perform remediation driven by XCCDF TestResult file or ARF.",
	.usage =	"[options] INPUT_FILE [oval-definitions-files]",
	.help =		"INPUT_FILE - XCCDF TestResult file or ARF\n\n"
		"Options:\n"
		"   --result-id                   - TestResult ID to be processed. Default is the most recent one.\n"
		"   --skip-validation             - Skip validation.\n"
		"   --cpe <name>                  - Use given CPE dictionary or language (autodetected)\n"
		"                                   for applicability checks.\n"
		"   --fetch-remote-resources      - Download remote content referenced by XCCDF.\n"
		"   --local-files <dir>           - Use locally downloaded copies of remote resources stored in the given directory.\n"
		"   --results <file>              - Write XCCDF Results into file.\n"
		"   --results-arf <file>          - Write ARF (result data stream) into file.\n"
		"   --stig-viewer <file>          - Writes XCCDF results into FILE in a format readable by DISA STIG Viewer\n"
		"   --report <file>               - Write HTML report into file.\n"
		"   --oval-results                - Save OVAL results.\n"
		"   --export-variables            - Export OVAL external variables provided by XCCDF.\n"
		"   --check-engine-results        - Save results from check engines loaded from plugins as well.\n"
		"   --progress                    - Switch to sparse output suitable for progress reporting.\n"
		"                                   Format is \"$rule_id:$result\\n\".\n"
		"   --progress-full               - Switch to sparse but a bit more saturated output also suitable for progress reporting.\n"
		"                                   Format is \"$rule_id|$rule_title|$result\\n\".\n"
	,
	.opt_parser = getopt_xccdf,
	.func = app_xccdf_remediate
};

#define GEN_OPTS \
        "Generate options:\n" \
	"   --profile <profile-id>        - Apply profile with given ID to the Benchmark before further processing takes place.\n"

static struct oscap_module XCCDF_GENERATE = {
    .name = "generate",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Convert XCCDF Benchmark to other formats",
    .usage = "[options]",
    .usage_extra = "<subcommand> [sub-options] benchmark-file.xml",
    .help = GEN_OPTS,
    .opt_parser = getopt_generate,
    .submodules = XCCDF_GEN_SUBMODULES
};

static struct oscap_module XCCDF_GEN_REPORT = {
    .name = "report",
    .parent = &XCCDF_GENERATE,
    .summary = "Generate results report",
    .usage = "[options] xccdf-file.xml",
    .help = GEN_OPTS
		"\nReport Options:\n"
		"   --result-id <id>              - TestResult ID to be processed. Default is the most recent one.\n"
		"   --output <file>               - Write the document into file.\n",
    .opt_parser = getopt_xccdf,
    .user = "xccdf-report.xsl",
    .func = app_xccdf_xslt
};

static struct oscap_module XCCDF_GEN_GUIDE = {
    .name = "guide",
    .parent = &XCCDF_GENERATE,
    .summary = "Generate security guide",
    .usage = "[options] xccdf-file.xml",
    .help = GEN_OPTS
		"\nGuide Options:\n"
		"   --output <file>               - Write the document into file.\n"
		"   --hide-profile-info           - This option has no effect.\n"
		"   --benchmark-id <id>           - ID of XCCDF Benchmark in some component in the data stream that should be used.\n"
		"                                   (only applicable for source data streams)\n"
		"   --xccdf-id <id>               - ID of component-ref with XCCDF in the data stream that should be evaluated.\n"
		"                                   (only applicable for source data streams)\n"
		"   --tailoring-file <file>       - Use given XCCDF Tailoring file.\n"
		"                                   (only applicable for source data streams)\n"
		"   --tailoring-id <component-id> - Use given DS component as XCCDF Tailoring file.\n"
		"                                   (only applicable for source data streams)\n"
		"   --skip-signature-validation   - Skip data stream signature validation.\n"
		"                                   (only applicable for source data streams)\n"
		"   --enforce-signature           - Process only signed data streams.\n",
    .opt_parser = getopt_xccdf,
    .user = NULL,
    .func = app_generate_guide
};

static struct oscap_module XCCDF_GEN_FIX = {
    .name = "fix",
    .parent = &XCCDF_GENERATE,
    .summary = "Generate a fix script from an XCCDF file",
    .usage = "[options] xccdf-file.xml",
    .help = GEN_OPTS
        "\nFix Options:\n"
		"   --fix-type <type>             - Fix type. Should be one of: bash, ansible, puppet, anaconda, ignition, kubernetes,\n"
		"                                   blueprint, kickstart (default: bash).\n"
		"   --output <file>               - Write the script into file.\n"
		"   --raw                         - Don't write extra headers or boilerplate instructions, only compose the content snippets.\n"
		"   --result-id <id>              - Fixes will be generated for failed rule-results of the specified TestResult.\n"
		"   --benchmark-id <id>           - ID of XCCDF Benchmark in some component in the data stream that should be used.\n"
		"                                   (only applicable for source data streams)\n"
		"   --xccdf-id <id>               - ID of component-ref with XCCDF in the data stream that should be evaluated.\n"
		"                                   (only applicable for source data streams)\n"
		"   --tailoring-file <file>       - Use given XCCDF Tailoring file.\n"
		"                                   (only applicable for source data streams)\n"
		"   --tailoring-id <component-id> - Use given DS component as XCCDF Tailoring file.\n"
		"                                   (only applicable for source data streams)\n"
		"   --skip-signature-validation   - Skip data stream signature validation.\n"
		"                                   (only applicable for source data streams)\n"
		"   --enforce-signature           - Process only signed data streams.\n",
    .opt_parser = getopt_xccdf,
    .user = "legacy-fix.xsl",
    .func = app_generate_fix
};

static struct oscap_module XCCDF_GEN_CUSTOM = {
    .name = "custom",
    .parent = &XCCDF_GENERATE,
    .summary = "Generate a custom output (depending on given XSLT file) from an XCCDF file",
    .usage = "--stylesheet <file> [--output <file>] xccdf-file.xml",
    .help = GEN_OPTS
		"\nCustom Options:\n"
		"   --stylesheet <file>           - Specify an absolute path to a custom stylesheet to format the output.\n"
		"   --output <file>               - Write the document into file.\n",
    .opt_parser = getopt_xccdf,
    .user = NULL,
    .func = app_xccdf_xslt
};

static struct oscap_module* XCCDF_GEN_SUBMODULES[XCCDF_GEN_SUBMODULES_NUM] = {
    &XCCDF_GEN_REPORT,
    &XCCDF_GEN_GUIDE,
    &XCCDF_GEN_FIX,
    &XCCDF_GEN_CUSTOM,
    NULL
};

static struct oscap_module* XCCDF_SUBMODULES[XCCDF_SUBMODULES_NUM] = {
    &XCCDF_EVAL,
    &XCCDF_RESOLVE,
    &XCCDF_VALIDATE,
    &XCCDF_EXPORT_OVAL_VARIABLES,
    &XCCDF_GENERATE,
	&XCCDF_REMEDIATE,
    NULL
};

enum progress_output_opt {
	PROGRESS_OPT_NONE = 0,
	PROGRESS_OPT_SPARSE,
	PROGRESS_OPT_FULL,
};

/**
 * XCCDF Result Colors:
 * PASS:green(32), FAIL:red(31), ERROR:lred(1;31), UNKNOWN:grey(1;30), NOT_APPLICABLE:default bold(1), NOT_CHECKED:default bold(1),
 * NOT_SELECTED:default dim(2), INFORMATIONAL:blue(34), FIXED:yellow(1;33)
 */
#if defined(OS_WINDOWS)
int RESULT_COLORS[] = {0, 10, 12, 12, 8, 15, 15, 15, 9, 14};
#else
static const char * RESULT_COLORS[] = {"", "32", "31", "1;31", "1;30", "1", "1", "2", "34", "1;33" };
#endif

static char custom_stylesheet_path[PATH_MAX];

static int callback_scr_rule(struct xccdf_rule *rule, void *arg)
{
	const char * rule_id = xccdf_rule_get_id(rule);

	/* is rule selected? we print only selected rules */
	const bool selected = xccdf_policy_is_item_selected((struct xccdf_policy *) arg, rule_id);
	if (!selected)
		return 0;

	const char *title = xccdf_policy_get_readable_item_title((struct xccdf_policy *)arg, (struct xccdf_item *) rule, NULL);

	/* print */
	if (isatty(1)) {
#if defined(OS_WINDOWS)
		HANDLE console;
		console = GetStdHandle(STD_OUTPUT_HANDLE);
		printf("Title");
		SetConsoleTextAttribute(console, 15);
		printf("\t%s\n", title);
		SetConsoleTextAttribute(console, 7);
#else
		printf("Title\r\t\033[1m%s\033[0;0m\n", title);
#endif
	} else
		printf("Title\r\t%s\n", title);
	free((char *)title);
#if defined(OS_WINDOWS)
	printf("Rule\t%s\n", rule_id);
#else
	printf("Rule\r\t%s\n", rule_id);
#endif

	struct xccdf_ident_iterator *idents = xccdf_rule_get_idents(rule);
	while (xccdf_ident_iterator_has_more(idents)) {
		const struct xccdf_ident *ident = xccdf_ident_iterator_next(idents);
		const char *ident_id = xccdf_ident_get_id(ident);
#if defined(OS_WINDOWS)
		printf("Ident\t%s\n", ident_id);
#else
		printf("Ident\r\t%s\n", ident_id);
#endif
	}
	xccdf_ident_iterator_free(idents);

	fflush(stdout);

	return 0;
}

static int callback_scr_result(struct xccdf_rule_result *rule_result, void *arg)
{
	xccdf_test_result_type_t result = xccdf_rule_result_get_result(rule_result);

	/* is result from selected rule? we print only selected rules */
	if (result == XCCDF_RESULT_NOT_SELECTED)
		return 0;

	/* print result */
#if defined(OS_WINDOWS)
	printf("Result\t");
#else
	printf("Result\r\t");
#endif
	const char * result_str = xccdf_test_result_type_get_text(result);
	if (isatty(1)) {
#if defined(OS_WINDOWS)
		HANDLE console;
		console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(console, RESULT_COLORS[result]);
		printf("%s", result_str);
		SetConsoleTextAttribute(console, 7);
		printf("\n\n");
#else
		printf("\033[%sm%s\033[0m\n\n", RESULT_COLORS[result], result_str);
#endif
	} else
		printf("%s\n\n", result_str);

	return 0;
}

static int callback_scr_result_progress(struct xccdf_rule_result *rule_result, void *arg)
{
	xccdf_test_result_type_t result = xccdf_rule_result_get_result(rule_result);

	/* is result from selected rule? we print only selected rules */
	if (result == XCCDF_RESULT_NOT_SELECTED)
		return 0;

	/* print result */
	const char *rule_id = xccdf_rule_result_get_idref(rule_result);
	const char *result_str = xccdf_test_result_type_get_text(result);

	printf("%s:%s\n", rule_id, result_str);
	fflush(stdout);

	return 0;
}

static int callback_scr_rule_progress_full(struct xccdf_rule *rule, void *arg)
{
	const char *rule_id = xccdf_rule_get_id(rule);

	if (!xccdf_policy_is_item_selected((struct xccdf_policy *)arg, rule_id))
		return 0;

	char *title = xccdf_policy_get_readable_item_title((struct xccdf_policy *)arg, (struct xccdf_item *)rule, NULL);
	printf("%s|%s|", rule_id, title);
	free(title);
	fflush(stdout);

	return 0;
}

static int callback_scr_result_progress_full(struct xccdf_rule_result *rule_result, void *arg)
{
	xccdf_test_result_type_t result = xccdf_rule_result_get_result(rule_result);

	/* is result from selected rule? we print only selected rules */
	if (result == XCCDF_RESULT_NOT_SELECTED)
		return 0;

	/* print result */
	const char *result_str = xccdf_test_result_type_get_text(result);

	printf("%s\n", result_str);
	fflush(stdout);

	return 0;
}

static int callback_scr_multicheck(struct oval_definition *definition, void *arg)
{
	printf("OVAL Definition ID\t%s\n", oval_definition_get_id(definition));
	printf("OVAL Definition Title\t%s\n", oval_definition_get_title(definition));
	return 0;
}

/*
 * Send XCCDF Rule Results info message to syslog
 *
static int callback_syslog_result(struct xccdf_rule_result *rule_result, void *arg)
{
	xccdf_test_result_type_t result = xccdf_rule_result_get_result(rule_result);

	// do we log it?
	if ((result != XCCDF_RESULT_FAIL) && (result != XCCDF_RESULT_UNKNOWN))
		return 0;

	// yes we do
	const char * result_str = xccdf_test_result_type_get_text(result);
	const char * ident_id = NULL;
	int priority = LOG_NOTICE;

	// get ident
	struct xccdf_ident_iterator *idents = xccdf_rule_result_get_idents(rule_result);
	if (xccdf_ident_iterator_has_more(idents)) {
		const struct xccdf_ident *ident = xccdf_ident_iterator_next(idents);
		ident_id = xccdf_ident_get_id(ident);
	}
	xccdf_ident_iterator_free(idents);

	// emit the message
	syslog(priority, "Rule: %s, Ident: %s, Result: %s.", xccdf_rule_result_get_idref(rule_result), ident_id, result_str);

	return 0;
}
*/


static void _register_progress_callback(struct xccdf_session *session, int progress)
{
	struct xccdf_policy_model *policy_model = xccdf_session_get_policy_model(session);
	if (progress == PROGRESS_OPT_SPARSE) {
		xccdf_policy_model_register_output_callback(policy_model, callback_scr_result_progress, NULL);
	} else if (progress == PROGRESS_OPT_FULL) {
		xccdf_policy_model_register_start_callback(policy_model, callback_scr_rule_progress_full,
		                                           (void *) xccdf_session_get_xccdf_policy(session));
		xccdf_policy_model_register_output_callback(policy_model, callback_scr_result_progress_full, NULL);
	} else {
		xccdf_policy_model_register_start_callback(policy_model, callback_scr_rule,
		                                           (void *) xccdf_session_get_xccdf_policy(session));
		xccdf_policy_model_register_output_callback(policy_model, callback_scr_result, NULL);
		xccdf_policy_model_register_multicheck_callback(policy_model, callback_scr_multicheck, NULL);
	}
	/* xccdf_policy_model_register_output_callback(policy_model, callback_syslog_result, NULL); */
}

void report_missing_profile(const char *profile_suffix, const char *source_file)
{
	fprintf(stderr,
		"No profile matching suffix \"%s\" was found. Get available profiles using:\n"
		"$ oscap info \"%s\"\n", profile_suffix, source_file);
}

void report_multiple_profile_matches(const char *profile_suffix, const char *source_file)
{
	fprintf(stderr,
		"At least two profiles matched suffix \"%s\". Use a more specific suffix "
		"to get an exact match. Get list of profiles using:\n"
		"$ oscap info \"%s\"\n", profile_suffix, source_file);
}

int evaluate_suffix_match_result_with_custom_reports(int suffix_match_result, const char *profile_suffix, const char *source_file,
		void (* report_missing)(const char *, const char *), void (* report_multiple)(const char *, const char *))
{
	if (suffix_match_result == OSCAP_PROFILE_NO_MATCH) {
		if (report_missing)
			report_missing(profile_suffix, source_file);
		return OSCAP_ERROR;
	} else if (suffix_match_result == OSCAP_PROFILE_MULTIPLE_MATCHES) {
		if (report_multiple)
			report_multiple(profile_suffix, source_file);
		return OSCAP_ERROR;
	}
	return OSCAP_OK;
}

int evaluate_suffix_match_result(int suffix_match_result, const char *profile_suffix, const char *source_file)
{
	return evaluate_suffix_match_result_with_custom_reports(suffix_match_result, profile_suffix, source_file, &report_missing_profile, &report_multiple_profile_matches);
}

int xccdf_set_profile_or_report_bad_id(struct xccdf_session *session, const char *profile_id, const char *source_file)
{
	const int suffix_match_result = xccdf_session_set_profile_id_by_suffix(session, profile_id);
	int return_code = evaluate_suffix_match_result(suffix_match_result, profile_id, source_file);
	return return_code;
}

/**
 * XCCDF Processing fucntion
 * @param action OSCAP Action structure
 * @param sess OVAL Agent Session
 */
int app_evaluate_xccdf(const struct oscap_action *action)
{
	struct xccdf_session *session = NULL;

	int result = OSCAP_ERROR;
#if defined(HAVE_SYSLOG_H)
	int priority = LOG_NOTICE;

	/* syslog message */
	syslog(priority, "Evaluation started. Content: %s, Profile: %s.", action->f_xccdf, action->profile);
#endif
	session = xccdf_session_new(action->f_xccdf);
	if (session == NULL)
		goto cleanup;
	xccdf_session_set_validation(session, action->validate, getenv("OSCAP_FULL_VALIDATION") != NULL);
	xccdf_session_set_signature_validation(session, action->validate_signature);
	xccdf_session_set_signature_enforcement(session, action->enforce_signature);
	if (action->thin_results) {
		xccdf_session_set_thin_results(session, true);
		xccdf_session_set_without_sys_chars_export(session, true);
	}
	if (xccdf_session_is_sds(session)) {
		xccdf_session_set_datastream_id(session, action->f_datastream_id);
		xccdf_session_set_component_id(session, action->f_xccdf_id);
		xccdf_session_set_benchmark_id(session, action->f_benchmark_id);
	}
	xccdf_session_set_user_cpe(session, action->cpe);
	// The tailoring_file may be NULL but the tailoring file may have been
	// autonegotiated from the input file, we don't want to lose that.
	if (action->tailoring_file != NULL)
		xccdf_session_set_user_tailoring_file(session, action->tailoring_file);
	xccdf_session_set_user_tailoring_cid(session, action->tailoring_id);
	xccdf_session_configure_remote_resources(session, action->remote_resources, action->local_files, download_reporting_callback);
	xccdf_session_set_custom_oval_files(session, action->f_ovals);
	xccdf_session_set_product_cpe(session, OSCAP_PRODUCTNAME);
	struct oscap_string_iterator *it = oscap_stringlist_get_strings(action->rules);
	while (oscap_string_iterator_has_more(it)) {
		const char *rid = oscap_string_iterator_next(it);
		xccdf_session_add_rule(session, rid);
	}
	oscap_string_iterator_free(it);
	struct oscap_string_iterator *sit = oscap_stringlist_get_strings(action->skip_rules);
	while (oscap_string_iterator_has_more(sit)) {
		const char *rid = oscap_string_iterator_next(sit);
		xccdf_session_skip_rule(session, rid);
	}
	oscap_string_iterator_free(sit);
	if (action->reference) {
		if (strchr(action->reference, ':') == NULL) {
			fprintf(stderr, "The --reference argument needs to be in form NAME:IDENTIFIER, using a colon as a separator.\n");
			goto cleanup;
		}
		xccdf_session_set_reference_filter(session, action->reference);
	}

	if (xccdf_session_load(session) != 0)
		goto cleanup;


	/* Select profile */
	if (!xccdf_session_set_profile_id(session, action->profile)) {
		if (action->profile != NULL) {
			if (xccdf_set_profile_or_report_bad_id(session, action->profile, action->f_xccdf) == OSCAP_ERROR)
				goto cleanup;
		} else {
			fprintf(stderr, "No Policy was found for default profile.\n");
			goto cleanup;
		}
	}

	_register_progress_callback(session, action->progress);

	if (action->progress == PROGRESS_OPT_SPARSE) {
		// Don't pronounce phases in this mode
	} else if (action->progress == PROGRESS_OPT_FULL) {
		printf("---evaluation\n");
	} else {
		printf("--- Starting Evaluation ---\n\n");
	}

	/* Perform evaluation */
	if (xccdf_session_evaluate(session) != 0)
		goto cleanup;

	xccdf_session_set_without_sys_chars_export(session, action->without_sys_chars);
	xccdf_session_set_oval_results_export(session, action->oval_results);
	xccdf_session_set_oval_variables_export(session, action->export_variables);
	xccdf_session_set_arf_export(session, action->f_results_arf);

	if (xccdf_session_export_oval(session) != 0)
		goto cleanup;
	else if (action->validate && getenv("OSCAP_FULL_VALIDATION") != NULL &&
		(action->oval_results == true || action->f_results_arf))
		fprintf(stdout, "OVAL Results are exported correctly.\n");

	xccdf_session_set_check_engine_plugins_results_export(session, action->check_engine_results);
	if (xccdf_session_export_check_engine_plugins(session) != 0)
		goto cleanup;

	if (action->remediate) {
		if (action->progress == PROGRESS_OPT_SPARSE) {
			// Don't pronounce phases in this mode
		} else if (action->progress == PROGRESS_OPT_FULL) {
			printf("---remediation\n");
		} else {
			printf("\n--- Starting Remediation ---\n\n");
		}
		xccdf_session_remediate(session);
	}

	/* Get the result from TestResult model and decide if end with error or with correct return code */
	int evaluation_result = xccdf_session_contains_fail_result(session) ? OSCAP_FAIL : OSCAP_OK;

	/* syslog message */
#if defined(HAVE_SYSLOG_H)
	syslog(priority, "Evaluation finished. Return code: %d, Base score %f.", evaluation_result, xccdf_session_get_base_score(session));
#endif

	xccdf_session_set_xccdf_export(session, action->f_results);
	xccdf_session_set_xccdf_stig_viewer_export(session, action->f_results_stig);
	xccdf_session_set_report_export(session, action->f_report);
	if (xccdf_session_export_all(session) != 0)
		goto cleanup;

	if (action->validate && getenv("OSCAP_FULL_VALIDATION") != NULL &&
		(action->f_results || action->f_report || action->f_results_arf || action->f_results_stig))
		fprintf(stdout, "XCCDF Results are exported correctly.\n");

	if (action->f_results_arf && getenv("OSCAP_FULL_VALIDATION") != NULL)
		fprintf(stdout, "Result data stream exported correctly.\n");

	result = evaluation_result;

cleanup:
	xccdf_session_free(session);
	oscap_print_error();
	return result;
}

static xccdf_test_result_type_t resolve_variables_wrapper(struct xccdf_policy *policy, const char *rule_id,
	const char *id, const char *href, struct xccdf_value_binding_iterator *bnd_itr,
	struct xccdf_check_import_iterator *check_import_it, void *usr)
{
	if (0 != oval_agent_resolve_variables((struct oval_agent_session *) usr, bnd_itr))
		return XCCDF_RESULT_UNKNOWN;

	return XCCDF_RESULT_PASS;
}

static int app_xccdf_export_oval_variables(const struct oscap_action *action)
{
	struct xccdf_policy *policy = NULL;
	struct xccdf_result *xres;
	int result = OSCAP_ERROR;
	struct xccdf_session *session = NULL;

	session = xccdf_session_new(action->f_xccdf);
	if (session == NULL)
		goto cleanup;

	xccdf_session_set_validation(session, action->validate, getenv("OSCAP_FULL_VALIDATION") != NULL);

	if (xccdf_session_is_sds(session)) {
		xccdf_session_set_datastream_id(session, action->f_datastream_id);
		xccdf_session_set_component_id(session, action->f_xccdf_id);
		xccdf_session_set_benchmark_id(session, action->f_benchmark_id);
	}
	xccdf_session_set_user_cpe(session, action->cpe);
	xccdf_session_configure_remote_resources(session, action->remote_resources, action->local_files, download_reporting_callback);
	xccdf_session_set_custom_oval_files(session, action->f_ovals);
	xccdf_session_set_custom_oval_eval_fn(session, resolve_variables_wrapper);

	if (xccdf_session_load(session) != 0)
		goto cleanup;

	/* select a profile */
	policy = xccdf_policy_model_get_policy_by_id(xccdf_session_get_policy_model(session), action->profile);
	if (policy == NULL) {
		if (action->profile != NULL)
			report_missing_profile(action->profile, action->f_xccdf);
		else
			fprintf(stderr, "No Policy was found for default profile.\n");
		goto cleanup;
	}

	/* perform evaluation */
	xres = xccdf_policy_evaluate(policy);
	if (xres == NULL)
		goto cleanup;

	xccdf_session_set_oval_variables_export(session, true);
	if (xccdf_session_export_oval(session) == 0)
		result = OSCAP_OK;

 cleanup:
	oscap_print_error();
	if (session != NULL)
		xccdf_session_free(session);

	return result;
}

int app_xccdf_remediate(const struct oscap_action *action)
{
	struct xccdf_session *session = NULL;
	int result = OSCAP_ERROR;
	session = xccdf_session_new(action->f_xccdf);
	if (session == NULL)
		goto cleanup;
	xccdf_session_set_validation(session, action->validate, getenv("OSCAP_FULL_VALIDATION") != NULL);
	xccdf_session_set_user_cpe(session, action->cpe);
	xccdf_session_configure_remote_resources(session, action->remote_resources, action->local_files, download_reporting_callback);
	xccdf_session_set_custom_oval_files(session, action->f_ovals);

	if (xccdf_session_load(session) != 0)
		goto cleanup;

	if (xccdf_session_build_policy_from_testresult(session, action->id) != 0)
		goto cleanup;

	_register_progress_callback(session, action->progress);

	xccdf_session_remediate(session);

	xccdf_session_set_oval_results_export(session, action->oval_results);
	xccdf_session_set_oval_variables_export(session, action->export_variables);
	xccdf_session_set_arf_export(session, action->f_results_arf);
	xccdf_session_set_xccdf_export(session, action->f_results);
	xccdf_session_set_xccdf_stig_viewer_export(session, action->f_results_stig);
	xccdf_session_set_report_export(session, action->f_report);

	if (xccdf_session_export_oval(session) != 0)
		goto cleanup;

	xccdf_session_set_check_engine_plugins_results_export(session, action->check_engine_results);
	if (xccdf_session_export_check_engine_plugins(session) != 0)
		goto cleanup;

	/* Get the result from TestResult model and decide if end with error or with correct return code */
	int evaluation_result = xccdf_session_contains_fail_result(session) ? OSCAP_FAIL : OSCAP_OK;

	if (xccdf_session_export_all(session) != 0)
		goto cleanup;

	result = evaluation_result;

cleanup:
	xccdf_session_free(session);
	oscap_print_error();
	return result;
}

int app_xccdf_resolve(const struct oscap_action *action)
{
	int ret = OSCAP_ERROR;
	struct xccdf_benchmark *bench = NULL;

	if (!action->f_xccdf) {
		fprintf(stderr, "No input document specified!\n");
		return OSCAP_ERROR;
	}
	if (!action->f_results) {
		fprintf(stderr, "No output document filename specified!\n");
		return OSCAP_ERROR;
	}

	struct oscap_source *source = oscap_source_new_from_file(action->f_xccdf);
	/* validate input */
	if (action->validate) {
		if (oscap_source_validate(source, reporter, (void *) action) != 0) {
			oscap_source_free(source);
			goto cleanup;
		}
	}

	bench = xccdf_benchmark_import_source(source);
	oscap_source_free(source);
	if (!bench)
		goto cleanup;

	if (action->force)
		xccdf_benchmark_set_resolved(bench, false);

	if (xccdf_benchmark_get_resolved(bench))
		fprintf(stderr, "Benchmark is already resolved!\n");
	else {
		if (!xccdf_benchmark_resolve(bench))
			fprintf(stderr, "Benchmark resolving failure (probably a dependency loop)!\n");
		else
		{
			if (xccdf_benchmark_export(bench, action->f_results) == 0) {
				ret = OSCAP_OK;

				/* validate exported results */
				const char* full_validation = getenv("OSCAP_FULL_VALIDATION");
				if (action->validate && full_validation) {
					struct oscap_source *result_source = oscap_source_new_from_file(action->f_results);
					if (oscap_source_validate(result_source, reporter, (void *) action) != 0) {
						ret = OSCAP_ERROR;
					}
					else
						fprintf(stdout, "Resolved XCCDF has been exported correctly.\n");
					oscap_source_free(result_source);
				}
			}
		}
	}

cleanup:
	oscap_print_error();
	if (bench)
		xccdf_benchmark_free(bench);

	return ret;
}

static bool _some_result_exists(struct oscap_source *xccdf_source, const char *namespace)
{
	struct xccdf_benchmark *benchmark = NULL;
	struct xccdf_policy_model *policy_model = NULL;
	struct oscap_file_entry_list *files = NULL;
	struct oscap_file_entry_iterator *files_it = NULL;
	char *oval_result = NULL;
	bool result = false;

	benchmark = xccdf_benchmark_import_source(xccdf_source);
	if (benchmark == NULL)
		return false;

	policy_model = xccdf_policy_model_new(benchmark);
	files = xccdf_policy_model_get_systems_and_files(policy_model);
	files_it = oscap_file_entry_list_get_files(files);
	oval_result = malloc(PATH_MAX * sizeof(char));
	while (oscap_file_entry_iterator_has_more(files_it)) {
		struct oscap_file_entry *file_entry = (struct oscap_file_entry *) oscap_file_entry_iterator_next(files_it);;
		struct stat sb;
		if (strcmp(oscap_file_entry_get_system(file_entry), namespace))
			continue;
		snprintf(oval_result, PATH_MAX, "./%s.result.xml", oscap_file_entry_get_file(file_entry));
		if (stat(oval_result, &sb) == 0) {
			result = true;
			break;
		}
	}
	free(oval_result);
	oscap_file_entry_iterator_free(files_it);
	oscap_file_entry_list_free(files);
	xccdf_policy_model_free(policy_model);
	return result;
}

int app_generate_fix(const struct oscap_action *action)
{
	struct xccdf_session *session = NULL;
	struct ds_rds_session *arf_session = NULL;
	const char *remediation_system = NULL;

	if (action->fix_type != NULL) {
		if (strcmp(action->fix_type, "bash") == 0) {
			remediation_system = "urn:xccdf:fix:script:sh";
		} else if (strcmp(action->fix_type, "ansible") == 0) {
			remediation_system = "urn:xccdf:fix:script:ansible";
		} else if (strcmp(action->fix_type, "kickstart") == 0) {
			remediation_system = "urn:xccdf:fix:script:kickstart";
		} else if (strcmp(action->fix_type, "puppet") == 0) {
			remediation_system = "urn:xccdf:fix:script:puppet";
		} else if (strcmp(action->fix_type, "anaconda") == 0) {
			remediation_system = "urn:redhat:anaconda:pre";
		} else if (strcmp(action->fix_type, "ignition") == 0) {
			remediation_system = "urn:xccdf:fix:script:ignition";
		} else if (strcmp(action->fix_type, "kubernetes") == 0) {
			remediation_system = "urn:xccdf:fix:script:kubernetes";
		} else if (strcmp(action->fix_type, "blueprint") == 0) {
			remediation_system = "urn:redhat:osbuild:blueprint";
		} else {
			fprintf(stderr,
					"Unknown fix type '%s'.\n"
					"Please provide one of: bash, ansible, kickstart, puppet, anaconda, ignition, kubernetes, blueprint.\n",
					action->fix_type);
			return OSCAP_ERROR;
		}
	} else {
		remediation_system = "urn:xccdf:fix:script:sh";
	}

	if (action->id != NULL && action->fix_type != NULL && !strcmp(action->fix_type, "kickstart")) {
		fprintf(stderr, "It isn't possible to generate results-oriented Kickstarts.\n");
		return OSCAP_ERROR;
	}

	int ret = OSCAP_ERROR;
	struct oscap_source *source = oscap_source_new_from_file(action->f_xccdf);
	oscap_document_type_t document_type = oscap_source_get_scap_type(source);
	if (document_type == OSCAP_DOCUMENT_ARF) {
		arf_session = ds_rds_session_new_from_source(source);
		if (arf_session == NULL) {
			goto cleanup;
		}
		struct oscap_source *report_source = ds_rds_session_select_report(arf_session, NULL);
		if (report_source == NULL) {
			goto cleanup;
		}
		struct oscap_source *report_request_source = ds_rds_session_select_report_request(arf_session, NULL);
		if (report_request_source == NULL) {
			goto cleanup;
		}
		session = xccdf_session_new_from_source(oscap_source_clone(report_request_source));
		if (action->id != NULL) {
			if (xccdf_session_add_report_from_source(session, oscap_source_clone(report_source))) {
				goto cleanup;
			}
		}
		oscap_source_free(source);
	} else {
		session = xccdf_session_new_from_source(source);
	}
	if (session == NULL)
		goto cleanup;

	xccdf_session_set_validation(session, action->validate, getenv("OSCAP_FULL_VALIDATION") != NULL);
	xccdf_session_set_signature_validation(session, action->validate_signature);
	xccdf_session_set_signature_enforcement(session, action->enforce_signature);
	xccdf_session_set_user_cpe(session, action->cpe);
	xccdf_session_configure_remote_resources(session, action->remote_resources, action->local_files, download_reporting_callback);
	xccdf_session_set_custom_oval_files(session, action->f_ovals);
	xccdf_session_set_user_tailoring_file(session, action->tailoring_file);
	xccdf_session_set_user_tailoring_cid(session, action->tailoring_id);
	if (xccdf_session_is_sds(session)) {
		xccdf_session_set_component_id(session, action->f_xccdf_id);
		xccdf_session_set_benchmark_id(session, action->f_benchmark_id);
	}
	xccdf_session_set_loading_flags(session, XCCDF_SESSION_LOAD_XCCDF);
	if (xccdf_session_load(session) != 0)
		goto cleanup;

#ifdef OS_WINDOWS
	int output_fd = _fileno(stdout);
#else
	int output_fd = STDOUT_FILENO;
#endif
	if (action->f_results != NULL) {
		if ((output_fd = open(action->f_results, O_CREAT|O_TRUNC|O_NOFOLLOW|O_WRONLY, 0700)) < 0) {
			fprintf(stderr, "Could not open %s: %s", action->f_results, strerror(errno));
			goto cleanup;
		}
	}

	struct oscap_source *tailoring = xccdf_session_get_user_tailoring_file(session);
	if (action->id != NULL) {
		/* Result-oriented fixes */
		if (xccdf_session_build_policy_from_testresult(session, action->id) != 0)
			goto cleanup2;

		struct xccdf_policy *policy = xccdf_session_get_xccdf_policy(session);
		struct xccdf_result *result = xccdf_policy_get_result_by_id(policy, xccdf_session_get_result_id(session));
		if (xccdf_policy_generate_fix(policy, result, remediation_system, action->f_xccdf, tailoring, output_fd, action->raw) == 0)
			ret = OSCAP_OK;
	} else { // Fallback to profile if result id is missing
		/* Profile-oriented fixes */
		if (!xccdf_session_set_profile_id(session, action->profile)) {
			if (action->profile != NULL) {
				if (xccdf_set_profile_or_report_bad_id(session, action->profile, action->f_xccdf) == OSCAP_ERROR)
					goto cleanup2;
			} else {
				fprintf(stderr, "No Policy was found for default profile.\n");
				goto cleanup2;
			}
		}
		struct xccdf_policy *policy = xccdf_session_get_xccdf_policy(session);
		if (xccdf_policy_generate_fix(policy, NULL, remediation_system, action->f_xccdf, tailoring, output_fd, action->raw) == 0)
			ret = OSCAP_OK;
	}
cleanup2:
#ifdef OS_WINDOWS
	if (output_fd != _fileno(stdout))
#else
	if (output_fd != STDOUT_FILENO)
#endif
		close(output_fd);
cleanup:
	ds_rds_session_free(arf_session);
	xccdf_session_free(session);
	oscap_print_error();
	return ret;
}

int app_generate_guide(const struct oscap_action *action)
{
	int ret = OSCAP_ERROR;

	struct oscap_source *source = oscap_source_new_from_file(action->f_xccdf);
	struct xccdf_session *session = xccdf_session_new_from_source(source);
	if (session == NULL) {
		goto cleanup;
	}

	xccdf_session_set_validation(session, action->validate, getenv("OSCAP_FULL_VALIDATION") != NULL);
	xccdf_session_set_signature_validation(session, action->validate_signature);
	xccdf_session_set_signature_enforcement(session, action->enforce_signature);
	xccdf_session_configure_remote_resources(session, action->remote_resources, action->local_files, download_reporting_callback);
	xccdf_session_set_user_tailoring_file(session, action->tailoring_file);
	xccdf_session_set_user_tailoring_cid(session, action->tailoring_id);
	if (xccdf_session_is_sds(session)) {
		xccdf_session_set_component_id(session, action->f_xccdf_id);
		xccdf_session_set_benchmark_id(session, action->f_benchmark_id);
	}
	xccdf_session_set_loading_flags(session, XCCDF_SESSION_LOAD_XCCDF);

	if (xccdf_session_load(session) != 0) {
		goto cleanup;
	}

	if (!xccdf_session_set_profile_id(session, action->profile)) {
		if (action->profile != NULL) {
			if (xccdf_set_profile_or_report_bad_id(session, action->profile, action->f_xccdf) == OSCAP_ERROR)
				goto cleanup;
		} else {
			fprintf(stderr, "No Policy was found for default profile.\n");
			goto cleanup;
		}
	}

	if (xccdf_session_generate_guide(session, action->f_results) == 0) {
		ret = OSCAP_OK;
	}

cleanup:
	xccdf_session_free(session);
	oscap_print_error();
	return ret;
}

int app_xccdf_xslt(const struct oscap_action *action)
{
	const char *oval_template = NULL;
	const char *sce_template = NULL;

	if (action->module == &XCCDF_GEN_REPORT) {
		/* If generating the report and the option is missing -> use defaults */
		struct oscap_source *xccdf_source = oscap_source_new_from_file(action->f_xccdf);
		/* We want to define default template because we strive to serve user the
		 * best. However, we must not offer a template, if there is a risk it might
		 * be incorrect. Otherwise, libxml2 will throw a lot of misleading messages
		 * to stderr. */
		if (_some_result_exists(xccdf_source, "http://oval.mitre.org/XMLSchema/oval-definitions-5")) {
			oval_template = "%.result.xml";
		}
		if (_some_result_exists(xccdf_source, "http://open-scap.org/page/SCE")) {
			sce_template = "%.result.xml";
		}
		oscap_source_free(xccdf_source);
	}

	if (action->module == &XCCDF_GEN_CUSTOM) {
	        action->module->user = (void*)action->stylesheet;
	}

	const char *params[] = {
		"result-id",         action->id,
		"benchmark_id",      action->f_benchmark_id,
		"profile_id",        action->profile,
		"oval-template",     oval_template,
		"sce-template",      sce_template,
		"verbosity",         "",
		"hide-profile-info", action->hide_profile_info ? "yes" : NULL,
		NULL
	};

	int ret = app_xslt(action->f_xccdf, action->module->user, action->f_results, params);
	return ret;
}

bool getopt_generate(int argc, char **argv, struct oscap_action *action)
{
	static const struct option long_options[] = {
		{"profile", 1, 0, 3},
		{0, 0, 0, 0}
	};

	int c;
	while ((c = getopt_long(argc, argv, "+", long_options, NULL)) != -1) {
		switch (c) {
		case 3: action->profile = optarg; break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}
    return true;
}

enum oval_opt {
    XCCDF_OPT_RESULT_FILE = 1,
    XCCDF_OPT_RESULT_FILE_STIG,
    XCCDF_OPT_RESULT_FILE_ARF,
    XCCDF_OPT_DATASTREAM_ID,
    XCCDF_OPT_XCCDF_ID,
    XCCDF_OPT_BENCHMARK_ID,
    XCCDF_OPT_PROFILE,
    XCCDF_OPT_RULE,
    XCCDF_OPT_SKIP_RULE,
    XCCDF_OPT_REPORT_FILE,
    XCCDF_OPT_FORMAT,
    XCCDF_OPT_STYLESHEET_FILE,
    XCCDF_OPT_FILE_VERSION,
	XCCDF_OPT_TAILORING_FILE,
	XCCDF_OPT_TAILORING_ID,
    XCCDF_OPT_CPE,
    XCCDF_OPT_CPE_DICT,
    XCCDF_OPT_OUTPUT = 'o',
    XCCDF_OPT_RESULT_ID = 'i',
	XCCDF_OPT_FIX_TYPE,
	XCCDF_OPT_LOCAL_FILES,
	XCCDF_OPT_REFERENCE
};

bool getopt_xccdf(int argc, char **argv, struct oscap_action *action)
{
	assert(action != NULL);

	action->doctype = OSCAP_DOCUMENT_XCCDF;

	/* Command-options */
	const struct option long_options[] = {
	// options
		{"output",		required_argument, NULL, XCCDF_OPT_OUTPUT},
		{"results", 		required_argument, NULL, XCCDF_OPT_RESULT_FILE},
		{"results-arf",		required_argument, NULL, XCCDF_OPT_RESULT_FILE_ARF},
		{"stig-viewer", 	required_argument, NULL, XCCDF_OPT_RESULT_FILE_STIG},
		{"datastream-id",		required_argument, NULL, XCCDF_OPT_DATASTREAM_ID},
		{"xccdf-id",		required_argument, NULL, XCCDF_OPT_XCCDF_ID},
		{"benchmark-id",		required_argument, NULL, XCCDF_OPT_BENCHMARK_ID},
		{"profile", 		required_argument, NULL, XCCDF_OPT_PROFILE},
		{"rule", 		required_argument, NULL, XCCDF_OPT_RULE},
		{"skip-rule", 		required_argument, NULL, XCCDF_OPT_SKIP_RULE},
		{"result-id",		required_argument, NULL, XCCDF_OPT_RESULT_ID},
		{"report", 		required_argument, NULL, XCCDF_OPT_REPORT_FILE},
		{"stylesheet",	required_argument, NULL, XCCDF_OPT_STYLESHEET_FILE},
		{"tailoring-file", required_argument, NULL, XCCDF_OPT_TAILORING_FILE},
		{"tailoring-id", required_argument, NULL, XCCDF_OPT_TAILORING_ID},
		{"cpe",	required_argument, NULL, XCCDF_OPT_CPE},
		{"cpe-dict",	required_argument, NULL, XCCDF_OPT_CPE_DICT}, // DEPRECATED!
		{"fix-type", required_argument, NULL, XCCDF_OPT_FIX_TYPE},
		{"local-files", required_argument, NULL, XCCDF_OPT_LOCAL_FILES},
		{"reference", required_argument, NULL, XCCDF_OPT_REFERENCE},
	// flags
		{"force",		no_argument, &action->force, 1},
		{"oval-results",	no_argument, &action->oval_results, 1},
		{"check-engine-results", no_argument, &action->check_engine_results, 1},
		{"skip-validation",		no_argument, &action->validate, 0},
		{"skip-signature-validation", no_argument, &action->validate_signature, 0},
		{"enforce-signature", no_argument, &action->enforce_signature, 1},
		{"fetch-remote-resources", no_argument, &action->remote_resources, 1},
		{"progress", no_argument, &action->progress, PROGRESS_OPT_SPARSE},
		{"progress-full", no_argument, &action->progress, PROGRESS_OPT_FULL},
		{"remediate", no_argument, &action->remediate, 1},
		{"hide-profile-info",	no_argument, &action->hide_profile_info, 1},
		{"export-variables",	no_argument, &action->export_variables, 1},
		{"skip-schematron",     no_argument, &action->schematron, 0},
		{"without-syschar",     no_argument, &action->without_sys_chars, 1},
		{"thin-results",        no_argument, &action->thin_results, 1},
		{"raw",                 no_argument, &action->raw, 1},
	// end
		{0, 0, 0, 0}
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:i:", long_options, NULL)) != -1) {

		switch (c) {
		case XCCDF_OPT_OUTPUT:
		case XCCDF_OPT_RESULT_FILE:	action->f_results = optarg;	break;
		case XCCDF_OPT_RESULT_FILE_STIG: action->f_results_stig = optarg;	break;
		case XCCDF_OPT_RESULT_FILE_ARF:	action->f_results_arf = optarg;	break;
		case XCCDF_OPT_DATASTREAM_ID:	action->f_datastream_id = optarg;	break;
		case XCCDF_OPT_XCCDF_ID:	action->f_xccdf_id = optarg; break;
		case XCCDF_OPT_BENCHMARK_ID:	action->f_benchmark_id = optarg; break;
		case XCCDF_OPT_PROFILE:		action->profile = optarg;	break;
		case XCCDF_OPT_RULE:
			oscap_stringlist_add_string(action->rules, optarg);
			break;
		case XCCDF_OPT_SKIP_RULE:
			oscap_stringlist_add_string(action->skip_rules, optarg);
			break;
		case XCCDF_OPT_RESULT_ID:	action->id = optarg;		break;
		case XCCDF_OPT_REPORT_FILE:	action->f_report = optarg; 	break;
		/* we use realpath to get an absolute path to given XSLT to prevent openscap from looking
		   into /usr/share/openscap/xsl instead of CWD */
		case XCCDF_OPT_STYLESHEET_FILE: oscap_realpath(optarg, custom_stylesheet_path); action->stylesheet = custom_stylesheet_path; break;
		case XCCDF_OPT_TAILORING_FILE:	action->tailoring_file = optarg; break;
		case XCCDF_OPT_TAILORING_ID:	action->tailoring_id = optarg; break;
		case XCCDF_OPT_CPE:			action->cpe = optarg; break;
		case XCCDF_OPT_CPE_DICT:
			{
				fprintf(stdout, "Warning: --cpe-dict is a deprecated option. Please use --cpe instead!\n\n");
				action->cpe = optarg; break;
			}
		case XCCDF_OPT_FIX_TYPE:
			action->fix_type = optarg;
			break;
		case XCCDF_OPT_LOCAL_FILES:
			action->local_files = optarg;
			break;
		case XCCDF_OPT_REFERENCE:
			action->reference = optarg;
			break;
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	if (action->module == &XCCDF_EVAL) {
		/* We should have XCCDF file here */
		if (optind >= argc) {
			/* TODO */
			return oscap_module_usage(action->module, stderr, "XCCDF file need to be specified!");
		}

                action->f_xccdf = argv[optind];
                if (argc > (optind+1)) {
                    action->f_ovals = malloc((argc-(optind+1)+1) * sizeof(char *));
                    int i = 1;
                    while (argc > (optind+i)) {
                        action->f_ovals[i-1] = argv[optind + i];
                        i++;
                    }
                    action->f_ovals[i-1] = NULL;
                } else {
                    action->f_ovals = NULL;
                }
	} else if (action->module == &XCCDF_GEN_CUSTOM) {
		if (!action->stylesheet) {
			return oscap_module_usage(action->module, stderr, "XSLT Stylesheet needs to be specified!");
		}

		if (optind >= argc)
			return oscap_module_usage(action->module, stderr, "XCCDF file needs to be specified!");
		action->f_xccdf = argv[optind];
	} else {
		if (optind >= argc)
			return oscap_module_usage(action->module, stderr, "XCCDF file needs to be specified!");
		action->f_xccdf = argv[optind];
	}

	return true;
}

int app_xccdf_validate(const struct oscap_action *action) {
	int ret;
	int result = OSCAP_OK;

	struct oscap_source *source = oscap_source_new_from_file(action->f_xccdf);
	ret = oscap_source_validate(source, reporter, (void *) action);

	if (ret < 0) {
		result = OSCAP_ERROR;
	} else if (ret > 0) {
		result = OSCAP_FAIL;
	} else {
		if (action->schematron) {
			ret = oscap_source_validate_schematron(source);
			if (ret < 0) {
				result = OSCAP_ERROR;
			} else if (ret > 0) {
				result = OSCAP_FAIL;
			}
		}
	}

	oscap_source_free(source);
	oscap_print_error();

	return result;
}
