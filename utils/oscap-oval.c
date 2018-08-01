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
#if defined(OVAL_PROBES_ENABLED)
# include <oval_probe.h>
# include "probe-table.h"
#endif
#include <oval_agent_api.h>
#include <oval_session.h>
#include <oval_results.h>
#include <oval_variables.h>
#include <ds_sds_session.h>
#include <assert.h>
#include <limits.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include "oscap-tool.h"
#include "scap_ds.h"
#include <oscap_debug.h>

#if defined(OVAL_PROBES_ENABLED)
static int app_collect_oval(const struct oscap_action *action);
static int app_evaluate_oval(const struct oscap_action *action);
#endif
static int app_oval_validate(const struct oscap_action *action);
static int app_oval_xslt(const struct oscap_action *action);
static int app_analyse_oval(const struct oscap_action *action);

#if defined(OVAL_PROBES_ENABLED)
static bool getopt_oval_eval(int argc, char **argv, struct oscap_action *action);
static bool getopt_oval_collect(int argc, char **argv, struct oscap_action *action);
#endif
static bool getopt_oval_analyse(int argc, char **argv, struct oscap_action *action);
static bool getopt_oval_validate(int argc, char **argv, struct oscap_action *action);
static bool getopt_oval_report(int argc, char **argv, struct oscap_action *action);


static bool valid_inputs(const struct oscap_action *action);

#define OVAL_SUBMODULES_NUM	7
#define OVAL_GEN_SUBMODULES_NUM 2 /* See actual OVAL_GEN_SUBMODULES and
				OVAL_SUBMODULES arrays initialization below. */
static struct oscap_module* OVAL_SUBMODULES[OVAL_SUBMODULES_NUM];
static struct oscap_module* OVAL_GEN_SUBMODULES[OVAL_GEN_SUBMODULES_NUM];

struct oscap_module OSCAP_OVAL_MODULE = {
    .name = "oval",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "Open Vulnerability and Assessment Language",
    .submodules = OVAL_SUBMODULES
};

static struct oscap_module OVAL_VALIDATE = {
    .name = "validate",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Validate OVAL XML content",
    .usage = "[options] oval-file.xml",
    .help =
	"Options:\n"
	"   --definitions                 - Validate OVAL Definitions\n"
	"   --variables                   - Validate external OVAL Variables\n"
	"   --syschar                     - Validate OVAL System Characteristics\n"
	"   --results                     - Validate OVAL Results\n"
	"   --schematron                  - Use schematron-based validation in addition to XML Schema\n",
    .opt_parser = getopt_oval_validate,
    .func = app_oval_validate
};

#if defined(OVAL_PROBES_ENABLED)
static struct oscap_module OVAL_EVAL = {
    .name = "eval",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Probe the system and evaluate definitions from OVAL Definition file",
    .usage = "[options] oval-definitions.xml",
    .help =
	"Options:\n"
	"   --id <definition-id>          - ID of the definition we want to evaluate.\n"
	"   --variables <file>            - Provide external variables expected by OVAL Definitions.\n"
	"   --directives <file>           - Use OVAL Directives content to specify desired results content.\n"
	"   --without-syschar             - Don't provide system characteristic in result file.\n"
	"   --results <file>              - Write OVAL Results into file.\n"
	"   --report <file>               - Create human readable (HTML) report from OVAL Results.\n"
	"   --skip-valid                  - Skip validation.\n"
	"   --datastream-id <id>          - ID of the datastream in the collection to use.\n"
	"                                   (only applicable for source datastreams)\n"
	"   --oval-id <id>                - ID of the OVAL component ref in the datastream to use.\n"
	"                                   (only applicable for source datastreams)\n"
	"   --verbose <verbosity_level>   - Turn on verbose mode at specified verbosity level.\n"
	"   --verbose-log-file <file>     - Write verbose information into file.\n",
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
	"   --id <object>                 - Collect system characteristics ONLY for specified OVAL Object.\n"
	"   --syschar <file>              - Write OVAL System Characteristic into file.\n"
	"   --variables <file>            - Provide external variables expected by OVAL Definitions.\n"
	"   --skip-valid                  - Skip validation.\n"
	"   --verbose <verbosity_level>   - Turn on verbose mode at specified verbosity level.\n"
	"   --verbose-log-file <file>     - Write verbose information into file.\n",
    .opt_parser = getopt_oval_collect,
    .func = app_collect_oval
};
#endif /* OVAL_PROBES_ENABLED */

