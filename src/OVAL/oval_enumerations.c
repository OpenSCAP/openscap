/**
 * @file oval_enumerations.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#include <string.h>
#include "oval_definitions_impl.h"
#include "oval_string_map_impl.h"

#define OVAL_ENUMERATION_INVALID (-1)


int oval_enumeration_attr(xmlTextReaderPtr reader, char *attname, const struct oscap_string_map* map, int defval)
{
	char *attrstr = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
	if (attrstr == NULL)
		return defval;
	int ret = oscap_string_to_enum(map, attrstr);
	free(attrstr);
	return ret == OVAL_ENUMERATION_INVALID ? defval : ret;
}

const struct oscap_string_map OVAL_SYSCHAR_FLAG_MAP[] = {
	{ SYSCHAR_FLAG_ERROR,          "error" },
	{ SYSCHAR_FLAG_COMPLETE,       "complete" },
	{ SYSCHAR_FLAG_INCOMPLETE,     "incomplete" },
	{ SYSCHAR_FLAG_DOES_NOT_EXIST, "does not exist" },
	{ SYSCHAR_FLAG_NOT_APPLICABLE, "not applicable" },
	{ SYSCHAR_FLAG_NOT_COLLECTED,  "not collected" },
	{ OVAL_ENUMERATION_INVALID,    NULL }
};

oval_syschar_collection_flag_enum oval_syschar_flag_parse(xmlTextReaderPtr
  							       reader,
  							       char *attname,
  							       oval_syschar_collection_flag_enum
  							       defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_SYSCHAR_FLAG_MAP, defval);
}

const struct oscap_string_map OVAL_SYSCHAR_STATUS_MAP[] = {
	{ SYSCHAR_STATUS_ERROR,          "error"          },
	{ SYSCHAR_STATUS_DOES_NOT_EXIST, "does not exist" },
	{ SYSCHAR_STATUS_EXISTS,         "exists"         },
	{ SYSCHAR_STATUS_NOT_COLLECTED,  "not collected"  },
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_syschar_status_enum oval_syschar_status_parse(xmlTextReaderPtr
  							       reader,
  							       char *attname,
  							       oval_syschar_status_enum defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_SYSCHAR_STATUS_MAP, defval);
}

const char * oval_syschar_status_text(oval_syschar_status_enum idx){
	return oscap_enum_to_string(OVAL_SYSCHAR_STATUS_MAP, idx);
}

const struct oscap_string_map OVAL_MESSAGE_LEVEL_MAP[] = {
	{ OVAL_MESSAGE_LEVEL_DEBUG,   "debug"   },
	{ OVAL_MESSAGE_LEVEL_ERROR,   "error"   },
	{ OVAL_MESSAGE_LEVEL_FATAL,   "fatal"   },
	{ OVAL_MESSAGE_LEVEL_INFO,    "info"    },
	{ OVAL_MESSAGE_LEVEL_WARNING, "warning" },
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_message_level_enum oval_message_level_parse(
  			xmlTextReaderPtr reader,
  			char *attname,
  			oval_message_level_enum defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_MESSAGE_LEVEL_MAP, defval);
}
const char * oval_message_level_text(oval_message_level_enum level){
	return oscap_enum_to_string(OVAL_MESSAGE_LEVEL_MAP, level);
}

const struct oscap_string_map OVAL_ARITHMETIC_OPERATION_MAP[] = {
	{ OVAL_ARITHMETIC_ADD,      "add"      },
	{ OVAL_ARITHMETIC_MULTIPLY, "multiply" },
	{ OVAL_ARITHMETIC_SUBTRACT, "subtract" },
	{ OVAL_ARITHMETIC_DIVIDE  , "divide"   },
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_arithmetic_operation_enum oval_arithmetic_operation_parse(xmlTextReaderPtr
							   reader,
							   char *attname,
							   oval_arithmetic_operation_enum
							   defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_ARITHMETIC_OPERATION_MAP, defval);
}
const char *oval_arithmetic_operation_text(oval_arithmetic_operation_enum operation)
{
	return OVAL_ARITHMETIC_OPERATION_MAP[operation-1].string;
}

const struct oscap_string_map OVAL_DATETIME_FORMAT_MAP[] = {
	{ OVAL_DATETIME_YEAR_MONTH_DAY,      "year_month_day"      },
	{ OVAL_DATETIME_MONTH_DAY_YEAR,      "month_day_year"      },
	{ OVAL_DATETIME_DAY_MONTH_YEAR,      "day_month_year"      },
	{ OVAL_DATETIME_WIN_FILETIME,        "win_filetime"        },
	{ OVAL_DATETIME_SECONDS_SINCE_EPOCH, "seconds_since_epoch" },
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_datetime_format_enum oval_datetime_format_parse(xmlTextReaderPtr reader,
  						     char *attname,
  						     oval_arithmetic_operation_enum
  						     defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_DATETIME_FORMAT_MAP, defval);
}
const char *oval_datetime_format_text(oval_datetime_format_enum format)
{
	return OVAL_DATETIME_FORMAT_MAP[format-1].string;
}

const struct oscap_string_map OVAL_SET_OPERATION_MAP[] = {
	{ OVAL_SET_OPERATION_COMPLEMENT,   "COMPLEMENT"   },
	{ OVAL_SET_OPERATION_INTERSECTION, "INTERSECTION" },
	{ OVAL_SET_OPERATION_UNION,        "UNION"        },
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_set_operation_enum oval_set_operation_parse(xmlTextReaderPtr reader,
  						 char *attname,
  						 oval_set_operation_enum defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_SET_OPERATION_MAP, defval);
}
const char *oval_set_operation_text(oval_set_operation_enum operation)
{
	return OVAL_SET_OPERATION_MAP[operation-1].string;
}

const struct oscap_string_map OVAL_OPERATION_MAP[] = {
	{ OPERATION_EQUALS,                     "equals"                     },
	{ OPERATION_NOT_EQUAL,                  "not equal"                  },
	{ OPERATION_CASE_INSENSITIVE_EQUALS,    "case insensitive equals"    },
	{ OPERATION_CASE_INSENSITIVE_NOT_EQUAL, "case insensitive not equal" },
	{ OPERATION_GREATER_THAN,               "greater than"               },
	{ OPERATION_LESS_THAN,                  "less than"                  },
	{ OPERATION_GREATER_THAN_OR_EQUAL,      "greater than or equal"      },
	{ OPERATION_LESS_THAN_OR_EQUAL,         "less than or equal"         },
	{ OPERATION_BITWISE_AND,                "bitwise and"                },
	{ OPERATION_BITWISE_OR,                 "bitwise or"                 },
	{ OPERATION_PATTERN_MATCH,              "pattern match"              },
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_operation_enum oval_operation_parse(xmlTextReaderPtr reader, char *attname,
  					 oval_operation_enum defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_OPERATION_MAP, defval);
};
const char *oval_operation_text(oval_operation_enum operation)
{
	return OVAL_OPERATION_MAP[operation-1].string;
}

const struct oscap_string_map OVAL_CHECK_MAP[] = {
	{ OVAL_CHECK_ALL,          "all"          },
	{ OVAL_CHECK_AT_LEAST_ONE, "at least one" },
	{ OVAL_CHECK_NONE_EXIST,   "none exist"   },
	{ OVAL_CHECK_NONE_SATISFY, "none satisfy" },
	{ OVAL_CHECK_ONLY_ONE,     "only one"     },
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_check_enum oval_check_parse(xmlTextReaderPtr reader, char *attname,
  				 oval_check_enum defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_CHECK_MAP, defval);
}
const char * oval_check_text(oval_check_enum check)
{
	return OVAL_CHECK_MAP[check-1].string;
}


const struct oscap_string_map OVAL_DATATYPE_MAP[] = {
	{ OVAL_DATATYPE_BINARY,            "binary"           },
	{ OVAL_DATATYPE_BOOLEAN,           "boolean"          },
	{ OVAL_DATATYPE_EVR_STRING,        "evr_string"       },
	{ OVAL_DATATYPE_FILESET_REVISTION, "fileset_revision" },
	{ OVAL_DATATYPE_FLOAT,             "float"            },
	{ OVAL_DATATYPE_IOS_VERSION,       "ios_version"      },
	{ OVAL_DATATYPE_VERSION,           "version"          },
	{ OVAL_DATATYPE_INTEGER,           "int"              },
	{ OVAL_DATATYPE_STRING,            "string"           },
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_datatype_enum oval_datatype_parse(xmlTextReaderPtr reader, char *attname,
  				       oval_datatype_enum defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_DATATYPE_MAP, defval);
}
const char *oval_datatype_text(oval_datatype_enum datatype)
{
	return OVAL_DATATYPE_MAP[datatype-1].string;
}

const struct oscap_string_map OVAL_EXISTENCE_MAP[] = {
	{ ALL_EXIST,           "all_exist"           },
	{ ANY_EXIST,           "any_exist"           },
	{ AT_LEAST_ONE_EXISTS, "at_least_one_exists" },
	{ ONLY_ONE_EXISTS,     "only_one_exists"     },
	{ NONE_EXIST,          "none_exist"          },
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_existence_enum oval_existence_parse(xmlTextReaderPtr reader, char *attname,
				 oval_existence_enum defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_EXISTENCE_MAP, defval);
}
const char * oval_existence_text(oval_existence_enum existence)
{
	return OVAL_EXISTENCE_MAP[existence-1].string;
}

const struct oscap_string_map OVAL_OPERATOR_MAP[] = {
	{ OPERATOR_AND, "AND" },
	{ OPERATOR_ONE, "ONE" },
	{ OPERATOR_OR,  "OR"  },
	{ OPERATOR_XOR, "XOR" },
	{ -1, NULL }
};

oval_operator_enum oval_operator_parse(xmlTextReaderPtr reader, char *attname,
  				       oval_operator_enum defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_OPERATOR_MAP, defval);
}

const char* oval_operator_text(oval_operator_enum operator)
{
	return OVAL_OPERATOR_MAP[operator-1].string;
}

const struct oscap_string_map OVAL_FAMILY_MAP[] = {
	{ FAMILY_AIX,         "aix"         },
	{ FAMILY_APACHE,      "apache"      },
	{ FAMILY_CATOS,       "catos"       },
	{ FAMILY_ESX,         "esx"         },
	{ FAMILY_FREEBSD,     "freebsd"     },
	{ FAMILY_HPUX,        "hpux"        },
	{ FAMILY_INDEPENDENT, "independent" },
	{ FAMILY_IOS,         "ios"         },
	{ FAMILY_LINUX,       "linux"       },
	{ FAMILY_MACOS,       "macos"       },
	{ FAMILY_PIXOS,       "pixos"       },
	{ FAMILY_SOLARIS,     "solaris"     },
	{ FAMILY_UNIX,        "unix"        },
	{ FAMILY_WINDOWS,     "windows"     },
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_family_enum oval_family_parse(xmlTextReaderPtr reader)
{
  	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	char *family_text = strrchr(namespace, '#');
	if (family_text == NULL) {
		free(namespace);
		return FAMILY_UNKNOWN;
  	}
	int ret = oscap_string_to_enum(OVAL_FAMILY_MAP, ++family_text);
	free(namespace);
	return (ret != OVAL_ENUMERATION_INVALID ? ret : FAMILY_UNKNOWN);
}

const char *oval_family_text(oval_family_enum family)
{
	int family_idx = family/1000;
	return OVAL_FAMILY_MAP[family_idx-1].string;
}


const struct oscap_string_map OVAL_SUBTYPE_AIX_MAP[] = {
	{ AIX_FILESET, "fileset" },
	{ AIX_FIX,     "fix"     },
	{ AIX_OSLEVEL, "oslevel" },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_APACHE_MAP[] = {
	{ APACHE_HTTPD,   "httpd"   },
	{ APACHE_VERSION, "version" },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_CATOS_MAP[] = {
	{ CATOS_LINE,       "line"      },
	{ CATOS_MODULE,     "module"    },
	{ CATOS_VERSION,    "version"   },
	{ CATOS_VERSION_55, "version55" },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_ESX_MAP[] = {
	{ ESX_PATCH,   "patch"   },
	{ ESX_VERSION, "version" },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_FREEBSD_MAP[] = {
	{ FREEBSD_PORT_INFO, "portinfo" },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_HPUX_MAP[] = {
	{ HPUX_GETCONF,          "getconf"         },
	{ HPUX_PATCH,            "patch"           },
	{ HPUX_PATCH_53,         "patch53"         },
	{ HPUX_SOFTWARE_LIST,    "softwarelist"    },
	{ HPUX_TRUSTED_ACCOUNTS, "trustedaccounts" },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_INDEPENDENT_MAP[] = {
	{ INDEPENDENT_ENVIRONMENT_VARIABLE, "environmentvariable" },
	{ INDEPENDENT_FAMILY,               "family"              },
	{ INDEPENDENT_FILE_HASH,            "filehash"            },
	{ INDEPENDENT_FILE_MD5,             "filemd5"             },
	{ INDEPENDENT_SQL,                  "sql"                 },
	{ INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54"   },
	{ INDEPENDENT_TEXT_FILE_CONTENT,    "textfilecontent"     },
	{ INDEPENDENT_VARIABLE,             "variable"            },
	{ INDEPENDENT_UNKNOWN,              "unknown"             },
	{ INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent"      },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_IOS_MAP[] = {
	{ IOS_GLOBAL,     "global"    },
	{ IOS_INTERFACE,  "interface" },
	{ IOS_LINE,       "line"      },
	{ IOS_SNMP,       "snmp"      },
	{ IOS_TCLSH,      "tclsh"     },
	{ IOS_VERSION_55, "version55" },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_LINUX_MAP[] = {
	{ LINUX_DPKG_INFO,               "dpkginfo"             },
	{ LINUX_INET_LISTENING_SERVERS,  "inetlisteningservers" },
	{ LINUX_RPM_INFO,                "rpminfo"              },
	{ LINUX_SLACKWARE_PKG_INFO_TEST, "slackwarepkginfo"     },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_MACOS_MAP[] = {
	{ MACOS_ACCOUNT_INFO,           "accountinfo"          },
	{ MACOS_INET_LISTENING_SERVERS, "inetlisteningservers" },
	{ MACOS_NVRAM_INFO,             "nvraminfo"            },
	{ MACOS_PWPOLICY,               "pwpolicy"             },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_PIXOS_MAP[] = {
	{ PIXOS_LINE,    "line"    },
	{ PIXOS_VERSION, "version" },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_SOLARIS_MAP[] = {
	{ SOLARIS_ISAINFO, "isainfo" },
	{ SOLARIS_PACKAGE, "package" },
	{ SOLARIS_PATCH,   "patch"   },
	{ SOLARIS_SMF,     "smf"     },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_UNIX_MAP[] = {
	{ UNIX_FILE,      "file"      },
	{ UNIX_INETD,     "inetd"     },
	{ UNIX_INTERFACE, "interface" },
	{ UNIX_PASSWORD,  "password"  },
	{ UNIX_PROCESS,   "process"   },
	{ UNIX_RUNLEVEL,  "runlevel"  },
	{ UNIX_SCCS,      "sccs"      },
	{ UNIX_SHADOW,    "shadow"    },
	{ UNIX_UNAME,     "uname"     },
	{ UNIX_XINETD,    "xinetd"    },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};

const struct oscap_string_map OVAL_SUBTYPE_WINDOWS_MAP[] = {
	{ WINDOWS_ACCESS_TOKEN,                  "access_token"                  },
	{ WINDOWS_ACTIVE_DIRECTORY,              "active_directory"              },
	{ WINDOWS_AUDIT_EVENT_POLICY,            "audit_event_policy"            },
	{ WINDOWS_AUDIT_EVENT_SUBCATEGORIES,     "audit_event_subcatagories"     },
	{ WINDOWS_FILE,                          "file"                          },
	{ WINDOWS_FILE_AUDITED_PERMISSIONS_53,   "file_audited_permissions_53"   },
	{ WINDOWS_FILE_AUDITED_PERMISSIONS,      "file_audited_permissions"      },
	{ WINDOWS_FILE_EFFECTIVE_RIGHTS_53,      "file_effective_rights_53"      },
	{ WINDOWS_FILE_EFFECTIVE_RIGHTS,         "file_effective_rights"         },
	{ WINDOWS_GROUP,                         "group"                         },
	{ WINDOWS_GROUP_SID,                     "group_sid"                     },
	{ WINDOWS_INTERFACE,                     "interface"                     },
	{ WINDOWS_LOCKOUT_POLICY,                "lockout_policy"                },
	{ WINDOWS_METABASE,                      "metabase"                      },
	{ WINDOWS_PASSWORD_POLICY,               "password_policy"               },
	{ WINDOWS_PORT,                          "port"                          },
	{ WINDOWS_PRINTER_EFFECTIVE_RIGHTS,      "printer_effective_rights"      },
	{ WINDOWS_PROCESS,                       "process"                       },
	{ WINDOWS_REGISTRY,                      "registry"                      },
	{ WINDOWS_REGKEY_AUDITED_PERMISSIONS_53, "regkey_audited_permissions_53" },
	{ WINDOWS_REGKEY_AUDITED_PERMISSIONS,    "regkey_audited_permissions"    },
	{ WINDOWS_REGKEY_EFFECTIVE_RIGHTS_53,    "regkey_effective_rights_53"    },
	{ WINDOWS_REGKEY_EFFECTIVE_RIGHTS,       "regkey_effective_rights"       },
	{ WINDOWS_SHARED_RESOURCE,               "shared_resource"               },
	{ WINDOWS_SID,                           "sid"                           },
	{ WINDOWS_SID_SID,                       "sid_sid"                       },
	{ WINDOWS_USER_ACCESS_CONTROL,           "user_access_control"           },
	{ WINDOWS_USER,                          "user"                          },
	{ WINDOWS_USER_SID_55,                   "user_sid_55"                   },
	{ WINDOWS_USER_SID,                      "user_sid"                      },
	{ WINDOWS_VOLUME,                        "volume"                        },
	{ WINDOWS_WMI,                           "wmi"                           },
	{ WINDOWS_WUA_UPDATE_SEARCHER,           "wua_update_searcher"           },
	{ OVAL_SUBTYPE_UNKNOWN, NULL }
};


oval_subtype_enum oval_subtype_parse(xmlTextReaderPtr reader)
{
	oval_family_enum family = oval_family_parse(reader);
	if (family == FAMILY_UNKNOWN)
		return OVAL_SUBTYPE_UNKNOWN;

	oval_subtype_enum subtype = OVAL_SUBTYPE_UNKNOWN;
	char *tagname = (char*) xmlTextReaderLocalName(reader);
	char *endptr = strrchr(tagname, '_');
	if (endptr == NULL)
		goto cleanup;
	*endptr = '\0';

	const struct oscap_string_map *map = NULL;
	switch (family) {
	case FAMILY_AIX:         map = OVAL_SUBTYPE_AIX_MAP;         break;
	case FAMILY_APACHE:      map = OVAL_SUBTYPE_APACHE_MAP;      break;
	case FAMILY_CATOS:       map = OVAL_SUBTYPE_CATOS_MAP;       break;
	case FAMILY_ESX:         map = OVAL_SUBTYPE_ESX_MAP;         break;
	case FAMILY_FREEBSD:     map = OVAL_SUBTYPE_FREEBSD_MAP;     break;
	case FAMILY_HPUX:        map = OVAL_SUBTYPE_HPUX_MAP;        break;
	case FAMILY_INDEPENDENT: map = OVAL_SUBTYPE_INDEPENDENT_MAP; break;
	case FAMILY_IOS:         map = OVAL_SUBTYPE_IOS_MAP;         break;
	case FAMILY_LINUX:       map = OVAL_SUBTYPE_LINUX_MAP;       break;
	case FAMILY_MACOS:       map = OVAL_SUBTYPE_MACOS_MAP;       break;
	case FAMILY_PIXOS:       map = OVAL_SUBTYPE_PIXOS_MAP;       break;
	case FAMILY_SOLARIS:     map = OVAL_SUBTYPE_SOLARIS_MAP;     break;
	case FAMILY_UNIX:        map = OVAL_SUBTYPE_UNIX_MAP;        break;
	case FAMILY_WINDOWS:     map = OVAL_SUBTYPE_WINDOWS_MAP;     break;
	default: map = NULL;
	}

	subtype = oscap_string_to_enum(map, tagname);

	cleanup:
	free(tagname);
	return subtype;
}

const char *oval_subtype_text(oval_subtype_enum subtype)
{
	oval_family_enum family = (subtype/1000)*1000;
	const struct oscap_string_map *map = NULL;
	switch (family) {
	case FAMILY_AIX:         map = OVAL_SUBTYPE_AIX_MAP;         break;
	case FAMILY_APACHE:      map = OVAL_SUBTYPE_APACHE_MAP;      break;
	case FAMILY_CATOS:       map = OVAL_SUBTYPE_CATOS_MAP;       break;
	case FAMILY_ESX:         map = OVAL_SUBTYPE_ESX_MAP;         break;
	case FAMILY_FREEBSD:     map = OVAL_SUBTYPE_FREEBSD_MAP;     break;
	case FAMILY_HPUX:        map = OVAL_SUBTYPE_HPUX_MAP;        break;
	case FAMILY_INDEPENDENT: map = OVAL_SUBTYPE_INDEPENDENT_MAP; break;
	case FAMILY_IOS:         map = OVAL_SUBTYPE_IOS_MAP;         break;
	case FAMILY_LINUX:       map = OVAL_SUBTYPE_LINUX_MAP;       break;
	case FAMILY_MACOS:       map = OVAL_SUBTYPE_MACOS_MAP;       break;
	case FAMILY_PIXOS:       map = OVAL_SUBTYPE_PIXOS_MAP;       break;
	case FAMILY_SOLARIS:     map = OVAL_SUBTYPE_SOLARIS_MAP;     break;
	case FAMILY_UNIX:        map = OVAL_SUBTYPE_UNIX_MAP;        break;
	case FAMILY_WINDOWS:     map = OVAL_SUBTYPE_WINDOWS_MAP;     break;
	default: map = NULL;
	}

	int subidx = subtype%1000;

	return map[subidx-1].string;
}

const struct oscap_string_map OVAL_RESULT_MAP[] = {
	{ OVAL_RESULT_TRUE,           "true"          },
	{ OVAL_RESULT_FALSE,          "false"         },
	{ OVAL_RESULT_UNKNOWN,        "unknown"       },
	{ OVAL_RESULT_ERROR,          "error"         },
	{ OVAL_RESULT_NOT_EVALUATED,  "not evaluated" },
	{ OVAL_RESULT_NOT_APPLICABLE, "not applicable"},
	{ OVAL_ENUMERATION_INVALID, NULL }
};

oval_result_enum oval_result_parse(xmlTextReaderPtr reader, char *attname,
  				 oval_existence_enum defval)
{
	  return oval_enumeration_attr(reader, attname, OVAL_RESULT_MAP, defval);
}

const char* oval_result_text(oval_result_enum result)
{
	return OVAL_RESULT_MAP[result-1].string;
}
const char *oval_datetime_format_text(oval_datetime_format_enum);
