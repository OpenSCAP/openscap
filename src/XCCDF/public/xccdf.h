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

/**
 * @addtogroup XCCDF
 * @{
 * @file xccdf.h
 * Open-scap XCCDF library interface.
 * @author Lukas Kuklinek <lkuklinek@redhat.com>
 */

/**
 * @defgroup benchmark XCCDF Benchmark
 * Top level XCCDF structure containing profiles, rules, values and results.
 *
 * @defgroup profile   XCCDF Profile
 * XCCDF profile is a set of tests and their settings in a compact package.
 *
 * @defgroup rule      XCCDF Rule
 * XCCDF rule defines a test execution. Groups are in this module as well.
 *
 * @defgroup value     XCCDF Value
 * XCCDF Value allows test parametrization or capturing output of tests.
 *
 * @defgroup result    XCCDF Test Result
 * Actual results of running a XCCDF test or profile.
 */

#ifndef XCCDF_H_
#define XCCDF_H_

#include <stdbool.h>
#include <time.h>
#include "oscap.h"

/*--------------------*\
|     Enumerations     |
\*--------------------*/

/**
 * Type of an XCCDF object.
 *
 * When checking the type, you can use either operator == for type equivalence,
 * or operator &amp; to take a type inheriritance hierarchy into account.
 * For example, XCCDF_ITEM & XCCDF_RULE evaluates to true,
 * as the rule type is a subclass of the xccdf item type.
 */
typedef enum {
	XCCDF_BENCHMARK = 0x0100,	///< Type constant for xccdf_benchmark
	XCCDF_PROFILE = 0x0200,	///< Type constant for xccdf_profile
	XCCDF_RESULT = 0x0400,	///< Type constant for xccdf_result
	XCCDF_RULE = 0x1000,	///< Type constant for xccdf_rule
	XCCDF_GROUP = 0x2000,	///< Type constant for xccdf_group
	XCCDF_VALUE = 0x4000,	///< Type constant for xccdf_value
	/// Represents selectable items, i.e. rules and groups (see xccdf_item)
	XCCDF_CONTENT = XCCDF_RULE | XCCDF_GROUP,
	/// Represents items as described in the XCCDF documentation (see xccdf_item)
	XCCDF_ITEM = XCCDF_RULE | XCCDF_GROUP | XCCDF_VALUE,
	/// Represents an object, profile, result or whole benchmark (see xccdf_item)
	XCCDF_OBJECT = XCCDF_ITEM | XCCDF_PROFILE | XCCDF_BENCHMARK | XCCDF_RESULT,
} xccdf_type_t;

/// Interface hint.
typedef enum {
	XCCDF_IFACE_HINT_NONE,	///< No interface hint
	XCCDF_IFACE_HINT_CHOICE,	///< Choice from multiple values.
	XCCDF_IFACE_HINT_TEXTLINE,	///< Text line input widget.
	XCCDF_IFACE_HINT_TEXT,	///< Textarea.
	XCCDF_IFACE_HINT_DATE,	///< Date selection widget.
	XCCDF_IFACE_HINT_DATETIME,	///< Date and time selection widget.
} xccdf_interface_hint_t;

/// Status of an XCCDF item.
typedef enum {
	XCCDF_STATUS_NOT_SPECIFIED,	///< Status was not specified by benchmark.
	XCCDF_STATUS_ACCEPTED,	///< Accepted.
	XCCDF_STATUS_DEPRECATED,	///< Deprecated.
	XCCDF_STATUS_DRAFT,	///< Draft item.
	XCCDF_STATUS_INCOMPLETE,	///< The item is not complete.
	XCCDF_STATUS_INTERIM	///< Interim.
} xccdf_status_type_t;

/// Type of an xccdf_value.
typedef enum {
	XCCDF_TYPE_NUMBER = 1,	///< Integer.
	XCCDF_TYPE_STRING,	///< String.
	XCCDF_TYPE_BOOLEAN,	///< Boolean.
} xccdf_value_type_t;

/// Operator to be applied on an xccdf_value.
typedef enum {
	XCCDF_OPERATOR_EQUALS = 1,	///< Equality.
	XCCDF_OPERATOR_NOT_EQUAL,	///< Inequality.
	XCCDF_OPERATOR_GREATER,	///< Greater than.
	XCCDF_OPERATOR_GREATER_EQUAL,	///< Greater than or equal.
	XCCDF_OPERATOR_LESS,	///< Less than.
	XCCDF_OPERATOR_LESS_EQUAL,	///< Less than or equal.
	XCCDF_OPERATOR_PATTERN_MATCH	///< Match a regular expression.
} xccdf_operator_t;

/// Boolean operators for logical expressions
typedef enum {
	XCCDF_OPERATOR_AND = 0x0002,	///< Logical and.
	XCCDF_OPERATOR_OR = 0x0003,	///< Logical or.
	XCCDF_OPERATOR_NOT = 0x0100,	///< Logical negation.
	XCCDF_OPERATOR_NAND = XCCDF_OPERATOR_AND | XCCDF_OPERATOR_NOT,	///< Logical nand.
	XCCDF_OPERATOR_NOR = XCCDF_OPERATOR_OR | XCCDF_OPERATOR_NOT,	///< Logical nor.
	XCCDF_OPERATOR_MASK = 0x00ff	///< Mask to strip the negation away (using bitwise and).
} xccdf_bool_operator_t;

/// XCCDF error, complexity, disruption, or severity level
typedef enum {
	XCCDF_UNKNOWN = 1,	///< Unknown.
	XCCDF_INFO,		///< Info.
	XCCDF_LOW,		///< Low.
	XCCDF_MEDIUM,		///< Medium.
	XCCDF_HIGH		///< High.
} xccdf_level_t;

/// Severity of an xccdf_message.
typedef enum {
	XCCDF_MSG_INFO = XCCDF_INFO,	///< Info.
	XCCDF_MSG_WARNING = XCCDF_LOW,	///< Warning.
	XCCDF_MSG_ERROR = XCCDF_HIGH,	///< Error
} xccdf_message_severity_t;

/// XCCDF role.
typedef enum {
	XCCDF_ROLE_FULL = 1,	///< Check the rule and let the result contriburte to the score and appear in reports
	XCCDF_ROLE_UNSCORED,	///< Check the rule and include the result in reports, but do not include it into score computations
	XCCDF_ROLE_UNCHECKED	///< Don't check the rule, result will be XCCDF_RESULT_UNKNOWN
} xccdf_role_t;

/// Category of xccdf_warning.
typedef enum {
	XCCDF_WARNING_GENERAL = 1,	///< General-purpose warning
	XCCDF_WARNING_FUNCTIONALITY,	///< Warning about possible impacts to functionality
	XCCDF_WARNING_PERFORMANCE,	///< Warning about changes to target system performance
	XCCDF_WARNING_HARDWARE,	///< Warning about hardware restrictions or possible impacts to hardware
	XCCDF_WARNING_LEGAL,	///< Warning about legal implications
	XCCDF_WARNING_REGULATORY,	///< Warning about regulatory obligations
	XCCDF_WARNING_MANAGEMENT,	///< Warning about impacts to the mgmt or administration of the target system
	XCCDF_WARNING_AUDIT,	///< Warning about impacts to audit or logging
	XCCDF_WARNING_DEPENDENCY	///< Warning about dependencies between this Rule and other parts of the target system
} xccdf_warning_category_t;

/// Fix strategy type
typedef enum {
	XCCDF_STRATEGY_UNKNOWN,	///< Strategy not defined
	XCCDF_STRATEGY_CONFIGURE,	///< Adjust target config or settings
	XCCDF_STRATEGY_DISABLE,	///< Turn off or deinstall something
	XCCDF_STRATEGY_ENABLE,	///< Turn on or install something
	XCCDF_STRATEGY_PATCH,	///< Apply a patch, hotfix, or update
	XCCDF_STRATEGY_POLICY,	///< Remediation by changing policies/procedures
	XCCDF_STRATEGY_RESTRICT,	///< Adjust permissions or ACLs
	XCCDF_STRATEGY_UPDATE,	///< Install upgrade or update the system
	XCCDF_STRATEGY_COMBINATION	///< Combo of two or more of the above
} xccdf_strategy_t;

/// Test result
typedef enum {
	XCCDF_RESULT_PASS = 1,	///< The test passed
	XCCDF_RESULT_FAIL,	///< The test failed
	XCCDF_RESULT_ERROR,	///< An error occurred and test could not complete
	XCCDF_RESULT_UNKNOWN,	///< Could not tell what happened
	XCCDF_RESULT_NOT_APPLICABLE,	///< Rule did not apply to test target
	XCCDF_RESULT_NOT_CHECKED,	///< Rule did not cause any evaluation by the checking engine
	XCCDF_RESULT_NOT_SELECTED,	///< Rule was not selected in the @link xccdf_benchmark Benchmark@endlink
	XCCDF_RESULT_INFORMATIONAL,	///< Rule was evaluated by the checking engine, but isn't to be scored
	XCCDF_RESULT_FIXED	///< Rule failed, but was later fixed
} xccdf_test_result_type_t;

/*--------------------*\
|       Typedefs       |
\*--------------------*/

/**
 * Type of a numerical content for a XCCDF value.
 */
