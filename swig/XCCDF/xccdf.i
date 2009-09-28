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


typedef enum {
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
} xccdf_type_t;

/// Interface hint.
typedef enum {
        XCCDF_IFACE_HINT_NONE,     ///< No interface hint
        XCCDF_IFACE_HINT_CHOICE,   ///< Choice from multiple values.
        XCCDF_IFACE_HINT_TEXTLINE, ///< Text line input widget.
        XCCDF_IFACE_HINT_TEXT,     ///< Textarea.
        XCCDF_IFACE_HINT_DATE,     ///< Date selection widget.
        XCCDF_IFACE_HINT_DATETIME, ///< Date and time selection widget.
} xccdf_interface_hint_t;

/// Status of an XCCDF item.
typedef enum {
    XCCDF_STATUS_NOT_SPECIFIED, ///< Status was not specified by benchmark.
    XCCDF_STATUS_ACCEPTED,      ///< Accepted.
    XCCDF_STATUS_DEPRECATED,    ///< Deprecated.
    XCCDF_STATUS_DRAFT,         ///< Draft item.
    XCCDF_STATUS_INCOMPLETE,    ///< The item is not complete.
    XCCDF_STATUS_INTERIM        ///< Interim.
} xccdf_status_type_t;

/// Type of an xccdf_value.
typedef enum {
        XCCDF_TYPE_NUMBER  = 1, ///< Integer.
        XCCDF_TYPE_STRING,      ///< String.
        XCCDF_TYPE_BOOLEAN,     ///< Boolean.
} xccdf_value_type_t;

/// Operator to be applied on an xccdf_value.
typedef enum {
        XCCDF_OPERATOR_EQUALS = 1,     ///< Equality.
        XCCDF_OPERATOR_NOT_EQUAL,      ///< Inequality.
        XCCDF_OPERATOR_GREATER,        ///< Greater than.
        XCCDF_OPERATOR_GREATER_EQUAL,  ///< Greater than or equal.
        XCCDF_OPERATOR_LESS,           ///< Less than.
        XCCDF_OPERATOR_LESS_EQUAL,     ///< Less than or equal.
        XCCDF_OPERATOR_PATTERN_MATCH   ///< Match a regular expression.
} xccdf_operator_t;

/// Boolean operators for logical expressions
typedef enum {
        XCCDF_OPERATOR_AND  = 0x0002,                                   ///< Logical and.
        XCCDF_OPERATOR_OR   = 0x0003,                                   ///< Logical or.
        XCCDF_OPERATOR_NOT  = 0x0100,                                   ///< Logical negation.
        XCCDF_OPERATOR_NAND = XCCDF_OPERATOR_AND | XCCDF_OPERATOR_NOT,  ///< Logical nand.
        XCCDF_OPERATOR_NOR  = XCCDF_OPERATOR_OR  | XCCDF_OPERATOR_NOT,  ///< Logical nor.
        XCCDF_OPERATOR_MASK = 0x00ff                                    ///< Mask to strip the negation away (using bitwise and).
} xccdf_bool_operator_t;

/// XCCDF error, complexity, disruption, or severity level
typedef enum {
        XCCDF_UNKNOWN = 1, ///< Unknown.
        XCCDF_INFO,        ///< Info.
        XCCDF_LOW,         ///< Low.
        XCCDF_MEDIUM,      ///< Medium.
        XCCDF_HIGH         ///< High.
} xccdf_level_t;

/// Severity of an xccdf_message.
typedef enum {
        XCCDF_MSG_INFO    = XCCDF_INFO,  ///< Info.
        XCCDF_MSG_WARNING = XCCDF_LOW,   ///< Warning.
        XCCDF_MSG_ERROR   = XCCDF_HIGH,  ///< Error
} xccdf_message_severity_t;

/// XCCDF role.
typedef enum {
        XCCDF_ROLE_FULL = 1,  ///< Check the rule and let the result contriburte to the score and appear in reports
        XCCDF_ROLE_UNSCORED,  ///< Check the rule and include the result in reports, but do not include it into score computations
        XCCDF_ROLE_UNCHECKED  
} xccdf_role_t;

