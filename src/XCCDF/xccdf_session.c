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

#include <oscap.h>
#include <oscap_acquire.h>
#include <common/alloc.h>
#include "common/util.h"
#include "common/_error.h"
#include "DS/public/scap_ds.h"
#include "XCCDF_POLICY/public/xccdf_policy.h"
#include "public/xccdf_session.h"

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

OSCAP_GENERIC_GETTER(struct xccdf_policy_model *, xccdf_session, policy_model, xccdf.policy_model)