typedef float xccdf_numeric;

/*--------------------*\
|   Core structures    |
\*--------------------*/
/*
 * @addtogroup xccdf_item
 * @{
 * @addtogroup xccdf_item_setters Setters
 * @{
 * @ref xccdf_item set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_sysinfo_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup xccdf_item_getters Getters
 * @{
 * @ref xccdf_item get methods.
 * @}
 * @addtogroup xccdf_item_iterators Iterators
 * @{
 * @ref xccdf_item iterator methods.
 * @}
 * @addtogroup xccdf_item_eval Evaluators
 * @{
 * @ref xccdf_item evaluator methods
 * @}
 */
/** @struct xccdf_item
 * A base class for XCCDF items.
 */
struct xccdf_item;
/** @struct xccdf_benchmark
 * @ingroup benchmark
 * XCCDF Benchmark.
 */
struct xccdf_benchmark;

/** @struct xccdf_profile
 * @ingroup profile
 * XCCDF Profile.
 */
struct xccdf_profile;

/** @struct xccdf_rule
 * @ingroup rule
 * XCCDF Rule.
 */
struct xccdf_rule;

/** @struct xccdf_group
 * @ingroup rule
 * XCCDF rule group.
 */
struct xccdf_group;

/** @struct xccdf_value
 * @ingroup value
 * XCCDF Value
 */
struct xccdf_value;

/** @struct xccdf_result
 * @ingroup result
 * XCCDF Test Result.
 */
struct xccdf_result;

/*--------------------*\
|  Support structures  |
\*--------------------*/

/** @struct xccdf_notice
 * @ingroup benchmark
 * XCCDF benchmark legal notice.
 */
struct xccdf_notice;

/** @struct xccdf_status
 * @ingroup benchmark
 * XCCDF item status.
 */
struct xccdf_status;

/** @struct xccdf_model
 * @ingroup benchmark
 * XCCDF scoring model.
 */
struct xccdf_model;

/** @struct xccdf_warning
 * XCCDF warning.
 */
struct xccdf_warning;

/** @struct xccdf_select
 * XCCDF select option usen in the profile.
 * @ingroup profile
 */
struct xccdf_select;

/** @struct xccdf_set_value
 * XCCDF set value option used in the profile.
 * @ingroup profile
 */
struct xccdf_set_value;

/** @struct xccdf_refine_value
 * XCCDF refine value option used in the profile.
 * @ingroup profile
 */
struct xccdf_refine_value;
/** @struct xccdf_refine_rule
 * XCCDF refine rule option used in the profile.
 * @ingroup profile
 */
struct xccdf_refine_rule;

/** @struct xccdf_ident
 * XCCDF rule ident URI.
 * @ingroup rule
 */
struct xccdf_ident;

/** @struct xccdf_check
 * XCCDF simple or complex check.
 * @ingroup rule
 */
struct xccdf_check;
/** @struct xccdf_check_content_ref
 * XCCDF check content reference.
 * @ingroup rule
 */
struct xccdf_check_content_ref;

/** @struct xccdf_profile_note
 * XCCDF note for given rule in context of given profile.
 * @ingroup rule
 */
struct xccdf_profile_note;

/** @struct xccdf_check_import
 * XCCDF check import.
 * @ingroup rule
 * @see xccdf_check
 */
struct xccdf_check_import;

/** @struct xccdf_check_export
 * XCCDF check export.
 * @ingroup rule
 * @see xccdf_check
 */
struct xccdf_check_export;
/** @struct xccdf_fix
 * XCCDF automatic fix.
 * @ingroup rule
 * @see xccdf_rule
 */
struct xccdf_fix;
/** @struct xccdf_fixtext
 * XCCDF textual fix instructions.
 * @ingroup rule
 * @see xccdf_rule
 */
struct xccdf_fixtext;

/*--------------------*\
|       Iterators      |
\*--------------------*/

/** @struct xccdf_item_iterator
 * String iterator.
 * @see oscap_iterator
 */
struct xccdf_item_iterator;
/// @memberof xccdf_item_iterator
struct xccdf_item *xccdf_item_iterator_next(struct xccdf_item_iterator *it);
/// @memberof xccdf_item_iterator
bool xccdf_item_iterator_has_more(struct xccdf_item_iterator *it);
/// @memberof xccdf_item_iterator
void xccdf_item_iterator_free(struct xccdf_item_iterator *it);
/** @struct xccdf_notice_iterator
 * Notice iterator.
 * @see oscap_iterator
 */
struct xccdf_notice_iterator;
/// @memberof xccdf_notice_iterator
struct xccdf_notice *xccdf_notice_iterator_next(struct xccdf_notice_iterator *it);
/// @memberof xccdf_notice_iterator
bool xccdf_notice_iterator_has_more(struct xccdf_notice_iterator *it);
/// @memberof xccdf_notice_iterator
void xccdf_notice_iterator_free(struct xccdf_notice_iterator *it);
/** @struct xccdf_status_iterator
 * Status iterator.
 * @see oscap_iterator
 */
struct xccdf_status_iterator;
/// @memberof xccdf_status_iterator
struct xccdf_status *xccdf_status_iterator_next(struct xccdf_status_iterator *it);
/// @memberof xccdf_status_iterator
bool xccdf_status_iterator_has_more(struct xccdf_status_iterator *it);
/// @memberof xccdf_status_iterator
void xccdf_status_iterator_free(struct xccdf_status_iterator *it);
/** @struct xccdf_reference_iterator
 * Reference iterator.
 * @see oscap_iterator
 */
struct xccdf_reference_iterator;
/// @memberof xccdf_reference_iterator
struct xccdf_reference *xccdf_reference_iterator_next(struct xccdf_reference_iterator *it);
/// @memberof xccdf_reference_iterator
bool xccdf_reference_iterator_has_more(struct xccdf_reference_iterator *it);
/// @memberof xccdf_reference_iterator
void xccdf_reference_iterator_free(struct xccdf_reference_iterator *it);
/** @struct xccdf_model_iterator
 * Model iterator.
 * @see oscap_iterator
 */
struct xccdf_model_iterator;
/// @memberof xccdf_model_iterator
struct xccdf_model *xccdf_model_iterator_next(struct xccdf_model_iterator *it);
/// @memberof xccdf_model_iterator
bool xccdf_model_iterator_has_more(struct xccdf_model_iterator *it);
/// @memberof xccdf_model_iterator
void xccdf_model_iterator_free(struct xccdf_model_iterator *it);
/** @struct xccdf_profile_iterator
 * Profile iterator.
 * @see oscap_iterator
 */
struct xccdf_profile_iterator;
/// @memberof xccdf_profile_iterator
struct xccdf_profile *xccdf_profile_iterator_next(struct xccdf_profile_iterator *it);
/// @memberof xccdf_profile_iterator
bool xccdf_profile_iterator_has_more(struct xccdf_profile_iterator *it);
/// @memberof xccdf_profile_iterator
void xccdf_profile_iterator_free(struct xccdf_profile_iterator *it);

/** @struct xccdf_select_iterator
 * Select iterator.
 * @see oscap_iterator
 */
struct xccdf_select_iterator;
/// @memberof xccdf_select_iterator
struct xccdf_select *xccdf_select_iterator_next(struct xccdf_select_iterator *it);
/// @memberof xccdf_select_iterator
bool xccdf_select_iterator_has_more(struct xccdf_select_iterator *it);
/// @memberof xccdf_select_iterator
void xccdf_select_iterator_free(struct xccdf_select_iterator *it);

/** @struct xccdf_set_value_iterator
 * Set value iterator.
 * @see oscap_iterator
 */
struct xccdf_set_value_iterator;
/// @memberof xccdf_set_value_iterator
struct xccdf_set_value *xccdf_set_value_iterator_next(struct xccdf_set_value_iterator *it);
/// @memberof xccdf_set_value_iterator
bool xccdf_set_value_iterator_has_more(struct xccdf_set_value_iterator *it);
/// @memberof xccdf_set_value_iterator
void xccdf_set_value_iterator_free(struct xccdf_set_value_iterator *it);

/** @struct xccdf_refine_value_iterator
 * Refine value iterator.
 * @see oscap_iterator
 */
struct xccdf_refine_value_iterator;
/// @memberof xccdf_refine_value_iterator
struct xccdf_refine_value *xccdf_refine_value_iterator_next(struct xccdf_refine_value_iterator *it);
/// @memberof xccdf_refine_value_iterator
bool xccdf_refine_value_iterator_has_more(struct xccdf_refine_value_iterator *it);
/// @memberof xccdf_refine_value_iterator
void xccdf_refine_value_iterator_free(struct xccdf_refine_value_iterator *it);

/** @struct xccdf_refine_rule_iterator
 * Refine rule iterator.
 * @see oscap_iterator
 */
struct xccdf_refine_rule_iterator;
/// @memberof xccdf_refine_rule_iterator
struct xccdf_refine_rule *xccdf_refine_rule_iterator_next(struct xccdf_refine_rule_iterator *it);
/// @memberof xccdf_refine_rule_iterator
bool xccdf_refine_rule_iterator_has_more(struct xccdf_refine_rule_iterator *it);
/// @memberof xccdf_refine_rule_iterator
void xccdf_refine_rule_iterator_free(struct xccdf_refine_rule_iterator *it);

