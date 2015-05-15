/**
 * @file oval_enumerations.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "oval_agent_api_impl.h"
#include "oval_definitions_impl.h"
#include "adt/oval_string_map_impl.h"
#include "oval_system_characteristics_impl.h"
#include "results/oval_results_impl.h"
#include "common/util.h"
#include "common/_error.h"
#include "common/debug_priv.h"

const char _invalid[] = "**INVALID**";

static int oval_enumeration_attr(xmlTextReaderPtr reader, char *attname, const struct oscap_string_map *map, int defval)
{
	char *attrstr = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST attname);
	if (attrstr == NULL)
		return defval;
	int ret = oscap_string_to_enum(map, attrstr);
	oscap_free(attrstr);
	return ret == OVAL_ENUMERATION_INVALID ? defval : ret;
}

static const char *oval_enumeration_get_text(const struct oscap_string_map *map, int val)
{
	return oscap_enum_to_string(map, val);
}

static int oval_enumeration_from_text(const struct oscap_string_map *map, const char *text)
{
	return oscap_string_to_enum(map, text);
}

static const struct oscap_string_map OVAL_SYSCHAR_FLAG_MAP[] = {
	{SYSCHAR_FLAG_ERROR, "error"},
	{SYSCHAR_FLAG_COMPLETE, "complete"},
	{SYSCHAR_FLAG_INCOMPLETE, "incomplete"},
	{SYSCHAR_FLAG_DOES_NOT_EXIST, "does not exist"},
	{SYSCHAR_FLAG_NOT_APPLICABLE, "not applicable"},
	{SYSCHAR_FLAG_NOT_COLLECTED, "not collected"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_syschar_collection_flag_t oval_syschar_flag_parse(xmlTextReaderPtr
						       reader, char *attname, oval_syschar_collection_flag_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_SYSCHAR_FLAG_MAP, defval);
}

const char *oval_syschar_collection_flag_get_text(oval_syschar_collection_flag_t flag)
{
	return oval_enumeration_get_text(OVAL_SYSCHAR_FLAG_MAP, flag);
}

static const struct oscap_string_map OVAL_SYSCHAR_STATUS_MAP[] = {
	{SYSCHAR_STATUS_ERROR, "error"},
	{SYSCHAR_STATUS_DOES_NOT_EXIST, "does not exist"},
	{SYSCHAR_STATUS_EXISTS, "exists"},
	{SYSCHAR_STATUS_NOT_COLLECTED, "not collected"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_syschar_status_t oval_syschar_status_parse(xmlTextReaderPtr reader, char *attname, oval_syschar_status_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_SYSCHAR_STATUS_MAP, defval);
}

const char *oval_syschar_status_get_text(oval_syschar_status_t idx)
{
	return oscap_enum_to_string(OVAL_SYSCHAR_STATUS_MAP, idx);
}

static const struct oscap_string_map OVAL_MESSAGE_LEVEL_MAP[] = {
	{OVAL_MESSAGE_LEVEL_DEBUG, "debug"},
	{OVAL_MESSAGE_LEVEL_ERROR, "error"},
	{OVAL_MESSAGE_LEVEL_FATAL, "fatal"},
	{OVAL_MESSAGE_LEVEL_INFO, "info"},
	{OVAL_MESSAGE_LEVEL_WARNING, "warning"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_message_level_t oval_message_level_parse(xmlTextReaderPtr reader, char *attname, oval_message_level_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_MESSAGE_LEVEL_MAP, defval);
}

const char *oval_message_level_text(oval_message_level_t level)
{
	return oscap_enum_to_string(OVAL_MESSAGE_LEVEL_MAP, level);
}

static const struct oscap_string_map OVAL_ARITHMETIC_OPERATION_MAP[] = {
	{OVAL_ARITHMETIC_ADD, "add"},
	{OVAL_ARITHMETIC_MULTIPLY, "multiply"},
	{OVAL_ARITHMETIC_SUBTRACT, "subtract"},
	{OVAL_ARITHMETIC_DIVIDE, "divide"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_arithmetic_operation_t oval_arithmetic_operation_parse(xmlTextReaderPtr
							    reader, char *attname, oval_arithmetic_operation_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_ARITHMETIC_OPERATION_MAP, defval);
}

const char *oval_arithmetic_operation_get_text(oval_arithmetic_operation_t operation)
{
	return oval_enumeration_get_text(OVAL_ARITHMETIC_OPERATION_MAP, operation);
}

static const struct oscap_string_map OVAL_DATETIME_FORMAT_MAP[] = {
	{OVAL_DATETIME_YEAR_MONTH_DAY, "year_month_day"},
	{OVAL_DATETIME_MONTH_DAY_YEAR, "month_day_year"},
	{OVAL_DATETIME_DAY_MONTH_YEAR, "day_month_year"},
	{OVAL_DATETIME_WIN_FILETIME, "win_filetime"},
	{OVAL_DATETIME_SECONDS_SINCE_EPOCH, "seconds_since_epoch"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_datetime_format_t oval_datetime_format_parse(xmlTextReaderPtr reader,
						  char *attname, oval_arithmetic_operation_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_DATETIME_FORMAT_MAP, defval);
}

const char *oval_datetime_format_get_text(oval_datetime_format_t format)
{
	return oval_enumeration_get_text(OVAL_DATETIME_FORMAT_MAP, format);
}

static const struct oscap_string_map OVAL_SET_OPERATION_MAP[] = {
	{OVAL_SET_OPERATION_COMPLEMENT, "COMPLEMENT"},
	{OVAL_SET_OPERATION_INTERSECTION, "INTERSECTION"},
	{OVAL_SET_OPERATION_UNION, "UNION"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_setobject_operation_t oval_set_operation_parse(xmlTextReaderPtr reader,
						    char *attname, oval_setobject_operation_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_SET_OPERATION_MAP, defval);
}

const char *oval_set_operation_get_text(oval_setobject_operation_t operation)
{
	return oval_enumeration_get_text(OVAL_SET_OPERATION_MAP, operation);
}

static const struct oscap_string_map OVAL_FILTER_ACTION_MAP[] = {
	{OVAL_FILTER_ACTION_EXCLUDE, "exclude"},
	{OVAL_FILTER_ACTION_INCLUDE, "include"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_filter_action_t oval_filter_action_parse(xmlTextReaderPtr reader, char *attname,
					      oval_filter_action_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_FILTER_ACTION_MAP, defval);
}

const char *oval_filter_action_get_text(oval_filter_action_t action)
{
	return oval_enumeration_get_text(OVAL_FILTER_ACTION_MAP, action);
}

static const struct oscap_string_map OVAL_OPERATION_MAP[] = {
	{OVAL_OPERATION_EQUALS, "equals"},
	{OVAL_OPERATION_NOT_EQUAL, "not equal"},
	{OVAL_OPERATION_CASE_INSENSITIVE_EQUALS, "case insensitive equals"},
	{OVAL_OPERATION_CASE_INSENSITIVE_NOT_EQUAL, "case insensitive not equal"},
	{OVAL_OPERATION_GREATER_THAN, "greater than"},
	{OVAL_OPERATION_LESS_THAN, "less than"},
	{OVAL_OPERATION_GREATER_THAN_OR_EQUAL, "greater than or equal"},
	{OVAL_OPERATION_LESS_THAN_OR_EQUAL, "less than or equal"},
	{OVAL_OPERATION_BITWISE_AND, "bitwise and"},
	{OVAL_OPERATION_BITWISE_OR, "bitwise or"},
	{OVAL_OPERATION_PATTERN_MATCH, "pattern match"},
	{OVAL_OPERATION_SUBSET_OF, "subset of"},
	{OVAL_OPERATION_SUPERSET_OF, "superset of"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_operation_t oval_operation_parse(xmlTextReaderPtr reader, char *attname, oval_operation_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_OPERATION_MAP, defval);
};

const char *oval_operation_get_text(oval_operation_t operation)
{
	return oval_enumeration_get_text(OVAL_OPERATION_MAP, operation);
}

static const struct oscap_string_map OVAL_CHECK_MAP[] = {
	{OVAL_CHECK_ALL, "all"},
	{OVAL_CHECK_AT_LEAST_ONE, "at least one"},
	{OVAL_CHECK_NONE_EXIST, "none exist"},
	{OVAL_CHECK_NONE_SATISFY, "none satisfy"},
	{OVAL_CHECK_ONLY_ONE, "only one"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_check_t oval_check_parse(xmlTextReaderPtr reader, char *attname, oval_check_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_CHECK_MAP, defval);
}

const char *oval_check_get_text(oval_check_t check)
{
	return oval_enumeration_get_text(OVAL_CHECK_MAP, check);
}

static const struct oscap_string_map OVAL_DATATYPE_MAP[] = {
	{OVAL_DATATYPE_BINARY, "binary"},
	{OVAL_DATATYPE_BOOLEAN, "boolean"},
	{OVAL_DATATYPE_EVR_STRING, "evr_string"},
	{OVAL_DATATYPE_FILESET_REVISION, "fileset_revision"},
	{OVAL_DATATYPE_FLOAT, "float"},
	{OVAL_DATATYPE_IOS_VERSION, "ios_version"},
	{OVAL_DATATYPE_IPV4ADDR, "ipv4_address"},
	{OVAL_DATATYPE_IPV6ADDR, "ipv6_address"},
	{OVAL_DATATYPE_VERSION, "version"},
	{OVAL_DATATYPE_INTEGER, "int"},
	{OVAL_DATATYPE_STRING, "string"},
	{OVAL_DATATYPE_RECORD, "record"},
	{OVAL_DATATYPE_SEXP, "SEXP"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_datatype_t oval_datatype_parse(xmlTextReaderPtr reader, char *attname, oval_datatype_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_DATATYPE_MAP, defval);
}

const char *oval_datatype_get_text(oval_datatype_t datatype)
{
	return oval_enumeration_get_text(OVAL_DATATYPE_MAP, datatype);
}

oval_datatype_t oval_datatype_from_text(const char *text)
{
	return oval_enumeration_from_text(OVAL_DATATYPE_MAP, text);
}

static const struct oscap_string_map OVAL_EXISTENCE_MAP[] = {
	{OVAL_ALL_EXIST, "all_exist"},
	{OVAL_ANY_EXIST, "any_exist"},
	{OVAL_AT_LEAST_ONE_EXISTS, "at_least_one_exists"},
	{OVAL_ONLY_ONE_EXISTS, "only_one_exists"},
	{OVAL_NONE_EXIST, "none_exist"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_existence_t oval_existence_parse(xmlTextReaderPtr reader, char *attname, oval_existence_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_EXISTENCE_MAP, defval);
}

const char *oval_existence_get_text(oval_existence_t existence)
{
	return oval_enumeration_get_text(OVAL_EXISTENCE_MAP, existence);
}

static const struct oscap_string_map OVAL_OPERATOR_MAP[] = {
	{OVAL_OPERATOR_AND, "AND"},
	{OVAL_OPERATOR_ONE, "ONE"},
	{OVAL_OPERATOR_OR, "OR"},
	{OVAL_OPERATOR_XOR, "XOR"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_operator_t oval_operator_parse(xmlTextReaderPtr reader, char *attname, oval_operator_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_OPERATOR_MAP, defval);
}

const char *oval_operator_get_text(oval_operator_t operator)
{
	return oval_enumeration_get_text(OVAL_OPERATOR_MAP, operator);
}

static const struct oscap_string_map OVAL_VARIABLE_TYPE_MAP[] = {
	{OVAL_VARIABLE_CONSTANT, "constant_variable"},
	{OVAL_VARIABLE_EXTERNAL, "external_variable"},
	{OVAL_VARIABLE_LOCAL, "local_variable"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

const char *oval_variable_type_get_text(oval_variable_type_t type)
{
	return oval_enumeration_get_text(OVAL_VARIABLE_TYPE_MAP, type);
}

static const struct oscap_string_map OVAL_COMPONENT_TYPE_MAP[] = {
	{OVAL_COMPONENT_LITERAL, "literal_component"},
	{OVAL_COMPONENT_OBJECTREF, "object_component"},
	{OVAL_COMPONENT_VARREF, "variable_component"},
	{OVAL_ENUMERATION_INVALID, NULL},
	{OVAL_FUNCTION_BEGIN, "begin"},
	{OVAL_FUNCTION_CONCAT, "concat"},
	{OVAL_FUNCTION_END, "end"},
	{OVAL_FUNCTION_SPLIT, "split"},
	{OVAL_FUNCTION_SUBSTRING, "substring"},
	{OVAL_FUNCTION_TIMEDIF, "time_difference"},
	{OVAL_FUNCTION_ESCAPE_REGEX, "escape_regex"},
	{OVAL_FUNCTION_REGEX_CAPTURE, "regex_capture"},
	{OVAL_FUNCTION_ARITHMETIC, "arithmetic"},
	{OVAL_FUNCTION_COUNT, "count"},
	{OVAL_FUNCTION_GLOB_TO_REGEX, "glob_to_regex"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

const char *oval_component_type_get_text(oval_component_type_t type)
{
	if (type > OVAL_FUNCTION)
		type = type - OVAL_FUNCTION + OVAL_COMPONENT_FUNCTION;
	return oval_enumeration_get_text(OVAL_COMPONENT_TYPE_MAP, type);
}

static const struct oscap_string_map OVAL_FAMILY_MAP[] = {
	{OVAL_FAMILY_AIX, "aix"},
	{OVAL_FAMILY_APACHE, "apache"},
	{OVAL_FAMILY_CATOS, "catos"},
	{OVAL_FAMILY_ESX, "esx"},
	{OVAL_FAMILY_FREEBSD, "freebsd"},
	{OVAL_FAMILY_HPUX, "hpux"},
	{OVAL_FAMILY_INDEPENDENT, "independent"},
	{OVAL_FAMILY_IOS, "ios"},
	{OVAL_FAMILY_LINUX, "linux"},
	{OVAL_FAMILY_MACOS, "macos"},
	{OVAL_FAMILY_PIXOS, "pixos"},
	{OVAL_FAMILY_SOLARIS, "solaris"},
	{OVAL_FAMILY_UNIX, "unix"},
	{OVAL_FAMILY_WINDOWS, "windows"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_family_t oval_family_parse(xmlTextReaderPtr reader)
{
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	if (namespace == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Could not find namespace definition for prefix '%s'", (char *) xmlTextReaderConstPrefix(reader));
		return OVAL_FAMILY_UNKNOWN;
	}
	char *family_text = strrchr(namespace, '#');
	if (family_text == NULL) {
		dW("No OVAL family for namespace: %s\n", namespace);
		oscap_free(namespace);
		return OVAL_FAMILY_UNKNOWN;
	}

	int ret = oscap_string_to_enum(OVAL_FAMILY_MAP, ++family_text);

	if (ret == OVAL_ENUMERATION_INVALID) {
		dW("Unknown OVAL family: %s\n", family_text);
		ret = OVAL_FAMILY_UNKNOWN;
	}

	oscap_free(namespace);
	return ret;
}

const char *oval_family_get_text(oval_family_t family)
{
	return oval_enumeration_get_text(OVAL_FAMILY_MAP, family);
}

static const struct oscap_string_map OVAL_SUBTYPE_AIX_MAP[] = {
	{OVAL_AIX_FILESET, "fileset"},
	{OVAL_AIX_FIX, "fix"},
	{OVAL_AIX_OSLEVEL, "oslevel"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_APACHE_MAP[] = {
	{OVAL_APACHE_HTTPD, "httpd"},
	{OVAL_APACHE_VERSION, "version"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_CATOS_MAP[] = {
	{OVAL_CATOS_LINE, "line"},
	{OVAL_CATOS_MODULE, "module"},
	{OVAL_CATOS_VERSION_55, "version55"},
	{OVAL_CATOS_VERSION, "version"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_ESX_MAP[] = {
	{OVAL_ESX_PATCH, "patch"},
	{OVAL_ESX_PATCH56, "patch56"},
	{OVAL_ESX_VERSION, "version"},
	{OVAL_ESX_VISDKMANAGEDOBJECT, "visdkmanagedobject"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_FREEBSD_MAP[] = {
	{OVAL_FREEBSD_PORT_INFO, "portinfo"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_HPUX_MAP[] = {
	{OVAL_HPUX_GETCONF, "getconf"},
	{OVAL_HPUX_PATCH_53, "patch53"},
	{OVAL_HPUX_PATCH, "patch"},
	{OVAL_HPUX_SOFTWARE_LIST, "softwarelist"},
	{OVAL_HPUX_TRUSTED_ACCOUNTS, "trustedaccounts"},
	{OVAL_HPUX_NDD, "ndd"},
	{OVAL_HPUX_SWLIST, "swlist"},
	{OVAL_HPUX_TRUSTED, "trusted"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_INDEPENDENT_MAP[] = {
	{OVAL_INDEPENDENT_FAMILY, "family"},
	{OVAL_INDEPENDENT_FILE_MD5, "filemd5"},
	{OVAL_INDEPENDENT_FILE_HASH, "filehash"},
	{OVAL_INDEPENDENT_FILE_HASH58, "filehash58"},
	{OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE, "environmentvariable"},
	{OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE58, "environmentvariable58"},
	{OVAL_INDEPENDENT_SQL57, "sql57"},
	{OVAL_INDEPENDENT_SQL, "sql"},
	{OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54"},
	{OVAL_INDEPENDENT_TEXT_FILE_CONTENT, "textfilecontent"},
	{OVAL_INDEPENDENT_UNKNOWN, "unknown"},
	{OVAL_INDEPENDENT_VARIABLE, "variable"},
	{OVAL_INDEPENDENT_XML_FILE_CONTENT, "xmlfilecontent"},
	{OVAL_INDEPENDENT_SYSCHAR_SUBTYPE, "system_info"},
	{OVAL_INDEPENDENT_LDAP57, "ldap57"},
	{OVAL_INDEPENDENT_LDAP, "ldap"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_IOS_MAP[] = {
	{OVAL_IOS_GLOBAL, "global"},
	{OVAL_IOS_INTERFACE, "interface"},
	{OVAL_IOS_LINE, "line"},
	{OVAL_IOS_SNMP, "snmp"},
	{OVAL_IOS_TCLSH, "tclsh"},
	{OVAL_IOS_VERSION_55, "version55"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_LINUX_MAP[] = {
	{OVAL_LINUX_DPKG_INFO, "dpkginfo"},
	{OVAL_LINUX_INET_LISTENING_SERVERS, "inetlisteningservers"},
	{OVAL_LINUX_INET_LISTENING_SERVER, "inetlisteningserver"},
	{OVAL_LINUX_RPM_INFO, "rpminfo"},
	{OVAL_LINUX_SLACKWARE_PKG_INFO_TEST, "slackwarepkginfo"},
        {OVAL_LINUX_PARTITION, "partition"},
        {OVAL_LINUX_IFLISTENERS, "iflisteners"},
        {OVAL_LINUX_RPMVERIFY, "rpmverify"},
        {OVAL_LINUX_RPMVERIFYFILE, "rpmverifyfile"},
        {OVAL_LINUX_RPMVERIFYPACKAGE, "rpmverifypackage"},
        {OVAL_LINUX_SELINUXBOOLEAN, "selinuxboolean"},
        {OVAL_LINUX_SELINUXSECURITYCONTEXT, "selinuxsecuritycontext"},
        {OVAL_LINUX_SYSTEMDUNITPROPERTY, "systemdunitproperty"},
        {OVAL_LINUX_SYSTEMDUNITDEPENDENCY, "systemdunitdependency"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_MACOS_MAP[] = {
	{OVAL_MACOS_ACCOUNT_INFO, "accountinfo"},
	{OVAL_MACOS_INET_LISTENING_SERVERS, "inetlisteningservers"},
	{OVAL_MACOS_NVRAM_INFO, "nvraminfo"},
	{OVAL_MACOS_PWPOLICY, "pwpolicy"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_PIXOS_MAP[] = {
	{OVAL_PIXOS_LINE, "line"},
	{OVAL_PIXOS_VERSION, "version"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_SOLARIS_MAP[] = {
	{OVAL_SOLARIS_ISAINFO, "isainfo"},
	{OVAL_SOLARIS_PACKAGE, "package"},
	{OVAL_SOLARIS_PATCH, "patch"},
	{OVAL_SOLARIS_PATCH54, "patch54"},
	{OVAL_SOLARIS_SMF, "smf"},
	{OVAL_SOLARIS_NDD, "ndd"},
	{OVAL_SOLARIS_PACKAGECHECK, "packagecheck"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_UNIX_MAP[] = {
	{OVAL_UNIX_FILE, "file"},
	{OVAL_UNIX_INETD, "inetd"},
	{OVAL_UNIX_INTERFACE, "interface"},
	{OVAL_UNIX_PASSWORD, "password"},
	{OVAL_UNIX_PROCESS, "process"},
	{OVAL_UNIX_RUNLEVEL, "runlevel"},
	{OVAL_UNIX_SCCS, "sccs"},
	{OVAL_UNIX_SHADOW, "shadow"},
	{OVAL_UNIX_UNAME, "uname"},
	{OVAL_UNIX_XINETD, "xinetd"},
	{OVAL_UNIX_DNSCACHE, "dnscache"},
        {OVAL_UNIX_SYSCTL, "sysctl"},
	{OVAL_UNIX_PROCESS58, "process58"},
        {OVAL_UNIX_FILEEXTENDEDATTRIBUTE, "fileextendedattribute"},
        {OVAL_UNIX_GCONF, "gconf"},
        {OVAL_UNIX_ROUTINGTABLE, "routingtable"},
	{OVAL_UNIX_SYMLINK, "symlink"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

static const struct oscap_string_map OVAL_SUBTYPE_WINDOWS_MAP[] = {
	{OVAL_WINDOWS_ACCESS_TOKEN, "access_token"},
	{OVAL_WINDOWS_ACTIVE_DIRECTORY, "active_directory"},
	{OVAL_WINDOWS_AUDIT_EVENT_POLICY, "audit_event_policy"},
	{OVAL_WINDOWS_AUDIT_EVENT_SUBCATEGORIES, "audit_event_subcatagories"},
	{OVAL_WINDOWS_FILE, "file"},
	{OVAL_WINDOWS_FILE_AUDITED_PERMISSIONS_53, "file_audited_permissions_53"},
	{OVAL_WINDOWS_FILE_AUDITED_PERMISSIONS, "file_audited_permissions"},
	{OVAL_WINDOWS_FILE_EFFECTIVE_RIGHTS_53, "file_effective_rights_53"},
	{OVAL_WINDOWS_FILE_EFFECTIVE_RIGHTS, "file_effective_rights"},
	{OVAL_WINDOWS_GROUP, "group"},
	{OVAL_WINDOWS_GROUP_SID, "group_sid"},
	{OVAL_WINDOWS_INTERFACE, "interface"},
	{OVAL_WINDOWS_LOCKOUT_POLICY, "lockout_policy"},
	{OVAL_WINDOWS_METABASE, "metabase"},
	{OVAL_WINDOWS_PASSWORD_POLICY, "password_policy"},
	{OVAL_WINDOWS_PORT, "port"},
	{OVAL_WINDOWS_PRINTER_EFFECTIVE_RIGHTS, "printer_effective_rights"},
	{OVAL_WINDOWS_PROCESS, "process"},
	{OVAL_WINDOWS_REGISTRY, "registry"},
	{OVAL_WINDOWS_REGKEY_AUDITED_PERMISSIONS_53, "regkey_audited_permissions_53"},
	{OVAL_WINDOWS_REGKEY_AUDITED_PERMISSIONS, "regkey_audited_permissions"},
	{OVAL_WINDOWS_REGKEY_EFFECTIVE_RIGHTS_53, "regkey_effective_rights_53"},
	{OVAL_WINDOWS_REGKEY_EFFECTIVE_RIGHTS, "regkey_effective_rights"},
	{OVAL_WINDOWS_SHARED_RESOURCE, "shared_resource"},
	{OVAL_WINDOWS_SID, "sid"},
	{OVAL_WINDOWS_SID_SID, "sid_sid"},
	{OVAL_WINDOWS_USER_ACCESS_CONTROL, "user_access_control"},
	{OVAL_WINDOWS_USER, "user"},
	{OVAL_WINDOWS_USER_SID_55, "user_sid_55"},
	{OVAL_WINDOWS_USER_SID, "user_sid"},
	{OVAL_WINDOWS_VOLUME, "volume"},
	{OVAL_WINDOWS_WMI, "wmi"},
	{OVAL_WINDOWS_WUA_UPDATE_SEARCHER, "wua_update_searcher"},
	{OVAL_SUBTYPE_UNKNOWN, NULL}
};

oval_subtype_t oval_subtype_parse(xmlTextReaderPtr reader)
{
	oval_family_t family = oval_family_parse(reader);
	if (family == OVAL_FAMILY_UNKNOWN)
		return OVAL_SUBTYPE_UNKNOWN;

	oval_subtype_t subtype = OVAL_SUBTYPE_UNKNOWN;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *endptr = strrchr(tagname, '_');
	if (endptr == NULL)
		goto cleanup;
	*endptr = '\0';

	const struct oscap_string_map *map = NULL;
	switch (family) {
	case OVAL_FAMILY_AIX:
		map = OVAL_SUBTYPE_AIX_MAP;
		break;
	case OVAL_FAMILY_APACHE:
		map = OVAL_SUBTYPE_APACHE_MAP;
		break;
	case OVAL_FAMILY_CATOS:
		map = OVAL_SUBTYPE_CATOS_MAP;
		break;
	case OVAL_FAMILY_ESX:
		map = OVAL_SUBTYPE_ESX_MAP;
		break;
	case OVAL_FAMILY_FREEBSD:
		map = OVAL_SUBTYPE_FREEBSD_MAP;
		break;
	case OVAL_FAMILY_HPUX:
		map = OVAL_SUBTYPE_HPUX_MAP;
		break;
	case OVAL_FAMILY_INDEPENDENT:
		map = OVAL_SUBTYPE_INDEPENDENT_MAP;
		break;
	case OVAL_FAMILY_IOS:
		map = OVAL_SUBTYPE_IOS_MAP;
		break;
	case OVAL_FAMILY_LINUX:
		map = OVAL_SUBTYPE_LINUX_MAP;
		break;
	case OVAL_FAMILY_MACOS:
		map = OVAL_SUBTYPE_MACOS_MAP;
		break;
	case OVAL_FAMILY_PIXOS:
		map = OVAL_SUBTYPE_PIXOS_MAP;
		break;
	case OVAL_FAMILY_SOLARIS:
		map = OVAL_SUBTYPE_SOLARIS_MAP;
		break;
	case OVAL_FAMILY_UNIX:
		map = OVAL_SUBTYPE_UNIX_MAP;
		break;
	case OVAL_FAMILY_WINDOWS:
		map = OVAL_SUBTYPE_WINDOWS_MAP;
		break;
	default:
		goto cleanup;
	}

	int subtype_s = oscap_string_to_enum(map, tagname);
	if (subtype < 0) {
		dW("Unknown OVAL family subtype: %s\n", tagname);
		subtype = OVAL_ENUMERATION_INVALID;
	}
	else {
		subtype = subtype_s;
	}

 cleanup:
	oscap_free(tagname);
	return subtype;
}

oval_family_t oval_subtype_get_family(oval_subtype_t subtype)
{
	return (subtype / 1000) * 1000;
}

const char *oval_subtype_get_text(oval_subtype_t subtype)
{
	const struct oscap_string_map *map = NULL;
	switch (oval_subtype_get_family(subtype)) {
	case OVAL_FAMILY_AIX:
		map = OVAL_SUBTYPE_AIX_MAP;
		break;
	case OVAL_FAMILY_APACHE:
		map = OVAL_SUBTYPE_APACHE_MAP;
		break;
	case OVAL_FAMILY_CATOS:
		map = OVAL_SUBTYPE_CATOS_MAP;
		break;
	case OVAL_FAMILY_ESX:
		map = OVAL_SUBTYPE_ESX_MAP;
		break;
	case OVAL_FAMILY_FREEBSD:
		map = OVAL_SUBTYPE_FREEBSD_MAP;
		break;
	case OVAL_FAMILY_HPUX:
		map = OVAL_SUBTYPE_HPUX_MAP;
		break;
	case OVAL_FAMILY_INDEPENDENT:
		map = OVAL_SUBTYPE_INDEPENDENT_MAP;
		break;
	case OVAL_FAMILY_IOS:
		map = OVAL_SUBTYPE_IOS_MAP;
		break;
	case OVAL_FAMILY_LINUX:
		map = OVAL_SUBTYPE_LINUX_MAP;
		break;
	case OVAL_FAMILY_MACOS:
		map = OVAL_SUBTYPE_MACOS_MAP;
		break;
	case OVAL_FAMILY_PIXOS:
		map = OVAL_SUBTYPE_PIXOS_MAP;
		break;
	case OVAL_FAMILY_SOLARIS:
		map = OVAL_SUBTYPE_SOLARIS_MAP;
		break;
	case OVAL_FAMILY_UNIX:
		map = OVAL_SUBTYPE_UNIX_MAP;
		break;
	case OVAL_FAMILY_WINDOWS:
		map = OVAL_SUBTYPE_WINDOWS_MAP;
		break;
	default:
		map = NULL;
	}

	if (map) {
		return oval_enumeration_get_text(map, subtype);
	} else {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Warning: Zero family index");
		return _invalid;
	}
}

static const struct oscap_string_map OVAL_RESULT_MAP[] = {
	{OVAL_RESULT_TRUE, "true"},
	{OVAL_RESULT_FALSE, "false"},
	{OVAL_RESULT_UNKNOWN, "unknown"},
	{OVAL_RESULT_ERROR, "error"},
	{OVAL_RESULT_NOT_EVALUATED, "not evaluated"},
	{OVAL_RESULT_NOT_APPLICABLE, "not applicable"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

oval_result_t oval_result_parse(xmlTextReaderPtr reader, char *attname, oval_result_t defval)
{
	return oval_enumeration_attr(reader, attname, OVAL_RESULT_MAP, defval);
}

const char *oval_result_get_text(oval_result_t result)
{
	return oval_enumeration_get_text(OVAL_RESULT_MAP, result);
}


static const struct oscap_string_map OVAL_DEFINITION_CLASS_MAP[] = {
        {OVAL_CLASS_COMPLIANCE, "compliance"},
        {OVAL_CLASS_INVENTORY, "inventory"},
        {OVAL_CLASS_MISCELLANEOUS, "miscellaneous"},
        {OVAL_CLASS_PATCH, "patch"},
        {OVAL_CLASS_VULNERABILITY, "vulnerability"},
        {OVAL_CLASS_UNKNOWN, NULL}
};

oval_definition_class_t oval_definition_class_enum(char *class)
{
        return oscap_string_to_enum(OVAL_DEFINITION_CLASS_MAP, class);
}

const char *oval_definition_class_text(oval_definition_class_t class)
{
        return oscap_enum_to_string(OVAL_DEFINITION_CLASS_MAP, class);
}

