/*
 * oval_enumerations.c
 *
 *  Created on: Mar 14, 2009
 *      Author: Compaq_Administrator
 */
#include "includes/oval_definitions_impl.h"
#include "includes/oval_string_map_impl.h"

oval_affected_family_enum    oval_affected_family_parse   (xmlTextReaderPtr);
oval_definition_class_enum   oval_definition_class_parse  (xmlTextReaderPtr);
oval_existence_enum          oval_existence_parse         (xmlTextReaderPtr);
oval_entity_type_enum        oval_entity_type_parse       (xmlTextReaderPtr);
oval_datatype_enum           oval_datatype_parse          (xmlTextReaderPtr);
oval_entity_varref_type_enum oval_entity_varref_type_parse(xmlTextReaderPtr);
oval_set_type_enum           oval_set_type_parse          (xmlTextReaderPtr);
oval_set_operation_enum      oval_set_operation_parse     (xmlTextReaderPtr);
oval_variable_type_enum      oval_variable_type_parse     (xmlTextReaderPtr);
oval_component_type_enum     oval_component_type_parse    (xmlTextReaderPtr);
oval_function_type_enum      oval_function_type_parse     (xmlTextReaderPtr);

typedef struct _oval_enumeration_object_s{
	int enumval;
} _oval_enumeration_object_t;

_oval_enumeration_object_t *_oval_enumeration_object(int value){
	_oval_enumeration_object_t *starval = (_oval_enumeration_object_t*)malloc(sizeof(_oval_enumeration_object_t));
	starval->enumval = value;
	return starval;
}

_oval_enumeration_object_t *_oval_enumeration_attval(xmlTextReaderPtr reader, char *attname, struct oval_string_map_s *map){
	char *attval = xmlTextReaderGetAttribute(reader, attname);
	_oval_enumeration_object_t *starval = NULL;
	if(attval!=NULL){
		starval = (_oval_enumeration_object_t*)oval_string_map_get_value(map,attval);
		if(starval==NULL){
			printf("NOTICE::@%s=%s::unknown value (line %d)\n"
					,attname, attval, xmlTextReaderGetParserLineNumber(reader));
		}
		free(attval);
	}
	return starval;
}

struct oval_string_map_s *_oval_check_map = NULL;
oval_check_enum oval_check_parse(xmlTextReaderPtr reader, char* attname){
	if(_oval_check_map==NULL){
		_oval_check_map = oval_string_map_new();
		oval_string_map_put(_oval_check_map, "all"         , _oval_enumeration_object(OVAL_CHECK_ALL));
		oval_string_map_put(_oval_check_map, "at_least_one", _oval_enumeration_object(OVAL_CHECK_AT_LEAST_ONE));
		oval_string_map_put(_oval_check_map, "none_exist"  , _oval_enumeration_object(OVAL_CHECK_NONE_EXIST));
		oval_string_map_put(_oval_check_map, "none_satisfy", _oval_enumeration_object(OVAL_CHECK_NONE_SATISFY));
		oval_string_map_put(_oval_check_map, "only_one"    , _oval_enumeration_object(OVAL_CHECK_ONLY_ONE));
	}
	_oval_enumeration_object_t *starval = _oval_enumeration_attval(reader, attname,_oval_check_map);
	if(starval==NULL){
		char* attval = xmlTextReaderGetAttribute(reader,attname);
		if(attval!=NULL){
			printf("NOTICE:oval_check_parse:@%s = %s not resolved\n",attname,attval);
			free(attval);
		}
	}
	int returns = (starval==NULL)?OPERATOR_AND:starval->enumval;
	return returns;
}

struct oval_string_map_s *_oval_existence_map = NULL;
oval_existence_enum oval_existence_parse(xmlTextReaderPtr reader){
	if(_oval_existence_map==NULL){
		_oval_existence_map = oval_string_map_new();
		oval_string_map_put(_oval_existence_map, "all_exist"          , _oval_enumeration_object(ALL_EXIST));
		oval_string_map_put(_oval_existence_map, "any_exist"          , _oval_enumeration_object(ANY_EXIST));
		oval_string_map_put(_oval_existence_map, "at_least_one_exists", _oval_enumeration_object(AT_LEAST_ONE_EXISTS));
		oval_string_map_put(_oval_existence_map, "only_one_exists"    , _oval_enumeration_object(ONLY_ONE_EXISTS));
	}
	_oval_enumeration_object_t *starval = _oval_enumeration_attval(reader, "check_existence",_oval_existence_map);
	if(starval==NULL){
		char* attval = xmlTextReaderGetAttribute(reader,"check_existence");
		printf("NOTICE:oval_existence_parse:@check_existence = %s not resolved\n",attval);
		free(attval);
	}
	int returns = (starval==NULL)?OPERATOR_AND:starval->enumval;
	return returns;
}

