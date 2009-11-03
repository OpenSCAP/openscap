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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

/**
 * @addtogroup OVAL
 * @{
 * @addtogroup Definitions
 * @{
 * OVAL Definitions interface.
 *
 * This interface provides means to load and extract information from an OVAL Definitions file.
 *
 * Each oval_definition instance holds information on single OVAL definition
 * while the whole definitions infrastructure is held by an oval_definition_model instance.
 *
 * Main components of an OVAL Definitions file and their OpenSCAP API counterparts are as follows:
 * - oval_definition_model - whole definitions document
 * - oval_definition - definitions
 * - oval_test       - tests
 * - oval_object     - objects
 * - oval_state      - states
 * - oval_variable   - variables
 *
 * @file
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
 */


#ifndef OVAL_DEFINITIONS
#define OVAL_DEFINITIONS

#include <stdbool.h>

/// OVAL family
typedef enum {
	/**
	 * Unknown platform
	 * @see oval_subtype_t
	 */
	OVAL_FAMILY_UNKNOWN = 0,
	/**
	 * AIX family
	 * @see oval_aix_subtype_t
	 */
	OVAL_FAMILY_AIX = 1000,
	/**
	 * Apache family
	 * @see oval_apache_subtype_t
	 */
	OVAL_FAMILY_APACHE = 2000,
	/**
	 * CatOS family
	 * @see oval_catos_subtype_t
	 */
	OVAL_FAMILY_CATOS = 3000,
	/**
	 * ESX family
	 * @see oval_esx_subtype_t
	 */
	OVAL_FAMILY_ESX = 4000,
	/**
	 * FreeBSD family
	 * @see oval_freebsd_subtype_t
	 */
	OVAL_FAMILY_FREEBSD = 5000,
	/**
	 * HPUX family
	 * @see oval_hpux_subtype_t
	 */
	OVAL_FAMILY_HPUX = 6000,
	/**
	 * Platform independent family
	 * @see oval_independent_subtype_t
	 */
	OVAL_FAMILY_INDEPENDENT = 7000,
	/**
	 * IOS family
	 * @see oval_ios_subtype_t
	 */
	OVAL_FAMILY_IOS = 8000,
	/**
	 * Linux family
	 * @see oval_linux_subtype_t
	 */
	OVAL_FAMILY_LINUX = 9000,
	/**
	 * MacOS family
	 * @see oval_macos_subtype_t
	 */
	OVAL_FAMILY_MACOS = 10000,
	/**
	 * PIXOS family
	 * @see oval_pixos_subtype_t
	 */
	OVAL_FAMILY_PIXOS = 11000,
	/**
	 * Solaris family
	 * @see oval_solaris_subtype_t
	 */
	OVAL_FAMILY_SOLARIS = 12000,
	/**
	 * Unix family
	 * @see oval_unix_subtype_t
	 */
	OVAL_FAMILY_UNIX = 13000,
	/**
	 * Windows family
	 * @see oval_windows_subtype_t
	 */
	OVAL_FAMILY_WINDOWS = 14000
} oval_family_t;

/// Unknown subtypes
typedef enum {
	OVAL_SUBTYPE_UNKNOWN = 0
} oval_subtype_t;

/// AIX subtypes
typedef enum {
	OVAL_AIX_FILESET = 1001,
	OVAL_AIX_FIX = OVAL_FAMILY_AIX + 2,
	OVAL_AIX_OSLEVEL = OVAL_FAMILY_AIX + 3
} oval_aix_subtype_t;

/// Apache subtypes
typedef enum {
	OVAL_APACHE_HTTPD = OVAL_FAMILY_APACHE + 1,
	OVAL_APACHE_VERSION = OVAL_FAMILY_APACHE + 2
} oval_apache_subtype_t;

/// CatOS subtypes
typedef enum {
	OVAL_CATOS_LINE = OVAL_FAMILY_CATOS + 1,
	OVAL_CATOS_MODULE = OVAL_FAMILY_CATOS + 2,
	OVAL_CATOS_VERSION_55 = OVAL_FAMILY_CATOS + 3,
	OVAL_CATOS_VERSION = OVAL_FAMILY_CATOS + 4
} oval_catos_subtype_t;

/// ESX subtypes
typedef enum {
	OVAL_ESX_PATCH = OVAL_FAMILY_ESX + 1,
	OVAL_ESX_VERSION = OVAL_FAMILY_ESX + 2
} oval_esx_subtype_t;

/// FreeBSD subtypes
typedef enum {
	OVAL_FREEBSD_PORT_INFO = OVAL_FAMILY_FREEBSD + 1
} oval_freebsd_subtype_t;

/// HPUX subtypes
typedef enum {
	OVAL_HPUX_GETCONF = OVAL_FAMILY_HPUX + 1,
	OVAL_HPUX_PATCH_53 = OVAL_FAMILY_HPUX + 2,
	OVAL_HPUX_PATCH = OVAL_FAMILY_HPUX + 3,
	OVAL_HPUX_SOFTWARE_LIST = OVAL_FAMILY_HPUX + 4,
	OVAL_HPUX_TRUSTED_ACCOUNTS = OVAL_FAMILY_HPUX + 5
} oval_hpux_subtype_t;

/// Platform independent subtypes
typedef enum {
	OVAL_INDEPENDENT_FAMILY = OVAL_FAMILY_INDEPENDENT + 1,
	OVAL_INDEPENDENT_FILE_MD5 = OVAL_FAMILY_INDEPENDENT + 2,
	OVAL_INDEPENDENT_FILE_HASH = OVAL_FAMILY_INDEPENDENT + 3,
	OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE = OVAL_FAMILY_INDEPENDENT + 4,
	OVAL_INDEPENDENT_SQL = OVAL_FAMILY_INDEPENDENT + 5,
	OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54 = OVAL_FAMILY_INDEPENDENT + 6,
	OVAL_INDEPENDENT_TEXT_FILE_CONTENT = OVAL_FAMILY_INDEPENDENT + 7,
	OVAL_INDEPENDENT_UNKNOWN = OVAL_FAMILY_INDEPENDENT + 8,
	OVAL_INDEPENDENT_VARIABLE = OVAL_FAMILY_INDEPENDENT + 9,
	OVAL_INDEPENDENT_XML_FILE_CONTENT = OVAL_FAMILY_INDEPENDENT + 10
} oval_independent_subtype_t;

/// IOS subtypes
typedef enum {
	OVAL_IOS_GLOBAL = OVAL_FAMILY_IOS + 1,
	OVAL_IOS_INTERFACE = OVAL_FAMILY_IOS + 2,
	OVAL_IOS_LINE = OVAL_FAMILY_IOS + 3,
	OVAL_IOS_SNMP = OVAL_FAMILY_IOS + 4,
	OVAL_IOS_TCLSH = OVAL_FAMILY_IOS + 5,
	OVAL_IOS_VERSION_55 = OVAL_FAMILY_IOS + 6
} oval_ios_subtype_t;

/// Linux subtypes
typedef enum {
	OVAL_LINUX_DPKG_INFO = OVAL_FAMILY_LINUX + 1,
	OVAL_LINUX_INET_LISTENING_SERVERS = OVAL_FAMILY_LINUX + 2,
	OVAL_LINUX_RPM_INFO = OVAL_FAMILY_LINUX + 3,
	OVAL_LINUX_SLACKWARE_PKG_INFO_TEST = OVAL_FAMILY_LINUX + 4
} oval_linux_subtype_t;

