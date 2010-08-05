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

#include "oscap-tool.h"

static int app_collect_oval(const struct oscap_action *action);
static int app_evaluate_oval(const struct oscap_action *action);
static int app_evaluate_oval_id(const struct oscap_action *action);
static bool getopt_oval(int argc, char **argv, struct oscap_action *action);

static struct oscap_module* OVAL_SUBMODULES[];

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
        "   --syschar\r\t\t\t\t - Valiadate OVAL system characteristics\n"
        "   --definitions\r\t\t\t\t - Valiadate OVAL definitions\n"
        "   --results\r\t\t\t\t - Valiadate OVAL results\n"
        "   --file-version <version>\r\t\t\t\t - Use schema for given version of OVAL",
    .opt_parser = getopt_oval,
    .func = app_validate_xml
};

static struct oscap_module OVAL_EVAL = {
    .name = "eval",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Probe the system and evaluate all definitions from OVAL Definition file",
    .usage = "--result-file results.xml oval-definitions.xml",
    .help =
        "Options:\n"
        "   --result-file <file>\r\t\t\t\t - Write OVAL Results into file.",
    .opt_parser = getopt_oval,
    .func = app_evaluate_oval
};

static struct oscap_module OVAL_EVAL_ID = {
    .name = "eval-id",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Probe the system and evaluate specified definition from OVAL Definition file",
    .usage = "--id definition-id --result-file results.xml oval-definitions.xml",
    .help =
        "Options:\n"
        "   --id <definition-id>\r\t\t\t\t - ID of the definition we want to evaluate"
        "   --result-file <file>\r\t\t\t\t - Write OVAL Results into file.",
    .opt_parser = getopt_oval,
    .func = app_evaluate_oval_id
};

static struct oscap_module OVAL_COLLECT = {
    .name = "collect",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Probe the system and gather system characteristics for objects in OVAL Definition file",
    .usage = "oval-definitions.xml",
    .opt_parser = getopt_oval,
    .func = app_collect_oval
};

static struct oscap_module* OVAL_SUBMODULES[] = {
    &OVAL_COLLECT,
    &OVAL_EVAL,
    &OVAL_EVAL_ID,
    &OVAL_VALIDATE,
    NULL
};


int VERBOSE;

struct oval_usr {
	int result_false;
	int result_true;
	int result_invalid;
	int result_unknown;
	int result_neval;
	int result_napp;
};

static int app_oval_callback(const struct oscap_reporter_message *msg, void *arg)
{

	if (VERBOSE >= 0)
		printf("Evalutated definition %s: %s\n",
		       oscap_reporter_message_get_user1str(msg),
		       oval_result_get_text(oscap_reporter_message_get_user2num(msg)));
	switch ((oval_result_t) oscap_reporter_message_get_user2num(msg)) {
	case OVAL_RESULT_TRUE:
		((struct oval_usr *)arg)->result_true++;
		break;
	case OVAL_RESULT_FALSE:
		((struct oval_usr *)arg)->result_false++;
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
	int ret;
	struct oval_sysinfo *sysinfo;

	/* import definitions */
	struct oval_definition_model *def_model = oval_definition_model_import(action->f_oval);

	/* create empty syschar model */
	struct oval_syschar_model *sys_model = oval_syschar_model_new(def_model);

	/* create probe session */
	struct oval_probe_session *pb_sess = oval_probe_session_new(sys_model);

	/* query sysinfo */
	sysinfo = oval_probe_query_sysinfo(pb_sess);
	if (sysinfo == NULL) {
		oval_probe_session_destroy(pb_sess);
		oval_syschar_model_free(sys_model);
		oval_definition_model_free(def_model);
		return 1;
	}
	oval_syschar_model_set_sysinfo(sys_model, sysinfo);
	oval_sysinfo_free(sysinfo);

	/* query objects */
	ret = oval_probe_query_objects(pb_sess);
	if (ret != 0) {
		oval_probe_session_destroy(pb_sess);
		oval_syschar_model_free(sys_model);
		oval_definition_model_free(def_model);
		return 1;
	}

	/* report */
	oval_syschar_model_export(sys_model, "/dev/stdout");

	/* destroy */
	oval_probe_session_destroy(pb_sess);
	oval_syschar_model_free(sys_model);
	oval_definition_model_free(def_model);

	return 0;
}


int app_evaluate_oval(const struct oscap_action *action)
{

	struct oval_usr *usr = NULL;
	int ret = 0;

	struct oval_definition_model *def_model = oval_definition_model_import(action->f_oval);
	oval_agent_session_t *sess = oval_agent_new_session(def_model, basename(action->f_oval));

	/* Import OVAL definition file */
	if (oscap_err()) {
		fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;
	}

	/* Init usr structure */
	usr = malloc(sizeof(struct oval_usr));
	usr->result_false = 0;
	usr->result_true = 0;
	usr->result_invalid = 0;
	usr->result_unknown = 0;
	usr->result_neval = 0;
	usr->result_napp = 0;

	/* Evaluation */
	ret = oval_agent_eval_system(sess, app_oval_callback, usr);

	if (VERBOSE >= 0)
		printf("Evaluation: All done.\n");

	if (ret == -1 && (oscap_err())) {
		fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;
	}
	if (VERBOSE >= 0) {
		fprintf(stdout, "====== RESULTS ======\n");
		fprintf(stdout, "TRUE:          \r\t\t %d\n", usr->result_true);
		fprintf(stdout, "FALSE:         \r\t\t %d\n", usr->result_false);
		fprintf(stdout, "UNKNOWN:       \r\t\t %d\n", usr->result_unknown);
		fprintf(stdout, "NOT EVALUATED: \r\t\t %d\n", usr->result_neval);
		fprintf(stdout, "NOT APPLICABLE:\r\t\t %d\n", usr->result_napp);
	}

	/* export results to file */
	if (action->f_results != NULL) {
		/* get result model */
		struct oval_results_model *res_model = oval_agent_get_results_model(sess);

		/* set up directives */
		struct oval_result_directives *res_direct = oval_result_directives_new(res_model);
		oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE | OVAL_RESULT_FALSE |
						    OVAL_RESULT_UNKNOWN | OVAL_RESULT_NOT_EVALUATED |
						    OVAL_RESULT_ERROR | OVAL_RESULT_NOT_APPLICABLE, true);

		oval_result_directives_set_content(res_direct, OVAL_RESULT_FALSE, OVAL_DIRECTIVE_CONTENT_FULL);
		oval_result_directives_set_content(res_direct, OVAL_RESULT_TRUE, OVAL_DIRECTIVE_CONTENT_FULL);

		
		/* export result model to XML */
		oval_results_model_export(res_model, res_direct, action->f_results);
		oval_result_directives_free(res_direct);
	}
	/* clean up */
	oval_agent_destroy_session(sess);
	oval_definition_model_free(def_model);

	if (usr != NULL) {
		if ((usr->result_false == 0) && (usr->result_unknown == 0)) {
			free(usr);
			return 0;
		} else {
			free(usr);
			return 2;
		}
	} else
		return ret;
}

