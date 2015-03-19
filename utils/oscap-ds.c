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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      Martin Preisler <mpreisle@redhat.com>
 */

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* DS */
#include <scap_ds.h>
#include <oscap_source.h>
#include <ds_rds_session.h>
#include <ds_sds_session.h>

#include "oscap-tool.h"

static struct oscap_module* DS_SUBMODULES[];
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
	.summary = "DataStream utilities",
	.submodules = DS_SUBMODULES
};

static struct oscap_module DS_SDS_SPLIT_MODULE = {
	.name = "sds-split",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Split given SourceDataStream into separate files",
	.usage = "[options] SDS TARGET_DIRECTORY",
	.help =
		"SDS - Source data stream that will be split into multiple files.\n"
		"TARGET_DIRECTORY - Directory of the resulting files.\n"
		"\n"
		"Options:\n"
		"   --datastream-id <id> \r\t\t\t\t - ID of the datastream in the collection to use.\n"
		"   --xccdf-id <id> \r\t\t\t\t - ID of XCCDF in the datastream that should be evaluated.\n"
		"   --skip-valid \r\t\t\t\t - Skips validating of given XCCDF.\n",
	.opt_parser = getopt_ds,
	.func = app_ds_sds_split
};

static struct oscap_module DS_SDS_COMPOSE_MODULE = {
	.name = "sds-compose",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Compose SourceDataStream from given XCCDF",
	.usage = "[options] xccdf-file.xml target_datastream.xml",
	.help = "Options:\n"
		"   --skip-valid \r\t\t\t\t - Skips validating of given XCCDF.\n",
	.opt_parser = getopt_ds,
	.func = app_ds_sds_compose
};

static struct oscap_module DS_SDS_ADD_MODULE = {
	.name = "sds-add",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Add a component to the existing SourceDataStream",
	.usage = "[options] new-component.xml existing_datastream.xml",
	.help =	"Options:\n"
		"   --datastream-id <id> \r\t\t\t\t - ID of the datastream in the collection for adding to.\n"
		"   --skip-valid \r\t\t\t\t - Skips validating of given XCCDF.\n",
	.opt_parser = getopt_ds,
	.func = app_ds_sds_add
};

static struct oscap_module DS_SDS_VALIDATE_MODULE = {
	.name = "sds-validate",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Validate given SourceDataStream",
	.usage = "source_datastream.xml",
	.help = NULL,
	.opt_parser = getopt_ds,
	.func = app_ds_sds_validate
};

static struct oscap_module DS_RDS_SPLIT_MODULE = {
	.name = "rds-split",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Splits a ResultDataStream. Creating source datastream (from report-request) and report in target directory.",
	.usage = "[OPTIONS] rds.xml TARGET_DIRECTORY",
	.help =	"Options:\n"
		"   --report-id <id> \r\t\t\t\t - ID of report inside ARF that should be split.\n"
		"   --skip-valid \r\t\t\t\t - Skips validating of given XCCDF.\n",
	.opt_parser = getopt_ds,
	.func = app_ds_rds_split
};

static struct oscap_module DS_RDS_CREATE_MODULE = {
	.name = "rds-create",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Create a ResultDataStream from given SourceDataStream, XCCDF results and one or more OVAL results",
	.usage = "[options] sds.xml target-arf.xml results-xccdf.xml [results-oval1.xml [results-oval2.xml]]",
	.help =	"Options:\n"
		"   --skip-valid \r\t\t\t\t - Skips validating of given XCCDF.\n",
	.opt_parser = getopt_ds,
	.func = app_ds_rds_create
};

static struct oscap_module DS_RDS_VALIDATE_MODULE = {
	.name = "rds-validate",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Validate given ResultDataStream",
	.usage = "result_datastream.xml",
	.help = NULL,
	.opt_parser = getopt_ds,
	.func = app_ds_rds_validate
};

