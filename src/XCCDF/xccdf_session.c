/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
#include <string.h>
#include <limits.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>

#include <oscap.h>
#include <cpe_lang.h>
#include <OVAL/public/oval_agent_api.h>
#include <OVAL/public/oval_agent_xccdf_api.h>
#include "common/oscap_acquire.h"
#include <common/alloc.h>
#include "common/util.h"
#include "common/list.h"
#include "common/_error.h"
#include "DS/public/scap_ds.h"
#include "DS/ds_common.h"
#include "XCCDF_POLICY/public/xccdf_policy.h"
#include "XCCDF_POLICY/xccdf_policy_priv.h"
#include "XCCDF_POLICY/xccdf_policy_model_priv.h"
#include "item.h"
#include "public/xccdf_session.h"
#include "XCCDF_POLICY/public/check_engine_plugin.h"

struct oval_content_resource {
	char *href;					///< Coresponds with xccdf:check-content-ref/\@href.
	char *filename;					///< Points to the filename on the filesystem.
};

struct xccdf_session {
	const char *filename;				///< File name of SCAP (SDS or XCCDF) file for this session.
	char *temp_dir;					///< Temp directory used for decomposed component files.
	struct {
		char *file;				///< Path to XCCDF File (shall differ from the filename for sds).
		struct xccdf_policy_model *policy_model;///< Active policy model.
		char *doc_version;			///< Version of parsed XCCDF file
		char *profile_id;			///< Last selected profile.
		struct xccdf_result *result;		///< XCCDF Result model.
		float base_score;			///< Basec score of the latest evaluation.
	} xccdf;
	struct {
		struct ds_sds_index *sds_idx;		///< Index of Source DataStream (only applicable for sds).
		char *user_datastream_id;		///< Datastream id requested by user (only applicable for sds).
		char *user_component_id;		///< Component id requested by user (only applicable for sds).
		char *user_benchmark_id;		///< Benchmark id requested by user (only applicable for sds).
		char *datastream_id;			///< Datastream id used (only applicable for sds).
		char *component_id;			///< Component id used (only applicable for sds).
	} ds;
	struct {
		bool fetch_remote_resources;		///< Allows download of remote resources (not applicable when user sets custom oval files)
		download_progress_calllback_t progress;	///< Callback to report progress of download.
		struct oval_content_resource **custom_resources;///< OVAL files required by user
		struct oval_content_resource **resources;///< OVAL files referenced from XCCDF
		struct oval_agent_session **agents;	///< OVAL Agent Session
		xccdf_policy_engine_eval_fn user_eval_fn;///< Custom OVAL engine callback
		char *product_cpe;			///< CPE of scanner product.
		char **result_files;			///< Path to exported OVAL Result files
	} oval;
	struct {
		char *arf_file;				///< Path to ARF file to export
		char *xccdf_file;			///< Path to XCCDF file to export
		char *report_file;			///< Path to HTML file to eport
		bool oval_results;			///< Shall be the OVAL results files exported?
		bool oval_variables;			///< Shall be the OVAL variable files exported?
		bool check_engine_plugins_results; ///< Shall the check engine plugins results be exported?
	} export;					///< Settings of Session export
	char *user_cpe;					///< Path to CPE dictionary required by user
	char *user_tailoring_file;      ///< Path to Tailoring file requested by the user
	char *user_tailoring_cid;       ///< Component ID of the Tailoring file requested by the user
	oscap_document_type_t doc_type;			///< Document type of the session file (see filename member) used.
	bool validate;					///< False value indicates to skip any XSD validation.
	bool full_validation;				///< True value indicates that every possible step will be validated by XSD.

	struct oscap_list *check_engine_plugins; ///< Extra non-OVAL check engines that may or may not have been loaded
};

static void _oval_content_resources_free(struct oval_content_resource **resources);
static void _xccdf_session_free_oval_agents(struct xccdf_session *session);
static void _xccdf_session_free_oval_result_files(struct xccdf_session *session);

static const char *oscap_productname = "cpe:/a:open-scap:oscap";
static const char *oval_sysname = "http://oval.mitre.org/XMLSchema/oval-definitions-5";

struct xccdf_session *xccdf_session_new(const char *filename)
{
	struct xccdf_session *session = (struct xccdf_session *) oscap_calloc(1, sizeof(struct xccdf_session));
	session->filename = strdup(filename);

	if (oscap_determine_document_type(filename, &(session->doc_type)) != 0) {
		xccdf_session_free(session);
		return NULL;
	}
	if (session->doc_type != OSCAP_DOCUMENT_XCCDF && session->doc_type != OSCAP_DOCUMENT_SDS) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP,
			"Session input file was determined but it isn't an XCCDF file or a source datastream.");
		xccdf_session_free(session);
		return NULL;
	}
	session->validate = true;
	session->xccdf.base_score = 0;
	session->check_engine_plugins = oscap_list_new();

	return session;
}

static void xccdf_session_unload_check_engine_plugins(struct xccdf_session *session);

void xccdf_session_free(struct xccdf_session *session)
{
	if (session == NULL)
		return;
	oscap_free(session->xccdf.profile_id);
	oscap_free(session->export.xccdf_file);
	oscap_free(session->export.report_file);
	oscap_free(session->export.arf_file);
	_xccdf_session_free_oval_result_files(session);
	xccdf_session_unload_check_engine_plugins(session);
	oscap_list_free0(session->check_engine_plugins);
	oscap_free(session->user_cpe);
	oscap_free(session->oval.product_cpe);
	_xccdf_session_free_oval_agents(session);
	_oval_content_resources_free(session->oval.custom_resources);
	_oval_content_resources_free(session->oval.resources);
	oscap_free(session->xccdf.doc_version);
	oscap_free(session->xccdf.file);
	if (session->xccdf.policy_model != NULL)
		xccdf_policy_model_free(session->xccdf.policy_model);
	oscap_free(session->ds.user_datastream_id);
	oscap_free(session->ds.user_component_id);
	oscap_free(session->ds.user_benchmark_id);
	if (session->ds.sds_idx != NULL)
		ds_sds_index_free(session->ds.sds_idx);
	if (session->temp_dir != NULL)
		oscap_acquire_cleanup_dir((char **) &(session->temp_dir));
	oscap_free(session->filename);
	oscap_free(session->user_tailoring_file);
	oscap_free(session->user_tailoring_cid);
	oscap_free(session);
}

const char *xccdf_session_get_filename(const struct xccdf_session *session)
{
	return session->filename;
}

bool xccdf_session_is_sds(const struct xccdf_session *session)
{
	return session->doc_type == OSCAP_DOCUMENT_SDS;
}

