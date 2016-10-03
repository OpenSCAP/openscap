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
#include "oscap_download_cb.h"

/**
 * The ds_sds_session is structure tight closely to oscap_source.
 *
 * The ds_sds_session represents opened DataStream collection file.
 * It caches certain data to facilitate common queries and use cases
 * of DataStream consumers. The ds_sds_session caches DataStream
 * components in a form of oscap_source structure.
 *
 * The cache relates to selected XCCDF component and selected CPE
 * dictionaries. When another XCCDF is selected, the cache needs to
 * be invalidated. All cached oscap_sources are owned by ds_sds_session.
 * The reset function will invalidate user pointers to them.
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

/**
 * Get component from Source DataStream by its href. This assumes that the component
 * has been already cached by the session. You can cache component or its dependencies
 * by calling ds_sds_session_select_checklist or ds_sds_session_register_component_with_dependencies.
 * Returned oscap_source is owned by ds_sds_session.
 * @memberof ds_sds_session
 * @param session The Source DataStream session
 * @param href The href of the component. The href refers to either file name from the catalogue
 * element or the target name provided previously by caller of upper mentioned functions.
 * @returns oscap_source representing the component or NULL
 */
struct oscap_source *ds_sds_session_get_component_by_href(struct ds_sds_session *session, const char *href);

/**
 * Register component and its dependencies to internal cache. This functions extracts
 * component from selected datastream within collection and all it dependencies.
 * The components may be later queried by ds_sds_session_get_component_by_href.
 * @memberof ds_sds_session
 * @param session The Source DataStream session
 * @param container_name The name of container like: "checks", "checklists", or "dictionaries".
 * @param component_id Id of component within selected datastream
 * @param target filename or NULL
 * @returns 0 on success
 */
int ds_sds_session_register_component_with_dependencies(struct ds_sds_session *session, const char *container_name, const char *component_id, const char *target_filename);

/**
 * Store cached component files to the disc.
 * @memberof ds_sds_session
 * @param session The Source DataStream session
 * @returns 0 on success
 */
int ds_sds_session_dump_component_files(struct ds_sds_session *session);

/**
 * Set target directory for the component files
 * @memberof ds_sds_session
 * @param session The Source DataStream session
 * @param target_dir Path to the target storage dir
 * @returns 0 on success
 */
int ds_sds_session_set_target_dir(struct ds_sds_session *session, const char *target_dir);

/**
 * Reset session for further use.
 * Rationale: The ds_sds_session is not versatile structure that would allow
 * to work with all the DataStream content at once. The ds_sds_session structure
 * remembers certain selections like: selected datastream within collection,
 * selected checklist, and cached catalogue files. If caller wants to open different
 * checklist with different catalogue, they need to reset the session.
 * @memberof ds_sds_session
 * @param session The Source DataStream session to reset
 */
void ds_sds_session_reset(struct ds_sds_session *session);

/**
 * Set property of remote content.
 * @memberof ds_sds_session
 * @param session The Source DataStream Session
 * @param allowed Whether is download of remote resources allowed in this session (defaults to false)
 * @param callback used to notify user about download proceeds. This might be safely set
 * to NULL -- ignoring user notification.
 */
void ds_sds_session_set_remote_resources(struct ds_sds_session *session, bool allowed, download_progress_calllback_t callback);

/**
 * Returns HTML representation of selected checklist in form of OpenSCAP guide.
 * @memberof ds_sds_session
 * @param session The Source DataStream session to generate guide from
 * @param profile_id ID of XCCDF profile within selected checklist to
 * generate guide for
 * @returns a buffer of HTML content that should be freed by the caller
 */
char *ds_sds_session_get_html_guide(struct ds_sds_session *session, const char *profile_id);

#endif