int app_evaluate_oval_id(const struct oscap_action *action) {
	oval_result_t ret;

	struct oval_definition_model *def_model = oval_definition_model_import(action->f_oval);
	oval_agent_session_t *sess = oval_agent_new_session(def_model, basename(action->f_oval));

	/* Import OVAL definition file */
	if (oscap_err()) {
		fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;
	}

	/* evaluate */
	ret = oval_agent_eval_definition(sess, action->id);

	/* check err */
	if (oscap_err())  {
		fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;
	}

	/* print result */
        if (VERBOSE >= 0) {
		printf("Evalutated definition %s: %s\n", action->id, oval_result_get_text(ret));
                printf("Evaluation: All done.\n");
	}

	/* export results to file */
	if (action->f_results != NULL) {
		/* get result model */
		struct oval_results_model *res_model = oval_agent_get_results_model(sess);

		/* set up directives */
		struct oval_result_directives *res_direct = oval_result_directives_new(res_model);
		oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE | OVAL_RESULT_FALSE |
						    OVAL_RESULT_UNKNOWN | OVAL_RESULT_NOT_EVALUATED |
						    OVAL_RESULT_ERROR | OVAL_RESULT_NOT_APPLICABLE, true);

		oval_result_directives_set_content(res_direct, OVAL_RESULT_FALSE, OVAL_DIRECTIVE_CONTENT_FULL);
		oval_result_directives_set_content(res_direct, OVAL_RESULT_TRUE, OVAL_DIRECTIVE_CONTENT_FULL);		

		// export result model to XML 
		oval_results_model_export(res_model, res_direct, action->f_results);
		oval_result_directives_free(res_direct);
	}

	/* clean up*/
	oval_agent_destroy_session(sess);
	oval_definition_model_free(def_model);

	/* return code */
	if ((ret !=  OVAL_RESULT_FALSE) && (ret != OVAL_RESULT_UNKNOWN)) {
			return 0; /* pass */
	} else {
			return 2; /* fail */
	}
}



enum oval_opt {
    OVAL_OPT_RESULT_FILE = 1,
    OVAL_OPT_FILE_VERSION,
    OVAL_OPT_ID
};

bool getopt_oval(int argc, char **argv, struct oscap_action *action)
{
    VERBOSE = action->verbosity;

	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command-options */
	struct option long_options[] = {
		{ "result-file",  1,                0, OVAL_OPT_RESULT_FILE            },
		{ "id",  	  1,                0, OVAL_OPT_ID                     },
		{ "file-version", 1,                0, OVAL_OPT_FILE_VERSION           },
		{ "definitions",  0, &action->doctype, OSCAP_DOCUMENT_OVAL_DEFINITIONS },
		{ "syschar",      0, &action->doctype, OSCAP_DOCUMENT_OVAL_SYSCHAR     },
		{ "results",      0, &action->doctype, OSCAP_DOCUMENT_OVAL_RESULTS     },
		{ 0, 0, 0, 0 }
	};

	int c;
	while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
		switch (c) {
		case OVAL_OPT_RESULT_FILE: action->f_results = optarg; break;
		case OVAL_OPT_ID: action->id = optarg; break;
		case OVAL_OPT_FILE_VERSION: action->file_version = optarg; break;
        case 0: break;
		default: return false;
		}
	}

	/* We should have OVAL file here */
	if (optind >= argc) {
		/* TODO */
        	return oscap_module_usage(action->module, stderr, "Bad number of parameters. OVAL file needs to be specified!");
	}
	action->url_oval = argv[optind];

	return true;
}