void xccdf_session_set_validation(struct xccdf_session *session, bool validate, bool full_validation)
{
	session->validate = validate;
	session->full_validation = full_validation;
}

void xccdf_session_set_datastream_id(struct xccdf_session *session, const char *datastream_id)
{
	if (session->ds.user_datastream_id != NULL)
		oscap_free(session->ds.user_datastream_id);
	session->ds.user_datastream_id = oscap_strdup(datastream_id);
	session->ds.datastream_id = session->ds.user_datastream_id;
}

const char *xccdf_session_get_datastream_id(struct xccdf_session *session)
{
	return session->ds.datastream_id;
}

void xccdf_session_set_component_id(struct xccdf_session *session, const char *component_id)
{
	if (session->ds.user_component_id != NULL)
		oscap_free(session->ds.user_component_id);
	session->ds.user_component_id = oscap_strdup(component_id);
	session->ds.component_id = session->ds.user_component_id;
}

const char *xccdf_session_get_component_id(struct xccdf_session *session)
{
	return session->ds.component_id;
}

void xccdf_session_set_benchmark_id(struct xccdf_session *session, const char *benchmark_id)
{
	if (session->ds.user_benchmark_id != NULL)
		oscap_free(session->ds.user_benchmark_id);
	session->ds.user_benchmark_id = oscap_strdup(benchmark_id);
}

const char *xccdf_session_get_benchmark_id(struct xccdf_session *session)
{
	return session->ds.user_benchmark_id;
}

void xccdf_session_set_user_cpe(struct xccdf_session *session, const char *user_cpe)
{
	oscap_free(session->user_cpe);
	session->user_cpe = oscap_strdup(user_cpe);
}

void xccdf_session_set_user_tailoring_file(struct xccdf_session *session, const char *user_tailoring_file)
{
	oscap_free(session->user_tailoring_file);
	session->user_tailoring_file = oscap_strdup(user_tailoring_file);
}

void xccdf_session_set_user_tailoring_cid(struct xccdf_session *session, const char *user_tailoring_cid)
{
	oscap_free(session->user_tailoring_cid);
	session->user_tailoring_cid = oscap_strdup(user_tailoring_cid);
}

void xccdf_session_set_remote_resources(struct xccdf_session *session, bool allowed, download_progress_calllback_t callback)
{
	session->oval.fetch_remote_resources = allowed;
	session->oval.progress = callback;
}

void xccdf_session_set_custom_oval_eval_fn(struct xccdf_session *session, xccdf_policy_engine_eval_fn eval_fn)
{
	session->oval.user_eval_fn = eval_fn;
}

bool xccdf_session_set_product_cpe(struct xccdf_session *session, const char *product_cpe)
{
	oscap_free(session->oval.product_cpe);
	session->oval.product_cpe = oscap_strdup(product_cpe);
	return true;
}

void xccdf_session_set_oval_results_export(struct xccdf_session *session, bool to_export_oval_results)
{
	session->export.oval_results = to_export_oval_results;
}

void xccdf_session_set_oval_variables_export(struct xccdf_session *session, bool to_export_oval_variables)
{
	session->export.oval_variables = to_export_oval_variables;
}

void xccdf_session_set_check_engine_plugins_results_export(struct xccdf_session *session, bool to_export_results)
{
	session->export.check_engine_plugins_results = to_export_results;
}

void xccdf_session_set_sce_results_export(struct xccdf_session *session, bool to_export_sce_results)
{
	xccdf_session_set_check_engine_plugins_results_export(session, to_export_sce_results);
}

bool xccdf_session_set_arf_export(struct xccdf_session *session, const char *arf_file)
{
	oscap_free(session->export.arf_file);
	session->export.arf_file = oscap_strdup(arf_file);
	return true;
}

bool xccdf_session_set_xccdf_export(struct xccdf_session *session, const char *xccdf_file)
{
	oscap_free(session->export.xccdf_file);
	session->export.xccdf_file = oscap_strdup(xccdf_file);
	return true;
}

bool xccdf_session_set_report_export(struct xccdf_session *session, const char *report_file)
{
	oscap_free(session->export.report_file);
	session->export.report_file = oscap_strdup(report_file);
	return true;
}

bool xccdf_session_set_profile_id(struct xccdf_session *session, const char *profile_id)
{
	if (xccdf_policy_model_get_policy_by_id(session->xccdf.policy_model, profile_id) == NULL)
		return false;
	oscap_free(session->xccdf.profile_id);
	session->xccdf.profile_id = oscap_strdup(profile_id);
	return true;
}

const char *xccdf_session_get_profile_id(struct xccdf_session *session)
{
	return session->xccdf.profile_id;
}

/**
 * Get Source DataStream index of the session.
 * @memberof xccdf_session
 * @warning This is applicable only on sessions which are SDS.
 * @return sds index
 */
struct ds_sds_index *xccdf_session_get_sds_idx(struct xccdf_session *session)
{
	if (!xccdf_session_is_sds(session))
		return NULL;

	if (session->ds.sds_idx == NULL)
		session->ds.sds_idx = ds_sds_index_import(session->filename);
	return session->ds.sds_idx;
}

int xccdf_session_load(struct xccdf_session *session)
{
	int ret = 0;

	if ((ret = xccdf_session_load_xccdf(session)) != 0)
		return ret;
	if ((ret = xccdf_session_load_cpe(session)) != 0)
		return ret;
	if ((ret = xccdf_session_load_oval(session)) != 0)
		return ret;
	if ((ret = xccdf_session_load_check_engine_plugins(session)) != 0)
		return ret;
	return xccdf_session_load_tailoring(session);
}

static int _reporter(const char *file, int line, const char *msg, void *arg)
{
	oscap_seterr(OSCAP_EFAMILY_OSCAP, "File '%s' line %d: %s", file, line, msg);
	return 0;
}

static void _validation_failed(const char *xmlfile, oscap_document_type_t doc_type, const char *version)
{
	const char *doc_name = oscap_document_type_to_string(doc_type);
	if (doc_name == NULL)
		oscap_seterr(OSCAP_EFAMILY_XML, "Unrecognized document type in %s.", xmlfile);
	else
		oscap_seterr(OSCAP_EFAMILY_XML, "Invalid %s (%s) content in %s.", doc_name, version, xmlfile);
}