static struct oscap_module* DS_SUBMODULES[] = {
	&DS_SDS_SPLIT_MODULE,
	&DS_SDS_COMPOSE_MODULE,
	&DS_SDS_ADD_MODULE,
	&DS_SDS_VALIDATE_MODULE,
	&DS_RDS_SPLIT_MODULE,
	&DS_RDS_CREATE_MODULE,
	&DS_RDS_VALIDATE_MODULE,
	NULL
};

enum ds_opt {
	DS_OPT_DATASTREAM_ID = 1,
	DS_OPT_XCCDF_ID,
	DS_OPT_REPORT_ID,
};

bool getopt_ds(int argc, char **argv, struct oscap_action *action) {
	action->doctype = OSCAP_DOCUMENT_SDS;

	/* Command-options */
	const struct option long_options[] = {
	// options
		{"skip-valid",      no_argument, &action->validate, 0},
		{"datastream-id",		required_argument, NULL, DS_OPT_DATASTREAM_ID},
		{"xccdf-id",		required_argument, NULL, DS_OPT_XCCDF_ID},
		{"report-id",		required_argument, NULL, DS_OPT_REPORT_ID},
	// end
		{0, 0, 0, 0}
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:i:", long_options, NULL)) != -1) {

		switch (c) {
		case DS_OPT_DATASTREAM_ID:	action->f_datastream_id = optarg;	break;
		case DS_OPT_XCCDF_ID:	action->f_xccdf_id = optarg; break;
		case DS_OPT_REPORT_ID:	action->f_report_id = optarg; break;
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	if (action->module == &DS_SDS_SPLIT_MODULE) {
		if (optind + 2 != argc) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[optind];
		action->ds_action->target = argv[optind + 1];
	}
	else if (action->module == &DS_SDS_COMPOSE_MODULE) {
		if(optind + 2 != argc) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[optind];
		action->ds_action->target = argv[optind + 1];
	}
	else if (action->module == &DS_SDS_ADD_MODULE) {
		if (optind + 2 != argc) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[optind];
		action->ds_action->target = argv[optind + 1];
	}
	else if( (action->module == &DS_SDS_VALIDATE_MODULE) ) {
		if(  argc != 4 ) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[3];
	}
	else if (action->module == &DS_RDS_SPLIT_MODULE) {
		if (optind + 2 != argc) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[optind];
		action->ds_action->target = argv[optind + 1];
	}
	else if( (action->module == &DS_RDS_CREATE_MODULE) ) {
		if(argc - optind < 3 ) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[optind];
		action->ds_action->target = argv[optind + 1];
		action->ds_action->xccdf_result = argv[optind + 2];
		action->ds_action->oval_results = &argv[optind + 3];
		action->ds_action->oval_result_count = argc - optind - 3;
	}
	else if( (action->module == &DS_RDS_VALIDATE_MODULE) ) {
		if(  argc != 4 ) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[3];
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
		fprintf(stdout, "Failed to locate a datastream with ID matching '%s' ID "
				"and checklist inside matching '%s' ID.\n",
				action->f_datastream_id == NULL ? "<any>" : action->f_datastream_id,
				action->f_xccdf_id == NULL ? "<any>" : action->f_xccdf_id);
		goto cleanup;
	}
	ds_sds_session_set_datastream_id(session, f_datastream_id);

	ds_sds_session_set_target_dir(session, action->ds_action->target);
	if (ds_sds_session_register_component_with_dependencies(session, "checklists", f_component_id, NULL) != 0) {
		goto cleanup;
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
	chdir(dirname(temp_cwd));
	free(temp_cwd);

	char* source_xccdf = strdup(action->ds_action->file);
	ds_sds_compose_from_xccdf(basename(source_xccdf), target_abs_path);
	free(source_xccdf);

	chdir(previous_cwd);
	free(previous_cwd);

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
		fprintf(stdout, "Failed to split given result datastream '%s'.\n", action->ds_action->file);
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
		fprintf(stdout, "Failed to create result datastream in ARF.");
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
