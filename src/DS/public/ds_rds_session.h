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
#ifndef DS_RDS_SESSION_H
#define DS_RDS_SESSION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "oscap.h"
#include "oscap_source.h"
#include "scap_ds.h"

/**
 * The ds_rds_session is structure tight closely to oscap_source.
 *
 * The ds_rds_session represents opened DataStream collection file.
 * It caches certain data to facilitate common queries and use cases
 * of DataStream consumers.
 */
struct ds_rds_session;

/**
 * Create new ds_rds_session from existing oscap_source. This assumes that
 * the given oscap_source represents result DataStream (ARF). This function
 * does not own the oscap_source, but it needs it for operation.
 * @memberof ds_rds_session
 * @param source The oscap_source representing a result datastream
 * @returns newly created ds_rds_session structure
 */
struct ds_rds_session *ds_rds_session_new_from_source(struct oscap_source *source);

/**
 * Dispose ds_rds_session structure.
 * @memberof ds_rds_session
 * @param rds_session The session to dispose
 */
void ds_rds_session_free(struct ds_rds_session *rds_session);

/**
 * Get Result DataStream index
 * @memberof ds_rds_session
 * @param session Session to query RDS index from
 * @returns Result DataStream index owned by session
 */
struct rds_index *ds_rds_session_get_rds_idx(struct ds_rds_session *session);

/**
 * Set target directory for the component files
 * @memberof ds_rds_session
 * @param session The Result DataStream session
 * @param target_dir PAth to the target storage dir
 * @returns 0 on success
 */
int ds_rds_session_set_target_dir(struct ds_rds_session *session, const char *target_dir);

/**
 * Store cached component files to the hard drive
 * @memberof ds_rds_session
 * @param session The Result DataStream session
 * @returns zero on success
 */
int ds_rds_session_dump_component_files(struct ds_rds_session *session);

/**
 * Select arf:report from result DataStream and return it in form of oscap_source.
 * @memberof ds_rds_session
 * @param session The Result DataStream session
 * @param report_id The ID of arf:report. Session will find the first, if this argument is NULL.
 * this arguement is NULL.
 * @returns oscap_source owned by the ds_rds_session or NULL on error
 */
struct oscap_source *ds_rds_session_select_report(struct ds_rds_session *session, const char *report_id);

/**
 * Select arf:report-request from result DataStream and return it in form of oscap_source.
 * @memberof ds_rds_session
 * @param session The Result DataStream session
 * @param report_request_id The id of arf:report-request. Session will find the report_request
 * related (see arf:relationship) to the last selected report. If there was no report selected
 * session will error out.
 * @returns oscap_source owned by the ds_rds_session or NULL on error
 */
struct oscap_source *ds_rds_session_select_report_request(struct ds_rds_session *session, const char *report_request_id);

/**
 * Replace currently selected report with the content of the source. The previously
 * returned oscap_source assiciated with the selected report will be disposed.
 * The source will become owned by the session.
 * @memberof ds_rds_session
 * @param session The Result DataStream session
 * @param source The oscap_source to replase selected report with.
 * @returns 0 on success
 */
int ds_rds_session_replace_report_with_source(struct ds_rds_session *session, struct oscap_source *source);

/**
 * Returns HTML representation of the given result datastream
 * @memberof ds_rds_session
 * @param rds_session The ds_rds_session to build HTML from
 * @returns a buffer of HTML content that should be freed by the caller
 */
char *ds_rds_session_get_html_report(struct ds_rds_session *rds_session);

#endif
