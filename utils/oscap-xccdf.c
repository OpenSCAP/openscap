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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <oval_probe.h>
#include <oval_agent_api.h>
#include <oval_agent_xccdf_api.h>
#include <oval_results.h>
#include <oval_variables.h>

#include <xccdf.h>
#include <xccdf_policy.h>

#ifdef ENABLE_SCE
#include <sce_engine_api.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>

#include "oscap-tool.h"
#include "oscap.h"

static int app_evaluate_xccdf(const struct oscap_action *action);
static int app_xccdf_resolve(const struct oscap_action *action);
static int app_xccdf_export_oval_variables(const struct oscap_action *action);
static bool getopt_xccdf(int argc, char **argv, struct oscap_action *action);
static bool getopt_generate(int argc, char **argv, struct oscap_action *action);
static int xccdf_gen_report(const char *infile, const char *id, const char *outfile, const char *show, const char *oval_template, const char* sce_template);
static int app_xccdf_xslt(const struct oscap_action *action);

static struct oscap_module* XCCDF_SUBMODULES[];
static struct oscap_module* XCCDF_GEN_SUBMODULES[];

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

static struct oscap_module XCCDF_EXPORT_OVAL_VARIABLES = {
    .name = "export-oval-variables",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Export XCCDF values as OVAL external-variables document(s)",
    .usage = "[options] <xccdf benchmark file> [oval definitions files]",
    .opt_parser = getopt_xccdf,
    .func = app_xccdf_export_oval_variables,
};

static struct oscap_module XCCDF_EVAL = {
    .name = "eval",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Perform evaluation driven by XCCDF file and use OVAL as checking engine",
    .usage = "[options] xccdf-benchmark.xml [oval-definitions-files]",
    .help =
        "Options:\n"
        "   --profile <name>\r\t\t\t\t - The name of Profile to be evaluated.\n"
        "   --oval-results\r\t\t\t\t - Save OVAL results as well.\n"
#ifdef ENABLE_SCE
        "   --sce-results\r\t\t\t\t - Save SCE results as well.\n"
#endif
        "   --export-variables\r\t\t\t\t - Export OVAL external variables provided by XCCDF.\n"
        "   --results <file>\r\t\t\t\t - Write XCCDF Results into file.\n"
        "   --report <file>\r\t\t\t\t - Write HTML report into file.\n"
        "   --skip-valid \r\t\t\t\t - Skip validation.\n"
                                 "\t\t\t\t   (--results must be also specified for this to work)\n",
    .opt_parser = getopt_xccdf,
    .func = app_evaluate_xccdf
};

#define GEN_OPTS \
        "Generate options:\n" \
        "   --profile <profile-id>\r\t\t\t\t - Tailor XCCDF file with respect to a profile.\n" \
        "   --format <fmt>\r\t\t\t\t - Select output format. Can be html or docbook.\n"

static struct oscap_module XCCDF_GENERATE = {
    .name = "generate",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Convert XCCDF Benchmark to other formats",
    .usage = "[gen-options]",
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
        "\nOptions:\n"
        "   --result-id <id>\r\t\t\t\t - TestResult ID to be processed. Default is the most recent one.\n"
        "   --show <result-type*>\r\t\t\t\t - Rule results to show. Defaults to everything but notselected and notapplicable.\n"
        "   --output <file>\r\t\t\t\t - Write the document into file.",
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
        "\nOptions:\n"
        "   --output <file>\r\t\t\t\t - Write the document into file.\n"
        "   --hide-profile-info\r\t\t\t\t - Do not output additional information about selected profile.\n",
    .opt_parser = getopt_xccdf,
    .user = "security-guide.xsl",
    .func = app_xccdf_xslt
};

static struct oscap_module XCCDF_GEN_FIX = {
    .name = "fix",
    .parent = &XCCDF_GENERATE,
    .summary = "Generate a fix script from an XCCDF file",
    .usage = "[options] xccdf-file.xml",
    .help = GEN_OPTS
        "\nOptions:\n"
        "   --output <file>\r\t\t\t\t - Write the script into file.\n"
        "   --result-id <id>\r\t\t\t\t - Fixes will be generated for failed rule-results of the specified TestResult.\n"
        "   --template <id|filename>\r\t\t\t\t - Fix template. (default: bash)\n",
    .opt_parser = getopt_xccdf,
    .user = "fix.xsl",
    .func = app_xccdf_xslt
};