struct oval_string_map_s *_oval_operator_map = NULL;
oval_operator_enum oval_operator_parse(xmlTextReaderPtr reader){
	if(_oval_operator_map==NULL){
		_oval_operator_map = oval_string_map_new();
		oval_string_map_put(_oval_operator_map, "and", _oval_enumeration_object(OPERATOR_AND));
		oval_string_map_put(_oval_operator_map, "one", _oval_enumeration_object(OPERATOR_ONE));
		oval_string_map_put(_oval_operator_map, "or" , _oval_enumeration_object(OPERATOR_OR ));
		oval_string_map_put(_oval_operator_map, "xor", _oval_enumeration_object(OPERATOR_XOR));
	}
	_oval_enumeration_object_t *starval = _oval_enumeration_attval(reader, "operator",_oval_operator_map);
	int returns = (starval==NULL)?OPERATOR_AND:starval->enumval;
	return returns;
}

struct oval_string_map_s *_oval_family_map = NULL;
oval_family_enum oval_family_parse(xmlTextReaderPtr reader){
	if(_oval_family_map==NULL){
		_oval_family_map = oval_string_map_new();
		oval_string_map_put(_oval_family_map, "aix"        , _oval_enumeration_object(FAMILY_AIX));
		oval_string_map_put(_oval_family_map, "apache"     , _oval_enumeration_object(FAMILY_APACHE));
		oval_string_map_put(_oval_family_map, "catos"      , _oval_enumeration_object(FAMILY_CATOS));
		oval_string_map_put(_oval_family_map, "esx"        , _oval_enumeration_object(FAMILY_ESX));
		oval_string_map_put(_oval_family_map, "freebsd"    , _oval_enumeration_object(FAMILY_FREEBSD));
		oval_string_map_put(_oval_family_map, "hpux"       , _oval_enumeration_object(FAMILY_HPUX));
		oval_string_map_put(_oval_family_map, "independent", _oval_enumeration_object(FAMILY_INDEPENDENT));
		oval_string_map_put(_oval_family_map, "ios"        , _oval_enumeration_object(FAMILY_IOS));
		oval_string_map_put(_oval_family_map, "linux"      , _oval_enumeration_object(FAMILY_LINUX));
		oval_string_map_put(_oval_family_map, "macos"      , _oval_enumeration_object(FAMILY_MACOS));
		oval_string_map_put(_oval_family_map, "pixos"      , _oval_enumeration_object(FAMILY_PIXOS));
		oval_string_map_put(_oval_family_map, "solaris"    , _oval_enumeration_object(FAMILY_SOLARIS));
		oval_string_map_put(_oval_family_map, "unix"       , _oval_enumeration_object(FAMILY_UNIX));
		oval_string_map_put(_oval_family_map, "windows"    , _oval_enumeration_object(FAMILY_WINDOWS));
	}
	char* namespace = xmlTextReaderNamespaceUri(reader);
	int index;for(index=strlen(namespace);namespace[index]!='#';index--);
	char* family_text = namespace+index+1;
	_oval_enumeration_object_t *starval = oval_string_map_get_value(_oval_family_map,family_text);
	oval_family_enum family;
	if(starval==NULL){
		printf("NOTICE:oval_family_parse:%s not a registered family\n",family_text);
		family = FAMILY_UNKNOWN;
	}else family = starval->enumval;
	free(namespace);
	return family;
}

