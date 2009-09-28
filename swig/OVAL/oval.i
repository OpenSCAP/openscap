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

%module oval
%{
 #include "../../src/common/list.h"
 #include "../../src/common/alloc.h"
 #include "../../src/common/oscap.h"
 #include "../../src/common/util.h"
 #include "../../src/OVAL/api/oval_agent_api.h"
 #include "../../src/OVAL/api/oval_definitions.h"
 #include "../../src/OVAL/api/oval_system_characteristics.h"
 #include "../../src/OVAL/api/oval_results.h"
%}

/* oval_agent_api.h */

struct oval_export_target;
struct oval_import_source;
struct oval_import_source *oval_import_source_new_file(char *filename);
void oval_import_source_free(struct oval_import_source *source);
struct oval_export_target *oval_export_target_new_file(char *filename, char* encoding);
void oval_export_target_free(struct oval_export_target *target);
struct oval_object_model;
struct oval_syschar_model;
struct oval_results_model;
struct oval_object_model *oval_object_model_new();
void oval_object_model_free(struct oval_object_model * model);
typedef enum {
        XML_VALIDITY_WARNING = 1,
        XML_VALIDITY_ERROR   = 2,
        XML_WARNING          = 3,
        XML_ERROR            = 4,
        OVAL_LOG_INFO        = 15,
        OVAL_LOG_DEBUG       = 16,
        OVAL_LOG_WARN        = 17
};

struct oval_xml_error {
        void *user_arg;
        char *message;
        oval_xml_severity_t severity;
        int line_number;
        char *system_id;
};

/// OVAL XML error handler function pointer type.
typedef int (*oval_xml_error_handler) (struct oval_xml_error *, void *user_arg);
void oval_object_model_load(
                        struct oval_object_model *model,
                        struct oval_import_source *source,
                        oval_xml_error_handler error_handler, void *client);

struct oval_definition *oval_object_model_get_definition(
                struct oval_object_model *model,
                char *id);

struct oval_test *oval_object_model_get_test(
                struct oval_object_model *model,
                char *id);

struct oval_object *oval_object_model_get_object(
                struct oval_object_model *model,
                char *id);

struct oval_state *oval_object_model_get_state(
                struct oval_object_model *model,
                char *id);

struct oval_variable *oval_object_model_get_variable(
                struct oval_object_model *model,
                char *id);

struct oval_definition_iterator *oval_object_model_get_definitions(
                struct oval_object_model *model);

struct oval_test_iterator *oval_object_model_get_tests(
                struct oval_object_model *model);

struct oval_object_iterator *oval_object_model_get_objects(
                struct oval_object_model *model);

struct oval_state_iterator *oval_object_model_get_states(
                struct oval_object_model *model);

struct oval_variable_iterator *oval_object_model_get_variables(
                struct oval_object_model *model);

int oval_object_model_export(
                struct oval_object_model *, struct oval_export_target *);

void oval_object_model_add_definition(struct oval_object_model *, struct oval_definition *);
void oval_object_model_add_test(struct oval_object_model *, struct oval_test *);
void oval_object_model_add_object(struct oval_object_model *, struct oval_object *);
void oval_object_model_add_state(struct oval_object_model *, struct oval_state *);
void oval_object_model_add_variable(struct oval_object_model *, struct oval_variable *);
void oval_object_model_add_sysinfo(struct oval_object_model *, struct oval_sysinfo *);

struct oval_syschar_model *oval_syschar_model_new(
                struct oval_object_model *object_model,
                struct oval_variable_binding_iterator *bindings);

void oval_syschar_model_free(struct oval_syschar_model *model);
struct oval_object_model *oval_syschar_model_get_object_model(
                struct oval_syschar_model *model);
struct oval_syschar_iterator *oval_syschar_model_get_syschars(
                struct oval_syschar_model *model);
struct oval_sysinfo *oval_syschar_model_get_sysinfo(
                struct oval_syschar_model *model);

struct oval_syschar *oval_syschar_model_get_syschar(
                struct oval_syschar_model *model,
                char *object_id);

bool oval_syschar_model_add_variable_binding(struct oval_syschar_model *, struct oval_variable_binding *);
int oval_syschar_model_export(
                struct oval_syschar_model *, struct oval_export_target *);
void oval_syschar_model_load(struct oval_syschar_model*, struct oval_import_source*,
                        oval_xml_error_handler, void*);
struct oval_results_model *oval_results_model_new(
                struct oval_object_model *object_model, struct oval_syschar_model **);
void oval_results_model_free(struct oval_results_model *model);
struct oval_object_model *oval_results_model_get_object_model
        (struct oval_results_model *model);
struct oval_result_system_iterator *oval_results_model_get_systems
                (struct oval_results_model *);

void oval_results_model_add_system(struct oval_results_model *, struct oval_result_system *);
struct oval_result_directives *oval_results_model_load
        (struct oval_results_model *, struct oval_import_source *,
                        oval_xml_error_handler, void*);
struct oval_result_directives *oval_result_directives_new();
void oval_result_directives_free(struct oval_result_directives *);

int oval_results_model_export(struct oval_results_model *, struct oval_result_directives *, struct oval_export_target *);
struct oval_syschar *oval_object_probe(struct oval_object *, struct oval_object_model *model);

/* oval_definitions.h */