static struct oscap_module OVAL_ANALYSE = {
    .name = "analyse",
    .parent = &OSCAP_OVAL_MODULE,
    .summary = "Evaluate provided system characteristics file",
    .usage = "[options] --results FILE oval-definitions.xml system-characteristics.xml" ,
    .help =
	"Options:\n"
	"   --variables <file>            - Provide external variables expected by OVAL Definitions.\n"
	"   --directives <file>           - Use OVAL Directives content to specify desired results content.\n"
	"   --skip-valid                  - Skip validation.\n"
	"   --verbose <verbosity_level>   - Turn on verbose mode at specified verbosity level.\n"
	"   --verbose-log-file <file>     - Write verbose information into file.\n",
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
	"   --output <file>               - Write the HTML into file.",
    .opt_parser = getopt_oval_report,
    .user = "oval-results-report.xsl",
    .func = app_oval_xslt
};

static struct oscap_module* OVAL_GEN_SUBMODULES[OVAL_GEN_SUBMODULES_NUM] = {
    &OVAL_REPORT,
    NULL
};

static struct oscap_module* OVAL_SUBMODULES[OVAL_SUBMODULES_NUM] = {
#if defined(OVAL_PROBES_ENABLED)
    &OVAL_COLLECT,
    &OVAL_EVAL,
#endif
    &OVAL_ANALYSE,
    &OVAL_VALIDATE,
    &OVAL_GENERATE,
    NULL
};

static int app_oval_callback(const struct oval_result_definition * res_def, void *arg)
{
	oval_result_t result =  oval_result_definition_get_result(res_def);

	printf("Definition %s: %s\n", oval_result_definition_get_id(res_def), oval_result_get_text(result));

	return 0;
}

#if defined(OVAL_PROBES_ENABLED)
int app_collect_oval(const struct oscap_action *action)
{
	struct oval_definition_model	*def_model = NULL;
	struct oval_variable_model	*var_model = NULL;
	struct oval_syschar_model	*sys_model = NULL;
	struct oval_sysinfo		*sysinfo   = NULL;
	struct oval_probe_session	*pb_sess   = NULL;
	struct oval_generator		*generator = NULL;
	int ret = OSCAP_ERROR;

	/* Turn on verbosity */
	if (!oscap_set_verbose(action->verbosity_level, action->f_verbose_log, false)) {
		goto cleanup;
	}

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
	struct oval_session *session = NULL;
	oval_result_t eval_result;
	int ret = OSCAP_ERROR;

	/* Turn on verbosity */
	if (!oscap_set_verbose(action->verbosity_level, action->f_verbose_log, false)) {
		goto cleanup;
	}

	/* create a new OVAL session */
	if ((session = oval_session_new(action->f_oval)) == NULL) {
		oscap_print_error();
		return ret;
	}

	/* set validation level */
	oval_session_set_validation(session, action->validate, getenv("OSCAP_FULL_VALIDATION") != NULL);

	/* set source DS related IDs */
	oval_session_set_datastream_id(session, action->f_datastream_id);
	oval_session_set_component_id(session, action->f_oval_id);

	/* set reporter function for XML validation of inputs and outputs */
	oval_session_set_xml_reporter(session, reporter);

	/* set OVAL Variables */
	oval_session_set_variables(session, action->f_variables);

	oval_session_set_remote_resources(session, action->remote_resources, download_reporting_callback);
	/* load all necesary OVAL Definitions and bind OVAL Variables if provided */
	if ((oval_session_load(session)) != 0)
		goto cleanup;

	/* evaluation */
	if (action->id) {
		if ((oval_session_evaluate_id(session, action->id, &eval_result)) != 0)
			goto cleanup;
		printf("Definition %s: %s\n", action->id, oval_result_get_text(eval_result));
	}
	else {
		if ((oval_session_evaluate(session, app_oval_callback, NULL)) != 0)
			goto cleanup;
	}

	printf("Evaluation done.\n");

	oval_session_set_directives(session, action->f_directives);
	oval_session_set_results_export(session, action->f_results);
	oval_session_set_report_export(session, action->f_report);
	oval_session_set_export_system_characteristics(session, !action->without_sys_chars);
	if (oval_session_export(session) != 0)
		goto cleanup;

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();
	oval_session_free(session);
	return ret;
}
#endif /* OVAL_PROBES_ENABLED */

static int app_analyse_oval(const struct oscap_action *action) {
	struct oval_definition_model	*def_model = NULL;
	struct oval_syschar_model	*sys_model = NULL;
	struct oval_results_model	*res_model = NULL;
	struct oval_variable_model	*var_model = NULL;
	struct oval_directives_model	*dir_model = NULL;
 	struct oval_syschar_model	*sys_models[2];
	struct oval_generator		*generator = NULL;
	int ret = OSCAP_ERROR;

	/* Turn on verbosity */
	if (!oscap_set_verbose(action->verbosity_level, action->f_verbose_log, false)) {
		goto cleanup;
	}

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
	OVAL_OPT_VERBOSE,
	OVAL_OPT_VERBOSE_LOG_FILE
};

