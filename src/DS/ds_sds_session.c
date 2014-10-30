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

#include "common/oscap_acquire.h"
#include "common/alloc.h"
#include "common/elements.h"
#include "common/_error.h"
#include "common/list.h"
#include "common/public/oscap.h"
#include "common/util.h"
#include "ds_common.h"
#include "ds_sds_session.h"
#include "ds_sds_session_priv.h"
#include "sds_index_priv.h"
#include "sds_priv.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"
#include <libgen.h>
#include <libxml/tree.h>

struct ds_sds_session {
	struct oscap_source *source;            ///< Source DataStream raw representation
	struct ds_sds_index *index;             ///< Source DataStream index
	char *temp_dir;                         ///< Temp directory managed by the session
	const char *target_dir;                 ///< Target directory for current split
	const char *datastream_id;              ///< ID of selected datastream
	const char *checklist_id;               ///< ID of selected checklist
	struct oscap_htable *component_sources;	///< oscap_source for parsed components
};

struct ds_sds_session *ds_sds_session_new_from_source(struct oscap_source *source)
{
	if (oscap_source_get_scap_type(source) != OSCAP_DOCUMENT_SDS) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not create Source DataStream "
				"session: File is not Source DataStream.");
		return NULL;
	}
	struct ds_sds_session *sds_session = (struct ds_sds_session *) oscap_calloc(1, sizeof(struct ds_sds_session));
	sds_session->source = source;
	sds_session->component_sources = oscap_htable_new();
	return sds_session;
}

void ds_sds_session_free(struct ds_sds_session *sds_session)
{
	if (sds_session != NULL) {
		ds_sds_index_free(sds_session->index);
		if (sds_session->temp_dir != NULL) {
			oscap_acquire_cleanup_dir(&(sds_session->temp_dir));
		}
		oscap_htable_free(sds_session->component_sources, (oscap_destruct_func) oscap_source_free);
		oscap_free(sds_session);
	}
}

void ds_sds_session_reset(struct ds_sds_session *session)
{
	session->checklist_id = NULL;
	session->datastream_id = NULL;
	session->target_dir = NULL;
	oscap_htable_free(session->component_sources, (oscap_destruct_func) oscap_source_free);
	session->component_sources = oscap_htable_new();
}

struct ds_sds_index *ds_sds_session_get_sds_idx(struct ds_sds_session *session)
{
	if (session->index == NULL) {
		xmlTextReader *reader = oscap_source_get_xmlTextReader(session->source);
		if (reader == NULL) {
			return NULL;
		}
		session->index = ds_sds_index_parse(reader);
		xmlFreeTextReader(reader);
	}
	return session->index;
}

static const char *ds_sds_session_get_temp_dir(struct ds_sds_session *session)
{
	if (session->temp_dir == NULL) {
		session->temp_dir = oscap_acquire_temp_dir();
	}
	return session->temp_dir;
}

const char *ds_sds_session_get_target_dir(struct ds_sds_session *session)
{
	if (session->target_dir == NULL) {
		session->target_dir = ds_sds_session_get_temp_dir(session);
	}
	return session->target_dir;
}

int ds_sds_session_set_target_dir(struct ds_sds_session *session, const char *target_dir)
{
	if (session->target_dir == NULL) {
		session->target_dir = target_dir;
	}
	if (oscap_streq(session->target_dir, target_dir)) {
		return 0;
	} else {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Internal Error: Not implemented: Could not reset DataStream target_session in session.");
		return 1;
	}
}

int ds_sds_session_set_datastream_id(struct ds_sds_session *session, const char *datastream_id)
{
	if (session->datastream_id == NULL) {
		session->datastream_id = datastream_id;
	}
	if (oscap_streq(session->datastream_id, datastream_id)) {
		return 0;
	} else {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Internal Error: Not implemented: Could not rewrite datastream_id in the session.");
		return 1;
	}
}

const char *ds_sds_session_get_datastream_id(const struct ds_sds_session *session)
{
	return session->datastream_id;
}

const char *ds_sds_session_get_checklist_id(const struct ds_sds_session *session)
{
	return session->checklist_id;
}

struct oscap_htable *ds_sds_session_get_component_sources(struct ds_sds_session *session)
{
	return session->component_sources;
}

struct oscap_source *ds_sds_session_select_checklist(struct ds_sds_session *session, const char *datastream_id, const char *component_id, const char *benchmark_id)
{
	session->datastream_id = datastream_id;
	session->checklist_id = component_id;