int xccdf_session_load_xccdf(struct xccdf_session *session)
{
	static const char *XCCDF_XML = "xccdf.xml";
	struct xccdf_benchmark *benchmark = NULL;

	if (session->xccdf.policy_model != NULL) {
		xccdf_policy_model_free(session->xccdf.policy_model);
		session->xccdf.policy_model = NULL;
	}
	oscap_free(session->xccdf.file);
	session->xccdf.file = NULL;
	oscap_free(session->xccdf.doc_version);
	session->xccdf.doc_version = NULL;

	if (xccdf_session_is_sds(session)) {
		if (session->validate) {
			int ret;
			if ((ret = oscap_validate_document(session->filename, OSCAP_DOCUMENT_SDS, "1.2", _reporter, NULL)) != 0) {
				if (ret == 1)
					 _validation_failed(session->filename, OSCAP_DOCUMENT_SDS, "1.2");
				goto cleanup;
				}
		}
		if (session->temp_dir == NULL)
			session->temp_dir = oscap_acquire_temp_dir();
		if (session->temp_dir == NULL)
			goto cleanup;

		// We only use benchmark ID if datastream ID and/or component ID were NOT supplied.
		if (!session->ds.user_datastream_id && !session->ds.user_component_id && session->ds.user_benchmark_id) {
			if (ds_sds_index_select_checklist_by_benchmark_id(xccdf_session_get_sds_idx(session),
				session->ds.user_benchmark_id,
				(const char **) &(session->ds.datastream_id),
				(const char **) &(session->ds.component_id)) != 0) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to locate a datastream with component-ref "
					"that points to a component containing Benchmark with ID '%s'.", session->ds.user_benchmark_id);
				goto cleanup;
			}
		}
		else {
			if (session->ds.user_benchmark_id) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Not using benchmark ID ('%s') for component-ref lookup, "
					"datastream ID ('%s') and/or component-ref ID ('%s') were supplied, using them instead.",
					session->ds.user_benchmark_id,
					session->ds.user_datastream_id,
					session->ds.user_component_id);
			}

			if (ds_sds_index_select_checklist(xccdf_session_get_sds_idx(session),
				(const char **) &(session->ds.datastream_id),
				(const char **) &(session->ds.component_id)) != 0) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to locate a datastream with ID matching "
						"'%s' ID and checklist inside matching '%s' ID.",
						session->ds.user_datastream_id == NULL ? "<any>" : session->ds.user_datastream_id,
						session->ds.user_component_id == NULL ? "<any>" : session->ds.user_component_id);
				goto cleanup;
			}
		}

		if (ds_sds_decompose(session->filename, session->ds.datastream_id, session->ds.component_id, session->temp_dir, XCCDF_XML) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to decompose source datastream in '%s'.", session->filename);
			goto cleanup;
		}

		session->xccdf.file = malloc(PATH_MAX * sizeof(char));
		snprintf(session->xccdf.file, PATH_MAX, "%s/%s", session->temp_dir, XCCDF_XML);
	}
	else {
		session->xccdf.file = strdup(session->filename);
	}

	/* Validate documents */
	if (session->validate && (!xccdf_session_is_sds(session) || session->full_validation)) {
		session->xccdf.doc_version = xccdf_detect_version(session->xccdf.file);
		if (!session->xccdf.doc_version)
			goto cleanup;

		int ret;
		if ((ret = oscap_validate_document(session->xccdf.file, OSCAP_DOCUMENT_XCCDF, session->xccdf.doc_version, _reporter, NULL)) != 0) {
			if (ret==1)
				_validation_failed(session->xccdf.file, OSCAP_DOCUMENT_XCCDF, session->xccdf.doc_version);
			goto cleanup;
		}
	}

	/* Load XCCDF model and XCCDF Policy model */
	benchmark = xccdf_benchmark_import(session->xccdf.file);
	if (benchmark == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to import the XCCDF content from '%s'.", session->xccdf.file);
		goto cleanup;
	}

	/* create the policy model */
	session->xccdf.policy_model = xccdf_policy_model_new(benchmark);
cleanup:
	if (benchmark != NULL && session->xccdf.policy_model == NULL)
		xccdf_benchmark_free(benchmark);
	return session->xccdf.policy_model != NULL ? 0 : 1;
}

int xccdf_session_load_cpe(struct xccdf_session *session)
{
	int ret;
	if (session == NULL || session->xccdf.policy_model == NULL)
		return 1;

	/* Use custom CPE dict if given */
	if (session->user_cpe != NULL) {
		oscap_document_type_t cpe_doc_type;
		char* cpe_doc_version = NULL;

		if (oscap_determine_document_type(session->user_cpe, &cpe_doc_type) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Cannot determine document type of '%s'.", session->user_cpe);
			return 1;
		}

		if (cpe_doc_type == OSCAP_DOCUMENT_CPE_DICTIONARY) {
			cpe_doc_version = cpe_dict_detect_version(session->user_cpe);
		}
		else if (cpe_doc_type == OSCAP_DOCUMENT_CPE_LANGUAGE) {
			cpe_doc_version = cpe_lang_model_detect_version(session->user_cpe);
		}
		else {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Document '%s' passed "
				"as a CPE resource was not detected to be of type"
				" CPE dictionary or CPE language.\n", session->user_cpe);
			return 1;
		}

		if ((ret = oscap_validate_document(session->user_cpe, cpe_doc_type, cpe_doc_version, _reporter, NULL))) {
			if (ret == 1)
				_validation_failed(session->user_cpe, cpe_doc_type, cpe_doc_version);
			free(cpe_doc_version);
			return 1;
		}
		free(cpe_doc_version);

		xccdf_policy_model_add_cpe_autodetect(session->xccdf.policy_model, session->user_cpe);
	}

	if (xccdf_session_is_sds(session)) {
		struct ds_stream_index* stream_idx = ds_sds_index_get_stream(session->ds.sds_idx, session->ds.datastream_id);
		struct oscap_string_iterator* cpe_it = ds_stream_index_get_dictionaries(stream_idx);

		// This potentially allows us to skip yet another decompose if we are sure
		// there are no CPE dictionaries or language models inside the datastream.
		if (oscap_string_iterator_has_more(cpe_it)) {
			/* FIXME: Decomposing means that the source datastream will be parsed
			 *        into DOM even though it has already been parsed once when the
			 *        XCCDF was split from it. We should optimize this out someday!
			 */
			if (ds_sds_decompose_custom(session->filename, session->ds.datastream_id,
					session->temp_dir, "dictionaries", NULL, NULL) != 0) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't decompose CPE dictionaries from datastream '%s' "
						"from file '%s'!\n", session->ds.datastream_id, session->filename);
				oscap_string_iterator_free(cpe_it);
				return 1;
			}

			while (oscap_string_iterator_has_more(cpe_it)) {
				const char* cpe_filename = oscap_string_iterator_next(cpe_it);

				char* full_cpe_filename = malloc(PATH_MAX * sizeof(char));
				snprintf(full_cpe_filename, PATH_MAX, "%s/%s", session->temp_dir, cpe_filename);

				if (session->full_validation) {
					oscap_document_type_t cpe_doc_type;
					char* cpe_doc_version = NULL;

					if (oscap_determine_document_type(full_cpe_filename, &cpe_doc_type) != 0) {
						oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't determine document type of '%s'. "
							"This file was embedded in SDS '%s' and was split into that file as "
							"a CPE resource.\n", full_cpe_filename, session->filename);
						free(full_cpe_filename);
						oscap_string_iterator_free(cpe_it);
						return 1;
					}

					if (cpe_doc_type == OSCAP_DOCUMENT_CPE_DICTIONARY) {
						cpe_doc_version = cpe_dict_detect_version(full_cpe_filename);
					} else if (cpe_doc_type == OSCAP_DOCUMENT_CPE_LANGUAGE) {
						cpe_doc_version = cpe_lang_model_detect_version(full_cpe_filename);
					} else {
						oscap_seterr(OSCAP_EFAMILY_OSCAP, "Document '%s' that was split from SDS "
							"'%s' and passed as a CPE resource was not detected to be of type "
							"CPE dictionary or CPE language.\n", full_cpe_filename, session->filename);
						free(full_cpe_filename);
						oscap_string_iterator_free(cpe_it);
						return 1;
					}

					if ((ret = oscap_validate_document(full_cpe_filename, cpe_doc_type, cpe_doc_version, _reporter, NULL))) {
						if (ret == 1)
							_validation_failed(full_cpe_filename, cpe_doc_type, cpe_doc_version);
						free(cpe_doc_version);
						free(full_cpe_filename);
						oscap_string_iterator_free(cpe_it);
						return 1;
					}
					free(cpe_doc_version);
				}

				xccdf_policy_model_add_cpe_autodetect(session->xccdf.policy_model, full_cpe_filename);
				free(full_cpe_filename);
			}
		}
		oscap_string_iterator_free(cpe_it);
	}
	return 0;
}

