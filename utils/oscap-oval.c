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
static bool getopt_oval(int argc, char **argv, struct oscap_action *action);
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
	"   --variables <file>\r\t\t\t\t - Provide external variables expected by OVAL Definitions.\n",
    .opt_parser = getopt_oval,
    .func = app_collect_oval
};

static struct oscap_module OVAL_ANALYSE = {
    .name = "analyse",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Evaluate provided system characteristics file",
    .usage = "[options] oval-definitions.xml system-characteristics.xml" ,
    .help =
	"Options:\n"
	"   --variables <file>\r\t\t\t\t - Provide external variables expected by OVAL Definitions.\n",
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
	int ret;

	/* validate inputs */
	if (action->validate) {
		if (!valid_inputs(action)) {
			ret = OSCAP_ERROR;
			goto cleanup;
		}
	}

	/* import definitions */
	def_model = oval_definition_model_import(action->f_oval);
	if (def_model == NULL) {
		fprintf(stderr, "Failed to import the OVAL Definitions from (%s).\n", action->f_oval);
		if(oscap_err())
			fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
		ret = OSCAP_ERROR;
		goto cleanup;
	}

	/* bind external variables */
	if(action->f_variables) {
		var_model = oval_variable_model_import(action->f_variables);
		if (var_model == NULL) {
			fprintf(stderr, "Failed to import the OVAL Variables from (%s).\n", action->f_oval);
			if(oscap_err())
				fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			ret = OSCAP_ERROR;
			goto cleanup;
		}

		if (oval_definition_model_bind_variable_model(def_model, var_model)) {
			fprintf(stderr, "Failed to bind Variables to Definitions\n");
			if(oscap_err())
				fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			ret = OSCAP_ERROR;
			goto cleanup;
		}
	}

	/* create empty syschar model */
	sys_model = oval_syschar_model_new(def_model);

	/* create probe session */
	pb_sess = oval_probe_session_new(sys_model);

	/* query sysinfo */
	ret = oval_probe_query_sysinfo(pb_sess, &sysinfo);
	if (ret != 0) {
		fprintf(stderr, "Failed to query sysinfo\n");
		if(oscap_err())
			fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
		ret = OSCAP_ERROR;
		goto cleanup;
	}
	oval_syschar_model_set_sysinfo(sys_model, sysinfo);

	/* query objects */
	struct oval_object *object;
	struct oval_syschar *syschar;
	oval_syschar_collection_flag_t sc_flg;
	struct oval_message_iterator *messages;
	if (action->id) {
		object = oval_definition_model_get_object(def_model, action->id);
		if (VERBOSE >= 0) fprintf(stderr, "Collected: \"%s\" : ", oval_object_get_id(object));
		oval_probe_query_object(pb_sess, object, 0, &syschar);
		sc_flg = oval_syschar_get_flag(syschar);
		if (VERBOSE >= 0) {
			fprintf(stderr, "%s\n", oval_syschar_collection_flag_get_text(sc_flg));
			if (sc_flg == SYSCHAR_FLAG_ERROR) {
				messages = oval_syschar_get_messages(syschar);
				while (oval_message_iterator_has_more(messages)) {
					struct oval_message *message = oval_message_iterator_next(messages);
					fprintf(stderr, "\t%s\n", oval_message_get_text(message));
				}
				oval_message_iterator_free(messages);
			}
		}
	}
	else {
	        struct oval_object_iterator *objects = oval_definition_model_get_objects(def_model);
		while (oval_object_iterator_has_more(objects)) {
			object = oval_object_iterator_next(objects);
			if (VERBOSE >= 0) fprintf(stderr, "Collected: \"%s\" : ", oval_object_get_id(object));
			oval_probe_query_object(pb_sess, object, 0, &syschar);
			sc_flg = oval_syschar_get_flag(syschar);
			if (VERBOSE >= 0) {
				fprintf(stderr, "%s\n", oval_syschar_collection_flag_get_text(sc_flg));
				if (sc_flg == SYSCHAR_FLAG_ERROR) {
					messages = oval_syschar_get_messages(syschar);
					while (oval_message_iterator_has_more(messages)) {
						struct oval_message *message = oval_message_iterator_next(messages);
						fprintf(stderr, "\t%s\n", oval_message_get_text(message));
					}
					oval_message_iterator_free(messages);
				}
			}
		}
		oval_object_iterator_free(objects);
	}

	/* output */
	oval_syschar_model_export(sys_model, action->f_syschar);

	ret = OSCAP_OK;

cleanup:
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
	struct oval_usr			*usr       = NULL;
	oval_agent_session_t		*sess      = NULL;
	int ret;

	/* validate inputs */
	if (action->validate) {
		if (!valid_inputs(action)) {
			ret = OSCAP_ERROR;
			goto cleanup;
		}
	}