	// We only use benchmark ID if datastream ID and/or component ID were NOT supplied.
	if (!datastream_id && !component_id && benchmark_id) {
		if (ds_sds_index_select_checklist_by_benchmark_id(ds_sds_session_get_sds_idx(session), 	benchmark_id,
				(const char **) &(session->datastream_id), (const char **) &(session->checklist_id)) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to locate a datastream with component-ref "
				"that points to a component containing Benchmark with ID '%s'.", benchmark_id);
			return NULL;
		}
	}
	else {
		if (benchmark_id) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Not using benchmark ID ('%s') for component-ref lookup, "
				"datastream ID ('%s') and/or component-ref ID ('%s') were supplied, using them instead.",
				benchmark_id, datastream_id, component_id);
		}

		if (ds_sds_index_select_checklist(ds_sds_session_get_sds_idx(session), (const char **) &(session->datastream_id),
				(const char **) &(session->checklist_id)) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to locate a datastream with ID matching "
					"'%s' ID and checklist inside matching '%s' ID.",
					datastream_id == NULL ? "<any>" : datastream_id,
					component_id == NULL ? "<any>" : component_id);
			return NULL;
		}
	}
	if (ds_sds_session_register_component_with_dependencies(session, "checklists", session->checklist_id, "xccdf.xml") != 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not extract %s with all dependencies from datastream.", session->checklist_id);
		return NULL;
	}
	struct oscap_source *xccdf = oscap_htable_get(session->component_sources, "xccdf.xml");
	if (xccdf == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Internal error: Could not acquire handle to xccdf.xml source.\n");
	}
	return xccdf;
}

struct oscap_source *ds_sds_session_select_tailoring(struct ds_sds_session *session, const char *component_id)
{
	if (ds_sds_session_register_component_with_dependencies(session, "checklists", component_id, "tailoring.xml") != 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not extract %s with all dependencies from datastream.", component_id);
		return NULL;
	}
	struct oscap_source *tailoring = oscap_htable_get(session->component_sources, "tailoring.xml");
	if (tailoring == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Internal error: Could not acquire handle to tailoring.xml source.\n");
	}
	return tailoring;
}

xmlNode *ds_sds_session_get_selected_datastream(struct ds_sds_session *session)
{
	xmlDoc *doc = oscap_source_get_xmlDoc(session->source);
	xmlNode *datastream = ds_sds_lookup_datastream_in_collection(doc, session->datastream_id);
	if (datastream == NULL) {
		const char* error = session->datastream_id ?
			oscap_sprintf("Could not find any datastream of id '%s'", session->datastream_id) :
			oscap_sprintf("Could not find any datastream inside the file");
		oscap_seterr(OSCAP_EFAMILY_XML, error);
		oscap_free(error);
	}
	return datastream;
}

xmlDoc *ds_sds_session_get_xmlDoc(struct ds_sds_session *session)
{
	return oscap_source_get_xmlDoc(session->source);
}

int ds_sds_session_register_component_source(struct ds_sds_session *session, const char *relative_filepath, struct oscap_source *component)
{
	if (!oscap_htable_add(session->component_sources, relative_filepath, component)) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "File %s has already been register with Source DataStream session: %s",
			relative_filepath, oscap_source_readable_origin(session->source));
		return -1;
	}
	return 0;
}

struct oscap_source *ds_sds_session_get_component_by_href(struct ds_sds_session *session, const char *href)
{
	struct oscap_source *component = oscap_htable_get(session->component_sources, href);
	return component;
}

int ds_sds_session_register_component_with_dependencies(struct ds_sds_session *session, const char *container_name, const char *component_id, const char *target_filename)
{
	xmlNode *datastream = ds_sds_session_get_selected_datastream(session);
	if (!datastream) {
		return -1;
	}

	xmlNodePtr container = node_get_child_element(datastream, container_name);
	if (!container) {
		if (ds_sds_session_get_datastream_id(session) == NULL)
			oscap_seterr(OSCAP_EFAMILY_XML, "No '%s' container element found in file '%s' in the first datastream.",
					container_name, oscap_source_readable_origin(session->source));
		else
			oscap_seterr(OSCAP_EFAMILY_XML, "No '%s' container element found in file '%s' in datastream of id '%s'.",
					container_name, oscap_source_readable_origin(session->source), ds_sds_session_get_datastream_id(session));
		return -1;
	}

	int res = -1;
	xmlNode *component_ref = containter_get_component_ref_by_id(container, component_id);
	if (component_ref != NULL) {
		if (target_filename == NULL) {
			res = ds_sds_dump_component_ref(component_ref, session);
		} else {
			res = ds_sds_dump_component_ref_as(component_ref, session, ds_sds_session_get_target_dir(session), target_filename);
		}
	}
	return res;
}

int ds_sds_session_dump_component_files(struct ds_sds_session *session)
{
	return ds_dump_component_sources(session->component_sources);
}