static struct oscap_module XCCDF_GEN_CUSTOM = {
    .name = "custom",
    .parent = &XCCDF_GENERATE,
    .summary = "Generate a custom output (depending on given XSLT file) from an XCCDF file",
    .usage = "[options] xccdf-file.xml",
    .help = GEN_OPTS
        "\nOptions:\n"
        "   --stylesheet <file>\r\t\t\t\t - Specify an absolute path to a custom stylesheet to format the output.\n"
        "   --output <file>\r\t\t\t\t - Write the document into file.\n",
    .opt_parser = getopt_xccdf,
    .user = NULL,
    .func = app_xccdf_xslt
};

static struct oscap_module* XCCDF_GEN_SUBMODULES[] = {
    &XCCDF_GEN_REPORT,
    &XCCDF_GEN_GUIDE,
    &XCCDF_GEN_FIX,
    &XCCDF_GEN_CUSTOM,
    NULL
};

static struct oscap_module* XCCDF_SUBMODULES[] = {
    &XCCDF_EVAL,
    &XCCDF_RESOLVE,
    &XCCDF_VALIDATE,
    &XCCDF_EXPORT_OVAL_VARIABLES,
    &XCCDF_GENERATE,
    NULL
};

/**
 * XCCDF Result Colors:
 * PASS:green(32), FAIL:red(31), ERROR:lred(1;31), UNKNOWN:grey(1;30), NOT_APPLICABLE:white(1;37), NOT_CHECKED:white(1;37),
 * NOT_SELECTED:white(1;37), INFORMATIONAL:blue(34), FIXED:yellow(1;33)
 */
static const char * RESULT_COLORS[] = {"", "32", "31", "1;31", "1;30", "1;37", "1;37", "1;37", "34", "1;33" };

static char custom_stylesheet_path[PATH_MAX];

/**
 * Callback for XCCDF evaluation. Callback is called before each XCCDF Rule evaluation
 * @param msg OSCAP Reporter message
 * @param arg User defined data structure
 */
static int scallback(const struct oscap_reporter_message *msg, void *arg)
{
	if (((const struct oscap_action*) arg)->verbosity >= 0) {
            xccdf_test_result_type_t result = oscap_reporter_message_get_user2num(msg);
            if (result == XCCDF_RESULT_NOT_SELECTED) return 0;

            printf("\n");
            if (isatty(1)) 
                printf("Rule ID:\r\t\t\033[1m%s\033[0;0m\n", oscap_reporter_message_get_user1str(msg));
            else printf("Rule ID:\r\t\t%s\n", oscap_reporter_message_get_user1str(msg));
            printf("Title:\r\t\t%s\n", oscap_reporter_message_get_user3str(msg));
            /*printf("Description:\r\t\t%s\n", oscap_reporter_message_get_string(msg));*/
            printf("Result:\r\t\t");
            fflush(stdout);
        }

	return 0;
}
/**
 * Callback for XCCDF evaluation. Callback is called after each XCCDF Rule evaluation
 * @param msg OSCAP Reporter message
 * @param arg User defined data structure
 */
