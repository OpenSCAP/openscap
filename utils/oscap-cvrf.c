/*
 * Copyright 2017 Red Hat Inc., Durham, North Carolina.
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
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <math.h>

#include <oscap.h>
#include <oscap_error.h>
#include <cvrf.h>
#include <oscap_source.h>

#include "oscap-tool.h"

static bool getopt_cvrf(int argc, char **argv, struct oscap_action *action);
static int app_cvrf_evaluate(const struct oscap_action *action);
static int app_cvrf_export(const struct oscap_action *action);
static int app_cvrf_validate(const struct oscap_action *action);

static struct oscap_module* CVRF_SUBMODULES[];

struct oscap_module OSCAP_CVRF_MODULE = {
	.name = "cvrf",
	.parent = &OSCAP_ROOT_MODULE,
	.summary = "Common Vulnerability Reporting Framework",
	.submodules = CVRF_SUBMODULES
};

static struct oscap_module CVRF_EVALUATE_MODULE = {
	.name = "eval",
	.parent = &OSCAP_CVRF_MODULE,
	.summary = "Evaluate system for vulnerabilities",
	.usage = "[options] <cvrf file.xml>",
	.opt_parser = getopt_cvrf,
	.func = app_cvrf_evaluate,
	.help = "Options:\n"
		"   --index                       - Use index file to evaluate a directory of CVRF files.\n"
		"   --results                     - Filename to which evaluation results will be saved.\n",
};

static struct oscap_module CVRF_EXPORT_MODULE = {
	.name = "export",
	.parent = &OSCAP_CVRF_MODULE,
	.summary = "Download and export CVRF file to system",
	.usage = "[options] <cvrf file.xml>",
	.opt_parser = getopt_cvrf,
	.func = app_cvrf_export,
	.help = "Options:\n"
		"   --index                       - Use index file to export a directory of CVRF files \n"
		"   --output                      - Filename to which exported CVRF document will be saved.\n",
};

static struct oscap_module CVRF_VALIDATE_MODULE = {
	.name = "validate",
	.parent = &OSCAP_CVRF_MODULE,
	.summary = "Ensure that provided file input follows CVRF format correctly",
	.usage = "[options] <cvrf file.xml>",
	.opt_parser = getopt_cvrf,
	.func = app_cvrf_validate,
	.help = "Options:\n"
		"   --index                       - Use index file to validate a directory of CVRF files \n",
};

static struct oscap_module* CVRF_SUBMODULES[] = {
	&CVRF_EVALUATE_MODULE,
	&CVRF_EXPORT_MODULE,
	&CVRF_VALIDATE_MODULE,
	NULL
};


static int app_cvrf_evaluate(const struct oscap_action *action) {
	int result = OSCAP_OK;
	// Temporary hardcoded CPE until CPE name can be found without input by CVRF functions
	// themselves
	const char *os_name = "Red Hat Enterprise Linux Desktop Supplementary (v. 6)";
	struct oscap_source *import_source = oscap_source_new_from_file(action->cvrf_action->f_cvrf);
	struct oscap_source *export_source = cvrf_model_get_results_source(import_source, os_name);
	if (export_source == NULL)
		return -1;

	if (oscap_source_save_as(export_source, action->cvrf_action->f_results) == -1) {
		result = OSCAP_ERROR;
		goto cleanup;
	}

	cleanup:
		if (oscap_err())
			fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	oscap_source_free(export_source);
	free(action->cvrf_action);
	return result;
}

static int app_cvrf_export(const struct oscap_action *action) {
	struct oscap_source *import_source = oscap_source_new_from_file(action->cvrf_action->f_cvrf);
	if (import_source == NULL)
		return OSCAP_ERROR;

	int result = OSCAP_OK;
	if (action->cvrf_action->index == 1) {
		struct cvrf_index *index = cvrf_index_import(import_source);
		if (index == NULL) {
			result = OSCAP_ERROR;
			goto cleanup;
		}
		struct oscap_source *export_source = cvrf_index_get_export_source(index);
		if (oscap_source_save_as(export_source, action->cvrf_action->f_output) != 0)
			result = OSCAP_ERROR;
		oscap_source_free(export_source);
		cvrf_index_free(index);
	} else {
		struct cvrf_model *model = cvrf_model_import(import_source);
		if(model == NULL) {
			result = OSCAP_ERROR;
			goto cleanup;
		}
		struct oscap_source *export_source = cvrf_model_get_export_source(model);
		if (oscap_source_save_as(export_source, action->cvrf_action->f_output) != 0)
			result = OSCAP_ERROR;
		oscap_source_free(export_source);
		cvrf_model_free(model);
	}

	cleanup:
		if (oscap_err())
			fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	/* TODO: Refactor, cvrf_index_parse_xml (called by oscap_source_new_from_file) frees its argument as an unexpected side-effect.
	 * oscap_source_free(import_source);
	 */
	free(action->cvrf_action);
	return result;
}

static int app_cvrf_validate(const struct oscap_action *action) {
	int result;
	struct oscap_source *source = oscap_source_new_from_file(action->cvrf_action->f_cvrf);
	int ret = oscap_source_validate(source, reporter, (void *) action);

	if (ret==-1) {
		result=OSCAP_ERROR;
	} else if (ret==1) {
		result=OSCAP_FAIL;
	} else {
		result=OSCAP_OK;
	}

	oscap_source_free(source);
	oscap_print_error();
	return result;
}

// There will likely be more options needed in the future; this is
// just a basic set up for the command line usage
enum cvrf_opt {
	CVRF_OPT_INDEX,
	CVRF_OPT_RESULT_FILE,
	CVRF_OPT_OUTPUT_FILE,
};

bool getopt_cvrf(int argc, char **argv, struct oscap_action *action) {
	action->doctype = OSCAP_DOCUMENT_CVRF_FEED;
	action->cvrf_action = malloc(sizeof(struct cvrf_action));
	struct cvrf_action *cvrf_action = action->cvrf_action;

	static const struct option long_options[] = {
		{"index", 0, NULL, CVRF_OPT_INDEX},
		{"results", 1, NULL, CVRF_OPT_RESULT_FILE},
		{"output", 1, NULL, CVRF_OPT_OUTPUT_FILE},
		{0, 0, 0, 0}
	};

	int c;
	while ((c = getopt_long(argc, argv, "+", long_options, NULL)) != -1) {
		switch (c) {
			case CVRF_OPT_INDEX:
				cvrf_action->index = 1;
				break;
			case CVRF_OPT_RESULT_FILE:
				cvrf_action->f_results = optarg;
				break;
			case CVRF_OPT_OUTPUT_FILE:
				cvrf_action->f_output = optarg;
				break;
			default:
				return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	if (optind >= argc)
		return oscap_module_usage(action->module, stderr, "CVRF file needs to be specified!\n");
	cvrf_action->f_cvrf = argv[optind];
	return true;
}