#if defined(OVAL_PROBES_ENABLED)
bool getopt_oval_eval(int argc, char **argv, struct oscap_action *action)
{
	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command-options */
	struct option long_options[] = {
		{ "results", 	required_argument, NULL, OVAL_OPT_RESULT_FILE  },
		{ "report",  	required_argument, NULL, OVAL_OPT_REPORT_FILE  },
		{ "id",        	required_argument, NULL, OVAL_OPT_ID           },
		{ "variables",	required_argument, NULL, OVAL_OPT_VARIABLES    },
		{ "directives",	required_argument, NULL, OVAL_OPT_DIRECTIVES   },
		{ "without-syschar",	no_argument, &action->without_sys_chars, 1},
		{ "datastream-id",required_argument, NULL, OVAL_OPT_DATASTREAM_ID},
		{ "oval-id",    required_argument, NULL, OVAL_OPT_OVAL_ID},
		{ "skip-valid",	no_argument, &action->validate, 0 },
		{ "verbose", required_argument, NULL, OVAL_OPT_VERBOSE },
		{ "verbose-log-file", required_argument, NULL, OVAL_OPT_VERBOSE_LOG_FILE },
		{ "fetch-remote-resources", no_argument, &action->remote_resources, 1},
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
		case OVAL_OPT_VERBOSE:
			action->verbosity_level = optarg;
			break;
		case OVAL_OPT_VERBOSE_LOG_FILE:
			action->f_verbose_log = optarg;
			break;
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}
	if (!check_verbose_options(action)) {
		return false;
	}

	/* We should have Definitions file here */
	if (optind >= argc)
		return oscap_module_usage(action->module, stderr, "Definitions file is not specified!");
	action->f_oval = argv[optind];

	return true;
}
#endif /* OVAL_PROBES_ENABLED */

#if defined(OVAL_PROBES_ENABLED)
bool getopt_oval_collect(int argc, char **argv, struct oscap_action *action)
{
	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command-options */
	struct option long_options[] = {
		{ "id",        	required_argument, NULL, OVAL_OPT_ID           },
		{ "variables",	required_argument, NULL, OVAL_OPT_VARIABLES    },
		{ "syschar",	required_argument, NULL, OVAL_OPT_SYSCHAR      },
		{ "skip-valid",	no_argument, &action->validate, 0 },
		{ "verbose", required_argument, NULL, OVAL_OPT_VERBOSE },
		{ "verbose-log-file", required_argument, NULL, OVAL_OPT_VERBOSE_LOG_FILE },
		{ 0, 0, 0, 0 }
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:", long_options, NULL)) != -1) {
		switch (c) {
		case OVAL_OPT_ID: action->id = optarg; break;
		case OVAL_OPT_VARIABLES: action->f_variables = optarg; break;
		case OVAL_OPT_SYSCHAR: action->f_syschar = optarg; break;
		case OVAL_OPT_VERBOSE:
			action->verbosity_level = optarg;
			break;
		case OVAL_OPT_VERBOSE_LOG_FILE:
			action->f_verbose_log = optarg;
			break;
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}
	if (!check_verbose_options(action)) {
		return false;
	}

	/* We should have Definitions file here */
	if (optind >= argc)
		return oscap_module_usage(action->module, stderr, "Definitions file is not specified!");
	action->f_oval = argv[optind];

	return true;
}
#endif /* OVAL_PROBES_ENABLED */

bool getopt_oval_analyse(int argc, char **argv, struct oscap_action *action)
{
	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command-options */
	struct option long_options[] = {
		{ "results", 	required_argument, NULL, OVAL_OPT_RESULT_FILE  },
		{ "variables",	required_argument, NULL, OVAL_OPT_VARIABLES    },
		{ "directives",	required_argument, NULL, OVAL_OPT_DIRECTIVES   },
		{ "skip-valid",	no_argument, &action->validate, 0 },
		{ "verbose", required_argument, NULL, OVAL_OPT_VERBOSE },
		{ "verbose-log-file", required_argument, NULL, OVAL_OPT_VERBOSE_LOG_FILE },
		{ 0, 0, 0, 0 }
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:", long_options, NULL)) != -1) {
		switch (c) {
		case OVAL_OPT_RESULT_FILE: action->f_results = optarg; break;
		case OVAL_OPT_VARIABLES: action->f_variables = optarg; break;
		case OVAL_OPT_DIRECTIVES: action->f_directives = optarg; break;
		case OVAL_OPT_VERBOSE:
			action->verbosity_level = optarg;
			break;
		case OVAL_OPT_VERBOSE_LOG_FILE:
			action->f_verbose_log = optarg;
			break;
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}
	if (!check_verbose_options(action)) {
		return false;
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

