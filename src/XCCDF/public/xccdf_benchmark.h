/**
 * @addtogroup XCCDF
 * @{
 *
 * @file xccdf_benchmark.h
 * Open-scap XCCDF library interface.
 * @author Lukas Kuklinek <lkuklinek@redhat.com>
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 *      Josh Adams <jadams@tresys.com>
 */

#ifndef XCCDF_H_
#define XCCDF_H_

#include <stdbool.h>
#include <time.h>
#include <oscap_reference.h>
#include <oscap_source.h>
#include <oscap.h>
#include "cpe_dict.h"

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
	XCCDF_PROFILE = 0x0200,	        ///< Type constant for xccdf_profile
	XCCDF_RESULT = 0x0400,	        ///< Type constant for xccdf_result
	XCCDF_RULE = 0x1000,	        ///< Type constant for xccdf_rule
	XCCDF_GROUP = 0x2000,	        ///< Type constant for xccdf_group
	XCCDF_VALUE = 0x4000,	        ///< Type constant for xccdf_value

	/// Represents selectable items, i.e. rules and groups (see xccdf_item)
	XCCDF_CONTENT = XCCDF_RULE | XCCDF_GROUP,
	/// Represents items as described in the XCCDF documentation (see xccdf_item)
	XCCDF_ITEM = XCCDF_RULE | XCCDF_GROUP | XCCDF_VALUE,
	/// Represents an object, profile, result or whole benchmark (see xccdf_item)
	XCCDF_OBJECT = XCCDF_ITEM | XCCDF_PROFILE | XCCDF_BENCHMARK | XCCDF_RESULT,
} xccdf_type_t;

/// Interface hint.
typedef enum {
	XCCDF_IFACE_HINT_NONE,	        ///< No interface hint
	XCCDF_IFACE_HINT_CHOICE,	///< Choice from multiple values.
	XCCDF_IFACE_HINT_TEXTLINE,	///< Text line input widget.
	XCCDF_IFACE_HINT_TEXT,	        ///< Textarea.
	XCCDF_IFACE_HINT_DATE,	        ///< Date selection widget.
	XCCDF_IFACE_HINT_DATETIME,	///< Date and time selection widget.
} xccdf_interface_hint_t;

/// Status of an XCCDF item.
typedef enum {
	XCCDF_STATUS_NOT_SPECIFIED,	///< Status was not specified by benchmark.
	XCCDF_STATUS_ACCEPTED,	        ///< Accepted.
	XCCDF_STATUS_DEPRECATED,	///< Deprecated.
	XCCDF_STATUS_DRAFT,	        ///< Draft item.
	XCCDF_STATUS_INCOMPLETE,	///< The item is not complete.
	XCCDF_STATUS_INTERIM	        ///< Interim.
} xccdf_status_type_t;

/// Type of an xccdf_value.
typedef enum {
	XCCDF_TYPE_NUMBER = 1,	        ///< Integer.
	XCCDF_TYPE_STRING,	        ///< String.
	XCCDF_TYPE_BOOLEAN,	        ///< Boolean.
} xccdf_value_type_t;

/// Operator to be applied on an xccdf_value.
typedef enum {
	XCCDF_OPERATOR_EQUALS = 1,	///< Equality.
	XCCDF_OPERATOR_NOT_EQUAL,	///< Inequality.
	XCCDF_OPERATOR_GREATER,	        ///< Greater than.
	XCCDF_OPERATOR_GREATER_EQUAL,	///< Greater than or equal.
	XCCDF_OPERATOR_LESS,	        ///< Less than.
	XCCDF_OPERATOR_LESS_EQUAL,	///< Less than or equal.
	XCCDF_OPERATOR_PATTERN_MATCH	///< Match a regular expression.
} xccdf_operator_t;

/// Boolean operators for logical expressions
typedef enum {
	XCCDF_OPERATOR_AND = 0x0002,	///< Logical and.
	XCCDF_OPERATOR_OR = 0x0003,	///< Logical or.
} xccdf_bool_operator_t;

/// XCCDF error, complexity, disruption, or severity level
typedef enum {
	XCCDF_LEVEL_NOT_DEFINED = 0,
	XCCDF_UNKNOWN = 1,	        ///< Unknown.
	XCCDF_INFO,		        ///< Info.
	XCCDF_LOW,		        ///< Low.
	XCCDF_MEDIUM,		        ///< Medium.
	XCCDF_HIGH		        ///< High.
} xccdf_level_t;

/// Severity of an xccdf_message.
typedef enum {
	XCCDF_MSG_INFO = XCCDF_INFO,	///< Info.
	XCCDF_MSG_WARNING = XCCDF_LOW,	///< Warning.
	XCCDF_MSG_ERROR = XCCDF_HIGH,	///< Error
} xccdf_message_severity_t;

/// XCCDF role.
typedef enum {
	XCCDF_ROLE_FULL = 1,	        ///< Check the rule and let the result contriburte to the score and appear in reports
	XCCDF_ROLE_UNSCORED,	        ///< Check the rule and include the result in reports, but do not include it into score computations
	XCCDF_ROLE_UNCHECKED	        ///< Don't check the rule, result will be XCCDF_RESULT_UNKNOWN
} xccdf_role_t;

/// Category of xccdf_warning.
typedef enum {
        XCCDF_WARNING_NOT_SPECIFIED,
	XCCDF_WARNING_GENERAL = 1,	///< General-purpose warning
	XCCDF_WARNING_FUNCTIONALITY,	///< Warning about possible impacts to functionality
	XCCDF_WARNING_PERFORMANCE,	///< Warning about changes to target system performance
	XCCDF_WARNING_HARDWARE,	        ///< Warning about hardware restrictions or possible impacts to hardware
	XCCDF_WARNING_LEGAL,	        ///< Warning about legal implications
	XCCDF_WARNING_REGULATORY,	///< Warning about regulatory obligations
	XCCDF_WARNING_MANAGEMENT,	///< Warning about impacts to the mgmt or administration of the target system
	XCCDF_WARNING_AUDIT,	        ///< Warning about impacts to audit or logging
	XCCDF_WARNING_DEPENDENCY	///< Warning about dependencies between this Rule and other parts of the target system
} xccdf_warning_category_t;

/// Fix strategy type
typedef enum {
	XCCDF_STRATEGY_UNKNOWN = 0,	///< Strategy not defined
	XCCDF_STRATEGY_CONFIGURE,	///< Adjust target config or settings
	XCCDF_STRATEGY_DISABLE,	        ///< Turn off or deinstall something
	XCCDF_STRATEGY_ENABLE,	        ///< Turn on or install something
	XCCDF_STRATEGY_PATCH,	        ///< Apply a patch, hotfix, or update
	XCCDF_STRATEGY_POLICY,	        ///< Remediation by changing policies/procedures
	XCCDF_STRATEGY_RESTRICT,	///< Adjust permissions or ACLs
	XCCDF_STRATEGY_UPDATE,	        ///< Install upgrade or update the system
	XCCDF_STRATEGY_COMBINATION	///< Combo of two or more of the above
} xccdf_strategy_t;

/// Test result
typedef enum {
	XCCDF_RESULT_PASS = 1,	        ///< The test passed
	XCCDF_RESULT_FAIL,	        ///< The test failed
	XCCDF_RESULT_ERROR,	        ///< An error occurred and test could not complete
	XCCDF_RESULT_UNKNOWN,	        ///< Could not tell what happened
	XCCDF_RESULT_NOT_APPLICABLE,	///< Rule did not apply to test target
	XCCDF_RESULT_NOT_CHECKED,	///< Rule did not cause any evaluation by the checking engine
	XCCDF_RESULT_NOT_SELECTED,	///< Rule was not selected in the @link xccdf_benchmark Benchmark@endlink
	XCCDF_RESULT_INFORMATIONAL,	///< Rule was evaluated by the checking engine, but isn't to be scored
	XCCDF_RESULT_FIXED	        ///< Rule failed, but was later fixed
} xccdf_test_result_type_t;

/*--------------------*\
|       Typedefs       |
\*--------------------*/

/**
 * Type of a numerical content for a XCCDF value.
 */
typedef float xccdf_numeric;

/** 
 * @struct xccdf_benchmark
 * Top level XCCDF structure containing profiles, rules, values and results.
 */
struct xccdf_benchmark;

/** 
 * @struct xccdf_profile
 * XCCDF profile is a set of tests and their settings in a compact package
 */
struct xccdf_profile;

/** 
 * @struct xccdf_item
 * A base class for XCCDF items.
 */
struct xccdf_item;

/** 
 * @struct xccdf_rule
 * XCCDF rule defines a test execution. Groups are in this module as well.
 */
struct xccdf_rule;

/** 
 * @struct xccdf_group
 * XCCDF rule group.
 */
struct xccdf_group;

/** 
 * @struct xccdf_value
 * XCCDF Value allows test parametrization or capturing output of tests.
 */
struct xccdf_value;

/** 
 * @struct xccdf_result
 * Actual results of running a XCCDF test or profile.
 */
struct xccdf_result;

/**
 * @struct xccdf_tailoring
 * Stores content from xccdf:Tailoring element which can be loaded from
 * a separate file.
 */
struct xccdf_tailoring;

/*--------------------*\
|  Support structures  |
\*--------------------*/

/**
 * @struct xccdf_notice
 * XCCDF benchmark legal notice.
 */
struct xccdf_notice;

/**
 * @struct xccdf_status
 * XCCDF item status.
 */
struct xccdf_status;

/**
 * @struct xccdf_model
 * XCCDF scoring model.
 */
struct xccdf_model;

/**
 * @struct xccdf_warning
 * XCCDF warning.
 */
struct xccdf_warning;

/**
 * @struct xccdf_select
 * XCCDF select option usen in the profile.
 */
struct xccdf_select;

/**
 * @struct xccdf_setvalue
 * XCCDF set value option used in the profile.
 */
struct xccdf_setvalue;

/**
 * @struct xccdf_refine_value
 * XCCDF refine value option used in the profile.
 */
struct xccdf_refine_value;

/**
 * @struct xccdf_refine_rule
 * XCCDF refine rule option used in the profile.
 */
struct xccdf_refine_rule;

/**
 * @struct xccdf_ident
 * XCCDF rule ident URI.
 */
struct xccdf_ident;

/**
 * @struct xccdf_check
 * XCCDF simple or complex check.
 */
struct xccdf_check;
/**
 * @struct xccdf_check_content_ref
 * XCCDF check content reference.
 */
struct xccdf_check_content_ref;

/**
 * @struct xccdf_profile_note
 * XCCDF note for given rule in context of given profile.
 */
struct xccdf_profile_note;

/**
 * @struct xccdf_check_import
 * XCCDF check import.
 * @see xccdf_check
 */
struct xccdf_check_import;

/**
 * @struct xccdf_check_export
 * XCCDF check export.
 * @see xccdf_check
 */
struct xccdf_check_export;

/**
 * @struct xccdf_fix
 * XCCDF automatic fix.
 * @see xccdf_rule
 */
struct xccdf_fix;

/**
 * @struct xccdf_fixtext
 * XCCDF textual fix instructions.
 * @see xccdf_rule
 */
struct xccdf_fixtext;

/**
 * &struct xccdf_value_instance
 * XCCDF value instance.
 *
 * Represents single value (i.e. value properties tied to given selector)
 * @see xccdf_value
 */
struct xccdf_value_instance;

/**
 * @struct xccdf_identity
 * XCCDF identity.
 * @see xccdf_result
 */
struct xccdf_identity;

/**
 * @struct xccdf_instance
 * XCCDF instance.
 * @see xccdf_rule_result
 */
struct xccdf_instance;

/**
 * @struct xccdf_message
 * XCCDF message.
 * @see xccdf_rule_result
 */
struct xccdf_message;

/**
 * @struct xccdf_override
 * XCCDF override.
 * @see xccdf_rule_result
 */
struct xccdf_override;

/**
 * @struct xccdf_rule_result
 * XCCDF rule result.
 * @see xccdf_result
 */
struct xccdf_rule_result;

/**
 * @struct xccdf_score
 * XCCDF score.
 * @see xccdf_result
 */
struct xccdf_score;

/**
 * @struct xccdf_target_fact
 * XCCDF target fact.
 * @see xccdf_result
 */
struct xccdf_target_fact;

/**
 * @struct xccdf_target_identifier
 * Can be either <target-id-ref> or some other element.
 * @see xccdf_result
 * @note XCCDF 1.2+ only
 */
struct xccdf_target_identifier;

/**
 * @struct xccdf_plain_text
 * XCCDF target fact.
 * @see xccdf_result
 */
struct xccdf_plain_text;

/**
 * @struct xccdf_item_iterator
 * String iterator.
 * @see oscap_iterator
 */
struct xccdf_item_iterator;

/**
 * @struct xccdf_notice_iterator
 * Notice iterator.
 * @see oscap_iterator
 */
struct xccdf_notice_iterator;

/**
 * @struct xccdf_status_iterator
 * Status iterator.
 * @see oscap_iterator
 */
struct xccdf_status_iterator;

/**
 * @struct xccdf_identity_iterator
 * Reference iterator.
 * @see oscap_iterator
 */
struct xccdf_identity_iterator;

/**
 * @struct xccdf_model_iterator
 * Model iterator.
 * @see oscap_iterator
 */
struct xccdf_model_iterator;

/**
 * @struct xccdf_result_iterator
 * Result iterator.
 * @see oscap_iterator
 */
struct xccdf_result_iterator;

/**
 * @struct xccdf_profile_iterator
 * Profile iterator.
 * @see oscap_iterator
 */
struct xccdf_profile_iterator;

/**
 * @struct xccdf_select_iterator
 * Select iterator.
 * @see oscap_iterator
 */
struct xccdf_select_iterator;

/**
 * @struct xccdf_value_iterator
 * Select iterator.
 * @see oscap_iterator
 */
struct xccdf_value_iterator;

/**
 * @struct xccdf_setvalue_iterator
 * Set value iterator.
 * @see oscap_iterator
 */
struct xccdf_setvalue_iterator;

/** @struct xccdf_refine_value_iterator
 * Refine value iterator.
 * @see oscap_iterator
 */
struct xccdf_refine_value_iterator;

/**
 * @struct xccdf_refine_rule_iterator
 * Refine rule iterator.
 * @see oscap_iterator
 */
struct xccdf_refine_rule_iterator;

/**
 * @struct xccdf_ident_iterator
 * Ident iterator.
 * @see oscap_iterator
 */
struct xccdf_ident_iterator;

/**
 * @struct xccdf_check_iterator
 * Check iterator.
 * @see oscap_iterator
 */
struct xccdf_check_iterator;

/**
 * @struct xccdf_profile_note_iterator
 * Profile note iterator.
 * @see oscap_iterator
 */
struct xccdf_profile_note_iterator;

/**
 * @struct xccdf_fixtext_iterator
 * Textual fix iterator.
 * @see oscap_iterator
 */
struct xccdf_fixtext_iterator;

/**
 * @struct xccdf_check_content_ref_iterator
 * Check content references iterator.
 * @see oscap_iterator
 */
struct xccdf_check_content_ref_iterator;

/**
 * @struct xccdf_check_import_iterator
 * Check import iterator.
 * @see oscap_iterator
 */
struct xccdf_check_import_iterator;

