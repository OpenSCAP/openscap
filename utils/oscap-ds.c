/*
 * Copyright 2012--2014 Red Hat Inc., Durham, North Carolina.
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
 *      Martin Preisler <mpreisle@redhat.com>
 */

/* Standard header files */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* DS */
#include <scap_ds.h>
#include <oscap_source.h>
#include <ds_rds_session.h>
#include <ds_sds_session.h>
#include <xccdf_session.h>

#include "oscap-tool.h"
#include <oscap_debug.h>
#include "oscap_helpers.h"

#define DS_SUBMODULES_NUM 8 /* See actual DS_SUBMODULES array
				initialization below. */
static struct oscap_module* DS_SUBMODULES[DS_SUBMODULES_NUM];
bool getopt_ds(int argc, char **argv, struct oscap_action *action);
int app_ds_sds_split(const struct oscap_action *action);
int app_ds_sds_compose(const struct oscap_action *action);
int app_ds_sds_add(const struct oscap_action *action);
int app_ds_sds_validate(const struct oscap_action *action);
int app_ds_rds_split(const struct oscap_action *action);
int app_ds_rds_create(const struct oscap_action *action);
int app_ds_rds_validate(const struct oscap_action *action);

struct oscap_module OSCAP_DS_MODULE = {
	.name = "ds",
	.parent = &OSCAP_ROOT_MODULE,
	.summary = "Data stream utilities",
	.submodules = DS_SUBMODULES
};

static struct oscap_module DS_SDS_VALIDATE_MODULE = {
	.name = "sds-validate",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Validate given source data stream",
	.usage = "source_datastream.xml",
	.help = "Options:\n"
		"   --verbose <verbosity_level>   - Turn on verbose mode at specified verbosity level.\n"
		"                                   Verbosity level must be one of: DEVEL, INFO, WARNING, ERROR.\n"
		"   --verbose-log-file <file>     - Write verbose information into file.\n",
	.opt_parser = getopt_ds,
	.func = app_ds_sds_validate
};

static struct oscap_module DS_RDS_VALIDATE_MODULE = {
	.name = "rds-validate",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Validate given result data stream",
	.usage = "[options] result_datastream.xml",
	.help = "Options:\n"
		"   --verbose <verbosity_level>   - Turn on verbose mode at specified verbosity level.\n"
		"                                   Verbosity level must be one of: DEVEL, INFO, WARNING, ERROR.\n"
		"   --verbose-log-file <file>     - Write verbose information into file.\n",
	.opt_parser = getopt_ds,
	.func = app_ds_rds_validate
};

static struct oscap_module* DS_SUBMODULES[DS_SUBMODULES_NUM] = {
	&DS_SDS_VALIDATE_MODULE,
	&DS_RDS_VALIDATE_MODULE,
	NULL
};

enum ds_opt {
	DS_OPT_DATASTREAM_ID = 1,
	DS_OPT_XCCDF_ID,
	DS_OPT_REPORT_ID,
	DS_OPT_LOCAL_FILES,
	DS_OPT_VERBOSE,
	DS_OPT_VERBOSE_LOG_FILE,
};