/** @struct xccdf_ident_iterator
 * Ident iterator.
 * @see oscap_iterator
 */
struct xccdf_ident_iterator;
/// @memberof xccdf_ident_iterator
struct xccdf_ident *xccdf_ident_iterator_next(struct xccdf_ident_iterator *it);
/// @memberof xccdf_ident_iterator
bool xccdf_ident_iterator_has_more(struct xccdf_ident_iterator *it);
/// @memberof xccdf_ident_iterator
void xccdf_ident_iterator_free(struct xccdf_ident_iterator *it);

/** @struct xccdf_check_iterator
 * Check iterator.
 * @see oscap_iterator
 */
struct xccdf_check_iterator;
/// @memberof xccdf_check_iterator
struct xccdf_check *xccdf_check_iterator_next(struct xccdf_check_iterator *it);
/// @memberof xccdf_check_iterator
bool xccdf_check_iterator_has_more(struct xccdf_check_iterator *it);
/// @memberof xccdf_check_iterator
void xccdf_check_iterator_free(struct xccdf_check_iterator *it);
/** @struct xccdf_check_content_ref_iterator
 * Check content references iterator.
 * @see oscap_iterator
 */
struct xccdf_check_content_ref_iterator;
/// @memberof xccdf_check_content_ref_iterator
struct xccdf_check_content_ref *xccdf_check_content_ref_iterator_next(struct xccdf_check_content_ref_iterator *it);
/// @memberof xccdf_check_content_ref_iterator
bool xccdf_check_content_ref_iterator_has_more(struct xccdf_check_content_ref_iterator *it);
/// @memberof xccdf_check_content_ref_iterator
void xccdf_check_content_ref_iterator_free(struct xccdf_check_content_ref_iterator *it);
/** @struct xccdf_profile_note_iterator
 * Profile note iterator.
 * @see oscap_iterator
 */
struct xccdf_profile_note_iterator;
/// @memberof xccdf_profile_note_iterator
struct xccdf_profile_note *xccdf_profile_note_iterator_next(struct xccdf_profile_note_iterator *it);
/// @memberof xccdf_profile_note_iterator
bool xccdf_profile_note_iterator_has_more(struct xccdf_profile_note_iterator *it);
/// @memberof xccdf_profile_note_iterator
void xccdf_profile_note_iterator_free(struct xccdf_profile_note_iterator *it);
/** @struct xccdf_check_import_iterator
 * Check import iterator.
 * @see oscap_iterator
 */
struct xccdf_check_import_iterator;
/// @memberof xccdf_check_import_iterator
struct xccdf_check_import *xccdf_check_import_iterator_next(struct xccdf_check_import_iterator *it);
/// @memberof xccdf_check_import_iterator
bool xccdf_check_import_iterator_has_more(struct xccdf_check_import_iterator *it);
/// @memberof xccdf_check_import_iterator
void xccdf_check_import_iterator_free(struct xccdf_check_import_iterator *it);

/** @struct xccdf_check_export_iterator
 * Check export iterator.
 * @see oscap_iterator
 */
struct xccdf_check_export_iterator;
/// @memberof xccdf_check_export_iterator
struct xccdf_check_export *xccdf_check_export_iterator_next(struct xccdf_check_export_iterator *it);
/// @memberof xccdf_check_export_iterator
bool xccdf_check_export_iterator_has_more(struct xccdf_check_export_iterator *it);
/// @memberof xccdf_check_export_iterator
void xccdf_check_export_iterator_free(struct xccdf_check_export_iterator *it);

/** @struct xccdf_fix_iterator
 * Fix iterator.
 * @see oscap_iterator
 */
struct xccdf_fix_iterator;
/// @memberof xccdf_fix_iterator
struct xccdf_fix *xccdf_fix_iterator_next(struct xccdf_fix_iterator *it);
/// @memberof xccdf_fix_iterator
bool xccdf_fix_iterator_has_more(struct xccdf_fix_iterator *it);
/// @memberof xccdf_fix_iterator
void xccdf_fix_iterator_free(struct xccdf_fix_iterator *it);

/** @struct xccdf_fixtext_iterator
 * Textual fix iterator.
 * @see oscap_iterator
 */
struct xccdf_fixtext_iterator;
/// @memberof xccdf_fixtext_iterator
struct xccdf_fixtext *xccdf_fixtext_iterator_next(struct xccdf_fixtext_iterator *it);
/// @memberof xccdf_fixtext_iterator
bool xccdf_fixtext_iterator_has_more(struct xccdf_fixtext_iterator *it);
/// @memberof xccdf_fixtext_iterator
void xccdf_fixtext_iterator_free(struct xccdf_fixtext_iterator *it);

/** @struct xccdf_warning_iterator
 * Warning iterator.
 * @see oscap_iterator
 */
struct xccdf_warning_iterator;
/// @memberof xccdf_warning_iterator
struct xccdf_warning *xccdf_warning_iterator_next(struct xccdf_warning_iterator *it);
/// @memberof xccdf_warning_iterator
bool xccdf_warning_iterator_has_more(struct xccdf_warning_iterator *it);
/// @memberof xccdf_warning_iterator
void xccdf_warning_iterator_free(struct xccdf_warning_iterator *it);

/**
 * @}
 */

/*--------------------*\
|     Item methods     |
\*--------------------*/

/**
 * Get item type.
 * @memberof xccdf_item
 */
xccdf_type_t xccdf_item_get_type(const struct xccdf_item *item);

/**
 * Get item ID.
 * @memberof xccdf_item
 */
const char *xccdf_item_get_id(const struct xccdf_item *item);

/**
 * Get item title.
 * @memberof xccdf_item
 */
struct oscap_text_iterator *xccdf_item_get_title(const struct xccdf_item *item);

/**
 * Get item description.
 * @memberof xccdf_item
 */
struct oscap_text_iterator *xccdf_item_get_description(const struct xccdf_item *item);

/**
 * Get item version.
 * @memberof xccdf_item
 */
const char *xccdf_item_get_version(const struct xccdf_item *item);

/**
 * Return item's parent in the inheritance hierarchy.
 * @memberof xccdf_item
 */
struct xccdf_item *xccdf_item_get_extends(const struct xccdf_item *item);

/**
 * Return item's parent in the grouping hierarchy.
 * Returned item will be either a group or a benchmark.
 * @memberof xccdf_item
 */
struct xccdf_item *xccdf_item_get_parent(const struct xccdf_item *item);

/**
 * Get an iterator to item statuses.
 * @memberof xccdf_item
 */
struct xccdf_status_iterator *xccdf_item_get_statuses(const struct xccdf_item *item);

/**
 * Get an iterator to the XCCDF references of the item.
 * @memberof xccdf_item
 */
struct xccdf_reference_iterator *xccdf_item_get_references(const struct xccdf_item *item);

/**
 * Get item current status.
 * @memberof xccdf_item
 */
xccdf_status_type_t xccdf_item_get_current_status(const struct xccdf_item *item);

/**
 * Convert the item to a benchmark.
 * @memberof xccdf_item
 * @return Pointer to this item as the benchmark.
 * @retval NULL on faliure (e.g. item is not a benchmark)
 */
struct xccdf_benchmark* xccdf_item_to_benchmark(struct xccdf_item* item);

/**
 * Convert the item to a profile.
 * @memberof xccdf_item
 * @return Pointer to this item as the profile.
 * @retval NULL on faliure (e.g. item is not a profile)
 */
struct xccdf_profile* xccdf_item_to_profile(struct xccdf_item* item);

/**
 * Convert the item to a rule.
 * @memberof xccdf_item
 * @return Pointer to this item as the rule.
 * @retval NULL on faliure (e.g. item is not a rule)
 */
struct xccdf_rule* xccdf_item_to_rule(struct xccdf_item* item);

/**
 * Convert the item to a group.
 * @memberof xccdf_item
 * @return Pointer to this item as the group.
 * @retval NULL on faliure (e.g. item is not a group)
 */
struct xccdf_group* xccdf_item_to_group(struct xccdf_item* item);

/**
 * Convert the item to a value.
 * @memberof xccdf_item
 * @return Pointer to this item as the value.
 * @retval NULL on faliure (e.g. item is not a value)
 */
struct xccdf_value* xccdf_item_to_value(struct xccdf_item* item);

/**
 * Convert the item to a test result.
 * @memberof xccdf_item
 * @return Pointer to this item as the test result.
 * @retval NULL on faliure (e.g. item is not a test result)
 */
struct xccdf_result* xccdf_item_to_result(struct xccdf_item* item);

/*--------------------*\
|   Benchmark methods  |
\*--------------------*/

/**
 * Create a new benmchmark from a benchmark XML file.
 * @memberof xccdf_benchmark
 * @param filename Name of the file to load the contents from.
 * @return Pointer to the new benchmark.
 * @retval NULL on failure
 */
struct xccdf_benchmark *xccdf_benchmark_parse_xml(const char *filename);

/**
 * Create a group and append it to the benchmark.
 * @param id - the identifier of the appended benchmark.
 * @returns the handle of the new group.
 */
struct xccdf_group *xccdf_benchmark_append_new_group(const struct xccdf_benchmark *, const char *id);

/**
 * Get benchmark ID.
 * @memberof xccdf_benchmark
 */