/// MacOS subtypes
typedef enum {
	OVAL_MACOS_ACCOUNT_INFO = OVAL_FAMILY_MACOS + 1,
	OVAL_MACOS_INET_LISTENING_SERVERS = OVAL_FAMILY_MACOS + 2,
	OVAL_MACOS_NVRAM_INFO = OVAL_FAMILY_MACOS + 3,
	OVAL_MACOS_PWPOLICY = OVAL_FAMILY_MACOS + 4
} oval_macos_subtype_t;

/// PIXOS subtypes
typedef enum {
	OVAL_PIXOS_LINE = OVAL_FAMILY_PIXOS + 1,
	OVAL_PIXOS_VERSION = OVAL_FAMILY_PIXOS + 2
} oval_pixos_subtype_t;

/// Solaris subtypes
typedef enum {
	OVAL_SOLARIS_ISAINFO = OVAL_FAMILY_SOLARIS + 1,
	OVAL_SOLARIS_PACKAGE = OVAL_FAMILY_SOLARIS + 2,
	OVAL_SOLARIS_PATCH = OVAL_FAMILY_SOLARIS + 3,
	OVAL_SOLARIS_SMF = OVAL_FAMILY_SOLARIS + 4
} oval_solaris_subtype_t;

/// Unix subtypes
typedef enum {
	OVAL_UNIX_FILE = OVAL_FAMILY_UNIX + 1,
	OVAL_UNIX_INETD = OVAL_FAMILY_UNIX + 2,
	OVAL_UNIX_INTERFACE = OVAL_FAMILY_UNIX + 3,
	OVAL_UNIX_PASSWORD = OVAL_FAMILY_UNIX + 4,
	OVAL_UNIX_PROCESS = OVAL_FAMILY_UNIX + 5,
	OVAL_UNIX_RUNLEVEL = OVAL_FAMILY_UNIX + 6,
	OVAL_UNIX_SCCS = OVAL_FAMILY_UNIX + 7,
	OVAL_UNIX_SHADOW = OVAL_FAMILY_UNIX + 8,
	OVAL_UNIX_UNAME = OVAL_FAMILY_UNIX + 9,
	OVAL_UNIX_XINETD = OVAL_FAMILY_UNIX + 10
} oval_unix_subtype_t;

/// Windows subtypes
typedef enum {
	OVAL_WINDOWS_ACCESS_TOKEN = OVAL_FAMILY_WINDOWS                  +  1,
	OVAL_WINDOWS_ACTIVE_DIRECTORY = OVAL_FAMILY_WINDOWS              +  2,
	OVAL_WINDOWS_AUDIT_EVENT_POLICY = OVAL_FAMILY_WINDOWS            +  3,
	OVAL_WINDOWS_AUDIT_EVENT_SUBCATEGORIES = OVAL_FAMILY_WINDOWS     +  4,
	OVAL_WINDOWS_FILE = OVAL_FAMILY_WINDOWS                          +  5,
	OVAL_WINDOWS_FILE_AUDITED_PERMISSIONS_53 = OVAL_FAMILY_WINDOWS   +  6,
	OVAL_WINDOWS_FILE_AUDITED_PERMISSIONS = OVAL_FAMILY_WINDOWS      +  7,
	OVAL_WINDOWS_FILE_EFFECTIVE_RIGHTS_53 = OVAL_FAMILY_WINDOWS      +  8,
	OVAL_WINDOWS_FILE_EFFECTIVE_RIGHTS = OVAL_FAMILY_WINDOWS         +  9,
	OVAL_WINDOWS_GROUP = OVAL_FAMILY_WINDOWS                         + 10,
	OVAL_WINDOWS_GROUP_SID = OVAL_FAMILY_WINDOWS                     + 11,
	OVAL_WINDOWS_INTERFACE = OVAL_FAMILY_WINDOWS                     + 12,
	OVAL_WINDOWS_LOCKOUT_POLICY = OVAL_FAMILY_WINDOWS                + 13,
	OVAL_WINDOWS_METABASE = OVAL_FAMILY_WINDOWS                      + 14,
	OVAL_WINDOWS_PASSWORD_POLICY = OVAL_FAMILY_WINDOWS               + 15,
	OVAL_WINDOWS_PORT = OVAL_FAMILY_WINDOWS                          + 16,
	OVAL_WINDOWS_PRINTER_EFFECTIVE_RIGHTS = OVAL_FAMILY_WINDOWS      + 17,
	OVAL_WINDOWS_PROCESS = OVAL_FAMILY_WINDOWS                       + 18,
	OVAL_WINDOWS_REGISTRY = OVAL_FAMILY_WINDOWS                      + 19,
	OVAL_WINDOWS_REGKEY_AUDITED_PERMISSIONS_53 = OVAL_FAMILY_WINDOWS + 20,
	OVAL_WINDOWS_REGKEY_AUDITED_PERMISSIONS = OVAL_FAMILY_WINDOWS    + 21,
	OVAL_WINDOWS_REGKEY_EFFECTIVE_RIGHTS_53 = OVAL_FAMILY_WINDOWS    + 22,
	OVAL_WINDOWS_REGKEY_EFFECTIVE_RIGHTS = OVAL_FAMILY_WINDOWS       + 23,
	OVAL_WINDOWS_SHARED_RESOURCE = OVAL_FAMILY_WINDOWS               + 24,
	OVAL_WINDOWS_SID = OVAL_FAMILY_WINDOWS                           + 25,
	OVAL_WINDOWS_SID_SID = OVAL_FAMILY_WINDOWS                       + 26,
	OVAL_WINDOWS_USER_ACCESS_CONTROL = OVAL_FAMILY_WINDOWS           + 27,
	OVAL_WINDOWS_USER = OVAL_FAMILY_WINDOWS                          + 28,
	OVAL_WINDOWS_USER_SID_55 = OVAL_FAMILY_WINDOWS                   + 29,
	OVAL_WINDOWS_USER_SID = OVAL_FAMILY_WINDOWS                      + 30,
	OVAL_WINDOWS_VOLUME = OVAL_FAMILY_WINDOWS                        + 31,
	OVAL_WINDOWS_WMI = OVAL_FAMILY_WINDOWS                           + 32,
	OVAL_WINDOWS_WUA_UPDATE_SEARCHER = OVAL_FAMILY_WINDOWS           + 33
} oval_windows_subtype_t;

/// Affected family enumeration
typedef enum {
	OVAL_AFCFML_UNKNOWN  = 0,
	OVAL_AFCFML_CATOS    = 1,
	OVAL_AFCFML_IOS      = 2,
	OVAL_AFCFML_MACOS    = 3,
	OVAL_AFCFML_PIXOS    = 4,
	OVAL_AFCFML_UNDEFINED= 5,
	OVAL_AFCFML_UNIX     = 6,
	OVAL_AFCFML_WINDOWS  = 7
} oval_affected_family_t;

/// Criteria node type
typedef enum {
	OVAL_NODETYPE_UNKNOWN = 0,
	OVAL_NODETYPE_CRITERIA = 1,
	OVAL_NODETYPE_CRITERION = 2,
	OVAL_NODETYPE_EXTENDDEF = 3
} oval_criteria_node_type_t;

/// Boolean operators
typedef enum {
	OVAL_OPERATOR_UNKNOWN = 0,
	OVAL_OPERATOR_AND     = 1,
	OVAL_OPERATOR_ONE     = 2,
	OVAL_OPERATOR_OR      = 3,
	OVAL_OPERATOR_XOR     = 4
} oval_operator_t;

