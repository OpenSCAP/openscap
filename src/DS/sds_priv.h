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
#ifndef DS_SDS_PRIV_H
#define DS_SDS_PRIV_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include "common/public/oscap.h"
#include "common/util.h"
#include "ds_sds_session.h"


xmlNodePtr ds_sds_lookup_datastream_in_collection(xmlDocPtr doc, const char *datastream_id);

xmlNodePtr node_get_child_element(xmlNodePtr parent, const char* name);

xmlNode *containter_get_component_ref_by_id(xmlNode *container, const char *component_id);

int ds_sds_dump_component_ref(const xmlNodePtr component_ref, struct ds_sds_session *session);

int ds_sds_dump_component_ref_as(const xmlNodePtr component_ref, struct ds_sds_session *session, const char *sub_dir, const char *relative_filepath);

xmlDocPtr ds_sds_compose_xmlDoc_from_xccdf(const char *xccdf_file, oscap_document_version_t version);
xmlDocPtr ds_sds_compose_xmlDoc_from_xccdf_source(struct oscap_source *xccdf_source, oscap_document_version_t version);

xmlNodePtr lookup_component_in_collection(xmlDocPtr doc, const char *component_id);
xmlNodePtr ds_sds_find_component_ref(xmlNodePtr datastream, const char *id);

char *ds_sds_mangle_filepath(const char *filepath);

char *ds_sds_detect_version(xmlTextReader *reader);

#endif
