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
 *      Maros Barabas <mbarabas@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* OVAL & OSCAP common */
#include <oval_probe.h>
#include <oval_agent_api.h>
#include <oval_results.h>
#include <oval_variables.h>
#include <error.h>
#include <text.h>
#include <assert.h>

#include "oscap-tool.h"

static int app_collect_oval(const struct oscap_action *action);
static int app_evaluate_oval(const struct oscap_action *action);
static int app_oval_xslt(const struct oscap_action *action);
static int app_oval_list_probes(const struct oscap_action *action);
static bool getopt_oval(int argc, char **argv, struct oscap_action *action);
static bool getopt_oval_list_probes(int argc, char **argv, struct oscap_action *action);
static bool getopt_oval_validate(int argc, char **argv, struct oscap_action *action);
static int app_analyse_oval(const struct oscap_action *action);

static bool valid_inputs(const struct oscap_action *action);

static struct oscap_module* OVAL_SUBMODULES[];
static struct oscap_module* OVAL_GEN_SUBMODULES[];

struct oscap_module OSCAP_OVAL_MODULE = {
    .name = "oval",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "Open Vulnerability and Assessment Language",
    .submodules = OVAL_SUBMODULES
};

static struct oscap_module OVAL_VALIDATE = {
    .name = "validate-xml",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Validate OVAL XML content",
    .usage = "[options] oval-file.xml",
    .help =
        "Options:\n"
        "   --definitions\r\t\t\t\t - Validate OVAL Definitions\n"
        "   --variables\r\t\t\t\t - Validate external OVAL Variables\n"
        "   --syschar\r\t\t\t\t - Validate OVAL System Characteristics\n"
        "   --results\r\t\t\t\t - Validate OVAL Results\n"
        "   --schematron\r\t\t\t\t - Use schematron-based validation in addition to XML Schema\n",
    .opt_parser = getopt_oval_validate,
    .func = app_validate_xml
};

static struct oscap_module OVAL_EVAL = {
    .name = "eval",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Probe the system and evaluate definitions from OVAL Definition file",
    .usage = "[options] oval-definitions.xml",
    .help =
        "Options:\n"
	"   --id <definition-id>\r\t\t\t\t - ID of the definition we want to evaluate.\n"
	"   --variables <file>\r\t\t\t\t - Provide external variables expected by OVAL Definitions.\n"
        "   --directives <file>\r\t\t\t\t - Use OVAL Directives content to specify desired results content.\n"
        "   --results <file>\r\t\t\t\t - Write OVAL Results into file.\n"
        "   --report <file>\r\t\t\t\t - Create human readable (HTML) report from OVAL Results.\n"
        "   --skip-valid\r\t\t\t\t - Skip validation.\n",
    .opt_parser = getopt_oval,
    .func = app_evaluate_oval
};

static struct oscap_module OVAL_COLLECT = {
    .name = "collect",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Probe the system and create system characteristics",
    .usage = "[options] oval-definitions.xml",
    .help =
	"Options:\n"
	"   --id <object>\r\t\t\t\t - Collect system characteristics ONLY for specified OVAL Object.\n"
        "   --syschar <file>\r\t\t\t\t - Write OVAL System Characteristic into file.\n"
	"   --variables <file>\r\t\t\t\t - Provide external variables expected by OVAL Definitions.\n"
        "   --skip-valid\r\t\t\t\t - Skip validation.\n",
    .opt_parser = getopt_oval,
    .func = app_collect_oval
};

static struct oscap_module OVAL_ANALYSE = {
    .name = "analyse",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Evaluate provided system characteristics file",
    .usage = "[options] --results FILE oval-definitions.xml system-characteristics.xml" ,
    .help =
	"Options:\n"
	"   --variables <file>\r\t\t\t\t - Provide external variables expected by OVAL Definitions.\n"
        "   --directives <file>\r\t\t\t\t - Use OVAL Directives content to specify desired results content.\n"
        "   --skip-valid\r\t\t\t\t - Skip validation.\n",
    .opt_parser = getopt_oval,
    .func = app_analyse_oval
};

static struct oscap_module OVAL_GENERATE = {
    .name = "generate",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Convert an OVAL file to other formats",
    .usage_extra = "<subcommand> [sub-options] oval-file.xml",
    .submodules = OVAL_GEN_SUBMODULES
};

