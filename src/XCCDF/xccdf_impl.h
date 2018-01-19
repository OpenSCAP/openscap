/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
 * Copyright (C) 2010 Tresys Technology, LLC
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
 * Authors:
 *      Josh Adams <jadams@tresys.com>
 */

#ifndef XCCDF_IMPL
#define XCCDF_IMPL

#include <public/xccdf_benchmark.h>
#include <common/util.h>
#include <libxml/xmlreader.h>


#define XCCDF_DC_NAMESPACE	BAD_CAST "http://purl.org/dc/elements/1.1/"
#define XCCDF_DSIG_NAMESPACE	BAD_CAST "http://w3.org/2000/09/xmldsig#"
#define XCCDF_XHTML_NAMESPACE	BAD_CAST "http://www.w3.org/1999/xhtml"

xmlNode *xccdf_benchmark_to_dom(struct xccdf_benchmark *benchmark, xmlDocPtr doc,
				xmlNode *parent, void *user_args);
xmlNode *xccdf_item_to_dom(struct xccdf_item *item, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info *version_info);
xmlNode *xccdf_profile_note_to_dom(struct xccdf_profile_note *note, xmlDoc *doc, xmlNode *parent);
xmlNode *xccdf_fixtext_to_dom(struct xccdf_fixtext *fixtext, xmlDoc *doc, xmlNode *parent);
xmlNode *xccdf_fix_to_dom(struct xccdf_fix *fix, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info);
xmlNode *xccdf_status_to_dom(struct xccdf_status *status, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info);
xmlNode *xccdf_warning_to_dom(struct xccdf_warning *warning, xmlDoc *doc, xmlNode *parent);
xmlNode *xccdf_check_to_dom(struct xccdf_check *check, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info);
void xccdf_rule_to_dom(struct xccdf_rule *rule, xmlNode *rule_node, xmlDoc *doc, xmlNode *parent);
void xccdf_value_to_dom(struct xccdf_value *value, xmlNode *value_node, xmlDoc *doc, xmlNode *parent);
void xccdf_group_to_dom(struct xccdf_group *group, xmlNode *group_node, xmlDoc *doc, xmlNode *parent);
void xccdf_profile_to_dom(struct xccdf_profile *profile, xmlNode *profile_node, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info *version_info);
void xccdf_result_to_dom(struct xccdf_result *result, xmlNode *result_node, xmlDoc *doc, xmlNode *parent, bool use_stig_rule_id);
xmlNode *xccdf_target_identifier_to_dom(const struct xccdf_target_identifier *ti, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info);
xmlNode *xccdf_rule_result_to_dom(struct xccdf_rule_result *result, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info, struct xccdf_benchmark *benchmark, bool use_stig_rule_id);
xmlNode *xccdf_ident_to_dom(struct xccdf_ident *ident, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info);
xmlNode *xccdf_setvalue_to_dom(struct xccdf_setvalue *setvalue, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info);
xmlNode *xccdf_override_to_dom(struct xccdf_override *override, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info);


#endif