static int callback(const struct oscap_reporter_message *msg, void *arg)
{
	if (((const struct oscap_action*) arg)->verbosity >= 0) {
            xccdf_test_result_type_t result = oscap_reporter_message_get_user2num(msg);
            if (result == XCCDF_RESULT_NOT_SELECTED) return 0;

            if (isatty(1)) 
                printf("\033[%sm%s\033[0m\n", RESULT_COLORS[result], xccdf_test_result_type_get_text((xccdf_test_result_type_t) result));
            else printf("%s\n", xccdf_test_result_type_get_text((xccdf_test_result_type_t) result));
        }

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
	char * xccdf_pathcopy = NULL;
        void **def_models = NULL;
        void **sessions = NULL;
	char ** oval_files = NULL;
	int idx = 0;

#ifdef ENABLE_SCE
	struct sce_parameters* sce_parameters = 0;
#endif

	int retval = OSCAP_ERROR;

	/* Validate documents */
	if( action->validate ) {
		if (!oscap_validate_document(action->f_xccdf, OSCAP_DOCUMENT_XCCDF, xccdf_version_info_get_version(xccdf_detect_version(action->f_xccdf)), (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
			fprintf(stdout, "Invalid XCCDF content in %s\n", action->f_xccdf);
			goto cleanup;
        	}
	}

	/* Load XCCDF model and XCCDF Policy model */
	benchmark = xccdf_benchmark_import(action->f_xccdf);
	if (benchmark == NULL) {
		fprintf(stderr, "Failed to import the XCCDF content from (%s).\n", action->f_xccdf);
		goto cleanup;
        }

	/* Create policy model */
	policy_model = xccdf_policy_model_new(benchmark);

	/* Select profile */
	if (action->profile != NULL) {
		policy = xccdf_policy_model_get_policy_by_id(policy_model, action->profile);
		if(policy == NULL) {
                        fprintf(stderr, "Profile \"%s\" was not found.\n", action->profile);
                        goto cleanup;
                }
	} else { /* Take first policy */
		policy_it = xccdf_policy_model_get_policies(policy_model);
		if (xccdf_policy_iterator_has_more(policy_it)) {
			policy = xccdf_policy_iterator_next(policy_it);
		}
		xccdf_policy_iterator_free(policy_it);

		if (policy == NULL) {
			fprintf(stderr, "No Policy to evaluate. \n");
			goto cleanup;
		}
	}

	/* Register callback */
	xccdf_policy_model_register_start_callback(policy_model, scallback, (void*) action);
	xccdf_policy_model_register_output_callback(policy_model, callback, (void*) action);

	/* Use OVAL files from policy model */
	if (action->f_ovals == NULL) {
		struct stat sb;
		struct oscap_file_entry * file_entry;
		char * tmp_path;

		idx = 0;
		oval_files = malloc(sizeof(char *));
		oval_files[idx] = NULL;

		char * pathcopy =  strdup(action->f_xccdf);
		char * path = dirname(pathcopy);

		struct oscap_file_entry_list * files = xccdf_policy_model_get_systems_and_files(policy_model);
		struct oscap_file_entry_iterator *files_it = oscap_file_entry_list_get_files(files);
		while (oscap_file_entry_iterator_has_more(files_it)) {
			file_entry = (struct oscap_file_entry *)oscap_file_entry_iterator_next(files_it);

			// we only care about OVAL referenced files
			if (strcmp(oscap_file_entry_get_system(file_entry), "http://oval.mitre.org/XMLSchema/oval-definitions-5"))
				continue;

			tmp_path = malloc(PATH_MAX * sizeof(char));
			sprintf(tmp_path, "%s/%s", path, oscap_file_entry_get_file(file_entry));

			if (stat(tmp_path, &sb)) {
				fprintf(stderr, "WARNING: Skipping %s file which is referenced from XCCDF content\n", tmp_path);
				free(tmp_path);
			}
			else {
				oval_files[idx] = tmp_path;
				idx++;
				oval_files = realloc(oval_files, (idx + 1) * sizeof(char *));
				oval_files[idx] = NULL;
			}
		}
		oscap_file_entry_iterator_free(files_it);
		oscap_file_entry_list_free(files);
		free(pathcopy);
	} else
		oval_files = action->f_ovals;


	/* Validate OVAL files */
	if (action->validate) {
        	for (idx=0; oval_files[idx]; idx++) {
			xmlChar *doc_version;

			doc_version = oval_determine_document_schema_version((const char *) oval_files[idx],
				OSCAP_DOCUMENT_OVAL_DEFINITIONS);
			if (!oscap_validate_document(oval_files[idx],
				OSCAP_DOCUMENT_OVAL_DEFINITIONS, (const char *) doc_version,
				(action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
				fprintf(stdout, "Invalid OVAL Definition content in %s\n", oval_files[idx]);
				xmlFree(doc_version);
				goto cleanup;
			}
			xmlFree(doc_version);
		}
	}

	/* Register checking engines */
	for (idx=0; oval_files[idx]; idx++) {
		/* file -> def_model */
		struct oval_definition_model *tmp_def_model = oval_definition_model_import(oval_files[idx]);
		if (tmp_def_model == NULL) {
			fprintf(stderr, "Failed to create OVAL definition model from: %s.\n", oval_files[idx]);
			goto cleanup;
		}

		/* def_model -> session */
                struct oval_agent_session *tmp_sess = oval_agent_new_session(tmp_def_model, basename(oval_files[idx]));
		if (tmp_sess == NULL) {
			fprintf(stderr, "Failed to create new OVAL agent session for: %s.\n", oval_files[idx]);
			goto cleanup;
		}

		/* store our name in the generated documents */
		oval_agent_set_product_name(tmp_sess, OSCAP_PRODUCTNAME);

		/* remember def_models */
		def_models = realloc(def_models, (idx + 2) * sizeof(struct oval_definition_model *));
		def_models[idx] = tmp_def_model;
		def_models[idx+1] = NULL;

		/* remember sessions */
		sessions = realloc(sessions, (idx + 2) * sizeof(struct oval_agent_session *));
		sessions[idx] = tmp_sess;
		sessions[idx+1] = NULL;

		/* register session */
	        xccdf_policy_model_register_engine_oval(policy_model, tmp_sess);
	}

	// register sce system
	xccdf_pathcopy =  strdup(action->f_xccdf);

#ifdef ENABLE_SCE
	sce_parameters = sce_parameters_new();
	sce_parameters_set_xccdf_directory(sce_parameters, dirname(xccdf_pathcopy));
	sce_parameters_allocate_session(sce_parameters);

	xccdf_policy_model_register_engine_sce(policy_model, sce_parameters);
#endif

	/* Perform evaluation */
	struct xccdf_result *ritem = xccdf_policy_evaluate(policy);
        if (ritem == NULL) {
		goto cleanup;
	}

	/* Write results into XCCDF Test Result model */
	xccdf_result_set_benchmark_uri(ritem, action->f_xccdf);
	struct oscap_text *title = oscap_text_new();
	oscap_text_set_text(title, "OSCAP Scan Result");
	xccdf_result_add_title(ritem, title);
	if (policy != NULL) {
		const char *id = xccdf_policy_get_id(policy);
		if (id != NULL)
			xccdf_result_set_profile(ritem, id);
	}

	xccdf_result_fill_sysinfo(ritem);

	struct xccdf_model_iterator *model_it = xccdf_benchmark_get_models(benchmark);
	while (xccdf_model_iterator_has_more(model_it)) {
		struct xccdf_model *model = xccdf_model_iterator_next(model_it);
		struct xccdf_score *score = xccdf_policy_get_score(policy, ritem, xccdf_model_get_system(model));
		xccdf_result_add_score(ritem, score);
	}
	xccdf_model_iterator_free(model_it);

	const char* full_validation = getenv("OSCAP_FULL_VALIDATION");

	/* Export OVAL results */
	if (action->oval_results == true && sessions) {
		for (int i=0; sessions[i]; i++) {
			/* get result model and session name*/
			struct oval_results_model *res_model = oval_agent_get_results_model(sessions[i]);
			char * name =  malloc(PATH_MAX * sizeof(char));
			sprintf(name, "%s.result.xml", oval_agent_get_filename(sessions[i]));

			/* export result model to XML */
			oval_results_model_export(res_model, NULL, name);

			/* validate OVAL Results */
			if (action->validate && full_validation) {
				xmlChar *doc_version;

				doc_version = oval_determine_document_schema_version((const char *) name, OSCAP_DOCUMENT_OVAL_RESULTS);
				if (!oscap_validate_document(name, OSCAP_DOCUMENT_OVAL_RESULTS, (const char *) doc_version,
					(action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
					fprintf(stdout, "OVAL Results are NOT exported correctly.\n");
					free(name);
					xmlFree(doc_version);
					goto cleanup;
				}
				xmlFree(doc_version);
				fprintf(stdout, "OVAL Results are exported correctly.\n");
			}

			free(name);
		}
	}

#ifdef ENABLE_SCE
	/* Export SCE results */
	if (action->sce_results == true) {
		struct sce_check_result_iterator * it = sce_session_get_check_results(sce_parameters_get_session(sce_parameters));

		while(sce_check_result_iterator_has_more(it))
		{
			struct sce_check_result * result = sce_check_result_iterator_next(it);
			char target[2 + strlen(sce_check_result_get_basename(result)) + 11 + 1];
			snprintf(target, sizeof(target), "./%s.result.xml", sce_check_result_get_basename(result));
			sce_check_result_export(result, target);

			if (action->validate && full_validation) {
				if (!oscap_validate_document(target, OSCAP_DOCUMENT_SCE_RESULT, NULL,
					(action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout))
				{
						fprintf(stdout, "SCE Result file has NOT been exported correctly.\n");
						sce_check_result_iterator_free(it);
						goto cleanup;
				}
			}
		}

		sce_check_result_iterator_free(it);
	}
#endif

	/* Export results */
	if (action->f_results != NULL) {
		xccdf_benchmark_add_result(benchmark, xccdf_result_clone(ritem));
		xccdf_benchmark_export(benchmark, action->f_results);

		/* validate XCCDF Results */
		if (action->validate && full_validation) {
			if (!oscap_validate_document(action->f_results, OSCAP_DOCUMENT_XCCDF, xccdf_version_info_get_version(xccdf_detect_version(action->f_results)),
			    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
				fprintf(stdout, "XCCDF Results are NOT exported correctly.\n");
				goto cleanup;
			}
			fprintf(stdout, "XCCDF Results are exported correctly.\n");
		}

		/* generate report */
		if (action->f_report != NULL)
			xccdf_gen_report(action->f_results,
			                 xccdf_result_get_id(ritem),
			                 action->f_report,
			                 "",
			                 (action->oval_results ? "%.result.xml" : ""),
#ifdef ENABLE_SCE
			                 (action->sce_results  ? "%.result.xml" : "")
#else
			                 ""
#endif
			);
	}

	/* Export variables */
	if (action->export_variables && sessions) {
		int i;

		for (i = 0; sessions[i]; ++i) {
			int j;
			char *sname;
			struct oval_results_model *resmod;
			struct oval_definition_model *defmod;
			struct oval_variable_model_iterator *varmod_itr;

			sname = (char *) oval_agent_get_filename(sessions[i]);
			resmod = oval_agent_get_results_model(sessions[i]);
			defmod = oval_results_model_get_definition_model(resmod);

			j = 0;
			varmod_itr = oval_definition_model_get_variable_models(defmod);
			while (oval_variable_model_iterator_has_more(varmod_itr)) {
				char fname[strlen(sname) + 32];
				struct oval_variable_model *varmod;

				varmod = oval_variable_model_iterator_next(varmod_itr);
				snprintf(fname, sizeof(fname), "%s-%d.variables-%d.xml", sname, i, j++);
				oval_variable_model_export(varmod, fname);
			}
			oval_variable_model_iterator_free(varmod_itr);
		}
	}

	/* Get the result from TestResult model and decide if end with error or with correct return code */
	retval = OSCAP_OK;
	struct xccdf_rule_result_iterator *res_it = xccdf_result_get_rule_results(ritem);
	while (xccdf_rule_result_iterator_has_more(res_it)) {
		struct xccdf_rule_result *res = xccdf_rule_result_iterator_next(res_it);
		xccdf_test_result_type_t result = xccdf_rule_result_get_result(res);
		if ((result == XCCDF_RESULT_FAIL) || (result == XCCDF_RESULT_UNKNOWN))
			retval = OSCAP_FAIL;
	}
	xccdf_rule_result_iterator_free(res_it);


cleanup:
	if (oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

#ifdef ENABLE_SCE
	sce_parameters_free(sce_parameters);
#endif
	free(xccdf_pathcopy);

	/* Definition Models */
	if (def_models) {
		for (int i=0; def_models[i]; i++)
			oval_definition_model_free(def_models[i]);
		free(def_models);
	}

	/* Sessions */
	if (sessions) {
		for (int i=0; sessions[i]; i++)
			oval_agent_destroy_session(sessions[i]);
		free(sessions);
	}

	/* OVAL files imported from XCCDF */
	if (oval_files && (oval_files != action->f_ovals)) {
		for(int i=0; oval_files[i]; i++) {
			free(oval_files[i]);
		}
		free(oval_files);
	}

	if (policy_model)
		xccdf_policy_model_free(policy_model);

	return retval;
}

static xccdf_test_result_type_t resolve_variables_wrapper(struct xccdf_policy *policy, const char *rule_id,
	const char *id, const char *href, struct xccdf_value_binding_iterator *bnd_itr, void *usr)
{
	if (0 != oval_agent_resolve_variables((struct oval_agent_session *) usr, bnd_itr))
		return XCCDF_RESULT_UNKNOWN;

	return XCCDF_RESULT_PASS;
}

// todo: consolidate with app_evaluate_xccdf()
static int app_xccdf_export_oval_variables(const struct oscap_action *action)
{
	struct xccdf_benchmark *benchmark;
	struct xccdf_policy_model *policy_model = NULL;
	struct xccdf_policy *policy = NULL;
	struct xccdf_policy_iterator *policy_itr;
	struct oval_definition_model **def_mod_lst = NULL;
	struct oval_agent_session **ag_ses_lst = NULL;
	struct xccdf_result *xres;
	char **oval_file_lst = NULL;
	int of_cnt = 0, i, ret = OSCAP_ERROR;

	/* validate the XCCDF document */
	if (action->validate) {
		if (!oscap_validate_document(action->f_xccdf, OSCAP_DOCUMENT_XCCDF,
			xccdf_version_info_get_version(xccdf_detect_version(action->f_xccdf)), (action->verbosity >= 0) ? oscap_reporter_fd : NULL, stderr)) {
			fprintf(stderr, "Invalid XCCDF content in '%s'.\n", action->f_xccdf);
			goto cleanup;
		}
	}

	/* import the XCCDF document */
	benchmark = xccdf_benchmark_import(action->f_xccdf);
	if (benchmark == NULL) {
		fprintf(stderr, "Failed to import the XCCDF document from '%s'.\n", action->f_xccdf);
		goto cleanup;
	}

	/* create the policy model */
	policy_model = xccdf_policy_model_new(benchmark);

	/* select a profile */
	if (action->profile != NULL) {
		policy = xccdf_policy_model_get_policy_by_id(policy_model, action->profile);
		if (policy == NULL) {
			fprintf(stderr, "Unable to find profile '%s'.\n", action->profile);
			goto cleanup;
		}
	} else {
		/* use the first one if none specified */
		policy_itr = xccdf_policy_model_get_policies(policy_model);
		if (xccdf_policy_iterator_has_more(policy_itr))
			policy = xccdf_policy_iterator_next(policy_itr);
		xccdf_policy_iterator_free(policy_itr);
		if (policy == NULL) {
			fprintf(stderr, "No profile to evaluate.\n");
			goto cleanup;
		}
	}

	if (action->f_ovals != NULL) {
		oval_file_lst = action->f_ovals;
		for (of_cnt = 0; oval_file_lst[of_cnt]; of_cnt++);
	} else {
		char *xccdf_path_cpy, *dir_path;
		struct oscap_file_entry_list *files;
		struct oscap_file_entry_iterator *files_itr;

		oval_file_lst = malloc(sizeof(char *));
		oval_file_lst[0] = NULL;
		of_cnt = 0;

		xccdf_path_cpy = strdup(action->f_xccdf);
		dir_path = dirname(xccdf_path_cpy);

		files = xccdf_policy_model_get_systems_and_files(policy_model);
		files_itr = oscap_file_entry_list_get_files(files);
		while (oscap_file_entry_iterator_has_more(files_itr)) {
			struct oscap_file_entry *entry;
			char oval_path[PATH_MAX + 1];
			struct stat sb;

			entry = (struct oscap_file_entry *) oscap_file_entry_iterator_next(files_itr);

			// we only care about OVAL referenced files
			if (strcmp(oscap_file_entry_get_system(entry), "http://oval.mitre.org/XMLSchema/oval-definitions-5"))
				continue;

			snprintf(oval_path, sizeof(oval_path), "%s/%s", dir_path, oscap_file_entry_get_file(entry));
			if (stat(oval_path, &sb)) {
				fprintf(stderr, "warning: can't find file '%s' (referenced from XCCDF).\n", oval_path);
			} else {
				oval_file_lst[of_cnt++] = strdup(oval_path);
				oval_file_lst = realloc(oval_file_lst, (of_cnt + 1) * sizeof(char *));
				oval_file_lst[of_cnt] = NULL;
			}
		}
		oscap_file_entry_iterator_free(files_itr);
		oscap_file_entry_list_free(files);
		free(xccdf_path_cpy);
	}

	if (!oval_file_lst[0]) {
		fprintf(stderr, "No OVAL definition files present, aborting.\n");
		goto cleanup;
	}

	def_mod_lst = calloc(of_cnt, sizeof(struct oval_definition_model *));
	ag_ses_lst = calloc(of_cnt, sizeof(struct oval_agent_session *));

	for (i = 0; i < of_cnt; i++) {
		def_mod_lst[i] = oval_definition_model_import(oval_file_lst[i]);
		if (def_mod_lst[i] == NULL) {
			fprintf(stderr, "Failed to import definitions model from '%s'.\n", oval_file_lst[i]);
			goto cleanup;
		}

		ag_ses_lst[i] = oval_agent_new_session(def_mod_lst[i], basename(oval_file_lst[i]));
		if (ag_ses_lst[i] == NULL) {
			fprintf(stderr, "Failed to create new agent session for '%s'.\n", oval_file_lst[i]);
			goto cleanup;
		}

		xccdf_policy_model_register_engine_callback(policy_model,
			"http://oval.mitre.org/XMLSchema/oval-definitions-5",
			resolve_variables_wrapper, ag_ses_lst[i]);
	}

	/* perform evaluation */
	xres = xccdf_policy_evaluate(policy);
	if (xres == NULL)
		goto cleanup;

	for (i = 0; i < of_cnt; i++) {
		int j;
		char *ses_name;
		struct oval_variable_model_iterator *var_mod_itr;

		j = 0;
		ses_name = (char *) oval_agent_get_filename(ag_ses_lst[i]);

		var_mod_itr = oval_definition_model_get_variable_models(def_mod_lst[i]);
		while (oval_variable_model_iterator_has_more(var_mod_itr)) {
			struct oval_variable_model *var_mod;
			char fname[strlen(ses_name) + 32];

			var_mod = oval_variable_model_iterator_next(var_mod_itr);
			snprintf(fname, sizeof(fname), "%s-%d.variables-%d.xml", ses_name, i, j++);
			oval_variable_model_export(var_mod, fname);
		}
		oval_variable_model_iterator_free(var_mod_itr);
	}

	ret = OSCAP_OK;

 cleanup:
	if (oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	if (def_mod_lst != NULL) {
		for (i = 0; i < of_cnt; i++) {
			oval_agent_destroy_session(ag_ses_lst[i]);
			oval_definition_model_free(def_mod_lst[i]);
		}
		free(ag_ses_lst);
		free(def_mod_lst);
	}

	if (oval_file_lst && oval_file_lst != action->f_ovals) {
		for (i = 0; i < of_cnt; i++)
			free(oval_file_lst[i]);
		free(oval_file_lst);
	}

	if (policy_model)
		xccdf_policy_model_free(policy_model);

	return ret;
}

int app_xccdf_resolve(const struct oscap_action *action)
{
	if (action->f_xccdf == NULL) {
		fprintf(stderr, "No input document specified!\n");
		return OSCAP_ERROR;
	}
	if (action->f_results == NULL) {
		fprintf(stderr, "No output document filename specified!\n");
		return OSCAP_ERROR;
	}

	struct xccdf_benchmark *bench = xccdf_benchmark_import(action->f_xccdf);
	if (bench == NULL) {
		if (oscap_err())
			fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
	        return OSCAP_ERROR;
	}

	int ret = OSCAP_ERROR;

	if (action->force) xccdf_benchmark_set_resolved(bench, false);
	if (xccdf_benchmark_get_resolved(bench))
		fprintf(stderr, "Benchmark is already resolved!\n");
	else {
		if (xccdf_benchmark_resolve(bench)) {
			if (xccdf_benchmark_export(bench, action->f_results))
				ret = OSCAP_OK;
			else ret = OSCAP_ERROR;
		} else fprintf(stderr, "Benchmark resolving failure (probably a dependency loop)!\n");
	}

	xccdf_benchmark_free(bench);

	return ret;
}

static int xccdf_gen_report(const char *infile, const char *id, const char *outfile, const char *show, const char *oval_template, const char *sce_template)
{
    const char *params[] = { "result-id", id, "show", show, "verbosity", "", "oval-template", oval_template, "sce-template", sce_template, NULL };
    return app_xslt(infile, "xccdf-report.xsl", outfile, params);
}

int app_xccdf_xslt(const struct oscap_action *action)
{
    assert(action->module->user);
    const char *params[] = {
        "result-id",         action->id,
        "show",              action->show,
        "profile",           action->profile,
        "template",          action->tmpl,
        "format",            action->format,
        "oval-template",     action->oval_template,
#ifdef ENABLE_SCE
        "sce-template",      action->sce_template,
#endif
        "verbosity",         action->verbosity >= 0 ? "1" : "",
        "hide-profile-info", action->hide_profile_info ? "yes" : NULL,
        NULL };

    // in case user wants to "generate custom"
    if (action->module->user == NULL) {
        action->module->user = (void*)action->stylesheet;
    }

    return app_xslt(action->f_xccdf, action->module->user, action->f_results, params);
}

bool getopt_generate(int argc, char **argv, struct oscap_action *action)
{
	static const struct option long_options[] = {
		{"profile", 1, 0, 3},
		{"format", 1, 0, 4},
		{0, 0, 0, 0}
	};

	int c;
	while ((c = getopt_long(argc, argv, "+", long_options, NULL)) != -1) {
		switch (c) {
		case 3: action->profile = optarg; break;
		case 4: action->format = optarg; break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}
    return true;
}

enum oval_opt {
    XCCDF_OPT_RESULT_FILE = 1,
    XCCDF_OPT_PROFILE,
    XCCDF_OPT_REPORT_FILE,
    XCCDF_OPT_SHOW,
    XCCDF_OPT_TEMPLATE,
    XCCDF_OPT_FORMAT,
    XCCDF_OPT_OVAL_TEMPLATE,
    XCCDF_OPT_STYLESHEET_FILE,
#ifdef ENABLE_SCE
    XCCDF_OPT_SCE_TEMPLATE,
#endif
    XCCDF_OPT_FILE_VERSION,
    XCCDF_OPT_OUTPUT = 'o',
    XCCDF_OPT_RESULT_ID = 'i'
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
		{"profile", 		required_argument, NULL, XCCDF_OPT_PROFILE},
		{"result-id",		required_argument, NULL, XCCDF_OPT_RESULT_ID},
		{"report", 		required_argument, NULL, XCCDF_OPT_REPORT_FILE},
		{"show", 		required_argument, NULL, XCCDF_OPT_SHOW},
		{"template", 		required_argument, NULL, XCCDF_OPT_TEMPLATE},
		{"format", 		required_argument, NULL, XCCDF_OPT_FORMAT},
		{"oval-template", 	required_argument, NULL, XCCDF_OPT_OVAL_TEMPLATE},
		{"stylesheet",	required_argument, NULL, XCCDF_OPT_STYLESHEET_FILE},
#ifdef ENABLE_SCE
		{"sce-template", 	required_argument, NULL, XCCDF_OPT_SCE_TEMPLATE},
#endif
	// flags
		{"force",		no_argument, &action->force, 1},
		{"oval-results",	no_argument, &action->oval_results, 1},
#ifdef ENABLE_SCE
		{"sce-results",	no_argument, &action->sce_results, 1},
#endif
		{"skip-valid",		no_argument, &action->validate, 0},
		{"hide-profile-info",	no_argument, &action->hide_profile_info, 1},
		{"export-variables",	no_argument, &action->export_variables, 1},
	// end
		{0, 0, 0, 0}
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:i:", long_options, NULL)) != -1) {

		switch (c) {
		case XCCDF_OPT_OUTPUT: 
		case XCCDF_OPT_RESULT_FILE:	action->f_results = optarg;	break;
		case XCCDF_OPT_PROFILE:		action->profile = optarg;	break;
		case XCCDF_OPT_RESULT_ID:	action->id = optarg;		break;
		case XCCDF_OPT_REPORT_FILE:	action->f_report = optarg; 	break;
		case XCCDF_OPT_SHOW:		action->show = optarg;		break;
		case XCCDF_OPT_TEMPLATE:	action->tmpl = optarg;		break;
		case XCCDF_OPT_FORMAT:		action->format = optarg;	break;
		case XCCDF_OPT_OVAL_TEMPLATE:	action->oval_template = optarg; break;
		// we use realpath to get an absolute path to given XSLT to prevent openscap from looking
		// into /usr/share/openscap/xsl instead of CWD
		case XCCDF_OPT_STYLESHEET_FILE: realpath(optarg, custom_stylesheet_path); action->stylesheet = custom_stylesheet_path; break;
#ifdef ENABLE_SCE
		case XCCDF_OPT_SCE_TEMPLATE:	action->sce_template = optarg; break;
#endif
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
                if (action->f_report && !action->f_results)
                    return oscap_module_usage(action->module, stderr, "Please specify --results if you want to generate a HTML report.");

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
	} else {
		if (optind >= argc)
			return oscap_module_usage(action->module, stderr, "XCCDF file needs to be specified!");
		action->f_xccdf = argv[optind];
	}

	return true;
}