/// Operations
typedef enum {
	OVAL_OPERATION_UNKNOWN                    =  0,
	OVAL_OPERATION_EQUALS                     =  1,
	OVAL_OPERATION_NOT_EQUAL                  =  2,
	OVAL_OPERATION_CASE_INSENSITIVE_EQUALS    =  3,
	OVAL_OPERATION_CASE_INSENSITIVE_NOT_EQUAL =  4,
	OVAL_OPERATION_GREATER_THAN               =  5,
	OVAL_OPERATION_LESS_THAN                  =  6,
	OVAL_OPERATION_GREATER_THAN_OR_EQUAL      =  7,
	OVAL_OPERATION_LESS_THAN_OR_EQUAL         =  8,
	OVAL_OPERATION_BITWISE_AND                =  9,
	OVAL_OPERATION_BITWISE_OR                 = 10,
	OVAL_OPERATION_PATTERN_MATCH              = 11
} oval_operation_t;

/// Definition classes
typedef enum {
	OVAL_CLASS_UNKNOWN      = 0,
	OVAL_CLASS_COMPLIANCE   = 1,
	OVAL_CLASS_INVENTORY    = 2,
	OVAL_CLASS_MISCELLANEOUS= 3,
	OVAL_CLASS_PATCH        = 4,
	OVAL_CLASS_VULNERABILITY= 5,
} oval_definition_class_t;

/// Existence check enumeration
typedef enum {
	OVAL_EXISTENCE_UNKNOWN  = 0,
	OVAL_ALL_EXIST          = 1,
	OVAL_ANY_EXIST          = 2,
	OVAL_AT_LEAST_ONE_EXISTS= 3,
	OVAL_ONLY_ONE_EXISTS    = 4,
	OVAL_NONE_EXIST         = 5
} oval_existence_t;

/// Check enumeration
typedef enum {
	OVAL_CHECK_UNKNOWN     = 0,
	OVAL_CHECK_ALL         = 1,
	OVAL_CHECK_AT_LEAST_ONE= 2,
	OVAL_CHECK_NONE_EXIST  = 3,
	OVAL_CHECK_NONE_SATISFY= 4,
	OVAL_CHECK_ONLY_ONE    = 5
} oval_check_t;

/// Type of object content
typedef enum {
	OVAL_OBJECTCONTENT_UNKNOWN = 0,
	OVAL_OBJECTCONTENT_ENTITY  = 1,
	OVAL_OBJECTCONTENT_SET     = 2
} oval_object_content_type_t;

/// Type of OVAL entity
typedef enum {
	OVAL_ENTITY_TYPE_UNKNOWN,
	OVAL_ENTITY_TYPE_ANY,
	OVAL_ENTITY_TYPE_BINARY,
	OVAL_ENTITY_TYPE_BOOLEAN,
	OVAL_ENTITY_TYPE_FLOAT,
	OVAL_ENTITY_TYPE_INTEGER,
	OVAL_ENTITY_TYPE_STRING,
} oval_entity_type_t;

/// Datatypes
typedef enum {
	OVAL_DATATYPE_UNKNOWN           = 0,
	OVAL_DATATYPE_BINARY            = 1,
	OVAL_DATATYPE_BOOLEAN           = 2,
	OVAL_DATATYPE_EVR_STRING        = 3,
	OVAL_DATATYPE_FILESET_REVISION  = 4,
	OVAL_DATATYPE_FLOAT             = 5,
	OVAL_DATATYPE_IOS_VERSION       = 6,
	OVAL_DATATYPE_VERSION           = 7,
	OVAL_DATATYPE_INTEGER           = 8,
	OVAL_DATATYPE_STRING            = 9
} oval_datatype_t;

/// Varref types
typedef enum {
	OVAL_ENTITY_VARREF_UNKNOWN,
	OVAL_ENTITY_VARREF_NONE,
	OVAL_ENTITY_VARREF_ATTRIBUTE,
	OVAL_ENTITY_VARREF_ELEMENT
} oval_entity_varref_type_t;

/// OVAL set object types
typedef enum {
	OVAL_SET_UNKNOWN,
	OVAL_SET_AGGREGATE,
	OVAL_SET_COLLECTIVE
} oval_setobject_type_t;

/// Set operations
typedef enum {
	OVAL_SET_OPERATION_UNKNOWN,
	OVAL_SET_OPERATION_COMPLEMENT   = 1,
	OVAL_SET_OPERATION_INTERSECTION = 2,
	OVAL_SET_OPERATION_UNION        = 3
} oval_setobject_operation_t;

/// OVAL variable types
typedef enum {
	OVAL_VARIABLE_UNKNOWN,
	OVAL_VARIABLE_EXTERNAL,
	OVAL_VARIABLE_CONSTANT,
	OVAL_VARIABLE_LOCAL
} oval_variable_type_t;

#define OVAL_FUNCTION 10
/// Component types
typedef enum {
	OVAL_COMPONENT_UNKNOWN = 0,
	OVAL_COMPONENT_LITERAL = 1,
	OVAL_COMPONENT_OBJECTREF = 2,
	OVAL_COMPONENT_VARREF = 3,
	OVAL_COMPONENT_FUNCTION = 4,
	OVAL_FUNCTION_BEGIN = OVAL_FUNCTION + 1,
	OVAL_FUNCTION_CONCAT = OVAL_FUNCTION + 2,
	OVAL_FUNCTION_END = OVAL_FUNCTION + 3,
	OVAL_FUNCTION_SPLIT = OVAL_FUNCTION + 4,
	OVAL_FUNCTION_SUBSTRING = OVAL_FUNCTION + 5,
	OVAL_FUNCTION_TIMEDIF = OVAL_FUNCTION + 6,
	OVAL_FUNCTION_ESCAPE_REGEX = OVAL_FUNCTION + 7,
	OVAL_FUNCTION_REGEX_CAPTURE = OVAL_FUNCTION + 8,
	OVAL_FUNCTION_ARITHMETIC = OVAL_FUNCTION + 9
} oval_component_type_t;

/// Arithmetic format enumeration
typedef enum {
	OVAL_ARITHMETIC_UNKNOWN = 0,
	OVAL_ARITHMETIC_ADD     = 1,
	OVAL_ARITHMETIC_MULTIPLY =2,
	OVAL_ARITHMETIC_SUBTRACT =3,	//NOT YET SUPPORTED BY OVAL
	OVAL_ARITHMETIC_DIVIDE   =4	//NOT YET SUPPORTED BY OVAL
} oval_arithmetic_operation_t;

/// Datetime format enumeration.
typedef enum {
	OVAL_DATETIME_UNKNOWN             = 0,
	OVAL_DATETIME_YEAR_MONTH_DAY      = 1,
	OVAL_DATETIME_MONTH_DAY_YEAR      = 2,
	OVAL_DATETIME_DAY_MONTH_YEAR      = 3,
	OVAL_DATETIME_WIN_FILETIME        = 4,
	OVAL_DATETIME_SECONDS_SINCE_EPOCH = 5
} oval_datetime_format_t;

/// Message level.
typedef enum {
	OVAL_MESSAGE_LEVEL_NONE    = 0,
	OVAL_MESSAGE_LEVEL_DEBUG   = 1,
	OVAL_MESSAGE_LEVEL_INFO    = 2,
	OVAL_MESSAGE_LEVEL_WARNING = 3,
	OVAL_MESSAGE_LEVEL_ERROR   = 4,
	OVAL_MESSAGE_LEVEL_FATAL   = 5
} oval_message_level_t;

struct oval_string_iterator;


/**
 * @struct oval_affected
 * OVAL affected description.
 */
struct oval_affected;
struct oval_affected_iterator;

/**
 * @struct oval_test
 * OVAL test.
 */
struct oval_test;
struct oval_test_iterator;

/**
 * @struct oval_criteria_node
 * OVAL criteria node.
 */
struct oval_criteria_node;
struct oval_criteria_node_iterator;

/**
 * @struct oval_reference
 * OVAL reference
 */