static void _oval_content_resources_free(struct oval_content_resource **resources)
{
	if (resources) {
		for (int i=0; resources[i]; i++) {
			free(resources[i]->filename);
			free(resources[i]->href);
			free(resources[i]);
		}
		free(resources);
	}
}

void xccdf_session_set_custom_oval_files(struct xccdf_session *session, char **oval_filenames)
{
	_oval_content_resources_free(session->oval.custom_resources);
	session->oval.custom_resources = NULL;
	if (oval_filenames == NULL)
		return;

	struct oval_content_resource **resources = malloc(sizeof(struct oval_content_resource *));
	resources[0] = NULL;

	for (int i = 0; oval_filenames[i];) {
		resources[i] = malloc(sizeof(struct oval_content_resource));
		resources[i]->href = strdup(basename(oval_filenames[i]));
		resources[i]->filename = strdup(oval_filenames[i]);
		i++;
		resources = realloc(resources, (i + 1) * sizeof(struct oval_content_resource *));
		resources[i] = NULL;
	}
	session->oval.custom_resources = resources;
}

static int _xccdf_session_get_oval_from_model(struct xccdf_session *session)
{
	struct oval_content_resource **resources = NULL;
	struct oscap_file_entry_list *files = NULL;
	struct oscap_file_entry_iterator *files_it = NULL;
	int idx = 0;
	char *tmp_path;
	char *printable_path;
	bool fetch_option_suggested = false;
	char *xccdf_path_cpy = NULL;
	char *dir_path = NULL;

	_oval_content_resources_free(session->oval.resources);

	xccdf_path_cpy = strdup(session->xccdf.file);
	dir_path = dirname(xccdf_path_cpy);

	resources = malloc(sizeof(struct oval_content_resource *));
	resources[idx] = NULL;

	files = xccdf_policy_model_get_systems_and_files(session->xccdf.policy_model);
	files_it = oscap_file_entry_list_get_files(files);
	while (oscap_file_entry_iterator_has_more(files_it)) {
		struct oscap_file_entry *file_entry;
		struct stat sb;

		file_entry = (struct oscap_file_entry *) oscap_file_entry_iterator_next(files_it);

		// we only care about OVAL referenced files
		if (strcmp(oscap_file_entry_get_system(file_entry), oval_sysname))
			continue;

		tmp_path = malloc(PATH_MAX * sizeof(char));
		snprintf(tmp_path, PATH_MAX, "%s/%s", dir_path, oscap_file_entry_get_file(file_entry));

		if (stat(tmp_path, &sb) == 0) {
			resources[idx] = malloc(sizeof(struct oval_content_resource));
			resources[idx]->href = strdup(oscap_file_entry_get_file(file_entry));
			resources[idx]->filename = tmp_path;
			idx++;
			resources = realloc(resources, (idx + 1) * sizeof(struct oval_content_resource *));
			resources[idx] = NULL;
		}
		else {
			if (oscap_acquire_url_is_supported(oscap_file_entry_get_file(file_entry))) {
				// Strip out the 'path' for printing the url.
				printable_path = (char *) oscap_file_entry_get_file(file_entry);

				if (session->oval.fetch_remote_resources) {
					if (session->temp_dir == NULL)
						session->temp_dir = oscap_acquire_temp_dir();
					if (session->temp_dir == NULL) {
						oscap_file_entry_iterator_free(files_it);
						oscap_file_entry_list_free(files);
						free(tmp_path);
						_oval_content_resources_free(resources);
						free(xccdf_path_cpy);
						return 1;
					}

					if (session->oval.progress != NULL)
						session->oval.progress(false, "Downloading: %s ... ", printable_path);
					char *file = oscap_acquire_url_download(session->temp_dir, printable_path);
					if (file == NULL) {
						if (session->oval.progress != NULL)
							session->oval.progress(false, "error\n");
					}
					else {
						if (session->oval.progress != NULL)
							session->oval.progress(false, "ok\n");
						resources[idx] = malloc(sizeof(struct oval_content_resource));
						resources[idx]->href = strdup(printable_path);
						resources[idx]->filename = file;
						idx++;
						resources = realloc(resources, (idx + 1) * sizeof(struct oval_content_resource *));
						resources[idx] = NULL;
						free(tmp_path);
						continue;
					}
				}
				else if (!fetch_option_suggested) {
					if (session->oval.progress != NULL)
						session->oval.progress(false, "This content points out to the remote resources. "
								"Use `--fetch-remote-resources' option to download them.\n");
					fetch_option_suggested = true;
				}
			}
			else
				printable_path = tmp_path;
			if (session->oval.progress != NULL)
				session->oval.progress(true, "WARNING: Skipping %s file which is referenced from XCCDF content\n", printable_path);
			free(tmp_path);
		}
	}
	oscap_file_entry_iterator_free(files_it);
	oscap_file_entry_list_free(files);
	free(xccdf_path_cpy);
	session->oval.resources = resources;
	return 0;
}

