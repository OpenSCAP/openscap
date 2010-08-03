/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      Peter Vrabec   <pvrabec@redhat.com>
 */

#include <oval_probe.h>
#include <oval_agent_api.h>
#include <oval_results.h>
#include <oval_variables.h>

#include <xccdf.h>
#include <xccdf_policy.h>

#include <assert.h>

#include "oscap-tool.h"

extern int VERBOSE;

static int app_evaluate_xccdf(const struct oscap_action *action);
static int app_xccdf_resolve(const struct oscap_action *action);
static int app_xccdf_gen_report(const struct oscap_action *action);
static bool getopt_xccdf(int argc, char **argv, struct oscap_action *action);
static int xccdf_gen_report(const char *infile, const char *id, const char *outfile);

static struct oscap_module* XCCDF_SUBMODULES[];

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
        "   --force or -f\r\t\t\t\t - Force resolving XCCDF document even if it is aleready marked as resolved.",
    .opt_parser = getopt_xccdf,
    .func = app_xccdf_resolve
};

static struct oscap_module XCCDF_VALIDATE = {
    .name = "validate-xml",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Validate XCCDF XML content",
    .usage = "xccdf-file.xml",
    .opt_parser = getopt_xccdf,
    .func = app_validate_xml
};

static struct oscap_module XCCDF_GEN_REPORT = {
    .name = "generate-report",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Generate results report HTML file",
    .usage = "[options] -i result-id xccdf-file.xml",
    .help =
        "Options:\n"
        "   --result-id <id>\r\t\t\t\t - TestResult ID to be processed.\n"
        "   --output <file>\r\t\t\t\t - Write the HTML into file.",
    .opt_parser = getopt_xccdf,
    .func = app_xccdf_gen_report
};

static struct oscap_module XCCDF_EVAL = {
    .name = "eval",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Perform evaluation driven by XCCDF file and use OVAL as checking engine",
    .usage = "[options] oval-definitions.xml xccdf-benchmark.xml",
    .help =
        "Options:\n"
        "   --profile <name>\r\t\t\t\t - The name of Profile to be evaluated.\n"
        "   --result-file <file>\r\t\t\t\t - Write XCCDF Results into file.\n"
        "   --report-file <file>\r\t\t\t\t - Write HTML report into file.\n"
                                 "\t\t\t\t   (--result-file must be also specified for this to work)\n",
    .opt_parser = getopt_xccdf,
    .func = app_evaluate_xccdf
};

static struct oscap_module* XCCDF_SUBMODULES[] = {
    &XCCDF_EVAL,
    &XCCDF_RESOLVE,
    &XCCDF_VALIDATE,
    &XCCDF_GEN_REPORT,
    NULL
};

/**
 * Callback for XCCDF evaluation. Callback is called after each XCCDF Rule evaluation
 * @param msg OSCAP Reporter message
 * @param arg User defined data structure
 */
static int callback(const struct oscap_reporter_message *msg, void *arg)
{
	if (VERBOSE >= 0)
		printf("Rule \"%s\" result: %s\n",
		       oscap_reporter_message_get_user1str(msg),
		       xccdf_test_result_type_get_text((xccdf_test_result_type_t)
						       oscap_reporter_message_get_user2num(msg)));
	return 0;
}

/**
 * XCCDF Processing fucntion
 * @param action OSCAP Action structure 
 * @param sess OVAL Agent Session
 */