static struct oscap_module OVAL_REPORT = {
    .name = "report",
    .parent = &OVAL_GENERATE,
    .summary = "Generate a HTML report from OVAL results file",
    .usage = "[options] oval-file.xml",
    .help =
        "Options:\n"
        "   --output <file>\r\t\t\t\t - Write the HTML into file.",
    .opt_parser = getopt_oval,
    .user = "oval-results-report.xsl",
    .func = app_oval_xslt
};

static struct oscap_module OVAL_LIST_PROBES = {
    .name = "list-probes",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "List supported object types (i.e. probes)",
    .usage = "[options]",
    .help = "Options:\n"
            "   --static\r\t\t\t\t - List all probes defined in the internal tables.\n"
            "   --dynamic\r\t\t\t\t - List all probes supported on the current system (this is default behavior).\n"
            "   --verbose\r\t\t\t\t - Be verbose.",
    .opt_parser = getopt_oval_list_probes,
    .func = app_oval_list_probes
};

static struct oscap_module* OVAL_GEN_SUBMODULES[] = {
    &OVAL_REPORT,
    NULL
};
static struct oscap_module* OVAL_SUBMODULES[] = {
    &OVAL_COLLECT,
    &OVAL_EVAL,
    &OVAL_ANALYSE,
    &OVAL_VALIDATE,
    &OVAL_GENERATE,
    &OVAL_LIST_PROBES,
    NULL
};


int VERBOSE;

struct oval_usr {
	int result_false;
	int result_true;
	int result_error;
	int result_unknown;
	int result_neval;
	int result_napp;
};

static int oval_gen_report(const char *infile, const char *outfile)
{
    return app_xslt(infile, "oval-results-report.xsl", outfile, NULL);
}

static int app_oval_callback(const struct oscap_reporter_message *msg, void *arg)
{

	if (VERBOSE >= 0)
		printf("Definition %s: %s\n",
		       oscap_reporter_message_get_user1str(msg),
		       oval_result_get_text(oscap_reporter_message_get_user2num(msg)));
	switch ((oval_result_t) oscap_reporter_message_get_user2num(msg)) {
	case OVAL_RESULT_TRUE:
		((struct oval_usr *)arg)->result_true++;
		break;
	case OVAL_RESULT_FALSE:
		((struct oval_usr *)arg)->result_false++;
		break;
	case OVAL_RESULT_ERROR:
		((struct oval_usr *)arg)->result_error++;
		break;
	case OVAL_RESULT_UNKNOWN:
		((struct oval_usr *)arg)->result_unknown++;
		break;
	case OVAL_RESULT_NOT_EVALUATED:
		((struct oval_usr *)arg)->result_neval++;
		break;
	case OVAL_RESULT_NOT_APPLICABLE:
		((struct oval_usr *)arg)->result_napp++;
		break;
	default:
		break;
	}

	return 0;
}