static void _xccdf_session_free_oval_agents(struct xccdf_session *session)
{
	if (session->oval.agents != NULL) {
		for (int i=0; session->oval.agents[i]; i++) {
			struct oval_definition_model *def_model = oval_agent_get_definition_model(session->oval.agents[i]);
			oval_definition_model_free(def_model);
                        oval_agent_destroy_session(session->oval.agents[i]);
		}
                free(session->oval.agents);
		session->oval.agents = NULL;
	}
}

int xccdf_session_load_oval(struct xccdf_session *session)
{
	struct oval_content_resource **contents = NULL;

	_xccdf_session_free_oval_agents(session);

	/* Locate all OVAL files */
	if (session->oval.custom_resources == NULL) {
		/* Use OVAL files from policy model */
		if (_xccdf_session_get_oval_from_model(session) != 0)
			return 1;
	}

	contents = session->oval.custom_resources != NULL ? session->oval.custom_resources : session->oval.resources;

	/* Validate OVAL files. Only validate if the file doesn't come from a datastream
	 * or if full validation was explicitly requested.
	 */
	if (session->validate && (!xccdf_session_is_sds(session) || session->full_validation)) {
		int ret;
		for (int idx=0; contents[idx]; idx++) {
			char *doc_version;

			doc_version = oval_determine_document_schema_version((const char *) contents[idx]->filename, OSCAP_DOCUMENT_OVAL_DEFINITIONS);
			if ((ret = oscap_validate_document(contents[idx]->filename, OSCAP_DOCUMENT_OVAL_DEFINITIONS,
					(const char *) doc_version, _reporter, NULL))) {
				if (ret == 1)
					_validation_failed(contents[idx]->filename, OSCAP_DOCUMENT_OVAL_DEFINITIONS, doc_version);
				free(doc_version);
				return 1;
			}
			free(doc_version);
		}
	}

	for (int idx=0; contents[idx]; idx++) {
		/* file -> def_model */
		struct oval_definition_model *tmp_def_model = oval_definition_model_import(contents[idx]->filename);
		if (tmp_def_model == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to create OVAL definition model from: '%s'.", contents[idx]->filename);
			return 1;
		}

		/* def_model -> session */
		struct oval_agent_session *tmp_sess = oval_agent_new_session(tmp_def_model, contents[idx]->href);
		if (tmp_sess == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to create new OVAL agent session for: '%s'.", contents[idx]->href);
			oval_definition_model_free(tmp_def_model);
			return 2;
		}

		/* store our name in the generated documents */
		oval_agent_set_product_name(tmp_sess, session->oval.product_cpe != NULL ?
				session->oval.product_cpe : (char *) oscap_productname);

		/* remember sessions */
		session->oval.agents = realloc(session->oval.agents, (idx + 2) * sizeof(struct oval_agent_session *));
		session->oval.agents[idx] = tmp_sess;
		session->oval.agents[idx+1] = NULL;

		/* register session */
		if (session->oval.user_eval_fn != NULL)
			xccdf_policy_model_register_engine_and_query_callback(
					session->xccdf.policy_model, (char *) oval_sysname,
					session->oval.user_eval_fn, tmp_sess, NULL);
		else
			xccdf_policy_model_register_engine_oval(session->xccdf.policy_model, tmp_sess);
	}
	return 0;
}

int xccdf_session_load_check_engine_plugin(struct xccdf_session *session, const char *plugin_name)
{
	struct check_engine_plugin_def *plugin = check_engine_plugin_load(plugin_name);

	if (!plugin)
		return -1; // error already set

	oscap_list_add(session->check_engine_plugins, plugin);

	return check_engine_plugin_register(plugin, session->xccdf.policy_model, session->xccdf.file);
}

int xccdf_session_load_check_engine_plugins(struct xccdf_session *session)
{
	xccdf_session_unload_check_engine_plugins(session);

	const char * const *known_plugins = check_engine_plugin_get_known_plugins();

	while (*known_plugins) {
		// We do not report failure when a known plugin doesn't load properly, that's because they
		// are optional and we don't know if it's not there or if it just failed to load.
		if (xccdf_session_load_check_engine_plugin(session, *known_plugins) != 0)
			oscap_clearerr();

		known_plugins++;
	}

	return 0;
}

static void xccdf_session_unload_check_engine_plugins(struct xccdf_session *session)
{
	struct oscap_iterator *it = oscap_iterator_new(session->check_engine_plugins);

	while (oscap_iterator_has_more(it)) {
		struct check_engine_plugin_def *plugin = (struct check_engine_plugin_def *)oscap_iterator_next(it);

		check_engine_plugin_cleanup(plugin, session->xccdf.policy_model);
		check_engine_plugin_unload(plugin);
	}

	oscap_iterator_free(it);

	oscap_list_free0(session->check_engine_plugins);
	session->check_engine_plugins = oscap_list_new();
}

int xccdf_session_load_sce(struct xccdf_session *session)
{
	return xccdf_session_load_check_engine_plugins(session);
}

int xccdf_session_load_tailoring(struct xccdf_session *session)
{
	bool from_sds = false;
	char *tailoring_path = NULL;

	if (session->user_tailoring_file != NULL) {
		tailoring_path = oscap_strdup(session->user_tailoring_file);
	}
	else if (session->user_tailoring_cid != NULL) {
		static const char *TAILORING_XML = "tailoring.xml";

		if (!xccdf_session_is_sds(session)) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't use given tailoring component ID because file isn't a source datastream.");
			return 1;
		}

		// TODO: Is the "checklists" container the right one?
		if (ds_sds_decompose_custom(session->filename, session->ds.datastream_id,
				session->temp_dir, "checklists", session->user_tailoring_cid,
				TAILORING_XML) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to split component of id '%s' from the source datastream.",
				session->user_tailoring_cid);
			return 1;
		}

		tailoring_path = oscap_sprintf("%s/%s", session->temp_dir, TAILORING_XML);
		from_sds = true;
	}

	// TODO: We should warn if has a XCCDF tailoring-file hint and user isn't
	//       using that particular tailoring.

	if (tailoring_path == NULL)
		return 0; // nothing to do

	if (session->validate && (!from_sds || session->full_validation)) {
		char *xccdf_tailoring_version = xccdf_detect_version(tailoring_path);
		int ret = 0;

		if ((ret=oscap_validate_document(tailoring_path, OSCAP_DOCUMENT_XCCDF_TAILORING, xccdf_tailoring_version, _reporter, NULL))) {
			if (ret==1)
				_validation_failed(tailoring_path, OSCAP_DOCUMENT_XCCDF_TAILORING, xccdf_tailoring_version);
			free(xccdf_tailoring_version);
			free(tailoring_path);
			return 1;
		}

		free(xccdf_tailoring_version);
	}

	struct xccdf_benchmark *benchmark = xccdf_policy_model_get_benchmark(session->xccdf.policy_model);
	struct xccdf_tailoring *tailoring = xccdf_tailoring_import(tailoring_path,
		benchmark);

	free(tailoring_path);

	if (tailoring == NULL)
		return 1;

	return xccdf_policy_model_set_tailoring(session->xccdf.policy_model, tailoring) ? 0 : 1;
}

