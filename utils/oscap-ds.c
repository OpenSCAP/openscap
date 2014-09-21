/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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
		"   --xccdf-id <id> \r\t\t\t\t - ID of XCCDF in the datastream that should be evaluated.\n",
	.opt_parser = getopt_ds,
	.func = app_ds_sds_split
};

static struct oscap_module DS_SDS_COMPOSE_MODULE = {
	.name = "sds-compose",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Compose SourceDataStream from given XCCDF",
	.usage = "xccdf-file.xml target_datastream.xml",
	.help = NULL,
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
		,
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
		,
	.opt_parser = getopt_ds,
	.func = app_ds_rds_split
};

static struct oscap_module DS_RDS_CREATE_MODULE = {
	.name = "rds-create",
	.parent = &OSCAP_DS_MODULE,
	.summary = "Create a ResultDataStream from given SourceDataStream, XCCDF results and one or more OVAL results",
	.usage = "sds.xml target-arf.xml results-xccdf.xml [results-oval1.xml [results-oval2.xml]]",
	.help = NULL,
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
	else if ((action->module == &DS_SDS_COMPOSE_MODULE) || action->module == &DS_SDS_ADD_MODULE) {
		if(  argc != 5 ) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[3];
		action->ds_action->target = argv[4];
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
		if(  argc < 6 ) {
			oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
			return false;
		}
		action->ds_action = malloc(sizeof(struct ds_action));
		action->ds_action->file = argv[3];
		action->ds_action->target = argv[4];
		action->ds_action->xccdf_result = argv[5];
		action->ds_action->oval_results = &argv[6];
		action->ds_action->oval_result_count = argc - 6;
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

int app_ds_sds_split(const struct oscap_action *action) {
	int ret;
	struct ds_sds_index* sds_idx = NULL;
	const char* f_datastream_id = action->f_datastream_id;
	const char* f_component_id = action->f_xccdf_id;

	/* Validate */
	if (action->validate)
	{
		int valret;
		if ((valret = oscap_validate_document(action->ds_action->file, OSCAP_DOCUMENT_SDS, "1.2", reporter, (void*) action)))
		{
			if (valret == 1)
				validation_failed(action->ds_action->file, OSCAP_DOCUMENT_SDS, "1.2");

			ret = OSCAP_ERROR;
			goto cleanup;
		}
	}

	sds_idx = ds_sds_index_import(action->ds_action->file);

	if (ds_sds_index_select_checklist(sds_idx, &f_datastream_id, &f_component_id) != 0) {
		fprintf(stdout, "Failed to locate a datastream with ID matching '%s' ID "
				"and checklist inside matching '%s' ID.\n",
				action->f_datastream_id == NULL ? "<any>" : action->f_datastream_id,
				action->f_xccdf_id == NULL ? "<any>" : action->f_xccdf_id);
		ret = OSCAP_ERROR;
		goto cleanup;
	}

	if (ds_sds_decompose(action->ds_action->file, f_datastream_id, f_component_id, action->ds_action->target, NULL) != 0)
	{
		fprintf(stdout, "Failed to split given source datastream '%s'.\n", action->ds_action->file);
		ret = OSCAP_ERROR;
		goto cleanup;
	}

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	ds_sds_index_free(sds_idx);
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

	char previous_cwd[PATH_MAX + 1];
	if (getcwd(previous_cwd, PATH_MAX) == NULL)
	{
		fprintf(stdout, "Can't find out current working directory.\n");

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

	if (action->validate)
	{
		if (oscap_validate_document(target_abs_path, OSCAP_DOCUMENT_SDS, "1.2", reporter, (void*) action) != 0)
		{
			validation_failed(target_abs_path, OSCAP_DOCUMENT_SDS, "1.2");
			goto cleanup;
		}
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
		if (oscap_validate_document(action->ds_action->target, OSCAP_DOCUMENT_SDS, "1.2", reporter, (void*) action) != 0) {
			validation_failed(action->ds_action->target, OSCAP_DOCUMENT_SDS, "1.2");
			ret = OSCAP_ERROR;
		}
	}
	oscap_print_error();

	free(action->ds_action);
	return ret;
}

int app_ds_sds_validate(const struct oscap_action *action) {
	int ret;

	int valret;
	if ((valret = oscap_validate_document(action->ds_action->file, OSCAP_DOCUMENT_SDS, "1.2", reporter, (void*) action)))
	{
		if (valret == 1)
			validation_failed(action->ds_action->file, OSCAP_DOCUMENT_SDS, "1.2");

		ret = OSCAP_ERROR;
		goto cleanup;
	}

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	free(action->ds_action);
	return ret;
}

int app_ds_rds_split(const struct oscap_action *action) {
	int ret;
	struct rds_index *rds_idx = NULL;

	if (action->validate)
	{
		int valret;
		if ((valret = oscap_validate_document(action->ds_action->file, OSCAP_DOCUMENT_ARF, "1.1", reporter, (void*) action)))
		{
			if (valret == 1)
				validation_failed(action->ds_action->file, OSCAP_DOCUMENT_ARF, "1.1");

			ret = OSCAP_ERROR;
			goto cleanup;
		}
	}

	rds_idx = rds_index_import(action->ds_action->file);

	const char* f_report_id = action->f_report_id;
	if (rds_index_select_report(rds_idx, &f_report_id) != 0) {
		fprintf(stdout, "Failed to locate a report with ID matching '%s' ID.",
				action->f_report_id == NULL ? "<any>" : action->f_report_id);
		ret = OSCAP_ERROR;
		goto cleanup;
	}

	struct rds_report_index *report = rds_index_get_report(rds_idx, f_report_id);
	struct rds_report_request_index *request = rds_report_index_get_request(report);
	const char *request_id = request ? rds_report_request_index_get_id(request) : NULL;

	if (ds_rds_decompose(action->ds_action->file, f_report_id, request_id, action->ds_action->target) != 0)
	{
		fprintf(stdout, "Failed to split given result datastream '%s'.\n", action->ds_action->file);
		ret = OSCAP_ERROR;
		goto cleanup;
	}

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	rds_index_free(rds_idx);
	free(action->ds_action);

	return ret;
}

int app_ds_rds_create(const struct oscap_action *action) {
	int ret;

	if (action->validate)
	{
		int valret;
		if ((valret = oscap_validate_document(action->ds_action->file, OSCAP_DOCUMENT_SDS, "1.2", reporter, (void*) action)))
		{
			if (valret == 1)
				validation_failed(action->ds_action->file, OSCAP_DOCUMENT_SDS, "1.2");

			ret = OSCAP_ERROR;
			goto cleanup;
		}

		char *doc_version = NULL;
		doc_version = xccdf_detect_version(action->ds_action->xccdf_result);
		if (!doc_version) {
			ret = OSCAP_ERROR;
			goto cleanup;
		}
		if ((valret = oscap_validate_document(action->ds_action->xccdf_result, OSCAP_DOCUMENT_XCCDF, doc_version, reporter, (void*) action))) {
			if (valret == 1)
				validation_failed(action->ds_action->xccdf_result, OSCAP_DOCUMENT_XCCDF, doc_version);

			ret = OSCAP_ERROR;
			free(doc_version);
			goto cleanup;
		}
		free(doc_version);
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
		int valret;
		if ((valret = oscap_validate_document(action->ds_action->target, OSCAP_DOCUMENT_ARF, "1.1", reporter, (void*) action)))
		{
			if (valret == 1)
				validation_failed(action->ds_action->target, OSCAP_DOCUMENT_ARF, "1.1");

			ret = OSCAP_ERROR;
			goto cleanup;
		}
	}

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	free(action->ds_action);
	return ret;
}

int app_ds_rds_validate(const struct oscap_action *action) {
	int ret;

	int valret;
	if ((valret = oscap_validate_document(action->ds_action->file, OSCAP_DOCUMENT_ARF, "1.1", reporter, (void*) action)))
	{
		if (valret == 1)
			validation_failed(action->ds_action->file, OSCAP_DOCUMENT_ARF, "1.1");

		ret = OSCAP_ERROR;
		goto cleanup;
	}

	ret = OSCAP_OK;

cleanup:
	oscap_print_error();

	free(action->ds_action);
	return ret;
}