/// Category of xccdf_warning.
typedef enum {
        XCCDF_WARNING_GENERAL = 1,    ///< General-purpose warning
        XCCDF_WARNING_FUNCTIONALITY,  ///< Warning about possible impacts to functionality
        XCCDF_WARNING_PERFORMANCE,    ///< Warning about changes to target system performance
        XCCDF_WARNING_HARDWARE,       ///< Warning about hardware restrictions or possible impacts to hardware
        XCCDF_WARNING_LEGAL,          ///< Warning about legal implications
        XCCDF_WARNING_REGULATORY,     ///< Warning about regulatory obligations
        XCCDF_WARNING_MANAGEMENT,     ///< Warning about impacts to the mgmt or administration of the target system
        XCCDF_WARNING_AUDIT,          ///< Warning about impacts to audit or logging
        XCCDF_WARNING_DEPENDENCY      ///< Warning about dependencies between this Rule and other parts of the target system
} xccdf_warning_category_t;

/// Fix strategy type
typedef enum {
        XCCDF_STRATEGY_UNKNOWN,      ///< Strategy not defined
        XCCDF_STRATEGY_CONFIGURE,    ///< Adjust target config or settings
        XCCDF_STRATEGY_DISABLE,      ///< Turn off or deinstall something
        XCCDF_STRATEGY_ENABLE,       ///< Turn on or install something
        XCCDF_STRATEGY_PATCH,        ///< Apply a patch, hotfix, or update
        XCCDF_STRATEGY_POLICY,       ///< Remediation by changing policies/procedures
        XCCDF_STRATEGY_RESTRICT,     ///< Adjust permissions or ACLs
        XCCDF_STRATEGY_UPDATE,       ///< Install upgrade or update the system
        XCCDF_STRATEGY_COMBINATION   ///< Combo of two or more of the above
} xccdf_strategy_t;

/// Test result
typedef enum {
        XCCDF_RESULT_PASS = 1,        ///< The test passed
        XCCDF_RESULT_FAIL,            ///< The test failed
        XCCDF_RESULT_ERROR,           ///< An error occurred and test could not complete
        XCCDF_RESULT_UNKNOWN,         ///< Could not tell what happened
        XCCDF_RESULT_NOT_APPLICABLE,  ///< Rule did not apply to test target
        XCCDF_RESULT_NOT_CHECKED,     ///< Rule did not cause any evaluation by the checking engine
        XCCDF_RESULT_NOT_SELECTED,    ///< Rule was not selected in the @link xccdf_benchmark Benchmark@endlink
        XCCDF_RESULT_INFORMATIONAL,   
        XCCDF_RESULT_FIXED            
} xccdf_test_result_type_t;

/*--------------------*\
|       Typedefs       |
\*--------------------*/

typedef float xccdf_numeric;

/*--------------------*\
|   Core structures    |
\*--------------------*/

struct xccdf_item;
struct xccdf_benchmark;
struct xccdf_profile;
struct xccdf_rule;
struct xccdf_group;
struct xccdf_value;
struct xccdf_result;


/*--------------------*\
|  Support structures  |
\*--------------------*/

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



/*--------------------*\
|       Iterators      |
\*--------------------*/

