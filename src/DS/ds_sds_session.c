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
#include "common/public/oscap.h"
#include "common/util.h"
#include "ds_sds_session.h"
#include "sds_index_priv.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

struct ds_sds_session {
	struct oscap_source *source;            ///< Source DataStream raw representation
	struct ds_sds_index *index;             ///< Source DataStream index
	char *temp_dir;                         ///< Temp directory used by the session
	const char *datastream_id;              ///< ID of selected datastream
	const char *checklist_id;               ///< ID of selected checklist
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
	return sds_session;
}

void ds_sds_session_free(struct ds_sds_session *sds_session)
{
	if (sds_session != NULL) {
		ds_sds_index_free(sds_session->index);
		if (sds_session->temp_dir != NULL) {
			oscap_acquire_cleanup_dir(&(sds_session->temp_dir));
		}
		oscap_free(sds_session);
	}
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

static char *ds_sds_session_get_temp_dir(struct ds_sds_session *session)
{
	if (session->temp_dir == NULL) {
		session->temp_dir = oscap_acquire_temp_dir();
	}
	return session->temp_dir;
}

const char *ds_sds_session_get_datastream_id(const struct ds_sds_session *session)
{
	return session->datastream_id;
}

const char *ds_sds_session_get_checklist_id(const struct ds_sds_session *session)
{
	return session->checklist_id;
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
	return NULL;
}
