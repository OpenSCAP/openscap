/*
 * Open Vulnerability and Assessment Language
 * (http://oval.mitre.org/)
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 *      Maros Barabas   <mbarabas@redhat.com>
 *
 */

%module xccdf
%{
 #include "../../src/XCCDF/xccdf.h"
%}


/* Included file: ../../src/XCCDF/xccdf.h
 * Read xccdf.h for more information and 
 * documentation
 *
 */
enum xccdf_type {
        XCCDF_BENCHMARK = 0x0100, ///< Type constant for xccdf_benchmark
        XCCDF_PROFILE   = 0x0200, ///< Type constant for xccdf_profile
        XCCDF_RESULT    = 0x0400, ///< Type constant for xccdf_result
        XCCDF_RULE      = 0x1000, ///< Type constant for xccdf_rule
        XCCDF_GROUP     = 0x2000, ///< Type constant for xccdf_group
        XCCDF_VALUE     = 0x4000, ///< Type constant for xccdf_value
        /// Represents selectable items, i.e. rules and groups (see xccdf_item)
        XCCDF_CONTENT   = XCCDF_RULE | XCCDF_GROUP,
        /// Represents items as described in the XCCDF documentation (see xccdf_item)
        XCCDF_ITEM      = XCCDF_RULE | XCCDF_GROUP | XCCDF_VALUE,
        /// Represents an object, profile, result or whole benchmark (see xccdf_item)
        XCCDF_OBJECT    = XCCDF_ITEM | XCCDF_PROFILE | XCCDF_BENCHMARK | XCCDF_RESULT,
};
enum xccdf_interface_hint {
        XCCDF_IFACE_HINT_NONE,     ///< No interface hint
        XCCDF_IFACE_HINT_CHOICE,   ///< Choice from multiple values.
        XCCDF_IFACE_HINT_TEXTLINE, ///< Text line input widget.
        XCCDF_IFACE_HINT_TEXT,     ///< Textarea.
        XCCDF_IFACE_HINT_DATE,     ///< Date selection widget.
        XCCDF_IFACE_HINT_DATETIME, ///< Date and time selection widget.
};
enum xccdf_status_type {
    XCCDF_STATUS_NOT_SPECIFIED, ///< Status was not specified by benchmark.
    XCCDF_STATUS_ACCEPTED,      ///< Accepted.
    XCCDF_STATUS_DEPRECATED,    ///< Deprecated.
    XCCDF_STATUS_DRAFT,         ///< Draft item.
    XCCDF_STATUS_INCOMPLETE,    ///< The item is not complete.
    XCCDF_STATUS_INTERIM        ///< Interim.
};
enum xccdf_value_type {
        XCCDF_TYPE_NUMBER  = 1, ///< Integer.
        XCCDF_TYPE_STRING,      ///< String.
        XCCDF_TYPE_BOOLEAN,     ///< Boolean.
};
enum xccdf_operator {
        XCCDF_OPERATOR_EQUALS = 1,     ///< Equality.
        XCCDF_OPERATOR_NOT_EQUAL,      ///< Inequality.
        XCCDF_OPERATOR_GREATER,        ///< Greater than.
        XCCDF_OPERATOR_GREATER_EQUAL,  ///< Greater than or equal.
        XCCDF_OPERATOR_LESS,           ///< Less than.
        XCCDF_OPERATOR_LESS_EQUAL,     ///< Less than or equal.
        XCCDF_OPERATOR_PATTERN_MATCH   ///< Match a regular expression.
};
enum xccdf_bool_operator {
        XCCDF_OPERATOR_AND  = 0x0002,                                   ///< Logical and.
        XCCDF_OPERATOR_OR   = 0x0003,                                   ///< Logical or.
        XCCDF_OPERATOR_NOT  = 0x0100,                                   ///< Logical negation.
        XCCDF_OPERATOR_NAND = XCCDF_OPERATOR_AND | XCCDF_OPERATOR_NOT,  ///< Logical nand.
        XCCDF_OPERATOR_NOR  = XCCDF_OPERATOR_OR  | XCCDF_OPERATOR_NOT,  ///< Logical nor.
        XCCDF_OPERATOR_MASK = 0x00ff                                    ///< Mask to strip the negation away (using bitwise and).
};
enum xccdf_level {
        XCCDF_UNKNOWN = 1, ///< Unknown.
        XCCDF_INFO,        ///< Info.
        XCCDF_LOW,         ///< Low.
        XCCDF_MEDIUM,      ///< Medium.
        XCCDF_HIGH         ///< High.
};
enum xccdf_message_severity {
        XCCDF_MSG_INFO    = XCCDF_INFO,  ///< Info.
        XCCDF_MSG_WARNING = XCCDF_LOW,   ///< Warning.
        XCCDF_MSG_ERROR   = XCCDF_HIGH,  ///< Error
};
enum xccdf_role {
        XCCDF_ROLE_FULL = 1,  ///< Check the rule and let the result contriburte to the score and appear in reports
        XCCDF_ROLE_UNSCORED,  ///< Check the rule and include the result in reports, but do not include it into score computations
        XCCDF_ROLE_UNCHECKED  
};
enum xccdf_warning_category {
        XCCDF_WARNING_GENERAL = 1,    ///< General-purpose warning
        XCCDF_WARNING_FUNCTIONALITY,  ///< Warning about possible impacts to functionality
        XCCDF_WARNING_PERFORMANCE,    ///< Warning about changes to target system performance
        XCCDF_WARNING_HARDWARE,       ///< Warning about hardware restrictions or possible impacts to hardware
        XCCDF_WARNING_LEGAL,          ///< Warning about legal implications
        XCCDF_WARNING_REGULATORY,     ///< Warning about regulatory obligations
        XCCDF_WARNING_MANAGEMENT,     ///< Warning about impacts to the mgmt or administration of the target system
        XCCDF_WARNING_AUDIT,          ///< Warning about impacts to audit or logging
        XCCDF_WARNING_DEPENDENCY      ///< Warning about dependencies between this Rule and other parts of the target system
};
enum xccdf_strategy {
        XCCDF_STRATEGY_UNKNOWN,    
        XCCDF_STRATEGY_CONFIGURE,   
        XCCDF_STRATEGY_DISABLE,
        XCCDF_STRATEGY_ENABLE,  
        XCCDF_STRATEGY_PATCH,    
        XCCDF_STRATEGY_POLICY,    
        XCCDF_STRATEGY_RESTRICT,   
        XCCDF_STRATEGY_UPDATE,      
        XCCDF_STRATEGY_COMBINATION   
};
enum xccdf_test_result_type {
        XCCDF_RESULT_PASS = 1,       
        XCCDF_RESULT_FAIL,           
        XCCDF_RESULT_ERROR,          
        XCCDF_RESULT_UNKNOWN,        
        XCCDF_RESULT_NOT_APPLICABLE, 
        XCCDF_RESULT_NOT_CHECKED,     
        XCCDF_RESULT_NOT_SELECTED,    
        XCCDF_RESULT_INFORMATIONAL,   
        XCCDF_RESULT_FIXED            
};
typedef float xccdf_numeric;
struct xccdf_item;
struct xccdf_benchmark;
struct xccdf_profile;
struct xccdf_rule;
struct xccdf_group;
struct xccdf_value;
struct xccdf_result;
struct xccdf_notice;
struct xccdf_status;
struct xccdf_model;
struct xccdf_warning;
struct xccdf_select;
struct xccdf_set_value;
struct xccdf_refine_value;
struct xccdf_refine_rule;
struct xccdf_ident;
struct xccdf_check;
struct xccdf_check_content_ref;
struct xccdf_profile_note;
struct xccdf_check_import;
struct xccdf_check_export;
struct xccdf_fix;
struct xccdf_fixtext;
struct xccdf_item_iterator;
struct xccdf_item* xccdf_item_iterator_next(struct xccdf_item_iterator* it);
bool xccdf_item_iterator_has_more(struct xccdf_item_iterator* it);
struct xccdf_notice_iterator;
struct xccdf_notice* xccdf_notice_iterator_next(struct xccdf_notice_iterator* it);
bool xccdf_notice_iterator_has_more(struct xccdf_notice_iterator* it);
struct xccdf_status_iterator;
struct xccdf_status* xccdf_status_iterator_next(struct xccdf_status_iterator* it);
bool xccdf_status_iterator_has_more(struct xccdf_status_iterator* it);
struct xccdf_reference_iterator;
struct xccdf_reference* xccdf_reference_iterator_next(struct xccdf_reference_iterator* it);
bool xccdf_reference_iterator_has_more(struct xccdf_reference_iterator* it);
struct xccdf_model_iterator;
struct xccdf_model* xccdf_model_iterator_next(struct xccdf_model_iterator* it);
bool xccdf_model_iterator_has_more(struct xccdf_model_iterator* it);
struct xccdf_profile_iterator;
struct xccdf_profile* xccdf_profile_iterator_next(struct xccdf_profile_iterator* it);
bool xccdf_profile_iterator_has_more(struct xccdf_profile_iterator* it);
struct xccdf_select_iterator;
struct xccdf_select* xccdf_select_iterator_next(struct xccdf_select_iterator* it);
bool xccdf_select_iterator_has_more(struct xccdf_select_iterator* it);
struct xccdf_set_value_iterator;
struct xccdf_set_value* xccdf_set_value_iterator_next(struct xccdf_set_value_iterator* it);
bool xccdf_set_value_iterator_has_more(struct xccdf_set_value_iterator* it);
struct xccdf_refine_value_iterator;
struct xccdf_refine_value* xccdf_refine_value_iterator_next(struct xccdf_refine_value_iterator* it);
bool xccdf_refine_value_iterator_has_more(struct xccdf_refine_value_iterator* it);
struct xccdf_refine_rule_iterator;
struct xccdf_refine_rule* xccdf_refine_rule_iterator_next(struct xccdf_refine_rule_iterator* it);
bool xccdf_refine_rule_iterator_has_more(struct xccdf_refine_rule_iterator* it);
struct xccdf_ident_iterator;
struct xccdf_ident* xccdf_ident_iterator_next(struct xccdf_ident_iterator* it);
bool xccdf_ident_iterator_has_more(struct xccdf_ident_iterator* it);
struct xccdf_check_iterator;
struct xccdf_check* xccdf_check_iterator_next(struct xccdf_check_iterator* it);
bool xccdf_check_iterator_has_more(struct xccdf_check_iterator* it);
struct xccdf_check_content_ref_iterator;
struct xccdf_check_content_ref* xccdf_check_content_ref_iterator_next(struct xccdf_check_content_ref_iterator* it);
bool xccdf_check_content_ref_iterator_has_more(struct xccdf_check_content_ref_iterator* it);
struct xccdf_profile_note_iterator;
struct xccdf_profile_note* xccdf_profile_note_iterator_next(struct xccdf_profile_note_iterator* it);
bool xccdf_profile_note_iterator_has_more(struct xccdf_profile_note_iterator* it);
struct xccdf_check_import_iterator;
struct xccdf_check_import* xccdf_check_import_iterator_next(struct xccdf_check_import_iterator* it);
bool xccdf_check_import_iterator_has_more(struct xccdf_check_import_iterator* it);
struct xccdf_check_export_iterator;
struct xccdf_check_export* xccdf_check_export_iterator_next(struct xccdf_check_export_iterator* it);
bool xccdf_check_export_iterator_has_more(struct xccdf_check_export_iterator* it);
struct xccdf_fix_iterator;
struct xccdf_fix* xccdf_fix_iterator_next(struct xccdf_fix_iterator* it);
bool xccdf_fix_iterator_has_more(struct xccdf_fix_iterator* it);
struct xccdf_fixtext_iterator;
struct xccdf_fixtext* xccdf_fixtext_iterator_next(struct xccdf_fixtext_iterator* it);
bool xccdf_fixtext_iterator_has_more(struct xccdf_fixtext_iterator* it);
enum xccdf_type xccdf_item_type(const struct xccdf_item* item);
const char* xccdf_item_id(const struct xccdf_item* item);
const char* xccdf_item_title(const struct xccdf_item* item);
const char* xccdf_item_description(const struct xccdf_item* item);
const char* xccdf_item_version(const struct xccdf_item* item);
struct xccdf_item* xccdf_item_extends(const struct xccdf_item* item);
struct xccdf_item* xccdf_item_parent(const struct xccdf_item* item);
struct xccdf_status_iterator* xccdf_item_statuses(const struct xccdf_item* item);
struct xccdf_reference_iterator* xccdf_item_references(const struct xccdf_item* item);
enum xccdf_status_type xccdf_item_status_current(const struct xccdf_item* item);
struct xccdf_benchmark* xccdf_item_to_benchmark(struct xccdf_item* item);
struct xccdf_profile* xccdf_item_to_profile(struct xccdf_item* item);
struct xccdf_rule* xccdf_item_to_rule(struct xccdf_item* item);
struct xccdf_group* xccdf_item_to_group(struct xccdf_item* item);
struct xccdf_value* xccdf_item_to_value(struct xccdf_item* item);
struct xccdf_result* xccdf_item_to_result(struct xccdf_item* item);
struct xccdf_benchmark* xccdf_benchmark_new_from_file(const char* filename);
const char* xccdf_benchmark_id(const struct xccdf_benchmark* benchmark);
bool xccdf_benchmark_resolved(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_title(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_description(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_version(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_style(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_style_href(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_front_matter(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_rear_matter(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_metadata(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_plain_text(const struct xccdf_benchmark* benchmark, const char* id);
struct xccdf_item* xccdf_benchmark_item(const struct xccdf_benchmark* benchmark, const char* id);
struct xccdf_status_iterator* xccdf_benchmark_statuses(const struct xccdf_benchmark* benchmark);
struct xccdf_reference_iterator* xccdf_benchmark_references(const struct xccdf_benchmark* benchmark);
struct oscap_string_iterator*  xccdf_benchmark_platforms(const struct xccdf_benchmark* benchmark);
struct xccdf_notice_iterator*  xccdf_benchmark_notices(const struct xccdf_benchmark* benchmark);
struct xccdf_model_iterator*   xccdf_benchmark_models(const struct xccdf_benchmark* benchmark);
struct xccdf_profile_iterator* xccdf_benchmark_profiles(const struct xccdf_benchmark* benchmark);
struct xccdf_item_iterator* xccdf_benchmark_content(const struct xccdf_benchmark* benchmark);
enum xccdf_status_type xccdf_benchmark_status_current(const struct xccdf_benchmark* benchmark);
void xccdf_benchmark_delete(struct xccdf_benchmark* benchmark);
const char* xccdf_profile_id(const struct xccdf_profile* profile);
const char* xccdf_profile_title(const struct xccdf_profile* profile);
const char* xccdf_profile_description(const struct xccdf_profile* profile);
const char* xccdf_profile_version(const struct xccdf_profile* profile);
struct xccdf_profile* xccdf_profile_extends(const struct xccdf_profile* profile);
struct xccdf_benchmark* xccdf_profile_benchmark(const struct xccdf_profile* profile);
bool xccdf_profile_abstract(const struct xccdf_profile* profile);
bool xccdf_profile_prohibit_changes(const struct xccdf_profile* profile);
struct oscap_string_iterator* xccdf_profile_platforms(const struct xccdf_profile* profile);
struct xccdf_status_iterator* xccdf_profile_statuses(const struct xccdf_profile* profile);
struct xccdf_reference_iterator* xccdf_profile_references(const struct xccdf_profile* profile);
enum xccdf_status_type xccdf_profile_status_current(const struct xccdf_profile* profile);
const char* xccdf_profile_note_tag(const struct xccdf_profile* profile);
struct xccdf_select_iterator* xccdf_profile_selects(const struct xccdf_profile* profile);
struct xccdf_set_value_iterator* xccdf_profile_set_values(const struct xccdf_profile* profile);
struct xccdf_refine_value_iterator* xccdf_profile_refine_values(const struct xccdf_profile* profile);
struct xccdf_refine_rule_iterator* xccdf_profile_refine_rules(const struct xccdf_profile* profile);
const char* xccdf_rule_id(const struct xccdf_rule* rule);
const char* xccdf_rule_title(const struct xccdf_rule* rule);
const char* xccdf_rule_description(const struct xccdf_rule* rule);
const char* xccdf_rule_version(const struct xccdf_rule* rule);
const char* xccdf_rule_question(const struct xccdf_rule* rule);
const char* xccdf_rule_rationale(const struct xccdf_rule* rule);
const char* xccdf_rule_cluster_id(const struct xccdf_rule* rule);
struct xccdf_check* xccdf_rule_check(const struct xccdf_rule* rule);
float xccdf_rule_weight(const struct xccdf_rule* rule);
struct xccdf_rule* xccdf_rule_extends(const struct xccdf_rule* rule);
struct xccdf_item* xccdf_rule_parent(const struct xccdf_rule* rule);
bool xccdf_rule_abstract(const struct xccdf_rule* rule);
bool xccdf_rule_prohibit_changes(const struct xccdf_rule* rule);
bool xccdf_rule_hidden(const struct xccdf_rule* rule);
bool xccdf_rule_selected(const struct xccdf_rule* rule);
bool xccdf_rule_multiple(const struct xccdf_rule* rule);
struct oscap_string_iterator* xccdf_rule_platforms(const struct xccdf_rule* rule);
struct xccdf_status_iterator* xccdf_rule_statuses(const struct xccdf_rule* rule);
struct xccdf_reference_iterator* xccdf_rule_references(const struct xccdf_rule* rule);
enum xccdf_status_type xccdf_rule_status_current(const struct xccdf_rule* rule);
const char* xccdf_rule_impact_metric(const struct xccdf_rule* rule);
enum xccdf_role xccdf_rule_role(const struct xccdf_rule* rule);
enum xccdf_level xccdf_rule_severity(const struct xccdf_rule* rule);
struct xccdf_ident_iterator* xccdf_rule_idents(const struct xccdf_rule* rule);
struct xccdf_check_iterator* xccdf_rule_checks(const struct xccdf_rule* rule);
struct xccdf_profile_note_iterator* xccdf_rule_profile_notes(const struct xccdf_rule* rule);
struct xccdf_fix_iterator* xccdf_rule_fixes(const struct xccdf_rule* rule);
struct xccdf_fixtext_iterator* xccdf_rule_fixtexts(const struct xccdf_rule* rule);
const char* xccdf_group_id(const struct xccdf_group* group);
const char* xccdf_group_title(const struct xccdf_group* group);
const char* xccdf_group_description(const struct xccdf_group* group);
const char* xccdf_group_version(const struct xccdf_group* group);
const char* xccdf_group_question(const struct xccdf_group* group);
const char* xccdf_group_rationale(const struct xccdf_group* group);
const char* xccdf_group_cluster_id(const struct xccdf_group* group);
float xccdf_group_weight(const struct xccdf_group* group);
struct xccdf_group* xccdf_group_extends(const struct xccdf_group* group);
struct xccdf_item* xccdf_group_parent(const struct xccdf_group* group);
bool xccdf_group_abstract(const struct xccdf_group* group);
bool xccdf_group_prohibit_changes(const struct xccdf_group* group);
bool xccdf_group_hidden(const struct xccdf_group* group);
bool xccdf_group_selected(const struct xccdf_group* group);
struct oscap_string_iterator* xccdf_group_platforms(const struct xccdf_group* group);
struct xccdf_status_iterator* xccdf_group_statuses(const struct xccdf_group* group);
struct xccdf_reference_iterator* xccdf_group_references(const struct xccdf_group* group);
struct xccdf_item_iterator* xccdf_group_content(const struct xccdf_group* benchmark);
enum xccdf_status_type xccdf_group_status_current(const struct xccdf_group* group);
const char* xccdf_value_id(const struct xccdf_value* value);
const char* xccdf_value_title(const struct xccdf_value* value);
const char* xccdf_value_description(const struct xccdf_value* value);
struct xccdf_value* xccdf_value_extends(const struct xccdf_value* value);
struct xccdf_item* xccdf_value_parent(const struct xccdf_value* value);
bool xccdf_value_abstract(const struct xccdf_value* value);
bool xccdf_value_prohibit_changes(const struct xccdf_value* value);
bool xccdf_value_hidden(const struct xccdf_value* value);
struct xccdf_status_iterator* xccdf_value_statuses(const struct xccdf_value* value);
struct xccdf_reference_iterator* xccdf_value_references(const struct xccdf_value* value);
enum xccdf_status_type xccdf_value_status_current(const struct xccdf_value* value);
enum xccdf_value_type xccdf_value_type(const struct xccdf_value* value);
enum xccdf_interface_hint xccdf_value_interface_hint(const struct xccdf_value* value);
enum xccdf_operator xccdf_value_oper(const struct xccdf_value* value);
const char* xccdf_value_selector(const struct xccdf_value* value);
bool xccdf_value_set_selector(struct xccdf_item* value, const char* selector);
const char* xccdf_value_value_string(const struct xccdf_value* value);
xccdf_numeric xccdf_value_value_number(const struct xccdf_value* value);
bool xccdf_value_value_boolean(const struct xccdf_value* value);
const char* xccdf_value_defval_string(const struct xccdf_value* value);
xccdf_numeric xccdf_value_defval_number(const struct xccdf_value* value);
bool xccdf_value_defval_boolean(const struct xccdf_value* value);
xccdf_numeric xccdf_value_lower_bound(const struct xccdf_value* value);
xccdf_numeric xccdf_value_upper_bound(const struct xccdf_value* value);
const char* xccdf_value_match(const struct xccdf_value* value);
bool xccdf_value_must_match(const struct xccdf_value* value);
// struct oscap_string_iterator* xccdf_value_choices_string(const struct xccdf_value* value);
struct oscap_string_iterator* xccdf_value_sources(const struct xccdf_value* value);
time_t xccdf_status_date(const struct xccdf_status* status);
enum xccdf_status_type xccdf_status_status(const struct xccdf_status* status);
const char* xccdf_notice_id(const struct xccdf_notice* notice);
const char* xccdf_notice_text(const struct xccdf_notice* notice);
const char* xccdf_model_system(const struct xccdf_model* model);
const char* xccdf_model_param(const struct xccdf_model* model, const char* param_name);
const char* xccdf_ident_id(const struct xccdf_ident* ident);
const char* xccdf_ident_system(const struct xccdf_ident* ident);
const char* xccdf_check_id(const struct xccdf_check* check);
bool xccdf_check_complex(const struct xccdf_check* check);
enum xccdf_bool_operator xccdf_check_oper(const struct xccdf_check* check);
const char* xccdf_check_system(const struct xccdf_check* check);
const char* xccdf_check_selector(const struct xccdf_check* check);
const char* xccdf_check_content(const struct xccdf_check* check);
struct xccdf_rule* xccdf_check_parent(const struct xccdf_check* check);
struct xccdf_check_content_ref_iterator* xccdf_check_content_refs(const struct xccdf_check* check);
struct xccdf_check_import_iterator* xccdf_check_imports(const struct xccdf_check* check);
struct xccdf_check_export_iterator* xccdf_check_exports(const struct xccdf_check* check);
struct xccdf_check_iterator* xccdf_check_children(const struct xccdf_check* check);
const char* xccdf_check_content_ref_href(const struct xccdf_check_content_ref* ref);
const char* xccdf_check_content_ref_name(const struct xccdf_check_content_ref* ref);
const char* xccdf_profile_note_reftag(const struct xccdf_profile_note* note);
const char* xccdf_profile_note_text(const struct xccdf_profile_note* note);
const char* xccdf_check_import_name(const struct xccdf_check_import* item);
const char* xccdf_check_import_content(const struct xccdf_check_import* item);
const char* xccdf_check_export_name(const struct xccdf_check_export* item);
struct xccdf_value* xccdf_check_export_value(const struct xccdf_check_export* item);
const char* xccdf_fix_content(const struct xccdf_fix* fix);
bool xccdf_fix_reboot(const struct xccdf_fix* fix);
enum xccdf_strategy xccdf_fix_strategy(const struct xccdf_fix* fix);
enum xccdf_level xccdf_fix_complexity(const struct xccdf_fix* fix);
enum xccdf_level xccdf_fix_disruption(const struct xccdf_fix* fix);
const char* xccdf_fix_id(const struct xccdf_fix* fix);
const char* xccdf_fix_system(const struct xccdf_fix* fix);
const char* xccdf_fix_platform(const struct xccdf_fix* fix);
bool xccdf_fixtext_reboot(const struct xccdf_fixtext* fixtext);
enum xccdf_strategy xccdf_fixtext_strategy(const struct xccdf_fixtext* fixtext);
enum xccdf_level xccdf_fixtext_complexity(const struct xccdf_fixtext* fixtext);
enum xccdf_level xccdf_fixtext_disruption(const struct xccdf_fixtext* fixtext);
struct xccdf_fix* xccdf_fixtext_fixref(const struct xccdf_fixtext* fixtext);
void xccdf_cleanup(void);
/* End of xccdf.h */