int app_collect_oval(const struct oscap_action *action)
{
	struct oval_definition_model	*def_model = NULL;
	struct oval_variable_model	*var_model = NULL;
	struct oval_syschar_model	*sys_model = NULL;
	struct oval_sysinfo		*sysinfo   = NULL;
	struct oval_probe_session	*pb_sess   = NULL;
	struct oval_generator		*generator = NULL;
	int ret = OSCAP_ERROR;

	/* validate inputs */
	if (action->validate) {
		if (!valid_inputs(action)) {
			goto cleanup;
		}
	}

	/* import definitions */
	def_model = oval_definition_model_import(action->f_oval);
	if (def_model == NULL) {
		fprintf(stderr, "Failed to import the OVAL Definitions from (%s).\n", action->f_oval);
		goto cleanup;
	}

	/* bind external variables */
	if(action->f_variables) {
		var_model = oval_variable_model_import(action->f_variables);
		if (var_model == NULL) {
			fprintf(stderr, "Failed to import the OVAL Variables from (%s).\n", action->f_oval);
			goto cleanup;
		}

		if (oval_definition_model_bind_variable_model(def_model, var_model)) {
			fprintf(stderr, "Failed to bind Variables to Definitions\n");
			goto cleanup;
		}
	}

	/* create empty syschar model */
	sys_model = oval_syschar_model_new(def_model);

	/* set product name */
	generator = oval_syschar_model_get_generator(sys_model);
	oval_generator_set_product_name(generator, OSCAP_PRODUCTNAME);

	/* create probe session */
	pb_sess = oval_probe_session_new(sys_model);

	/* query sysinfo */
	ret = oval_probe_query_sysinfo(pb_sess, &sysinfo);
	if (ret != 0) {
		fprintf(stderr, "Failed to query sysinfo\n");
		goto cleanup;
	}
	oval_syschar_model_set_sysinfo(sys_model, sysinfo);

	/* query objects */
	struct oval_object *object;
	struct oval_syschar *syschar;
	oval_syschar_collection_flag_t sc_flg;
	if (action->id) {
		object = oval_definition_model_get_object(def_model, action->id);
		if (!object) {
			fprintf(stderr, "Object ID(%s) does not exist in %s.\n", action->id, action->f_oval);
			goto cleanup;
		}
		if (VERBOSE >= 0)
			fprintf(stderr, "Collected: \"%s\" : ", oval_object_get_id(object));
		oval_probe_query_object(pb_sess, object, 0, &syschar);
		sc_flg = oval_syschar_get_flag(syschar);
		if (VERBOSE >= 0)
			fprintf(stderr, "%s\n", oval_syschar_collection_flag_get_text(sc_flg));
	}
	else {
	        struct oval_object_iterator *objects = oval_definition_model_get_objects(def_model);
		while (oval_object_iterator_has_more(objects)) {
			object = oval_object_iterator_next(objects);
			if (VERBOSE >= 0)
				fprintf(stderr, "Collected: \"%s\" : ", oval_object_get_id(object));
			oval_probe_query_object(pb_sess, object, 0, &syschar);
			sc_flg = oval_syschar_get_flag(syschar);
			if (VERBOSE >= 0)
				fprintf(stderr, "%s\n", oval_syschar_collection_flag_get_text(sc_flg));
		}
		oval_object_iterator_free(objects);
	}

	const char* full_validation = getenv("OSCAP_FULL_VALIDATION");

	/* output */
	if (action->f_syschar != NULL) {
		/* export OVAL System Characteristics */
		oval_syschar_model_export(sys_model, action->f_syschar);

		/* validate OVAL System Characteristics */
		if (action->validate && full_validation) {
			xmlChar *doc_version;

			doc_version = oval_determine_document_schema_version((const char *) action->f_syschar, OSCAP_DOCUMENT_OVAL_SYSCHAR);
			if (!oscap_validate_document(action->f_syschar, OSCAP_DOCUMENT_OVAL_SYSCHAR, (const char *) doc_version,
			    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
				fprintf(stdout, "OVAL System Characteristics are NOT exported correctly.\n");
				xmlFree(doc_version);
				goto cleanup;
			}
			fprintf(stdout, "OVAL System Characteristics are exported correctly.\n");
			xmlFree(doc_version);
		}
	}

	ret = OSCAP_OK;

cleanup:
	if(oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	if (sysinfo) oval_sysinfo_free(sysinfo);
	if (pb_sess) oval_probe_session_destroy(pb_sess);
	if (sys_model) oval_syschar_model_free(sys_model);
	if (def_model) oval_definition_model_free(def_model);

	return ret;
}


int app_evaluate_oval(const struct oscap_action *action)
{

	struct oval_definition_model	*def_model = NULL;
	struct oval_variable_model	*var_model = NULL;
	struct oval_directives_model	*dir_model = NULL;
	struct oval_usr			*usr       = NULL;
	oval_agent_session_t		*sess      = NULL;
	int ret = OSCAP_ERROR;

	/* validate inputs */
	if (action->validate) {
		if (!valid_inputs(action)) {
			goto cleanup;
		}
	}

	/* import OVAL Definitions */
	def_model = oval_definition_model_import(action->f_oval);
	if (def_model == NULL) {
		fprintf(stderr, "Failed to import the OVAL Definitions from (%s).\n", action->f_oval);
		goto cleanup;
	}

	/* bind external variables */
	if(action->f_variables) {
		var_model = oval_variable_model_import(action->f_variables);
		if (var_model == NULL) {
			fprintf(stderr, "Failed to import the OVAL Variables from (%s).\n", action->f_oval);
			goto cleanup;
		}

		if (oval_definition_model_bind_variable_model(def_model, var_model)) {
			fprintf(stderr, "Failed to bind Variables to Definitions\n");
			goto cleanup;
		}
	}

	sess = oval_agent_new_session(def_model, basename(action->f_oval));
	if (sess == NULL) {
		fprintf(stderr, "Failed to create new agent session.\n");
		goto cleanup;
	}

	/* set product name */
	oval_agent_set_product_name(sess, OSCAP_PRODUCTNAME);

	/* Init usr structure */
	usr = malloc(sizeof(struct oval_usr));
	memset(usr, 0, sizeof(struct oval_usr));

	/* Evaluation */
	if (action->id)
		oval_agent_eval_definition(sess, action->id);
	else
		oval_agent_eval_system(sess, app_oval_callback, usr);

	if (oscap_err())
		goto cleanup;

	printf("Evaluation done.\n");

	/* export results to file */
	if (action->f_results != NULL) {
		/* get result model */
		struct oval_results_model *res_model = oval_agent_get_results_model(sess);

		/* import directives */
		if (action->f_directives != NULL) {
			dir_model = oval_directives_model_new();
			oval_directives_model_import(dir_model, action->f_directives);
		}

		/* export result model to XML */
		oval_results_model_export(res_model, dir_model, action->f_results);

		const char* full_validation = getenv("OSCAP_FULL_VALIDATION");

		/* validate OVAL Results */
		if (action->validate && full_validation) {
			xmlChar *doc_version;

			doc_version = oval_determine_document_schema_version((const char *) action->f_results, OSCAP_DOCUMENT_OVAL_RESULTS);
			if (!oscap_validate_document(action->f_results, OSCAP_DOCUMENT_OVAL_RESULTS, (const char *) doc_version,
			    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
				fprintf(stdout, "OVAL Results are NOT exported correctly.\n");
				xmlFree(doc_version);
				goto cleanup;
			}
			fprintf(stdout, "OVAL Results are exported correctly.\n");
			xmlFree(doc_version);
		}

		/* generate report */
	        if (action->f_report != NULL)
        		oval_gen_report(action->f_results, action->f_report);
	}

	/* "calculate" return code */
	if (action->id) {
		oval_result_t res;

		if (oval_agent_get_definition_result(sess, action->id, &res)==-1) {
			goto cleanup;
		}

		if (VERBOSE >= 0)
			printf("Definition %s: %s\n", action->id, oval_result_get_text(res));
		ret = (res == OVAL_RESULT_FALSE) ? OSCAP_FAIL : OSCAP_OK;
	} else {
		ret = (usr->result_false > 0) ? OSCAP_FAIL : OSCAP_OK;
	}

	/* clean up */
cleanup:
	if(oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	if (usr) free(usr);
	if (sess) oval_agent_destroy_session(sess);
	if (def_model) oval_definition_model_free(def_model);
	if (dir_model) oval_directives_model_free(dir_model);

	return ret;
}

static int app_analyse_oval(const struct oscap_action *action) {
	struct oval_definition_model	*def_model = NULL;
	struct oval_syschar_model	*sys_model = NULL;
	struct oval_results_model	*res_model = NULL;
	struct oval_variable_model	*var_model = NULL;
	struct oval_directives_model	*dir_model = NULL;
 	struct oval_syschar_model	*sys_models[2];
	struct oval_generator		*generator = NULL;
	int ret = OSCAP_ERROR;

	/* validate inputs */
	if (action->validate) {
		if (!valid_inputs(action)) {
			goto cleanup;
		}
	}

	/* load defnitions */
	def_model = oval_definition_model_import(action->f_oval);
        if (def_model == NULL) {
                fprintf(stderr, "Failed to import the OVAL Definitions from (%s).\n", action->f_oval);
		goto cleanup;
        }

	/* bind external variables */
	if(action->f_variables) {
		var_model = oval_variable_model_import(action->f_variables);
		if (var_model == NULL) {
			fprintf(stderr, "Failed to import the OVAL Variables from (%s).\n", action->f_oval);
			goto cleanup;
		}

		if (oval_definition_model_bind_variable_model(def_model, var_model)) {
			fprintf(stderr, "Failed to bind Variables to Definitions\n");
			goto cleanup;
		}
	}

	/* load system characteristics */
	sys_model = oval_syschar_model_new(def_model);
        if (oval_syschar_model_import(sys_model, action->f_syschar) ==  -1 ) {
                fprintf(stderr, "Failed to import the System Characteristics from (%s).\n", action->f_syschar);
                goto cleanup;
        }

	/* evaluate */
	sys_models[0] = sys_model;
	sys_models[1] = NULL;
	res_model = oval_results_model_new(def_model, sys_models);

	/* set product name */
        generator = oval_results_model_get_generator(res_model);
        oval_generator_set_product_name(generator, OSCAP_PRODUCTNAME);

	oval_results_model_eval(res_model);

	/* export results */
	if (action->f_results != NULL) {
		/* import directives */
		if (action->f_directives != NULL) {
			dir_model = oval_directives_model_new();
			oval_directives_model_import(dir_model, action->f_directives);
		}

		/* export result model to XML */
		oval_results_model_export(res_model, dir_model, action->f_results);

		const char* full_validation = getenv("OSCAP_FULL_VALIDATION");

		/* validate OVAL Results */
		if (action->validate && full_validation) {
			xmlChar *doc_version;

			doc_version = oval_determine_document_schema_version((const char *) action->f_results, OSCAP_DOCUMENT_OVAL_RESULTS);
			if (!oscap_validate_document(action->f_results, OSCAP_DOCUMENT_OVAL_RESULTS, (const char *) doc_version,
			    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
				fprintf(stdout, "OVAL Results are NOT exported correctly.\n");
				xmlFree(doc_version);
				goto cleanup;
			}
			fprintf(stdout, "OVAL Results are exported correctly.\n");
			xmlFree(doc_version);
		}
	}

	ret = OSCAP_OK;

	/* clean up */
cleanup:
	if(oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	if(res_model) oval_results_model_free(res_model);
	if(sys_model) oval_syschar_model_free(sys_model);
	if(def_model) oval_definition_model_free(def_model);
	if(dir_model) oval_directives_model_free(dir_model);

	return ret;
}

static int app_oval_xslt(const struct oscap_action *action)
{
    assert(action->module->user);
    return app_xslt(action->f_oval, action->module->user, action->f_results, NULL);
}

static int app_oval_list_probes(const struct oscap_action *action)
{
    int flags = 0;

    if (action->list_dynamic)
	flags |= OVAL_PROBEMETA_LIST_DYNAMIC;
    if (action->verbosity >= 10)
	flags |= OVAL_PROBEMETA_LIST_VERBOSE;

    oval_probe_meta_list(stdout, flags);
    return (0);
}

enum oval_opt {
    OVAL_OPT_RESULT_FILE = 1,
    OVAL_OPT_REPORT_FILE,
    OVAL_OPT_ID,
    OVAL_OPT_VARIABLES,
    OVAL_OPT_SYSCHAR,
    OVAL_OPT_DIRECTIVES,
    OVAL_OPT_OUTPUT = 'o'
};

bool getopt_oval(int argc, char **argv, struct oscap_action *action)
{
	VERBOSE = action->verbosity;

	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command-options */
	struct option long_options[] = {
        // options
		{ "results", 	required_argument, NULL, OVAL_OPT_RESULT_FILE  },
		{ "report",  	required_argument, NULL, OVAL_OPT_REPORT_FILE  },
		{ "id",        	required_argument, NULL, OVAL_OPT_ID           },
		{ "output",    	required_argument, NULL, OVAL_OPT_OUTPUT       },
		{ "variables",	required_argument, NULL, OVAL_OPT_VARIABLES    },
		{ "syschar",	required_argument, NULL, OVAL_OPT_SYSCHAR      },
		{ "directives",	required_argument, NULL, OVAL_OPT_DIRECTIVES   },
        // flags
		{ "skip-valid",	no_argument, &action->validate, 0 },
        // end
		{ 0, 0, 0, 0 }
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:", long_options, NULL)) != -1) {
		switch (c) {
		case OVAL_OPT_RESULT_FILE: action->f_results = optarg; break;
		case OVAL_OPT_REPORT_FILE: action->f_report  = optarg; break;
		case OVAL_OPT_OUTPUT: action->f_results = optarg; break;
		case OVAL_OPT_ID: action->id = optarg; break;
		case OVAL_OPT_VARIABLES: action->f_variables = optarg; break;
		case OVAL_OPT_SYSCHAR: action->f_syschar = optarg; break;
		case OVAL_OPT_DIRECTIVES: action->f_directives = optarg; break;
        	case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	/* We should have Definitions file here */
	if (optind >= argc)
		return oscap_module_usage(action->module, stderr, "Definitions file is not specified!");
	action->f_oval = argv[optind];

	if (action->module == &OVAL_ANALYSE) {
		/* We should have System Characteristics file here */
		if ((optind+1) > argc)
			return oscap_module_usage(action->module, stderr, "System characteristics file is not specified");
		action->f_syschar = argv[optind + 1];

		if (action->f_results == NULL) {
			return oscap_module_usage(action->module, stderr, "OVAL Results file is not specified(--results parameter)");
		}
	}

	return true;
}

bool getopt_oval_list_probes(int argc, char **argv, struct oscap_action *action)
{
#define PROBE_LIST_STATIC  0
#define PROBE_LIST_DYNAMIC 1

        int list_type = PROBE_LIST_DYNAMIC;
	VERBOSE = action->verbosity;

	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command-options */
	struct option long_options[] = {
        // flags
		{ "static",	no_argument, &list_type, PROBE_LIST_STATIC  },
		{ "dynamic",	no_argument, &list_type, PROBE_LIST_DYNAMIC },
		{ "verbose",    no_argument, &action->verbosity, 10},
        // end
		{ 0, 0, 0, 0 }
	};

	int c;
	while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
		switch (c) {
        	case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	action->list_dynamic = list_type == PROBE_LIST_DYNAMIC ? true : false;

	return true;
}

bool getopt_oval_validate(int argc, char **argv, struct oscap_action *action)
{
	VERBOSE = action->verbosity;

	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command-options */
	struct option long_options[] = {
        // flags
		{ "definitions",	no_argument, &action->doctype, OSCAP_DOCUMENT_OVAL_DEFINITIONS },
		{ "variables",		no_argument, &action->doctype, OSCAP_DOCUMENT_OVAL_VARIABLES   },
		{ "syschar",		no_argument, &action->doctype, OSCAP_DOCUMENT_OVAL_SYSCHAR     },
		{ "results",		no_argument, &action->doctype, OSCAP_DOCUMENT_OVAL_RESULTS     },
		{ "directives",		no_argument, &action->doctype, OSCAP_DOCUMENT_OVAL_DIRECTIVES  },
		// force schematron validation
		{ "schematron",		no_argument, &action->force, 1 },
        // end
		{ 0, 0, 0, 0 }
	};

	int c;
	while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
		switch (c) {
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	/* We should have Definitions file here */
	if (optind >= argc)
		return oscap_module_usage(action->module, stderr, "Definitions file needs to be specified!");
	action->f_oval = argv[optind];

	return true;
}

static bool valid_inputs(const struct oscap_action *action) {
        bool ret = false;
	xmlChar *doc_version;

	/* validate OVAL Definitions & Variables & Syschars */
	doc_version = oval_determine_document_schema_version((const char *) action->f_oval, OSCAP_DOCUMENT_OVAL_DEFINITIONS);
	if (!oscap_validate_document(action->f_oval, OSCAP_DOCUMENT_OVAL_DEFINITIONS, (const char *) doc_version,
	    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
		fprintf(stdout, "Invalid OVAL Definition content in %s\n", action->f_oval);
		goto cleanup;
	}

	if (action->f_variables) {
		xmlFree(doc_version);
		doc_version = oval_determine_document_schema_version((const char *) action->f_variables, OSCAP_DOCUMENT_OVAL_VARIABLES);
		if (!oscap_validate_document(action->f_variables, OSCAP_DOCUMENT_OVAL_VARIABLES, (const char *) doc_version,
		    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
			fprintf(stdout, "Invalid OVAL Variables content in %s\n", action->f_variables);
			goto cleanup;
		}
	}

	if (action->f_directives) {
		xmlFree(doc_version);
		doc_version = oval_determine_document_schema_version((const char *) action->f_directives, OSCAP_DOCUMENT_OVAL_DIRECTIVES);
		if (!oscap_validate_document(action->f_directives, OSCAP_DOCUMENT_OVAL_DIRECTIVES, (const char *) doc_version,
		    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
			fprintf(stdout, "Invalid OVAL Directives content in %s\n", action->f_directives);
			goto cleanup;
		}
	}

	if (action->module == &OVAL_ANALYSE && action->f_syschar) {
		xmlFree(doc_version);
		doc_version = oval_determine_document_schema_version((const char *) action->f_syschar, OSCAP_DOCUMENT_OVAL_SYSCHAR);
		if (!oscap_validate_document(action->f_syschar, OSCAP_DOCUMENT_OVAL_SYSCHAR, (const char *) doc_version,
		    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
			fprintf(stdout, "Invalid OVAL System Characteristics content in %s\n", action->f_syschar);
			goto cleanup;
		}
	}

	ret = true;

cleanup:
	xmlFree(doc_version);

	return ret;
}