struct oval_reference;
struct oval_reference_iterator;

/**
 * @struct oval_definition
 * OVAL definition
 */
struct oval_definition;
struct oval_definition_iterator;

/**
 * @struct oval_object
 * OVAL object.
 */
struct oval_object;
struct oval_object_iterator;

/**
 * @struct oval_state
 * OVAL state.
 */
struct oval_state;
struct oval_state_iterator;

/**
 * @struct oval_variable
 * OVAL variable.
 */
struct oval_variable;
struct oval_variable_iterator;

/**
 * @struct oval_variable_binding
 * Binding of an value to an OVAL variable.
 */
struct oval_variable_binding;
struct oval_variable_binding_iterator;

/**
 * @struct oval_object_content
 * OVAL object content.
 */
struct oval_object_content;
struct oval_object_content_iterator;

/**
 * @struct oval_state_content
 * OVAL state content.
 */
struct oval_state_content;
struct oval_state_content_iterator;

/**
 * @struct oval_behavior
 * OVAL behavior
 */
struct oval_behavior;
struct oval_behavior_iterator;

/**
 * @struct oval_entity
 * OVAL entity
 */
struct oval_entity;
struct oval_entity_iterator;

/**
 * @struct oval_set
 * OVAL set object
 */
struct oval_setobject;
struct oval_setobject_iterator;

/**
 * @struct oval_value
 * OVAL object or item entity value
 */
struct oval_value;
struct oval_value_iterator;

/**
 * @struct oval_component
 * OVAL variable component
 */
struct oval_component;
struct oval_component_iterator;

/**
 * @struct oval_message
 * OVAL message
 */
struct oval_message;
struct oval_message_iterator;

int   oval_string_iterator_has_more(struct oval_string_iterator *);
char *oval_string_iterator_next    (struct oval_string_iterator *);
void  oval_string_iterator_free    (struct oval_string_iterator *);



struct oval_affected *oval_affected_new();
void oval_affected_free(struct oval_affected *);

void oval_affected_set_family(struct oval_affected *, oval_affected_family_t);
void oval_affected_add_platform(struct oval_affected *, char *);
void oval_affected_add_product(struct oval_affected *, char *);

int                   oval_affected_iterator_has_more(struct oval_affected_iterator *);
struct oval_affected *oval_affected_iterator_next    (struct oval_affected_iterator *);
void                  oval_affected_iterator_free    (struct oval_affected_iterator *);

/**
 * Get family from an affected platform description.
 * @relates oval_affected
 */
oval_affected_family_t       oval_affected_get_family  (struct oval_affected *);

/**
 * Get platforms from an affected platform description.
 * @relates oval_affected
 */
struct oval_string_iterator *oval_affected_get_platforms(struct oval_affected *);

/**
 * Get product names from an affected platform description.
 * @relates oval_affected
 */
struct oval_string_iterator *oval_affected_get_products (struct oval_affected *);

struct oval_criteria_node *oval_criteria_node_new(oval_criteria_node_type_t);
void oval_criteria_node_free(struct oval_criteria_node *);

void oval_criteria_set_node_type(struct oval_criteria_node *, oval_criteria_node_type_t);
void oval_criteria_node_set_negate(struct oval_criteria_node *, int);
void oval_criteria_node_set_comment(struct oval_criteria_node *, char *);
void oval_criteria_node_set_operator(struct oval_criteria_node *, oval_operator_t);	//type==NODETYPE_CRITERIA
void oval_criteria_node_add_subnode(struct oval_criteria_node *, struct oval_criteria_node *);	//type==NODETYPE_CRITERIA
void oval_criteria_node_set_test(struct oval_criteria_node *, struct oval_test *);	//type==NODETYPE_CRITERION
void oval_criteria_node_set_definition(struct oval_criteria_node *, struct oval_definition *);	//type==NODETYPE_EXTENDDEF

int    oval_criteria_node_iterator_has_more                (struct oval_criteria_node_iterator *);
struct oval_criteria_node *oval_criteria_node_iterator_next(struct oval_criteria_node_iterator *);
void                       oval_criteria_node_iterator_free(struct oval_criteria_node_iterator *);

/**
 * Get OVAL criteria node type.
 * @relates oval_criteria_node
 */
oval_criteria_node_type_t           oval_criteria_node_get_type      (struct oval_criteria_node *);

/**
 * Get OVAL criteria node negation.
 * @relates oval_criteria_node
 */
int                                 oval_criteria_node_get_negate    (struct oval_criteria_node *);

/**
 * Get OVAL criteria node comment.
 * @relates oval_criteria_node
 */
char                               *oval_criteria_node_get_comment   (struct oval_criteria_node *);

/**
 * Get OVAL criteria node operator.
 * @relates oval_criteria_node
 */
oval_operator_t                     oval_criteria_node_get_operator  (struct oval_criteria_node *);//type==NODETYPE_CRITERIA

/**
 * Get OVAL criteria node subnodes.
 * @relates oval_criteria_node
 */
struct oval_criteria_node_iterator *oval_criteria_node_get_subnodes  (struct oval_criteria_node *);//type==NODETYPE_CRITERIA

/**
 * Get OVAL criteria node test.
 * @relates oval_criteria_node
 */
struct oval_test                   *oval_criteria_node_get_test      (struct oval_criteria_node *);//type==NODETYPE_CRITERION

/**
 * Get OVAL criteria node definition.
 * @relates oval_criteria_node
 */
struct oval_definition             *oval_criteria_node_get_definition(struct oval_criteria_node *);//type==NODETYPE_EXTENDDEF

struct oval_reference *oval_reference_new();
void oval_reference_free(struct oval_reference *);

void oval_reference_set_source(struct oval_reference *, char *);
void oval_reference_set_id(struct oval_reference *, char *);
void oval_reference_set_url(struct oval_reference *, char *);

int                    oval_reference_iterator_has_more(struct oval_reference_iterator *);
struct oval_reference *oval_reference_iterator_next    (struct oval_reference_iterator *);
void                   oval_reference_iterator_free    (struct oval_reference_iterator *);

/**
 * Get OVAL reference source.
 * @relates oval_reference
 */
char *oval_reference_get_source(struct oval_reference *);

/**
 * Get OVAL reference ID.
 * @relates oval_reference
 */
char *oval_reference_get_id    (struct oval_reference *);

/**
 * Get OVAL reference URL.
 * @relates oval_reference
 */
char *oval_reference_get_url   (struct oval_reference *);

struct oval_definition *oval_definition_new(char *id);
void oval_definition_free(struct oval_definition *);

void oval_definition_set_version(struct oval_definition *, int);
void oval_definition_set_class(struct oval_definition *, oval_definition_class_t);
void oval_definition_set_deprecated(struct oval_definition *, int);
void oval_definition_set_title(struct oval_definition *, char *);
void oval_definition_set_description(struct oval_definition *, char *);
void oval_definition_set_criteria(struct oval_definition *, struct oval_criteria_node *);
void oval_definition_add_affected(struct oval_definition *, struct oval_affected *);
void oval_definition_add_reference(struct oval_definition *, struct oval_reference *);
void oval_definition_add_note(struct oval_definition *, char *);

int                     oval_definition_iterator_has_more(struct oval_definition_iterator *);
struct oval_definition *oval_definition_iterator_next    (struct oval_definition_iterator *);
void                    oval_definition_iterator_free    (struct oval_definition_iterator *);

/**
 * Get OVAL definition ID.
 * @relates oval_definition
 */
char                           *oval_definition_get_id(struct oval_definition *);

/**
 * Get OVAL definition version.
 * @relates oval_definition
 */
int                             oval_definition_get_version(struct oval_definition *);

