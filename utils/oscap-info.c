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
#include <limits.h>

#include <oscap.h>
#include "oscap_source.h"
#include <xccdf_policy.h>
#include <oval_results.h>
#include <oval_variables.h>
#include <oval_system_characteristics.h>
#include <oval_directives.h>
#include <scap_ds.h>
#include <ds_rds_session.h>
#include <ds_sds_session.h>

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

static inline void _print_xccdf_status(struct xccdf_status *status, const char *prefix)
{
	if (status) {
		printf("%sStatus: %s\n", prefix, xccdf_status_type_to_text(xccdf_status_get_status(status)));
		const time_t date_time = xccdf_status_get_date(status);
		if (date_time != 0) {
			struct tm *date = localtime(&date_time);
			char date_str[] = "YYYY-DD-MM";
			snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", date->tm_year + 1900, date->tm_mon + 1, date->tm_mday);
			printf("%sGenerated: %s\n", prefix, date_str);
		}
	}
}

static inline void _print_xccdf_profiles(struct xccdf_profile_iterator *prof_it, const char *prefix)
{
	printf("%sProfiles:\n", prefix);
	while (xccdf_profile_iterator_has_more(prof_it)) {
		struct xccdf_profile * prof = xccdf_profile_iterator_next(prof_it);
		printf("%s\t%s%s\n", prefix,
			xccdf_profile_get_abstract(prof) ? "(abstract) " : "",
			xccdf_profile_get_id(prof));
	}
	xccdf_profile_iterator_free(prof_it);
}

static inline void _print_xccdf_referenced_files(struct xccdf_policy_model *policy_model, const char *prefix)
{
	struct oscap_file_entry_list *referenced_files = xccdf_policy_model_get_systems_and_files(policy_model);
	struct oscap_file_entry_iterator *files_it = oscap_file_entry_list_get_files(referenced_files);
	printf("%sReferenced check files:\n", prefix);
	while (oscap_file_entry_iterator_has_more(files_it)) {
		struct oscap_file_entry *file_entry = (struct oscap_file_entry *) oscap_file_entry_iterator_next(files_it);
		printf("%s\t%s\n", prefix, oscap_file_entry_get_file(file_entry));
		printf("%s\t\tsystem: %s\n", prefix, oscap_file_entry_get_system(file_entry));
	}
	oscap_file_entry_iterator_free(files_it);
	oscap_file_entry_list_free(referenced_files);
}

static inline void _print_xccdf_testresults(struct xccdf_benchmark *bench, const char *prefix)
{
	struct xccdf_result_iterator *res_it = xccdf_benchmark_get_results(bench);
	if (xccdf_result_iterator_has_more(res_it))
		printf("%sTest Results:\n", prefix);
	while (xccdf_result_iterator_has_more(res_it)) {
		struct xccdf_result *test_result = xccdf_result_iterator_next(res_it);
		printf("%s\t%s\n", prefix, xccdf_result_get_id(test_result));
	}
	xccdf_result_iterator_free(res_it);
}

static inline void _print_xccdf_benchmark(struct xccdf_benchmark *bench, const char *prefix)
{
	_print_xccdf_status(xccdf_benchmark_get_status_current(bench), prefix);
	printf("%sResolved: %s\n", prefix, xccdf_benchmark_get_resolved(bench) ? "true" : "false");
	_print_xccdf_profiles(xccdf_benchmark_get_profiles(bench), prefix);

	struct xccdf_policy_model *policy_model = xccdf_policy_model_new(bench);
	_print_xccdf_referenced_files(policy_model, prefix);
	_print_xccdf_testresults(bench, prefix);

	xccdf_policy_model_free(policy_model);
	// xccdf_benchmark_free not needed, it si already freed by the policy!
}

static inline void _print_xccdf_tailoring(struct oscap_source *source, const char *prefix)
{
	struct xccdf_tailoring *tailoring = xccdf_tailoring_import_source(source, NULL);
	if (tailoring == NULL) {
		return;
	}
	printf("%sBenchmark Hint: %s\n", prefix, xccdf_tailoring_get_benchmark_ref(tailoring));
	_print_xccdf_profiles(xccdf_tailoring_get_profiles(tailoring), prefix);
	xccdf_tailoring_free(tailoring);
}

