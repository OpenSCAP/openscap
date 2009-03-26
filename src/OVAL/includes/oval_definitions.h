#ifndef OVAL_DEFINITIONS
#define OVAL_DEFINITIONS

	typedef enum{
		FAMILY_UNKNOWN     = 0,
		FAMILY_AIX         = 1000,
		FAMILY_APACHE      = 2000,
		FAMILY_CATOS       = 3000,
		FAMILY_ESX         = 4000,
		FAMILY_FREEBSD     = 5000,
		FAMILY_HPUX        = 6000,
		FAMILY_INDEPENDENT = 7000,
		FAMILY_IOS         = 8000,
		FAMILY_LINUX       = 9000,
		FAMILY_MACOS       = 10000,
		FAMILY_PIXOS       = 11000,
		FAMILY_SOLARIS     = 12000,
		FAMILY_UNIX        = 13000,
		FAMILY_WINDOWS     = 14000
	} oval_family_enum;

	typedef enum{
		OVAL_SUBTYPE_UNKNOWN = 0
	} oval_subtype_enum;

	typedef enum {
		AIX_FILESET = FAMILY_AIX+1,
		AIX_FIX     = FAMILY_AIX+2,
		AIX_OSLEVEL = FAMILY_AIX+3
	}oval_aix_subtype_enum;

	typedef enum {
		APACHE_HTTPD   = FAMILY_APACHE+1,
		APACHE_VERSION = FAMILY_APACHE+2
	}oval_apache_subtype_enum;

	typedef enum{
		CATOS_LINE       = FAMILY_CATOS+1,
		CATOS_MODULE     = FAMILY_CATOS+2,
		CATOS_VERSION_55 = FAMILY_CATOS+3,
		CATOS_VERSION    = FAMILY_CATOS+4
	} oval_catos_subtype_enum;

	typedef enum {
		ESX_PATCH   = FAMILY_ESX+1,
		ESX_VERSION = FAMILY_ESX+2
	} oval_esx_subtype_enum;

	typedef enum{
		FREEBSD_PORT_INFO = FAMILY_FREEBSD+1
	}oval_freebsd_subtype_enum;

	typedef enum {
		HPUX_GETCONF          = FAMILY_HPUX+1,
		HPUX_PATCH_53         = FAMILY_HPUX+2,
		HPUX_PATCH            = FAMILY_HPUX+3,
		HPUX_SOFTWARE_LIST    = FAMILY_HPUX+4,
		HPUX_TRUSTED_ACCOUNTS = FAMILY_HPUX+5
	} oval_hpux_subtype_enum;

	typedef enum{
		INDEPENDENT_FAMILY               = FAMILY_INDEPENDENT+1,
		INDEPENDENT_FILE_MD5             = FAMILY_INDEPENDENT+2,
		INDEPENDENT_FILE_HASH            = FAMILY_INDEPENDENT+3,
		INDEPENDENT_ENVIRONMENT_VARIABLE = FAMILY_INDEPENDENT+4,
		INDEPENDENT_SQL                  = FAMILY_INDEPENDENT+5,
		INDEPENDENT_TEXT_FILE_CONTENT    = FAMILY_INDEPENDENT+6,
		INDEPENDENT_UNKNOWN              = FAMILY_INDEPENDENT+7,
		INDEPENDENT_VARIABLE             = FAMILY_INDEPENDENT+8,
		INDEPENDENT_XML_FILE_CONTENT     = FAMILY_INDEPENDENT+9
	} oval_independent_subtype_enum;

	typedef enum {
		IOS_GLOBAL     = FAMILY_IOS+1,
		IOS_INTERFACE  = FAMILY_IOS+2,
		IOS_LINE       = FAMILY_IOS+3,
		IOS_SNMP       = FAMILY_IOS+4,
		IOS_TCLSH      = FAMILY_IOS+5,
		IOS_VERSION_55 = FAMILY_IOS+6
	} oval_ios_subtype_enum;

	typedef enum {
		LINUX_DPKG_INFO               = FAMILY_LINUX+1,
		LINUX_INET_LISTENING_SERVERS  = FAMILY_LINUX+2,
		LINUX_RPM_INFO                = FAMILY_LINUX+3,
		LINUX_SLACKWARE_PKG_INFO_TEST = FAMILY_LINUX+4
	} oval_linux_subtype_enum;

	typedef enum {
		MACOS_ACCOUNT_INFO           = FAMILY_MACOS+1,
		MACOS_INET_LISTENING_SERVERS = FAMILY_MACOS+2,
		MACOS_NVRAM_INFO             = FAMILY_MACOS+3,
		MACOS_PWPOLICY               = FAMILY_MACOS+4
	}oval_macos_subtype_enum;

	typedef enum {
		PIXOS_LINE    = FAMILY_PIXOS+1,
		PIXOS_VERSION = FAMILY_PIXOS+2
	}oval_pixos_subtype_enum;

	typedef enum{
		SOLARIS_ISAINFO = FAMILY_SOLARIS+1,
		SOLARIS_PACKAGE = FAMILY_SOLARIS+2,
		SOLARIS_PATCH   = FAMILY_SOLARIS+3,
		SOLARIS_SMF     = FAMILY_SOLARIS+4
	}oval_solaris_subtype_enum;

	typedef enum{
		UNIX_FILE      = FAMILY_UNIX+1,
		UNIX_INETD     = FAMILY_UNIX+2,
		UNIX_INTERFACE = FAMILY_UNIX+3,
		UNIX_PASSWORD  = FAMILY_UNIX+4,
		UNIX_PROCESS   = FAMILY_UNIX+5,
		UNIX_RUNLEVEL  = FAMILY_UNIX+6,
		UNIX_SCCS      = FAMILY_UNIX+7,
		UNIX_SHADOW    = FAMILY_UNIX+8,
		UNIX_UNAME     = FAMILY_UNIX+9,
		UNIX_XINETD    = FAMILY_UNIX+10
	}oval_unix_subtype_enum;

	typedef enum{
		WINDOWS_ACCESS_TOKEN                  = FAMILY_WINDOWS+1,
		WINDOWS_ACTIVE_DIRECTORY              = FAMILY_WINDOWS+2,
		WINDOWS_AUDIT_EVENT_POLICY            = FAMILY_WINDOWS+3,
		WINDOWS_AUDIT_EVENT_SUBCATEGORIES     = FAMILY_WINDOWS+4,
		WINDOWS_FILE                          = FAMILY_WINDOWS+5,
		WINDOWS_FILE_AUDITED_PERMISSIONS_53   = FAMILY_WINDOWS+6,
		WINDOWS_FILE_AUDITED_PERMISSIONS      = FAMILY_WINDOWS+7,
		WINDOWS_FILE_EFFECTIVE_RIGHTS_53      = FAMILY_WINDOWS+8,
		WINDOWS_FILE_EFFECTIVE_RIGHTS         = FAMILY_WINDOWS+9,
		WINDOWS_GROUP                         = FAMILY_WINDOWS+10,
		WINDOWS_GROUP_SID                     = FAMILY_WINDOWS+11,
		WINDOWS_INTERFACE                     = FAMILY_WINDOWS+12,
		WINDOWS_LOCKOUT_POLICY                = FAMILY_WINDOWS+13,
		WINDOWS_METABASE                      = FAMILY_WINDOWS+14,
		WINDOWS_PASSWORD_POLICY               = FAMILY_WINDOWS+15,
		WINDOWS_PORT                          = FAMILY_WINDOWS+16,
		WINDOWS_PRINTER_EFFECTIVE_RIGHTS      = FAMILY_WINDOWS+17,
		WINDOWS_PROCESS                       = FAMILY_WINDOWS+18,
		WINDOWS_REGISTRY                      = FAMILY_WINDOWS+19,
		WINDOWS_REGKEY_AUDITED_PERMISSIONS_53 = FAMILY_WINDOWS+20,
		WINDOWS_REGKEY_AUDITED_PERMISSIONS    = FAMILY_WINDOWS+21,
		WINDOWS_REGKEY_EFFECTIVE_RIGHTS_53    = FAMILY_WINDOWS+22,
		WINDOWS_REGKEY_EFFECTIVE_RIGHTS       = FAMILY_WINDOWS+23,
		WINDOWS_SHARED_RESOURCE               = FAMILY_WINDOWS+24,
		WINDOWS_SID                           = FAMILY_WINDOWS+25,
		WINDOWS_SID_SID                       = FAMILY_WINDOWS+26,
		WINDOWS_USER_ACCESS_CONTROL           = FAMILY_WINDOWS+27,
		WINDOWS_USER                          = FAMILY_WINDOWS+28,
		WINDOWS_USER_SID_55                   = FAMILY_WINDOWS+29,
		WINDOWS_USER_SID                      = FAMILY_WINDOWS+30,
		WINDOWS_VOLUME                        = FAMILY_WINDOWS+31,
		WINDOWS_WMI                           = FAMILY_WINDOWS+32,
		WINDOWS_WUA_UPDATE_SEARCHER           = FAMILY_WINDOWS+33
	}oval_windows_subtype_enum;

	typedef enum {
		AFCFML_UNKNOWN,
		AFCFML_CATOS,
		AFCFML_IOS,
		AFCFML_MACOS,
		AFCFML_PIXOS,
		AFCFML_UNDEFINED,
		AFCFML_UNIX,
		AFCFML_WINDOWS
	} oval_affected_family_enum;

	typedef enum {
		NODETYPE_UNKNOWN   = 0,
		NODETYPE_CRITERIA  = 1,
		NODETYPE_CRITERION = 2,
		NODETYPE_EXTENDDEF = 3
	} oval_criteria_node_type_enum;

	typedef enum {
		OPERATOR_UNKNOWN,
		OPERATOR_AND,
		OPERATOR_ONE,
		OPERATOR_OR,
		OPERATOR_XOR
	} oval_operator_enum;

	typedef enum {
		CLASS_UNKNOWN,
		CLASS_COMPLIANCE,
		CLASS_INVENTORY,
		CLASS_MISCELLANEOUS,
		CLASS_PATCH,
		CLASS_VULNERABILITY
	} oval_definition_class_enum;

	typedef enum {
		EXISTENCE_UNKNOWN,
		ALL_EXIST,
		ANY_EXIST,
		AT_LEAST_ONE_EXISTS,
		ONLY_ONE_EXISTS
	} oval_existence_enum;

	typedef enum {
		OVAL_CHECK_UNKNOWN,
		OVAL_CHECK_ALL,
		OVAL_CHECK_AT_LEAST_ONE,
		OVAL_CHECK_NONE_EXIST,
		OVAL_CHECK_NONE_SATISFY,
		OVAL_CHECK_ONLY_ONE
	} oval_check_enum;

	typedef enum{
		OVAL_OBJECTCONTENT_ENTITY,
		OVAL_OBJECTCONTENT_SET
	} oval_object_content_type_enum;

	typedef enum{
		OVAL_ENTITY_TYPE_UNKNOWN,
		OVAL_ENTITY_TYPE_ANY,
		OVAL_ENTITY_TYPE_BINARY,
		OVAL_ENTITY_TYPE_BOOLEAN,
		OVAL_ENTITY_TYPE_FLOAT,
		OVAL_ENTITY_TYPE_INTEGER,
		OVAL_ENTITY_TYPE_STRING,
	}oval_entity_type_enum;

	typedef enum{
		OVAL_DATATYPE_UNKNOWN,
		OVAL_DATATYPE_BINARY,
		OVAL_DATATYPE_BOOLEAN,
		OVAL_DATATYPE_EVR_STRING,
		OVAL_DATATYPE_FILESET_REVISTION,
		OVAL_DATATYPE_FLOAT,
		OVAL_DATATYPE_IOS_VERSION,
		OVAL_DATATYPE_VERSION,
		OVAL_DATATYPE_INTEGER,
		OVAL_DATATYPE_STRING
	}oval_datatype_enum;;

	typedef enum{
		OVAL_ENTITY_VARREF_UNKNOWN,
		OVAL_ENTITY_VARREF_NONE,
		OVAL_ENTITY_VARREF_ATTRIBUTE,
		OVAL_ENTITY_VARREF_ELEMENT
	} oval_entity_varref_type_enum;

	typedef enum{
		OVAL_SET_AGGREGATE,
		OVAL_SET_COLLECTIVE
	}oval_set_type_enum;

	typedef enum{
		OVAL_SET_OPERATION_UNKNOWN,
		OVAL_SET_OPERATION_COMPLEMENT,
		OVAL_SET_OPERATION_INTERSECTION,
		OVAL_SET_OPERATION_UNION
	}oval_set_operation_enum;

	typedef enum{
		OVAL_VARIABLE_UNKNOWN,
		OVAL_VARIABLE_EXTERNAL,
		OVAL_VARIABLE_CONSTANT,
		OVAL_VARIABLE_LOCAL
	}oval_variable_type_enum;

	typedef enum{
		OVAL_COMPONENT_LITERAL,
		OVAL_COMPONENT_OBJECTREF,
		OVAL_COMPONENT_VARREF,
		OVAL_COMPONENT_FUNCTION
	}oval_component_type_enum;

	typedef enum{
		OVAL_FUNCTION_UNKNOWN,
		OVAL_FUNCTION_BEGIN,
		OVAL_FUNCTION_CONCAT,
		OVAL_FUNCTION_END,
		OVAL_FUNCTION_SPLIT,
		OVAL_FUNCTION_SUBSTRING,
		OVAL_FUNCTION_TIMEDIF,
		OVAL_FUNCTION_ESCAPE_REGEX,
		OVAL_FUNCTION_REGEX_CAPTURE,
		OVAL_FUNCTION_ARITHMATIC
	}oval_function_type_enum;

	struct oval_iterator_string_s;

	struct oval_affected_s;
	struct oval_iterator_affected_s;

	struct oval_test_s;
	struct oval_iterator_test_s;

	struct oval_criteria_node_s;
	struct oval_iterator_criteria_node_s;

	struct oval_reference_s;
	struct oval_iterator_reference_s;

	struct oval_definition_s;
	struct oval_iterator_definition_s;

	struct oval_object_s;
	struct oval_iterator_object_s;

	struct oval_state_s;
	struct oval_iterator_state_s;

	struct oval_variable_s;
	struct oval_iterator_variable_s;

	struct oval_variable_binding_s;
	struct oval_iterator_variable_binding_s;

	struct oval_object_content_s;
	struct oval_iterator_object_content_s;

	struct oval_behavior_s;
	struct oval_iterator_behavior_s;

	struct oval_entity_s;
	struct oval_iterator_entity_s;

	struct oval_iterator_set_s;
	struct oval_set_s;

	struct oval_value_s;
	struct oval_iterator_value_s;

	struct oval_component_s;
	struct oval_iterator_component_s;

	int   oval_iterator_string_has_more(struct oval_iterator_string_s*);
	char *oval_iterator_string_next    (struct oval_iterator_string_s*);

	int  oval_iterator_affected_has_more               (struct oval_iterator_affected_s*);
	struct oval_affected_s *oval_iterator_affected_next(struct oval_iterator_affected_s*);

	int  oval_iterator_test_has_more           (struct oval_iterator_test_s*);
	struct oval_test_s *oval_iterator_test_next(struct oval_iterator_test_s*);

	oval_affected_family_enum oval_affected_family       (struct oval_affected_s*);
	struct oval_iterator_string_s *oval_affected_platform(struct oval_affected_s*);
	struct oval_iterator_string_s *oval_affected_product (struct oval_affected_s*);

	int  oval_iterator_criteria_node_has_more                    (struct oval_iterator_criteria_node_s*);
	struct oval_criteria_node_s *oval_iterator_criteria_node_next(struct oval_iterator_criteria_node_s*);

	oval_criteria_node_type_enum oval_criteria_node_type              (struct oval_criteria_node_s*);
	int oval_criteria_node_negate                                     (struct oval_criteria_node_s*);
	char *oval_criteria_node_comment                                  (struct oval_criteria_node_s*);
	oval_operator_enum oval_criteria_node_operator                    (struct oval_criteria_node_s*);//type==NODETYPE_CRITERIA
	struct oval_iterator_criteria_node_s *oval_criteria_node_subnodes (struct oval_criteria_node_s*);//type==NODETYPE_CRITERIA
	struct oval_test_s *oval_criteria_node_test                       (struct oval_criteria_node_s*);//type==NODETYPE_CRITERION
	struct oval_definition_s *oval_criteria_node_definition           (struct oval_criteria_node_s*);//type==NODETYPE_EXTENDDEF

	int  oval_iterator_reference_has_more                (struct oval_iterator_reference_s*);
	struct oval_reference_s *oval_iterator_reference_next(struct oval_iterator_reference_s*);

	char *oval_reference_source(struct oval_reference_s*);
	char *oval_reference_id    (struct oval_reference_s*);
	char *oval_reference_url   (struct oval_reference_s*);

	int  oval_iterator_definition_has_more                 (struct oval_iterator_definition_s*);
	struct oval_definition_s *oval_iterator_definition_next(struct oval_iterator_definition_s*);

	char *oval_definition_id                                    (struct oval_definition_s*);
	int oval_definition_version                                 (struct oval_definition_s*);
	oval_definition_class_enum oval_definition_class            (struct oval_definition_s*);
	int oval_definition_deprecated                              (struct oval_definition_s*);
	char *oval_definition_title                                 (struct oval_definition_s*);
	char *oval_definition_description                           (struct oval_definition_s*);
	struct oval_iterator_affected_s *oval_definition_affected   (struct oval_definition_s*);
	struct oval_iterator_reference_s *oval_definition_reference (struct oval_definition_s*);
	struct oval_criteria_node_s *oval_definition_criteria       (struct oval_definition_s*);

	int  oval_iterator_object_has_more              (struct oval_iterator_object_s*);
	struct oval_object_s *oval_iterator_object_next(struct oval_iterator_object_s*);

	oval_family_enum oval_object_family                              (struct oval_object_s*);
	oval_subtype_enum oval_object_subtype                            (struct oval_object_s*);
	char *oval_object_name                                           (struct oval_object_s*);
	struct oval_iterator_string_s *oval_object_notes                 (struct oval_object_s*);
	char *oval_object_comment                                        (struct oval_object_s*);
	char *oval_object_id                                             (struct oval_object_s*);
	int oval_object_deprecated                                       (struct oval_object_s*);
	int oval_object_version                                          (struct oval_object_s*);
	struct oval_iterator_object_content_s *oval_object_object_content(struct oval_object_s*);
	struct oval_iterator_behavior_s *oval_object_behaviors           (struct oval_object_s*);

	int  oval_iterator_test_has_more            (struct oval_iterator_test_s*);
	struct oval_test_s *oval_iterator_test_next(struct oval_iterator_test_s*);

	oval_family_enum oval_test_family               (struct oval_test_s*);
	oval_subtype_enum oval_test_subtype             (struct oval_test_s*);
	struct oval_iterator_string_s *oval_test_notes  (struct oval_test_s*);
	char *oval_test_comment                         (struct oval_test_s*);
	char *oval_test_id                              (struct oval_test_s*);
	int oval_test_deprecated                        (struct oval_test_s*);
	int oval_test_version                           (struct oval_test_s*);
	oval_existence_enum oval_test_existence         (struct oval_test_s*);
	oval_check_enum oval_test_check                 (struct oval_test_s*);
	struct oval_object_s *oval_test_object          (struct oval_test_s*);
	struct oval_state_s *oval_test_state            (struct oval_test_s*);

	int  oval_iterator_variable_binding_has_more                        (struct oval_iterator_variable_binding_s*);
	struct oval_variable_binding_s *oval_iterator_variable_binding_next (struct oval_iterator_variable_binding_s*);

	struct oval_variable_s *oval_variable_binding_variable(struct oval_variable_binding_s*);
	char *oval_variable_binding_value                     (struct oval_variable_binding_s*);

	int  oval_iterator_object_content_has_more                     (struct oval_iterator_object_content_s*);
	struct oval_object_content_s *oval_iterator_object_content_next(struct oval_iterator_object_content_s*);

	char *oval_object_content_field_name                   (struct oval_object_content_s*);
	oval_object_content_type_enum oval_object_content_type(struct oval_object_content_s*);
	struct oval_entity_s *oval_object_content_entity      (struct oval_object_content_s*);//type == OVAL_OBJECTCONTENT_ENTITY
	oval_check_enum oval_object_content_varCheck          (struct oval_object_content_s*);//type == OVAL_OBJECTCONTENT_ENTITY
	struct oval_set_s *oval_object_content_set            (struct oval_object_content_s*);//type == OVAL_OBJECTCONTENT_SET

	int  oval_iterator_entity_has_more             (struct oval_iterator_entity_s*);
	struct oval_entity_s *oval_iterator_entity_next(struct oval_iterator_entity_s*);

	oval_entity_type_enum oval_entity_type              (struct oval_entity_s*);
	oval_datatype_enum oval_entity_datatype             (struct oval_entity_s*);
	oval_operator_enum oval_entity_operator             (struct oval_entity_s*);
	int oval_entity_mask                                (struct oval_entity_s*);
	oval_entity_varref_type_enum oval_entity_varref_type(struct oval_entity_s*);
	struct oval_variable_s *oval_entity_variable        (struct oval_entity_s*);
	struct oval_value_s *oval_entity_value              (struct oval_entity_s*);

	int  oval_iterator_set_has_more          (struct oval_iterator_set_s*);
	struct oval_set_s *oval_iterator_set_next(struct oval_iterator_set_s*);

	oval_set_type_enum oval_set_type                 (struct oval_set_s*);
	oval_set_operation_enum oval_set_operation       (struct oval_set_s*);
	struct oval_iterator_set_s *oval_set_subsets   (struct oval_set_s*);//type==OVAL_SET_AGGREGATE;
	struct oval_iterator_object_s *oval_set_objects(struct oval_set_s*);//type==OVAL_SET_COLLECTIVE;
	struct oval_iterator_state_s *oval_set_filters (struct oval_set_s*);//type==OVAL_SET_COLLECTIVE;

	int  oval_iterator_behavior_has_more               (struct oval_iterator_behavior_s*);
	struct oval_behavior_s *oval_iterator_behavior_next(struct oval_iterator_behavior_s*);

	struct oval_value_s *oval_behavior_value                    (struct oval_behavior_s*);
	struct oval_iterator_string_s *oval_behavior_attribute_keys(struct oval_behavior_s*);
	struct oval_value_s *oval_behavior_value_for_key             (struct oval_behavior_s*, char *attribute_key);

	int  oval_iterator_value_has_more            (struct oval_iterator_value_s*);
	struct oval_value_s *oval_iterator_value_next(struct oval_iterator_value_s*);

	oval_datatype_enum oval_value_datatype(struct oval_value_s*);
	char *oval_value_text                 (struct oval_value_s*);
	unsigned char *oval_value_binary      (struct oval_value_s*);//datatype==OVAL_DATATYPE_BINARY
	char oval_value_boolean               (struct oval_value_s*);//datatype==OVAL_DATATYPE_BOOLEAN
	float oval_value_float                (struct oval_value_s*);//datatype==OVAL_DATATYPE_FLOAT
	long oval_value_integer               (struct oval_value_s*);//datatype==OVAL_DATATYPE_INTEGER


	int  oval_iterator_state_has_more            (struct oval_iterator_state_s*);
	struct oval_state_s *oval_iterator_state_next(struct oval_iterator_state_s*);

	oval_family_enum oval_state_family             (struct oval_state_s*);
	oval_subtype_enum oval_state_subtype           (struct oval_state_s*);
	char *oval_state_name                          (struct oval_state_s*);
	struct oval_iterator_string_s *oval_state_notes(struct oval_state_s*);
	char *oval_state_comment                       (struct oval_state_s*);
	char *oval_state_id                            (struct oval_state_s*);
	int oval_state_deprecated                      (struct oval_state_s*);
	int oval_state_version                         (struct oval_state_s*);

	int  oval_iterator_variable_has_more               (struct oval_iterator_variable_s*);
	struct oval_variable_s *oval_iterator_variable_next(struct oval_iterator_variable_s*);

	char *oval_variable_id                             (struct oval_variable_s*);
	oval_variable_type_enum oval_variable_type         (struct oval_variable_s*);
	oval_datatype_enum oval_variable_datatype          (struct oval_variable_s*);
	struct oval_iterator_value_s *oval_variable_values (struct oval_variable_s*);//type==OVAL_VARIABLE_CONSTANT
	struct oval_component_s *oval_variable_component   (struct oval_variable_s*);//type==OVAL_VARIABLE_LOCAL

	int  oval_iterator_component_has_more                (struct oval_iterator_component_s*);
	struct oval_component_s *oval_iterator_component_next(struct oval_iterator_component_s*);

	oval_component_type_enum oval_component_type                        (struct oval_component_s*);
	struct oval_value_s *oval_component_literal_value                   (struct oval_component_s*);              //type==OVAL_COMPONENT_LITERAL
	struct oval_object_s *oval_component_object                         (struct oval_component_s*);              //type==OVAL_COMPONENT_OBJECTREF
	char *oval_component_object_field                                   (struct oval_component_s*);              //type==OVAL_COMPONENT_OBJECTREF
	struct oval_variable_s *oval_component_variable                     (struct oval_component_s*);              //type==OVAL_COMPONENT_VARREF
	oval_function_type_enum oval_component_function_type                (struct oval_component_s*);              //type==OVAL_COMPONENT_FUNCTION
	struct oval_iterator_component_s *oval_component_function_components(struct oval_component_s*);              //type==OVAL_COMPONENT_FUNCTION
	struct oval_iterator_string_s *oval_component_function_keys         (struct oval_component_s*);              //type==OVAL_COMPONENT_FUNCTION
	struct oval_value_s *oval_component_function_value                  (struct oval_component_s*, char *key_s); //type==OVAL_COMPONENT_FUNCTION

#endif