/**
 * Get OVAL definition class.
 * @relates oval_definition
 */
oval_definition_class_t         oval_definition_get_class(struct oval_definition *);

/**
 * Get OVAL definition deprecation status.
 * @relates oval_definition
 */
int                             oval_definition_get_deprecated(struct oval_definition *);

/**
 * Get OVAL definition title.
 * @relates oval_definition
 */
char                           *oval_definition_get_title(struct oval_definition *);

/**
 * Get OVAL definition description.
 * @relates oval_definition
 */
char                           *oval_definition_get_description(struct oval_definition *);

/**
 * Get iterator to list of affected platforms.
 * @relates oval_definition
 */
struct oval_affected_iterator  *oval_definition_get_affected(struct oval_definition*);

/**
 * Get iterator to list of definition references.
 * @relates oval_definition
 */
struct oval_reference_iterator *oval_definition_get_references(struct oval_definition*);

/**
 * Get iterator to list of notes.
 * @relates oval_definition
 */
struct oval_string_iterator    *oval_definition_get_notes(struct oval_definition *);

/**
 * Get iterator to list of OVAL definition criteria.
 * @relates oval_definition
 */
struct oval_criteria_node      *oval_definition_get_criteria(struct oval_definition *);

struct oval_object *oval_object_new(char *id);
void oval_object_free(struct oval_object *);

void oval_object_set_subtype(struct oval_object *, oval_subtype_t);
void oval_object_add_note(struct oval_object *, char *);
void oval_object_set_comment(struct oval_object *, char *);
void oval_object_set_deprecated(struct oval_object *, int);
void oval_object_set_version(struct oval_object *, int);
void oval_object_add_object_content(struct oval_object *, struct oval_object_content *);
void oval_object_add_behavior(struct oval_object *, struct oval_behavior *);

int                 oval_object_iterator_has_more(struct oval_object_iterator *);
struct oval_object *oval_object_iterator_next    (struct oval_object_iterator *);
void                oval_object_iterator_free    (struct oval_object_iterator *);

/**
 * Get OVAL object family.
 * @relates oval_object
 */
oval_family_t                        oval_object_get_family        (struct oval_object *);

/**
 * Get OVAL object subtype.
 * @relates oval_object
 */
oval_subtype_t                       oval_object_get_subtype       (struct oval_object *);

/**
 * Get OVAL object name.
 * @relates oval_object
 */
const char                          *oval_object_get_name          (struct oval_object *);

/**
 * Get iterator to OVAL object notes.
 * @relates oval_object
 */
struct oval_string_iterator         *oval_object_get_notes         (struct oval_object *);

/**
 * Get OVAL object comment.
 * @relates oval_object
 */
char                                *oval_object_get_comment       (struct oval_object *);

/**
 * Get OVAL object ID.
 * @relates oval_object
 */
char                                *oval_object_get_id            (struct oval_object *);

/**
 * Get OVAL object depracation status.
 * @relates oval_object
 */
int                                  oval_object_get_deprecated    (struct oval_object *);

/**
 * Get OVAL object version.
 * @relates oval_object
 */
int                                  oval_object_get_version       (struct oval_object *);
struct oval_object_content_iterator *oval_object_get_object_contents(struct oval_object *);
struct oval_behavior_iterator       *oval_object_get_behaviors     (struct oval_object *);

struct oval_test *oval_test_new(char *);
void oval_test_free(struct oval_test *);

void oval_test_set_subtype(struct oval_test *, oval_subtype_t);
void oval_test_set_comment(struct oval_test *, char *);
void oval_test_set_deprecated(struct oval_test *, int);
void oval_test_set_version(struct oval_test *, int);
void oval_test_set_existence(struct oval_test *, oval_existence_t);
void oval_test_set_check(struct oval_test *, oval_check_t);
void oval_test_set_object(struct oval_test *, struct oval_object *);
void oval_test_set_state(struct oval_test *, struct oval_state *);
void oval_test_add_note(struct oval_test *, char *);

int               oval_test_iterator_has_more(struct oval_test_iterator *);
struct oval_test *oval_test_iterator_next    (struct oval_test_iterator *);
void              oval_test_iterator_free    (struct oval_test_iterator *);

/**
 * Get OVAL test family.
 * @relates oval_test
 */
oval_family_t                oval_test_get_family    (struct oval_test *);

/**
 * Get OVAL test subtype.
 * @relates oval_test
 */
oval_subtype_t               oval_test_get_subtype   (struct oval_test *);

/**
 * Get OVAL test notes.
 * @relates oval_test
 */
struct oval_string_iterator *oval_test_get_notes     (struct oval_test *);

/**
 * Get OVAL test comment.
 * @relates oval_test
 */
char                        *oval_test_get_comment   (struct oval_test *);

/**
 * Get OVAL test ID.
 * @relates oval_test
 */
char                        *oval_test_get_id        (struct oval_test *);

/**
 * Get OVAL test deprecation status.
 * @relates oval_test
 */
int                          oval_test_get_deprecated(struct oval_test *);

/**
 * Get OVAL test version.
 * @relates oval_test
 */
int                          oval_test_get_version   (struct oval_test *);

/**
 * Get OVAL test existence check type.
 * @relates oval_test
 */
oval_existence_t             oval_test_get_existence (struct oval_test *);

/**
 * Get OVAL test check type.
 * @relates oval_test
 */
oval_check_t                 oval_test_get_check     (struct oval_test *);

/**
 * Get OVAL test tested object.
 * @relates oval_test
 */
struct oval_object          *oval_test_get_object    (struct oval_test *);

/**
 * Get OVAL test state.
 * @relates oval_test
 */
struct oval_state           *oval_test_get_state     (struct oval_test *);

struct oval_variable_binding *oval_variable_binding_new(struct oval_variable *, char *);
void oval_variable_binding_free(struct oval_variable_binding *);

void oval_variable_binding_set_variable(struct oval_variable_binding *, struct oval_variable *);
void oval_variable_binding_set_value   (struct oval_variable_binding *, char *);

int                           oval_variable_binding_iterator_has_more(struct oval_variable_binding_iterator *);
struct oval_variable_binding *oval_variable_binding_iterator_next    (struct oval_variable_binding_iterator *);
void                          oval_variable_binding_iterator_free    (struct oval_variable_binding_iterator *);

/**
 * Get variable for this binding.
 * @relates oval_variable_binding
 */
struct oval_variable *oval_variable_binding_get_variable(struct oval_variable_binding *);

/**
 * Get value of this binding.
 * @relates oval_variable_binding
 */
char                 *oval_variable_binding_get_value   (struct oval_variable_binding *);

struct oval_object_content *oval_object_content_new(oval_object_content_type_t type);
void oval_object_content_free(struct oval_object_content *);

void oval_object_content_set_type(struct oval_object_content *, oval_object_content_type_t);
void oval_object_content_set_field_name(struct oval_object_content *, char *);
void oval_object_content_set_entity(struct oval_object_content *, struct oval_entity *);//type == OVAL_OBJECTCONTENT_ENTITY
void oval_object_content_set_varCheck(struct oval_object_content *, oval_check_t);	    //type == OVAL_OBJECTCONTENT_ENTITY
void oval_object_content_set_setobject(struct oval_object_content *, struct oval_setobject *);//type == OVAL_OBJECTCONTENT_SET

int                         oval_object_content_iterator_has_more(struct oval_object_content_iterator *);
struct oval_object_content *oval_object_content_iterator_next    (struct oval_object_content_iterator *);
void                        oval_object_content_iterator_free    (struct oval_object_content_iterator *);

/**
 * Get field name of a object content.
 * @relates oval_object_content
 */
char                          *oval_object_content_get_field_name(struct oval_object_content *);