typedef enum {
        OVAL_FAMILY_UNKNOWN = 0,
        OVAL_FAMILY_AIX = 1000,
        OVAL_FAMILY_APACHE = 2000,
        OVAL_FAMILY_CATOS = 3000,
        OVAL_FAMILY_ESX = 4000,
        OVAL_FAMILY_FREEBSD = 5000,
        OVAL_FAMILY_HPUX = 6000,
        OVAL_FAMILY_INDEPENDENT = 7000,
        OVAL_FAMILY_IOS = 8000,
        OVAL_FAMILY_LINUX = 9000,
        OVAL_FAMILY_MACOS = 10000,
        OVAL_FAMILY_PIXOS = 11000,
        OVAL_FAMILY_SOLARIS = 12000,
        OVAL_FAMILY_UNIX = 13000,
        OVAL_FAMILY_WINDOWS = 14000
} oval_family_t;

typedef enum {
        OVAL_SUBTYPE_UNKNOWN = 0
} oval_subtype_t;

typedef enum {
        OVAL_AIX_FILESET = 1001,
        OVAL_AIX_FIX = OVAL_FAMILY_AIX + 2,
        OVAL_AIX_OSLEVEL = OVAL_FAMILY_AIX + 3
} oval_aix_subtype_t;

typedef enum {
        OVAL_APACHE_HTTPD = OVAL_FAMILY_APACHE + 1,
        OVAL_APACHE_VERSION = OVAL_FAMILY_APACHE + 2
} oval_apache_subtype_t;

typedef enum {
        OVAL_CATOS_LINE = OVAL_FAMILY_CATOS + 1,
        OVAL_CATOS_MODULE = OVAL_FAMILY_CATOS + 2,
        OVAL_CATOS_VERSION_55 = OVAL_FAMILY_CATOS + 3,
        OVAL_CATOS_VERSION = OVAL_FAMILY_CATOS + 4
} oval_catos_subtype_t;

typedef enum {
        OVAL_ESX_PATCH = OVAL_FAMILY_ESX + 1,
        OVAL_ESX_VERSION = OVAL_FAMILY_ESX + 2
} oval_esx_subtype_t;

typedef enum {
        OVAL_FREEBSD_PORT_INFO = OVAL_FAMILY_FREEBSD + 1
} oval_freebsd_subtype_t;

typedef enum {
        OVAL_HPUX_GETCONF = OVAL_FAMILY_HPUX + 1,
        OVAL_HPUX_PATCH_53 = OVAL_FAMILY_HPUX + 2,
        OVAL_HPUX_PATCH = OVAL_FAMILY_HPUX + 3,
        OVAL_HPUX_SOFTWARE_LIST = OVAL_FAMILY_HPUX + 4,
        OVAL_HPUX_TRUSTED_ACCOUNTS = OVAL_FAMILY_HPUX + 5
} oval_hpux_subtype_t;

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

typedef enum {
        OVAL_IOS_GLOBAL = OVAL_FAMILY_IOS + 1,
        OVAL_IOS_INTERFACE = OVAL_FAMILY_IOS + 2,
        OVAL_IOS_LINE = OVAL_FAMILY_IOS + 3,
        OVAL_IOS_SNMP = OVAL_FAMILY_IOS + 4,
        OVAL_IOS_TCLSH = OVAL_FAMILY_IOS + 5,
        OVAL_IOS_VERSION_55 = OVAL_FAMILY_IOS + 6
} oval_ios_subtype_t;

typedef enum {
        OVAL_LINUX_DPKG_INFO = OVAL_FAMILY_LINUX + 1,
        OVAL_LINUX_INET_LISTENING_SERVERS = OVAL_FAMILY_LINUX + 2,
        OVAL_LINUX_RPM_INFO = OVAL_FAMILY_LINUX + 3,
        OVAL_LINUX_SLACKWARE_PKG_INFO_TEST = OVAL_FAMILY_LINUX + 4
} oval_linux_subtype_t;

typedef enum {
        OVAL_MACOS_ACCOUNT_INFO = OVAL_FAMILY_MACOS + 1,
        OVAL_MACOS_INET_LISTENING_SERVERS = OVAL_FAMILY_MACOS + 2,
        OVAL_MACOS_NVRAM_INFO = OVAL_FAMILY_MACOS + 3,
        OVAL_MACOS_PWPOLICY = OVAL_FAMILY_MACOS + 4
} oval_macos_subtype_t;

typedef enum {
        OVAL_PIXOS_LINE = OVAL_FAMILY_PIXOS + 1,
        OVAL_PIXOS_VERSION = OVAL_FAMILY_PIXOS + 2
} oval_pixos_subtype_t;

typedef enum {
        OVAL_SOLARIS_ISAINFO = OVAL_FAMILY_SOLARIS + 1,
        OVAL_SOLARIS_PACKAGE = OVAL_FAMILY_SOLARIS + 2,
        OVAL_SOLARIS_PATCH = OVAL_FAMILY_SOLARIS + 3,
        OVAL_SOLARIS_SMF = OVAL_FAMILY_SOLARIS + 4
} oval_solaris_subtype_t;

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

typedef enum {
        OVAL_NODETYPE_UNKNOWN = 0,
        OVAL_NODETYPE_CRITERIA = 1,
        OVAL_NODETYPE_CRITERION = 2,
        OVAL_NODETYPE_EXTENDDEF = 3
} oval_criteria_node_type_t;

typedef enum {
        OVAL_OPERATOR_UNKNOWN = 0,
        OVAL_OPERATOR_AND     = 1,
        OVAL_OPERATOR_ONE     = 2,
        OVAL_OPERATOR_OR      = 3,
        OVAL_OPERATOR_XOR     = 4
} oval_operator_t;

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

