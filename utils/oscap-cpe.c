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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      Peter Vrabec  <pvrabec@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

/* CPE */
#include <cpe_name.h>
#include <cpe_dict.h>
#include <cpe_lang.h>
#include <oscap_source.h>

#include "oscap-tool.h"

#define CPE_SUBMODULES_NUM 4 /* See actual CPE_SUBMODULES array
				initialization below. */
static struct oscap_module* CPE_SUBMODULES[CPE_SUBMODULES_NUM];
bool getopt_cpe(int argc, char **argv, struct oscap_action *action);
int app_cpe_check(const struct oscap_action *action);
int app_cpe_match(const struct oscap_action *action);
int app_cpe_validate(const struct oscap_action *action);

struct oscap_module OSCAP_CPE_MODULE = {
    .name = "cpe",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "Common Platform Enumeration",
    .submodules = CPE_SUBMODULES
};

static struct oscap_module CPE_MATCH_MODULE = {
    .name = "match",
    .parent = &OSCAP_CPE_MODULE,
    .summary = "Match CPE name against provided dictionary",
    .usage = "name dictionary.xml",
	.help = "Options:\n"
		"   --verbose <verbosity_level>   - Turn on verbose mode at specified verbosity level.\n"
		"                                   Verbosity level must be one of: DEVEL, INFO, WARNING, ERROR.\n"
		"   --verbose-log-file <file>     - Write verbose information into file.\n",
    .opt_parser = getopt_cpe,
    .func = app_cpe_match
};

static struct oscap_module CPE_CHECK_MODULE = {
    .name = "check",
    .parent = &OSCAP_CPE_MODULE,
    .summary = "Check if CPE name is valid",
    .usage = "name",
	.help = "Options:\n"
		"   --verbose <verbosity_level>   - Turn on verbose mode at specified verbosity level.\n"
		"                                   Verbosity level must be one of: DEVEL, INFO, WARNING, ERROR.\n"
		"   --verbose-log-file <file>     - Write verbose information into file.\n",
    .opt_parser = getopt_cpe,
    .func = app_cpe_check
};

static struct oscap_module CPE_VALIDATE = {
    .name = "validate",
    .parent = &OSCAP_CPE_MODULE,
    .summary = "Validate CPE Dictionary content",
    .usage = "cpe-dict.xml",
	.help = "Options:\n"
		"   --verbose <verbosity_level>   - Turn on verbose mode at specified verbosity level.\n"
		"                                   Verbosity level must be one of: DEVEL, INFO, WARNING, ERROR.\n"
		"   --verbose-log-file <file>     - Write verbose information into file.\n",
    .opt_parser = getopt_cpe,
    .func = app_cpe_validate
};

static struct oscap_module* CPE_SUBMODULES[CPE_SUBMODULES_NUM] = {
    &CPE_MATCH_MODULE,
    &CPE_CHECK_MODULE,
    &CPE_VALIDATE,
    NULL
};

bool getopt_cpe(int argc, char **argv, struct oscap_action *action) {
	enum oscap_cpe_opts {
		OSCAP_CPE_OPT_VERBOSE = 1,
		OSCAP_CPE_OPT_VERBOSE_LOG_FILE,
	};
	const struct option long_options[] = {
	// options
		{"verbose", required_argument, NULL, OSCAP_CPE_OPT_VERBOSE},
		{"verbose-log-file", required_argument, NULL, OSCAP_CPE_OPT_VERBOSE_LOG_FILE},
	// end
		{0, 0, 0, 0}
	};
	int c;
	while ((c = getopt_long(argc, argv, "o:i:", long_options, NULL)) != -1) {
		switch (c) {
		case OSCAP_CPE_OPT_VERBOSE:
			action->verbosity_level = optarg;
			break;
		case OSCAP_CPE_OPT_VERBOSE_LOG_FILE:
			action->f_verbose_log = optarg;
			break;
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}
	if (optind >= argc) {
		return oscap_module_usage(action->module, stderr, "Wrong number of arguments!\n");
	}
	action->cpe_action = malloc(sizeof(struct cpe_action));
	if (action->cpe_action == NULL) {
		fprintf(stderr, "Memory allocation error.\n");
		return false;
	}
	if (action->module == &CPE_MATCH_MODULE) {
		action->cpe_action->name = argv[optind];
		action->cpe_action->dict = argv[optind + 1];
	}
	if (action->module == &CPE_CHECK_MODULE) {
		action->cpe_action->name = argv[optind];
	}
	if (action->module == &CPE_VALIDATE) {
		action->cpe_action->dict = argv[optind];
	}
	return true;
}

int app_cpe_check(const struct oscap_action *action) {
	int ret;

	if (!cpe_name_check(action->cpe_action->name)) {
		fprintf(stdout,"'%s' is NOT Valid CPE name.\n", action->cpe_action->name);
		ret = OSCAP_FAIL;
	}
	else {
		fprintf(stdout,"'%s' is Valid CPE name.\n", action->cpe_action->name);
		ret = OSCAP_OK;
	}
	free(action->cpe_action);
	return ret;
}

int app_cpe_match(const struct oscap_action *action) {

	int ret;
	struct cpe_name *candidate_cpe = NULL;
        struct cpe_dict_model *dict = NULL;

	struct oscap_source *source = NULL;

        /* is CPE well formated? */
        if( ! cpe_name_check(action->cpe_action->name) ) {
                fprintf(stdout, "%s is not in valid CPE format.\n", action->cpe_action->name);
		ret = OSCAP_ERROR;
                goto clean;
        }
        candidate_cpe = cpe_name_new(action->cpe_action->name);

        /* load dictionary */
	source = oscap_source_new_from_file(action->cpe_action->dict);
	if( (dict = cpe_dict_model_import_source(source)) == NULL ) {
                fprintf(stdout, "can't load CPE dictionary from: %s.\n", action->cpe_action->dict);
		ret = OSCAP_ERROR;
                goto clean;
        }

	/* matching */
        if( cpe_name_match_dict(candidate_cpe, dict) ) {
        	fprintf(stdout, "The exact CPE match is found.\n");
		ret = OSCAP_OK;
	}
        else {
                fprintf(stdout, "No match found.\n");
		ret = OSCAP_FAIL;
	}

        /* clean up */
clean:
        cpe_name_free(candidate_cpe);
        cpe_dict_model_free(dict);
	free(action->cpe_action);
	oscap_source_free(source);
	return ret;
}

int app_cpe_validate(const struct oscap_action *action) {

	int ret;
	int result;

	struct oscap_source *source = oscap_source_new_from_file(action->cpe_action->dict);
	ret = oscap_source_validate(source, reporter, (void *) action);
	oscap_source_free(source);

	if (ret==-1) {
		result=OSCAP_ERROR;
		goto cleanup;
	}
	else if (ret==1) {
		result=OSCAP_FAIL;
	}
	else {
		result=OSCAP_OK;
	}

cleanup:
	if (oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	free(action->cpe_action);
	return result;
}