/**
 * @struct xccdf_fix_iterator
 * Fix iterator.
 * @see oscap_iterator
 */
struct xccdf_fix_iterator;

/**
 * @struct xccdf_check_export_iterator
 * Check export iterator.
 * @see oscap_iterator
 */
struct xccdf_check_export_iterator;

/**
 * @struct xccdf_warning_iterator
 * Warning iterator.
 * @see oscap_iterator
 */
struct xccdf_warning_iterator;

/**
 * @struct xccdf_instance_iterator
 * Instance iterator.
 * @see oscap_iterator
 */
struct xccdf_instance_iterator;

/**
 * @struct xccdf_message_iterator
 * Message iterator.
 * @see oscap_iterator
 */
struct xccdf_message_iterator;

/**
 * @struct xccdf_override_iterator
 * Override iterator.
 * @see oscap_iterator
 */
struct xccdf_override_iterator;

/**
 * @struct xccdf_rule_result_iterator
 * Override iterator.
 * @see oscap_iterator
 */
struct xccdf_rule_result_iterator;

/**
 * @struct xccdf_value_instance_iterator
 * Value instance iterator.
 * @see oscap_iterator
 */
struct xccdf_value_instance_iterator;

/**
 * @struct xccdf_score_iterator
 * Override iterator.
 * @see oscap_iterator
 */
struct xccdf_score_iterator;

/**
 * @struct xccdf_target_fact_iterator
 * Override iterator.
 * @see oscap_iterator
 */
struct xccdf_target_fact_iterator;

/**
 * @struct xccdf_target_identifier_iterator
 * Override iterator.
 * @see oscap_iterator
 */
struct xccdf_target_identifier_iterator;

/**
 * @struct xccdf_plain_text_iterator
 * Plain text iterator.
 * @see oscap_iterator
 */
struct xccdf_plain_text_iterator;

/**
 * Groups info about XCCDF spec versions, namespace, CPE version, etc...
 */
struct xccdf_version_info;

/// @memberof xccdf_version_info
const char* xccdf_version_info_get_version(const struct xccdf_version_info* v);
/// @memberof xccdf_version_info
const char* xccdf_version_info_get_namespace_uri(const struct xccdf_version_info* v);
/// @memberof xccdf_version_info
const char* xccdf_version_info_get_cpe_version(const struct xccdf_version_info* v);

/**
 * Starts parsing given XCCDF benchmark file to detect its version,
 * stops as soon as the version is found. Returned string should be
 * freed by caller. Return NULL if error occur.
 * @memberof xccdf_benchmark
 * @deprecated This function has been deprecated by @ref oscap_source_get_schema_version.
 * This function may be dropped from later versions of the library.
 */
OSCAP_DEPRECATED(char * xccdf_detect_version(const char* file));

/************************************************************/

/// @memberof xccdf_item
void xccdf_item_free(struct xccdf_item *item);

/// @memberof xccdf_item
struct xccdf_item * xccdf_item_clone(const struct xccdf_item * old_item);

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

/**
 * Import the content from a specified XML stream into a benchmark.
 * @memberof xccdf_benchmark
 * @param file filename.
 * @return Pointer to the new benchmark.
 * @retval NULL on failure
 * @deprecated This function has been deprecated by @ref xccdf_benchmark_import_source.
 * This function may be dropped from later versions of the library.
 */
OSCAP_DEPRECATED(struct xccdf_benchmark* xccdf_benchmark_import(const char *file));

/**
 * Import the content from oscap_source into a benchmark
 * @memberof xccdf_benchmark
 * @param source The oscap_source to import from
 * @returns newly created benchmark element or NULL
 */
struct xccdf_benchmark* xccdf_benchmark_import_source(struct oscap_source *source);

/**
 * Export a benchmark to an XML stream
 * @memberof xccdf_benchmark
 * @return Integer
 * @retval -1 if error occurred
 */
int xccdf_benchmark_export(struct xccdf_benchmark *benchmark, const char *file);

/**
 * Import the content of oscap_source into a xccdf_result
 * @memberof xccdf_result
 * @param source The oscap_source to import from
 * @returns newly created test-result element or NULL on error
 */
struct xccdf_result *xccdf_result_import_source(struct oscap_source *source);

/**
 * Collect system info and store it in the TestResult.
 * @memberof xccdf_result
 */
void xccdf_result_fill_sysinfo(struct xccdf_result *result);

/**
 * Export a TestResult to an XML stream
 * @memberof xccdf_result
 * @return Integer
 * @retval -1 if error occurred
 * @deprecated This function has been deprecated by @ref xccdf_benchmark_export_source.
 * This function may be dropped from later versions of the library.
 */
OSCAP_DEPRECATED(int xccdf_result_export(struct xccdf_result *result, const char *file));

/**
 * Export TestResult to oscap_source structure
 * @memberof xccdf_result
 * @returns newly created oscap_source or NULL on error
 */
struct oscap_source *xccdf_result_export_source(struct xccdf_result *result, const char *filepath);

/**
 * Resolve an benchmark.
 * @returns whether the resolving process has been successful
 * @retval true on success
 * @retval false on dependency loop
 */
bool xccdf_benchmark_resolve(struct xccdf_benchmark *benchmark);