const char *xccdf_benchmark_get_id(const struct xccdf_benchmark *benchmark);

/**
 * Check benchmark resolved property.
 * @memberof xccdf_benchmark
 */
bool xccdf_benchmark_get_resolved(const struct xccdf_benchmark *benchmark);

/**
 * Get benchmark title.
 * @memberof xccdf_benchmark
 */
struct oscap_text_iterator *xccdf_benchmark_get_title(const struct xccdf_benchmark *benchmark);

/**
 * Get benchmark description.
 * @memberof xccdf_benchmark
 */
struct oscap_text_iterator *xccdf_benchmark_get_description(const struct xccdf_benchmark *benchmark);

/**
 * Get benchmark version.
 * @memberof xccdf_benchmark
 */
const char *xccdf_benchmark_get_version(const struct xccdf_benchmark *benchmark);

/**
 * Get benchmark inline stylesheet.
 * @memberof xccdf_benchmark
 */
const char *xccdf_benchmark_get_style(const struct xccdf_benchmark *benchmark);

/**
 * Get benchmark stylesheet URL.
 * @memberof xccdf_benchmark
 */
const char *xccdf_benchmark_get_style_href(const struct xccdf_benchmark *benchmark);

/**
 * Get benchmark front matter.
 * @memberof xccdf_benchmark
 */
struct oscap_text_iterator *xccdf_benchmark_get_front_matter(const struct xccdf_benchmark *benchmark);

/**
 * Get benchmark rear matter.
 * @memberof xccdf_benchmark
 */
struct oscap_text_iterator *xccdf_benchmark_get_rear_matter(const struct xccdf_benchmark *benchmark);

/**
 * Get benchmark metadata.
 * @memberof xccdf_benchmark
 */
const char *xccdf_benchmark_get_metadata(const struct xccdf_benchmark *benchmark);

/**
 * Get a plain text by ID.
 * @memberof xccdf_benchmark
 * @param id ID of the plain text to get.
 * @return Plain text content.
 * @retval NULL if given plain text does not exist
 */
const char *xccdf_benchmark_get_plain_text(const struct xccdf_benchmark *benchmark, const char *id);

/**
 * Get benchmark item by ID.
 * @memberof xccdf_benchmark
 * @param item ID
 * @return Item with given ID
 * @retval NULL if no such item exists
 */
struct xccdf_item *xccdf_benchmark_get_item(const struct xccdf_benchmark *benchmark, const char *id);

/**
 * Get an iterator to benchmark statuses.
 * @memberof xccdf_benchmark
 */
struct xccdf_status_iterator *xccdf_benchmark_get_statuses(const struct xccdf_benchmark *benchmark);

/**
 * Get an iterator to the XCCDF references of the benchmark.
 * @memberof xccdf_benchmark
 */
struct xccdf_reference_iterator *xccdf_benchmark_get_references(const struct xccdf_benchmark *benchmark);

/**
 * Get an iterator to the benchmark platforms.
 * @memberof xccdf_benchmark
 */
struct oscap_string_iterator *xccdf_benchmark_get_platforms(const struct xccdf_benchmark *benchmark);

/**
 * Get an iterator to the benchmark legal notices.
 * @memberof xccdf_benchmark
 * @see xccdf_notice
 */
struct xccdf_notice_iterator *xccdf_benchmark_get_notices(const struct xccdf_benchmark *benchmark);

/**
 * Get an iterator to the benchmark scoring models.
 * @memberof xccdf_benchmark
 * @see xccdf_model
 */
struct xccdf_model_iterator *xccdf_benchmark_get_models(const struct xccdf_benchmark *benchmark);

/**
 * Get an iterator to the benchmark XCCDF profiles.
 * @memberof xccdf_benchmark
 * @see xccdf_profile
 */
struct xccdf_profile_iterator *xccdf_benchmark_get_profiles(const struct xccdf_benchmark *benchmark);

/**
 * Get an iterator to the bencmark content. The items are either groups or rules.
 * @memberof xccdf_benchmark
 * @see xccdf_rule
 * @see xccdf_group
 * @see xccdf_item
 */
struct xccdf_item_iterator *xccdf_benchmark_get_content(const struct xccdf_benchmark *benchmark);

/**
 * Get benchmark current status.
 * @memberof xccdf_benchmark
 */
xccdf_status_type_t xccdf_benchmark_get_status_current(const struct xccdf_benchmark *benchmark);

/**
 * Delete the benchmark.
 * @memberof xccdf_benchmark
 * @param benchmark Benchmark to be deleted.
 */
void xccdf_benchmark_free(struct xccdf_benchmark *benchmark);

/**
 * Get supported version of XCCDF XML
 * @return version of XML file format
 * @memberof xccdf_benchmark
 */
const char * xccdf_benchmark_supported(void);


/*--------------------*\
|    Profile methods   |
\*--------------------*/

/**
 * Get profile ID.
 * @memberof xccdf_profile
 */
const char *xccdf_profile_get_id(const struct xccdf_profile *profile);

/**
 * Get profile title.
 * @memberof xccdf_profile
 */
struct oscap_text_iterator *xccdf_profile_get_title(const struct xccdf_profile *profile);

/**
 * Get profile description.
 * @memberof xccdf_profile
 */
struct oscap_text_iterator *xccdf_profile_get_description(const struct xccdf_profile *profile);

/**
 * Get profile version.
 * @memberof xccdf_profile
 */
const char *xccdf_profile_get_version(const struct xccdf_profile *profile);

/**
 * Return profile's parent in the inheritance hierarchy.
 * @memberof xccdf_profile
 */
struct xccdf_profile *xccdf_profile_get_extends(const struct xccdf_profile *profile);

/**
 * Return a benchmark containing this profile.
 * @memberof xccdf_profile
 */
struct xccdf_benchmark *xccdf_profile_get_benchmark(const struct xccdf_profile *profile);

/**
 * Return whether the profile is abstract.
 * @memberof xccdf_profile
 */
bool xccdf_profile_get_abstract(const struct xccdf_profile *profile);

/**
 * Return whether the profile has the prohibit changes flag set to true.
 * @memberof xccdf_profile
 */
bool xccdf_profile_get_prohibit_changes(const struct xccdf_profile *profile);

/**
 * Get an iterator to profile platforms.
 * @memberof xccdf_profile
 */
struct oscap_string_iterator *xccdf_profile_get_platforms(const struct xccdf_profile *profile);

/**
 * Get an iterator to profile statuses.
 * @memberof xccdf_profile
 */
struct xccdf_status_iterator *xccdf_profile_get_statuses(const struct xccdf_profile *profile);

/**
 * Get an iterator to the XCCDF references of the profile.
 * @memberof xccdf_profile
 */
struct xccdf_reference_iterator *xccdf_profile_get_references(const struct xccdf_profile *profile);

/**
 * Get profile current status.
 * @memberof xccdf_profile
 */
xccdf_status_type_t xccdf_profile_get_status_current(const struct xccdf_profile *profile);

/**
 * Get profile note tag.
 * @memberof xccdf_profile
 */
/* const char* xccdf_profile_note_get_tag(const struct xccdf_profile* profile); TODO */

/**
 * Get an iterator to the select options of the profile.
 * @memberof xccdf_profile
 */
struct xccdf_select_iterator *xccdf_profile_get_selects(const struct xccdf_profile *profile);

/**
 * Get an iterator to the set value options of the profile.
 * @memberof xccdf_profile
 */
struct xccdf_set_value_iterator *xccdf_profile_get_set_values(const struct xccdf_profile *profile);

/**
 * Get an iterator to the refine value options of the profile.
 * @memberof xccdf_profile
 */
struct xccdf_refine_value_iterator *xccdf_profile_get_refine_values(const struct xccdf_profile *profile);

/**
 * Get an iterator to the refine rule options of the profile.
 * @memberof xccdf_profile
 */
struct xccdf_refine_rule_iterator *xccdf_profile_get_refine_rules(const struct xccdf_profile *profile);

/*--------------------*\
|     Rule methods     |
\*--------------------*/

/**
 * Get rule ID.
 * @memberof xccdf_rule
 */
const char *xccdf_rule_get_id(const struct xccdf_rule *rule);

/**
 * Get rule title.
 * @memberof xccdf_rule
 */
struct oscap_text_iterator *xccdf_rule_get_title(const struct xccdf_rule *rule);

/**
 * Get rule description.
 * @memberof xccdf_rule
 */
struct oscap_text_iterator *xccdf_rule_get_description(const struct xccdf_rule *rule);

/**
 * Get rule version.
 * @memberof xccdf_rule
 */
const char *xccdf_rule_get_version(const struct xccdf_rule *rule);

/**
 * Get rule question.
 * @memberof xccdf_rule
 */
struct oscap_text_iterator *xccdf_rule_get_question(const struct xccdf_rule *rule);

/**
 * Get rule question.
 * @memberof xccdf_rule
 */
struct xccdf_warning_iterator *xccdf_rule_get_warnings(const struct xccdf_rule *rule);

/**
 * Get rule rationale.
 * @memberof xccdf_rule
 */
struct oscap_text_iterator *xccdf_rule_get_rationale(const struct xccdf_rule *rule);