typedef enum {
        OVAL_CLASS_UNKNOWN      = 0,
        OVAL_CLASS_COMPLIANCE   = 1,
        OVAL_CLASS_INVENTORY    = 2,
        OVAL_CLASS_MISCELLANEOUS= 3,
        OVAL_CLASS_PATCH        = 4,
        OVAL_CLASS_VULNERABILITY= 5,
} oval_definition_class_t;

typedef enum {
        OVAL_EXISTENCE_UNKNOWN  = 0,
        OVAL_ALL_EXIST          = 1,
        OVAL_ANY_EXIST          = 2,
        OVAL_AT_LEAST_ONE_EXISTS= 3,
        OVAL_ONLY_ONE_EXISTS    = 4,
        OVAL_NONE_EXIST         = 5
} oval_existence_t;

typedef enum {
        OVAL_CHECK_UNKNOWN     = 0,
        OVAL_CHECK_ALL         = 1,
        OVAL_CHECK_AT_LEAST_ONE= 2,
        OVAL_CHECK_NONE_EXIST  = 3,
        OVAL_CHECK_NONE_SATISFY= 4,
        OVAL_CHECK_ONLY_ONE    = 5
} oval_check_t;

typedef enum {
        OVAL_OBJECTCONTENT_UNKNOWN = 0,
        OVAL_OBJECTCONTENT_ENTITY  = 1,
        OVAL_OBJECTCONTENT_SET     = 2
} oval_object_content_type_t;

typedef enum {
        OVAL_ENTITY_TYPE_UNKNOWN,
        OVAL_ENTITY_TYPE_ANY,
        OVAL_ENTITY_TYPE_BINARY,
        OVAL_ENTITY_TYPE_BOOLEAN,
        OVAL_ENTITY_TYPE_FLOAT,
        OVAL_ENTITY_TYPE_INTEGER,
        OVAL_ENTITY_TYPE_STRING,
} oval_entity_type_t;

typedef enum {
        OVAL_DATATYPE_UNKNOWN           = 0,
        OVAL_DATATYPE_BINARY            = 1,
        OVAL_DATATYPE_BOOLEAN           = 2,
        OVAL_DATATYPE_EVR_STRING        = 3,
        OVAL_DATATYPE_FILESET_REVISTION = 4,
        OVAL_DATATYPE_FLOAT             = 5,
        OVAL_DATATYPE_IOS_VERSION       = 6,
        OVAL_DATATYPE_VERSION           = 7,
        OVAL_DATATYPE_INTEGER           = 8,
        OVAL_DATATYPE_STRING            = 9
} oval_datatype_t;

typedef enum {
        OVAL_ENTITY_VARREF_UNKNOWN,
        OVAL_ENTITY_VARREF_NONE,
        OVAL_ENTITY_VARREF_ATTRIBUTE,
        OVAL_ENTITY_VARREF_ELEMENT
} oval_entity_varref_type_t;

typedef enum {
        OVAL_SET_UNKNOWN,
        OVAL_SET_AGGREGATE,
        OVAL_SET_COLLECTIVE
} oval_setobject_type_t;

typedef enum {
        OVAL_SET_OPERATION_UNKNOWN,
        OVAL_SET_OPERATION_COMPLEMENT   = 1,
        OVAL_SET_OPERATION_INTERSECTION = 2,
        OVAL_SET_OPERATION_UNION        = 3
} oval_setobject_operation_t;

typedef enum {
        OVAL_VARIABLE_UNKNOWN,
        OVAL_VARIABLE_EXTERNAL,
        OVAL_VARIABLE_CONSTANT,
        OVAL_VARIABLE_LOCAL
} oval_variable_type_t;

#define OVAL_FUNCTION 10
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

typedef enum {
        OVAL_ARITHMETIC_UNKNOWN = 0,
        OVAL_ARITHMETIC_ADD     = 1,
        OVAL_ARITHMETIC_MULTIPLY =2,
        OVAL_ARITHMETIC_SUBTRACT =3,
        OVAL_ARITHMETIC_DIVIDE   =4
} oval_arithmetic_operation_t;

typedef enum {
        OVAL_DATETIME_UNKNOWN             = 0,
        OVAL_DATETIME_YEAR_MONTH_DAY      = 1,
        OVAL_DATETIME_MONTH_DAY_YEAR      = 2,
        OVAL_DATETIME_DAY_MONTH_YEAR      = 3,
        OVAL_DATETIME_WIN_FILETIME        = 4,
        OVAL_DATETIME_SECONDS_SINCE_EPOCH = 5
} oval_datetime_format_t;

typedef enum {
        OVAL_MESSAGE_LEVEL_NONE    = 0,
        OVAL_MESSAGE_LEVEL_DEBUG   = 1,
        OVAL_MESSAGE_LEVEL_INFO    = 2,
        OVAL_MESSAGE_LEVEL_WARNING = 3,
        OVAL_MESSAGE_LEVEL_ERROR   = 4,
        OVAL_MESSAGE_LEVEL_FATAL   = 5
} oval_message_level_t;

