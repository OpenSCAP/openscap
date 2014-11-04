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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      Maros Barabas <mbarabas@redhat.com>
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* OVAL & OSCAP common */
#include "oscap_source.h"
#include <oval_probe.h>
#include <oval_agent_api.h>
#include <oval_results.h>
#include <oval_variables.h>
#include <ds_sds_session.h>
#include <assert.h>
#include <limits.h>

#include "oscap-tool.h"
#include "scap_ds.h"

static int app_collect_oval(const struct oscap_action *action);
static int app_evaluate_oval(const struct oscap_action *action);
static int app_oval_validate(const struct oscap_action *action);
static int app_oval_xslt(const struct oscap_action *action);
static int app_oval_list_probes(const struct oscap_action *action);
static int app_analyse_oval(const struct oscap_action *action);

static bool getopt_oval_eval(int argc, char **argv, struct oscap_action *action);
static bool getopt_oval_collect(int argc, char **argv, struct oscap_action *action);
static bool getopt_oval_analyse(int argc, char **argv, struct oscap_action *action);
static bool getopt_oval_list_probes(int argc, char **argv, struct oscap_action *action);
static bool getopt_oval_validate(int argc, char **argv, struct oscap_action *action);
static bool getopt_oval_report(int argc, char **argv, struct oscap_action *action);


static bool valid_inputs(const struct oscap_action *action);

static struct oscap_module* OVAL_SUBMODULES[];
static struct oscap_module* OVAL_GEN_SUBMODULES[];

struct oscap_module OSCAP_OVAL_MODULE = {
    .name = "oval",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "Open Vulnerability and Assessment Language",
    .submodules = OVAL_SUBMODULES
};

static struct oscap_module OVAL_VALIDATE_XML = {
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
    .func = app_oval_validate
};

static struct oscap_module OVAL_VALIDATE = {
    .name = "validate",
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
    .func = app_oval_validate
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
        "   --skip-valid\r\t\t\t\t - Skip validation.\n"
        "   --datastream-id <id> \r\t\t\t\t - ID of the datastream in the collection to use.\n"
        "                        \r\t\t\t\t   (only applicable for source datastreams)\n"
        "   --oval-id <id> \r\t\t\t\t - ID of the OVAL component ref in the datastream to use.\n"
        "                  \r\t\t\t\t   (only applicable for source datastreams)\n"
	"   --probe-root <dir>\r\t\t\t\t - Change the root directory before scanning the system.\n",
    .opt_parser = getopt_oval_eval,
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
    .opt_parser = getopt_oval_collect,
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
    .opt_parser = getopt_oval_analyse,
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
    .opt_parser = getopt_oval_report,
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
    &OVAL_VALIDATE_XML,
    &OVAL_GENERATE,
    &OVAL_LIST_PROBES,
    NULL
};

static int oval_gen_report(const char *infile, const char *outfile)
{
    return app_xslt(infile, "oval-results-report.xsl", outfile, NULL);
}