/**
 * Get rule cluster ID.
 * @memberof xccdf_rule
 */
const char *xccdf_rule_get_cluster_id(const struct xccdf_rule *rule);

/**
 * Get rule's currently selected check.
 * @memberof xccdf_rule
 */
struct xccdf_check *xccdf_rule_get_check(const struct xccdf_rule *rule);

/**
 * Get rule scoring weight.
 * @memberof xccdf_rule
 */
float xccdf_rule_get_weight(const struct xccdf_rule *rule);

/**
 * Return rule's parent in the inheritance hierarchy.
 * @memberof xccdf_rule
 */
struct xccdf_rule *xccdf_rule_get_extends(const struct xccdf_rule *rule);

/**
 * Return rule's parent in the grouping hierarchy.
 * Returned item will be either a group or a benchmark.
 * @memberof xccdf_rule
 */
struct xccdf_item *xccdf_rule_get_parent(const struct xccdf_rule *rule);

/**
 * Return whether the rule is abstract.
 * @memberof xccdf_rule
 */
bool xccdf_rule_get_abstract(const struct xccdf_rule *rule);

/**
 * Return whether the rule has the prohibit changes flag set to true.
 * @memberof xccdf_rule
 */
bool xccdf_rule_get_prohibit_changes(const struct xccdf_rule *rule);

/**
 * Return whether the rule has the hidden flag set to true.
 * @memberof xccdf_rule
 */
bool xccdf_rule_get_hidden(const struct xccdf_rule *rule);

/**
 * Return whether the rule is selected.
 * @memberof xccdf_rule
 */
bool xccdf_rule_get_selected(const struct xccdf_rule *rule);

/**
 * Return whether the rule has the multiple flag set to true.
 * @memberof xccdf_rule
 */
bool xccdf_rule_get_multiple(const struct xccdf_rule *rule);

/**
 * Get an iterator to rule platforms.
 * @memberof xccdf_rule
 */
struct oscap_string_iterator *xccdf_rule_get_platforms(const struct xccdf_rule *rule);

/**
 * Get an iterator to rule statuses.
 * @memberof xccdf_rule
 */
struct xccdf_status_iterator *xccdf_rule_get_statuses(const struct xccdf_rule *rule);

/**
 * Get an iterator to the XCCDF references of the rule.
 * @memberof xccdf_rule
 */
struct xccdf_reference_iterator *xccdf_rule_get_references(const struct xccdf_rule *rule);

/**
 * Get rule current status.
 * @memberof xccdf_rule
 */
xccdf_status_type_t xccdf_rule_get_status_current(const struct xccdf_rule *rule);

/**
 * Get rule impact metric.
 * @memberof xccdf_rule
 */
const char *xccdf_rule_get_impact_metric(const struct xccdf_rule *rule);

/**
 * Get rule role.
 * @memberof xccdf_rule
 */
xccdf_role_t xccdf_rule_get_role(const struct xccdf_rule *rule);

/**
 * Get rule severity level.
 * @memberof xccdf_rule
 */
xccdf_level_t xccdf_rule_get_severity(const struct xccdf_rule *rule);

/**
 * Get an iterator to the idents of the rule.
 * @memberof xccdf_rule
 */
struct xccdf_ident_iterator *xccdf_rule_get_idents(const struct xccdf_rule *rule);

/**
 * Get an iterator to the checks of the rule.
 * @memberof xccdf_rule
 */
struct xccdf_check_iterator *xccdf_rule_get_checks(const struct xccdf_rule *rule);

/**
 * Get an iterator to the profile notes of the rule.
 * @memberof xccdf_rule
 */
struct xccdf_profile_note_iterator *xccdf_rule_get_profile_notes(const struct xccdf_rule *rule);

/**
 * Get an iterator to fixes for the rule.
 * @memberof xccdf_rule
 */
struct xccdf_fix_iterator *xccdf_rule_get_fixes(const struct xccdf_rule *rule);

/**
 * Get an iterator to fix descriptions the rule.
 * @memberof xccdf_rule
 */
struct xccdf_fixtext_iterator *xccdf_rule_get_fixtexts(const struct xccdf_rule *rule);

/*--------------------*\
|     Group methods    |
\*--------------------*/

/**
 * Get group ID.
 * @memberof xccdf_group
 */
const char *xccdf_group_get_id(const struct xccdf_group *group);

/**
 * Get group title.
 * @memberof xccdf_group
 */
struct oscap_text_iterator *xccdf_group_get_title(const struct xccdf_group *group);

/**
 * Get group description.
 * @memberof xccdf_group
 */
struct oscap_text_iterator *xccdf_group_get_description(const struct xccdf_group *group);

/**
 * Get group version.
 * @memberof xccdf_group
 */
const char *xccdf_group_get_version(const struct xccdf_group *group);

/**
 * Get group question.
 * @memberof xccdf_group
 */
struct oscap_text_iterator *xccdf_group_get_question(const struct xccdf_group *group);

/**
 * Get group question.
 * @memberof xccdf_group
 */
struct xccdf_warning_iterator *xccdf_group_get_warnings(const struct xccdf_group *group);

/**
 * Get group rationale.
 * @memberof xccdf_group
 */
struct oscap_text_iterator *xccdf_group_get_rationale(const struct xccdf_group *group);

/**
 * Get group cluster ID.
 * @memberof xccdf_group
 */
const char *xccdf_group_get_cluster_id(const struct xccdf_group *group);

/**
 * Get group scoring weight.
 * @memberof xccdf_group
 */
float xccdf_group_get_weight(const struct xccdf_group *group);

/**
 * Return group's parent in the inheritance hierarchy.
 * @memberof xccdf_group
 */
struct xccdf_group *xccdf_group_get_extends(const struct xccdf_group *group);

/**
 * Return group's parent in the grouping hierarchy.
 * Returned item will be either a group or a benchmark.
 * @memberof xccdf_group
 */
struct xccdf_item *xccdf_group_get_parent(const struct xccdf_group *group);

/**
 * Return whether the group is abstract.
 * @memberof xccdf_group
 */
bool xccdf_group_get_abstract(const struct xccdf_group *group);

/**
 * Return whether the group has the prohibit changes flag set to true.
 * @memberof xccdf_group
 */
bool xccdf_group_get_prohibit_changes(const struct xccdf_group *group);

/**
 * Return whether the group has the hidden flag set to true.
 * @memberof xccdf_group
 */
bool xccdf_group_get_hidden(const struct xccdf_group *group);

/**
 * Return whether the group is selected.
 * @memberof xccdf_group
 */
bool xccdf_group_get_selected(const struct xccdf_group *group);

/**
 * Get an iterator to group platforms.
 * @memberof xccdf_group
 */
struct oscap_string_iterator *xccdf_group_get_platforms(const struct xccdf_group *group);

/**
 * Get an iterator to group statuses.
 * @memberof xccdf_group
 */
struct xccdf_status_iterator *xccdf_group_get_statuses(const struct xccdf_group *group);

/**
 * Get an iterator to the XCCDF references of the group.
 * @memberof xccdf_group
 */
struct xccdf_reference_iterator *xccdf_group_get_references(const struct xccdf_group *group);

/**
 * Get an iterator to the group content. The items are either groups or rules.
 * @memberof xccdf_group
 * @see xccdf_rule
 * @see xccdf_group
 * @see xccdf_item
 */
struct xccdf_item_iterator *xccdf_group_get_content(const struct xccdf_group *benchmark);

/**
 * Get group current status.
 * @memberof xccdf_group
 */
xccdf_status_type_t xccdf_group_get_status_current(const struct xccdf_group *group);

/*--------------------*\
|     Value methods    |
\*--------------------*/

/**
 * Get value ID.
 * @memberof xccdf_value
 */
const char *xccdf_value_get_id(const struct xccdf_value *value);

/**
 * Get value title.
 * @memberof xccdf_value
 */
struct oscap_text_iterator *xccdf_value_get_title(const struct xccdf_value *value);

/**
 * Get value description.
 * @memberof xccdf_value
 */
struct oscap_text_iterator *xccdf_value_get_description(const struct xccdf_value *value);

/**
 * Return value's parent in the inheritance hierarchy.
 * @memberof xccdf_value
 */
struct xccdf_value *xccdf_value_get_extends(const struct xccdf_value *value);

/**
 * Return value's parent in the grouping hierarchy.
 * Returned item will be either a value or a benchmark.
 * @memberof xccdf_value
 */
struct xccdf_item *xccdf_value_get_parent(const struct xccdf_value *value);

/**
 * Return whether the value is abstract.
 * @memberof xccdf_value
 */
bool xccdf_value_get_abstract(const struct xccdf_value *value);

/**
 * Return whether the value has the prohibit changes flag set to true.
 * @memberof xccdf_value
 */
bool xccdf_value_get_prohibit_changes(const struct xccdf_value *value);

/**
 * Return whether the value has the hidden flag set to true.
 * @memberof xccdf_value
 */
bool xccdf_value_get_hidden(const struct xccdf_value *value);

/**
 * Get an iterator to value statuses.
 * @memberof xccdf_value
 */
struct xccdf_status_iterator *xccdf_value_get_statuses(const struct xccdf_value *value);

/**
 * Get an iterator to the XCCDF references of the value.
 * @memberof xccdf_value
 */
