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
#include "rds_priv.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"
#include "source/xslt_priv.h"

struct ds_rds_session {
	struct oscap_source *source;            ///< Result DataStream raw representation
	struct rds_index *index;                ///< Result DataStream index
	const char *target_dir;			///< Target directory for current split
	const char *report_id;			///< Last selected report ID
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

xmlDoc *ds_rds_session_get_xmlDoc(struct ds_rds_session *session)
{
	return oscap_source_get_xmlDoc(session->source);
}

const char *ds_rds_session_get_target_dir(struct ds_rds_session *session)
{
	return session->target_dir;
}

int ds_rds_session_set_target_dir(struct ds_rds_session *session, const char *target_dir)
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

struct oscap_source *ds_rds_session_select_report(struct ds_rds_session *session, const char *report_id)
{
	if (report_id == NULL || !oscap_streq(session->report_id, report_id)) {
		session->report_id = report_id;
		if (rds_index_select_report(ds_rds_session_get_rds_idx(session), &(session->report_id)) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to locate a report with ID matching '%s' ID.",
					session->report_id == NULL ? "<any>" : session->report_id);
			return NULL;
		}
		if (ds_rds_dump_arf_content(session, "reports", "report", session->report_id) != 0) {
			return NULL;
		}
	}
	return oscap_htable_get(session->component_sources, session->report_id);
}

struct oscap_source *ds_rds_session_select_report_request(struct ds_rds_session *session, const char *report_request_id)
{
	if (report_request_id == NULL) {
		if (session->report_id == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Internal Error: Not implemented: "
					"Could not select report-request: '<any>': No report selected.");
			return NULL;
		}
		struct rds_report_index *report = rds_index_get_report(ds_rds_session_get_rds_idx(session), session->report_id);
		struct rds_report_request_index *request = rds_report_index_get_request(report);
		if (request == NULL) {
			return NULL;
		}
		report_request_id = rds_report_request_index_get_id(request);
	}
	if (ds_rds_dump_arf_content(session, "report-requests", "report-request", report_request_id) != 0) {
		return NULL;
	}
	return oscap_htable_get(session->component_sources, report_request_id);
}

int ds_rds_session_replace_report_with_source(struct ds_rds_session *session, struct oscap_source *source)
{
	xmlDoc *doc = oscap_source_get_xmlDoc(session->source);
	xmlNode *reports_node = ds_rds_lookup_container(doc, "reports");
	xmlNode *report_node = ds_rds_lookup_component(doc, "reports", "report", session->report_id);
	xmlDOMWrapCtxtPtr wrap_ctxt = xmlDOMWrapNewCtxt();
	if (xmlDOMWrapRemoveNode(wrap_ctxt, doc, report_node, 0) != 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not remove arf:report[@id='%s'] from result DataStream", session->report_id);
		return 1;
	}
	struct oscap_source *prev_source = oscap_htable_detach(session->component_sources, session->report_id);
	oscap_source_free(prev_source);
	if (ds_rds_session_register_component_source(session, session->report_id, source) != 0) {
		return 1;
	}
	return ds_rds_create_report(doc, reports_node, oscap_source_get_xmlDoc(source), session->report_id) == NULL;
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