static int app_oval_callback(const struct oval_result_definition * res_def, void *arg)
{
	oval_result_t result =  oval_result_definition_get_result(res_def);

	printf("Definition %s: %s\n", oval_result_definition_get_id(res_def), oval_result_get_text(result));

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
	struct oscap_source *source = oscap_source_new_from_file(action->f_oval);
	def_model = oval_definition_model_import_source(source);
	oscap_source_free(source);
	if (def_model == NULL) {
		fprintf(stderr, "Failed to import the OVAL Definitions from '%s'.\n", action->f_oval);
		goto cleanup;
	}

	/* bind external variables */
	if(action->f_variables) {
		struct oscap_source *var_source = oscap_source_new_from_file(action->f_variables);
		var_model = oval_variable_model_import_source(var_source);
		oscap_source_free(var_source);
		if (var_model == NULL) {
			fprintf(stderr, "Failed to import the OVAL Variables from '%s'.\n", action->f_variables);
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
			fprintf(stderr, "Object ID(%s) does not exist in '%s'.\n", action->id, action->f_oval);
			goto cleanup;
		}
		printf("Collected: \"%s\" : ", oval_object_get_id(object));
		oval_probe_query_object(pb_sess, object, 0, &syschar);
		sc_flg = oval_syschar_get_flag(syschar);
		printf("%s\n", oval_syschar_collection_flag_get_text(sc_flg));
	}
	else {
	        struct oval_object_iterator *objects = oval_definition_model_get_objects(def_model);
		while (oval_object_iterator_has_more(objects)) {
			object = oval_object_iterator_next(objects);
			printf("Collected: \"%s\" : ", oval_object_get_id(object));
			oval_probe_query_object(pb_sess, object, 0, &syschar);
			sc_flg = oval_syschar_get_flag(syschar);
			printf("%s\n", oval_syschar_collection_flag_get_text(sc_flg));
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
			struct oscap_source *syschar_source = oscap_source_new_from_file(action->f_syschar);
			if (oscap_source_validate(syschar_source, reporter, (void *)action)) {
				oscap_source_free(syschar_source);
				goto cleanup;
			}
			fprintf(stdout, "OVAL System Characteristics are exported correctly.\n");
			oscap_source_free(syschar_source);
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
	oval_agent_session_t		*sess      = NULL;
	int ret = OSCAP_ERROR;

	struct oscap_source *main_source = oscap_source_new_from_file(action->f_oval);
	struct ds_sds_session *sds_session = NULL;
	struct oscap_source *oval_source = NULL;

	if (action->probe_root) {
		if (setenv("OSCAP_PROBE_ROOT", action->probe_root, 1) != 0) {
			fprintf(stderr, "Failed to set the OSCAP_PROBE_ROOT environment variable.\n");
			goto cleanup;
		}
	}

	/* validate inputs */
	if (action->validate) {
		if (!valid_inputs(action)) {
			goto cleanup;
		}
	}

	if (oscap_source_get_scap_type(main_source) == OSCAP_DOCUMENT_SDS)
	{
		sds_session = ds_sds_session_new_from_source(main_source);
		if (sds_session == NULL) {
			goto cleanup;
		}

		ds_sds_session_set_datastream_id(sds_session, action->f_datastream_id);
		if (ds_sds_session_register_component_with_dependencies(sds_session, "checks", action->f_oval_id, "oval.xml") != 0) {
			goto cleanup;
		}
		oval_source = ds_sds_session_get_component_by_href(sds_session, "oval.xml");
		if (oval_source == NULL) {
			fprintf(stderr, "Internal error: OVAL file was not found in Source DataStream session cache!");
			goto cleanup;
		}
	}
	else
	{
		oval_source = main_source;
	}

	/* import OVAL Definitions */
	def_model = oval_definition_model_import_source(oval_source);
	if (def_model == NULL) {
		fprintf(stderr, "Failed to import the OVAL Definitions from '%s'.\n", oscap_source_readable_origin(oval_source));
		goto cleanup;
	}

	/* bind external variables */
	if(action->f_variables) {
		struct oscap_source *var_source = oscap_source_new_from_file(action->f_variables);
		var_model = oval_variable_model_import_source(var_source);
		oscap_source_free(var_source);
		if (var_model == NULL) {
			fprintf(stderr, "Failed to import the OVAL Variables from '%s'.\n", action->f_variables);
			goto cleanup;
		}

		if (oval_definition_model_bind_variable_model(def_model, var_model)) {
			fprintf(stderr, "Failed to bind Variables to Definitions\n");
			goto cleanup;
		}
	}

	char *path_clone = strdup(oscap_source_readable_origin(oval_source));
	sess = oval_agent_new_session(def_model, basename(path_clone));
	free(path_clone);

	if (sess == NULL) {
		fprintf(stderr, "Failed to create new agent session.\n");
		goto cleanup;
	}

	/* set product name */
	oval_agent_set_product_name(sess, OSCAP_PRODUCTNAME);

	/* Evaluation */
	if (action->id) {
		oval_agent_eval_definition(sess, action->id);
		oval_result_t result = OVAL_RESULT_NOT_EVALUATED;
		oval_agent_get_definition_result(sess, action->id, &result);
		printf("Definition %s: %s\n", action->id, oval_result_get_text(result));
	} else
		oval_agent_eval_system(sess, app_oval_callback, NULL);

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
			struct oscap_source *dir_source = oscap_source_new_from_file(action->f_directives);
			oval_directives_model_import_source(dir_model, dir_source);
			oscap_source_free(dir_source);
		}

		/* export result model to XML */
		oval_results_model_export(res_model, dir_model, action->f_results);

		const char* full_validation = getenv("OSCAP_FULL_VALIDATION");

		/* validate OVAL Results */
		if (action->validate && full_validation) {
			struct oscap_source *result_source = oscap_source_new_from_file(action->f_results);
			if (oscap_source_validate(result_source, reporter, (void *) action)) {
				oscap_source_free(result_source);
				goto cleanup;
			}
			fprintf(stdout, "OVAL Results are exported correctly.\n");
			oscap_source_free(result_source);
		}

		/* generate report */
	        if (action->f_report != NULL)
        		oval_gen_report(action->f_results, action->f_report);
	}

	ret = OSCAP_OK;

	/* clean up */
cleanup:
	oscap_print_error();

	ds_sds_session_free(sds_session);
	oscap_source_free(main_source);
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
	struct oscap_source *source = oscap_source_new_from_file(action->f_oval);
	def_model = oval_definition_model_import_source(source);
	oscap_source_free(source);
        if (def_model == NULL) {
                fprintf(stderr, "Failed to import the OVAL Definitions from '%s'.\n", action->f_oval);
		goto cleanup;
        }

	/* bind external variables */
	if(action->f_variables) {
		struct oscap_source *var_source = oscap_source_new_from_file(action->f_variables);
		var_model = oval_variable_model_import_source(var_source);
		oscap_source_free(var_source);
		if (var_model == NULL) {
			fprintf(stderr, "Failed to import the OVAL Variables from '%s'.\n", action->f_variables);
			goto cleanup;
		}

		if (oval_definition_model_bind_variable_model(def_model, var_model)) {
			fprintf(stderr, "Failed to bind Variables to Definitions\n");
			goto cleanup;
		}
	}

	/* load system characteristics */
	sys_model = oval_syschar_model_new(def_model);
	source = oscap_source_new_from_file(action->f_syschar);
	if (oval_syschar_model_import_source(sys_model, source) ==  -1 ) {
                fprintf(stderr, "Failed to import the System Characteristics from '%s'.\n", action->f_syschar);
		oscap_source_free(source);
                goto cleanup;
        }
	oscap_source_free(source);

	/* evaluate */
	sys_models[0] = sys_model;
	sys_models[1] = NULL;
	res_model = oval_results_model_new(def_model, sys_models);

	/* set product name */
        generator = oval_results_model_get_generator(res_model);
        oval_generator_set_product_name(generator, OSCAP_PRODUCTNAME);
	oval_generator_set_product_version(generator, oscap_get_version());

	oval_results_model_eval(res_model);

	/* export results */
	if (action->f_results != NULL) {
		/* import directives */
		if (action->f_directives != NULL) {
			dir_model = oval_directives_model_new();
			struct oscap_source *dir_source = oscap_source_new_from_file(action->f_directives);
			oval_directives_model_import_source(dir_model, dir_source);
			oscap_source_free(dir_source);
		}

		/* export result model to XML */
		oval_results_model_export(res_model, dir_model, action->f_results);

		const char* full_validation = getenv("OSCAP_FULL_VALIDATION");

		/* validate OVAL Results */
		if (action->validate && full_validation) {
			struct oscap_source *result_source = oscap_source_new_from_file(action->f_results);
			if (oscap_source_validate(result_source, reporter, (void *) action)) {
				oscap_source_free(result_source);
				goto cleanup;
			}
			fprintf(stdout, "OVAL Results are exported correctly.\n");
			oscap_source_free(result_source);
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
    OVAL_OPT_DATASTREAM_ID,
    OVAL_OPT_OVAL_ID,
    OVAL_OPT_OUTPUT = 'o',
    OVAL_OPT_PROBE_ROOT
};

bool getopt_oval_eval(int argc, char **argv, struct oscap_action *action)
{
	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;
	action->probe_root = NULL;

	/* Command-options */
	struct option long_options[] = {
		{ "results", 	required_argument, NULL, OVAL_OPT_RESULT_FILE  },
		{ "report",  	required_argument, NULL, OVAL_OPT_REPORT_FILE  },
		{ "id",        	required_argument, NULL, OVAL_OPT_ID           },
		{ "variables",	required_argument, NULL, OVAL_OPT_VARIABLES    },
		{ "directives",	required_argument, NULL, OVAL_OPT_DIRECTIVES   },
		{ "datastream-id",required_argument, NULL, OVAL_OPT_DATASTREAM_ID},
		{ "oval-id",    required_argument, NULL, OVAL_OPT_OVAL_ID},
		{ "skip-valid",	no_argument, &action->validate, 0 },
		{ "probe-root", required_argument, NULL, OVAL_OPT_PROBE_ROOT},
		{ 0, 0, 0, 0 }
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:", long_options, NULL)) != -1) {
		switch (c) {
		case OVAL_OPT_RESULT_FILE: action->f_results = optarg; break;
		case OVAL_OPT_REPORT_FILE: action->f_report  = optarg; break;
		case OVAL_OPT_ID: action->id = optarg; break;
		case OVAL_OPT_VARIABLES: action->f_variables = optarg; break;
		case OVAL_OPT_DIRECTIVES: action->f_directives = optarg; break;
		case OVAL_OPT_DATASTREAM_ID: action->f_datastream_id = optarg;	break;
		case OVAL_OPT_OVAL_ID: action->f_oval_id = optarg;	break;
		case OVAL_OPT_PROBE_ROOT: action->probe_root = optarg; break;
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	/* We should have Definitions file here */
	if (optind >= argc)
		return oscap_module_usage(action->module, stderr, "Definitions file is not specified!");
	action->f_oval = argv[optind];

	return true;
}

bool getopt_oval_collect(int argc, char **argv, struct oscap_action *action)
{
	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command-options */
	struct option long_options[] = {
		{ "id",        	required_argument, NULL, OVAL_OPT_ID           },
		{ "variables",	required_argument, NULL, OVAL_OPT_VARIABLES    },
		{ "syschar",	required_argument, NULL, OVAL_OPT_SYSCHAR      },
		{ "skip-valid",	no_argument, &action->validate, 0 },
		{ 0, 0, 0, 0 }
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:", long_options, NULL)) != -1) {
		switch (c) {
		case OVAL_OPT_ID: action->id = optarg; break;
		case OVAL_OPT_VARIABLES: action->f_variables = optarg; break;
		case OVAL_OPT_SYSCHAR: action->f_syschar = optarg; break;
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	/* We should have Definitions file here */
	if (optind >= argc)
		return oscap_module_usage(action->module, stderr, "Definitions file is not specified!");
	action->f_oval = argv[optind];

	return true;
}

bool getopt_oval_analyse(int argc, char **argv, struct oscap_action *action)
{
	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command-options */
	struct option long_options[] = {
		{ "results", 	required_argument, NULL, OVAL_OPT_RESULT_FILE  },
		{ "variables",	required_argument, NULL, OVAL_OPT_VARIABLES    },
		{ "directives",	required_argument, NULL, OVAL_OPT_DIRECTIVES   },
		{ "skip-valid",	no_argument, &action->validate, 0 },
		{ 0, 0, 0, 0 }
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:", long_options, NULL)) != -1) {
		switch (c) {
		case OVAL_OPT_RESULT_FILE: action->f_results = optarg; break;
		case OVAL_OPT_VARIABLES: action->f_variables = optarg; break;
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

bool getopt_oval_report(int argc, char **argv, struct oscap_action *action)
{
	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command-options */
	struct option long_options[] = {
		{ "output",    	required_argument, NULL, OVAL_OPT_OUTPUT       },
		{ 0, 0, 0, 0 }
	};

	/*
	 * it is not nice that we use action->f_results for output and
	 * action->f_oval as input here.
	 */
	int c;
	while ((c = getopt_long(argc, argv, "o:", long_options, NULL)) != -1) {
		switch (c) {
		case OVAL_OPT_OUTPUT: action->f_results = optarg; break;
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	/* We should have oval results file here */
	if (optind >= argc)
		return oscap_module_usage(action->module, stderr, "Definitions file is not specified!");
	action->f_oval = argv[optind];

	return true;
}

bool getopt_oval_list_probes(int argc, char **argv, struct oscap_action *action)
{
#define PROBE_LIST_STATIC  0
#define PROBE_LIST_DYNAMIC 1

        int list_type = PROBE_LIST_DYNAMIC;
	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;
	action->verbosity = 0;

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
	/* we assume 0 is unknown */
	action->doctype = 0;

	/* Command-options */
	struct option long_options[] = {
        // flags
		{ "definitions",	no_argument, &action->doctype, OSCAP_DOCUMENT_OVAL_DEFINITIONS },
		{ "variables",		no_argument, &action->doctype, OSCAP_DOCUMENT_OVAL_VARIABLES   },
		{ "syschar",		no_argument, &action->doctype, OSCAP_DOCUMENT_OVAL_SYSCHAR     },
		{ "results",		no_argument, &action->doctype, OSCAP_DOCUMENT_OVAL_RESULTS     },
		{ "directives",		no_argument, &action->doctype, OSCAP_DOCUMENT_OVAL_DIRECTIVES  },
		// force schematron validation
		{ "schematron",		no_argument, &action->schematron, 1 },
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

	/* we should have OVAL content here */
	if (optind >= argc)
		return oscap_module_usage(action->module, stderr, "OVAL file needs to be specified!");
	action->f_oval = argv[optind];

	return true;
}

static bool valid_inputs(const struct oscap_action *action) {
        bool result = true;

	/* validate SDS or OVAL Definitions & Variables & Syschars,
	   depending on the data */
	struct oscap_source *definitions_source = oscap_source_new_from_file(action->f_oval);
	if (oscap_source_get_scap_type(definitions_source) != OSCAP_DOCUMENT_OVAL_DEFINITIONS &&
			oscap_source_get_scap_type(definitions_source) != OSCAP_DOCUMENT_SDS) {
		fprintf(stderr, "Type mismatch: %s. Expecting OVAL Definition or Source DataStream, but found %s.\n",
			action->f_oval, oscap_document_type_to_string(oscap_source_get_scap_type(definitions_source)));
		result = false;
	}
	if (oscap_source_validate(definitions_source, reporter, (void *) action)) {
		result = false;
	}
	oscap_source_free(definitions_source);

	if (action->f_variables) {
		struct oscap_source *variables_source = oscap_source_new_from_file(action->f_variables);
		if (oscap_source_get_scap_type(variables_source) != OSCAP_DOCUMENT_OVAL_VARIABLES) {
			fprintf(stderr, "Type mismatch: %s. Expecting OVAL Variables, but found %s.\n",
				action->f_variables, oscap_document_type_to_string(oscap_source_get_scap_type(variables_source)));
			result = false;
		}
		if (oscap_source_validate(variables_source, reporter, (void *) action)) {
			result = false;
		}
		oscap_source_free(variables_source);
	}

	if (action->f_directives) {
		struct oscap_source *directives_source = oscap_source_new_from_file(action->f_directives);
		if (oscap_source_get_scap_type(directives_source) != OSCAP_DOCUMENT_OVAL_DIRECTIVES) {
			fprintf(stderr, "Type mismatch: %s. Expecting OVAL Directives, but found %s.\n",
				action->f_directives, oscap_document_type_to_string(oscap_source_get_scap_type(directives_source)));
			result = false;
		}
		if (oscap_source_validate(directives_source, reporter, (void *) action)) {
			result = false;
		}
		oscap_source_free(directives_source);
	}

	if (action->module == &OVAL_ANALYSE && action->f_syschar) {
		struct oscap_source *syschar_source = oscap_source_new_from_file(action->f_syschar);
		if (oscap_source_get_scap_type(syschar_source) != OSCAP_DOCUMENT_OVAL_SYSCHAR) {
			fprintf(stderr, "Type mismatch: %s. Expecting OVAL System Characteristic, but found %s.\n",
				action->f_syschar, oscap_document_type_to_string(oscap_source_get_scap_type(syschar_source)));
			result = false;
		}
		if (oscap_source_validate(syschar_source, reporter, (void *) action)) {
			result = false;
		}
		oscap_source_free(syschar_source);
	}

	return result;
}

static int app_oval_validate(const struct oscap_action *action) {
	int ret;
	int result = OSCAP_ERROR;

	struct oscap_source *source = oscap_source_new_from_file(action->f_oval);
	ret = oscap_source_validate(source, reporter, (void *) action);
	if (ret == -1) {
		result = OSCAP_ERROR;
		goto cleanup;
	}
	else {
		result = ret == 1 ? OSCAP_FAIL : OSCAP_OK;
	}

	/* schematron-based validation requested
	   We can only do schematron validation if the file isn't a source datastream
	*/
	if (action->schematron && oscap_source_get_scap_type(source) != OSCAP_DOCUMENT_SDS) {
		ret = oscap_source_validate_schematron(source, NULL);
		if (ret==-1) {
			result=OSCAP_ERROR;
		}
		else if (ret>0) {
			result=OSCAP_FAIL;
		}
	}

cleanup:
	oscap_source_free(source);
	if (oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	return result;

}

