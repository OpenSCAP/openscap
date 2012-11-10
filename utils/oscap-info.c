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
 *      Peter Vrabec  <pvrabec@redhat.com>
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
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <oscap.h>
#include <oval_results.h>
#include <oval_variables.h>
#include <oval_system_characteristics.h>
#include <oval_directives.h>
#include <scap_ds.h>

#include "oscap-tool.h"

static bool getopt_info(int argc, char **argv, struct oscap_action *action);
static int app_info(const struct oscap_action *action);

struct oscap_module OSCAP_INFO_MODULE = {
    .name = "info",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "info module",
    .usage = "some-file.xml",
    .help = "Print information about a file",
    .opt_parser = getopt_info,
    .func = app_info
};

static void print_time(const char *file) {
	struct stat buffer;
	char timeStr[ 100 ] = "";

	if(!stat(file, &buffer)) {
		strftime(timeStr, 100, "%Y-%m-%dT%H:%M:%S", localtime(&buffer.st_mtime));
		printf("Imported: %s\n", timeStr);
	}
}


static int app_info(const struct oscap_action *action)
{
	oscap_document_type_t doc_type;
        int result = OSCAP_ERROR;

	if(oscap_determine_document_type(action->file, &doc_type))
		goto cleanup;

	switch (doc_type) {
	case OSCAP_DOCUMENT_OVAL_DEFINITIONS: {
		printf("Document type: OVAL Definitions\n");
		struct oval_definition_model * def_model = oval_definition_model_import(action->file);
		if(!def_model)
			goto cleanup;
		struct oval_generator *gen = oval_definition_model_get_generator(def_model);
		printf("OVAL version: %s\n", oval_generator_get_schema_version(gen));
		printf("Generated: %s\n", oval_generator_get_timestamp(gen));
		print_time(action->file);
		oval_definition_model_free(def_model);
	}
	break;
	case OSCAP_DOCUMENT_OVAL_VARIABLES: {
		printf("Document type: OVAL Variables\n");
		struct oval_variable_model * var_model = oval_variable_model_import(action->file);
		if(!var_model)
			goto cleanup;
		struct oval_generator *gen = oval_variable_model_get_generator(var_model);
		printf("OVAL version: %s\n", oval_generator_get_schema_version(gen));
		printf("Generated: %s\n", oval_generator_get_timestamp(gen));
		print_time(action->file);
		oval_variable_model_free(var_model);
	}
	break;
	case OSCAP_DOCUMENT_OVAL_DIRECTIVES: {
		printf("Document type: OVAL Directives\n");
		struct oval_directives_model *dir_model = oval_directives_model_new();
		int ret = oval_directives_model_import(dir_model, action->file);
		if(ret)
			goto cleanup;
		struct oval_generator *gen = oval_directives_model_get_generator(dir_model);
		printf("OVAL version: %s\n", oval_generator_get_schema_version(gen));
		printf("Generated: %s\n", oval_generator_get_timestamp(gen));
		print_time(action->file);
		oval_directives_model_free(dir_model);
	}
	break;
	case OSCAP_DOCUMENT_OVAL_SYSCHAR: {
		printf("Document type: OVAL System Characteristics\n");
		struct oval_definition_model * def_model = oval_definition_model_new();
		struct oval_syschar_model * sys_model = oval_syschar_model_new(def_model);
		int ret = oval_syschar_model_import(sys_model, action->file);
		if(ret)
			goto cleanup;
		struct oval_generator *gen = oval_syschar_model_get_generator(sys_model);
		printf("OVAL version: %s\n", oval_generator_get_schema_version(gen));
		printf("Generated: %s\n", oval_generator_get_timestamp(gen));
		print_time(action->file);
		oval_syschar_model_free(sys_model);
		oval_definition_model_free(def_model);
	}
	break;
	case OSCAP_DOCUMENT_OVAL_RESULTS: {
		printf("Document type: OVAL Results\n");
		struct oval_definition_model * def_model=oval_definition_model_new();
		struct oval_results_model * res_model = oval_results_model_new(def_model,NULL);
		int ret = oval_results_model_import(res_model, action->file);
		if(ret)
			goto cleanup;
		struct oval_generator *gen = oval_results_model_get_generator(res_model);
		printf("OVAL version: %s\n", oval_generator_get_schema_version(gen));
		printf("Generated: %s\n", oval_generator_get_timestamp(gen));
		print_time(action->file);
		oval_results_model_free(res_model);
		oval_definition_model_free(def_model);
	}
	break;
	case OSCAP_DOCUMENT_XCCDF: {
		printf("Document type: XCCDF Checklist\n");
		struct xccdf_benchmark* bench = xccdf_benchmark_import(action->file);
		if(!bench)
			goto cleanup;
		char * doc_version =  xccdf_detect_version(action->file);
		printf("Checklist version: %s\n", doc_version);
		/* get current status */
		struct xccdf_status * status = NULL;
		status = xccdf_benchmark_get_status_current(bench);
		if (status) {
			printf("Status: %s\n", xccdf_status_type_to_text(xccdf_status_get_status(status)));
			const time_t date_time = xccdf_status_get_date(status);
			struct tm *date = localtime(&date_time);
			char date_str[] = "YYYY-DD-MM";
			snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", date->tm_year + 1900, date->tm_mon + 1, date->tm_mday);
			printf("Generated: %s\n", date_str);
		}
		print_time(action->file);
		printf("Resolved: %s\n", xccdf_benchmark_get_resolved(bench) ? "true" : "false");
		struct xccdf_profile_iterator * prof_it = xccdf_benchmark_get_profiles(bench);
		printf("Profiles:\n");
		struct xccdf_profile * prof = NULL;
		while (xccdf_profile_iterator_has_more(prof_it)) {
			prof = xccdf_profile_iterator_next(prof_it);
			printf("\t%s\n", xccdf_profile_get_id(prof));
		}
		free(doc_version);
		xccdf_profile_iterator_free(prof_it);
		xccdf_benchmark_free(bench);
	}
	break;
	case OSCAP_DOCUMENT_CPE_LANGUAGE: {
		printf("Document type: CPE Language\n");
		print_time(action->file);
	}
	break;
	case OSCAP_DOCUMENT_CPE_DICTIONARY: {
		printf("Document type: CPE Dictionary\n");
		struct cpe_dict_model *dict_model = cpe_dict_model_import(action->file);
		if (!dict_model)
			goto cleanup;
		struct cpe_generator *gen = cpe_dict_model_get_generator(dict_model);
		printf("CPE version: %s\n", cpe_generator_get_schema_version(gen));
		printf("Generated: %s\n", cpe_generator_get_timestamp(gen));
		print_time(action->file);
		cpe_dict_model_free(dict_model);
	}
	break;
	case OSCAP_DOCUMENT_SDS: {
		printf("Document type: Source Data Stream\n");
		print_time(action->file);
		/* get collection */
		struct ds_sds_index *sds = ds_sds_index_import(action->file);
		if (!sds)
			goto cleanup;
		/* iterate over streams */
		printf("Streams:\n");
		struct ds_stream_index_iterator* sds_it = ds_sds_index_get_streams(sds);
		while (ds_stream_index_iterator_has_more(sds_it)) {
			struct ds_stream_index * stream = ds_stream_index_iterator_next(sds_it);

			printf("\tID: %s\n", ds_stream_index_get_id(stream));
			printf("\tGenerated: %s\n", ds_stream_index_get_timestamp(stream));
			printf("\tVersion: %s\n", ds_stream_index_get_version(stream));

			printf("\tChecklists:\n");
			struct oscap_string_iterator* checklist_it = ds_stream_index_get_checklists(stream);
			while (oscap_string_iterator_has_more(checklist_it)) {
				char * id = oscap_string_iterator_next(checklist_it);
				printf("\t\t Ref-Id: %s\n", id);
			}
			oscap_string_iterator_free(checklist_it);

			printf("\tChecks:\n");
			struct oscap_string_iterator* checks_it = ds_stream_index_get_checks(stream);
			while (oscap_string_iterator_has_more(checks_it)) {
				char * id = oscap_string_iterator_next(checks_it);
				printf("\t\t Ref-Id: %s\n", id);
			}
			oscap_string_iterator_free(checks_it);

			struct oscap_string_iterator* dict_it = ds_stream_index_get_dictionaries(stream);
			if (oscap_string_iterator_has_more(dict_it))
				printf("\tDictionaries:\n");
			else
				printf("\tNo dictionaries.\n");
			while (oscap_string_iterator_has_more(dict_it)) {
				char * id = oscap_string_iterator_next(dict_it);
				printf("\t\t Ref-Id: %s\n", id);
			}
			oscap_string_iterator_free(dict_it);
		}
		ds_stream_index_iterator_free(sds_it);
		ds_sds_index_free(sds);
	}
	break;
	default:
		printf("Document type not handled yet\n");
		break;
	}

	result=OSCAP_OK;

cleanup:
	if (oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	return result;
}

bool getopt_info(int argc, char **argv, struct oscap_action *action)
{
	if(  argc != 3) {
		oscap_module_usage(action->module, stderr, "Wrong number of parameteres.\n");
		return false;
	}
	action->file = argv[2];

	return true;
}
