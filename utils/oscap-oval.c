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

extern int VERBOSE;


struct oval_usr {
	int result_false;
	int result_true;
	int result_invalid;
	int result_unknown;
	int result_neval;
	int result_napp;
};

void print_oval_usage(const char *pname, FILE * out, char *msg)
{
	fprintf(out,
		"Usage: %s [general-options] oval command [command-options] OVAL-DEFINITIONS-FILE \n"
		"(Specify the --help global option for a list of other help options)\n"
		"\n"
		"OVAL-DEFINITIONS-FILE is the OVAL XML file specified either by the full path to the xml file "
		"or an URL from which to download it.\n"
		"\n"
		"Commands:\n"
		"   collect\r\t\t\t\t - Probe the system and gather system characteristics for objects in OVAL Definition file.\n"
		"   eval\r\t\t\t\t - Probe the system and evaluate all definitions from OVAL Definition file\n"
		"   validate-xml\r\t\t\t\t - validate OVAL XML content.\n"
		"\n"
		"Command options:\n"
		"   -h --help\r\t\t\t\t - show this help\n"
		"   --result-file <file>\r\t\t\t\t - Write OVAL Results into file.\n", pname);
	if (msg != NULL)
		fprintf(out, "\n%s\n", msg);
}

static int app_oval_callback(const struct oscap_reporter_message *msg, void *arg)
{

	if (VERBOSE > 0)
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

	struct oval_results_model *res_model = NULL;
	struct oval_usr *usr = NULL;
	int ret = 0;

	struct oval_definition_model *def_model = oval_definition_model_import(action->f_oval);
	oval_agent_session_t *sess = oval_agent_new_session(def_model);

	/* Import OVAL definition file */
	if (oscap_err()) {
		if (VERBOSE >= 0)
			fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;
	}

	res_model = oval_agent_get_results_model(sess);

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

	if (ret == -1) {
		if ((oscap_err()) && (VERBOSE >= 0))
			if (VERBOSE >= 0)
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

	/*=============== PRINT RESULTS =====================*/
	if (action->f_results != NULL) {
		// set up directives 
		struct oval_result_directives *res_direct = oval_result_directives_new(res_model);
		oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE | OVAL_RESULT_FALSE |
						    OVAL_RESULT_UNKNOWN | OVAL_RESULT_NOT_EVALUATED |
						    OVAL_RESULT_ERROR | OVAL_RESULT_NOT_APPLICABLE, true);

		oval_result_directives_set_content(res_direct, OVAL_RESULT_FALSE, OVAL_DIRECTIVE_CONTENT_FULL);
		oval_result_directives_set_content(res_direct, OVAL_RESULT_TRUE, OVAL_DIRECTIVE_CONTENT_FULL);

		// Export result model to XML 
		oval_results_model_export(res_model, res_direct, action->f_results);
		oval_result_directives_free(res_direct);
	}
	/* Clear */
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

int getopt_oval(int argc, char **argv, struct oscap_action *action)
{
	/* Usage: oscap oval command [command-options] */
	if (action == NULL) {
		/* TODO: Problem ? */
		return -1;
	}

	action->std = OSCAP_STD_OVAL;
	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command */
	optind++;
	if (optind >= argc) {
		print_oval_usage("oscap", stderr, "Error: Bad number of parameters. Command and OVAL file required.");
		return -1;
	}
	if (!strcmp(argv[optind], "eval"))
		action->op = OSCAP_OP_EVAL;
	else if (!strcmp(argv[optind], "collect"))
		action->op = OSCAP_OP_COLLECT;
	else if (!strcmp(argv[optind], "validate-xml"))
		action->op = OSCAP_OP_VALIDATE_XML;
	else {
		/* oscap OVAL --help */
		optind--;
	}

	/* Command-options */
	struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"result-file", 1, 0, 0},
		{"file-version", 1, 0, 1},
		{"definitions", 0, 0, 2},
		{"syschar", 0, 0, 3},
		{"results", 0, 0, 4},
		{0, 0, 0, 0}
	};

	int c;
	int getopt_index = 0;	/* index is not neccesary because we know the option from "val" */
	optind++;		/* Increment global variable pointeing to argv array to get next opt */
	while ((c = getopt_long(argc, argv, "+h012", long_options, &getopt_index)) != -1) {
		switch (c) {
		case 'h':	/* XCCDF HELP */
			print_oval_usage("oscap", stdout, NULL);
			return 0;
		case 0:	/* RESULT FILE */
			if (optarg == NULL)
				return -1;
			action->f_results = optarg;
			break;
		case 1:
			if (optarg == NULL)
				return -1;
			action->file_version = optarg;
			break;
		case 2:
			action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;
			break;
		case 3:
			action->doctype = OSCAP_DOCUMENT_OVAL_SYSCHAR;
			break;
		case 4:
			action->doctype = OSCAP_DOCUMENT_OVAL_RESULTS;
			break;
		default:
			fprintf(stderr, "FOUND BAD OPTION %d :: %d :: %s\n", optind, optopt, argv[optind]);
			break;
		}
	}
	if (action->op == OSCAP_OP_UNKNOWN) {
		print_oval_usage("oscap", stderr,
				  "Error: No operation specified. Use \"oscap oval eval/collect OVAL_FILE\"");
		return -1;
	}

	/* We should have OVAL file here */
	if (optind >= argc) {
		/* TODO */
		print_oval_usage("oscap", stderr,
				  "Error: Bad number of parameters. OVAL file needs to be specified !");
		return -1;
	}
	action->url_oval = argv[optind];

	return 1;
}

