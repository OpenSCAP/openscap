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
 *
 */
#ifndef DS_RDS_PRIV_H
#define DS_RDS_PRIV_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/tree.h>

#include "common/public/oscap.h"
#include "common/util.h"
#include "ds_rds_session.h"
#include "source/public/oscap_source.h"

OSCAP_HIDDEN_START;

xmlNode *ds_rds_lookup_container(xmlDocPtr doc, const char *container_name);
xmlNode *ds_rds_lookup_component(xmlDocPtr doc, const char *container_name, const char *component_name, const char *id);
int ds_rds_dump_arf_content(struct ds_rds_session *session, const char *container_name, const char *component_name, const char *content_id);
struct oscap_source *ds_rds_create_source(struct oscap_source *sds_source, struct oscap_source *xccdf_result_source, struct oscap_htable *oval_result_sources, struct oscap_htable *oval_result_mapping, struct oscap_htable *arf_report_mapping, const char *target_file);
xmlNodePtr ds_rds_create_report(xmlDocPtr target_doc, xmlNodePtr reports_node, xmlDocPtr source_doc, const char* report_id);

OSCAP_HIDDEN_END;
#endif