struct oval_string_iterator;
struct oval_affected;
struct oval_affected_iterator;
struct oval_test;
struct oval_test_iterator;
struct oval_criteria_node;
struct oval_criteria_node_iterator;
struct oval_reference;
struct oval_reference_iterator;
struct oval_definition;
struct oval_definition_iterator;
struct oval_object;
struct oval_object_iterator;
struct oval_state;
struct oval_state_iterator;
struct oval_variable;
struct oval_variable_iterator;
struct oval_variable_binding;
struct oval_variable_binding_iterator;
struct oval_object_content;
struct oval_object_content_iterator;
struct oval_state_content;
struct oval_state_content_iterator;
struct oval_behavior;
struct oval_behavior_iterator;
struct oval_entity;
struct oval_entity_iterator;
struct oval_setobject;
struct oval_setobject_iterator;
struct oval_value;
struct oval_value_iterator;
struct oval_component;
struct oval_component_iterator;
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
oval_affected_family_t       oval_affected_get_family  (struct oval_affected *);
struct oval_string_iterator *oval_affected_get_platform(struct oval_affected *);
struct oval_string_iterator *oval_affected_get_product (struct oval_affected *);

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
oval_criteria_node_type_t           oval_criteria_node_get_type      (struct oval_criteria_node *);
int                                 oval_criteria_node_get_negate    (struct oval_criteria_node *);
char                               *oval_criteria_node_get_comment   (struct oval_criteria_node *);
oval_operator_t                     oval_criteria_node_get_operator  (struct oval_criteria_node *);//type==NODETYPE_CRITERIA
struct oval_criteria_node_iterator *oval_criteria_node_get_subnodes  (struct oval_criteria_node *);//type==NODETYPE_CRITERIA
struct oval_test                   *oval_criteria_node_get_test      (struct oval_criteria_node *);//type==NODETYPE_CRITERION
struct oval_definition             *oval_criteria_node_get_definition(struct oval_criteria_node *);//type==NODETYPE_EXTENDDEF

struct oval_reference *oval_reference_new();
void oval_reference_free(struct oval_reference *);

void oval_reference_set_source(struct oval_reference *, char *);
void oval_reference_set_id(struct oval_reference *, char *);
void oval_reference_set_url(struct oval_reference *, char *);

int                    oval_reference_iterator_has_more(struct oval_reference_iterator *);
struct oval_reference *oval_reference_iterator_next    (struct oval_reference_iterator *);
void                   oval_reference_iterator_free    (struct oval_reference_iterator *);
char *oval_reference_get_source(struct oval_reference *);
char *oval_reference_get_id    (struct oval_reference *);
char *oval_reference_get_url   (struct oval_reference *);

struct oval_definition *oval_definition_new(char *id);
void oval_definition_free(struct oval_definition *);

void oval_definition_set_id(struct oval_definition *, char *);
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
char                           *oval_definition_get_id(struct oval_definition *);
int                             oval_definition_get_version(struct oval_definition *);
oval_definition_class_t         oval_definition_get_class(struct oval_definition *);
int                             oval_definition_get_deprecated(struct oval_definition *);
char                           *oval_definition_get_title(struct oval_definition *);
char                           *oval_definition_get_description(struct oval_definition *);
struct oval_affected_iterator  *oval_definition_get_affected(struct oval_definition*);
struct oval_reference_iterator *oval_definition_get_references(struct oval_definition*);
struct oval_string_iterator    *oval_definition_get_notes(struct oval_definition *);
struct oval_criteria_node      *oval_definition_get_criteria(struct oval_definition *);

struct oval_object *oval_object_new(char *id);
void oval_object_free(struct oval_object *);

void oval_object_set_family(struct oval_object *, oval_family_t);
void oval_object_set_subtype(struct oval_object *, oval_subtype_t);
void oval_object_set_name(struct oval_object *, char *);
void oval_object_add_note(struct oval_object *, char *);
void oval_object_set_comment(struct oval_object *, char *);
void oval_object_set_deprecated(struct oval_object *, int);
void oval_object_set_version(struct oval_object *, int);
void oval_object_add_object_content(struct oval_object *, struct oval_object_content *);
void oval_object_add_behaviors(struct oval_object *, struct oval_behavior *);

int                 oval_object_iterator_has_more(struct oval_object_iterator *);
struct oval_object *oval_object_iterator_next    (struct oval_object_iterator *);
void                oval_object_iterator_free    (struct oval_object_iterator *);
oval_family_t                        oval_object_get_family        (struct oval_object *);
oval_subtype_t                       oval_object_get_subtype       (struct oval_object *);
char                                *oval_object_get_name          (struct oval_object *);
struct oval_string_iterator         *oval_object_get_notes         (struct oval_object *);
char                                *oval_object_get_comment       (struct oval_object *);
char                                *oval_object_get_id            (struct oval_object *);
int                                  oval_object_get_deprecated    (struct oval_object *);
int                                  oval_object_get_version       (struct oval_object *);
struct oval_object_content_iterator *oval_object_get_object_content(struct oval_object *);
struct oval_behavior_iterator       *oval_object_get_behaviors     (struct oval_object *);

struct oval_test *oval_test_new(char *);
void oval_test_free(struct oval_test *);

void oval_test_set_family(struct oval_test *, oval_family_t);
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
oval_family_t                oval_test_get_family    (struct oval_test *);
oval_subtype_t               oval_test_get_subtype   (struct oval_test *);
struct oval_string_iterator *oval_test_get_notes     (struct oval_test *);
char                        *oval_test_get_comment   (struct oval_test *);
char                        *oval_test_get_id        (struct oval_test *);
int                          oval_test_get_deprecated(struct oval_test *);
int                          oval_test_get_version   (struct oval_test *);
oval_existence_t             oval_test_get_existence (struct oval_test *);
oval_check_t                 oval_test_get_check     (struct oval_test *);
struct oval_object          *oval_test_get_object    (struct oval_test *);
struct oval_state           *oval_test_get_state     (struct oval_test *);

struct oval_variable_binding *oval_variable_binding_new(struct oval_variable *, char *);
void oval_variable_binding_free(struct oval_variable_binding *);