int app_evaluate_xccdf(const struct oscap_action *action)
{

	struct xccdf_policy_iterator *policy_it = NULL;
	struct xccdf_policy *policy = NULL;
	struct xccdf_benchmark *benchmark = NULL;
	struct xccdf_policy_model *policy_model = NULL;
	struct oval_definition_model *def_model = NULL;
	struct oval_agent_session *sess = NULL;

	def_model = oval_definition_model_import(action->f_oval);
	sess = oval_agent_new_session(def_model);

	/* Load XCCDF model and XCCDF Policy model */
	benchmark = xccdf_benchmark_import(action->f_xccdf);
	policy_model = xccdf_policy_model_new(benchmark);

	/* Get the first policy, just for prototype - if there is no Policy 
	 * report error and return -1 */
	if (action->profile != NULL) {
		policy = xccdf_policy_model_get_policy_by_id(policy_model, action->profile);
	} else {
		policy_it = xccdf_policy_model_get_policies(policy_model);
		if (xccdf_policy_iterator_has_more(policy_it)) {
			policy = xccdf_policy_iterator_next(policy_it);
		}
		xccdf_policy_iterator_free(policy_it);
	}

	if (policy == NULL) {
		fprintf(stderr, "No Policy to evaluate. \n");
		return -1;
	}

	/* Register callback */
	xccdf_policy_model_register_output_callback(policy_model, (oscap_reporter) callback, NULL);
	xccdf_policy_model_register_engine_oval(policy_model, sess);

	/* Perform evaluation */
	struct xccdf_result *ritem = xccdf_policy_evaluate(policy);

	/* Write results into XCCDF Test Result model */
	xccdf_result_set_benchmark_uri(ritem, action->url_xccdf);
	struct oscap_text *title = oscap_text_new();
	oscap_text_set_text(title, "OSCAP Scan Result");
	xccdf_result_add_title(ritem, title);
	if (policy != NULL) {
		const char *id = xccdf_policy_get_id(policy);
		if (id != NULL)
			xccdf_result_set_profile(ritem, id);
	}
	oval_agent_export_sysinfo_to_xccdf_result(sess, ritem);

	struct xccdf_model_iterator *model_it = xccdf_benchmark_get_models(benchmark);
	while (xccdf_model_iterator_has_more(model_it)) {
		struct xccdf_model *model = xccdf_model_iterator_next(model_it);
		struct xccdf_score *score = xccdf_policy_get_score(policy, ritem, xccdf_model_get_system(model));
		xccdf_result_add_score(ritem, score);
	}
	xccdf_model_iterator_free(model_it);

	/* Export results */
	if (action->f_results != NULL) {
        xccdf_benchmark_add_result(benchmark, xccdf_result_clone(ritem));
        xccdf_benchmark_export(benchmark, action->f_results);
        if (action->f_report != NULL)
            xccdf_gen_report(action->f_results, xccdf_result_get_id(ritem), action->f_report);
    }

	/* Get the result from TestResult model and decide if end with error or with correct return code */
	int retval = 0;
	struct xccdf_rule_result_iterator *res_it = xccdf_result_get_rule_results(ritem);
	while (xccdf_rule_result_iterator_has_more(res_it)) {
		struct xccdf_rule_result *res = xccdf_rule_result_iterator_next(res_it);
		xccdf_test_result_type_t result = xccdf_rule_result_get_result(res);
		if ((result == XCCDF_RESULT_FAIL) || (result == XCCDF_RESULT_UNKNOWN))
			retval = 2;
	}
	xccdf_rule_result_iterator_free(res_it);

	/* Clear & End */
	oval_agent_destroy_session(sess);
	oval_definition_model_free(def_model);
	xccdf_policy_model_free(policy_model);

	return retval;
}

int app_xccdf_resolve(const struct oscap_action *action)
{
	if (action->f_xccdf == NULL) {
		fprintf(stderr, "No input document specified!\n");
		return 2;
	}
	if (action->f_results == NULL) {
		fprintf(stderr, "No output document filename specified!\n");
		return 2;
	}

	struct xccdf_benchmark *bench = xccdf_benchmark_import(action->f_xccdf);
	if (bench == NULL) {
		fprintf(stderr, "Benchmark import failure!\n");
		return 2;
	}

	int ret = 1;

	if (action->force) xccdf_benchmark_set_resolved(bench, false);
	if (xccdf_benchmark_get_resolved(bench))
		fprintf(stderr, "Benchmark is already resolved!\n");
	else {
		if (xccdf_benchmark_resolve(bench)) {
			if (xccdf_benchmark_export(bench, action->f_results))
				ret = 0;
			else ret = 2;
		} else fprintf(stderr, "Benchmark resolving failure (probably a dependency loop)!\n");
	}

	xccdf_benchmark_free(bench);

	return ret;
}

static int xccdf_gen_report(const char *infile, const char *id, const char *outfile)
{
    int ret = 1;

    char result_id[strlen(id ? id : "") + 3];
    sprintf(result_id, "'%s'", id);
    const char *params[] = { "result-id", result_id, NULL };

    if (oscap_apply_xslt(infile, "xccdf-results-report.xsl", outfile, params)) ret = 0;
    else fprintf(stderr, "ERROR: %s\n", oscap_err_desc());

    return ret;
}

int app_xccdf_gen_report(const struct oscap_action *action)
{
    return xccdf_gen_report(action->f_xccdf, action->profile, action->f_results);
}

bool getopt_xccdf(int argc, char **argv, struct oscap_action *action)
{
    assert(action != NULL);

	action->doctype = OSCAP_DOCUMENT_XCCDF;

	/* Command-options */
	static const struct option long_options[] = {
		{"force", 0, 0, 'f'},
		{"output", 1, 0, 'o'},
		{"result-id", 1, 0, 'i'},
		{"result-file", 1, 0, 0},
		{"xccdf-profile", 1, 0, 1},
		{"file-version", 1, 0, 2},
		{"profile", 1, 0, 3},
		{"report-file", 1, 0, 4},
		{0, 0, 0, 0}
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:i:f", long_options, NULL)) != -1) {
		switch (c) {
		case 'o': case 0: action->f_results = optarg; break;
		case 3: case 1: case 'i': action->profile = optarg; break;
		case 2: action->file_version = optarg; break;
        case 4: action->f_report = optarg; break;
		case 'f': action->force = true; break;
		default: return false;
		}
	}

	if (action->module == &XCCDF_EVAL) {
		/* We should have XCCDF file here */
		if (optind + 1 >= argc) {
			/* TODO */
			return oscap_module_usage(action->module, stderr, "OVAL file and XCCDF file need to be specified!");
		}
        if (action->f_report && !action->f_results)
            return oscap_module_usage(action->module, stderr, "Please specify --result-file if you want to generate a HTML report.");
		action->url_oval = argv[optind];
		action->url_xccdf = argv[optind + 1];
	} else {
		if (optind >= argc)
			return oscap_module_usage(action->module, stderr, "XCCDF file needs to be specified!");
		action->url_xccdf = argv[optind];
	}

	return true;
}