bool getopt_ds(int argc, char **argv, struct oscap_action *action) {
	action->doctype = OSCAP_DOCUMENT_SDS;

	/* Command-options */
	const struct option long_options[] = {
	// options
		{"skip-validation",      no_argument, &action->validate, 0},
		{"datastream-id",		required_argument, NULL, DS_OPT_DATASTREAM_ID},
		{"xccdf-id",		required_argument, NULL, DS_OPT_XCCDF_ID},
		{"report-id",		required_argument, NULL, DS_OPT_REPORT_ID},
		{"fetch-remote-resources", no_argument, &action->remote_resources, 1},
		{"local-files", required_argument, NULL, DS_OPT_LOCAL_FILES},
		{"verbose", required_argument, NULL, DS_OPT_VERBOSE},
		{"verbose-log-file", required_argument, NULL, DS_OPT_VERBOSE_LOG_FILE},
	// end
		{0, 0, 0, 0}
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:i:", long_options, NULL)) != -1) {

		switch (c) {
		case DS_OPT_DATASTREAM_ID:	action->f_datastream_id = optarg;	break;
		case DS_OPT_XCCDF_ID:	action->f_xccdf_id = optarg; break;
		case DS_OPT_REPORT_ID:	action->f_report_id = optarg; break;
		case DS_OPT_LOCAL_FILES:
			action->local_files = optarg;
			break;
		case DS_OPT_VERBOSE:
			action->verbosity_level = optarg;
			break;
		case DS_OPT_VERBOSE_LOG_FILE:
			action->f_verbose_log = optarg;
			break;
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

    if (action->module == &DS_SDS_VALIDATE_MODULE) {
		if(  argc != 4 ) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[3];
	}
	else if (action->module == &DS_RDS_VALIDATE_MODULE) {
		if(optind >= argc) {
			oscap_module_usage(action->module, stderr, "Result data stream file need to be specified!\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[optind];
	}
	return true;
}

static inline char *_gcwd(void)
{
	char *cwd = malloc(sizeof(char) * (PATH_MAX + 1));
	if (getcwd(cwd, PATH_MAX) == NULL) {
		perror("Can't find out current working directory.\n");
		free(cwd);
		cwd = NULL;
	}
	return cwd;
}

int app_ds_sds_split(const struct oscap_action *action) {
	int ret = OSCAP_ERROR;
	const char* f_datastream_id = action->f_datastream_id;
	const char* f_component_id = action->f_xccdf_id;
	struct ds_sds_session *session = NULL;

	struct oscap_source *source = oscap_source_new_from_file(action->ds_action->file);
	/* Validate */
	if (action->validate)
	{
		if (oscap_source_validate(source, reporter, (void *) action) != 0) {
			goto cleanup;
		}
	}

	session = ds_sds_session_new_from_source(source);
	if (session == NULL) {
		goto cleanup;
	}
	if (ds_sds_index_select_checklist(ds_sds_session_get_sds_idx(session), &f_datastream_id, &f_component_id) != 0) {
		fprintf(stdout, "Failed to locate a data stream with ID matching '%s' ID "
				"and checklist inside matching '%s' ID.\n",
				action->f_datastream_id == NULL ? "<any>" : action->f_datastream_id,
				action->f_xccdf_id == NULL ? "<any>" : action->f_xccdf_id);
		goto cleanup;
	}
	ds_sds_session_set_datastream_id(session, f_datastream_id);

	ds_sds_session_configure_remote_resources(session, action->remote_resources, action->local_files, download_reporting_callback);
	ds_sds_session_set_target_dir(session, action->ds_action->target);
	if (ds_sds_session_register_component_with_dependencies(session, "checklists", f_component_id, NULL) != 0) {
		goto cleanup;
	}
	// CPE dictionaries aren't required in datastreams, just silently continue
	// if we can't register them
	if (ds_sds_session_can_register_component(session, "dictionaries", NULL)) {
		if (ds_sds_session_register_component_with_dependencies(session, "dictionaries", NULL, NULL) != 0) {
			goto cleanup;
		}
	}
	if (ds_sds_session_dump_component_files(session) != 0) {
		goto cleanup;
	}

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	ds_sds_session_free(session);
	oscap_source_free(source);
	free(action->ds_action);

	return ret;
}

int app_ds_sds_compose(const struct oscap_action *action) {
	int ret = OSCAP_ERROR;

	// The API will correctly deal with the file parameter having directory
	// references in it. However this will create a hard to navigate mangled
	// component IDs in the resulting datastream.
	//
	// To fix this we will chdir to parent dir of the given XCCDF and chdir
	// back after we are done.

	char *previous_cwd = _gcwd();
	if (previous_cwd == NULL) {
		goto cleanup;
	}

	char target_abs_path[PATH_MAX + 1];

	// if the path is already absolute we just use it as it is
	if (*action->ds_action->target == '/')
		snprintf(target_abs_path, PATH_MAX, "%s", action->ds_action->target);
	else
		snprintf(target_abs_path, PATH_MAX, "%s/%s", previous_cwd, action->ds_action->target);

	char* temp_cwd = strdup(action->ds_action->file);
	char *temp_cwd_dirname = oscap_dirname(temp_cwd);
	if (chdir(temp_cwd_dirname) < 0) {
		free(temp_cwd_dirname);
		free(temp_cwd);
		goto cleanup;
	}
	free(temp_cwd_dirname);
	free(temp_cwd);

	char* source_xccdf = strdup(action->ds_action->file);
	char *base_name = oscap_basename(source_xccdf);
	ds_sds_compose_from_xccdf(base_name, target_abs_path);
	free(base_name);
	free(source_xccdf);

	if (chdir(previous_cwd) < 0) {
		goto cleanup;
	}

	if (action->validate)
	{
		struct oscap_source *source = oscap_source_new_from_file(target_abs_path);
		if (oscap_source_validate(source, reporter, (void *) action) != 0) {
			oscap_source_free(source);
			goto cleanup;
		}
		oscap_source_free(source);
	}

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	free(previous_cwd);
	free(action->ds_action);
	return ret;
}

int app_ds_sds_add(const struct oscap_action *action)
{
	int ret = OSCAP_ERROR;
	// TODO: chdir to the directory of the component (same as when composing new sds)
	ret = ds_sds_compose_add_component(action->ds_action->target, action->f_datastream_id, action->ds_action->file, false);
	if (action->validate) {
		struct oscap_source *source = oscap_source_new_from_file(action->ds_action->file);
		if (oscap_source_validate(source, reporter, (void *) action) != 0) {
			ret = OSCAP_ERROR;
		}
		oscap_source_free(source);
	}
	oscap_print_error();

	free(action->ds_action);
	return ret;
}

int app_ds_sds_validate(const struct oscap_action *action) {
	int ret = OSCAP_ERROR;
	struct oscap_source *source = oscap_source_new_from_file(action->ds_action->file);
	if (oscap_source_validate(source, reporter, (void*) action) != 0) {
		goto cleanup;
	}

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	free(action->ds_action);
	oscap_source_free(source);
	return ret;
}

int app_ds_rds_split(const struct oscap_action *action) {
	int ret = OSCAP_ERROR;
	struct ds_rds_session *session = NULL;

	struct oscap_source *source = oscap_source_new_from_file(action->ds_action->file);
	if (action->validate)
	{
		if (oscap_source_validate(source, reporter, (void *) action) != 0) {
			goto cleanup;
		}
	}
	session = ds_rds_session_new_from_source(source);
	if (session == NULL
			|| ds_rds_session_set_target_dir(session, action->ds_action->target) != 0
			|| ds_rds_session_select_report(session, action->f_report_id) == NULL
			|| ds_rds_session_select_report_request(session, NULL) == NULL
			|| ds_rds_session_dump_component_files(session) != 0) {
		fprintf(stdout, "Failed to split given result data stream '%s'.\n", action->ds_action->file);
		goto cleanup;
	}

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	ds_rds_session_free(session);
	free(action->ds_action);
	oscap_source_free(source);

	return ret;
}

int app_ds_rds_create(const struct oscap_action *action) {
	int ret = OSCAP_ERROR;

	if (action->validate)
	{
		struct oscap_source *sds = oscap_source_new_from_file(action->ds_action->file);
		if (oscap_source_validate(sds, reporter, (void *) action) != 0) {
			oscap_source_free(sds);
			goto cleanup;
		}
		oscap_source_free(sds);

		struct oscap_source *result = oscap_source_new_from_file(action->ds_action->xccdf_result);
		if (oscap_source_validate(result, reporter, (void *) action) != 0) {
			ret = OSCAP_ERROR;
			oscap_source_free(result);
			goto cleanup;
		}
		oscap_source_free(result);
	}

	char** oval_result_files = malloc(sizeof(char*) * (action->ds_action->oval_result_count + 1));

	size_t i;
	for (i = 0; i < action->ds_action->oval_result_count; ++i)
	{
		oval_result_files[i] = action->ds_action->oval_results[i];

		if (action->validate)
		{
			struct oscap_source *source = oscap_source_new_from_file(oval_result_files[i]);
			if (oscap_source_validate(source, reporter, (void *) action) != 0) {
				ret = OSCAP_ERROR;
				oscap_source_free(source);
				free(oval_result_files);
				goto cleanup;
			}
			oscap_source_free(source);
		}
	}
	oval_result_files[i] = NULL;

	ret = ds_rds_create(action->ds_action->file, action->ds_action->xccdf_result,
		   (const char**)oval_result_files, action->ds_action->target);

	free(oval_result_files);

	if (ret != 0)
	{
		fprintf(stdout, "Failed to create result data stream in ARF.");
		ret = OSCAP_ERROR;
		goto cleanup;
	}

	const char* full_validation = getenv("OSCAP_FULL_VALIDATION");

	if (action->validate && full_validation)
	{
		struct oscap_source *rds = oscap_source_new_from_file(action->ds_action->target);
		if (oscap_source_validate(rds, reporter, (void *) action) != 0) {
			oscap_source_free(rds);
			goto cleanup;
		}
		oscap_source_free(rds);
	}

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	free(action->ds_action);
	return ret;
}

int app_ds_rds_validate(const struct oscap_action *action) {
	int ret = OSCAP_ERROR;

	struct oscap_source *rds = oscap_source_new_from_file(action->ds_action->file);
	if (oscap_source_validate(rds, reporter, (void *) action) != 0) {
		oscap_source_free(rds);
		goto cleanup;
	}
	oscap_source_free(rds);

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	free(action->ds_action);
	return ret;
}