int xccdf_session_evaluate(struct xccdf_session *session)
{
	struct xccdf_policy *policy = xccdf_session_get_xccdf_policy(session);
	if (policy == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Cannot build xccdf_policy.");
		return 1;
	}

	session->xccdf.result = xccdf_policy_evaluate(policy);
	if (session->xccdf.result == NULL)
		return 1;

	/* Write results into XCCDF Test Result model */
	xccdf_result_set_benchmark_uri(session->xccdf.result, session->filename);
	struct oscap_text *title = oscap_text_new();
	oscap_text_set_text(title, "OSCAP Scan Result");
	xccdf_result_add_title(session->xccdf.result, title);

	xccdf_result_fill_sysinfo(session->xccdf.result);

	struct xccdf_model_iterator *model_it = xccdf_benchmark_get_models(xccdf_policy_model_get_benchmark(session->xccdf.policy_model));
	while (xccdf_model_iterator_has_more(model_it)) {
		struct xccdf_model *model = xccdf_model_iterator_next(model_it);
		const char *score_model = xccdf_model_get_system(model);
		struct xccdf_score *score = xccdf_policy_get_score(policy, session->xccdf.result, score_model);
		xccdf_result_add_score(session->xccdf.result, score);

		/* record default base score for later use */
		if (!strcmp(score_model, "urn:xccdf:scoring:default"))
			session->xccdf.base_score = xccdf_score_get_score(score);
	}
	xccdf_model_iterator_free(model_it);
	return 0;
}

static size_t _paramlist_size(const char **p) { size_t s = 0; if (!p) return s; while (p[s]) s += 2; return s; }

static size_t _paramlist_cpy(const char **to, const char **p) {
	size_t s = 0;
	if (!p) return s;
	for (;p && p[s]; s += 2) to[s] = p[s], to[s+1] = p[s+1];
	to[s] = p[s];
	return s;
}

static int _app_xslt(const char *infile, const char *xsltfile, const char *outfile, const char **params)
{
	char pwd[PATH_MAX];

	if (getcwd(pwd, sizeof(pwd)) == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Getcwd() failed: %s", strerror(errno));
		return 1;
	}

	/* add params oscap-version & pwd */
	const char *stdparams[] = { "oscap-version", oscap_get_version(), "pwd", pwd, NULL };
	const char *par[_paramlist_size(params) + _paramlist_size(stdparams) + 1];
	size_t s = _paramlist_cpy(par, params);
	s += _paramlist_cpy(par + s, stdparams);

	return oscap_apply_xslt(infile, xsltfile, outfile, par) == -1;
}

static inline int _xccdf_gen_report(const char *infile, const char *id, const char *outfile, const char *show, const char *oval_template, const char* sce_template, const char* profile)
{
	const char *params[] = {
		"result-id",		id,
		"show",			show,
		"profile",		profile,
		"oval-template",	oval_template,
		"sce-template",		sce_template,
		"verbosity",		"",
		"hide-profile-info",	NULL,
		NULL};

	return _app_xslt(infile, "xccdf-report.xsl", outfile, params);
}

int xccdf_session_export_xccdf(struct xccdf_session *session)
{
	if (session->export.xccdf_file == NULL && (session->export.report_file != NULL || session->export.arf_file != NULL))
	{
		if (!session->temp_dir)
			session->temp_dir = oscap_acquire_temp_dir();
		if (session->temp_dir == NULL)
			return 1;

		session->export.xccdf_file = malloc(PATH_MAX * sizeof(char));
		snprintf(session->export.xccdf_file, PATH_MAX, "%s/xccdf-result.xml", session->temp_dir);
	}

	/* Export results */
	if (session->export.xccdf_file != NULL) {
		if (session->xccdf.result == NULL) {
			// Attempt to export session before evaluation
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "No XCCDF results to export.");
			return 1;
		}
		xccdf_benchmark_add_result(xccdf_policy_model_get_benchmark(session->xccdf.policy_model),
				xccdf_result_clone(session->xccdf.result));
		xccdf_benchmark_export(xccdf_policy_model_get_benchmark(session->xccdf.policy_model), session->export.xccdf_file);

		/* validate XCCDF Results */
		if (session->validate && session->full_validation) {
			/* we assume there is a same xccdf doc_version on input and output */
			if (oscap_validate_document(session->export.xccdf_file, OSCAP_DOCUMENT_XCCDF, session->xccdf.doc_version, _reporter, NULL)) {
				_validation_failed(session->export.xccdf_file, OSCAP_DOCUMENT_XCCDF, session->xccdf.doc_version);
				return 1;
			}
		}

		/* generate report */
		if (session->export.report_file != NULL)
			_xccdf_gen_report(session->export.xccdf_file,
					xccdf_result_get_id(session->xccdf.result),
					session->export.report_file,
					"",
					(session->export.oval_results ? "%.result.xml" : ""),
					(session->export.check_engine_plugins_results ? "%.result.xml" : ""),
					session->xccdf.profile_id == NULL ? "" : session->xccdf.profile_id
			);
	}
	return 0;
}

static void _xccdf_session_free_oval_result_files(struct xccdf_session *session)
{
	if (session->oval.result_files != NULL) {
		for(int i = 0; session->oval.result_files[i] != NULL; i++)
			oscap_free(session->oval.result_files[i]);
		free(session->oval.result_files);
		session->oval.result_files = NULL;
	}
}

static bool _real_paths_equal(const char *path1, const char *path2)
{
	// Assumption of this function: path1 points to an existing file!

	// Can't use stat because one of the paths may not exist at this point.
	// The point of all this code is to prevent collisions, not to detect them.

	char *path1_rp = malloc(PATH_MAX * sizeof(char));
	char *path2_rp = malloc(PATH_MAX * sizeof(char));

	bool ret = false;

	char *path1_rp_ret = realpath(path1, path1_rp);
	char *path2_rp_ret = realpath(path2, path2_rp);

	if (path1_rp_ret != path1_rp) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Unfortunately, realpath for '%s' didn't yield results usable for path comparison!", path1);
		goto cleanup;
	}

	// We assume the first file exists, if the second file doesn't they can't be equal.
	if (path2_rp_ret != path2_rp) {
		goto cleanup;
	}

	ret = strcmp(path1_rp, path2_rp) == 0;

