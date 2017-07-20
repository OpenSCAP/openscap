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
    .usage = "cvrf-file.xml",
    .help = "Evaluate vulnerabilities of the system",
    .opt_parser = getopt_cvrf,
    .func = app_cvrf_evaluate
};

static struct oscap_module CVRF_EXPORT_MODULE = {
    .name = "export",
    .parent = &OSCAP_CVRF_MODULE,
    .summary = "Download and export CVRF file to system",
    .usage = "cvrf-file.xml",
    .help = "Download and export CVRF file to system",
    .opt_parser = getopt_cvrf,
    .func = app_cvrf_export
};

static struct oscap_module* CVRF_SUBMODULES[] = {
    &CVRF_EVALUATE_MODULE,
    &CVRF_EXPORT_MODULE,
    NULL
};


static int app_cvrf_evaluate(const struct oscap_action *action)
{
	int result = 0;

	const char *os_name = "Fedora";
	cvrf_export_results(action->cvrf_action->cvrf, action->cvrf_action->file, os_name);

        free(action->cvrf_action);
        return result;
}

static int app_cvrf_export(const struct oscap_action *action) {
	return 0;
}

bool getopt_cvrf(int argc, char **argv, struct oscap_action *action)
{
        if( (action->module == &CVRF_EVALUATE_MODULE)) {
                if( argc != 5 ) {
                        oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
                        return false;
                }
                action->doctype = OSCAP_DOCUMENT_CVRF_FEED;
                action->cvrf_action = malloc(sizeof(struct cvrf_action));
        		action->cvrf_action->cvrf=argv[3];
                action->cvrf_action->file=argv[4];
        }
	else if (action->module == &CVRF_EXPORT_MODULE) {
	        if( argc != 5 ) {
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
