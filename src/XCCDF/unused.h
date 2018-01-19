/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#pragma once

#ifndef OPENSCAP_XCCDF_UNUSED_H_
#define OPENSCAP_XCCDF_UNUSED_H_


// Prototypes to supress compiler warnings
// these funcs are automatically generated but useless
struct xccdf_warning_iterator *xccdf_profile_get_warnings(const struct xccdf_profile *profile);
const char *xccdf_benchmark_get_extends(const struct xccdf_benchmark *);
bool xccdf_benchmark_get_hidden(const struct xccdf_benchmark *benchmark);
bool xccdf_benchmark_get_selected(const struct xccdf_benchmark *benchmark);
bool xccdf_benchmark_get_multiple(const struct xccdf_benchmark *benchmark);
bool xccdf_benchmark_get_prohibit_changes(const struct xccdf_benchmark *benchmark);
bool xccdf_benchmark_get_abstract(const struct xccdf_benchmark *benchmark);
bool xccdf_benchmark_get_interactive(const struct xccdf_benchmark *benchmark);
bool xccdf_benchmark_set_selected(struct xccdf_benchmark *item, bool newval);
bool xccdf_benchmark_set_abstract(struct xccdf_benchmark *item, bool newval);
bool xccdf_benchmark_set_hidden(struct xccdf_benchmark *item, bool newval);
bool xccdf_benchmark_set_interactive(struct xccdf_benchmark *item, bool newval);
bool xccdf_benchmark_set_multiple(struct xccdf_benchmark *item, bool newval);
bool xccdf_benchmark_set_prohibit_changes(struct xccdf_benchmark *item, bool newval);
bool xccdf_group_get_resolved(const struct xccdf_group *group);
bool xccdf_group_get_multiple(const struct xccdf_group *group);
bool xccdf_group_get_interactive(const struct xccdf_group *group);
bool xccdf_group_set_resolved(struct xccdf_group *item, bool newval);
bool xccdf_group_set_interactive(struct xccdf_group *item, bool newval);
bool xccdf_group_set_multiple(struct xccdf_group *item, bool newval);
bool xccdf_rule_set_resolved(struct xccdf_rule *item, bool newval);
bool xccdf_rule_get_resolved(const struct xccdf_rule *rule);
bool xccdf_rule_get_interactive(const struct xccdf_rule *rule);
bool xccdf_value_get_multiple(const struct xccdf_value *value);
bool xccdf_value_set_resolved(struct xccdf_value *item, bool newval);
bool xccdf_value_set_selected(struct xccdf_value *item, bool newval);
bool xccdf_value_get_resolved(const struct xccdf_value *value);
bool xccdf_value_get_selected(const struct xccdf_value *value);
bool xccdf_profile_get_resolved(const struct xccdf_profile *profile);
bool xccdf_profile_get_hidden(const struct xccdf_profile *profile);
bool xccdf_profile_get_selected(const struct xccdf_profile *profile);
bool xccdf_profile_get_multiple(const struct xccdf_profile *profile);
bool xccdf_profile_get_interactive(const struct xccdf_profile *profile);
bool xccdf_profile_set_resolved(struct xccdf_profile *item, bool newval);
bool xccdf_profile_set_selected(struct xccdf_profile *item, bool newval);
bool xccdf_profile_set_hidden(struct xccdf_profile *item, bool newval);
bool xccdf_profile_set_interactive(struct xccdf_profile *item, bool newval);
bool xccdf_profile_set_multiple(struct xccdf_profile *item, bool newval);
struct xccdf_item *xccdf_profile_get_parent(const struct xccdf_profile *profile);
struct oscap_text_iterator *xccdf_benchmark_get_question(const struct xccdf_benchmark *benchmark);
const char *xccdf_profile_get_cluster_id(const struct xccdf_profile *profile);
const char *xccdf_benchmark_get_cluster_id(const struct xccdf_benchmark *benchmark);
struct oscap_text_iterator *xccdf_profile_get_rationale(const struct xccdf_profile *profile);
struct oscap_text_iterator *xccdf_benchmark_get_rationale(const struct xccdf_benchmark *benchmark);
struct oscap_text_iterator *xccdf_value_get_rationale(const struct xccdf_value *value);
struct oscap_text_iterator *xccdf_profile_get_question(const struct xccdf_profile *profile);
struct oscap_string_iterator *xccdf_value_get_platforms(const struct xccdf_value *value);
struct xccdf_benchmark *xccdf_benchmark_get_benchmark(const struct xccdf_benchmark *benchmark);
struct xccdf_item *xccdf_benchmark_get_parent(const struct xccdf_benchmark *benchmark);
float xccdf_profile_get_weight(const struct xccdf_profile *profile);
float xccdf_benchmark_get_weight(const struct xccdf_benchmark *benchmark);
float xccdf_value_get_weight(const struct xccdf_value *value);
struct oscap_text_iterator *xccdf_result_get_question(const struct xccdf_result *item);
struct oscap_text_iterator *xccdf_result_get_rationale(const struct xccdf_result *item);
struct oscap_text_iterator *xccdf_result_get_description(const struct xccdf_result *item);
struct oscap_reference_iterator *xccdf_result_get_references(const struct xccdf_result *item);
struct xccdf_warning_iterator *xccdf_result_get_warnings(const struct xccdf_result *item);
const char *xccdf_result_get_cluster_id(const struct xccdf_result *item);
const char *xccdf_result_get_version_update(const struct xccdf_result *item);
const char *xccdf_result_get_version_time(const struct xccdf_result *item);
float xccdf_result_get_weight(const struct xccdf_result *item);
struct xccdf_item *xccdf_result_get_parent(const struct xccdf_result *item);
const char *xccdf_result_get_extends(const struct xccdf_result *item);
bool xccdf_result_get_resolved(const struct xccdf_result *item);
bool xccdf_result_get_hidden(const struct xccdf_result *item);
bool xccdf_result_get_selected(const struct xccdf_result *item);
bool xccdf_result_get_multiple(const struct xccdf_result *item);
bool xccdf_result_get_prohibit_changes(const struct xccdf_result *item);
bool xccdf_result_get_abstract(const struct xccdf_result *item);
bool xccdf_result_get_interactive(const struct xccdf_result *item);
bool xccdf_result_set_abstract(struct xccdf_result *item, bool newval);
bool xccdf_result_set_hidden(struct xccdf_result *item, bool newval);
bool xccdf_result_set_interactive(struct xccdf_result *item, bool newval);
bool xccdf_result_set_multiple(struct xccdf_result *item, bool newval);
bool xccdf_result_set_prohibit_changes(struct xccdf_result *item, bool newval);
bool xccdf_result_set_resolved(struct xccdf_result *item, bool newval);
bool xccdf_result_set_selected(struct xccdf_result *item, bool newval);
bool xccdf_item_get_resolved(const struct xccdf_item *item);
bool xccdf_item_get_multiple(const struct xccdf_item *item);
bool xccdf_item_set_multiple(struct xccdf_item *item, bool newval);
bool xccdf_item_set_resolved(struct xccdf_item *item, bool newval);
bool xccdf_benchmark_set_weight(struct xccdf_benchmark *item, xccdf_numeric newval);
bool xccdf_profile_set_weight(struct xccdf_profile *item, xccdf_numeric newval);
bool xccdf_value_set_weight(struct xccdf_value *item, xccdf_numeric newval);
bool xccdf_result_set_weight(struct xccdf_result *item, xccdf_numeric newval);
bool xccdf_result_set_cluster_id(struct xccdf_result *item, const char *newval);
bool xccdf_result_set_extends(struct xccdf_result *item, const char *newval);
bool xccdf_result_set_version_time(struct xccdf_result *item, const char *newval);
bool xccdf_result_set_version_update(struct xccdf_result *item, const char *newval);
bool xccdf_profile_set_cluster_id(struct xccdf_profile *item, const char *newval);
bool xccdf_benchmark_set_cluster_id(struct xccdf_benchmark *item, const char *newval);
bool xccdf_benchmark_set_extends(struct xccdf_benchmark *item, const char *newval);
bool xccdf_benchmark_add_question(struct xccdf_benchmark *item, struct oscap_text *newval);
bool xccdf_benchmark_add_rationale(struct xccdf_benchmark *item, struct oscap_text *newval);
bool xccdf_benchmark_add_warning(struct xccdf_benchmark *item, struct xccdf_warning *newval);
bool xccdf_profile_add_question(struct xccdf_profile *item, struct oscap_text *newval);
bool xccdf_profile_add_rationale(struct xccdf_profile *item, struct oscap_text *newval);
bool xccdf_profile_add_warning(struct xccdf_profile *item, struct xccdf_warning *newval);
bool xccdf_value_add_rationale(struct xccdf_value *item, struct oscap_text *newval);
bool xccdf_value_add_platform(struct xccdf_value *item, const char *newval);
bool xccdf_result_add_description(struct xccdf_result *item, struct oscap_text *newval);
bool xccdf_result_add_platform(struct xccdf_result *item, const char *newval);
bool xccdf_result_add_question(struct xccdf_result *item, struct oscap_text *newval);
bool xccdf_result_add_rationale(struct xccdf_result *item, struct oscap_text *newval);
bool xccdf_result_add_reference(struct xccdf_result *item, struct oscap_reference *newval);
bool xccdf_result_add_status(struct xccdf_result *item, struct xccdf_status *newval);
bool xccdf_result_add_dc_status(struct xccdf_result *item, struct oscap_reference *newval);
bool xccdf_result_add_warning(struct xccdf_result *item, struct xccdf_warning *newval);
bool xccdf_rule_set_interactive(struct xccdf_rule *item, bool newval);
bool xccdf_item_get_interactive(const struct xccdf_item *item);
bool xccdf_item_set_interactive(struct xccdf_item *item, bool newval);


#endif