struct xccdf_reference_iterator *xccdf_value_get_references(const struct xccdf_value *value);

/**
 * Get value current status.
 * @memberof xccdf_value
 */
xccdf_status_type_t xccdf_value_get_status_current(const struct xccdf_value *value);

/**
 * Return type of the value.
 * @memberof xccdf_value
 */
xccdf_value_type_t xccdf_value_get_type(const struct xccdf_value *value);

/**
 * Return an interface hint for the value.
 * @memberof xccdf_value
 */
xccdf_interface_hint_t xccdf_value_get_interface_hint(const struct xccdf_value *value);

/**
 * Return an operator to be applied on the value.
 * @memberof xccdf_value
 */
xccdf_operator_t xccdf_value_get_oper(const struct xccdf_value *value);

/**
 * Return currently active selector of the value.
 * @memberof xccdf_value
 */
const char *xccdf_value_get_selector(const struct xccdf_value *value);

/**
 * Apply a selector on the value.
 * @memberof xccdf_value
 * @param The selector character string.
 * @return whether setting the new selector was successful
 */
bool xccdf_value_get_set_selector(struct xccdf_item *value, const char *selector);

/**
 * Return the item's value as a string.
 * @memberof xccdf_value
 * @return string value
 * @retval NULL if value is not a string or is not set
 */
const char *xccdf_value_get_value_string(const struct xccdf_value *value);

/**
 * Return the item's value as a number.
 * @memberof xccdf_value
 * @return numeric value
 * @retval NAN if value is not a number or is not set.
 */
xccdf_numeric xccdf_value_get_value_number(const struct xccdf_value *value);

/**
 * Return the item's value as a boolean value.
 * Implicit conversion is performed on non-boolean types:
 * value is true for nonempty strings and nonzero numbers.
 * @memberof xccdf_value
 * @return boolean value
 */
bool xccdf_value_get_value_boolean(const struct xccdf_value *value);

/**
 * Return the item's default value as a string.
 * @memberof xccdf_value
 * @return string value
 * @retval NULL if value is not a string or is not set
 */
const char *xccdf_value_get_defval_string(const struct xccdf_value *value);

/**
 * Return the item's default value as a number.
 * @memberof xccdf_value
 * @return numeric value
 * @retval NAN if value is not a number or is not set.
 */
xccdf_numeric xccdf_value_get_defval_number(const struct xccdf_value *value);

/**
 * Return the item's default value as a boolean value.
 * Implicit conversion is performed on non-boolean types:
 * value is true for nonempty strings and nonzero numbers.
 * @memberof xccdf_value
 * @return boolean value
 */
bool xccdf_value_get_defval_boolean(const struct xccdf_value *value);

/**
 * Return upper limit for the numeric value.
 * @memberof xccdf_value
 * @retval NAN if value is not a number or lower limit is not set.
 */
xccdf_numeric xccdf_value_get_lower_bound(const struct xccdf_value *value);

/**
 * Return lower limit for the numeric value.
 * @memberof xccdf_value
 * @retval NAN if value is not a number or upper limit is not set.
 */
xccdf_numeric xccdf_value_get_upper_bound(const struct xccdf_value *value);

/**
 * Regex the values should match.
 * @memberof xccdf_value
 * @retval NULL if regex was not set or the value is not a string.
 */
const char *xccdf_value_get_match(const struct xccdf_value *value);

/**
 * Return mustMatch property.
 * Returns whether value must match conditions given by choices,
 * or these are just hints.
 * @memberof xccdf_value
 */
bool xccdf_value_get_must_match(const struct xccdf_value *value);

/*
 * Get an iterator to the list of XCCDF value's possible (or suggested) values.
 * @ralates xccdf_value
 * @retval NULL on failure (e.g. the value is not a string)
 */
// struct oscap_string_iterator* xccdf_value_choices_string(const struct xccdf_value* value);

/**
 * Get an iterator to the XCCDF value's source URIs.
 * @memberof xccdf_value
 */
/* struct oscap_string_iterator* xccdf_value_sources(const struct xccdf_value* value); TODO */

/*--------------------*\
|     Other methods    |
\*--------------------*/

/**
 * Get status date.
 * @memberof xccdf_status
 */
time_t xccdf_status_get_date(const struct xccdf_status *status);

/**
 * Get status type.
 * @memberof xccdf_status
 */
xccdf_status_type_t xccdf_status_get_status(const struct xccdf_status *status);

/**
 * Get copyright notice ID.
 * @memberof xccdf_notice
 */
const char *xccdf_notice_get_id(const struct xccdf_notice *notice);

/**
 * Get copyright notice text.
 * @memberof xccdf_notice
 */
struct oscap_text *xccdf_notice_get_text(const struct xccdf_notice *notice);

/**
 * Get scoring model system URI.
 * @memberof xccdf_model
 */
const char *xccdf_model_get_system(const struct xccdf_model *model);

/**
 * Get scoring model parameter.
 * @memberof xccdf_model
 */
/* const char* xccdf_model_get_param(const struct xccdf_model* model, const char* param_name); TODO */

/**
 * Get ident ID.
 * @memberof xccdf_ident
 */
const char *xccdf_ident_get_id(const struct xccdf_ident *ident);

/**
 * Get ident system URI.
 * @memberof xccdf_ident
 */
const char *xccdf_ident_get_system(const struct xccdf_ident *ident);

/**
 * Get check ID.
 * @memberof xccdf_check
 */
const char *xccdf_check_get_id(const struct xccdf_check *check);

/**
 * True if the check is a complex check.
 * @memberof xccdf_check
 * @see xccdf_check_get_children
 */
bool xccdf_check_get_complex(const struct xccdf_check *check);

/**
 * Get an operator to be applied no children of the complex check.
 * @memberof xccdf_check
 * @see xccdf_check_get_children
 */
xccdf_bool_operator_t xccdf_check_get_oper(const struct xccdf_check *check);

/**
 * Get check system URI.
 * @memberof xccdf_check
 */
const char *xccdf_check_get_system(const struct xccdf_check *check);

/**
 * Get check selector.
 * @memberof xccdf_check
 */
const char *xccdf_check_get_selector(const struct xccdf_check *check);

/**
 * Get check content.
 * @memberof xccdf_check
 */
const char *xccdf_check_get_content(const struct xccdf_check *check);

/**
 * Get rule containing this check.
 * @memberof xccdf_check
 */
struct xccdf_rule *xccdf_check_get_parent(const struct xccdf_check *check);

/**
 * Get an iterator to the check content references.
 * @memberof xccdf_check
 * @see xccdf_check_get_content_ref
 */
/*  struct xccdf_check_content_ref_iterator* xccdf_check_content_refs(const struct xccdf_check* check); TODO */

/**
 * Get an iterator to the check imports.
 * @memberof xccdf_check
 * @see xccdf_check_get_import
 */
/* struct xccdf_check_import_iterator* xccdf_check_imports(const struct xccdf_check* check); TODO */

/**
 * Get an iterator to the check exports.
 * @memberof xccdf_check
 * @see xccdf_check_get_export
 */
/* struct xccdf_check_export_iterator* xccdf_check_exports(const struct xccdf_check* check); TODO */

/**
 * Get an iterator to nested checks of the complex check.
 * @memberof xccdf_check
 * @see xccdf_check_get_export
 */
struct xccdf_check_iterator *xccdf_check_get_children(const struct xccdf_check *check);

/**
 * Get URI of the check content reference.
 * @memberof xccdf_check_content_ref
 */
const char *xccdf_check_content_ref_get_href(const struct xccdf_check_content_ref *ref);

/**
 * Get name of a part of the check content document this reference is pointing to.
 * @memberof xccdf_check_content_ref
 */
const char *xccdf_check_content_ref_get_name(const struct xccdf_check_content_ref *ref);

/**
 * Get profile note tag.
 * @memberof xccdf_profile_note
 */
const char *xccdf_profile_note_get_reftag(const struct xccdf_profile_note *note);

/**
 * Get profile note text as a piece of XML.
 * @memberof xccdf_profile_note
 */
const char *xccdf_profile_note_get_text(const struct xccdf_profile_note *note);

/**
 * Get check import target system variable name.
 * @memberof xccdf_check_import
 */
const char *xccdf_check_import_get_name(const struct xccdf_check_import *item);

/**
 * Get content of the imported variable.
 * @memberof xccdf_check_import
 */
const char *xccdf_check_import_get_content(const struct xccdf_check_import *item);

/**
 * Get check export target system variable name.
 * @memberof xccdf_check_export
 */
const char *xccdf_check_export_get_name(const struct xccdf_check_export *item);

/**
 * Get an XCCDF value bound to the check export item.
 * @memberof xccdf_check_export
 */
struct xccdf_value *xccdf_check_export_get_value(const struct xccdf_check_export *item);

/**
 * Get fix content (e.g. a script).
 * @memberof xccdf_fix
 */
const char *xccdf_fix_get_content(const struct xccdf_fix *fix);

/**
 * True if the fix needs rebooting a system.
 * @memberof xccdf_fix
 */
bool xccdf_fix_get_reboot(const struct xccdf_fix *fix);

/**
 * Get fix strategy.
 * @memberof xccdf_fix
 */
xccdf_strategy_t xccdf_fix_get_strategy(const struct xccdf_fix *fix);