	/* import OVAL Definitions */
	def_model = oval_definition_model_import(action->f_oval);
	if (def_model == NULL) {
		fprintf(stderr, "Failed to import the OVAL Definitions from (%s).\n", action->f_oval);
		if(oscap_err())
			fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
		ret = OSCAP_ERROR;
		goto cleanup;
	}

	/* bind external variables */
	if(action->f_variables) {
		var_model = oval_variable_model_import(action->f_variables);
		if (var_model == NULL) {
			fprintf(stderr, "Failed to import the OVAL Variables from (%s).\n", action->f_oval);
			if(oscap_err())
				fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			ret = OSCAP_ERROR;
			goto cleanup;
		}

		if (oval_definition_model_bind_variable_model(def_model, var_model)) {
			fprintf(stderr, "Failed to bind Variables to Definitions\n");
			if(oscap_err())
				fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			ret = OSCAP_ERROR;
			goto cleanup;
		}
	}

	sess = oval_agent_new_session(def_model, basename(action->f_oval));
	if (sess == NULL) {
		if (oscap_err())
			fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
		fprintf(stderr, "Failed to create new agent session.\n");
		ret = OSCAP_ERROR;
		goto cleanup;
	}

	/* Init usr structure */
	usr = malloc(sizeof(struct oval_usr));
	memset(usr, 0, sizeof(struct oval_usr));

	/* Evaluation */
	if (action->id)
		ret = oval_agent_eval_definition(sess, action->id);
	else
		ret = oval_agent_eval_system(sess, app_oval_callback, usr);

	if (ret && (oscap_err())) {
		fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
		ret = OSCAP_ERROR;
		goto cleanup;
	} else
		printf("Evaluation done.\n");


	/* export results to file */
	if (action->f_results != NULL) {
		/* get result model */
		struct oval_results_model *res_model = oval_agent_get_results_model(sess);

		/* set up directives */
		struct oval_result_directives *res_direct = oval_result_directives_new(res_model);
		oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE | OVAL_RESULT_FALSE |
						    OVAL_RESULT_UNKNOWN | OVAL_RESULT_NOT_EVALUATED |
						    OVAL_RESULT_ERROR | OVAL_RESULT_NOT_APPLICABLE, true);

		oval_result_directives_set_content(res_direct,
						   OVAL_RESULT_TRUE |
						   OVAL_RESULT_FALSE |
						   OVAL_RESULT_UNKNOWN |
						   OVAL_RESULT_NOT_EVALUATED |
						   OVAL_RESULT_NOT_APPLICABLE |
						   OVAL_RESULT_ERROR,
						   OVAL_DIRECTIVE_CONTENT_FULL);

		
		/* export result model to XML */
		oval_results_model_export(res_model, res_direct, action->f_results);
		oval_result_directives_free(res_direct);

		/* validate OVAL Results */
		if (!oscap_validate_document(action->f_results, OSCAP_DOCUMENT_OVAL_RESULTS, NULL,
		    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
			if (oscap_err()) {
				fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			}
			fprintf(stdout, "OVAL Results are NOT exported correctly.\n");
			ret = OSCAP_ERROR;
			goto cleanup;
		}
		fprintf(stdout, "OVAL Results are exported correctly.\n");

		/* generate report */
	        if (action->f_report != NULL)
        		oval_gen_report(action->f_results, action->f_report);
	}

	/* "calculate" return code */
	if (action->id) {
		oval_result_t res;

		res = oval_agent_get_definition_result(sess, action->id);
		if (VERBOSE >= 0)
			printf("Definition %s: %s\n", action->id, oval_result_get_text(res));
		ret = (res == OVAL_RESULT_FALSE) ? OSCAP_FAIL : OSCAP_OK;
	} else {
		ret = (usr->result_false > 0) ? OSCAP_FAIL : OSCAP_OK;
	}

	/* clean up */
cleanup:
	if (usr) free(usr);
	if (sess) oval_agent_destroy_session(sess);
	if (def_model) oval_definition_model_free(def_model);

	return ret;
}