void oval_variable_binding_set_variable(struct oval_variable_binding *, struct oval_variable *);
void oval_variable_binding_set_value   (struct oval_variable_binding *, char *);

int                           oval_variable_binding_iterator_has_more(struct oval_variable_binding_iterator *);
struct oval_variable_binding *oval_variable_binding_iterator_next    (struct oval_variable_binding_iterator *);
void                          oval_variable_binding_iterator_free    (struct oval_variable_binding_iterator *);
struct oval_variable *oval_variable_binding_get_variable(struct oval_variable_binding *);
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
char                          *oval_object_content_get_field_name(struct oval_object_content *);
oval_object_content_type_t     oval_object_content_get_type      (struct oval_object_content *);
struct oval_entity            *oval_object_content_get_entity    (struct oval_object_content *);//type == OVAL_OBJECTCONTENT_ENTITY
oval_check_t                   oval_object_content_get_varCheck  (struct oval_object_content *);//type == OVAL_OBJECTCONTENT_ENTITY
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
void oval_entity_set_operator(struct oval_entity *, oval_operator_t);
void oval_entity_set_mask(struct oval_entity *, int);
void oval_entity_set_varref_type(struct oval_entity *, oval_entity_varref_type_t);
void oval_entity_set_variable(struct oval_entity *, struct oval_variable *);
void oval_entity_set_value(struct oval_entity *, struct oval_value *);

int                        oval_state_content_iterator_has_more(struct oval_state_content_iterator *);
struct oval_state_content *oval_state_content_iterator_next    (struct oval_state_content_iterator *);
void                       oval_state_content_iterator_free    (struct oval_state_content_iterator *);
struct oval_entity *oval_state_content_get_entity   (struct oval_state_content *);
oval_check_t        oval_state_content_get_var_check(struct oval_state_content *);
oval_check_t        oval_state_content_get_ent_check(struct oval_state_content *);

int                 oval_entity_iterator_has_more(struct oval_entity_iterator *);
struct oval_entity *oval_entity_iterator_next    (struct oval_entity_iterator *);
void                oval_entity_iterator_free    (struct oval_entity_iterator *);

char                         *oval_entity_get_name       (struct oval_entity *);
oval_entity_type_t            oval_entity_get_type       (struct oval_entity *);
oval_datatype_t               oval_entity_get_datatype   (struct oval_entity *);
oval_operation_t              oval_entity_get_operation  (struct oval_entity *);
int                           oval_entity_get_mask       (struct oval_entity *);
oval_entity_varref_type_t     oval_entity_get_varref_type(struct oval_entity *);
struct oval_variable         *oval_entity_get_variable   (struct oval_entity *);
struct oval_value            *oval_entity_get_value      (struct oval_entity *);

struct oval_setobject *oval_setobject_new();
void oval_set_free(struct oval_setobject *);

void oval_setobject_set_type(struct oval_setobject *, oval_setobject_type_t);
void oval_setobject_set_operation(struct oval_setobject *, oval_setobject_operation_t);
void oval_setobject_add_subset(struct oval_setobject *, struct oval_setobject *);//type==OVAL_SET_AGGREGATE;
void oval_setobject_add_object(struct oval_setobject *, struct oval_object *);	//type==OVAL_SET_COLLECTIVE;
void oval_setobject_add_filter(struct oval_setobject *, struct oval_state *);	//type==OVAL_SET_COLLECTIVE;

int                    oval_setobject_iterator_has_more(struct oval_setobject_iterator *);
struct oval_setobject *oval_setobject_iterator_next    (struct oval_setobject_iterator *);
void                   oval_setobject_iterator_free    (struct oval_setobject_iterator *);

oval_setobject_type_t              oval_setobject_get_type     (struct oval_setobject *);
oval_setobject_operation_t         oval_setobject_get_operation(struct oval_setobject *);
struct oval_setobject_iterator    *oval_setobject_get_subsets  (struct oval_setobject *);//type==OVAL_SET_AGGREGATE;
struct oval_object_iterator *oval_setobject_get_objects  (struct oval_setobject *);//type==OVAL_SET_COLLECTIVE;
struct oval_state_iterator  *oval_setobject_get_filters  (struct oval_setobject *);//type==OVAL_SET_COLLECTIVE;

struct oval_behavior *oval_behavior_new();
void oval_behavior_free(struct oval_behavior *);

void behavior_set_keyval(struct oval_behavior *behavior, const char* key, const char* value);

int                   oval_behavior_iterator_has_more(struct oval_behavior_iterator *);
struct oval_behavior *oval_behavior_iterator_next    (struct oval_behavior_iterator *);
void                  oval_behavior_iterator_free    (struct oval_behavior_iterator *);
char *oval_behavior_get_key  (struct oval_behavior *);
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

oval_datatype_t    oval_value_get_datatype(struct oval_value *);
char              *oval_value_get_text    (struct oval_value *);
unsigned char     *oval_value_get_binary  (struct oval_value *);//datatype==OVAL_DATATYPE_BINARY
bool               oval_value_get_boolean (struct oval_value *);//datatype==OVAL_DATATYPE_BOOLEAN
float              oval_value_get_float   (struct oval_value *);//datatype==OVAL_DATATYPE_FLOAT
long               oval_value_get_integer (struct oval_value *);//datatype==OVAL_DATATYPE_INTEGER

struct oval_state *oval_state_new(char *);
void oval_state_free(struct oval_state *);

