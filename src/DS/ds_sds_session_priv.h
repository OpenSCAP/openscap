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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
 */
#ifndef OSCAP_DS_SDS_SESSION_PRIV_H
#define OSCAP_DS_SDS_SESSION_PRIV_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/public/oscap.h"
#include "common/util.h"
#include "DS/public/scap_ds.h"
#include "DS/public/ds_sds_session.h"
#include <libxml/tree.h>


xmlNode *ds_sds_session_get_selected_datastream(struct ds_sds_session *session);
xmlDoc *ds_sds_session_get_xmlDoc(struct ds_sds_session *session);
int ds_sds_session_register_component_source(struct ds_sds_session *session, const char *relative_filepath, struct oscap_source *component);
const char *ds_sds_session_get_target_dir(struct ds_sds_session *session);
struct oscap_htable *ds_sds_session_get_component_sources(struct ds_sds_session *session);
struct oscap_htable *ds_sds_session_get_component_uris(struct ds_sds_session *session);
const char *ds_sds_session_get_readable_origin(const struct ds_sds_session *session);
bool ds_sds_session_fetch_remote_resources(struct ds_sds_session *session);
const char *ds_sds_session_local_files(struct ds_sds_session *session);
download_progress_calllback_t ds_sds_session_remote_resources_progress(struct ds_sds_session *session);

void download_progress_empty_calllback(bool warning, const char * format, ...);
#endif