/**
 * Get fix complexity.
 * @memberof xccdf_fix
 */
xccdf_level_t xccdf_fix_get_complexity(const struct xccdf_fix *fix);

/**
 * Get fix disruption.
 * @memberof xccdf_fix
 */
xccdf_level_t xccdf_fix_get_disruption(const struct xccdf_fix *fix);

/**
 * Get fix ID,
 * @memberof xccdf_fix
 */
const char *xccdf_fix_get_id(const struct xccdf_fix *fix);

/**
 * Get fix fixing system URI,
 * @memberof xccdf_fix
 */
const char *xccdf_fix_get_system(const struct xccdf_fix *fix);

/**
 * Get URI of a platform the fix applies to.
 * @memberof xccdf_fix
 */
const char *xccdf_fix_get_platform(const struct xccdf_fix *fix);

/**
 * True if the fixtext needs rebooting a system.
 * @memberof xccdf_fixtext
 */
bool xccdf_fixtext_get_reboot(const struct xccdf_fixtext *fixtext);

/**
 * Get fixtext strategy.
 * @memberof xccdf_fixtext
 */
xccdf_strategy_t xccdf_fixtext_get_strategy(const struct xccdf_fixtext *fixtext);

/**
 * Get fixtext complexity.
 * @memberof xccdf_fixtext
 */
xccdf_level_t xccdf_fixtext_get_complexity(const struct xccdf_fixtext *fixtext);

/**
 * Get fixtext disruption.
 * @memberof xccdf_fixtext
 */
xccdf_level_t xccdf_fixtext_get_disruption(const struct xccdf_fixtext *fixtext);

/**
 * Get a fix this text references to.
 * @memberof xccdf_fixtext
 */
struct xccdf_fix *xccdf_fixtext_get_fixref(const struct xccdf_fixtext *fixtext);

/* Get/Set functions */

/// @memberof xccdf_value
const char *xccdf_value_get_id(const struct xccdf_value *value);
/// @memberof xccdf_value
const char *xccdf_value_get_version(const struct xccdf_value *value);
/// @memberof xccdf_value
struct oscap_text_iterator *xccdf_value_get_question(const struct xccdf_value *value);
/// @memberof xccdf_value
struct xccdf_warning_iterator *xccdf_value_get_warnings(const struct xccdf_value *value);
/// @memberof xccdf_value
struct oscap_text_iterator *xccdf_value_get_rationale(const struct xccdf_value *value);
/// @memberof xccdf_value
const char *xccdf_value_get_cluster_id(const struct xccdf_value *value);
/// @memberof xccdf_value
const char *xccdf_value_get_version_update(const struct xccdf_value *value);
/// @memberof xccdf_value
time_t xccdf_value_get_version_time(const struct xccdf_value *value);
/// @memberof xccdf_value
float xccdf_value_get_weight(const struct xccdf_value *value);
/// @memberof xccdf_value
struct xccdf_benchmark *xccdf_value_get_benchmark(const struct xccdf_value *value);
/// @memberof xccdf_value
struct oscap_string_iterator *xccdf_value_get_platforms(const struct xccdf_value *value);
/// @memberof xccdf_value
struct oscap_string_iterator *xccdf_value_get_sources(const struct xccdf_value *value);

/// @memberof xccdf_value
bool xccdf_value_get_resolved(const struct xccdf_value *value);
/// @memberof xccdf_value
bool xccdf_value_get_hidden(const struct xccdf_value *value);
/// @memberof xccdf_value
bool xccdf_value_get_selected(const struct xccdf_value *value);
/// @memberof xccdf_value
bool xccdf_value_get_multiple(const struct xccdf_value *value);
/// @memberof xccdf_value
bool xccdf_value_get_prohibit_changes(const struct xccdf_value *value);
/// @memberof xccdf_value
bool xccdf_value_get_abstract(const struct xccdf_value *value);
/// @memberof xccdf_value
bool xccdf_value_get_interactive(const struct xccdf_value *value);

/// @memberof xccdf_item
const char *xccdf_item_get_id(const struct xccdf_item *item);
/// @memberof xccdf_item
const char *xccdf_item_get_version(const struct xccdf_item *item);
/// @memberof xccdf_item
struct oscap_text_iterator *xccdf_item_get_question(const struct xccdf_item *item);
/// @memberof xccdf_item
struct xccdf_warning_iterator *xccdf_item_get_warnings(const struct xccdf_item *item);
/// @memberof xccdf_item
struct oscap_text_iterator *xccdf_item_get_rationale(const struct xccdf_item *item);
/// @memberof xccdf_item
const char *xccdf_item_get_cluster_id(const struct xccdf_item *item);
/// @memberof xccdf_item
const char *xccdf_item_get_version_update(const struct xccdf_item *item);
/// @memberof xccdf_item
time_t xccdf_item_get_version_time(const struct xccdf_item *item);
/// @memberof xccdf_item
float xccdf_item_get_weight(const struct xccdf_item *item);
/// @memberof xccdf_item
struct xccdf_benchmark *xccdf_item_get_benchmark(const struct xccdf_item *item);
/// @memberof xccdf_item
struct oscap_string_iterator *xccdf_item_get_platforms(const struct xccdf_item *item);