void oval_state_set_family(struct oval_state *, oval_family_t);
void oval_state_set_subtype(struct oval_state *, oval_subtype_t);
void oval_state_set_name(struct oval_state *, char *);
void oval_state_add_note(struct oval_state *, char *);
void oval_state_set_comment(struct oval_state *, char *);
void oval_state_set_deprecated(struct oval_state *, int);
void oval_state_set_version(struct oval_state *, int);
void oval_state_add_content(struct oval_state *, struct oval_state_content *);

int                oval_state_iterator_has_more(struct oval_state_iterator *);
struct oval_state *oval_state_iterator_next    (struct oval_state_iterator *);
void               oval_state_iterator_free    (struct oval_state_iterator *);
oval_family_t                       oval_state_get_family    (struct oval_state *);

/**
 * Get OVAL state subtype.
 * @relates oval_state
 */
oval_subtype_t                      oval_state_get_subtype   (struct oval_state *);
char                               *oval_state_get_name      (struct oval_state *);
char                               *oval_state_get_comment   (struct oval_state *);
char                               *oval_state_get_id        (struct oval_state *);
int                                 oval_state_get_deprecated(struct oval_state *);
int                                 oval_state_get_version   (struct oval_state *);
struct oval_string_iterator        *oval_state_get_notes     (struct oval_state *);
struct oval_state_content_iterator *oval_state_get_contents  (struct oval_state *);

struct oval_variable *oval_variable_new(char *, oval_variable_type_t);
void oval_variable_free(struct oval_variable *);

void oval_variable_set_type(struct oval_variable *, oval_variable_type_t);
void oval_variable_set_datatype(struct oval_variable *, oval_datatype_t);
void oval_variable_add_value(struct oval_variable *, struct oval_value *);	//type==OVAL_VARIABLE_CONSTANT
void oval_variable_set_component(struct oval_variable *, struct oval_component *);	//type==OVAL_VARIABLE_LOCAL

int                   oval_variable_iterator_has_more(struct oval_variable_iterator *);
struct oval_variable *oval_variable_iterator_next    (struct oval_variable_iterator *);
void                  oval_variable_iterator_free    (struct oval_variable_iterator *);
char                       *oval_variable_get_id        (struct oval_variable *);
char                       *oval_variable_get_comment   (struct oval_variable *);
int                         oval_variable_get_version   (struct oval_variable *);
int                         oval_variable_get_deprecated(struct oval_variable *);
oval_variable_type_t        oval_variable_get_type      (struct oval_variable *);
oval_datatype_t             oval_variable_get_datatype  (struct oval_variable *);
struct oval_value_iterator *oval_variable_get_values    (struct oval_variable *);//type==OVAL_VARIABLE_CONSTANT
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
oval_component_type_t           oval_component_get_type                (struct oval_component *);
struct oval_value              *oval_component_get_literal_value       (struct oval_component *);//type==OVAL_COMPONENT_LITERAL
struct oval_object             *oval_component_get_object              (struct oval_component *);//type==OVAL_COMPONENT_OBJECTREF
char                           *oval_component_get_object_field        (struct oval_component *);//type==OVAL_COMPONENT_OBJECTREF
struct oval_variable           *oval_component_get_variable            (struct oval_component *);//type==OVAL_COMPONENT_VARREF
struct oval_component_iterator *oval_component_get_function_components (struct oval_component *);//type==OVAL_COMPONENT_FUNCTION
oval_arithmetic_operation_t     oval_component_get_arithmetic_operation(struct oval_component *);//type==OVAL_COMPONENT_ARITHMETIC
char                           *oval_component_get_begin_character     (struct oval_component *);//type==OVAL_COMPONENT_BEGIN
char                           *oval_component_get_end_character       (struct oval_component *);//type==OVAL_COMPONENT_END
char                           *oval_component_get_split_delimiter     (struct oval_component *);//type==OVAL_COMPONENT_SPLIT
int                             oval_component_get_substring_start     (struct oval_component *);//type==OVAL_COMPONENT_SUBSTRING
int                             oval_component_get_substring_length    (struct oval_component *);//type==OVAL_COMPONENT_SUBSTRING
oval_datetime_format_t          oval_component_get_timedif_format_1    (struct oval_component *);//type==OVAL_COMPONENT_TIMEDIF
oval_datetime_format_t          oval_component_get_timedif_format_2    (struct oval_component *);//type==OVAL_COMPONENT_TIMEDIF
char                           *oval_component_get_regex_pattern       (struct oval_component *); //type==OVAL_COMPONENT_REGEX_CAPTURE

struct oval_message *oval_message_new();
void oval_message_free(struct oval_message *);

void oval_message_set_text(struct oval_message *, char *);
void oval_message_set_level(struct oval_message *, oval_message_level_t);

int                  oval_message_iterator_has_more(struct oval_message_iterator *oc_message);
struct oval_message *oval_message_iterator_next    (struct oval_message_iterator *oc_message);
void                 oval_message_iterator_free    (struct oval_message_iterator *oc_message);
char                   *oval_message_get_text (struct oval_message *message);
oval_message_level_t    oval_message_get_level(struct oval_message *message);
const char*   oval_operator_get_text(oval_operator_t);
oval_family_t oval_subtype_get_family(oval_subtype_t);
const char*   oval_subtype_get_text(oval_subtype_t);
const char*   oval_family_get_text(oval_family_t);

const char *oval_check_get_text(oval_check_t);
const char *oval_existence_get_text(oval_existence_t);
const char *oval_affected_family_get_text(oval_affected_family_t);
const char *oval_datatype_get_text(oval_datatype_t);
const char *oval_operation_get_text(oval_operation_t);
const char *oval_set_operation_get_text(oval_setobject_operation_t);
const char *oval_datetime_format_get_text(oval_datetime_format_t);
const char *oval_arithmetic_operation_get_text(oval_arithmetic_operation_t);