/**
 * Get type of a object content (entity or set).
 * @relates oval_object_content
 */
oval_object_content_type_t     oval_object_content_get_type      (struct oval_object_content *);

/**
 * Get entity of a simple object content.
 * @relates oval_object_content
 */
struct oval_entity            *oval_object_content_get_entity    (struct oval_object_content *);//type == OVAL_OBJECTCONTENT_ENTITY

/**
 * Get varCheck of a simple object content.
 * @relates oval_object_content
 */
oval_check_t                   oval_object_content_get_varCheck  (struct oval_object_content *);//type == OVAL_OBJECTCONTENT_ENTITY

/**
 * Get varCheck of a set object content.
 * @relates oval_object_content
 */
struct oval_setobject     *oval_object_content_get_setobject (struct oval_object_content *);//type == OVAL_OBJECTCONTENT_SET

struct oval_state_content *oval_state_content_new();
void oval_state_content_free(struct oval_state_content *);

void oval_state_content_set_entity(struct oval_state_content *, struct oval_entity *);
void oval_state_content_set_varcheck(struct oval_state_content *, oval_check_t);
void oval_state_content_set_entcheck(struct oval_state_content *, oval_check_t);

struct oval_entity *oval_entity_new();
void oval_entity_free(struct oval_entity *);

void oval_entity_set_type(struct oval_entity *, oval_entity_type_t);
void oval_entity_set_datatype(struct oval_entity *, oval_datatype_t);
void oval_entity_set_mask(struct oval_entity *, int);
void oval_entity_set_varref_type(struct oval_entity *, oval_entity_varref_type_t);
void oval_entity_set_variable(struct oval_entity *, struct oval_variable *);
void oval_entity_set_value(struct oval_entity *, struct oval_value *);
void oval_entity_set_name(struct oval_entity *, char *);
void oval_entity_set_operation(struct oval_entity *, oval_operation_t);

int                        oval_state_content_iterator_has_more(struct oval_state_content_iterator *);
struct oval_state_content *oval_state_content_iterator_next    (struct oval_state_content_iterator *);
void                       oval_state_content_iterator_free    (struct oval_state_content_iterator *);

/**
 * Get entity of a state content.
 * @relates oval_object_content
 */
struct oval_entity *oval_state_content_get_entity   (struct oval_state_content *);

/**
 * Get multipe variable values processing of a state content.
 * @relates oval_object_content
 */
oval_check_t        oval_state_content_get_var_check(struct oval_state_content *);

/**
 * Get multipe entities processing of a state content.
 * @relates oval_object_content
 */
oval_check_t        oval_state_content_get_ent_check(struct oval_state_content *);

int                 oval_entity_iterator_has_more(struct oval_entity_iterator *);
struct oval_entity *oval_entity_iterator_next    (struct oval_entity_iterator *);
void                oval_entity_iterator_free    (struct oval_entity_iterator *);


/**
 * Get OVAL entity name.
 * @relates oval_entity
 */
char                         *oval_entity_get_name       (struct oval_entity *);

/**
 * Get OVAL entity type.
 * @relates oval_entity
 */
oval_entity_type_t            oval_entity_get_type       (struct oval_entity *);

/**
 * Get OVAL entity datatype.
 * @relates oval_entity
 */
oval_datatype_t               oval_entity_get_datatype   (struct oval_entity *);

/**
 * Get OVAL entity operation type.
 * @relates oval_entity
 */
oval_operation_t              oval_entity_get_operation  (struct oval_entity *);

/**
 * Get OVAL entity mask.
 * @relates oval_entity
 */
int                           oval_entity_get_mask       (struct oval_entity *);

/**
 * Get OVAL entity varref type.
 * @relates oval_entity
 */
oval_entity_varref_type_t     oval_entity_get_varref_type(struct oval_entity *);

/**
 * Get OVAL entity varref variable.
 * @relates oval_entity
 */
struct oval_variable         *oval_entity_get_variable   (struct oval_entity *);

/**
 * Get OVAL entity value.
 * @relates oval_entity
 */
struct oval_value            *oval_entity_get_value      (struct oval_entity *);

struct oval_setobject *oval_setobject_new();
void oval_setobject_free(struct oval_setobject *);

void oval_setobject_set_type(struct oval_setobject *, oval_setobject_type_t);
void oval_setobject_set_operation(struct oval_setobject *, oval_setobject_operation_t);
void oval_setobject_add_subset(struct oval_setobject *, struct oval_setobject *);//type==OVAL_SET_AGGREGATE;
void oval_setobject_add_object(struct oval_setobject *, struct oval_object *);	//type==OVAL_SET_COLLECTIVE;
void oval_setobject_add_filter(struct oval_setobject *, struct oval_state *);	//type==OVAL_SET_COLLECTIVE;

int                    oval_setobject_iterator_has_more(struct oval_setobject_iterator *);
struct oval_setobject *oval_setobject_iterator_next    (struct oval_setobject_iterator *);
void                   oval_setobject_iterator_free    (struct oval_setobject_iterator *);


/**
 * Get OVAL set object type.
 * @relates oval_set
 */
oval_setobject_type_t              oval_setobject_get_type     (struct oval_setobject *);

/**
 * Get OVAL set object operation type.
 * @relates oval_set
 */
oval_setobject_operation_t         oval_setobject_get_operation(struct oval_setobject *);

/**
 * Get OVAL set object subsets.
 * This works only with sets of OVAL_SET_AGGREGATE type.
 * @relates oval_set
 */
struct oval_setobject_iterator    *oval_setobject_get_subsets  (struct oval_setobject *);//type==OVAL_SET_AGGREGATE;


/**
 * Get OVAL set object referenced objects.
 * This works only with sets of OVAL_SET_COLLECTIVE type.
 * @relates oval_set
 */
struct oval_object_iterator *oval_setobject_get_objects  (struct oval_setobject *);//type==OVAL_SET_COLLECTIVE;

/**
 * Get OVAL set object filters.
 * This works only with sets of OVAL_SET_COLLECTIVE type.
 * @relates oval_set
 */
struct oval_state_iterator  *oval_setobject_get_filters  (struct oval_setobject *);//type==OVAL_SET_COLLECTIVE;

struct oval_behavior *oval_behavior_new();
void oval_behavior_free(struct oval_behavior *);

void oval_behavior_set_keyval(struct oval_behavior *behavior, const char* key, const char* value);

int                   oval_behavior_iterator_has_more(struct oval_behavior_iterator *);
struct oval_behavior *oval_behavior_iterator_next    (struct oval_behavior_iterator *);
void                  oval_behavior_iterator_free    (struct oval_behavior_iterator *);

/**
 * Get OVAL behavior name.
 * @relates oval_behavior
 */
char *oval_behavior_get_key  (struct oval_behavior *);

/**
 * Get OVAL behavior value.
 * @relates oval_behavior
 */
char *oval_behavior_get_value(struct oval_behavior *);

struct oval_value *oval_value_new();
void oval_value_free(struct oval_value *);

void oval_value_set_datatype(struct oval_value *, oval_datatype_t);
void oval_value_set_text(struct oval_value *, char *);
void oval_value_set_binary(struct oval_value *, unsigned char *);	//datatype==OVAL_DATATYPE_BINARY
void oval_value_set_boolean(struct oval_value *, int);	//datatype==OVAL_DATATYPE_BOOLEAN
void oval_value_set_float(struct oval_value *, float);	//datatype==OVAL_DATATYPE_FLOAT
void oval_value_set_integer(struct oval_value *, long);	//datatype==OVAL_DATATYPE_INTEGER