struct xccdf_item_iterator;
/// @relates xccdf_item_iterator
struct xccdf_item* xccdf_item_iterator_next(struct xccdf_item_iterator* it);
/// @relates xccdf_item_iterator
bool xccdf_item_iterator_has_more(struct xccdf_item_iterator* it);
/// @relates xccdf_item_iterator
void xccdf_item_iterator_free(struct xccdf_item_iterator* it);
struct xccdf_notice_iterator;
/// @relates xccdf_notice_iterator
struct xccdf_notice* xccdf_notice_iterator_next(struct xccdf_notice_iterator* it);
/// @relates xccdf_notice_iterator
bool xccdf_notice_iterator_has_more(struct xccdf_notice_iterator* it);
/// @relates xccdf_notice_iterator
void xccdf_notice_iterator_free(struct xccdf_notice_iterator* it);
struct xccdf_status_iterator;
/// @relates xccdf_status_iterator
struct xccdf_status* xccdf_status_iterator_next(struct xccdf_status_iterator* it);
/// @relates xccdf_status_iterator
bool xccdf_status_iterator_has_more(struct xccdf_status_iterator* it);
/// @relates xccdf_status_iterator
void xccdf_status_iterator_free(struct xccdf_status_iterator* it);
struct xccdf_reference_iterator;
/// @relates xccdf_reference_iterator
struct xccdf_reference* xccdf_reference_iterator_next(struct xccdf_reference_iterator* it);
/// @relates xccdf_reference_iterator
bool xccdf_reference_iterator_has_more(struct xccdf_reference_iterator* it);
/// @relates xccdf_reference_iterator
void xccdf_reference_iterator_free(struct xccdf_reference_iterator* it);
struct xccdf_model_iterator;
/// @relates xccdf_model_iterator
struct xccdf_model* xccdf_model_iterator_next(struct xccdf_model_iterator* it);
/// @relates xccdf_model_iterator
bool xccdf_model_iterator_has_more(struct xccdf_model_iterator* it);
/// @relates xccdf_model_iterator
void xccdf_model_iterator_free(struct xccdf_model_iterator* it);
struct xccdf_profile_iterator;
/// @relates xccdf_profile_iterator
struct xccdf_profile* xccdf_profile_iterator_next(struct xccdf_profile_iterator* it);
/// @relates xccdf_profile_iterator
bool xccdf_profile_iterator_has_more(struct xccdf_profile_iterator* it);
/// @relates xccdf_profile_iterator
void xccdf_profile_iterator_free(struct xccdf_profile_iterator* it);
struct xccdf_select_iterator;
/// @relates xccdf_select_iterator
struct xccdf_select* xccdf_select_iterator_next(struct xccdf_select_iterator* it);
/// @relates xccdf_select_iterator
bool xccdf_select_iterator_has_more(struct xccdf_select_iterator* it);
/// @relates xccdf_select_iterator
void xccdf_select_iterator_free(struct xccdf_select_iterator* it);
struct xccdf_set_value_iterator;
/// @relates xccdf_set_value_iterator
struct xccdf_set_value* xccdf_set_value_iterator_next(struct xccdf_set_value_iterator* it);
/// @relates xccdf_set_value_iterator
bool xccdf_set_value_iterator_has_more(struct xccdf_set_value_iterator* it);
/// @relates xccdf_set_value_iterator
void xccdf_set_value_iterator_free(struct xccdf_set_value_iterator* it);
struct xccdf_refine_value_iterator;
/// @relates xccdf_refine_value_iterator
struct xccdf_refine_value* xccdf_refine_value_iterator_next(struct xccdf_refine_value_iterator* it);
/// @relates xccdf_refine_value_iterator
bool xccdf_refine_value_iterator_has_more(struct xccdf_refine_value_iterator* it);
/// @relates xccdf_refine_value_iterator
void xccdf_refine_value_iterator_free(struct xccdf_refine_value_iterator* it);
struct xccdf_refine_rule_iterator;
/// @relates xccdf_refine_rule_iterator
struct xccdf_refine_rule* xccdf_refine_rule_iterator_next(struct xccdf_refine_rule_iterator* it);
/// @relates xccdf_refine_rule_iterator
bool xccdf_refine_rule_iterator_has_more(struct xccdf_refine_rule_iterator* it);
/// @relates xccdf_refine_rule_iterator
void xccdf_refine_rule_iterator_free(struct xccdf_refine_rule_iterator* it);
struct xccdf_ident_iterator;
/// @relates xccdf_ident_iterator
struct xccdf_ident* xccdf_ident_iterator_next(struct xccdf_ident_iterator* it);
/// @relates xccdf_ident_iterator
bool xccdf_ident_iterator_has_more(struct xccdf_ident_iterator* it);
/// @relates xccdf_ident_iterator
void xccdf_ident_iterator_free(struct xccdf_ident_iterator* it);
struct xccdf_check_iterator;
/// @relates xccdf_check_iterator
struct xccdf_check* xccdf_check_iterator_next(struct xccdf_check_iterator* it);
/// @relates xccdf_check_iterator
bool xccdf_check_iterator_has_more(struct xccdf_check_iterator* it);
/// @relates xccdf_check_iterator
void xccdf_check_iterator_free(struct xccdf_check_iterator* it);
struct xccdf_check_content_ref_iterator;
/// @relates xccdf_check_content_ref_iterator
struct xccdf_check_content_ref* xccdf_check_content_ref_iterator_next(struct xccdf_check_content_ref_iterator* it);
/// @relates xccdf_check_content_ref_iterator
bool xccdf_check_content_ref_iterator_has_more(struct xccdf_check_content_ref_iterator* it);
/// @relates xccdf_check_content_ref_iterator
void xccdf_check_content_ref_iterator_free(struct xccdf_check_content_ref_iterator* it);
struct xccdf_profile_note_iterator;
/// @relates xccdf_profile_note_iterator
struct xccdf_profile_note* xccdf_profile_note_iterator_next(struct xccdf_profile_note_iterator* it);
/// @relates xccdf_profile_note_iterator
bool xccdf_profile_note_iterator_has_more(struct xccdf_profile_note_iterator* it);
/// @relates xccdf_profile_note_iterator
void xccdf_profile_note_iterator_free(struct xccdf_profile_note_iterator* it);
struct xccdf_check_import_iterator;
/// @relates xccdf_check_import_iterator
struct xccdf_check_import* xccdf_check_import_iterator_next(struct xccdf_check_import_iterator* it);
/// @relates xccdf_check_import_iterator
bool xccdf_check_import_iterator_has_more(struct xccdf_check_import_iterator* it);
/// @relates xccdf_check_import_iterator
void xccdf_check_import_iterator_free(struct xccdf_check_import_iterator* it);
struct xccdf_check_export_iterator;
/// @relates xccdf_check_export_iterator
struct xccdf_check_export* xccdf_check_export_iterator_next(struct xccdf_check_export_iterator* it);
/// @relates xccdf_check_export_iterator
bool xccdf_check_export_iterator_has_more(struct xccdf_check_export_iterator* it);
/// @relates xccdf_check_export_iterator
void xccdf_check_export_iterator_free(struct xccdf_check_export_iterator* it);
struct xccdf_fix_iterator;
/// @relates xccdf_fix_iterator
struct xccdf_fix* xccdf_fix_iterator_next(struct xccdf_fix_iterator* it);
/// @relates xccdf_fix_iterator
bool xccdf_fix_iterator_has_more(struct xccdf_fix_iterator* it);
/// @relates xccdf_fix_iterator
void xccdf_fix_iterator_free(struct xccdf_fix_iterator* it);
struct xccdf_fixtext_iterator;
/// @relates xccdf_fixtext_iterator
struct xccdf_fixtext* xccdf_fixtext_iterator_next(struct xccdf_fixtext_iterator* it);
/// @relates xccdf_fixtext_iterator
bool xccdf_fixtext_iterator_has_more(struct xccdf_fixtext_iterator* it);
/// @relates xccdf_fixtext_iterator
void xccdf_fixtext_iterator_free(struct xccdf_fixtext_iterator* it);