cleanup:
	free(path1_rp);
	free(path2_rp);

	return ret;
}

static char *_xccdf_session_get_unique_oval_result_filename(struct xccdf_session *session, struct oval_agent_session *oval_session, const char *oval_results_directory)
{
	char *escaped_url = NULL;
	const char *filename = oval_agent_get_filename(oval_session);
	if (!filename) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Given oval_session doesn't have a valid filename. "
			"Can't deduce destination where we should be exporting it!");
		return NULL;
	}

	if (oscap_acquire_url_is_supported(filename) || filename[0] == '/') {
		// We need escaping if:
		// - filename is a URL
		// - filename is an absolute path

		escaped_url = oscap_acquire_url_to_filename(filename);
		if (escaped_url == NULL)
			return NULL;
	}
	else if (!session->export.oval_results && strchr(filename, '/')) {
		// We need recursive mkdir_p if:
		// - filename is not a basename and we are exporting to a temp dir (ARF)
		// (we are assuming that the dir structure exists if we are exporting to '.')

		char *filename_cpy = oscap_sprintf("%s/%s", oval_results_directory, filename);
		const char *dirname_ = dirname(filename_cpy);
		if (ds_common_mkdir_p(dirname_) == -1) {
			oscap_seterr(OSCAP_EFAMILY_GLIBC, "Failed to create directory '%s' for OVAL result '%s'!.", dirname_, filename);
			oscap_free(filename_cpy);
			return NULL;
		}

		oscap_free(filename_cpy);
	}

	char *name = NULL;
	unsigned int suffix = 1;
	while (suffix < UINT_MAX)
	{
		name = malloc(PATH_MAX * sizeof(char));
		if (suffix == 1)
			snprintf(name, PATH_MAX, "%s/%s.result.xml", oval_results_directory, escaped_url != NULL ? escaped_url : filename);
		else
			snprintf(name, PATH_MAX, "%s/%s.result%i.xml", oval_results_directory, escaped_url != NULL ? escaped_url : filename, suffix);

		// Check if this export name conflicts with any other exported OVAL result.
		//
		// One example where a conflict can easily happen is if we have the
		// same OVAL file used for CPE platform evaluation and check evaluation.
		//
		// openscap will create 2 OVAL sessions for the same file and will try
		// to export 2 different OVAL result files to the same path.

		bool conflict_found = false;
		int i;
		for (i=0; session->oval.result_files[i]; i++) {

			// result_files[i] has to be first here, _real_paths_equal assumes
			// the first argument points to an existing file
			if (_real_paths_equal(session->oval.result_files[i], name)) {
				conflict_found = true;
				break;
			}
		}

		if (!conflict_found)
			break;

		free(name);
		name = NULL;
		++suffix;
	}

	if (escaped_url != NULL)
		free(escaped_url);

	return name;
}

static char *_xccdf_session_export_oval_result_file(struct xccdf_session *session, struct oval_agent_session *oval_session)
{
	/* get result model and session name */
	struct oval_results_model *res_model = oval_agent_get_results_model(oval_session);
	const char* oval_results_directory = NULL;

	if (session->export.oval_results == true) {
		oval_results_directory = ".";
	}
	else {
		if (!session->temp_dir)
			session->temp_dir = oscap_acquire_temp_dir();
		if (session->temp_dir == NULL)
			return NULL;

		oval_results_directory = session->temp_dir;
	}

	char *name = _xccdf_session_get_unique_oval_result_filename(session, oval_session, oval_results_directory);

	if (name == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't figure out the right filename for OVAL result file. Can't export that file!");
	}
	else {
		/* export result model to XML */
		if (oval_results_model_export(res_model, NULL, name) == -1) {
			free(name);
			return NULL;
		}

		/* validate OVAL Results */
		if (session->validate && session->full_validation) {
			char *doc_version;

			doc_version = oval_determine_document_schema_version((const char *) name, OSCAP_DOCUMENT_OVAL_RESULTS);
			if (oscap_validate_document(name, OSCAP_DOCUMENT_OVAL_RESULTS, (const char *) doc_version,
						_reporter, NULL)) {
				_validation_failed(name, OSCAP_DOCUMENT_OVAL_RESULTS, doc_version);
				free(name);
				free(doc_version);
				return NULL;
			}
			free(doc_version);
		}
	}

	return name;
}

int xccdf_session_export_oval(struct xccdf_session *session)
{
	_xccdf_session_free_oval_result_files(session);

	if ((session->export.oval_results || session->export.arf_file != NULL) && session->oval.agents) {

		int xccdf_oval_agent_count = xccdf_session_get_oval_agents_count(session);
		int cpe_oval_agent_count = xccdf_session_get_cpe_oval_agents_count(session);

		/* Export OVAL results */
		session->oval.result_files = malloc((xccdf_oval_agent_count + cpe_oval_agent_count + 1) * sizeof(char*));
		session->oval.result_files[0] = NULL;
		int i;
		for (i=0; session->oval.agents[i]; i++) {
			char *filename = _xccdf_session_export_oval_result_file(session, session->oval.agents[i]);
			if (filename == NULL) {
				_xccdf_session_free_oval_result_files(session);
				return 1;
			}

			session->oval.result_files[i] = filename;
			session->oval.result_files[i + 1] = NULL;
		}

		struct oscap_htable_iterator *cpe_it = xccdf_policy_model_get_cpe_oval_sessions(session->xccdf.policy_model);
		while (oscap_htable_iterator_has_more(cpe_it)) {
			const char *key = NULL;
			struct oval_agent_session *value = NULL;
			oscap_htable_iterator_next_kv(cpe_it, &key, (void*)&value);

			char *filename = _xccdf_session_export_oval_result_file(session, value);
			if (filename == NULL) {
				_xccdf_session_free_oval_result_files(session);
				oscap_htable_iterator_free(cpe_it);
				return 1;
			}

			session->oval.result_files[i] = filename;
			session->oval.result_files[i + 1] = NULL;

			i++;
		}
		oscap_htable_iterator_free(cpe_it);
	}

	/* Export variables */
	if (session->export.oval_variables && session->oval.agents != NULL) {
		for (int i = 0; session->oval.agents[i]; ++i) {
			int j;
			char *sname;
			struct oval_definition_model *defmod;
			struct oval_variable_model_iterator *varmod_itr;
			char *escaped_url = NULL;

			sname = (char *) oval_agent_get_filename(session->oval.agents[i]);
			if (oscap_acquire_url_is_supported(sname)) {
				escaped_url = oscap_acquire_url_to_filename(sname);
				if (escaped_url == NULL)
					return 1;
				sname = escaped_url;
			}
			defmod = oval_agent_get_definition_model(session->oval.agents[i]);
			j = 0;
			varmod_itr = oval_definition_model_get_variable_models(defmod);
			while (oval_variable_model_iterator_has_more(varmod_itr)) {
				char fname[strlen(sname) + 32];
				struct oval_variable_model *varmod;

				varmod = oval_variable_model_iterator_next(varmod_itr);
				snprintf(fname, sizeof(fname), "%s-%d.variables-%d.xml", sname, i, j++);
				oval_variable_model_export(varmod, fname);
			}
			if (escaped_url != NULL)
				free(escaped_url);
			oval_variable_model_iterator_free(varmod_itr);
		}
	}
	return 0;
}

