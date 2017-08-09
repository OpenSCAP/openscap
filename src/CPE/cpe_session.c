/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
 * Author:
 *     Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/alloc.h"
#include "common/_error.h"
#include "common/list.h"
#include "common/public/oscap.h"
#include "common/util.h"
#include "cpe_session_priv.h"
#include "CPE/public/cpe_dict.h"
#include "CPE/public/cpe_lang.h"
#include "OVAL/public/oval_agent_api.h"
#include "source/public/oscap_source.h"
#include "source/oscap_source_priv.h"

static inline bool cpe_session_add_default_cpe(struct cpe_session *session)
{
	char* cpe_dict_path = oscap_sprintf("%s/openscap-cpe-dict.xml", oscap_path_to_cpe());
	struct oscap_source *source = oscap_source_new_from_file(cpe_dict_path);
	free(cpe_dict_path);
	const bool ret = cpe_session_add_cpe_dict_source(session, source);
	oscap_source_free(source);
	return ret;
}

struct cpe_session *cpe_session_new(void)
{
	struct cpe_session *cpe = oscap_calloc(1, sizeof(struct cpe_session));
	cpe->dicts = oscap_list_new();
	cpe->lang_models = oscap_list_new();
	cpe->oval_sessions = oscap_htable_new();
	cpe->applicable_platforms = oscap_htable_new();
	cpe->thin_results = false;
	if (!cpe_session_add_default_cpe(cpe)) {
		oscap_seterr(OSCAP_EFAMILY_XCCDF, "Failed to add default CPE to newly created CPE Session.");
	}
	return cpe;
}

static inline void _xccdf_policy_destroy_cpe_oval_session(void* ptr)
{
	struct oval_agent_session* session = (struct oval_agent_session*)ptr;
	struct oval_definition_model* model = oval_agent_get_definition_model(session);
	oval_agent_destroy_session(session);
	oval_definition_model_free(model);
}

void cpe_session_free(struct cpe_session *session)
{
	if (session != NULL) {
		oscap_list_free(session->dicts, (oscap_destruct_func) cpe_dict_model_free);
		oscap_list_free(session->lang_models, (oscap_destruct_func) cpe_lang_model_free);
		oscap_htable_free(session->oval_sessions, (oscap_destruct_func) _xccdf_policy_destroy_cpe_oval_session);
		oscap_htable_free(session->applicable_platforms, NULL);
		free(session);
	}
}

static inline struct oscap_source *_lookup_source_in_cache(struct cpe_session *session, const char *prefixed_href)
{
	if (session->sources_cache == NULL) {
		return NULL;
	}
	struct oscap_source *source = oscap_htable_get(session->sources_cache, prefixed_href);
	return source;
}

void cpe_session_set_thin_results(struct cpe_session *cpe, bool thin_results)
{
	cpe->thin_results = thin_results;
}

struct oval_agent_session *cpe_session_lookup_oval_session(struct cpe_session *cpe, const char *prefixed_href)
{
	struct oval_agent_session* session = (struct oval_agent_session*)oscap_htable_get(cpe->oval_sessions, prefixed_href);

	if (session == NULL)
	{
		struct oscap_source *source = NULL;
		struct oscap_source *cached = _lookup_source_in_cache(cpe, prefixed_href);
		if (cached == NULL) {
			source = oscap_source_new_from_file(prefixed_href);
			cached = source;
		}
		struct oval_definition_model* oval_model = oval_definition_model_import_source(cached);
		oscap_source_free(source);

		if (oval_model == NULL)
		{
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't import OVAL definition model '%s' for CPE applicability checking", prefixed_href);
			return NULL;
		}

		session = oval_agent_new_session(oval_model, prefixed_href);
		if (session == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Cannot create OVAL session for '%s' for CPE applicability checking", prefixed_href);
			return NULL;
		}
		if (cpe->thin_results) {
			struct oval_results_model *res_model = oval_agent_get_results_model(session);
			struct oval_directives_model *dir_model = oval_results_model_get_directives_model(res_model);
			// This is the worst function name in existence, despite its name,
			// it's getting the oval_result_directives of the oval_directives_model.
			// You would expect oval_directives_model_getresdirs at least, but no..
			struct oval_result_directives *dir = oval_directives_model_get_defdirs(dir_model);
			oval_result_directives_set_content(dir,  OVAL_RESULT_TRUE | OVAL_RESULT_FALSE |
							OVAL_RESULT_UNKNOWN | OVAL_RESULT_NOT_EVALUATED |
							OVAL_RESULT_NOT_APPLICABLE | OVAL_RESULT_ERROR,
							OVAL_DIRECTIVE_CONTENT_THIN);
		}
		oscap_htable_add(cpe->oval_sessions, prefixed_href, session);
	}
	return session;
}

bool cpe_session_add_cpe_lang_model_source(struct cpe_session *session, struct oscap_source *source)
{
	struct cpe_lang_model *lang_model = cpe_lang_model_import_source(source);
	return oscap_list_add(session->lang_models, lang_model);
}

bool cpe_session_add_cpe_dict_source(struct cpe_session *session, struct oscap_source *source)
{
	struct cpe_dict_model *dict = cpe_dict_model_import_source(source);
	return oscap_list_add(session->dicts, dict);
}

bool cpe_session_add_cpe_autodetect_source(struct cpe_session *session, struct oscap_source *source)
{
	oscap_document_type_t doc_type = oscap_source_get_scap_type(source);
	if (doc_type == OSCAP_DOCUMENT_CPE_DICTIONARY) {
		return cpe_session_add_cpe_dict_source(session, source);
	} else if (doc_type == OSCAP_DOCUMENT_CPE_LANGUAGE) {
		return cpe_session_add_cpe_lang_model_source(session, source);
	} else {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "File '%s' wasn't detected as either CPE dictionary or "
			"CPE lang model. Can't register it to the XCCDF policy model.", oscap_source_readable_origin(source));
		return false;
	}
}

void cpe_session_set_cache(struct cpe_session *session, struct oscap_htable *sources_cache)
{
	session->sources_cache = sources_cache;
}