/*--------------------*\
|     Item methods     |
\*--------------------*/

xccdf_type_t xccdf_item_get_type(const struct xccdf_item* item);
const char* xccdf_item_get_id(const struct xccdf_item* item);
const char* xccdf_item_get_title(const struct xccdf_item* item);
const char* xccdf_item_get_description(const struct xccdf_item* item);
const char* xccdf_item_get_version(const struct xccdf_item* item);
struct xccdf_item* xccdf_item_get_extends(const struct xccdf_item* item);
struct xccdf_item* xccdf_item_get_parent(const struct xccdf_item* item);
struct xccdf_status_iterator* xccdf_item_get_statuses(const struct xccdf_item* item);
struct xccdf_reference_iterator* xccdf_item_get_references(const struct xccdf_item* item);
xccdf_status_type_t xccdf_item_get_status_current(const struct xccdf_item* item);
/*struct xccdf_benchmark* xccdf_item_cast_benchmark(struct xccdf_item* item);*/
/*struct xccdf_profile* xccdf_item_cast_profile(struct xccdf_item* item);*/
/*struct xccdf_rule* xccdf_item_cast_rule(struct xccdf_item* item);*/
/*struct xccdf_group* xccdf_item_cast_group(struct xccdf_item* item);*/
/*struct xccdf_value* xccdf_item_cast_value(struct xccdf_item* item);*/
/*struct xccdf_result* xccdf_item_cast_result(struct xccdf_item* item);*/

/*--------------------*\
|   Benchmark methods  |
\*--------------------*/