/// @memberof xccdf_benchmark
struct xccdf_benchmark *xccdf_benchmark_new(void);
/// @memberof xccdf_benchmark
void xccdf_benchmark_free(struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
struct xccdf_item *xccdf_benchmark_to_item(struct xccdf_benchmark *item);
/// @memberof xccdf_benchmark
struct xccdf_benchmark * xccdf_benchmark_clone( const struct  xccdf_benchmark * benchmark );

/**
 * Get supported version of XCCDF XML
 * @return version of XML file format
 * @memberof xccdf_benchmark
 */
const char * xccdf_benchmark_supported(void);

/// @memberof xccdf_profile
struct xccdf_profile *xccdf_profile_new(void);
/// @memberof xccdf_profile
void xccdf_profile_free(struct xccdf_item *prof);
/// @memberof xccdf_profile
struct xccdf_item *xccdf_profile_to_item(struct xccdf_profile *item);
/// @memberof xccdf_profile
struct xccdf_profile * xccdf_profile_clone( const struct xccdf_profile * profile);

/// @memberof xccdf_rule
struct xccdf_rule *xccdf_rule_new(void);
/// @memberof xccdf_rule
void xccdf_rule_free(struct xccdf_item *rule);
/// @memberof xccdf_rule
struct xccdf_item *xccdf_rule_to_item(struct xccdf_rule *item);
/// @memberof xccdf_rule
struct xccdf_rule * xccdf_rule_clone(const struct xccdf_rule * rule);

/// @memberof xccdf_group
struct xccdf_group *xccdf_group_new(void);
/// @memberof xccdf_group
void xccdf_group_free(struct xccdf_item *group);
/// @memberof xccdf_group
struct xccdf_item *xccdf_group_to_item(struct xccdf_group *item);
/// @memberof xccdf_group
struct xccdf_group * xccdf_group_clone(const struct xccdf_group * group);

/// @memberof xccdf_value
struct xccdf_value *xccdf_value_new(xccdf_value_type_t type);
/// @memberof xccdf_value
void xccdf_value_free(struct xccdf_item *val);
/// @memberof xccdf_value
struct xccdf_item *xccdf_value_to_item(struct xccdf_value *item);
/// @memberof xccdf_value
struct xccdf_value * xccdf_value_clone(const struct xccdf_value * value);

/// @memberof xccdf_status
struct xccdf_status *xccdf_status_new(void);
/// @memberof xccdf_status
struct xccdf_status * xccdf_status_clone(const struct xccdf_status * old_status);
/// @memberof xccdf_status
struct xccdf_status *xccdf_status_new_fill(const char *status, const char *date);
/// @memberof xccdf_status
void xccdf_status_free(struct xccdf_status *status);
/// @memberof xccdf_notice
struct xccdf_notice *xccdf_notice_new(void);
/// @memberof xccdf_notice
void xccdf_notice_free(struct xccdf_notice *notice);
/// @memberof xccdf_notice
struct xccdf_notice * xccdf_notice_clone(const struct xccdf_notice * notice);

/// @memberof xccdf_model
struct xccdf_model *xccdf_model_new(void);
/// @memberof xccdf_model
struct xccdf_model * xccdf_model_clone(const struct xccdf_model * old_model);
/// @memberof xccdf_model
void xccdf_model_free(struct xccdf_model *model);

/// @memberof xccdf_ident
struct xccdf_ident *xccdf_ident_new(void);
/// @memberof xccdf_ident
struct xccdf_ident *xccdf_ident_new_fill(const char *id, const char *sys);
/// @memberof xccdf_ident
struct xccdf_ident *xccdf_ident_clone(const struct xccdf_ident * ident);
/// @memberof xccdf_ident
void xccdf_ident_free(struct xccdf_ident *ident);


/// @memberof xccdf_check
struct xccdf_check *xccdf_check_new(void);
/// @memberof xccdf_check
void xccdf_check_free(struct xccdf_check *check);

/// @memberof xccdf_check
struct xccdf_check *xccdf_check_clone(const struct xccdf_check *old_check);
/// @memberof xccdf_check_import
struct xccdf_check_import *xccdf_check_import_clone(const struct xccdf_check_import *old_import);
/// @memberof xccdf_check_export
struct xccdf_check_export *xccdf_check_export_clone(const struct xccdf_check_export *old_export);
/// @memberof xccdf_check_content_ref
struct xccdf_check_content_ref *xccdf_check_content_ref_clone(const struct xccdf_check_content_ref *old_ref);

/// @memberof xccdf_check_content_ref
struct xccdf_check_content_ref *xccdf_check_content_ref_new(void);
/// @memberof xccdf_check_content_ref
void xccdf_check_content_ref_free(struct xccdf_check_content_ref *ref);

/// @memberof xccdf_profile_note
struct xccdf_profile_note *xccdf_profile_note_new(void);
/// @memberof xccdf_profile_note
void xccdf_profile_note_free(struct xccdf_profile_note *note);

/// @memberof xccdf_check_import
struct xccdf_check_import *xccdf_check_import_new(void);
/// @memberof xccdf_check_import
void xccdf_check_import_free(struct xccdf_check_import *item);

/// @memberof xccdf_check_export
struct xccdf_check_export *xccdf_check_export_new(void);
/// @memberof xccdf_check_export
void xccdf_check_export_free(struct xccdf_check_export *item);

/// @memberof xccdf_fix
struct xccdf_fix *xccdf_fix_new(void);
/// @memberof xccdf_fix
struct xccdf_fix *xccdf_fix_clone(const struct xccdf_fix *old_fix);
/// @memberof xccdf_fix
void xccdf_fix_free(struct xccdf_fix *item);

/// @memberof xccdf_fixtext
struct xccdf_fixtext *xccdf_fixtext_new(void);
/// @memberof xccdf_fixtext
struct xccdf_fixtext * xccdf_fixtext_clone(const struct xccdf_fixtext * fixtext);
/// @memberof xccdf_fixtext
void xccdf_fixtext_free(struct xccdf_fixtext *item);

/// @memberof xccdf_select
void xccdf_select_free(struct xccdf_select *sel);
/// @memberof xccdf_select
struct xccdf_select *xccdf_select_clone(const struct xccdf_select * select);
/// @memberof xccdf_select
struct xccdf_select *xccdf_select_new(void);

/// @memberof xccdf_warning
struct xccdf_warning *xccdf_warning_new(void);
/// @memberof xccdf_warning
struct xccdf_warning *xccdf_warning_clone(const struct xccdf_warning *old_warning);
/// @memberof xccdf_warning
void xccdf_warning_free(struct xccdf_warning * warn);

/// @memberof xccdf_refine_rule
void xccdf_refine_rule_free(struct xccdf_refine_rule *obj);

/// @memberof xccdf_refine_value
void xccdf_refine_value_free(struct xccdf_refine_value *rv);

void xccdf_setvalue_free(struct xccdf_setvalue *sv);

/// @memberof xccdf_tailoring
struct xccdf_tailoring *xccdf_tailoring_new(void);
/// @memberof xccdf_tailoring
void xccdf_tailoring_free(struct xccdf_tailoring *tailoring);
/// @memberof xccdf_tailoring
int xccdf_tailoring_export(struct xccdf_tailoring *tailoring, const char *file, const struct xccdf_version_info *version_info);

/**
 * Release library internal caches.
 * @deprecated Use @ref oscap_cleanup() instead.
 */
OSCAP_DEPRECATED(void xccdf_cleanup(void));

/**
 * Create a group and append it to the benchmark.
 * @param id - the identifier of the appended value.
 * @return the handle of the new group.
 */
struct xccdf_group *xccdf_benchmark_append_new_group(struct xccdf_benchmark *, const char *id);

/**
 * Create a value and append it to the benchmark.
 * @param id - the identifier of the appended value.
 * @return the handle of the new value.
 */
struct xccdf_value *xccdf_benchmark_append_new_value(struct xccdf_benchmark *, const char *id, xccdf_value_type_t type);

/**
 * Create a rule and append it to the benchmark.
 * @param id - the identifier of the appended rule.
 * @return the handle of the new rule.
 */
struct xccdf_rule *xccdf_benchmark_append_new_rule(struct xccdf_benchmark *, const char *id);

/// @memberof xccdf_plain_text
struct xccdf_plain_text *xccdf_plain_text_new(void);
/// @memberof xccdf_plain_text
struct xccdf_plain_text *xccdf_plain_text_new_fill(const char *id, const char *text);
/// @memberof xccdf_plain_text
void xccdf_plain_text_free(struct xccdf_plain_text *plain);
/// @memberof xccdf_plain_text
struct xccdf_plain_text *xccdf_plain_text_clone(const struct xccdf_plain_text * pt);

/// @memberof xccdf_result
struct xccdf_result *xccdf_result_new(void);
/// @memberof xccdf_result
void xccdf_result_free(struct xccdf_result *item);
/// @memberof xccdf_result
struct xccdf_item *xccdf_result_to_item(struct xccdf_result *item);
/// @memberof xccdf_result
struct xccdf_result * xccdf_result_clone(const struct xccdf_result * result);

/// @memberof xccdf_rule_result
struct xccdf_rule_result *xccdf_rule_result_new(void);
/// @memberof xccdf_rule_result
struct xccdf_rule_result * xccdf_rule_result_clone(const struct xccdf_rule_result * result);
/// @memberof xccdf_rule_result
void xccdf_rule_result_free(struct xccdf_rule_result *rr);

/// @memberof xccdf_identity
struct xccdf_identity *xccdf_identity_new(void);
/// @memberof xccdf_identity
struct xccdf_identity * xccdf_identity_clone(const struct xccdf_identity * identity);
/// @memberof xccdf_identity
void xccdf_identity_free(struct xccdf_identity *identity);

/// @memberof xccdf_score
struct xccdf_score *xccdf_score_new(void);
/// @memberof xccdf_score
struct xccdf_score * xccdf_score_clone(const struct xccdf_score * score);
/// @memberof xccdf_score
void xccdf_score_free(struct xccdf_score *score);

/// @memberof xccdf_override
struct xccdf_override *xccdf_override_new(void);
/// @memberof xccdf_override
struct xccdf_override * xccdf_override_clone(const struct xccdf_override * override);
/// @memberof xccdf_override
void xccdf_override_free(struct xccdf_override *oride);

/// @memberof xccdf_message
struct xccdf_message *xccdf_message_new(void);
/// @memberof xccdf_message
struct xccdf_message * xccdf_message_clone(const struct xccdf_message * message);
/// @memberof xccdf_message
void xccdf_message_free(struct xccdf_message *msg);

/// @memberof xccdf_target_fact
struct xccdf_target_fact *xccdf_target_fact_new(void);
/// @memberof xccdf_target_fact
struct xccdf_target_fact * xccdf_target_fact_clone(const struct xccdf_target_fact * tf);
/// @memberof xccdf_target_fact
void xccdf_target_fact_free(struct xccdf_target_fact *fact);

/// @memberof xccdf_target_identifier
struct xccdf_target_identifier *xccdf_target_identifier_new(void);
/// @memberof xccdf_target_identifier
struct xccdf_target_identifier * xccdf_target_identifier_clone(const struct xccdf_target_identifier * ti);
/// @memberof xccdf_target_identifier
void xccdf_target_identifier_free(struct xccdf_target_identifier *ti);

/// @memberof xccdf_instance
struct xccdf_instance *xccdf_instance_new(void);
/// @memberof xccdf_instance
struct xccdf_instance * xccdf_instance_clone(const struct xccdf_instance * instance);
/// @memberof xccdf_instance
void xccdf_instance_free(struct xccdf_instance *inst);

/// @memberof xccdf_value_instance
struct oscap_string_iterator *xccdf_value_instance_get_choices(const struct xccdf_value_instance *item);

/************************************************************/
/**
 * @name Iterators
 * @{
 * */

/**
 * Return the next xccdf_item structure from the list and increment the iterator
 * @memberof xccdf_item_iterator
 */
struct xccdf_item *xccdf_item_iterator_next(struct xccdf_item_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_item_iterator
 */
bool xccdf_item_iterator_has_more(struct xccdf_item_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_item_iterator
 */
void xccdf_item_iterator_free(struct xccdf_item_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_item_iterator
 */
void xccdf_item_iterator_reset(struct xccdf_item_iterator *it);


/**
 * Return the next xccdf_notice structure from the list and increment the iterator
 * @memberof xccdf_notice_iterator
 */
struct xccdf_notice *xccdf_notice_iterator_next(struct xccdf_notice_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_notice_iterator
 */
bool xccdf_notice_iterator_has_more(struct xccdf_notice_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_notice_iterator
 */
void xccdf_notice_iterator_free(struct xccdf_notice_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_notice_iterator
 */
void xccdf_notice_iterator_reset(struct xccdf_notice_iterator *it);


/**
 * Return the next xccdf_status structure from the list and increment the iterator
 * @memberof xccdf_status_iterator
 */
struct xccdf_status *xccdf_status_iterator_next(struct xccdf_status_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_status_iterator
 */
bool xccdf_status_iterator_has_more(struct xccdf_status_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_status_iterator
 */
void xccdf_status_iterator_free(struct xccdf_status_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_status_iterator
 */
void xccdf_status_iterator_reset(struct xccdf_status_iterator *it);


/**
 * Return the next xccdf_model structure from the list and increment the iterator
 * @memberof xccdf_model_iterator
 */
struct xccdf_model *xccdf_model_iterator_next(struct xccdf_model_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_model_iterator
 */
bool xccdf_model_iterator_has_more(struct xccdf_model_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_model_iterator
 */
void xccdf_model_iterator_free(struct xccdf_model_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_model_iterator
 */
void xccdf_model_iterator_reset(struct xccdf_model_iterator *it);


/**
 * Return the next xccdf_result structure from the list and increment the iterator
 * @memberof xccdf_result_iterator
 */
struct xccdf_result *xccdf_result_iterator_next(struct xccdf_result_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_result_iterator
 */
bool xccdf_result_iterator_has_more(struct xccdf_result_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_result_iterator
 */
void xccdf_result_iterator_free(struct xccdf_result_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_result_iterator
 */
void xccdf_result_iterator_reset(struct xccdf_result_iterator *it);


/**
 * Return the next xccdf_profile structure from the list and increment the iterator
 * @memberof xccdf_profile_iterator
 */
struct xccdf_profile *xccdf_profile_iterator_next(struct xccdf_profile_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_profile_iterator
 */
bool xccdf_profile_iterator_has_more(struct xccdf_profile_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_profile_iterator
 */
void xccdf_profile_iterator_free(struct xccdf_profile_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_profile_iterator
 */
void xccdf_profile_iterator_reset(struct xccdf_profile_iterator *it);


/**
 * Return the next xccdf_select structure from the list and increment the iterator
 * @memberof xccdf_select_iterator
 */
struct xccdf_select *xccdf_select_iterator_next(struct xccdf_select_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_select_iterator
 */
bool xccdf_select_iterator_has_more(struct xccdf_select_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_select_iterator
 */
void xccdf_select_iterator_free(struct xccdf_select_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_select_iterator
 */
void xccdf_select_iterator_reset(struct xccdf_select_iterator *it);


/**
 * Return the next xccdf_setvalue structure from the list and increment the iterator
 * @memberof xccdf_setvalue_iterator
 */
struct xccdf_setvalue *xccdf_setvalue_iterator_next(struct xccdf_setvalue_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_setvalue_iterator
 */
bool xccdf_setvalue_iterator_has_more(struct xccdf_setvalue_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_setvalue_iterator
 */
void xccdf_setvalue_iterator_free(struct xccdf_setvalue_iterator *it);
/**
 * Reset the iterator structure (it will point to the first item in the list)
 * @memberof xccdf_setvalue_iterator
 */
void xccdf_setvalue_iterator_reset(struct xccdf_setvalue_iterator *it);


/**
 * Return the next xccdf_refine_value structure from the list and increment the iterator
 * @memberof xccdf_refine_value_iterator
 */
struct xccdf_refine_value *xccdf_refine_value_iterator_next(struct xccdf_refine_value_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_refine_value_iterator
 */
bool xccdf_refine_value_iterator_has_more(struct xccdf_refine_value_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_refine_value_iterator
 */
void xccdf_refine_value_iterator_free(struct xccdf_refine_value_iterator *it);
/**
 * Reset the iterator structure (it will point to the first item in the list)
 * @memberof xccdf_refine_value_iterator
 */
void xccdf_refine_value_iterator_reset(struct xccdf_refine_value_iterator *it);


/**
 * Return the next xccdf_refine_rule structure from the list and increment the iterator
 * @memberof xccdf_refine_rule_iterator
 */
struct xccdf_refine_rule *xccdf_refine_rule_iterator_next(struct xccdf_refine_rule_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_refine_rule_iterator
 */
bool xccdf_refine_rule_iterator_has_more(struct xccdf_refine_rule_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_refine_rule_iterator
 */
void xccdf_refine_rule_iterator_free(struct xccdf_refine_rule_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_refine_rule_iterator
 */
void xccdf_refine_rule_iterator_reset(struct xccdf_refine_rule_iterator *it);


/**
 * Return the next xccdf_ident structure from the list and increment the iterator
 * @memberof xccdf_ident_iterator
 */
struct xccdf_ident *xccdf_ident_iterator_next(struct xccdf_ident_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_ident_iterator
 */
bool xccdf_ident_iterator_has_more(struct xccdf_ident_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_ident_iterator
 */
void xccdf_ident_iterator_free(struct xccdf_ident_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_ident_iterator
 */
void xccdf_ident_iterator_reset(struct xccdf_ident_iterator *it);


/**
 * Return the next xccdf_check structure from the list and increment the iterator
 * @memberof xccdf_check_iterator
 */
struct xccdf_check *xccdf_check_iterator_next(struct xccdf_check_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_check_iterator
 */
bool xccdf_check_iterator_has_more(struct xccdf_check_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_check_iterator
 */
void xccdf_check_iterator_free(struct xccdf_check_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_check_iterator
 */
void xccdf_check_iterator_reset(struct xccdf_check_iterator *it);


/**
 * Return the next xccdf_check_content_ref structure from the list and increment the iterator
 * @memberof xccdf_check_content_ref_iterator
 */
struct xccdf_check_content_ref *xccdf_check_content_ref_iterator_next(struct xccdf_check_content_ref_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_check_content_ref_iterator
 */
bool xccdf_check_content_ref_iterator_has_more(struct xccdf_check_content_ref_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_check_content_ref_iterator
 */
void xccdf_check_content_ref_iterator_free(struct xccdf_check_content_ref_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_check_content_ref_iterator
 */
void xccdf_check_content_ref_iterator_reset(struct xccdf_check_content_ref_iterator *it);


/**
 * Return the next xccdf_profile_note structure from the list and increment the iterator
 * @memberof xccdf_profile_note_iterator
 */
struct xccdf_profile_note *xccdf_profile_note_iterator_next(struct xccdf_profile_note_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_profile_note_iterator
 */
bool xccdf_profile_note_iterator_has_more(struct xccdf_profile_note_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_profile_note_iterator
 */
void xccdf_profile_note_iterator_free(struct xccdf_profile_note_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_profile_note_iterator
 */
void xccdf_profile_note_iterator_reset(struct xccdf_profile_note_iterator *it);


/**
 * Return the next xccdf_check_import structure from the list and increment the iterator
 * @memberof xccdf_check_import_iterator
 */
struct xccdf_check_import *xccdf_check_import_iterator_next(struct xccdf_check_import_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_check_import_iterator
 */
bool xccdf_check_import_iterator_has_more(struct xccdf_check_import_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_check_import_iterator
 */
void xccdf_check_import_iterator_free(struct xccdf_check_import_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_check_import_iterator
 */
void xccdf_check_import_iterator_reset(struct xccdf_check_import_iterator *it);


/**
 * Return the next xccdf_check_export structure from the list and increment the iterator
 * @memberof xccdf_check_export_iterator
 */
struct xccdf_check_export *xccdf_check_export_iterator_next(struct xccdf_check_export_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_check_export_iterator
 */
bool xccdf_check_export_iterator_has_more(struct xccdf_check_export_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_check_export_iterator
 */
void xccdf_check_export_iterator_free(struct xccdf_check_export_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_check_export_iterator
 */
void xccdf_check_export_iterator_reset(struct xccdf_check_export_iterator *it);


/**
 * Return the next xccdf_fix structure from the list and increment the iterator
 * @memberof xccdf_fix_iterator
 */
struct xccdf_fix *xccdf_fix_iterator_next(struct xccdf_fix_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_fix_iterator
 */
bool xccdf_fix_iterator_has_more(struct xccdf_fix_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_fix_iterator
 */
void xccdf_fix_iterator_free(struct xccdf_fix_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_fix_iterator
 */
void xccdf_fix_iterator_reset(struct xccdf_fix_iterator *it);


/**
 * Return the next xccdf_fixtext structure from the list and increment the iterator
 * @memberof xccdf_fixtext_iterator
 */
struct xccdf_fixtext *xccdf_fixtext_iterator_next(struct xccdf_fixtext_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_fixtext_iterator
 */
bool xccdf_fixtext_iterator_has_more(struct xccdf_fixtext_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_fixtext_iterator
 */
void xccdf_fixtext_iterator_free(struct xccdf_fixtext_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_fixtext_iterator
 */
void xccdf_fixtext_iterator_reset(struct xccdf_fixtext_iterator *it);


/**
 * Return the next xccdf_warning structure from the list and increment the iterator
 * @memberof xccdf_warning_iterator
 */
struct xccdf_warning *xccdf_warning_iterator_next(struct xccdf_warning_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_warning_iterator
 */
bool xccdf_warning_iterator_has_more(struct xccdf_warning_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_warning_iterator
 */
void xccdf_warning_iterator_free(struct xccdf_warning_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the underlying list)
 * @memberof xccdf_warning_iterator
 */
void xccdf_warning_iterator_reset(struct xccdf_warning_iterator *it);


/**
 * Return the next xccdf_instance structure from the list and increment the iterator
 * @memberof xccdf_instance_iterator
 */
struct xccdf_instance *xccdf_instance_iterator_next(struct xccdf_instance_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_instance_iterator
 */
bool xccdf_instance_iterator_has_more(struct xccdf_instance_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_instance_iterator
 */
void xccdf_instance_iterator_free(struct xccdf_instance_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_instance_iterator
 */
void xccdf_instance_iterator_reset(struct xccdf_instance_iterator *it);


/**
 * Return the next xccdf_message structure from the list and increment the iterator
 * @memberof xccdf_message_iterator
 */
struct xccdf_message *xccdf_message_iterator_next(struct xccdf_message_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_message_iterator
 */
bool xccdf_message_iterator_has_more(struct xccdf_message_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_message_iterator
 */
void xccdf_message_iterator_free(struct xccdf_message_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_message_iterator
 */
void xccdf_message_iterator_reset(struct xccdf_message_iterator *it);


/**
 * Return the next xccdf_override structure from the list and increment the iterator
 * @memberof xccdf_override_iterator
 */
struct xccdf_override *xccdf_override_iterator_next(struct xccdf_override_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_override_iterator
 */
bool xccdf_override_iterator_has_more(struct xccdf_override_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_override_iterator
 */
void xccdf_override_iterator_free(struct xccdf_override_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_override_iterator
 */
void xccdf_override_iterator_reset(struct xccdf_override_iterator *it);


/**
 * Return the next xccdf_identity structure from the list and increment the iterator
 * @memberof xccdf_identity_iterator
 */
struct xccdf_identity *xccdf_identity_iterator_next(struct xccdf_identity_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_identity_iterator
 */
bool xccdf_identity_iterator_has_more(struct xccdf_identity_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_identity_iterator
 */
void xccdf_identity_iterator_free(struct xccdf_identity_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_identity_iterator
 */
void xccdf_identity_iterator_reset(struct xccdf_identity_iterator *it);


/**
 * Return the next xccdf_rule_result structure from the list and increment the iterator
 * @memberof xccdf_rule_result_iterator
 */
struct xccdf_rule_result *xccdf_rule_result_iterator_next(struct xccdf_rule_result_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_rule_result_iterator
 */
bool xccdf_rule_result_iterator_has_more(struct xccdf_rule_result_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_rule_result_iterator
 */
void xccdf_rule_result_iterator_free(struct xccdf_rule_result_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_rule_result_iterator
 */
void xccdf_rule_result_iterator_reset(struct xccdf_rule_result_iterator *it);


/**
 * Return the next xccdf_value_instance structure from the list and increment the iterator
 * @memberof xccdf_value_instance_iterator
 */
struct xccdf_value_instance *xccdf_value_instance_iterator_next(struct xccdf_value_instance_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_value_instance_iterator
 */
bool xccdf_value_instance_iterator_has_more(struct xccdf_value_instance_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_value_instance_iterator
 */
void xccdf_value_instance_iterator_free(struct xccdf_value_instance_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_value_instance_iterator
 */
void xccdf_value_instance_iterator_reset(struct xccdf_value_instance_iterator *it);


/**
 * Return the next xccdf_score structure from the list and increment the iterator
 * @memberof xccdf_score_iterator
 */
struct xccdf_score *xccdf_score_iterator_next(struct xccdf_score_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_score_iterator
 */
bool xccdf_score_iterator_has_more(struct xccdf_score_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_score_iterator
 */
void xccdf_score_iterator_free(struct xccdf_score_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_score_iterator
 */
void xccdf_score_iterator_reset(struct xccdf_score_iterator *it);


/**
 * Return the next xccdf_target_fact structure from the list and increment the iterator
 * @memberof xccdf_target_fact_iterator
 */
struct xccdf_target_fact *xccdf_target_fact_iterator_next(struct xccdf_target_fact_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_target_fact_iterator
 */
bool xccdf_target_fact_iterator_has_more(struct xccdf_target_fact_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_target_fact_iterator
 */
void xccdf_target_fact_iterator_free(struct xccdf_target_fact_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_target_fact_iterator
 */
void xccdf_target_fact_iterator_reset(struct xccdf_target_fact_iterator *it);

/**
 * Return the next xccdf_target_identifier structure from the list and increment the iterator
 * @memberof xccdf_target_identifier_iterator
 */
struct xccdf_target_identifier *xccdf_target_identifier_iterator_next(struct xccdf_target_identifier_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_target_identifier_iterator
 */
bool xccdf_target_identifier_iterator_has_more(struct xccdf_target_identifier_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_target_identifier_iterator
 */
void xccdf_target_identifier_iterator_free(struct xccdf_target_identifier_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_target_identifier_iterator
 */
void xccdf_target_identifier_iterator_reset(struct xccdf_target_identifier_iterator *it);


/**
 * Return the next xccdf_plain_text structure from the list and increment the iterator
 * @memberof xccdf_plain_text_iterator
 */
struct xccdf_plain_text *xccdf_plain_text_iterator_next(struct xccdf_plain_text_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_plain_text_iterator
 */
bool xccdf_plain_text_iterator_has_more(struct xccdf_plain_text_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_plain_text_iterator
 */
void xccdf_plain_text_iterator_free(struct xccdf_plain_text_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_plain_text_iterator
 */
void xccdf_plain_text_iterator_reset(struct xccdf_plain_text_iterator *it);


/**
 * Return the next xccdf_value structure from the list and increment the iterator
 * @memberof xccdf_value_iterator
 */
struct xccdf_value *xccdf_value_iterator_next(struct xccdf_value_iterator *it);
/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_value_iterator
 */
bool xccdf_value_iterator_has_more(struct xccdf_value_iterator *it);
/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_value_iterator
 */
void xccdf_value_iterator_free(struct xccdf_value_iterator *it);
/**
 * Reset the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_value_iterator
 */
void xccdf_value_iterator_reset(struct xccdf_value_iterator *it);

/************************************************************
 ** @} End of Iterators group */

/************************************************************/
/**
 * @name Getters
 * Return value is pointer to structure's member. Do not free unless you null the pointer in the structure. 
 * Use remove function otherwise.
 * @{
 * */

/**
 * @memberof xccdf_item
 */
xccdf_type_t xccdf_item_get_type(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
const char *xccdf_item_get_id(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
struct oscap_text_iterator *xccdf_item_get_title(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
struct oscap_text_iterator *xccdf_item_get_description(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
const char *xccdf_item_get_version(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
const char *xccdf_item_get_extends(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
struct xccdf_status_iterator *xccdf_item_get_statuses(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
struct oscap_reference_iterator *xccdf_item_get_dc_statuses(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
struct oscap_reference_iterator *xccdf_item_get_references(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
struct oscap_string_iterator *xccdf_item_get_conflicts(const struct xccdf_item* item);
/**
 * @memberof xccdf_item
 */
struct oscap_stringlist_iterator *xccdf_item_get_requires(const struct xccdf_item* item);
/**
 * @memberof xccdf_item
 */
struct xccdf_status * xccdf_item_get_current_status(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
bool xccdf_item_get_hidden(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
bool xccdf_item_get_selected(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
bool xccdf_item_get_prohibit_changes(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
bool xccdf_item_get_abstract(const struct xccdf_item *item);
/**
 * @memberof xccdf_item
 */
struct xccdf_item_iterator *xccdf_item_get_content(const struct xccdf_item *item);
/**
 * @memberof xccdf_test_result
 */
const char * xccdf_test_result_type_get_text(xccdf_test_result_type_t id);
/**
 * @memberof xccdf_result
 */
struct xccdf_rule_result * xccdf_result_get_rule_result_by_id(struct xccdf_result * result, const char * id);

/**
 * Return item's parent in the grouping hierarchy.
 * Returned item will be either a group or a benchmark.
 * @memberof xccdf_item
 */
struct xccdf_item *xccdf_item_get_parent(const struct xccdf_item *item);

/**
 * Retrieves the XCCDF version of top-level benchmark item.
 *
 * This is the version we use to determine how to process the item.
 * Valid return values include "1.1.4", "1.2".
 * You can use strverscmp to compare versions if you need to.
 * Don't deallocate the returned buffer!
 * @memberof xccdf_item
 */
const struct xccdf_version_info* xccdf_item_get_schema_version(struct xccdf_item* item);

/**
 * @memberof xccdf_item
 */
struct oscap_string_iterator *xccdf_item_get_metadata(const struct xccdf_item *item);

/**
 * @memberof xccdf_benchmark
 */
const char *xccdf_benchmark_get_id(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
bool xccdf_benchmark_get_resolved(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
struct oscap_text_iterator *xccdf_benchmark_get_title(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
struct oscap_text_iterator *xccdf_benchmark_get_description(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
const char *xccdf_benchmark_get_version(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
const struct xccdf_version_info* xccdf_benchmark_get_schema_version(const struct xccdf_benchmark* item);
/**
 * @memberof xccdf_benchmark
 */
const char *xccdf_benchmark_get_style(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
const char *xccdf_benchmark_get_style_href(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
struct oscap_text_iterator *xccdf_benchmark_get_front_matter(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
struct oscap_text_iterator *xccdf_benchmark_get_rear_matter(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
struct xccdf_status_iterator *xccdf_benchmark_get_statuses(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
struct oscap_reference_iterator *xccdf_benchmark_get_dc_statuses(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
struct oscap_reference_iterator *xccdf_benchmark_get_references(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
struct oscap_string_iterator *xccdf_benchmark_get_platforms(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
struct xccdf_status * xccdf_benchmark_get_status_current(const struct xccdf_benchmark *benchmark);
/**
 * @memberof xccdf_benchmark
 */
struct xccdf_plain_text_iterator *xccdf_benchmark_get_plain_texts(const struct xccdf_benchmark *item);
/**
 * @memberof xccdf_benchmark
 */
struct xccdf_result_iterator* xccdf_benchmark_get_results(const struct xccdf_benchmark *bench);
/**
 * @memberof xccdf_benchmark
 */
struct xccdf_value_iterator *xccdf_benchmark_get_values(const struct xccdf_benchmark *item);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_lang(struct xccdf_benchmark *item, const char *newval);
/// @memberof xccdf_benchmark
const char *xccdf_benchmark_get_lang(const struct xccdf_benchmark *item);

/**
 * Get a plain text by ID.
 * @memberof xccdf_benchmark
 * @param id ID of the plain text to get.
 * @return Plain text content.
 * @retval NULL if given plain text does not exist
 */
const char *xccdf_benchmark_get_plain_text(const struct xccdf_benchmark *benchmark, const char *id);

/**
 * Get benchmark xccdf:Item by ID.
 * @memberof xccdf_benchmark
 * @param id ID
 * @return Item with given ID
 * @retval NULL if no such item exists
 */
struct xccdf_item *xccdf_benchmark_get_item(const struct xccdf_benchmark *benchmark, const char *id);

/**
 * Get a registered member of xccdf_benchmakr by ID.
 * @memberof xccdf_benchmark
 * @param type of member: either XCCDF_ITEM, XCCDF_PROFILE, or XCCDF_RESULT
 * @return xccdf_item with given ID and type
 * @return NULL if no such member exists
 */
struct xccdf_item *xccdf_benchmark_get_member(const struct xccdf_benchmark *benchmark, xccdf_type_t type, const char *key);

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
 * @memberof xccdf_benchmark
 */
struct oscap_string_iterator *xccdf_benchmark_get_metadata(const struct xccdf_benchmark *benchmark);

/**
 * @memberof xccdf_benchmark
 */
struct cpe_dict_model *xccdf_benchmark_get_cpe_list(const struct xccdf_benchmark *benchmark);

/**
 * @memberof xccdf_benchmark
 */
struct cpe_lang_model *xccdf_benchmark_get_cpe_lang_model(const struct xccdf_benchmark *benchmark);

/**
 * @memberof xccdf_profile
 */
const char *xccdf_profile_get_id(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct oscap_text_iterator *xccdf_profile_get_title(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct oscap_text_iterator *xccdf_profile_get_description(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
const char *xccdf_profile_get_version(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
const char *xccdf_profile_get_extends(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct xccdf_benchmark *xccdf_profile_get_benchmark(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
bool xccdf_profile_get_abstract(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
bool xccdf_profile_get_prohibit_changes(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct oscap_string_iterator *xccdf_profile_get_platforms(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct xccdf_status_iterator *xccdf_profile_get_statuses(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct oscap_reference_iterator *xccdf_profile_get_dc_statuses(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct oscap_reference_iterator *xccdf_profile_get_references(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct xccdf_status * xccdf_profile_get_status_current(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct xccdf_select_iterator *xccdf_profile_get_selects(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct xccdf_setvalue_iterator *xccdf_profile_get_setvalues(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct xccdf_refine_value_iterator *xccdf_profile_get_refine_values(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct xccdf_refine_rule_iterator *xccdf_profile_get_refine_rules(const struct xccdf_profile *profile);
/**
 * @memberof xccdf_profile
 */
struct oscap_string_iterator *xccdf_profile_get_metadata(const struct xccdf_profile *profile);

/**
 * Return rule's parent in the grouping hierarchy.
 * Returned item will be either a group or a benchmark.
 * @memberof xccdf_rule
 */
struct xccdf_item *xccdf_rule_get_parent(const struct xccdf_rule *rule);

/**
 * @memberof xccdf_rule
 */
const char *xccdf_rule_get_id(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct oscap_text_iterator *xccdf_rule_get_title(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct oscap_text_iterator *xccdf_rule_get_description(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
const char *xccdf_rule_get_version(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct oscap_text_iterator *xccdf_rule_get_question(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct xccdf_warning_iterator *xccdf_rule_get_warnings(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct oscap_text_iterator *xccdf_rule_get_rationale(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
const char *xccdf_rule_get_cluster_id(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
float xccdf_rule_get_weight(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
bool xccdf_rule_set_weight(struct xccdf_rule *item, xccdf_numeric newval);
/**
 * @memberof xccdf_rule
 */
const char *xccdf_rule_get_extends(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
bool xccdf_rule_get_abstract(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
bool xccdf_rule_get_prohibit_changes(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
bool xccdf_rule_get_hidden(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
bool xccdf_rule_get_selected(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
bool xccdf_rule_get_multiple(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct oscap_string_iterator *xccdf_rule_get_platforms(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct xccdf_status_iterator *xccdf_rule_get_statuses(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct oscap_reference_iterator *xccdf_rule_get_dc_statuses(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct oscap_reference_iterator *xccdf_rule_get_references(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct xccdf_status * xccdf_rule_get_status_current(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
const char *xccdf_rule_get_impact_metric(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
xccdf_role_t xccdf_rule_get_role(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
xccdf_level_t xccdf_rule_get_severity(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct xccdf_ident_iterator *xccdf_rule_get_idents(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct xccdf_check_iterator *xccdf_rule_get_checks(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct xccdf_profile_note_iterator *xccdf_rule_get_profile_notes(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct xccdf_fix_iterator *xccdf_rule_get_fixes(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct xccdf_fixtext_iterator *xccdf_rule_get_fixtexts(const struct xccdf_rule *rule);
/**
 * @memberof xccdf_rule
 */
struct oscap_string_iterator *xccdf_rule_get_conflicts(const struct xccdf_rule* rule);
/**
 * @memberof xccdf_rule
 */
struct oscap_stringlist_iterator *xccdf_rule_get_requires(const struct xccdf_rule* rule);
/**
 * @memberof xccdf_rule
 */
struct oscap_string_iterator *xccdf_rule_get_metadata(const struct xccdf_rule *rule);

/*
 * Return group's parent in the grouping hierarchy.
 * Returned item will be either a group or a benchmark.
 * @memberof xccdf_group
 */
struct xccdf_item *xccdf_group_get_parent(const struct xccdf_group *group);

/**
 * Get an iterator to the group content. The items are either groups or rules.
 * @memberof xccdf_group
 * @see xccdf_rule
 * @see xccdf_group
 * @see xccdf_item
 */
struct xccdf_item_iterator *xccdf_group_get_content(const struct xccdf_group *group);

/// @memberof xccdf_group
struct xccdf_value_iterator *xccdf_group_get_values(const struct xccdf_group *group);

/// @memberof xccdf_group
const char *xccdf_group_get_id(const struct xccdf_group *group);
/// @memberof xccdf_group
struct oscap_text_iterator *xccdf_group_get_title(const struct xccdf_group *group);
/// @memberof xccdf_group
struct oscap_text_iterator *xccdf_group_get_description(const struct xccdf_group *group);
/// @memberof xccdf_group
const char *xccdf_group_get_version(const struct xccdf_group *group);
/// @memberof xccdf_group
struct oscap_text_iterator *xccdf_group_get_question(const struct xccdf_group *group);
/// @memberof xccdf_group
struct xccdf_warning_iterator *xccdf_group_get_warnings(const struct xccdf_group *group);
/// @memberof xccdf_group
struct oscap_text_iterator *xccdf_group_get_rationale(const struct xccdf_group *group);
/// @memberof xccdf_group
const char *xccdf_group_get_cluster_id(const struct xccdf_group *group);
/// @memberof xccdf_group
float xccdf_group_get_weight(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_set_weight(struct xccdf_group *item, xccdf_numeric newval);
/// @memberof xccdf_group
const char *xccdf_group_get_extends(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_get_abstract(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_get_prohibit_changes(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_get_hidden(const struct xccdf_group *group);
/// @memberof xccdf_group
bool xccdf_group_get_selected(const struct xccdf_group *group);
/// @memberof xccdf_group
struct oscap_string_iterator *xccdf_group_get_platforms(const struct xccdf_group *group);
/// @memberof xccdf_group
struct xccdf_status_iterator *xccdf_group_get_statuses(const struct xccdf_group *group);
/// @memberof xccdf_group
struct oscap_reference_iterator *xccdf_group_get_dc_statuses(const struct xccdf_group *group);
/// @memberof xccdf_group
struct oscap_reference_iterator *xccdf_group_get_references(const struct xccdf_group *group);
/// @memberof xccdf_group
struct xccdf_status * xccdf_group_get_status_current(const struct xccdf_group *group);
/// @memberof xccdf_group
struct oscap_string_iterator *xccdf_group_get_conflicts(const struct xccdf_group* group);
/// @memberof xccdf_group
struct oscap_stringlist_iterator *xccdf_group_get_requires(const struct xccdf_group* group);
/// @memberof xccdf_group
struct oscap_string_iterator *xccdf_group_get_metadata(const struct xccdf_group *group);

/// @memberof xccdf_value
struct oscap_text_iterator *xccdf_value_get_title(const struct xccdf_value *value);
/// @memberof xccdf_value
const char *xccdf_value_get_id(const struct xccdf_value *value);
/// @memberof xccdf_value
struct oscap_text_iterator *xccdf_value_get_description(const struct xccdf_value *value);
/// @memberof xccdf_value
const char *xccdf_value_get_extends(const struct xccdf_value *value);
/// @memberof xccdf_value
bool xccdf_value_get_abstract(const struct xccdf_value *value);
/// @memberof xccdf_value
bool xccdf_value_get_prohibit_changes(const struct xccdf_value *value);
/// @memberof xccdf_value
bool xccdf_value_get_hidden(const struct xccdf_value *value);
/// @memberof xccdf_value
bool xccdf_value_get_interactive(const struct xccdf_value *value);
/// @memberof xccdf_value
struct xccdf_status_iterator *xccdf_value_get_statuses(const struct xccdf_value *value);
/// @memberof xccdf_value
struct oscap_reference_iterator *xccdf_value_get_dc_statuses(const struct xccdf_value *value);
/// @memberof xccdf_value
struct oscap_reference_iterator *xccdf_value_get_references(const struct xccdf_value *value);
/// @memberof xccdf_value
struct xccdf_status * xccdf_value_get_status_current(const struct xccdf_value *value);
/// @memberof xccdf_value
xccdf_value_type_t xccdf_value_get_type(const struct xccdf_value *value);
/// @memberof xccdf_value
xccdf_interface_hint_t xccdf_value_get_interface_hint(const struct xccdf_value *value);
/// @memberof xccdf_value
xccdf_operator_t xccdf_value_get_oper(const struct xccdf_value *value);
/// @memberof xccdf_value
struct xccdf_value_instance *xccdf_value_get_instance_by_selector(const struct xccdf_value *value, const char *selector);
/// @memberof xccdf_value
bool xccdf_value_add_instance(struct xccdf_value *value, struct xccdf_value_instance *instance);
/// @memberof xccdf_value
struct xccdf_value_instance_iterator *xccdf_value_get_instances(const struct xccdf_value *item);
/// @memberof xccdf_value
struct oscap_string_iterator *xccdf_value_get_metadata(const struct xccdf_value *value);

/// @memberof xccdf_value_instance
void xccdf_value_instance_free(struct xccdf_value_instance *inst);
/// @memberof xccdf_value
struct xccdf_value_instance *xccdf_value_new_instance(struct xccdf_value *val);
/// @memberof xccdf_value_instance
const char *xccdf_value_instance_get_selector(const struct xccdf_value_instance *item);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_selector(struct xccdf_value_instance *obj, const char *newval);
/// @memberof xccdf_value_instance
xccdf_value_type_t xccdf_value_instance_get_type(const struct xccdf_value_instance *item);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_get_must_match(const struct xccdf_value_instance *item);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_must_match(struct xccdf_value_instance *obj, bool newval);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_get_value_boolean(const struct xccdf_value_instance *inst);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_value_boolean(struct xccdf_value_instance *inst, bool newval);
/// @memberof xccdf_value_instance
xccdf_numeric xccdf_value_instance_get_value_number(const struct xccdf_value_instance *inst);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_value_number(struct xccdf_value_instance *inst, xccdf_numeric newval);
/// @memberof xccdf_value_instance
const char *xccdf_value_instance_get_value_string(const struct xccdf_value_instance *inst);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_value_string(struct xccdf_value_instance *inst, const char *newval);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_get_defval_boolean(const struct xccdf_value_instance *inst);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_defval_boolean(struct xccdf_value_instance *inst, bool newval);
/// @memberof xccdf_value_instance
xccdf_numeric xccdf_value_instance_get_defval_number(const struct xccdf_value_instance *inst);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_defval_number(struct xccdf_value_instance *inst, xccdf_numeric newval);
/// @memberof xccdf_value_instance
const char *xccdf_value_instance_get_defval_string(const struct xccdf_value_instance *inst);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_defval_string(struct xccdf_value_instance *inst, const char *newval);
/// @memberof xccdf_value_instance
xccdf_numeric xccdf_value_instance_get_lower_bound(const struct xccdf_value_instance *inst);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_lower_bound(struct xccdf_value_instance *inst, xccdf_numeric newval);
/// @memberof xccdf_value_instance
xccdf_numeric xccdf_value_instance_get_upper_bound(const struct xccdf_value_instance *inst);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_upper_bound(struct xccdf_value_instance *inst, xccdf_numeric newval);
/// @memberof xccdf_value_instance
const char *xccdf_value_instance_get_match(const struct xccdf_value_instance *inst);
/// @memberof xccdf_value_instance
bool xccdf_value_instance_set_match(struct xccdf_value_instance *inst, const char *newval);
/// @memberof xccdf_value_instance
const char *  xccdf_value_instance_get_value(const struct xccdf_value_instance * val);

/**
 * Return value's parent in the grouping hierarchy.
 * Returned item will be either a value or a benchmark.
 * @memberof xccdf_value
 */
struct xccdf_item *xccdf_value_get_parent(const struct xccdf_value *value);


/// @memberof xccdf_status
time_t xccdf_status_get_date(const struct xccdf_status *status);
/// @memberof xccdf_status
xccdf_status_type_t xccdf_status_get_status(const struct xccdf_status *status);
/// @memberof xccdf_status
const char *xccdf_status_type_to_text(xccdf_status_type_t id);

/// @memberof xccdf_notice
const char *xccdf_notice_get_id(const struct xccdf_notice *notice);
/// @memberof xccdf_notice
struct oscap_text *xccdf_notice_get_text(const struct xccdf_notice *notice);
/// @memberof xccdf_model
const char *xccdf_model_get_system(const struct xccdf_model *model);
/// @memberof xccdf_ident
const char *xccdf_ident_get_id(const struct xccdf_ident *ident);
/// @memberof xccdf_ident
const char *xccdf_ident_get_system(const struct xccdf_ident *ident);
/// @memberof xccdf_check
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
/// @memberof xccdf_check
const char *xccdf_check_get_system(const struct xccdf_check *check);
/// @memberof xccdf_check
const char *xccdf_check_get_selector(const struct xccdf_check *check);
/// @memberof xccdf_check
const char *xccdf_check_get_content(const struct xccdf_check *check);
/// @memberof xccdf_check
bool xccdf_check_get_multicheck(const struct xccdf_check *check);
/// @memberof xccdf_check
bool xccdf_check_get_negate(const struct xccdf_check *check);
/// @memberof xccdf_check
//struct xccdf_rule *xccdf_check_get_parent(const struct xccdf_check *check);
/**
 * Get an iterator to nested checks of the complex check.
 * @memberof xccdf_check
 * @see xccdf_check_get_export
 */
struct xccdf_check_iterator *xccdf_check_get_children(const struct xccdf_check *check);

/// @memberof xccdf_check_content_ref
const char *xccdf_check_content_ref_get_href(const struct xccdf_check_content_ref *ref);
/// @memberof xccdf_check_content_ref
const char *xccdf_check_content_ref_get_name(const struct xccdf_check_content_ref *ref);
/// @memberof xccdf_profile_note
const char *xccdf_profile_note_get_reftag(const struct xccdf_profile_note *note);
/// @memberof xccdf_profile_note
struct oscap_text *xccdf_profile_note_get_text(const struct xccdf_profile_note *note);
/// @memberof xccdf_check_import
const char *xccdf_check_import_get_name(const struct xccdf_check_import *item);
/// @memberof xccdf_check_import
const char *xccdf_check_import_get_xpath(const struct xccdf_check_import *item);
/// @memberof xccdf_check_import
const char *xccdf_check_import_get_content(const struct xccdf_check_import *item);
/// @memberof xccdf_check_export
const char *xccdf_check_export_get_value(const struct xccdf_check_export *item);
/// @memberof xccdf_check_export
const char *xccdf_check_export_get_name(const struct xccdf_check_export *item);

/// @memberof xccdf_fix
const char *xccdf_fix_get_content(const struct xccdf_fix *fix);
/// @memberof xccdf_fix
bool xccdf_fix_get_reboot(const struct xccdf_fix *fix);
/// @memberof xccdf_fix
xccdf_strategy_t xccdf_fix_get_strategy(const struct xccdf_fix *fix);
/// @memberof xccdf_fix
xccdf_level_t xccdf_fix_get_complexity(const struct xccdf_fix *fix);
/// @memberof xccdf_fix
xccdf_level_t xccdf_fix_get_disruption(const struct xccdf_fix *fix);
/// @memberof xccdf_fix
const char *xccdf_fix_get_id(const struct xccdf_fix *fix);
/// @memberof xccdf_fix
const char *xccdf_fix_get_system(const struct xccdf_fix *fix);
/// @memberof xccdf_fix
const char *xccdf_fix_get_platform(const struct xccdf_fix *fix);
/// @memberof xccdf_fixtext
bool xccdf_fixtext_get_reboot(const struct xccdf_fixtext *fixtext);
/// @memberof xccdf_fixtext
xccdf_strategy_t xccdf_fixtext_get_strategy(const struct xccdf_fixtext *fixtext);
/// @memberof xccdf_fixtext
xccdf_level_t xccdf_fixtext_get_complexity(const struct xccdf_fixtext *fixtext);
/// @memberof xccdf_fixtext
xccdf_level_t xccdf_fixtext_get_disruption(const struct xccdf_fixtext *fixtext);
/// @memberof xccdf_fixtext
const char *xccdf_fixtext_get_fixref(const struct xccdf_fixtext *fixtext);
/// @memberof xccdf_fixtext
struct oscap_text *xccdf_fixtext_get_text(const struct xccdf_fixtext *fixtext);
/// @memberof xccdf_value
const char *xccdf_value_get_version(const struct xccdf_value *value);
/// @memberof xccdf_value
struct oscap_text_iterator *xccdf_value_get_question(const struct xccdf_value *value);
/// @memberof xccdf_value
struct xccdf_warning_iterator *xccdf_value_get_warnings(const struct xccdf_value *value);
/// @memberof xccdf_value
const char *xccdf_value_get_version_update(const struct xccdf_value *value);
/// @memberof xccdf_value
const char *xccdf_value_get_version_time(const struct xccdf_value *value);
/// @memberof xccdf_value
struct xccdf_benchmark *xccdf_value_get_benchmark(const struct xccdf_value *value);
/// @memberof xccdf_value
struct oscap_string_iterator *xccdf_value_get_sources(const struct xccdf_value *value);
/// @memberof xccdf_value
const char *xccdf_value_get_cluster_id(const struct xccdf_value *value);

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
const char *xccdf_item_get_version_time(const struct xccdf_item *item);
/// @memberof xccdf_item
float xccdf_item_get_weight(const struct xccdf_item *item);
/// @memberof xccdf_item
struct xccdf_benchmark *xccdf_item_get_benchmark(const struct xccdf_item *item);
/// @memberof xccdf_item
struct oscap_string_iterator *xccdf_item_get_platforms(const struct xccdf_item *item);

/// @memberof xccdf_benchmark
struct xccdf_warning_iterator *xccdf_benchmark_get_warnings(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
const char *xccdf_benchmark_get_version_update(const struct xccdf_benchmark *benchmark);
/// @memberof xccdf_benchmark
const char *xccdf_benchmark_get_version_time(const struct xccdf_benchmark *benchmark);

/// @memberof xccdf_profile
const char *xccdf_profile_get_version_update(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
const char *xccdf_profile_get_version_time(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
bool xccdf_profile_get_tailoring(const struct xccdf_profile *profile);
/// @memberof xccdf_profile
const char *xccdf_profile_get_note_tag(const struct xccdf_profile *profile);

/// @memberof xccdf_rule
const char *xccdf_rule_get_version_update(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
const char *xccdf_rule_get_version_time(const struct xccdf_rule *rule);
/// @memberof xccdf_rule
struct xccdf_benchmark *xccdf_rule_get_benchmark(const struct xccdf_rule *rule);

/// @memberof xccdf_group
const char *xccdf_group_get_version_time(const struct xccdf_group *group);
/// @memberof xccdf_group
const char *xccdf_group_get_version_update(const struct xccdf_group *group);
/// @memberof xccdf_group
struct xccdf_benchmark *xccdf_group_get_benchmark(const struct xccdf_group *group);

/// @memberof xccdf_check
struct xccdf_check_import_iterator *xccdf_check_get_imports(const struct xccdf_check *check);
/// @memberof xccdf_check
struct xccdf_check_export_iterator *xccdf_check_get_exports(const struct xccdf_check *check);
/// @memberof xccdf_check
struct xccdf_check_content_ref_iterator *xccdf_check_get_content_refs(const struct xccdf_check *check);

/// @memberof xccdf_select
bool xccdf_select_get_selected(const struct xccdf_select *select);
/// @memberof xccdf_select
const char *xccdf_select_get_item(const struct xccdf_select *select);
/// @memberof xccdf_select
struct oscap_text_iterator *xccdf_select_get_remarks(const struct xccdf_select *select);

/// @memberof xccdf_warning
xccdf_warning_category_t xccdf_warning_get_category(const struct xccdf_warning *warning);
/// @memberof xccdf_warning
struct oscap_text *xccdf_warning_get_text(const struct xccdf_warning *warning);
/// @memberof xccdf_refine_rule
const char *  xccdf_refine_rule_get_item(const struct xccdf_refine_rule* rr);
/// @memberof xccdf_refine_rule
const char *  xccdf_refine_rule_get_selector(const struct xccdf_refine_rule* rr);
/// @memberof xccdf_refine_rule
xccdf_role_t  xccdf_refine_rule_get_role(const struct xccdf_refine_rule* rr);
/// @memberof xccdf_refine_rule
xccdf_level_t xccdf_refine_rule_get_severity(const struct xccdf_refine_rule* rr);
/// @memberof xccdf_refine_rule
struct oscap_text_iterator* xccdf_refine_rule_get_remarks(const struct xccdf_refine_rule *rr);
/// @memberof xccdf_refine_rule
xccdf_numeric xccdf_refine_rule_get_weight(const struct xccdf_refine_rule *item);
/// @memberof xccdf_refine_rule
bool xccdf_refine_rule_weight_defined(const struct xccdf_refine_rule *item);
/// @memberof xccdf_refine_value
const char *     xccdf_refine_value_get_item(const struct xccdf_refine_value* rv);
/// @memberof xccdf_refine_value
const char *     xccdf_refine_value_get_selector(const struct xccdf_refine_value* rv);
/// @memberof xccdf_refine_value
xccdf_operator_t xccdf_refine_value_get_oper(const struct xccdf_refine_value* rv);
/// @memberof xccdf_refine_value
struct oscap_text_iterator* xccdf_refine_value_get_remarks(const struct xccdf_refine_value *rv);
/// @memberof xccdf_set_value
const char *xccdf_setvalue_get_item(const struct xccdf_setvalue* sv);
/// @memberof xccdf_set_value
const char *xccdf_setvalue_get_value(const struct xccdf_setvalue* sv);

/// @memberof xccdf_plain_text
const char *xccdf_plain_text_get_id(const struct xccdf_plain_text *item);
/// @memberof xccdf_plain_text
const char *xccdf_plain_text_get_text(const struct xccdf_plain_text *item);

/// @memberof xccdf_result
struct xccdf_benchmark *xccdf_result_get_benchmark(const struct xccdf_result *item);
/// @memberof xccdf_result
const char *xccdf_result_get_id(const struct xccdf_result *item);
/// @memberof xccdf_result
struct oscap_text_iterator *xccdf_result_get_title(const struct xccdf_result *item);
/// @memberof xccdf_result
const char *xccdf_result_get_version(const struct xccdf_result *item);
/// @memberof xccdf_result
struct oscap_string_iterator *xccdf_result_get_platforms(const struct xccdf_result *item);
/// @memberof xccdf_result
struct xccdf_status_iterator *xccdf_result_get_statuses(const struct xccdf_result *item);
/// @memberof xccdf_result
const char *xccdf_result_get_test_system(const struct xccdf_result *item);
/// @memberof xccdf_result
const char *xccdf_result_get_benchmark_uri(const struct xccdf_result *item);
/// @memberof xccdf_result
const char *xccdf_result_get_profile(const struct xccdf_result *item);
/// @memberof xccdf_result
struct xccdf_identity_iterator *xccdf_result_get_identities(const struct xccdf_result *item);
/// @memberof xccdf_result
struct oscap_string_iterator *xccdf_result_get_targets(const struct xccdf_result *item);
/// @memberof xccdf_result
struct oscap_string_iterator *xccdf_result_get_target_addresses(const struct xccdf_result *item);
/// @memberof xccdf_result
struct oscap_string_iterator *xccdf_result_get_applicable_platforms(const struct xccdf_result *item);
/// @memberof xccdf_result
struct oscap_string_iterator *xccdf_result_get_organizations(const struct xccdf_result *item);
/// @memberof xccdf_result
struct oscap_text_iterator *xccdf_result_get_remarks(const struct xccdf_result *item);
/// @memberof xccdf_result
struct xccdf_target_fact_iterator *xccdf_result_get_target_facts(const struct xccdf_result *item);
/// @memberof xccdf_result
struct xccdf_target_identifier_iterator *xccdf_result_get_target_id_refs(const struct xccdf_result *item);
/// @memberof xccdf_result
struct xccdf_setvalue_iterator *xccdf_result_get_setvalues(const struct xccdf_result *item);
/// @memberof xccdf_result
struct xccdf_rule_result_iterator *xccdf_result_get_rule_results(const struct xccdf_result *item);
/// @memberof xccdf_result
struct xccdf_score_iterator *xccdf_result_get_scores(const struct xccdf_result *item);
/// @memberof xccdf_result
const char * xccdf_result_get_start_time(const struct xccdf_result *item);
/// @memberof xccdf_result
const char * xccdf_result_get_end_time(const struct xccdf_result *item);
/// @memberof xccdf_result
struct oscap_string_iterator *xccdf_result_get_metadata(const struct xccdf_result *result);

/// @memberof xccdf_rule_result
const char * xccdf_rule_result_get_time(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
xccdf_role_t xccdf_rule_result_get_role(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
float xccdf_rule_result_get_weight(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
xccdf_level_t xccdf_rule_result_get_severity(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
xccdf_test_result_type_t xccdf_rule_result_get_result(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
const char *xccdf_rule_result_get_version(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
const char *xccdf_rule_result_get_idref(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
struct xccdf_ident_iterator *xccdf_rule_result_get_idents(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
struct xccdf_fix_iterator *xccdf_rule_result_get_fixes(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
struct xccdf_check_iterator *xccdf_rule_result_get_checks(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
struct xccdf_override_iterator *xccdf_rule_result_get_overrides(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
struct xccdf_message_iterator *xccdf_rule_result_get_messages(const struct xccdf_rule_result *item);
/// @memberof xccdf_rule_result
struct xccdf_instance_iterator *xccdf_rule_result_get_instances(const struct xccdf_rule_result *item);
/// @memberof xccdf_identity
bool xccdf_identity_get_authenticated(const struct xccdf_identity *item);
/// @memberof xccdf_identity
bool xccdf_identity_get_privileged(const struct xccdf_identity *item);
/// @memberof xccdf_identity
const char *xccdf_identity_get_name(const struct xccdf_identity *item);
/// @memberof xccdf_score
xccdf_numeric xccdf_score_get_maximum(const struct xccdf_score *item);
/// @memberof xccdf_score
xccdf_numeric xccdf_score_get_score(const struct xccdf_score *item);
/// @memberof xccdf_score
const char *xccdf_score_get_system(const struct xccdf_score *item);
/// @memberof xccdf_override
const char *xccdf_override_get_time(const struct xccdf_override *item);
/// @memberof xccdf_override
xccdf_test_result_type_t xccdf_override_get_new_result(const struct xccdf_override *item);
/// @memberof xccdf_override
xccdf_test_result_type_t xccdf_override_get_old_result(const struct xccdf_override *item);
/// @memberof xccdf_override
const char *xccdf_override_get_authority(const struct xccdf_override *item);
/// @memberof xccdf_override
struct oscap_text *xccdf_override_get_remark(const struct xccdf_override *item);
/// @memberof xccdf_message
xccdf_message_severity_t xccdf_message_get_severity(const struct xccdf_message *item);
/// @memberof xccdf_message
const char *xccdf_message_get_content(const struct xccdf_message *item);
/// @memberof xccdf_target_fact
xccdf_value_type_t xccdf_target_fact_get_type(const struct xccdf_target_fact *item);
/// @memberof xccdf_target_fact
const char *xccdf_target_fact_get_value(const struct xccdf_target_fact *item);
/// @memberof xccdf_target_fact
const char *xccdf_target_fact_get_name(const struct xccdf_target_fact *item);
/// @memberof xccdf_target_identifier
void* xccdf_target_identifier_get_xml_node(const struct xccdf_target_identifier *item);
/// @memberof xccdf_target_identifier
const char *xccdf_target_identifier_get_system(const struct xccdf_target_identifier *item);
/// @memberof xccdf_target_identifier
const char *xccdf_target_identifier_get_href(const struct xccdf_target_identifier *item);
/// @memberof xccdf_target_identifier
const char *xccdf_target_identifier_get_name(const struct xccdf_target_identifier *item);
/// @memberof xccdf_instance
const char *xccdf_instance_get_context(const struct xccdf_instance *item);
/// @memberof xccdf_instance
const char *xccdf_instance_get_parent_context(const struct xccdf_instance *item);
/// @memberof xccdf_instance
const char *xccdf_instance_get_content(const struct xccdf_instance *item);
/// @memberof xccdf_tailoring
struct xccdf_tailoring *xccdf_tailoring_import_source(struct oscap_source *source, struct xccdf_benchmark *benchmark);
/*
 * @memberof xccdf_tailoring
 * @deprecated This function has been deprecated by @ref xccdf_tailoring_import_source.
 * This function may be dropped from later versions of the library.
 */
OSCAP_DEPRECATED(struct xccdf_tailoring *xccdf_tailoring_import(const char *file, struct xccdf_benchmark *benchmark));

/// @memberof xccdf_tailoring
const char *xccdf_tailoring_get_id(const struct xccdf_tailoring *tailoring);
/// @memberof xccdf_tailoring
const char *xccdf_tailoring_get_version(const struct xccdf_tailoring *tailoring);
/// @memberof xccdf_tailoring
const char *xccdf_tailoring_get_version_update(const struct xccdf_tailoring *tailoring);
/// @memberof xccdf_tailoring
const char *xccdf_tailoring_get_version_time(const struct xccdf_tailoring *tailoring);
/// @memberof xccdf_tailoring
const char *xccdf_tailoring_get_benchmark_ref(const struct xccdf_tailoring *tailoring);
/// @memberof xccdf_tailoring
const char *xccdf_tailoring_get_benchmark_ref_version(const struct xccdf_tailoring *tailoring);
/// @memberof xccdf_tailoring
struct oscap_string_iterator *xccdf_tailoring_get_metadata(const struct xccdf_tailoring *tailoring);
/// @memberof xccdf_tailoring
struct xccdf_profile_iterator *xccdf_tailoring_get_profiles(const struct xccdf_tailoring *tailoring);
/// @memberof xccdf_tailoring
struct xccdf_status_iterator *xccdf_tailoring_get_statuses(const struct xccdf_tailoring *tailoring);
/// @memberof xccdf_tailoring
struct oscap_reference_iterator *xccdf_tailoring_get_dc_statuses(const struct xccdf_tailoring *tailoring);
/**
 * @param profile_id id of the profile that should be returned or NULL for default profile
 * @note
 * Unlike in XCCDF Benchmark, the default profile from Tailoring element needn't
 * match the "selected" attribute from each individual XCCDF Item.
 * @memberof xccdf_tailoring
 */
struct xccdf_profile *xccdf_tailoring_get_profile_by_id(const struct xccdf_tailoring *tailoring, const char *profile_id);

/************************************************************
 ** @} End of Getters group */

/************************************************************/
/**
 * @name Setters
 * For lists use add functions. Parameters of set functions are duplicated in memory and need to 
 * be freed by caller.
 * @{
 */

/// @memberof xccdf_item
bool xccdf_item_set_weight(struct xccdf_item *item, xccdf_numeric newval);
/// @memberof xccdf_item
bool xccdf_item_set_id(struct xccdf_item *item, const char *newval);
/// @memberof xccdf_item
bool xccdf_item_set_cluster_id(struct xccdf_item *item, const char *newval);
/// @memberof xccdf_item
bool xccdf_item_set_extends(struct xccdf_item *item, const char *newval);
/// @memberof xccdf_item
bool xccdf_item_set_version(struct xccdf_item *item, const char *newval);
/// @memberof xccdf_item
bool xccdf_item_set_version_time(struct xccdf_item *item, const char *newval);
/// @memberof xccdf_item
bool xccdf_item_set_version_update(struct xccdf_item *item, const char *newval);
/// @memberof xccdf_item
bool xccdf_item_set_abstract(struct xccdf_item *item, bool newval);
/// @memberof xccdf_item
bool xccdf_item_set_hidden(struct xccdf_item *item, bool newval);
/// @memberof xccdf_item
bool xccdf_item_set_prohibit_changes(struct xccdf_item *item, bool newval);
/// @memberof xccdf_item
bool xccdf_item_set_selected(struct xccdf_item *item, bool newval);

/// @memberof xccdf_item
bool xccdf_item_add_metadata(struct xccdf_item *item, const char* metadata);

/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_resolved(struct xccdf_benchmark *item, bool newval);

/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_style_href(struct xccdf_benchmark *item, const char *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_style(struct xccdf_benchmark *item, const char *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_id(struct xccdf_benchmark *item, const char *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_version(struct xccdf_benchmark *item, const char *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_version_time(struct xccdf_benchmark *item, const char *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_version_update(struct xccdf_benchmark *item, const char *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_schema_version(struct xccdf_benchmark* item, const struct xccdf_version_info* newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_metadata(struct xccdf_benchmark* item, const char* metadata);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_cpe_list(struct xccdf_benchmark* item, struct cpe_dict_model* cpe_list);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_set_cpe_lang_model(struct xccdf_benchmark* item, struct cpe_lang_model* cpe_lang_model);
/// @memberof xccdf_profile
bool xccdf_profile_set_note_tag(struct xccdf_profile *item, const char *newval);
/// @memberof xccdf_profile
bool xccdf_profile_set_id(struct xccdf_profile *item, const char *newval);
/// @memberof xccdf_profile
bool xccdf_profile_set_abstract(struct xccdf_profile *item, bool newval);
/// @memberof xccdf_profile
bool xccdf_profile_set_prohibit_changes(struct xccdf_profile *item, bool newval);
/// @memberof xccdf_profile
bool xccdf_profile_set_extends(struct xccdf_profile *item, const char *newval);
/// @memberof xccdf_profile
bool xccdf_profile_set_version(struct xccdf_profile *item, const char *newval);
/// @memberof xccdf_profile
bool xccdf_profile_set_version_time(struct xccdf_profile *item, const char *newval);
/// @memberof xccdf_profile
bool xccdf_profile_set_version_update(struct xccdf_profile *item, const char *newval);
/// @memberof xccdf_profile
bool xccdf_profile_set_tailoring(struct xccdf_profile *item, bool tailoring);
/// @memberof xccdf_profile
bool xccdf_profile_add_metadata(struct xccdf_profile* item, const char* metadata);

/// @memberof xccdf_rule
bool xccdf_rule_set_id(struct xccdf_rule *item, const char *newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_cluster_id(struct xccdf_rule *item, const char *newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_extends(struct xccdf_rule *item, const char *newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_version(struct xccdf_rule *item, const char *newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_version_time(struct xccdf_rule *item, const char *newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_version_update(struct xccdf_rule *item, const char *newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_abstract(struct xccdf_rule *item, bool newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_hidden(struct xccdf_rule *item, bool newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_prohibit_changes(struct xccdf_rule *item, bool newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_selected(struct xccdf_rule *item, bool newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_multiple(struct xccdf_rule *item, bool newval);
/// @memberof xccdf_rule
//bool xccdf_rule_set_selector(struct xccdf_rule *item, const char * selector);
/// @memberof xccdf_rule
bool xccdf_rule_set_impact_metric(struct xccdf_rule *item, const char *newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_role(struct xccdf_rule *item, xccdf_role_t newval);
/// @memberof xccdf_rule
bool xccdf_rule_set_severity(struct xccdf_rule *item, xccdf_level_t newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_metadata(struct xccdf_rule* item, const char* metadata);

/// @memberof xccdf_group
bool xccdf_group_set_id(struct xccdf_group *item, const char *newval);
/// @memberof xccdf_group
bool xccdf_group_set_cluster_id(struct xccdf_group *item, const char *newval);
/// @memberof xccdf_group
bool xccdf_group_set_extends(struct xccdf_group *item, const char *newval);
/// @memberof xccdf_group
bool xccdf_group_set_version(struct xccdf_group *item, const char *newval);
/// @memberof xccdf_group
bool xccdf_group_set_version_time(struct xccdf_group *item, const char *newval);
/// @memberof xccdf_group
bool xccdf_group_set_version_update(struct xccdf_group *item, const char *newval);
/// @memberof xccdf_group
bool xccdf_group_set_abstract(struct xccdf_group *item, bool newval);
/// @memberof xccdf_group
bool xccdf_group_set_hidden(struct xccdf_group *item, bool newval);
/// @memberof xccdf_group
bool xccdf_group_set_prohibit_changes(struct xccdf_group *item, bool newval);
/// @memberof xccdf_group
bool xccdf_group_set_selected(struct xccdf_group *item, bool newval);
/// @memberof xccdf_group
bool xccdf_group_add_metadata(struct xccdf_group* item, const char* metadata);

/// @memberof xccdf_value
bool xccdf_value_set_id(struct xccdf_value *item, const char *newval);
/// @memberof xccdf_value
bool xccdf_value_set_cluster_id(struct xccdf_value *item, const char *newval);
/// @memberof xccdf_value
bool xccdf_value_set_extends(struct xccdf_value *item, const char *newval);
/// @memberof xccdf_value
bool xccdf_value_set_version(struct xccdf_value *item, const char *newval);
/// @memberof xccdf_value
bool xccdf_value_set_version_time(struct xccdf_value *item, const char *newval);
/// @memberof xccdf_value
bool xccdf_value_set_version_update(struct xccdf_value *item, const char *newval);
/// @memberof xccdf_value
bool xccdf_value_set_abstract(struct xccdf_value *item, bool newval);
/// @memberof xccdf_value
bool xccdf_value_set_hidden(struct xccdf_value *item, bool newval);
/// @memberof xccdf_value
bool xccdf_value_set_multiple(struct xccdf_value *item, bool newval);
/// @memberof xccdf_value
bool xccdf_value_set_prohibit_changes(struct xccdf_value *item, bool newval);
/// @memberof xccdf_value
bool xccdf_value_set_oper(struct xccdf_value * item, xccdf_operator_t oper);
/// @memberof xccdf_value
bool xccdf_value_set_interactive(struct xccdf_value *item, bool newval);
/// @memberof xccdf_value
bool xccdf_value_add_metadata(struct xccdf_value* item, const char* metadata);

/// @memberof xccdf_status
bool xccdf_status_set_date(struct xccdf_status *obj, time_t newval);
/// @memberof xccdf_status
bool xccdf_status_set_status(struct xccdf_status *obj, xccdf_status_type_t newval);

/// @memberof xccdf_notice
bool xccdf_notice_set_id(struct xccdf_notice *obj, const char *newval);
/// @memberof xccdf_notice
bool xccdf_notice_set_text(struct xccdf_notice *obj, struct oscap_text *newval);

/// @memberof xccdf_model
bool xccdf_model_set_system(struct xccdf_model *obj, const char *newval);

/// @memberof xccdf_check
bool xccdf_check_set_id(struct xccdf_check *obj, const char *newval);
/// @memberof xccdf_check
bool xccdf_check_set_system(struct xccdf_check *obj, const char *newval);
/// @memberof xccdf_check
bool xccdf_check_set_selector(struct xccdf_check *obj, const char *newval);
/// @memberof xccdf_check
bool xccdf_check_set_content(struct xccdf_check *obj, const char *newval);
/// @memberof xccdf_check
bool xccdf_check_set_oper(struct xccdf_check *obj, xccdf_bool_operator_t newval);
/// @memberof xccdf_check
bool xccdf_check_set_multicheck(struct xccdf_check *obj, bool newval);
/// @memberof xccdf_check
bool xccdf_check_set_negate(struct xccdf_check *obj, bool newval);

/// @memberof xccdf_check_content_ref
bool xccdf_check_content_ref_set_name(struct xccdf_check_content_ref *obj, const char *newval);
/// @memberof xccdf_check_content_ref
bool xccdf_check_content_ref_set_href(struct xccdf_check_content_ref *obj, const char *newval);

/// @memberof xccdf_profile_note
bool xccdf_profile_note_set_reftag(struct xccdf_profile_note *obj, const char *newval);
/// @memberof xccdf_profile_note
bool xccdf_profile_note_set_text(struct xccdf_profile_note *obj, struct oscap_text *newval);

/// @memberof xccdf_check_import
bool xccdf_check_import_set_name(struct xccdf_check_import *obj, const char *newval);
/// @memberof xccdf_check_import
bool xccdf_check_import_set_xpath(struct xccdf_check_import *obj, const char *newval);
/// @memberof xccdf_check_import
bool xccdf_check_import_set_content(struct xccdf_check_import *obj, const char *newval);

/// @memberof xccdf_check_export
bool xccdf_check_export_set_name(struct xccdf_check_export *obj, const char *newval);
/// @memberof xccdf_check_export
bool xccdf_check_export_set_value(struct xccdf_check_export *obj, const char *newval);

/// @memberof xccdf_fix
bool xccdf_fix_set_strategy(struct xccdf_fix *obj, xccdf_strategy_t newval);
/// @memberof xccdf_fix
bool xccdf_fix_set_disruption(struct xccdf_fix *obj, xccdf_level_t newval);
/// @memberof xccdf_fix
bool xccdf_fix_set_complexity(struct xccdf_fix *obj, xccdf_level_t newval);
/// @memberof xccdf_fix
bool xccdf_fix_set_reboot(struct xccdf_fix *obj, bool newval);
/// @memberof xccdf_fix
bool xccdf_fix_set_content(struct xccdf_fix *obj, const char *newval);
/// @memberof xccdf_fix
bool xccdf_fix_set_system(struct xccdf_fix *obj, const char *newval);
/// @memberof xccdf_fix
bool xccdf_fix_set_platform(struct xccdf_fix *obj, const char *newval);
/// @memberof xccdf_fix
bool xccdf_fix_set_id(struct xccdf_fix *obj, const char *newval);

/// @memberof xccdf_fixtext
bool xccdf_fixtext_set_strategy(struct xccdf_fixtext *obj, xccdf_strategy_t newval);
/// @memberof xccdf_fixtext
bool xccdf_fixtext_set_disruption(struct xccdf_fixtext *obj, xccdf_level_t newval);
/// @memberof xccdf_fixtext
bool xccdf_fixtext_set_complexity(struct xccdf_fixtext *obj, xccdf_level_t newval);
/// @memberof xccdf_fixtext
bool xccdf_fixtext_set_reboot(struct xccdf_fixtext *obj, bool newval);
/// @memberof xccdf_fixtext
bool xccdf_fixtext_set_text(struct xccdf_fixtext *obj, struct oscap_text *newval);
/// @memberof xccdf_fixtext
bool xccdf_fixtext_set_fixref(struct xccdf_fixtext *obj, const char *newval);

/// @memberof xccdf_select
bool xccdf_select_set_item(struct xccdf_select *obj, const char *newval);
/// @memberof xccdf_select
bool xccdf_select_set_selected(struct xccdf_select *obj, bool newval);

/// @memberof xccdf_warning
bool xccdf_warning_set_category(struct xccdf_warning *obj, xccdf_warning_category_t newval);
/// @memberof xccdf_warning
bool xccdf_warning_set_text(struct xccdf_warning *obj, struct oscap_text *newval);
/// @memberof xccdf_refine_rule
struct xccdf_refine_rule *xccdf_refine_rule_new(void);

/// @memberof xccdf_refine_rule
struct xccdf_refine_rule * xccdf_refine_rule_clone(const struct xccdf_refine_rule * old_rule);
/// @memberof xccdf_refine_rule
bool xccdf_refine_rule_set_item(struct xccdf_refine_rule *obj, const char *newval);
/// @memberof xccdf_refine_rule
bool xccdf_refine_rule_set_selector(struct xccdf_refine_rule *obj, const char *newval);
/// @memberof xccdf_refine_rule
bool xccdf_refine_rule_set_role(struct xccdf_refine_rule *obj, xccdf_role_t newval);
/// @memberof xccdf_refine_rule
bool xccdf_refine_rule_set_severity(struct xccdf_refine_rule *obj, xccdf_level_t newval);
/// @memberof xccdf_refine_rule
bool xccdf_refine_rule_set_weight(struct xccdf_refine_rule *obj, xccdf_numeric newval);

/// @memberof xccdf_refine_value
struct xccdf_refine_value *xccdf_refine_value_new(void);
/// @memberof xccdf_refine_value
struct xccdf_refine_value * xccdf_refine_value_clone(const struct xccdf_refine_value * old_value);
/// @memberof xccdf_refine_value
bool xccdf_refine_value_set_item(struct xccdf_refine_value *obj, const char *newval);
/// @memberof xccdf_refine_value
bool xccdf_refine_value_set_selector(struct xccdf_refine_value *obj, const char *newval);
/// @memberof xccdf_refine_value
bool xccdf_refine_value_set_oper(struct xccdf_refine_value *obj, xccdf_operator_t newval);

/// @memberof xccdf_set_value
struct xccdf_setvalue *xccdf_setvalue_new(void);
/// @memberof xccdf_set_value
struct xccdf_setvalue * xccdf_setvalue_clone(const struct xccdf_setvalue * old_value);
/// @memberof xccdf_set_value
bool xccdf_setvalue_set_item(struct xccdf_setvalue *obj, const char *newval);
/// @memberof xccdf_set_value
bool xccdf_setvalue_set_value(struct xccdf_setvalue *obj, const char *newval);
/// @memberof xccdf_plain_text
bool xccdf_plain_text_set_id(struct xccdf_plain_text *obj, const char *newval);
/// @memberof xccdf_plain_text
bool xccdf_plain_text_set_text(struct xccdf_plain_text *obj, const char *newval);

/// @memberof xccdf_result
bool xccdf_result_set_id(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_set_test_system(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_set_benchmark_uri(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_set_profile(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_set_start_time(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_set_end_time(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_set_version(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_add_metadata(struct xccdf_result *item, const char *metadata);

/// @memberof xccdf_rule_result
bool xccdf_rule_result_set_time(struct xccdf_rule_result *obj, const char *newval);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_set_role(struct xccdf_rule_result *obj, xccdf_role_t newval);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_set_weight(struct xccdf_rule_result *obj, float newval);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_set_severity(struct xccdf_rule_result *obj, xccdf_level_t newval);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_set_result(struct xccdf_rule_result *obj, xccdf_test_result_type_t newval);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_set_version(struct xccdf_rule_result *obj, const char *newval);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_set_idref(struct xccdf_rule_result *obj, const char *newval);

/// @memberof xccdf_identity
bool xccdf_identity_set_authenticated(struct xccdf_identity *obj, bool newval);
/// @memberof xccdf_identity
bool xccdf_identity_set_privileged(struct xccdf_identity *obj, bool newval);
/// @memberof xccdf_identity
bool xccdf_identity_set_name(struct xccdf_identity *obj, const char *newval);

/// @memberof xccdf_score
bool xccdf_score_set_maximum(struct xccdf_score *obj, xccdf_numeric newval);
/// @memberof xccdf_score
bool xccdf_score_set_score(struct xccdf_score *obj, xccdf_numeric newval);
/// @memberof xccdf_score
bool xccdf_score_set_system(struct xccdf_score *obj, const char *newval);

/// @memberof xccdf_override
bool xccdf_override_set_time(struct xccdf_override *obj, const char *newval);
/// @memberof xccdf_override
bool xccdf_override_set_new_result(struct xccdf_override *obj, xccdf_test_result_type_t newval);
/// @memberof xccdf_override
bool xccdf_override_set_old_result(struct xccdf_override *obj, xccdf_test_result_type_t newval);
/// @memberof xccdf_override
bool xccdf_override_set_authority(struct xccdf_override *obj, const char *newval);
/// @memberof xccdf_override
bool xccdf_override_set_remark(struct xccdf_override *obj, struct oscap_text *newval);

/// @memberof xccdf_message
bool xccdf_message_set_severity(struct xccdf_message *obj, xccdf_message_severity_t newval);
/// @memberof xccdf_message
bool xccdf_message_set_content(struct xccdf_message *obj, const char *newval);

/// @memberof xccdf_target_fact
bool xccdf_target_fact_set_string(struct xccdf_target_fact *fact, const char *str);
/// @memberof xccdf_target_fact
bool xccdf_target_fact_set_number(struct xccdf_target_fact *fact, xccdf_numeric val);
/// @memberof xccdf_target_fact
bool xccdf_target_fact_set_boolean(struct xccdf_target_fact *fact, bool val);
/// @memberof xccdf_target_fact
bool xccdf_target_fact_set_name(struct xccdf_target_fact *obj, const char *newval);

/// @memberof xccdf_target_identifier
bool xccdf_target_identifier_set_xml_node(struct xccdf_target_identifier *ti, void* node);
/// @memberof xccdf_target_identifier
bool xccdf_target_identifier_set_system(struct xccdf_target_identifier *ti, const char *newval);
/// @memberof xccdf_target_identifier
bool xccdf_target_identifier_set_href(struct xccdf_target_identifier *ti, const char *newval);
/// @memberof xccdf_target_identifier
bool xccdf_target_identifier_set_name(struct xccdf_target_identifier *ti, const char *newval);

/// @memberof xccdf_instance
bool xccdf_instance_set_context(struct xccdf_instance *obj, const char *newval);
/// @memberof xccdf_instance
bool xccdf_instance_set_parent_context(struct xccdf_instance *obj, const char *newval);
/// @memberof xccdf_instance
bool xccdf_instance_set_content(struct xccdf_instance *obj, const char *newval);

/// @memberof xccdf_tailoring
bool xccdf_tailoring_set_id(struct xccdf_tailoring *tailoring, const char* newval);
/// @memberof xccdf_tailoring
bool xccdf_tailoring_set_version(struct xccdf_tailoring *tailoring, const char* newval);
/// @memberof xccdf_tailoring
bool xccdf_tailoring_set_version_update(struct xccdf_tailoring *tailoring, const char *newval);
/// @memberof xccdf_tailoring
bool xccdf_tailoring_set_version_time(struct xccdf_tailoring *tailoring, const char *newval);
/// @memberof xccdf_tailoring
bool xccdf_tailoring_set_benchmark_ref(struct xccdf_tailoring *tailoring, const char *newval);
/// @memberof xccdf_tailoring
bool xccdf_tailoring_set_benchmark_ref_version(struct xccdf_tailoring *tailoring, const char *newval);

/// @memberof xccdf_tailoring
bool xccdf_tailoring_add_profile(struct xccdf_tailoring *tailoring, struct xccdf_profile *profile);
/**
 * Removes given profile from tailoring.
 *
 * The profile must not be an ancestor of any other profile in tailoring.
 * If it is this function will fail to remove the profile and signal the error.
 *
 * This function does NOT notify xccdf_session, xccdf_policy or xccdf_policy_model
 * of this change. You are responsible for refreshing the higher-level structures yourself!
 *
 * @note User is responsible for freeing the profile!
 * @memberof xccdf_tailoring
 */
bool xccdf_tailoring_remove_profile(struct xccdf_tailoring *tailoring, struct xccdf_profile *profile);
/// @memberof xccdf_tailoring
bool xccdf_tailoring_resolve(struct xccdf_tailoring *tailoring, struct xccdf_benchmark *benchmark);

// @memberof xccdf_ident
void xccdf_ident_set_id(struct xccdf_ident * ident, const char *id);
// @memberof xccdf_ident
void xccdf_ident_set_system(struct xccdf_ident * ident, const char *sys);

/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_result(struct xccdf_benchmark *bench, struct xccdf_result *result);

/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_description(struct xccdf_benchmark *item, struct oscap_text *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_platform(struct xccdf_benchmark *item, const char *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_reference(struct xccdf_benchmark *item, struct oscap_reference *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_status(struct xccdf_benchmark *item, struct xccdf_status *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_dc_status(struct xccdf_benchmark *item, struct oscap_reference *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_title(struct xccdf_benchmark *item, struct oscap_text *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_front_matter(struct xccdf_benchmark *item, struct oscap_text *newval);
/// @memberof xccdf_benchmark
//bool xccdf_benchmark_add_item(struct xccdf_benchmark *item, struct xccdf_item *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_model(struct xccdf_benchmark *item, struct xccdf_model *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_notice(struct xccdf_benchmark *item, struct xccdf_notice *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_plain_text(struct xccdf_benchmark *item, struct xccdf_plain_text *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_profile(struct xccdf_benchmark *item, struct xccdf_profile *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_rear_matter(struct xccdf_benchmark *item, struct oscap_text *newval);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_rule(struct xccdf_benchmark *benchmark, struct xccdf_rule *rule);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_group(struct xccdf_benchmark *benchmark, struct xccdf_group *group);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_value(struct xccdf_benchmark *benchmark, struct xccdf_value *value);
/// @memberof xccdf_benchmark
bool xccdf_benchmark_add_content(struct xccdf_benchmark *bench, struct xccdf_item *item);

/// @memberof xccdf_profile
bool xccdf_profile_add_select(struct xccdf_profile *item, struct xccdf_select *newval);
/// @memberof xccdf_profile
bool xccdf_profile_add_setvalue(struct xccdf_profile *item, struct xccdf_setvalue *newval);
/// @memberof xccdf_profile
bool xccdf_profile_add_refine_value(struct xccdf_profile *item, struct xccdf_refine_value *newval);
/// @memberof xccdf_profile
bool xccdf_profile_add_refine_rule(struct xccdf_profile *item, struct xccdf_refine_rule *newval);

/// @memberof xccdf_profile
bool xccdf_profile_add_description(struct xccdf_profile *item, struct oscap_text *newval);
/// @memberof xccdf_profile
bool xccdf_profile_add_platform(struct xccdf_profile *item, const char *newval);
/// @memberof xccdf_profile
bool xccdf_profile_add_reference(struct xccdf_profile *item, struct oscap_reference *newval);
/// @memberof xccdf_profile
bool xccdf_profile_add_status(struct xccdf_profile *item, struct xccdf_status *newval);
/// @memberof xccdf_profile
bool xccdf_profile_add_dc_status(struct xccdf_profile *item, struct oscap_reference *newval);
/// @memberof xccdf_profile
bool xccdf_profile_add_title(struct xccdf_profile *item, struct oscap_text *newval);

/// @memberof xccdf_rule
bool xccdf_rule_add_description(struct xccdf_rule *item, struct oscap_text *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_platform(struct xccdf_rule *item, const char *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_question(struct xccdf_rule *item, struct oscap_text *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_rationale(struct xccdf_rule *item, struct oscap_text *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_reference(struct xccdf_rule *item, struct oscap_reference *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_status(struct xccdf_rule *item, struct xccdf_status *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_dc_status(struct xccdf_rule *item, struct oscap_reference *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_title(struct xccdf_rule *item, struct oscap_text *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_warning(struct xccdf_rule *item, struct xccdf_warning *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_ident(struct xccdf_rule *item, struct xccdf_ident *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_check(struct xccdf_rule *item, struct xccdf_check *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_profile_note(struct xccdf_rule *item, struct xccdf_profile_note *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_fix(struct xccdf_rule *item, struct xccdf_fix *newval);
/// @memberof xccdf_rule
bool xccdf_rule_add_fixtext(struct xccdf_rule *item, struct xccdf_fixtext *newval);

/// @memberof xccdf_group
bool xccdf_group_add_description(struct xccdf_group *item, struct oscap_text *newval);
/// @memberof xccdf_group
bool xccdf_group_add_platform(struct xccdf_group *item, const char *newval);
/// @memberof xccdf_group
bool xccdf_group_add_question(struct xccdf_group *item, struct oscap_text *newval);
/// @memberof xccdf_group
bool xccdf_group_add_rationale(struct xccdf_group *item, struct oscap_text *newval);
/// @memberof xccdf_group
bool xccdf_group_add_reference(struct xccdf_group *item, struct oscap_reference *newval);
/// @memberof xccdf_group
bool xccdf_group_add_status(struct xccdf_group *item, struct xccdf_status *newval);
/// @memberof xccdf_group
bool xccdf_group_add_dc_status(struct xccdf_group *item, struct oscap_reference *newval);
/// @memberof xccdf_group
bool xccdf_group_add_title(struct xccdf_group *item, struct oscap_text *newval);
/// @memberof xccdf_group
bool xccdf_group_add_warning(struct xccdf_group *item, struct xccdf_warning *newval);
/// @memberof xccdf_group
bool xccdf_group_add_rule(struct xccdf_group *group, struct xccdf_rule *item);
/// @memberof xccdf_group
bool xccdf_group_add_group(struct xccdf_group *group, struct xccdf_group *item);
/// @memberof xccdf_group
bool xccdf_group_add_value(struct xccdf_group *group, struct xccdf_value *item);
/// @memberof xccdf_group
bool xccdf_group_add_content(struct xccdf_group *rule, struct xccdf_item *item);

/// @memberof xccdf_value
bool xccdf_value_add_description(struct xccdf_value *item, struct oscap_text *newval);
/// @memberof xccdf_value
bool xccdf_value_add_question(struct xccdf_value *item, struct oscap_text *newval);
/// @memberof xccdf_value
bool xccdf_value_add_reference(struct xccdf_value *item, struct oscap_reference *newval);
/// @memberof xccdf_value
bool xccdf_value_add_status(struct xccdf_value *item, struct xccdf_status *newval);
/// @memberof xccdf_value
bool xccdf_value_add_dc_status(struct xccdf_value *item, struct oscap_reference *newval);
/// @memberof xccdf_value
bool xccdf_value_add_title(struct xccdf_value *item, struct oscap_text *newval);
/// @memberof xccdf_value
bool xccdf_value_add_warning(struct xccdf_value *item, struct xccdf_warning *newval);

/// @memberof xccdf_check
bool xccdf_check_add_import(struct xccdf_check *obj, struct xccdf_check_import *item);
/// @memberof xccdf_check
bool xccdf_check_add_export(struct xccdf_check *obj, struct xccdf_check_export *item);
/// @memberof xccdf_check
bool xccdf_check_add_content_ref(struct xccdf_check *obj, struct xccdf_check_content_ref *item);
/// @memberof xccdf_check
bool xccdf_check_add_child(struct xccdf_check *obj, struct xccdf_check *item);
/// @memberof xccdf_select
bool xccdf_select_add_remark(struct xccdf_select *obj, struct oscap_text *item);
/// @memberof xccdf_refine_value
bool xccdf_refine_value_add_remark(struct xccdf_refine_value *obj, struct oscap_text *item);
/// @memberof xccdf_result
bool xccdf_result_add_rule_result(struct xccdf_result *item, struct xccdf_rule_result *newval);
/// @memberof xccdf_result
bool xccdf_result_add_setvalue(struct xccdf_result *item, struct xccdf_setvalue *newval);
/// @memberof xccdf_result
bool xccdf_result_add_target_fact(struct xccdf_result *item, struct xccdf_target_fact *newval);
/// @memberof xccdf_result
bool xccdf_result_add_target_identifier(struct xccdf_result *item, struct xccdf_target_identifier *newval);
/// @memberof xccdf_result
bool xccdf_result_add_applicable_platform(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_add_remark(struct xccdf_result *item, struct oscap_text *newval);
/// @memberof xccdf_result
bool xccdf_result_add_organization(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_add_target(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_add_identity(struct xccdf_result *item, struct xccdf_identity *newval);
/// @memberof xccdf_result
bool xccdf_result_add_score(struct xccdf_result *item, struct xccdf_score *newval);
/// @memberof xccdf_result
bool xccdf_result_add_title(struct xccdf_result *item, struct oscap_text *newval);
/// @memberof xccdf_result
bool xccdf_result_add_target_address(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_result
bool xccdf_result_add_applicable_platform(struct xccdf_result *item, const char *newval);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_add_ident(struct xccdf_rule_result *obj, struct xccdf_ident *item);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_add_fix(struct xccdf_rule_result *obj, struct xccdf_fix *item);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_add_check(struct xccdf_rule_result *obj, struct xccdf_check *item);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_add_override(struct xccdf_rule_result *obj, struct xccdf_override *item);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_add_message(struct xccdf_rule_result *obj, struct xccdf_message *item);
/// @memberof xccdf_rule_result
bool xccdf_rule_result_add_instance(struct xccdf_rule_result *obj, struct xccdf_instance *item);
/// @memberof xccdf_item
bool xccdf_item_add_description(struct xccdf_item *item, struct oscap_text *newval);
/// @memberof xccdf_item
bool xccdf_item_add_platform(struct xccdf_item *item, const char *newval);
/// @memberof xccdf_item
bool xccdf_item_add_question(struct xccdf_item *item, struct oscap_text *newval);
/// @memberof xccdf_item
bool xccdf_item_add_rationale(struct xccdf_item *item, struct oscap_text *newval);
/// @memberof xccdf_item
bool xccdf_item_add_reference(struct xccdf_item *item, struct oscap_reference *newval);
/// @memberof xccdf_item
bool xccdf_item_add_dc_status(struct xccdf_item *item, struct oscap_reference *newval);
/// @memberof xccdf_item
bool xccdf_item_add_status(struct xccdf_item *item, struct xccdf_status *newval);
/// @memberof xccdf_item
bool xccdf_item_add_title(struct xccdf_item *item, struct oscap_text *newval);
/// @memberof xccdf_item
bool xccdf_item_add_warning(struct xccdf_item *item, struct xccdf_warning *newval);
/// @memberof xccdf_refine_rule
bool xccdf_refine_rule_add_remark(struct xccdf_refine_rule *obj, struct oscap_text *item);

/// @memberof xccdf_rule
bool xccdf_rule_add_requires(struct xccdf_rule *rule, struct oscap_stringlist *requires);
/// @memberof xccdf_group
bool xccdf_group_add_requires(struct xccdf_group *group, struct oscap_stringlist *requires);
/// @memberof xccdf_item
bool xccdf_item_add_requires(struct xccdf_item *item, struct oscap_stringlist *requires);
/// @memberof xccdf_rule
bool xccdf_rule_add_conflicts(struct xccdf_rule *rule, const char *conflicts);
/// @memberof xccdf_group
bool xccdf_group_add_conflicts(struct xccdf_group *group, const char *conflicts);
/// @memberof xccdf_item
bool xccdf_item_add_conflicts(struct xccdf_item *item, const char *conflicts);

/************************************************************
 ** @} End of Setters group */

// remove operations

/// @memberof xccdf_notice_iterator
void xccdf_notice_iterator_remove(struct xccdf_notice_iterator *it);
/// @memberof xccdf_model_iterator
void xccdf_model_iterator_remove(struct xccdf_model_iterator *it);
/// @memberof xccdf_profile_iterator
void xccdf_profile_iterator_remove(struct xccdf_profile_iterator *it);
/// @memberof xccdf_item_iterator
void xccdf_item_iterator_remove(struct xccdf_item_iterator *it);
/// @memberof xccdf_status_iterator
void xccdf_status_iterator_remove(struct xccdf_status_iterator *it);
/// @memberof xccdf_profile_note_iterator
void xccdf_profile_note_iterator_remove(struct xccdf_profile_note_iterator *it);
/// @memberof xccdf_refine_value_iterator
void xccdf_refine_value_iterator_remove(struct xccdf_refine_value_iterator *it);
/// @memberof xccdf_refine_rule_iterator
void xccdf_refine_rule_iterator_remove(struct xccdf_refine_rule_iterator *it);
/// @memberof xccdf_setvalue_iterator
void xccdf_setvalue_iterator_remove(struct xccdf_setvalue_iterator *it);
/// @memberof xccdf_select_iterator
void xccdf_select_iterator_remove(struct xccdf_select_iterator *it);
/// @memberof xccdf_ident_iterator
void xccdf_ident_iterator_remove(struct xccdf_ident_iterator *it);
/// @memberof xccdf_check_content_ref_iterator
void xccdf_check_content_ref_iterator_remove(struct xccdf_check_content_ref_iterator *it);
/// @memberof xccdf_check_export_iterator
void xccdf_check_export_iterator_remove(struct xccdf_check_export_iterator *it);
/// @memberof xccdf_check_import_iterator
void xccdf_check_import_iterator_remove(struct xccdf_check_import_iterator *it);
/// @memberof xccdf_check_iterator
void xccdf_check_iterator_remove(struct xccdf_check_iterator *it);
/// @memberof xccdf_fixtext_iterator
void xccdf_fixtext_iterator_remove(struct xccdf_fixtext_iterator *it);
/// @memberof xccdf_fix_iterator
void xccdf_fix_iterator_remove(struct xccdf_fix_iterator *it);
/// @memberof xccdf_value_iterator
void xccdf_value_iterator_remove(struct xccdf_value_iterator *it);
/// @memberof xccdf_plain_text_iterator
void xccdf_plain_text_iterator_remove(struct xccdf_plain_text_iterator *it);
/// @memberof xccdf_warning_iterator
void xccdf_warning_iterator_remove(struct xccdf_warning_iterator *it);
/// @memberof xccdf_result_iterator
void xccdf_result_iterator_remove(struct xccdf_result_iterator *it);
/// @memberof xccdf_override_iterator
void xccdf_override_iterator_remove(struct xccdf_override_iterator *it);
/// @memberof xccdf_message_iterator
void xccdf_message_iterator_remove(struct xccdf_message_iterator *it);
/// @memberof xccdf_instance_iterator
void xccdf_instance_iterator_remove(struct xccdf_instance_iterator *it);
/// @memberof xccdf_rule_result_iterator
void xccdf_rule_result_iterator_remove(struct xccdf_rule_result_iterator *it);
/// @memberof xccdf_identity_iterator
void xccdf_identity_iterator_remove(struct xccdf_identity_iterator *it);
/// @memberof xccdf_score_iterator
void xccdf_score_iterator_remove(struct xccdf_score_iterator *it);
/// @memberof xccdf_target_fact_iterator
void xccdf_target_fact_iterator_remove(struct xccdf_target_fact_iterator *it);
/// @memberof xccdf_target_identifier_iterator
void xccdf_target_identifier_iterator_remove(struct xccdf_target_identifier_iterator *it);
/// @memberof xccdf_value_instance_iterator
void xccdf_value_instance_iterator_remove(struct xccdf_value_instance_iterator *it);


// textual substitution interface

/**
 * Type of textual substitution
 * @deprecated This type is deprecated. It is not flexible enough to be applied on XCCDF 1.2+
 * documents. Please use @ref xml_iterate module instead.
 */
typedef enum xccdf_subst_type {
    XCCDF_SUBST_NONE,
    XCCDF_SUBST_SUB,       ///< substitute cdf:sub element
    XCCDF_SUBST_LINK,      ///< substitute a hyperlink
    XCCDF_SUBST_INSTANCE   ///< substitute cdf:instance element
} xccdf_subst_type_t;

/**
 * Textual substitution callback.
 * This function is supposed to return textual representation of the substitution with given ID.
 * @param type Type of the substitution
 * @param id   ID of the object to be substituted
 * @param arg  arbitrary user-defined argument
 * @return     newly allocated substitution string (will be free'd)
 * @deprecated This callback has been deprecated. It cannot be applied on XCCDF 1.2+ documents
 * given the xccdf:sub/@use parameter.
 */
typedef char*(*xccdf_substitution_func)(xccdf_subst_type_t type, const char *id, void *arg);


/**
 * Perform a textual substitution.
 * Calls @a cb on each substitution (e.g. a cdf:sub element) of @a text and returns a string with substitutions done.
 * @param text source text
 * @param cb substitution callback
 * @param arg arbitrary argument to be passed to he callback
 * @return substituted string (it is up to the caller to free it)
 * @deprecated This function has been deprecated. It cannot be applied on XCCDF 1.2+ documents
 * given the xccdf:sub/@use parameter.
 */
OSCAP_DEPRECATED(char* oscap_text_xccdf_substitute(const char *text, xccdf_substitution_func cb, void *arg));

/************************************************************/
/** @} End of XCCDF group */
/// @}

#endif
