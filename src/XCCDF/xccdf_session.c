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

#include <oscap.h>
#include <oscap_acquire.h>
#include <cpe_lang.h>
#include <OVAL/public/oval_agent_api.h>
#include <OVAL/public/oval_agent_xccdf_api.h>
#include <common/alloc.h>
#include "common/util.h"
#include "common/_error.h"
#include "DS/public/scap_ds.h"
#include "XCCDF_POLICY/public/xccdf_policy.h"
#ifdef ENABLE_SCE
#include <sce_engine_api.h>
#endif
#include "public/xccdf_session.h"

static void _oval_content_resources_free(struct oval_content_resource **resources);
static void _xccdf_session_free_oval_agents(struct xccdf_session *session);

static const char *oscap_productname = "cpe:/a:open-scap:oscap";

struct xccdf_session *xccdf_session_new(const char *filename)
{
	struct xccdf_session *session = (struct xccdf_session *) oscap_calloc(1, sizeof(struct xccdf_session));
	session->filename = strdup(filename);

	if (oscap_determine_document_type(filename, &(session->doc_type)) != 0) {
		xccdf_session_free(session);
		return NULL;
	}
	session->validate = true;
	return session;
}

void xccdf_session_free(struct xccdf_session *session)
{
#ifdef ENABLE_SCE
	if (session->sce.parameters != NULL)
		sce_parameters_free(session->sce.parameters);
#endif
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
	if (session->ds.sds_idx != NULL)
		ds_sds_index_free(session->ds.sds_idx);
	if (session->temp_dir != NULL)
		oscap_acquire_cleanup_dir((char **) &(session->temp_dir));
	oscap_free(session->filename);
	oscap_free(session);
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

void xccdf_session_set_component_id(struct xccdf_session *session, const char *component_id)
{
	if (session->ds.user_component_id != NULL)
		oscap_free(session->ds.user_component_id);
	session->ds.user_component_id = oscap_strdup(component_id);
	session->ds.component_id = session->ds.user_component_id;
}

void xccdf_session_set_user_cpe(struct xccdf_session *session, const char *user_cpe)
{
	oscap_free(session->user_cpe);
	session->user_cpe = oscap_strdup(user_cpe);
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

/**
 * Get Source DataStream index of the session.
 * @memberof xccdf_session
 * @warning This is applicable only on sessions which are SDS.
 * @return sds index
 */
static inline struct ds_sds_index *_xccdf_session_get_sds_idx(struct xccdf_session *session)
{
	/* If you want to make this symbol public to allow users interactively
	 * select datastream and component id. Please make sure to handle validation
	 * and is_sds() check first. */
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
	return xccdf_session_load_sce(session);
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
		oscap_seterr(OSCAP_EFAMILY_XML, "Invalid %s (%s) content in %s.\n", doc_name, version, xmlfile);
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

		if (ds_sds_index_select_checklist(_xccdf_session_get_sds_idx(session),
				(const char **) &(session->ds.datastream_id),
				(const char **) &(session->ds.component_id)) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to locate a datastream with ID matching "
					"'%s' ID and checklist inside matching '%s' ID.",
					session->ds.user_datastream_id == NULL ? "<any>" : session->ds.user_datastream_id,
					session->ds.user_component_id == NULL ? "<any>" : session->ds.user_component_id);
			goto cleanup;
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
		if (strcmp(oscap_file_entry_get_system(file_entry), "http://oval.mitre.org/XMLSchema/oval-definitions-5"))
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
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to create OVAL definition model from: '%s'.\n", contents[idx]->filename);
			return 1;
		}

		/* def_model -> session */
		struct oval_agent_session *tmp_sess = oval_agent_new_session(tmp_def_model, contents[idx]->href);
		if (tmp_sess == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to create new OVAL agent session for: '%s'.\n", contents[idx]->href);
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
					session->xccdf.policy_model,
					"http://oval.mitre.org/XMLSchema/oval-definitions-5",
					session->oval.user_eval_fn, tmp_sess, NULL);
		else
			xccdf_policy_model_register_engine_oval(session->xccdf.policy_model, tmp_sess);
	}
	return 0;
}

int xccdf_session_load_sce(struct xccdf_session *session)
{
#ifdef ENABLE_SCE
	char *xccdf_pathcopy;
	if (session->sce.parameters != NULL)
		sce_parameters_free(session->sce.parameters);

	session->sce.parameters = sce_parameters_new();
	xccdf_pathcopy = oscap_strdup(session->xccdf.file);
	sce_parameters_set_xccdf_directory(session->sce.parameters, dirname(xccdf_pathcopy));
	oscap_free(xccdf_pathcopy);
	return !xccdf_policy_model_register_engine_sce(session->xccdf.policy_model, session->sce.parameters);
#else
	return 0;
#endif
}

OSCAP_GENERIC_GETTER(struct xccdf_policy_model *, xccdf_session, policy_model, xccdf.policy_model)

unsigned int xccdf_session_get_oval_agents_count(const struct xccdf_session *session)
{
	unsigned int i = 0;
	if (session->oval.agents != NULL)
		while (session->oval.agents[i])
			i++;
	return i;
}
