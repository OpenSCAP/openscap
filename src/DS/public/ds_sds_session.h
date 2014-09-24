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
#ifndef DS_SDS_SESSION_H
#define DS_SDS_SESSION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "oscap.h"
#include "oscap_source.h"
#include "scap_ds.h"

/**
 * The ds_sds_session is structure tight closely to oscap_source.
 *
 * The ds_sds_session represents resource for further processing representing
 * Source DataStream (file). The ds_sds_session can be decomposed into multiple
 * oscap_source structures.
 *
 */
struct ds_sds_session;

/**
 * Create new ds_sds_session from existing oscap_source. This assumes that
 * the given oscap_source represents source DataStream. This function does
 * not own the oscap_source, but it needs it for operation.
 * @memberof ds_sds_session
 * @param source The oscap_source representing a source datastream
 * @returns newly created ds_sds_session structure
 */
struct ds_sds_session *ds_sds_session_new_from_source(struct oscap_source *source);

/**
 * Get Source DataStream index
 * @memberof ds_sds_session
 * @param session Registry to query SDS index from
 * @returns source DataStream owned by session
 */
struct ds_sds_index *ds_sds_session_get_sds_idx(struct ds_sds_session *session);

/**
 * Dispose ds_sds_session structure.
 * @param sds_session Registry to dispose
 */
void ds_sds_session_free(struct ds_sds_session *sds_session);

/**
 * Select Checklist (XCCDF presumably) from DataStream collection. Parameters may be
 * skipped (passing NULL) to let the ds_sds_session guess them.
 * @memberof ds_sds_session
 * @param session Source DataStream session to choose from
 * @param datastream_id ID of DataStream within collection
 * @param component_id ID of (XCCDF) checklist within datastream
 * @param benchmark_id ID of Benchmark element within checklist
 * @returns XCCDF checklist in form of oscap_source
 */
struct oscap_source *ds_sds_session_select_checklist(struct ds_sds_session *session, const char *datastream_id, const char *component_id, const char *benchmark_id);

/**
 * Select XCCDF Tailoring from DataStream collection. The ds_sds_session_select_checklist
 * needs to be called first. To determine what datastream should be used within collection.
 * @memberof ds_sds_session
 * @param session Source DataStream session to choose from
 * @param component_id ID of tailoring file within checklist container
 * @returns XCCdF tailoring in form of oscap_source
 */
struct oscap_source *ds_sds_session_select_tailoring(struct ds_sds_session *session, const char *component_id);

/**
 * Set the ID of DataStream within collection to use. Note that
 * ds_sds_session_select_checklist may set this automatically.
 * @memberof ds_sds_session
 * @param session Source DataStream session
 * @param datastream_id DataStream ID to set
 * @returns 0 on success
 */
int ds_sds_session_set_datastream_id(struct ds_sds_session *session, const char *datastream_id);

/**
 * Return ID of currently selected DataStream within the DataStream collection.
 * @memberof ds_sds_session
 * @param session The Source DataStream sesssion
 * @returns ID of selected datastream or NULL
 */
const char *ds_sds_session_get_datastream_id(const struct ds_sds_session *session);

/**
 * Return ID of currently selected component representing XCCDF within the DataStream
 * @memberof ds_sds_session
 * @param session The Source DataStream session
 * @returns ID of selected component or NULL
 */
const char *ds_sds_session_get_checklist_id(const struct ds_sds_session *session);

#endif