int                oval_value_iterator_has_more(struct oval_value_iterator *);
struct oval_value *oval_value_iterator_next    (struct oval_value_iterator *);
void               oval_value_iterator_free    (struct oval_value_iterator *);


/**
 * Get OVAL value datatype.
 * @relates oval_value
 */
oval_datatype_t    oval_value_get_datatype(struct oval_value *);

/**
 * Get OVAL value as a text.
 * @relates oval_value
 */
char              *oval_value_get_text    (struct oval_value *);

/**
 * Get OVAL value as a piece of raw binary data.
 * @relates oval_value
 */
unsigned char     *oval_value_get_binary  (struct oval_value *);//datatype==OVAL_DATATYPE_BINARY

/**
 * Get OVAL value as a boolean.
 * @relates oval_value
 */
bool               oval_value_get_boolean (struct oval_value *);//datatype==OVAL_DATATYPE_BOOLEAN

/**
 * Get OVAL value as a floating point number.
 * @relates oval_value
 */
float              oval_value_get_float   (struct oval_value *);//datatype==OVAL_DATATYPE_FLOAT

/**
 * Get OVAL value as an integer.
 * @relates oval_value
 */
long               oval_value_get_integer (struct oval_value *);//datatype==OVAL_DATATYPE_INTEGER

struct oval_state *oval_state_new(char *);
void oval_state_free(struct oval_state *);

void oval_state_set_subtype(struct oval_state *, oval_subtype_t);
void oval_state_add_note(struct oval_state *, char *);
void oval_state_set_comment(struct oval_state *, char *);
void oval_state_set_deprecated(struct oval_state *, int);
void oval_state_set_version(struct oval_state *, int);
void oval_state_add_content(struct oval_state *, struct oval_state_content *);

int                oval_state_iterator_has_more(struct oval_state_iterator *);
struct oval_state *oval_state_iterator_next    (struct oval_state_iterator *);
void               oval_state_iterator_free    (struct oval_state_iterator *);

/**
 * Get OVAL state family.
 * @relates oval_state
 */
oval_family_t                       oval_state_get_family    (struct oval_state *);

/**
 * Get OVAL state subtype.
 * @relates oval_state
 */
oval_subtype_t                      oval_state_get_subtype   (struct oval_state *);

/**
 * Get OVAL state name.
 * @relates oval_state
 */
const char                         *oval_state_get_name      (struct oval_state *);

/**
 * Get OVAL state comment.
 * @relates oval_state
 */
char                               *oval_state_get_comment   (struct oval_state *);

/**
 * Get OVAL state ID.
 * @relates oval_state
 */
char                               *oval_state_get_id        (struct oval_state *);

/**
 * Get OVAL state deprecation status.
 * @relates oval_state
 */
int                                 oval_state_get_deprecated(struct oval_state *);

/**
 * Get OVAL state version.
 * @relates oval_state
 */
int                                 oval_state_get_version   (struct oval_state *);

/**
 * Get OVAL state notes.
 * @relates oval_state
 */
struct oval_string_iterator        *oval_state_get_notes     (struct oval_state *);

/**
 * Get OVAL state contents.
 * @relates oval_state
 */
struct oval_state_content_iterator *oval_state_get_contents  (struct oval_state *);

struct oval_variable *oval_variable_new(char *, oval_variable_type_t);
void oval_variable_free(struct oval_variable *);

void oval_variable_set_comment   (struct oval_variable *, char *);
void oval_variable_set_deprecated(struct oval_variable *, bool);
void oval_variable_set_version   (struct oval_variable *, int);
void oval_variable_set_datatype(struct oval_variable *, oval_datatype_t);
void oval_variable_add_value(struct oval_variable *, struct oval_value *);	//type==OVAL_VARIABLE_CONSTANT
void oval_variable_set_component(struct oval_variable *, struct oval_component *);	//type==OVAL_VARIABLE_LOCAL

int                   oval_variable_iterator_has_more(struct oval_variable_iterator *);
struct oval_variable *oval_variable_iterator_next    (struct oval_variable_iterator *);
void                  oval_variable_iterator_free    (struct oval_variable_iterator *);

/**
 * Get OVAL variable ID.
 * @relates oval_variable
 */
char                       *oval_variable_get_id        (struct oval_variable *);

/**
 * Get OVAL variable comment.
 * @relates oval_variable
 */
char                       *oval_variable_get_comment   (struct oval_variable *);

/**
 * Get OVAL variable version.
 * @relates oval_variable
 */
int                         oval_variable_get_version   (struct oval_variable *);

/**
 * Get OVAL variable deprecation status.
 * @relates oval_variable
 */
int                         oval_variable_get_deprecated(struct oval_variable *);

/**
 * Get OVAL variable type.
 * @relates oval_variable
 */
oval_variable_type_t        oval_variable_get_type      (struct oval_variable *);

/**
 * Get OVAL variable datatype.
 * @relates oval_variable
 */
oval_datatype_t             oval_variable_get_datatype  (struct oval_variable *);

/**
 * Get OVAL constant variable values.
 * @relates oval_variable
 */
struct oval_value_iterator *oval_variable_get_values    (struct oval_variable *);//type==OVAL_VARIABLE_CONSTANT

/**
 * Get OVAL local variable main component.
 * @relates oval_variable
 */
struct oval_component      *oval_variable_get_component (struct oval_variable *);//type==OVAL_VARIABLE_LOCAL

struct oval_component *oval_component_new(oval_component_type_t);
void oval_component_free(struct oval_component *);

void oval_component_set_type(struct oval_component *, oval_component_type_t);
void oval_component_set_literal_value(struct oval_component *, struct oval_value *);	//type==OVAL_COMPONENT_LITERAL
void oval_component_set_object(struct oval_component *, struct oval_object *);	//type==OVAL_COMPONENT_OBJECTREF
void oval_component_set_object_field(struct oval_component *, char *);	//type==OVAL_COMPONENT_OBJECTREF
void oval_component_set_variable(struct oval_component *, struct oval_variable *);	//type==OVAL_COMPONENT_VARREF
void oval_component_add_function_component(struct oval_component *, struct oval_component *);	//type==OVAL_COMPONENT_FUNCTION

int                    oval_component_iterator_has_more(struct oval_component_iterator *);
struct oval_component *oval_component_iterator_next    (struct oval_component_iterator *);
void                   oval_component_iterator_free    (struct oval_component_iterator *);

/**
 * Get OVAL component type.
 * @relates oval_component
 */
oval_component_type_t           oval_component_get_type                (struct oval_component *);

/**
 * Get OVAL component literal value.
 * @see OVAL_COMPONENT_LITERAL
 * @relates oval_component
 */
struct oval_value              *oval_component_get_literal_value       (struct oval_component *);//type==OVAL_COMPONENT_LITERAL

/**
 * Get OVAL component object.
 * @see OVAL_COMPONENT_OBJECTREF
 * @relates oval_component
 */
struct oval_object             *oval_component_get_object              (struct oval_component *);//type==OVAL_COMPONENT_OBJECTREF

/**
 * Get OVAL component object field.
 * @see OVAL_COMPONENT_OBJECTREF
 * @relates oval_component
 */
char                           *oval_component_get_object_field        (struct oval_component *);//type==OVAL_COMPONENT_OBJECTREF

/**
 * Get OVAL component variable.
 * @see OVAL_COMPONENT_VARREF
 * @relates oval_component
 */
struct oval_variable           *oval_component_get_variable            (struct oval_component *);//type==OVAL_COMPONENT_VARREF

/**
 * Get OVAL component function components.
 * @see OVAL_COMPONENT_FUNCTION
 * @relates oval_component
 */
struct oval_component_iterator *oval_component_get_function_components (struct oval_component *);//type==OVAL_COMPONENT_FUNCTION