/* oval_system_characteristics.h */

typedef enum {
        SYSCHAR_FLAG_UNKNOWN         = 0,
        SYSCHAR_FLAG_ERROR           = 1,
        SYSCHAR_FLAG_COMPLETE        = 2,
        SYSCHAR_FLAG_INCOMPLETE      = 3,
        SYSCHAR_FLAG_DOES_NOT_EXIST  = 4,
        SYSCHAR_FLAG_NOT_COLLECTED   = 5,
        SYSCHAR_FLAG_NOT_APPLICABLE  = 6
} oval_syschar_collection_flag_t;

/// System characteristics status
typedef enum{
        SYSCHAR_STATUS_UNKNOWN        = 0,
        SYSCHAR_STATUS_ERROR          = 1,
        SYSCHAR_STATUS_EXISTS         = 2,
        SYSCHAR_STATUS_DOES_NOT_EXIST = 3,
        SYSCHAR_STATUS_NOT_COLLECTED  = 4
} oval_syschar_status_t;
struct oval_sysint;
struct oval_sysint_iterator;
struct oval_sysinfo;
struct oval_sysinfo_iterator;
struct oval_sysdata;
struct oval_sysdata_iterator;
struct oval_sysitem;
struct oval_sysitem_iterator;
struct oval_syschar;
struct oval_syschar_iterator;

int                 oval_sysint_iterator_has_more(struct oval_sysint_iterator *);
struct oval_sysint *oval_sysint_iterator_next    (struct oval_sysint_iterator *);
void                oval_sysint_iterator_free    (struct oval_sysint_iterator *);
char *oval_sysint_get_name       (struct oval_sysint *);
char *oval_sysint_get_ip_address (struct oval_sysint *);
char *oval_sysint_get_mac_address(struct oval_sysint *);

int                  oval_sysinfo_iterator_has_more(struct oval_sysinfo_iterator *);
struct oval_sysinfo *oval_sysinfo_iterator_next    (struct oval_sysinfo_iterator *);
void                 oval_sysinfo_iterator_free    (struct oval_sysinfo_iterator *);
char                        *oval_sysinfo_get_os_name          (struct oval_sysinfo *);
char                        *oval_sysinfo_get_os_version       (struct oval_sysinfo *);
char                        *oval_sysinfo_get_os_architecture  (struct oval_sysinfo *);
char                        *oval_sysinfo_get_primary_host_name(struct oval_sysinfo *);
struct oval_sysint_iterator *oval_sysinfo_get_interfaces       (struct oval_sysinfo *);

int                  oval_sysdata_iterator_has_more(struct oval_sysdata_iterator *);
struct oval_sysdata *oval_sysdata_iterator_next    (struct oval_sysdata_iterator *);
void                 oval_sysdata_iterator_free    (struct oval_sysdata_iterator *);
oval_subtype_t                oval_sysdata_get_subtype      (struct oval_sysdata *);
char                         *oval_sysdata_get_id           (struct oval_sysdata *);
oval_syschar_status_t         oval_sysdata_get_status       (struct oval_sysdata *);
struct oval_sysitem_iterator *oval_sysdata_get_items        (struct oval_sysdata *);
char                         *oval_sysdata_get_message      (struct oval_sysdata *);
oval_message_level_t          oval_sysdata_get_message_level(struct oval_sysdata *);

int                  oval_sysitem_iterator_has_more(struct oval_sysitem_iterator *);
struct oval_sysitem *oval_sysitem_iterator_next    (struct oval_sysitem_iterator *);
void                 oval_sysitem_iterator_free    (struct oval_sysitem_iterator *);
char                    *oval_sysitem_get_name    (struct oval_sysitem *);
char                    *oval_sysitem_get_value   (struct oval_sysitem *);
oval_syschar_status_t    oval_sysitem_get_status  (struct oval_sysitem *);
oval_datatype_t          oval_sysitem_get_datatype(struct oval_sysitem *);
int                      oval_sysitem_get_mask    (struct oval_sysitem *);

int                  oval_syschar_iterator_has_more(struct oval_syschar_iterator *);
struct oval_syschar *oval_syschar_iterator_next    (struct oval_syschar_iterator *);
void                 oval_syschar_iterator_free    (struct oval_syschar_iterator *);
oval_syschar_collection_flag_t         oval_syschar_get_flag             (struct oval_syschar *);
struct oval_message_iterator          *oval_syschar_get_messages         (struct oval_syschar *);
struct oval_sysinfo                   *oval_syschar_get_sysinfo          (struct oval_syschar *);
struct oval_object                    *oval_syschar_get_object           (struct oval_syschar *);
struct oval_variable_binding_iterator *oval_syschar_get_variable_bindings(struct oval_syschar *);
struct oval_sysdata_iterator          *oval_syschar_sysdata              (struct oval_syschar *);

const char *oval_syschar_collection_flag_get_text(oval_syschar_collection_flag_t flag);

/* oval_results.h */

typedef enum {
        OVAL_RESULT_INVALID        = 0,
        OVAL_RESULT_TRUE           = 1,
        OVAL_RESULT_FALSE          = 2,
        OVAL_RESULT_UNKNOWN        = 3,
        OVAL_RESULT_ERROR          = 4,
        OVAL_RESULT_NOT_EVALUATED  = 5,
        OVAL_RESULT_NOT_APPLICABLE = 6
} oval_result_t;