struct xccdf_benchmark* xccdf_benchmark_new_from_file(const char* filename);
const char* xccdf_benchmark_get_id(const struct xccdf_benchmark* benchmark);
bool xccdf_benchmark_get_resolved(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_get_title(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_get_description(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_get_version(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_get_style(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_get_style_href(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_get_front_matter(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_get_rear_matter(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_get_metadata(const struct xccdf_benchmark* benchmark);
const char* xccdf_benchmark_get_plain_text(const struct xccdf_benchmark* benchmark, const char* id);
struct xccdf_item* xccdf_benchmark_get_item(const struct xccdf_benchmark* benchmark, const char* id);
struct xccdf_status_iterator* xccdf_benchmark_get_statuses(const struct xccdf_benchmark* benchmark);
struct xccdf_reference_iterator* xccdf_benchmark_get_references(const struct xccdf_benchmark* benchmark);
struct oscap_string_iterator*  xccdf_benchmark_get_platforms(const struct xccdf_benchmark* benchmark);
struct xccdf_notice_iterator*  xccdf_benchmark_get_notices(const struct xccdf_benchmark* benchmark);
struct xccdf_model_iterator*   xccdf_benchmark_get_models(const struct xccdf_benchmark* benchmark);
struct xccdf_profile_iterator* xccdf_benchmark_get_profiles(const struct xccdf_benchmark* benchmark);
struct xccdf_item_iterator* xccdf_benchmark_get_content(const struct xccdf_benchmark* benchmark);
xccdf_status_type_t xccdf_benchmark_get_status_current(const struct xccdf_benchmark* benchmark);
void xccdf_benchmark_free(struct xccdf_benchmark* benchmark);


/*--------------------*\
|    Profile methods   |
\*--------------------*/

const char* xccdf_profile_get_id(const struct xccdf_profile* profile);
const char* xccdf_profile_get_title(const struct xccdf_profile* profile);
const char* xccdf_profile_get_description(const struct xccdf_profile* profile);
const char* xccdf_profile_get_version(const struct xccdf_profile* profile);
struct xccdf_profile* xccdf_profile_get_extends(const struct xccdf_profile* profile);
struct xccdf_benchmark* xccdf_profile_get_benchmark(const struct xccdf_profile* profile);
bool xccdf_profile_get_abstract(const struct xccdf_profile* profile);
bool xccdf_profile_get_prohibit_changes(const struct xccdf_profile* profile);
struct oscap_string_iterator* xccdf_profile_get_platforms(const struct xccdf_profile* profile);
struct xccdf_status_iterator* xccdf_profile_get_statuses(const struct xccdf_profile* profile);
struct xccdf_reference_iterator* xccdf_profile_get_references(const struct xccdf_profile* profile);
xccdf_status_type_t xccdf_profile_get_status_current(const struct xccdf_profile* profile);
/*const char* xccdf_profile_note_get_tag(const struct xccdf_profile* profile);*/
struct xccdf_select_iterator* xccdf_profile_get_selects(const struct xccdf_profile* profile);
struct xccdf_set_value_iterator* xccdf_profile_get_set_values(const struct xccdf_profile* profile);
struct xccdf_refine_value_iterator* xccdf_profile_get_refine_values(const struct xccdf_profile* profile);
struct xccdf_refine_rule_iterator* xccdf_profile_get_refine_rules(const struct xccdf_profile* profile);



/*--------------------*\
|     Rule methods     |
\*--------------------*/

const char* xccdf_rule_get_id(const struct xccdf_rule* rule);
const char* xccdf_rule_get_title(const struct xccdf_rule* rule);
const char* xccdf_rule_get_description(const struct xccdf_rule* rule);
const char* xccdf_rule_get_version(const struct xccdf_rule* rule);
const char* xccdf_rule_get_question(const struct xccdf_rule* rule);
const char* xccdf_rule_get_rationale(const struct xccdf_rule* rule);
const char* xccdf_rule_get_cluster_id(const struct xccdf_rule* rule);
struct xccdf_check* xccdf_rule_get_check(const struct xccdf_rule* rule);
float xccdf_rule_get_weight(const struct xccdf_rule* rule);
struct xccdf_rule* xccdf_rule_get_extends(const struct xccdf_rule* rule);
struct xccdf_item* xccdf_rule_get_parent(const struct xccdf_rule* rule);
bool xccdf_rule_get_abstract(const struct xccdf_rule* rule);
bool xccdf_rule_get_prohibit_changes(const struct xccdf_rule* rule);
bool xccdf_rule_get_hidden(const struct xccdf_rule* rule);
bool xccdf_rule_get_selected(const struct xccdf_rule* rule);
bool xccdf_rule_get_multiple(const struct xccdf_rule* rule);
struct oscap_string_iterator* xccdf_rule_get_platforms(const struct xccdf_rule* rule);
struct xccdf_status_iterator* xccdf_rule_get_statuses(const struct xccdf_rule* rule);
struct xccdf_reference_iterator* xccdf_rule_get_references(const struct xccdf_rule* rule);
xccdf_status_type_t xccdf_rule_get_status_current(const struct xccdf_rule* rule);
const char* xccdf_rule_get_impact_metric(const struct xccdf_rule* rule);
xccdf_role_t xccdf_rule_get_role(const struct xccdf_rule* rule);
xccdf_level_t xccdf_rule_get_severity(const struct xccdf_rule* rule);
struct xccdf_ident_iterator* xccdf_rule_get_idents(const struct xccdf_rule* rule);
struct xccdf_check_iterator* xccdf_rule_get_checks(const struct xccdf_rule* rule);
struct xccdf_profile_note_iterator* xccdf_rule_get_profile_notes(const struct xccdf_rule* rule);
struct xccdf_fix_iterator* xccdf_rule_get_fixes(const struct xccdf_rule* rule);
struct xccdf_fixtext_iterator* xccdf_rule_get_fixtexts(const struct xccdf_rule* rule);



/*--------------------*\
|     Group methods    |
\*--------------------*/

const char* xccdf_group_get_id(const struct xccdf_group* group);
const char* xccdf_group_get_title(const struct xccdf_group* group);
const char* xccdf_group_get_description(const struct xccdf_group* group);
const char* xccdf_group_get_version(const struct xccdf_group* group);
const char* xccdf_group_get_question(const struct xccdf_group* group);
const char* xccdf_group_get_rationale(const struct xccdf_group* group);
const char* xccdf_group_get_cluster_id(const struct xccdf_group* group);
float xccdf_group_get_weight(const struct xccdf_group* group);
struct xccdf_group* xccdf_group_get_extends(const struct xccdf_group* group);
struct xccdf_item* xccdf_group_get_parent(const struct xccdf_group* group);
bool xccdf_group_get_abstract(const struct xccdf_group* group);
bool xccdf_group_get_prohibit_changes(const struct xccdf_group* group);
bool xccdf_group_get_hidden(const struct xccdf_group* group);
bool xccdf_group_get_selected(const struct xccdf_group* group);
struct oscap_string_iterator* xccdf_group_get_platforms(const struct xccdf_group* group);
struct xccdf_status_iterator* xccdf_group_get_statuses(const struct xccdf_group* group);
struct xccdf_reference_iterator* xccdf_group_get_references(const struct xccdf_group* group);
struct xccdf_item_iterator* xccdf_group_get_content(const struct xccdf_group* benchmark);
xccdf_status_type_t xccdf_group_get_status_current(const struct xccdf_group* group);


/*--------------------*\
|     Value methods    |
\*--------------------*/

const char* xccdf_value_get_id(const struct xccdf_value* value);
const char* xccdf_value_get_title(const struct xccdf_value* value);
const char* xccdf_value_get_description(const struct xccdf_value* value);
struct xccdf_value* xccdf_value_get_extends(const struct xccdf_value* value);
struct xccdf_item* xccdf_value_get_parent(const struct xccdf_value* value);
bool xccdf_value_get_abstract(const struct xccdf_value* value);
bool xccdf_value_get_prohibit_changes(const struct xccdf_value* value);
bool xccdf_value_get_hidden(const struct xccdf_value* value);
struct xccdf_status_iterator* xccdf_value_get_statuses(const struct xccdf_value* value);
struct xccdf_reference_iterator* xccdf_value_get_references(const struct xccdf_value* value);
xccdf_status_type_t xccdf_value_get_status_current(const struct xccdf_value* value);
xccdf_value_type_t xccdf_value_get_type(const struct xccdf_value* value);
xccdf_interface_hint_t xccdf_value_get_interface_hint(const struct xccdf_value* value);
xccdf_operator_t xccdf_value_get_oper(const struct xccdf_value* value);
const char* xccdf_value_get_selector(const struct xccdf_value* value);
bool xccdf_value_get_set_selector(struct xccdf_item* value, const char* selector);
const char* xccdf_value_get_value_string(const struct xccdf_value* value);
xccdf_numeric xccdf_value_get_value_number(const struct xccdf_value* value);
bool xccdf_value_get_value_boolean(const struct xccdf_value* value);
const char* xccdf_value_get_defval_string(const struct xccdf_value* value);
xccdf_numeric xccdf_value_get_defval_number(const struct xccdf_value* value);
bool xccdf_value_get_defval_boolean(const struct xccdf_value* value);
xccdf_numeric xccdf_value_get_lower_bound(const struct xccdf_value* value);
xccdf_numeric xccdf_value_get_upper_bound(const struct xccdf_value* value);
const char* xccdf_value_get_match(const struct xccdf_value* value);
bool xccdf_value_get_must_match(const struct xccdf_value* value);
// struct oscap_string_iterator* xccdf_value_choices_string(const struct xccdf_value* value);
/*struct oscap_string_iterator* xccdf_value_sources(const struct xccdf_value* value);*/




/*--------------------*\
|     Other methods    |
\*--------------------*/

time_t xccdf_status_get_date(const struct xccdf_status* status);
xccdf_status_type_t xccdf_status_get_status(const struct xccdf_status* status);
const char* xccdf_notice_get_id(const struct xccdf_notice* notice);
const char* xccdf_notice_get_text(const struct xccdf_notice* notice);
const char* xccdf_model_get_system(const struct xccdf_model* model);
/*const char* xccdf_model_get_param(const struct xccdf_model* model, const char* param_name);*/
const char* xccdf_ident_get_id(const struct xccdf_ident* ident);
const char* xccdf_ident_get_system(const struct xccdf_ident* ident);
const char* xccdf_check_get_id(const struct xccdf_check* check);
bool xccdf_check_get_complex(const struct xccdf_check* check);
xccdf_bool_operator_t xccdf_check_get_oper(const struct xccdf_check* check);
const char* xccdf_check_get_system(const struct xccdf_check* check);
const char* xccdf_check_get_selector(const struct xccdf_check* check);
const char* xccdf_check_get_content(const struct xccdf_check* check);
struct xccdf_rule* xccdf_check_get_parent(const struct xccdf_check* check);
/*struct xccdf_check_content_ref_iterator* xccdf_check_content_refs(const struct xccdf_check* check);*/
/*struct xccdf_check_import_iterator* xccdf_check_imports(const struct xccdf_check* check);*/
/*struct xccdf_check_export_iterator* xccdf_check_exports(const struct xccdf_check* check);*/
struct xccdf_check_iterator* xccdf_check_get_children(const struct xccdf_check* check);
const char* xccdf_check_content_ref_get_href(const struct xccdf_check_content_ref* ref);
const char* xccdf_check_content_ref_get_name(const struct xccdf_check_content_ref* ref);
const char* xccdf_profile_note_get_reftag(const struct xccdf_profile_note* note);
const char* xccdf_profile_note_get_text(const struct xccdf_profile_note* note);
const char* xccdf_check_import_get_name(const struct xccdf_check_import* item);
const char* xccdf_check_import_get_content(const struct xccdf_check_import* item);
const char* xccdf_check_export_get_name(const struct xccdf_check_export* item);
struct xccdf_value* xccdf_check_export_get_value(const struct xccdf_check_export* item);
const char* xccdf_fix_get_content(const struct xccdf_fix* fix);
bool xccdf_fix_get_reboot(const struct xccdf_fix* fix);
xccdf_strategy_t xccdf_fix_get_strategy(const struct xccdf_fix* fix);
xccdf_level_t xccdf_fix_get_complexity(const struct xccdf_fix* fix);
xccdf_level_t xccdf_fix_get_disruption(const struct xccdf_fix* fix);
const char* xccdf_fix_get_id(const struct xccdf_fix* fix);
const char* xccdf_fix_get_system(const struct xccdf_fix* fix);
const char* xccdf_fix_get_platform(const struct xccdf_fix* fix);
bool xccdf_fixtext_get_reboot(const struct xccdf_fixtext* fixtext);
xccdf_strategy_t xccdf_fixtext_get_strategy(const struct xccdf_fixtext* fixtext);
xccdf_level_t xccdf_fixtext_get_complexity(const struct xccdf_fixtext* fixtext);
xccdf_level_t xccdf_fixtext_get_disruption(const struct xccdf_fixtext* fixtext);
struct xccdf_fix* xccdf_fixtext_get_fixref(const struct xccdf_fixtext* fixtext);
void xccdf_cleanup(void);

/* End of xccdf.h */

