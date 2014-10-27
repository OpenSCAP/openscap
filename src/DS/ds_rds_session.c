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
#include "common/oscapxml.h"
#include "common/public/oscap.h"
#include "common/util.h"
#include "ds_common.h"
#include "ds_rds_session.h"
#include "ds_rds_session_priv.h"
#include "rds_index_priv.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"
#include "source/xslt_priv.h"

struct ds_rds_session {
	struct oscap_source *source;            ///< Result DataStream raw representation
	struct rds_index *index;                ///< Result DataStream index
	struct oscap_htable *component_sources; ///< oscap_sources for parsed contents (arf:content)
};

struct ds_rds_session *ds_rds_session_new_from_source(struct oscap_source *source)
{
	if (oscap_source_get_scap_type(source) != OSCAP_DOCUMENT_ARF) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not create Result DataStream "
				"session: File is not Result DataStream.");
		return NULL;
	}
	struct ds_rds_session *rds_session = (struct ds_rds_session *) oscap_calloc(1, sizeof(struct ds_rds_session));
	rds_session->source = source;
	rds_session->component_sources = oscap_htable_new();
	return rds_session;
}

void ds_rds_session_free(struct ds_rds_session *rds_session)
{
	if (rds_session != NULL) {
		rds_index_free(rds_session->index);
		oscap_htable_free(rds_session->component_sources, (oscap_destruct_func) oscap_source_free);
		oscap_free(rds_session);
	}
}

struct rds_index *ds_rds_session_get_rds_idx(struct ds_rds_session *session)
{
	if (session->index == NULL) {
		xmlTextReader *reader = oscap_source_get_xmlTextReader(session->source);
		if (reader == NULL) {
			return NULL;
		}
		session->index = rds_index_parse(reader);
		xmlFreeTextReader(reader);
	}
	return session->index;
}

int ds_rds_session_register_component_source(struct ds_rds_session *session, const char *content_id, struct oscap_source *component)
{
	if (!oscap_htable_add(session->component_sources, content_id, component)) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Content '%s' has already been register with Result DataStream session: %s",
				content_id, oscap_source_readable_origin(session->source));
		return -1;
	}
	return 0;
}

int ds_rds_session_dump_component_files(struct ds_rds_session *session)
{
	return ds_dump_component_sources(session->component_sources);
}

char *ds_rds_session_get_html_report(struct ds_rds_session *rds_session)
{
	const char *params[] = {
		"show",              "",
		"verbosity",         "",
		"hide-profile-info", NULL,
		"oscap-version",     oscap_get_version(),
		"pwd",               NULL,
		NULL
        };
	return oscap_source_apply_xslt_path_mem(rds_session->source, "xccdf-report.xsl", params, oscap_path_to_xslt());
}