struct oval_string_map_s *_oval_subtype_aix_map = NULL;
_oval_enumeration_object_t *_oval_subtype_aix(char* type_text){
	if(_oval_subtype_aix_map ==NULL){
		_oval_subtype_aix_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_aix_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_apache_map = NULL;
_oval_enumeration_object_t *_oval_subtype_apache(char* type_text){
	if(_oval_subtype_apache_map ==NULL){
		_oval_subtype_apache_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_apache_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_catos_map = NULL;
_oval_enumeration_object_t *_oval_subtype_catos(char* type_text){
	if(_oval_subtype_catos_map ==NULL){
		_oval_subtype_catos_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_catos_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_esx_map = NULL;
_oval_enumeration_object_t *_oval_subtype_esx(char* type_text){
	if(_oval_subtype_esx_map ==NULL){
		_oval_subtype_esx_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_esx_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_freebsd_map = NULL;
_oval_enumeration_object_t *_oval_subtype_freebsd(char* type_text){
	if(_oval_subtype_freebsd_map ==NULL){
		_oval_subtype_freebsd_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_freebsd_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_hpux_map = NULL;
_oval_enumeration_object_t *_oval_subtype_hpux(char* type_text){
	if(_oval_subtype_hpux_map ==NULL){
		_oval_subtype_hpux_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_hpux_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_independent_map = NULL;
_oval_enumeration_object_t *_oval_subtype_independent(char* type_text){
	if(_oval_subtype_independent_map ==NULL){
		_oval_subtype_independent_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_independent_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_ios_map = NULL;
_oval_enumeration_object_t *_oval_subtype_ios(char* type_text){
	if(_oval_subtype_ios_map ==NULL){
		_oval_subtype_ios_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_ios_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_linux_map = NULL;
_oval_enumeration_object_t *_oval_subtype_linux(char* type_text){
	if(_oval_subtype_linux_map ==NULL){
		_oval_subtype_linux_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_linux_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_macos_map = NULL;
_oval_enumeration_object_t *_oval_subtype_macos(char* type_text){
	if(_oval_subtype_macos_map ==NULL){
		_oval_subtype_macos_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_macos_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_pixos_map = NULL;
_oval_enumeration_object_t *_oval_subtype_pixos(char* type_text){
	if(_oval_subtype_pixos_map ==NULL){
		_oval_subtype_pixos_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_pixos_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_solaris_map = NULL;
_oval_enumeration_object_t *_oval_subtype_solaris(char* type_text){
	if(_oval_subtype_solaris_map ==NULL){
		_oval_subtype_solaris_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = (_oval_enumeration_object_t*)oval_string_map_get_value(_oval_subtype_solaris_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_unix_map = NULL;
_oval_enumeration_object_t *_oval_subtype_unix(char* type_text){
	if(_oval_subtype_unix_map ==NULL){
		_oval_subtype_unix_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_unix_map, type_text);
	return vstar;
}

struct oval_string_map_s *_oval_subtype_windows_map = NULL;
_oval_enumeration_object_t *_oval_subtype_windows(char* type_text){
	if(_oval_subtype_windows_map ==NULL){
		_oval_subtype_windows_map = oval_string_map_new();
		oval_string_map_put(_oval_subtype_windows_map, "access_token"                 , _oval_enumeration_object(WINDOWS_ACCESS_TOKEN));
		oval_string_map_put(_oval_subtype_windows_map, "active_directory"             , _oval_enumeration_object(WINDOWS_ACTIVE_DIRECTORY));
		oval_string_map_put(_oval_subtype_windows_map, "audit_event_policy"           , _oval_enumeration_object(WINDOWS_AUDIT_EVENT_POLICY));
		oval_string_map_put(_oval_subtype_windows_map, "audit_event_subcatagories"    , _oval_enumeration_object(WINDOWS_AUDIT_EVENT_SUBCATEGORIES));
		oval_string_map_put(_oval_subtype_windows_map, "file"                         , _oval_enumeration_object(WINDOWS_FILE));
		oval_string_map_put(_oval_subtype_windows_map, "file_audited_permissions_53"  , _oval_enumeration_object(WINDOWS_FILE_AUDITED_PERMISSIONS_53));
		oval_string_map_put(_oval_subtype_windows_map, "file_audited_permissions"     , _oval_enumeration_object(WINDOWS_FILE_AUDITED_PERMISSIONS));
		oval_string_map_put(_oval_subtype_windows_map, "file_effective_rights_53"     , _oval_enumeration_object(WINDOWS_FILE_EFFECTIVE_RIGHTS_53));
		oval_string_map_put(_oval_subtype_windows_map, "file_effective_rights"        , _oval_enumeration_object(WINDOWS_FILE_EFFECTIVE_RIGHTS));
		oval_string_map_put(_oval_subtype_windows_map, "group"                        , _oval_enumeration_object(WINDOWS_GROUP));
		oval_string_map_put(_oval_subtype_windows_map, "group_sid"                    , _oval_enumeration_object(WINDOWS_GROUP_SID));
		oval_string_map_put(_oval_subtype_windows_map, "interface"                    , _oval_enumeration_object(WINDOWS_INTERFACE));
		oval_string_map_put(_oval_subtype_windows_map, "lockout_policy"               , _oval_enumeration_object(WINDOWS_LOCKOUT_POLICY));
		oval_string_map_put(_oval_subtype_windows_map, "metabase"                     , _oval_enumeration_object(WINDOWS_METABASE));
		oval_string_map_put(_oval_subtype_windows_map, "password_policy"              , _oval_enumeration_object(WINDOWS_PASSWORD_POLICY));
		oval_string_map_put(_oval_subtype_windows_map, "port"                         , _oval_enumeration_object(WINDOWS_PORT));
		oval_string_map_put(_oval_subtype_windows_map, "printer_effective_rights"     , _oval_enumeration_object(WINDOWS_PRINTER_EFFECTIVE_RIGHTS));
		oval_string_map_put(_oval_subtype_windows_map, "process"                      , _oval_enumeration_object(WINDOWS_PROCESS));
		oval_string_map_put(_oval_subtype_windows_map, "registry"                     , _oval_enumeration_object(WINDOWS_REGISTRY));
		oval_string_map_put(_oval_subtype_windows_map, "regkey_audited_permissions_53", _oval_enumeration_object(WINDOWS_REGKEY_AUDITED_PERMISSIONS_53));
		oval_string_map_put(_oval_subtype_windows_map, "regkey_audited_permissions"   , _oval_enumeration_object(WINDOWS_REGKEY_AUDITED_PERMISSIONS));
		oval_string_map_put(_oval_subtype_windows_map, "regkey_effective_rights_53"   , _oval_enumeration_object(WINDOWS_REGKEY_EFFECTIVE_RIGHTS_53));
		oval_string_map_put(_oval_subtype_windows_map, "regkey_effective_rights"      , _oval_enumeration_object(WINDOWS_REGKEY_EFFECTIVE_RIGHTS));
		oval_string_map_put(_oval_subtype_windows_map, "shared_resource"              , _oval_enumeration_object(WINDOWS_SHARED_RESOURCE));
		oval_string_map_put(_oval_subtype_windows_map, "std"                          , _oval_enumeration_object(WINDOWS_SID));
		oval_string_map_put(_oval_subtype_windows_map, "sid_sid"                      , _oval_enumeration_object(WINDOWS_SID_SID));
		oval_string_map_put(_oval_subtype_windows_map, "user_access_control"          , _oval_enumeration_object(WINDOWS_USER_ACCESS_CONTROL));
		oval_string_map_put(_oval_subtype_windows_map, "user"                         , _oval_enumeration_object(WINDOWS_USER));
		oval_string_map_put(_oval_subtype_windows_map, "user_sid_55"                  , _oval_enumeration_object(WINDOWS_USER_SID_55));
		oval_string_map_put(_oval_subtype_windows_map, "user_sid"                     , _oval_enumeration_object(WINDOWS_USER_SID));
		oval_string_map_put(_oval_subtype_windows_map, "volume"                       , _oval_enumeration_object(WINDOWS_VOLUME));
		oval_string_map_put(_oval_subtype_windows_map, "wmi"                          , _oval_enumeration_object(WINDOWS_WMI));
		oval_string_map_put(_oval_subtype_windows_map, "wua_update_searcher"          , _oval_enumeration_object(WINDOWS_WUA_UPDATE_SEARCHER));
	}
	_oval_enumeration_object_t *vstar = oval_string_map_get_value(_oval_subtype_windows_map, type_text);
	return vstar;
}

oval_subtype_enum oval_subtype_parse (xmlTextReaderPtr reader){
	oval_family_enum family = oval_family_parse(reader);
	oval_subtype_enum subtype;
	if(family == FAMILY_UNKNOWN){
		subtype = OVAL_SUBTYPE_UNKNOWN;
	}else{
		char* tagname = xmlTextReaderName(reader);
		int index;for(index=strlen(tagname);tagname[index]!='_';index--);
		tagname[index] = 0;
		_oval_enumeration_object_t *starval;
		switch(family){
			case FAMILY_AIX        :starval = _oval_subtype_aix        (tagname);break;
			case FAMILY_APACHE     :starval = _oval_subtype_apache     (tagname);break;
			case FAMILY_CATOS      :starval = _oval_subtype_catos      (tagname);break;
			case FAMILY_ESX        :starval = _oval_subtype_esx        (tagname);break;
			case FAMILY_FREEBSD    :starval = _oval_subtype_freebsd    (tagname);break;
			case FAMILY_HPUX       :starval = _oval_subtype_hpux       (tagname);break;
			case FAMILY_INDEPENDENT:starval = _oval_subtype_independent(tagname);break;
			case FAMILY_IOS        :starval = _oval_subtype_ios        (tagname);break;
			case FAMILY_LINUX      :starval = _oval_subtype_linux      (tagname);break;
			case FAMILY_MACOS      :starval = _oval_subtype_macos      (tagname);break;
			case FAMILY_PIXOS      :starval = _oval_subtype_pixos      (tagname);break;
			case FAMILY_SOLARIS    :starval = _oval_subtype_solaris    (tagname);break;
			case FAMILY_UNIX       :starval = _oval_subtype_unix       (tagname);break;
			case FAMILY_WINDOWS    :starval = _oval_subtype_windows    (tagname);break;
			default: starval = NULL;
		}
		if(starval==NULL){
			printf("NOTICE:oval_subtype_parse: %s(family = %d) not a registered subtype\n",tagname, family);
			subtype = OVAL_SUBTYPE_UNKNOWN;
		}else subtype = starval->enumval;
		free(tagname);
	}
	return subtype;
}

