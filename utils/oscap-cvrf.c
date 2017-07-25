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

static struct oscap_module* CVRF_SUBMODULES[];

struct oscap_module OSCAP_CVRF_MODULE = {
	.name = "cvrf",
	.parent = &OSCAP_ROOT_MODULE,
	.summary = "Common Vulnerability Reporting Framework",
	.submodules = CVRF_SUBMODULES
};

static struct oscap_module CVRF_EVALUATE_MODULE = {
	.name = "evaluate",
	.parent = &OSCAP_CVRF_MODULE,
	.summary = "Evaluate system for vulnerabilities",
	.usage = "[options] <cvrf file> results-file.xml",
	.opt_parser = getopt_cvrf,
	.func = app_cvrf_evaluate,
	.help = "Options:\n"
		"   --index\r\t\t\t\t - Use index file to evaluate a directory of CVRF files \n"
	,
};

static struct oscap_module CVRF_EXPORT_MODULE = {
	.name = "export",
	.parent = &OSCAP_CVRF_MODULE,
	.summary = "Download and export CVRF file to system",
	.usage = "[options] <cvrf file> export-file.xml",
	.opt_parser = getopt_cvrf,
	.func = app_cvrf_export,
	.help = "Options:\n"
		"   --index\r\t\t\t\t - Use index file to export a directory of CVRF files \n"
	,
};

static struct oscap_module* CVRF_SUBMODULES[] = {
	&CVRF_EVALUATE_MODULE,
	&CVRF_EXPORT_MODULE,
	NULL
};


static int app_cvrf_evaluate(const struct oscap_action *action)
{
	int result = OSCAP_OK;

	const char *os_name = "Red Hat Enterprise Linux Desktop Supplementary (v. 6)";
	cvrf_export_results(action->cvrf_action->cvrf, action->cvrf_action->file, os_name);

	cleanup:
		if (oscap_err())
			fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	oscap_free(action->cvrf_action);
	return result;
}

static int app_cvrf_export(const struct oscap_action *action) {

	int result;
	struct cvrf_model *model = cvrf_model_import(action->cvrf_action->cvrf);

	if(!model) {
		result = OSCAP_ERROR;
		goto cleanup;
	}

	cvrf_model_export(model, action->cvrf_action->file);
	result = OSCAP_OK;

	cleanup:
		if (oscap_err())
			fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	if (model)
		cvrf_model_free(model);
	oscap_free(action->cvrf_action);
	return result;
}

bool getopt_cvrf(int argc, char **argv, struct oscap_action *action)
{
	if( (action->module == &CVRF_EVALUATE_MODULE)) {
		if( argc < 5 || argc > 6) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->doctype = OSCAP_DOCUMENT_CVRF_FEED;
		action->cvrf_action = malloc(sizeof(struct cvrf_action));
		action->cvrf_action->cvrf=argv[3];
		action->cvrf_action->file=argv[4];

	}
	else if (action->module == &CVRF_EXPORT_MODULE) {
		if( argc < 5 || argc > 6) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->doctype = OSCAP_DOCUMENT_CVRF_FEED;
		action->cvrf_action = malloc(sizeof(struct cvrf_action));
		action->cvrf_action->cvrf=argv[3];
		action->cvrf_action->file=argv[4];
	}

	return true;
}