typedef enum {
        OVAL_DIRECTIVE_CONTENT_UNKNOWN = 0,
        OVAL_DIRECTIVE_CONTENT_THIN    = 1,
        OVAL_DIRECTIVE_CONTENT_FULL    = 2
} oval_result_directive_content_t;


struct oval_result_definition;
struct oval_result_definition_iterator;

struct oval_result_item;
struct oval_result_item_iterator;

struct oval_result_test;
struct oval_result_test_iterator;

struct oval_result_criteria_node;
struct oval_result_criteria_node_iterator;

struct oval_result_directives;

struct oval_result_system;
struct oval_result_system_iterator;

int                        oval_result_system_iterator_has_more(struct oval_result_system_iterator *);
struct oval_result_system *oval_result_system_iterator_next    (struct oval_result_system_iterator *);
void                       oval_result_system_iterator_free    (struct oval_result_system_iterator *);

struct oval_result_definition_iterator *oval_result_system_get_definitions  (struct oval_result_system *);
struct oval_result_test_iterator       *oval_result_system_get_tests        (struct oval_result_system *);
struct oval_syschar_model              *oval_result_system_get_syschar_model(struct oval_result_system *);
struct oval_sysinfo                    *oval_result_system_get_sysinfo      (struct oval_result_system *);

void oval_result_system_add_definition_(struct oval_result_system *, struct oval_result_definition *);
void oval_result_system_add_test       (struct oval_result_system *, struct oval_result_test *);

int                            oval_result_definition_iterator_has_more(struct oval_result_definition_iterator *);
struct oval_result_definition *oval_result_definition_iterator_next    (struct oval_result_definition_iterator *);
void                           oval_result_definition_iterator_free    (struct oval_result_definition_iterator *);

struct oval_definition           *oval_result_definition_get_definition(struct oval_result_definition *);
struct oval_result_system        *oval_result_definition_get_system    (struct oval_result_definition *);
int                               oval_result_definition_get_instance  (struct oval_result_definition *);
oval_result_t                     oval_result_definition_get_result    (struct oval_result_definition *);
struct oval_message_iterator     *oval_result_definition_messages      (struct oval_result_definition *);
struct oval_result_criteria_node *oval_result_definition_criteria      (struct oval_result_definition *);

void oval_result_definition_set_result  (struct oval_result_definition *, oval_result_t);
void oval_result_definition_set_instance(struct oval_result_definition *, int);
void oval_result_definition_set_criteria(struct oval_result_definition *, struct oval_result_criteria_node *);
void oval_result_definition_add_message (struct oval_result_definition *, struct oval_message *);

int                      oval_result_item_iterator_has_more(struct oval_result_item_iterator *);
struct oval_result_item *oval_result_item_iterator_next    (struct oval_result_item_iterator *);
void                     oval_result_item_iterator_free    (struct oval_result_item_iterator *);

struct oval_sysdata          *oval_result_item_get_sysdata (struct oval_result_item *);
oval_result_t                 oval_result_item_get_result  (struct oval_result_item *);
struct oval_message_iterator *oval_result_item_get_messages(struct oval_result_item *);

int                      oval_result_test_iterator_has_more(struct oval_result_test_iterator *);
struct oval_result_test *oval_result_test_iterator_next    (struct oval_result_test_iterator *);
void                     oval_result_test_iterator_free    (struct oval_result_test_iterator *);

struct oval_test                      *oval_result_test_get_test    (struct oval_result_test *);
struct oval_result_system             *oval_result_test_get_system  (struct oval_result_test *);
oval_result_t                          oval_result_test_get_result  (struct oval_result_test *);
int                                    oval_result_test_get_instance(struct oval_result_test *);
struct oval_message                   *oval_result_test_get_message (struct oval_result_test *);
struct oval_result_item_iterator      *oval_result_test_get_items   (struct oval_result_test *);
struct oval_variable_binding_iterator *oval_result_test_get_bindings(struct oval_result_test *);

int                               oval_result_criteria_node_iterator_has_more(struct oval_result_criteria_node_iterator *);
struct oval_result_criteria_node *oval_result_criteria_node_iterator_next    (struct oval_result_criteria_node_iterator *);
void                              oval_result_criteria_node_iterator_free    (struct oval_result_criteria_node_iterator *);

oval_criteria_node_type_t                  oval_result_criteria_node_get_type    (struct oval_result_criteria_node *);
oval_result_t                              oval_result_criteria_node_get_result  (struct oval_result_criteria_node *);
bool                                       oval_result_criteria_node_get_negate  (struct oval_result_criteria_node *);
oval_operator_t                            oval_result_criteria_node_get_operator(struct oval_result_criteria_node *);//type==NODETYPE_CRITERIA
struct oval_result_criteria_node_iterator *oval_result_criteria_node_get_subnodes(struct oval_result_criteria_node *);//type==NODETYPE_CRITERIA
struct oval_result_test                   *oval_result_criteria_node_get_test    (struct oval_result_criteria_node *);//type==NODETYPE_CRITERION
struct oval_result_definition             *oval_result_criteria_node_get_extends (struct oval_result_criteria_node *);//type==NODETYPE_EXTENDDEF

bool                               oval_result_directive_get_reported(struct oval_result_directives *, oval_result_t);
oval_result_directive_content_t    oval_result_directive_get_content (struct oval_result_directives *, oval_result_t);

void oval_result_directive_set_reported(struct oval_result_directives *, oval_result_t, bool);
void oval_result_directive_set_content (struct oval_result_directives *, oval_result_t, oval_result_directive_content_t);

const char * oval_result_get_text(oval_result_t);