int xccdf_session_export_check_engine_plugins(struct xccdf_session *session)
{
	if (!session->export.check_engine_plugins_results)
		return 0;

	struct oscap_iterator *it = oscap_iterator_new(session->check_engine_plugins);

	int ret = 0;
	while (oscap_iterator_has_more(it)) {
		struct check_engine_plugin_def *plugin = (struct check_engine_plugin_def *)oscap_iterator_next(it);

		if (check_engine_plugin_export_results(
				plugin,
				session->xccdf.policy_model,
				session->validate && session->full_validation,
				session->xccdf.file) != 0)
			ret = 1;
	}

	oscap_iterator_free(it);

	return ret;
}

int xccdf_session_export_sce(struct xccdf_session *session)
{
	return xccdf_session_export_check_engine_plugins(session);
}

int xccdf_session_export_arf(struct xccdf_session *session)
{
	if (session->export.arf_file != NULL) {
		char* sds_path = 0;

		if (xccdf_session_is_sds(session)) {
			sds_path = strdup(session->filename);
		}
		else {
			if (!session->temp_dir)
				session->temp_dir = oscap_acquire_temp_dir();
			if (session->temp_dir == NULL)
				return 1;

			sds_path = malloc(PATH_MAX * sizeof(char));
			snprintf(sds_path, PATH_MAX, "%s/sds.xml", session->temp_dir);
			ds_sds_compose_from_xccdf(session->filename, sds_path);
		}

		int res = ds_rds_create(sds_path, session->export.xccdf_file, (const char**)(session->oval.result_files), session->export.arf_file);
		free(sds_path);
		if (res != 0) {
			return res;
		}

		if (session->full_validation) {
			if (oscap_validate_document(session->export.arf_file, OSCAP_DOCUMENT_ARF, "1.1", _reporter, NULL)) {
				_validation_failed(session->export.arf_file, OSCAP_DOCUMENT_ARF, "1.1");
				return 1;
			}
		}
	}
	return 0;
}

OSCAP_GENERIC_GETTER(struct xccdf_policy_model *, xccdf_session, policy_model, xccdf.policy_model)
OSCAP_GENERIC_GETTER(float, xccdf_session, base_score, xccdf.base_score);

struct xccdf_policy *xccdf_session_get_xccdf_policy(const struct xccdf_session *session)
{
	if (session->xccdf.policy_model == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Cannot build xccdf_policy.");
		return NULL;
	}
	return xccdf_policy_model_get_policy_by_id(session->xccdf.policy_model, session->xccdf.profile_id);
}

unsigned int xccdf_session_get_oval_agents_count(const struct xccdf_session *session)
{
	unsigned int i = 0;
	if (session->oval.agents != NULL)
		while (session->oval.agents[i])
			i++;
	return i;
}

unsigned int xccdf_session_get_cpe_oval_agents_count(const struct xccdf_session *session)
{
	if (session->xccdf.policy_model == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Cannot build xccdf_policy.");
		return 0;
	}

	unsigned int i = 0;
	struct oscap_htable_iterator *it = xccdf_policy_model_get_cpe_oval_sessions(session->xccdf.policy_model);
	while (oscap_htable_iterator_has_more(it)) {
		oscap_htable_iterator_next(it);
		i++;
	}
	oscap_htable_iterator_free(it);

	return i;
}

bool xccdf_session_contains_fail_result(const struct xccdf_session *session)
{
	struct xccdf_rule_result_iterator *res_it = xccdf_result_get_rule_results(session->xccdf.result);
	while (xccdf_rule_result_iterator_has_more(res_it)) {
		struct xccdf_rule_result *res = xccdf_rule_result_iterator_next(res_it);
		xccdf_test_result_type_t rule_result = xccdf_rule_result_get_result(res);
		if (rule_result == XCCDF_RESULT_FAIL ||
				rule_result == XCCDF_RESULT_UNKNOWN ||
				rule_result == XCCDF_RESULT_ERROR) {
			xccdf_rule_result_iterator_free(res_it);
			return true;
		}
	}
	xccdf_rule_result_iterator_free(res_it);
	return false;
}

int xccdf_session_remediate(struct xccdf_session *session)
{
	int res = 0;
	if (session == NULL || session->xccdf.policy_model == NULL ||
			xccdf_session_get_xccdf_policy(session) == NULL ||
			session->xccdf.result == NULL)
		return 1;
	if(getenv("OSCAP_PROBE_ROOT") != NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't perform remediation in offline mode: not implemented");
		return 1;
	}
	xccdf_policy_model_unregister_engines(session->xccdf.policy_model, oval_sysname);
	if ((res = xccdf_session_load_oval(session)) != 0)
		return res;
	xccdf_result_fill_sysinfo(session->xccdf.result);
	return xccdf_policy_remediate(xccdf_session_get_xccdf_policy(session), session->xccdf.result);
}

int xccdf_session_build_policy_from_testresult(struct xccdf_session *session, const char *testresult_id)
{
	session->xccdf.result = NULL;
	struct xccdf_benchmark *benchmark = xccdf_policy_model_get_benchmark(session->xccdf.policy_model);
	struct xccdf_result *result = xccdf_benchmark_get_result_by_id(benchmark, testresult_id);
	if (result == NULL) {
		if (testresult_id == NULL)
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not find latest TestResult element.");
		else
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not find TestResult/@id=\"%s\"", testresult_id);
		return 1;
	}

	const char *profile_id = xccdf_result_get_profile(result);
	if (xccdf_session_set_profile_id(session, profile_id) == false) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP,
			"Could not find Profile/@id=\"%s\" to build policy from TestResult/@id=\"%s\"",
			profile_id, testresult_id);
		return 1;
	}
	struct xccdf_policy *xccdf_policy = xccdf_session_get_xccdf_policy(session);
	if (xccdf_policy == NULL)
		return 1;
	session->xccdf.result = xccdf_result_clone(result);
	xccdf_result_set_start_time_current(session->xccdf.result);
	xccdf_policy_add_result(xccdf_policy, session->xccdf.result);
	return 0;
}