/// @memberof xccdf_benchmark
bool xccdf_benchmark_get_resolved(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_get_hidden(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_get_selected(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_get_multiple(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_get_prohibit_changes(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_get_abstract(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_get_interactive(const struct xccdf_benchmark *benchmark);

//const char* xccdf_bechmark_get_id(const struct xccdf_benchmark* benchmark);
//const char *xccdf_benchmark_get_title(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
const char *xccdf_benchmark_get_version(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
struct oscap_text_iterator *xccdf_benchmark_get_question(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
struct xccdf_warning_iterator *xccdf_benchmark_get_warnings(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
struct oscap_text_iterator *xccdf_benchmark_get_rationale(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
const char *xccdf_benchmark_get_cluster_id(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
const char *xccdf_benchmark_get_version_update(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
time_t xccdf_benchmark_get_version_time(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
float xccdf_benchmark_get_weight(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
struct xccdf_benchmark *xccdf_benchmark_get_benchmark(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
struct xccdf_item *xccdf_benchmark_get_parent(const struct xccdf_benchmark *benchmark);

/// @memberof xccdf_profile
const char *xccdf_profile_get_id(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
const char *xccdf_profile_get_version(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
struct oscap_text_iterator *xccdf_profile_get_question(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
struct oscap_text_iterator *xccdf_profile_get_rationale(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
const char *xccdf_profile_get_cluster_id(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
const char *xccdf_profile_get_version_update(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
time_t xccdf_profile_get_version_time(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
float xccdf_profile_get_weight(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
struct xccdf_item *xccdf_profile_get_parent(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
const char *xccdf_profile_get_note_tag(const struct xccdf_profile *profile);

/// @memberof xccdf_profile
bool xccdf_profile_get_resolved(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
bool xccdf_profile_get_hidden(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
bool xccdf_profile_get_selected(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
bool xccdf_profile_get_multiple(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
bool xccdf_profile_get_prohibit_changes(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
bool xccdf_profile_get_abstract(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
bool xccdf_profile_get_interactive(const struct xccdf_profile *profile);

/// @memberof xccdf_rule
const char *xccdf_rule_get_id(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
const char *xccdf_rule_get_version(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
const char *xccdf_rule_get_cluster_id(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
const char *xccdf_rule_get_version_update(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
time_t xccdf_rule_get_version_time(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
float xccdf_rule_get_weight(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
struct xccdf_benchmark *xccdf_rule_get_benchmark(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
struct oscap_string_iterator *xccdf_rule_get_platforms(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
bool xccdf_rule_get_resolved(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
bool xccdf_rule_get_interactive(const struct xccdf_rule *rule);

/// @memberof xccdf_group
bool xccdf_group_get_resolved(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_get_hidden(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_get_selected(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_get_multiple(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_get_prohibit_changes(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_get_abstract(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_get_interactive(const struct xccdf_group *group);
/// @memberof xccdf_group
time_t xccdf_group_get_version_time(const struct xccdf_group *group);
/// @memberof xccdf_group
const char *xccdf_group_get_version_update(const struct xccdf_group *group);
/// @memberof xccdf_group
float xccdf_group_get_weight(const struct xccdf_group *group);
/// @memberof xccdf_group
struct xccdf_benchmark *xccdf_group_get_benchmark(const struct xccdf_group *group);

/// @memberof xccdf_check
struct xccdf_check_import_iterator *xccdf_check_get_imports(const struct xccdf_check *check);
/// @memberof xccdf_check
struct xccdf_check_export_iterator *xccdf_check_get_exports(const struct xccdf_check *check);
/// @memberof xccdf_check
struct xccdf_check_content_ref_iterator *xccdf_check_get_content_refs(const struct xccdf_check *check);
/// @memberof xccdf_check
const char *xccdf_fixtext_get_content(const struct xccdf_fixtext *fixtext);

/// @memberof xccdf_reference
bool xccdf_reference_get_override(const struct xccdf_reference *reference);
/// @memberof xccdf_reference
const char *xccdf_reference_get_href(const struct xccdf_reference *reference);
/// @memberof xccdf_reference
const char *xccdf_reference_get_text(const struct xccdf_reference *reference);


/// @memberof xccdf_select
bool xccdf_select_get_selected(const struct xccdf_select *select);
/// @memberof xccdf_select
const char *xccdf_select_get_idref(const struct xccdf_select *select);
/// @memberof xccdf_select
const char *xccdf_select_get_remark(const struct xccdf_select *select);
/// @memberof xccdf_select
struct xccdf_item *xccdf_select_get_item(const struct xccdf_select *select);

/// @memberof xccdf_warning
xccdf_warning_category_t xccdf_warning_get_category(const struct xccdf_warning *warning);
/// @memberof xccdf_warning
struct oscap_text *xccdf_warning_get_text(const struct xccdf_warning *warning);

/**
 * Release library internal caches.
 * @deprecated Use oscap_cleanup() instead.
 */
void xccdf_cleanup(void);


/*--------------------*\
|   Benchmark methods  |
\*--------------------*/

/**
 * Create a new benchmark from a benchmark XML file.
 * @memberof xccdf_benchmark
 * @param id the benchmark identifier.
 * @return Pointer to the new benchmark.
 */
struct xccdf_benchmark *xccdf_benchmark_create(const char *id);

/**
 * Create a group and append it to the benchmark.
 * @param id - the identifier of the appended value.
 * @return the handle of the new group.
 */
struct xccdf_group *xccdf_benchmark_append_new_group(const struct xccdf_benchmark *, const char *id);

/**
 * Create a value and append it to the benchmark.
 * @param id - the identifier of the appended value.
 * @return the handle of the new value.
 */
struct xccdf_value *xccdf_benchmark_append_new_value(const struct xccdf_benchmark *, const char *id, xccdf_value_type_t type);

/**
 * Create a rule and append it to the benchmark.
 * @param id - the identifier of the appended rule.
 * @return the handle of the new rule.
 */
struct xccdf_rule *xccdf_benchmark_append_new_rule(const struct xccdf_benchmark *, const char *id);


/// @memberof xccdf_item
void xccdf_item_set_title(const struct xccdf_item* item, const char *title);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_title(const struct xccdf_benchmark* item, const char *title);
/// @memberof xccdf_profile
void xccdf_profile_set_title(const struct xccdf_profile* item, const char *title);
/// @memberof xccdf_rule
void xccdf_rule_set_title(const struct xccdf_rule* item, const char *title);
/// @memberof xccdf_value
void xccdf_value_set_title(const struct xccdf_value* item, const char *title);
/// @memberof xccdf_group
void xccdf_group_set_title(const struct xccdf_group* item, const char *title);
/// @memberof xccdf_item
void xccdf_item_set_description(const struct xccdf_item* item, const char *description);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_description(const struct xccdf_benchmark* item, const char *description);
/// @memberof xccdf_profile
void xccdf_profile_set_description(const struct xccdf_profile* item, const char *description);
/// @memberof xccdf_rule
void xccdf_rule_set_description(const struct xccdf_rule* item, const char *description);
/// @memberof xccdf_value
void xccdf_value_set_description(const struct xccdf_value* item, const char *description);
/// @memberof xccdf_group
void xccdf_group_set_description(const struct xccdf_group* item, const char *description);
/// @memberof xccdf_item
void xccdf_item_set_question(const struct xccdf_item* item, const char *question);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_question(const struct xccdf_benchmark* item, const char *question);
/// @memberof xccdf_profile
void xccdf_profile_set_question(const struct xccdf_profile* item, const char *question);
/// @memberof xccdf_rule
void xccdf_rule_set_question(const struct xccdf_rule* item, const char *question);
/// @memberof xccdf_value
void xccdf_value_set_question(const struct xccdf_value* item, const char *question);
/// @memberof xccdf_group
void xccdf_group_set_question(const struct xccdf_group* item, const char *question);
/// @memberof xccdf_item
void xccdf_item_set_rationale(const struct xccdf_item* item, const char *rationale);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_rationale(const struct xccdf_benchmark* item, const char *rationale);
/// @memberof xccdf_profile
void xccdf_profile_set_rationale(const struct xccdf_profile* item, const char *rationale);
/// @memberof xccdf_rule
void xccdf_rule_set_rationale(const struct xccdf_rule* item, const char *rationale);
/// @memberof xccdf_value
void xccdf_value_set_rationale(const struct xccdf_value* item, const char *rationale);
/// @memberof xccdf_group
void xccdf_group_set_rationale(const struct xccdf_group* item, const char *rationale);
/// @memberof xccdf_item
void xccdf_item_set_cluster_id(const struct xccdf_item* item, const char *cluster_id);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_cluster_id(const struct xccdf_benchmark* item, const char *cluster_id);
/// @memberof xccdf_profile
void xccdf_profile_set_cluster_id(const struct xccdf_profile* item, const char *cluster_id);
/// @memberof xccdf_rule
void xccdf_rule_set_cluster_id(const struct xccdf_rule* item, const char *cluster_id);
/// @memberof xccdf_value
void xccdf_value_set_cluster_id(const struct xccdf_value* item, const char *cluster_id);
/// @memberof xccdf_group
void xccdf_group_set_cluster_id(const struct xccdf_group* item, const char *cluster_id);
/// @memberof xccdf_item
void xccdf_item_set_version(const struct xccdf_item* item, const char *version);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_version(const struct xccdf_benchmark* item, const char *version);
/// @memberof xccdf_profile
void xccdf_profile_set_version(const struct xccdf_profile* item, const char *version);
/// @memberof xccdf_rule
void xccdf_rule_set_version(const struct xccdf_rule* item, const char *version);
/// @memberof xccdf_value
void xccdf_value_set_version(const struct xccdf_value* item, const char *version);
/// @memberof xccdf_group
void xccdf_group_set_version(const struct xccdf_group* item, const char *version);
/// @memberof xccdf_item
void xccdf_item_set_version_update(const struct xccdf_item* item, const char *version_update);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_version_update(const struct xccdf_benchmark* item, const char *version_update);
/// @memberof xccdf_profile
void xccdf_profile_set_version_update(const struct xccdf_profile* item, const char *version_update);
/// @memberof xccdf_rule
void xccdf_rule_set_version_update(const struct xccdf_rule* item, const char *version_update);
/// @memberof xccdf_value
void xccdf_value_set_version_update(const struct xccdf_value* item, const char *version_update);
/// @memberof xccdf_group
void xccdf_group_set_version_update(const struct xccdf_group* item, const char *version_update);

/// @memberof xccdf_item
void xccdf_item_set_weight(const struct xccdf_item* item, float weight);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_weight(const struct xccdf_benchmark* item, float weight);
/// @memberof xccdf_profile
void xccdf_profile_set_weight(const struct xccdf_profile* item, float weight);
/// @memberof xccdf_rule
void xccdf_rule_set_weight(const struct xccdf_rule* item, float weight);
/// @memberof xccdf_value
void xccdf_value_set_weight(const struct xccdf_value* item, float weight);
/// @memberof xccdf_group
void xccdf_group_set_weight(const struct xccdf_group* item, float weight);
/// @memberof xccdf_item
void xccdf_item_set_version_time(const struct xccdf_item* item, time_t version_time);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_version_time(const struct xccdf_benchmark* item, time_t version_time);
/// @memberof xccdf_profile
void xccdf_profile_set_version_time(const struct xccdf_profile* item, time_t version_time);
/// @memberof xccdf_rule
void xccdf_rule_set_version_time(const struct xccdf_rule* item, time_t version_time);
/// @memberof xccdf_value
void xccdf_value_set_version_time(const struct xccdf_value* item, time_t version_time);
/// @memberof xccdf_group
void xccdf_group_set_version_time(const struct xccdf_group* item, time_t version_time);
/// @memberof xccdf_item
void xccdf_item_set_extends(const struct xccdf_item* item, struct xccdf_item * extends);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_extends(const struct xccdf_benchmark* item, struct xccdf_item * extends);
/// @memberof xccdf_profile
void xccdf_profile_set_extends(const struct xccdf_profile* item, struct xccdf_item * extends);
/// @memberof xccdf_rule
void xccdf_rule_set_extends(const struct xccdf_rule* item, struct xccdf_item * extends);
/// @memberof xccdf_value
void xccdf_value_set_extends(const struct xccdf_value* item, struct xccdf_item * extends);
/// @memberof xccdf_group
void xccdf_group_set_extends(const struct xccdf_group* item, struct xccdf_item * extends);
/// @memberof xccdf_item
void xccdf_item_set_parent(const struct xccdf_item* item, struct xccdf_item * parent);
/// @memberof xccdf_benchmark
void xccdf_benchmark_set_parent(const struct xccdf_benchmark* item, struct xccdf_item * parent);
/// @memberof xccdf_profile
void xccdf_profile_set_parent(const struct xccdf_profile* item, struct xccdf_item * parent);
/// @memberof xccdf_rule
void xccdf_rule_set_parent(const struct xccdf_rule* item, struct xccdf_item * parent);
/// @memberof xccdf_value
void xccdf_value_set_parent(const struct xccdf_value* item, struct xccdf_item * parent);
/// @memberof xccdf_group
void xccdf_group_set_parent(const struct xccdf_group* item, struct xccdf_item * parent);

#endif