/**
 * Get OVAL component arithmetic operation.
 * @see OVAL_COMPONENT_ARITHMETIC
 * @relates oval_component
 */
oval_arithmetic_operation_t     oval_component_get_arithmetic_operation(struct oval_component *);//type==OVAL_COMPONENT_ARITHMETIC

/**
 * Get OVAL component begin character.
 * @see OVAL_COMPONENT_BEGIN
 * @relates oval_component
 */
char                           *oval_component_get_begin_character     (struct oval_component *);//type==OVAL_COMPONENT_BEGIN

/**
 * Get OVAL component end character.
 * @see OVAL_COMPONENT_END
 * @relates oval_component
 */
char                           *oval_component_get_end_character       (struct oval_component *);//type==OVAL_COMPONENT_END

/**
 * Get OVAL component split delimiter.
 * @see OVAL_COMPONENT_SPLIT
 * @relates oval_component
 */
char                           *oval_component_get_split_delimiter     (struct oval_component *);//type==OVAL_COMPONENT_SPLIT

/**
 * Get OVAL component start of a substring.
 * @see OVAL_COMPONENT_SUBSTRING
 * @relates oval_component
 */
int                             oval_component_get_substring_start     (struct oval_component *);//type==OVAL_COMPONENT_SUBSTRING

/**
 * Get OVAL component substring length.
 * @see OVAL_COMPONENT_SUBSTRING
 * @relates oval_component
 */
int                             oval_component_get_substring_length    (struct oval_component *);//type==OVAL_COMPONENT_SUBSTRING

/**
 * Get OVAL component timedif format 1.
 * @see OVAL_COMPONENT_TIMEDIF
 * @relates oval_component
 */
oval_datetime_format_t          oval_component_get_timedif_format_1    (struct oval_component *);//type==OVAL_COMPONENT_TIMEDIF

/**
 * Get OVAL component timedif format 2.
 * @see OVAL_COMPONENT_TIMEDIF
 * @relates oval_component
 */
oval_datetime_format_t          oval_component_get_timedif_format_2    (struct oval_component *);//type==OVAL_COMPONENT_TIMEDIF

/**
 * Get OVAL component regex pattern.
 * @see OVAL_COMPONENT_REGEX_CAPTURE
 * @relates oval_component
 */
char                           *oval_component_get_regex_pattern       (struct oval_component *); //type==OVAL_COMPONENT_REGEX_CAPTURE
struct oval_value              *oval_component_get_literal_value       (struct oval_component *);//type==OVAL_COMPONENT_LITERAL

/**
 * Get OVAL component object.
 * @see OVAL_COMPONENT_OBJECTREF
 * @relates oval_component
 */
struct oval_object             *oval_component_get_object              (struct oval_component *);//type==OVAL_COMPONENT_OBJECTREF

/**
 * Get OVAL component object field.
 * @see OVAL_COMPONENT_OBJECTREF
 * @relates oval_component
 */
char                           *oval_component_get_object_field        (struct oval_component *);//type==OVAL_COMPONENT_OBJECTREF

/**
 * Get OVAL component variable.
 * @see OVAL_COMPONENT_VARREF
 * @relates oval_component
 */
struct oval_variable           *oval_component_get_variable            (struct oval_component *);//type==OVAL_COMPONENT_VARREF

/**
 * Get OVAL component function components.
 * @see OVAL_COMPONENT_FUNCTION
 * @relates oval_component
 */
struct oval_component_iterator *oval_component_get_function_components (struct oval_component *);//type==OVAL_COMPONENT_FUNCTION

/**
 * Set OVAL component arithmetic operation.
 * @see OVAL_COMPONENT_ARITHMETIC
 * @relates oval_component
 */
void oval_component_set_arithmetic_operation
	(struct oval_component *,oval_arithmetic_operation_t);//type==OVAL_COMPONENT_ARITHMETIC

/**
 * Set OVAL component begin character.
 * @see OVAL_COMPONENT_BEGIN
 * @relates oval_component
 */
void oval_component_set_begin_character(struct oval_component *,char *);//type==OVAL_COMPONENT_BEGIN

/**
 * Set OVAL component end character.
 * @see OVAL_COMPONENT_END
 * @relates oval_component
 */
void oval_component_set_end_character(struct oval_component *, char *);//type==OVAL_COMPONENT_END

/**
 * Set OVAL component split delimiter.
 * @see OVAL_COMPONENT_SPLIT
 * @relates oval_component
 */
void oval_component_set_split_delimiter(struct oval_component *, char *);//type==OVAL_COMPONENT_SPLIT

/**
 * Set OVAL component start of a substring.
 * @see OVAL_COMPONENT_SUBSTRING
 * @relates oval_component
 */
void oval_component_set_substring_start(struct oval_component *, int);//type==OVAL_COMPONENT_SUBSTRING

/**
 * Set OVAL component substring length.
 * @see OVAL_COMPONENT_SUBSTRING
 * @relates oval_component
 */
void oval_component_set_substring_length    (struct oval_component *, int);//type==OVAL_COMPONENT_SUBSTRING

/**
 * Set OVAL component timedif format 1.
 * @see OVAL_COMPONENT_TIMEDIF
 * @relates oval_component
 */
void oval_component_set_timedif_format_1(struct oval_component *, oval_datetime_format_t);//type==OVAL_COMPONENT_TIMEDIF

/**
 * Set OVAL component timedif format 2.
 * @see OVAL_COMPONENT_TIMEDIF
 * @relates oval_component
 */
void oval_component_set_timedif_format_2(struct oval_component *, oval_datetime_format_t);//type==OVAL_COMPONENT_TIMEDIF

/**
 * Set OVAL component regex pattern.
 * @see OVAL_COMPONENT_REGEX_CAPTURE
 * @relates oval_component
 */
void oval_component_set_regex_pattern(struct oval_component *, char *); //type==OVAL_COMPONENT_REGEX_CAPTURE

struct oval_message *oval_message_new();
struct oval_message *oval_message_clone(struct oval_message *old_message);
void oval_message_free(struct oval_message *);

void oval_message_set_text(struct oval_message *, char *);
void oval_message_set_level(struct oval_message *, oval_message_level_t);

int                  oval_message_iterator_has_more(struct oval_message_iterator *oc_message);
struct oval_message *oval_message_iterator_next    (struct oval_message_iterator *oc_message);
void                 oval_message_iterator_free    (struct oval_message_iterator *oc_message);

/**
 * Get OVAL message text.
 * @relates oval_message
 */
char                   *oval_message_get_text (struct oval_message *message);

/**
 * Get OVAL message level.
 * @relates oval_message
 */
oval_message_level_t    oval_message_get_level(struct oval_message *message);

/**
 * Convert an operator to its text representation.
 */
const char*   oval_operator_get_text(oval_operator_t);

/**
 * Get the family associated with a given subtype.
 */
oval_family_t oval_subtype_get_family(oval_subtype_t);

/**
 * Convert OVAL subtype to its text representation.
 */
const char*   oval_subtype_get_text(oval_subtype_t);

/**
 * Convert OVAL family to its text representation.
 */
const char*   oval_family_get_text(oval_family_t);

const char *oval_check_get_text(oval_check_t);
const char *oval_existence_get_text(oval_existence_t);
const char *oval_affected_family_get_text(oval_affected_family_t);
const char *oval_datatype_get_text(oval_datatype_t);
const char *oval_operation_get_text(oval_operation_t);
const char *oval_set_operation_get_text(oval_setobject_operation_t);
const char *oval_datetime_format_get_text(oval_datetime_format_t);
const char *oval_arithmetic_operation_get_text(oval_arithmetic_operation_t);

#endif