static int app_info(const struct oscap_action *action)
{
        int result = OSCAP_ERROR;

	struct oscap_source *source = oscap_source_new_from_file(action->file);

	switch (oscap_source_get_scap_type(source)) {
	case OSCAP_DOCUMENT_OVAL_DEFINITIONS: {
		printf("Document type: OVAL Definitions\n");
		struct oval_definition_model *def_model = oval_definition_model_import_source(source);
		if(!def_model)
			goto cleanup;
		struct oval_generator *gen = oval_definition_model_get_generator(def_model);
		printf("OVAL version: %s\n", oval_generator_get_core_schema_version(gen));
		printf("Generated: %s\n", oval_generator_get_timestamp(gen));
		print_time(action->file);
		oval_definition_model_free(def_model);
	}
	break;
	case OSCAP_DOCUMENT_OVAL_VARIABLES: {
		printf("Document type: OVAL Variables\n");
		struct oval_variable_model *var_model = oval_variable_model_import_source(source);
		if(!var_model)
			goto cleanup;
		struct oval_generator *gen = oval_variable_model_get_generator(var_model);
		printf("OVAL version: %s\n", oval_generator_get_core_schema_version(gen));
		printf("Generated: %s\n", oval_generator_get_timestamp(gen));
		print_time(action->file);
		oval_variable_model_free(var_model);
	}
	break;
	case OSCAP_DOCUMENT_OVAL_DIRECTIVES: {
		printf("Document type: OVAL Directives\n");
		struct oval_directives_model *dir_model = oval_directives_model_new();
		int ret = oval_directives_model_import_source(dir_model, source);
		if(ret)
			goto cleanup;
		struct oval_generator *gen = oval_directives_model_get_generator(dir_model);
		printf("OVAL version: %s\n", oval_generator_get_core_schema_version(gen));
		printf("Generated: %s\n", oval_generator_get_timestamp(gen));
		print_time(action->file);
		oval_directives_model_free(dir_model);
	}
	break;
	case OSCAP_DOCUMENT_OVAL_SYSCHAR: {
		printf("Document type: OVAL System Characteristics\n");
		struct oval_definition_model * def_model = oval_definition_model_new();
		struct oval_syschar_model * sys_model = oval_syschar_model_new(def_model);
		int ret = oval_syschar_model_import_source(sys_model, source);
		if(ret)
			goto cleanup;
		struct oval_generator *gen = oval_syschar_model_get_generator(sys_model);
		printf("OVAL version: %s\n", oval_generator_get_core_schema_version(gen));
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
		int ret = oval_results_model_import_source(res_model, source);
		if(ret)
			goto cleanup;
		struct oval_generator *gen = oval_results_model_get_generator(res_model);
		printf("OVAL version: %s\n", oval_generator_get_core_schema_version(gen));
		printf("Generated: %s\n", oval_generator_get_timestamp(gen));
		print_time(action->file);
		oval_results_model_free(res_model);
		oval_definition_model_free(def_model);
	}
	break;
	case OSCAP_DOCUMENT_XCCDF: {
		printf("Document type: XCCDF Checklist\n");
		struct xccdf_benchmark* bench = xccdf_benchmark_import_source(source);
		if(!bench)
			goto cleanup;
		printf("Checklist version: %s\n", oscap_source_get_schema_version(source));
		print_time(action->file);

		_print_xccdf_benchmark(bench, "");
	}
	break;
	case OSCAP_DOCUMENT_CPE_LANGUAGE: {
		printf("Document type: CPE Language\n");
		print_time(action->file);
	}
	break;
	case OSCAP_DOCUMENT_CPE_DICTIONARY: {
		printf("Document type: CPE Dictionary\n");
		struct cpe_dict_model *dict_model = cpe_dict_model_import_source(source);
		if (!dict_model)
			goto cleanup;
		struct cpe_generator *gen = cpe_dict_model_get_generator(dict_model);
		if (gen != NULL) {
			printf("CPE version: %s\n", cpe_generator_get_schema_version(gen));
			printf("Generated: %s\n", cpe_generator_get_timestamp(gen));
		}
		print_time(action->file);
		cpe_dict_model_free(dict_model);
	}
	break;
	case OSCAP_DOCUMENT_SDS: {
		printf("Document type: Source Data Stream\n");
		print_time(action->file);
		struct ds_sds_session *session = ds_sds_session_new_from_source(source);
		if (session == NULL) {
			goto cleanup;
		}
		/* get collection */
		struct ds_sds_index *sds = ds_sds_session_get_sds_idx(session);
		if (!sds) {
			ds_sds_session_free(session);
			goto cleanup;
		}
		/* iterate over streams */
		struct ds_stream_index_iterator* sds_it = ds_sds_index_get_streams(sds);
		while (ds_stream_index_iterator_has_more(sds_it)) {
			struct ds_stream_index * stream = ds_stream_index_iterator_next(sds_it);

			printf("\nStream: %s\n", ds_stream_index_get_id(stream));
			printf("Generated: %s\n", ds_stream_index_get_timestamp(stream));
			printf("Version: %s\n", ds_stream_index_get_version(stream));

			printf("Checklists:\n");
			struct oscap_string_iterator* checklist_it = ds_stream_index_get_checklists(stream);
			while (oscap_string_iterator_has_more(checklist_it)) {
				const char * id = oscap_string_iterator_next(checklist_it);
				printf("\tRef-Id: %s\n", id);

				/* decompose */
				struct oscap_source *xccdf_source = ds_sds_session_select_checklist(session, ds_stream_index_get_id(stream), id, NULL);
				if (xccdf_source == NULL) {
					oscap_string_iterator_free(checklist_it);
					ds_stream_index_iterator_free(sds_it);
					ds_sds_session_free(session);
					goto cleanup;
				}

				const char *prefix = "\t\t";
				if (oscap_source_get_scap_type(xccdf_source) == OSCAP_DOCUMENT_XCCDF) {
					struct xccdf_benchmark* bench = xccdf_benchmark_import_source(xccdf_source);
					if(!bench) {
						oscap_string_iterator_free(checklist_it);
						ds_stream_index_iterator_free(sds_it);
						ds_sds_session_free(session);
						goto cleanup;
					}
					_print_xccdf_benchmark(bench, prefix);
				} else if (oscap_source_get_scap_type(xccdf_source) == OSCAP_DOCUMENT_XCCDF_TAILORING) {
					_print_xccdf_tailoring(xccdf_source, prefix);
				}
				ds_sds_session_reset(session);
			}
			oscap_string_iterator_free(checklist_it);

			printf("Checks:\n");
			struct oscap_string_iterator* checks_it = ds_stream_index_get_checks(stream);
			while (oscap_string_iterator_has_more(checks_it)) {
				const char * id = oscap_string_iterator_next(checks_it);
				printf("\tRef-Id: %s\n", id);
			}
			oscap_string_iterator_free(checks_it);

			struct oscap_string_iterator* dict_it = ds_stream_index_get_dictionaries(stream);
			if (oscap_string_iterator_has_more(dict_it))
				printf("Dictionaries:\n");
			else
				printf("No dictionaries.\n");
			while (oscap_string_iterator_has_more(dict_it)) {
				const char * id = oscap_string_iterator_next(dict_it);
				printf("\tRef-Id: %s\n", id);
			}
			oscap_string_iterator_free(dict_it);
		}
		ds_stream_index_iterator_free(sds_it);
		ds_sds_session_free(session);
	}
	break;
	case OSCAP_DOCUMENT_ARF: {
		printf("Document type: Result Data Stream\n");
		struct ds_rds_session *session = ds_rds_session_new_from_source(source);
		if (session == NULL) {
			goto cleanup;
		}
		struct rds_index *rds = ds_rds_session_get_rds_idx(session);
		if (!rds) {
			ds_rds_session_free(session);
			goto cleanup;
		}

		struct rds_asset_index_iterator* asset_it = rds_index_get_assets(rds);
		while (rds_asset_index_iterator_has_more(asset_it)) {
			struct rds_asset_index* asset = rds_asset_index_iterator_next(asset_it);
			printf("\nAsset: %s\n", rds_asset_index_get_id(asset));

			struct rds_report_index_iterator* report_it = rds_asset_index_get_reports(asset);
			while (rds_report_index_iterator_has_more(report_it)) {
				struct rds_report_index* report = rds_report_index_iterator_next(report_it);
				struct rds_report_request_index* request = rds_report_index_get_request(report);

				printf(" - %s -> %s\n",
					rds_report_request_index_get_id(request),
					rds_report_index_get_id(report));
			}
			rds_report_index_iterator_free(report_it);
		}
		rds_asset_index_iterator_free(asset_it);
		ds_rds_session_free(session);
	}
	break;
	case OSCAP_DOCUMENT_XCCDF_TAILORING:
		printf("Document type: XCCDF Tailoring\n");
		print_time(action->file);
		_print_xccdf_tailoring(source, "");
	break;
	case OSCAP_DOCUMENT_CVE_FEED:
		printf("Document type: CVE Feed\n");
		// TODO: Provide more info about CVE feeds
	break;
	case OSCAP_DOCUMENT_SCE_RESULT:
		printf("Document type: SCE Result File\n");
		// Currently, we do not have any SCE result file parsing capabilities.
	break;
	case OSCAP_DOCUMENT_OCIL:
		printf("Document type: OCIL Definitions file\n");
		// we don't support OCIL yet
	break;
	default:
		printf("Could not determine document type\n");
		goto cleanup;
		break;
	}

	result=OSCAP_OK;

cleanup:
	oscap_source_free(source);
	oscap_print_error();

	return result;
}

bool getopt_info(int argc, char **argv, struct oscap_action *action)
{
	if(  argc != 3) {
		oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
		return false;
	}
	action->file = argv[2];

	return true;
}