static int app_analyse_oval(const struct oscap_action *action) {
	struct oval_definition_model	*def_model = NULL;
	struct oval_syschar_model	*sys_model = NULL;
	struct oval_results_model	*res_model = NULL;
	struct oval_variable_model	*var_model = NULL;
	struct oval_result_directives	*res_direct = NULL;
 	struct oval_syschar_model	*sys_models[2];
	int ret;

	/* validate inputs */
	if (action->validate) {
		if (!valid_inputs(action)) {
			ret = OSCAP_ERROR;
			goto cleanup;
		}
	}

	/* load defnitions */
	def_model = oval_definition_model_import(action->f_oval);
        if (def_model == NULL) {
                fprintf(stderr, "Failed to import the OVAL Definitions from (%s).\n", action->f_oval);
                ret = OSCAP_ERROR;
		goto cleanup;
        }

	/* bind external variables */
	if(action->f_variables) {
		var_model = oval_variable_model_import(action->f_variables);
		if (var_model == NULL) {
			fprintf(stderr, "Failed to import the OVAL Variables from (%s).\n", action->f_oval);
			ret = OSCAP_ERROR;
			goto cleanup;
		}

		if (oval_definition_model_bind_variable_model(def_model, var_model)) {
			fprintf(stderr, "Failed to bind Variables to Definitions\n");
			if(oscap_err())
				fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			ret = OSCAP_ERROR;
			goto cleanup;
		}
	}

	/* load system characteristics */
	sys_model = oval_syschar_model_new(def_model);
        if (oval_syschar_model_import(sys_model, action->f_syschar) ==  -1 ) {
                fprintf(stderr, "Failed to import the System Characteristics from (%s).\n", action->f_syschar);
		if(oscap_err())
			fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
		ret = OSCAP_ERROR;
                goto cleanup;
        }

	/* evaluate */
	sys_models[0] = sys_model;
	sys_models[1] = NULL;
	res_model = oval_results_model_new(def_model, sys_models);
	oval_results_model_eval(res_model);

	/* export results */
	res_direct = oval_result_directives_new(res_model);
	oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE | OVAL_RESULT_FALSE |
					    OVAL_RESULT_UNKNOWN | OVAL_RESULT_NOT_EVALUATED |
					    OVAL_RESULT_ERROR | OVAL_RESULT_NOT_APPLICABLE, true);
	oval_result_directives_set_content(res_direct,
					   OVAL_RESULT_TRUE |
					   OVAL_RESULT_FALSE |
					   OVAL_RESULT_UNKNOWN |
					   OVAL_RESULT_NOT_EVALUATED |
					   OVAL_RESULT_NOT_APPLICABLE |
					   OVAL_RESULT_ERROR,
					   OVAL_DIRECTIVE_CONTENT_FULL);
	oval_results_model_export(res_model, res_direct, "/dev/stdout");

	ret = OSCAP_OK;

	/* clean up */
cleanup:
	if(res_direct) oval_result_directives_free(res_direct);
	if(res_model) oval_results_model_free(res_model);
	if(sys_model) oval_syschar_model_free(sys_model);
	if(def_model) oval_definition_model_free(def_model);

	return ret;
}

static int app_oval_xslt(const struct oscap_action *action)
{
    assert(action->module->user);
    return app_xslt(action->f_oval, action->module->user, action->f_results, NULL);
}


enum oval_opt {
    OVAL_OPT_RESULT_FILE = 1,
    OVAL_OPT_REPORT_FILE,
    OVAL_OPT_ID,
    OVAL_OPT_VARIABLES,
    OVAL_OPT_SYSCHAR,
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
        // flags
		{ "skip-valid",	no_argument, &action->validate, 0 },
        // end
		{ 0, 0, 0, 0 }
	};

	int c;
	while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
		switch (c) {
		case OVAL_OPT_RESULT_FILE: action->f_results = optarg; break;
		case OVAL_OPT_REPORT_FILE: action->f_report  = optarg; break;
		case OVAL_OPT_OUTPUT: action->f_results = optarg; break;
		case OVAL_OPT_ID: action->id = optarg; break;
		case OVAL_OPT_VARIABLES: action->f_variables = optarg; break;
		case OVAL_OPT_SYSCHAR: action->f_syschar = optarg; break;
        	case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	/* We should have Definitions file here */
	if (optind >= argc)
        	return oscap_module_usage(action->module, stderr, "Definitions file needs to be specified!");
	action->f_oval = argv[optind];

	/* We should have System Characteristics file here */
	if (action->module == &OVAL_ANALYSE) {
		if ((optind+1) > argc)
			return oscap_module_usage(action->module, stderr, "System characteristics file needs to be specified!");
		action->f_syschar = argv[optind + 1];
	}

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
	/* validate OVAL Definitions & Variables & Syschars */
	if (!oscap_validate_document(action->f_oval, OSCAP_DOCUMENT_OVAL_DEFINITIONS, NULL,
	    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
		if (oscap_err()) {
			fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
		}
		else {
			fprintf(stdout, "Invalid OVAL Definition content in %s\n", action->f_oval);
		}
		return false;
	}

	if (action->f_variables) {
		if (!oscap_validate_document(action->f_variables, OSCAP_DOCUMENT_OVAL_VARIABLES, NULL,
		    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
			if (oscap_err()) {
				fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			}
			else {
				fprintf(stdout, "Invalid OVAL Variables content in %s\n", action->f_variables);
			}
			return false;
		}
	}

	if (action->f_syschar) {
		if (!oscap_validate_document(action->f_syschar, OSCAP_DOCUMENT_OVAL_SYSCHAR, NULL,
		    (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
			if (oscap_err()) {
				fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			}
			else {
				fprintf(stdout, "Invalid OVAL System Characteristics content in %s\n", action->f_syschar);
			}
			return false;
		}
	}

	return true;
}

