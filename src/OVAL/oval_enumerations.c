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

typedef struct _oval_enumeration_object {
	int enumval;
} _oval_enumeration_object_t;

void *_oval_enumeration_object(int value)
{
	_oval_enumeration_object_t *starval =
	    (_oval_enumeration_object_t *)
	    malloc(sizeof(_oval_enumeration_object_t));
	starval->enumval = value;
	return (void *)starval;
}

_oval_enumeration_object_t *_oval_enumeration_attval(xmlTextReaderPtr reader,
						     char *attname,
						     struct oval_string_map
						     *map)
{
	char *attval = (char *) (char *) xmlTextReaderGetAttribute(reader, BAD_CAST BAD_CAST attname);
	_oval_enumeration_object_t *starval = NULL;
	if (attval != NULL) {
		starval =
		    (_oval_enumeration_object_t *)
		    oval_string_map_get_value(map, attval);
		if (starval == NULL) {
			printf("NOTICE::@%s=%s::unknown value (line %d)\n",
			       attname, attval,
			       xmlTextReaderGetParserLineNumber(reader));
		}
		free(attval);
	}
	return starval;
}

struct oval_string_map *_oval_syschar_flag_map = NULL;
oval_syschar_collection_flag_enum oval_syschar_flag_parse(xmlTextReaderPtr
							       reader,
							       char *attname,
							       oval_syschar_collection_flag_enum
							       defval)
{

	if (_oval_syschar_flag_map == NULL) {
		_oval_syschar_flag_map = oval_string_map_new();
		oval_string_map_put(_oval_syschar_flag_map, "error",
				    _oval_enumeration_object
				    (SYSCHAR_FLAG_ERROR));
		oval_string_map_put(_oval_syschar_flag_map, "complete",
				    _oval_enumeration_object
				    (SYSCHAR_FLAG_COMPLETE));
		oval_string_map_put(_oval_syschar_flag_map, "incomplete",
				    _oval_enumeration_object
				    (SYSCHAR_FLAG_INCOMPLETE));
		oval_string_map_put(_oval_syschar_flag_map, "does not exist",
				    _oval_enumeration_object
				    (SYSCHAR_FLAG_DOES_NOT_EXIST));
		oval_string_map_put(_oval_syschar_flag_map, "not applicable",
				    _oval_enumeration_object
				    (SYSCHAR_FLAG_NOT_APPLICABLE));
		oval_string_map_put(_oval_syschar_flag_map, "not collected",
				    _oval_enumeration_object
				    (SYSCHAR_FLAG_NOT_COLLECTED));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname,
				     _oval_syschar_flag_map);
	if (starval == NULL) {
		char *attval = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
		if (attval != NULL) {
			printf
			    ("NOTICE:oval_syschar_flag_parse:@%s = %s not resolved\n",
			     attname, attval);
			free(attval);
		}
	}
	return (starval == NULL) ? defval : (oval_syschar_collection_flag_enum)starval->enumval;
}


struct oval_string_map *_oval_syschar_status_map = NULL;
oval_syschar_status_enum oval_syschar_status_parse(xmlTextReaderPtr
							       reader,
							       char *attname,
							       oval_syschar_status_enum defval)
{

	if (_oval_syschar_status_map == NULL) {
		_oval_syschar_status_map = oval_string_map_new();
		oval_string_map_put(_oval_syschar_status_map, "error",
				    _oval_enumeration_object
				    (SYSCHAR_STATUS_ERROR));
		oval_string_map_put(_oval_syschar_status_map, "does not exist",
				    _oval_enumeration_object
				    (SYSCHAR_STATUS_DOES_NOT_EXIST));
		oval_string_map_put(_oval_syschar_status_map, "exists",
				    _oval_enumeration_object
				    (SYSCHAR_STATUS_EXISTS));
		oval_string_map_put(_oval_syschar_status_map, "not collected",
				    _oval_enumeration_object
				    (SYSCHAR_STATUS_NOT_COLLECTED));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname,
				     _oval_syschar_status_map);
	if (starval == NULL) {
		char *attval = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
		if (attval != NULL) {
			printf
			    ("NOTICE:oval_syschar_status_parse:@%s = %s not resolved\n",
			     attname, attval);
			free(attval);
		}
	}
	return (starval == NULL) ? defval : (oval_syschar_collection_flag_enum)starval->enumval;
}

struct oval_string_map *_oval_message_level_map = NULL;
oval_message_level_enum oval_message_level_parse(
			xmlTextReaderPtr reader,
			char *attname,
			oval_message_level_enum defval)
{
	if (_oval_message_level_map == NULL) {
		_oval_message_level_map = oval_string_map_new();
		oval_string_map_put(_oval_message_level_map, "debug",
				    _oval_enumeration_object
				    (OVAL_MESSAGE_LEVEL_DEBUG));
		oval_string_map_put(_oval_message_level_map, "error",
				    _oval_enumeration_object
				    (OVAL_MESSAGE_LEVEL_ERROR));
		oval_string_map_put(_oval_message_level_map, "fatal",
				    _oval_enumeration_object
				    (OVAL_MESSAGE_LEVEL_FATAL));
		oval_string_map_put(_oval_message_level_map, "info",
				    _oval_enumeration_object
				    (OVAL_MESSAGE_LEVEL_INFO));
		oval_string_map_put(_oval_message_level_map, "warning",
				    _oval_enumeration_object
				    (OVAL_MESSAGE_LEVEL_WARNING));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname,
				     _oval_message_level_map);
	if (starval == NULL) {
		char *attval = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
		if (attval != NULL) {
			printf
			    ("NOTICE:oval_message_level_parse:@%s = %s not resolved\n",
			     attname, attval);
			free(attval);
		}
	}
	return (starval == NULL) ? defval : (oval_syschar_collection_flag_enum)starval->enumval;
}

struct oval_string_map *_oval_arithmetic_operation_map = NULL;
oval_arithmetic_operation_enum oval_arithmetic_operation_parse(xmlTextReaderPtr
							       reader,
							       char *attname,
							       oval_arithmetic_operation_enum
							       defval)
{
	if (_oval_arithmetic_operation_map == NULL) {
		_oval_arithmetic_operation_map = oval_string_map_new();
		oval_string_map_put(_oval_arithmetic_operation_map, "add",
				    _oval_enumeration_object
				    (OVAL_ARITHMETIC_ADD));
		oval_string_map_put(_oval_arithmetic_operation_map, "divide",
				    _oval_enumeration_object
				    (OVAL_ARITHMETIC_DIVIDE));
		oval_string_map_put(_oval_arithmetic_operation_map, "multiply",
				    _oval_enumeration_object
				    (OVAL_ARITHMETIC_MULTIPLY));
		oval_string_map_put(_oval_arithmetic_operation_map, "subtract",
				    _oval_enumeration_object
				    (OVAL_ARITHMETIC_SUBTRACT));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname,
				     _oval_arithmetic_operation_map);
	if (starval == NULL) {
		char *attval = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
		if (attval != NULL) {
			printf
			    ("NOTICE:oval_arithmetic_operation_parse:@%s = %s not resolved\n",
			     attname, attval);
			free(attval);
		}
	}
	return (starval == NULL) ? defval : (oval_syschar_collection_flag_enum)starval->enumval;
}

struct oval_string_map *_oval_datetime_format_map = NULL;
oval_datetime_format_enum oval_datetime_format_parse(xmlTextReaderPtr reader,
						     char *attname,
						     oval_arithmetic_operation_enum
						     defval)
{
	if (_oval_datetime_format_map == NULL) {
		_oval_datetime_format_map = oval_string_map_new();
		oval_string_map_put(_oval_datetime_format_map, "day_month_year",
				    _oval_enumeration_object
				    (OVAL_DATETIME_DAY_MONTH_YEAR));
		oval_string_map_put(_oval_datetime_format_map, "month_day_year",
				    _oval_enumeration_object
				    (OVAL_DATETIME_MONTH_DAY_YEAR));
		oval_string_map_put(_oval_datetime_format_map,
				    "seconds_since_epoch",
				    _oval_enumeration_object
				    (OVAL_DATETIME_SECONDS_SINCE_EPOCH));
		oval_string_map_put(_oval_datetime_format_map, "win_filetime",
				    _oval_enumeration_object
				    (OVAL_DATETIME_WIN_FILETIME));
		oval_string_map_put(_oval_datetime_format_map, "year_month_day",
				    _oval_enumeration_object
				    (OVAL_DATETIME_YEAR_MONTH_DAY));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname,
				     _oval_datetime_format_map);
	if (starval == NULL) {
		char *attval = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
		if (attval != NULL) {
			printf
			    ("NOTICE:oval_datetime_format_parse:@%s = %s not resolved\n",
			     attname, attval);
			free(attval);
		}
	}
	return (starval == NULL) ? defval : (oval_syschar_collection_flag_enum)starval->enumval;
}

struct oval_string_map *_oval_set_operation_map = NULL;
oval_set_operation_enum oval_set_operation_parse(xmlTextReaderPtr reader,
						 char *attname,
						 oval_set_operation_enum defval)
{
	if (_oval_set_operation_map == NULL) {
		_oval_set_operation_map = oval_string_map_new();
		oval_string_map_put(_oval_set_operation_map, "COMPLEMENT",
				    _oval_enumeration_object
				    (OVAL_SET_OPERATION_COMPLEMENT));
		oval_string_map_put(_oval_set_operation_map, "INTERSECTION",
				    _oval_enumeration_object
				    (OVAL_SET_OPERATION_INTERSECTION));
		oval_string_map_put(_oval_set_operation_map, "UNION",
				    _oval_enumeration_object
				    (OVAL_SET_OPERATION_UNION));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname, _oval_set_operation_map);
	if (starval == NULL) {
		char *attval = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
		if (attval != NULL) {
			printf
			    ("NOTICE:oval_set_operation_parse:@%s = %s not resolved\n",
			     attname, attval);
			free(attval);
		}
	}
	return (starval == NULL) ? defval : (oval_syschar_collection_flag_enum)starval->enumval;
}

struct oval_string_map *_oval_operation_map = NULL;
oval_operation_enum oval_operation_parse(xmlTextReaderPtr reader, char *attname,
					 oval_operation_enum defval)
{
	if (_oval_operation_map == NULL) {
		_oval_operation_map = oval_string_map_new();
		oval_string_map_put(_oval_operation_map, "bitwise and",
				    _oval_enumeration_object
				    (OPERATION_BITWISE_AND));
		oval_string_map_put(_oval_operation_map, "bitwise or",
				    _oval_enumeration_object
				    (OPERATION_BITWISE_OR));
		oval_string_map_put(_oval_operation_map,
				    "case insensitive equals",
				    _oval_enumeration_object
				    (OPERATION_CASE_INSENSITIVE_EQUALS));
		oval_string_map_put(_oval_operation_map,
				    "case insensitive not equal",
				    _oval_enumeration_object
				    (OPERATION_CASE_INSENSITIVE_NOT_EQUAL));
		oval_string_map_put(_oval_operation_map, "equals",
				    _oval_enumeration_object(OPERATION_EQUALS));
		oval_string_map_put(_oval_operation_map, "greater than",
				    _oval_enumeration_object
				    (OPERATION_GREATER_THAN));
		oval_string_map_put(_oval_operation_map,
				    "greater than or equal",
				    _oval_enumeration_object
				    (OPERATION_GREATER_THAN_OR_EQUAL));
		oval_string_map_put(_oval_operation_map, "less than",
				    _oval_enumeration_object
				    (OPERATION_LESS_THAN));
		oval_string_map_put(_oval_operation_map, "less than or equal",
				    _oval_enumeration_object
				    (OPERATION_LESS_THAN_OR_EQUAL));
		oval_string_map_put(_oval_operation_map, "not equal",
				    _oval_enumeration_object
				    (OPERATION_NOT_EQUAL));
		oval_string_map_put(_oval_operation_map, "pattern match",
				    _oval_enumeration_object
				    (OPERATION_PATTERN_MATCH));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname, _oval_operation_map);
	if (starval == NULL) {
		char *attval = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
		if (attval != NULL) {
			printf
			    ("NOTICE:oval_operation_parse:@%s = %s not resolved\n",
			     attname, attval);
			free(attval);
		}
	}
	return (starval == NULL) ? defval : (oval_syschar_collection_flag_enum)starval->enumval;
}

struct oval_string_map *_oval_check_map = NULL;
oval_check_enum oval_check_parse(xmlTextReaderPtr reader, char *attname,
				 oval_check_enum defval)
{
	if (_oval_check_map == NULL) {
		_oval_check_map = oval_string_map_new();
		oval_string_map_put(_oval_check_map, "all",
				    _oval_enumeration_object(OVAL_CHECK_ALL));
		oval_string_map_put(_oval_check_map, "at least one",
				    _oval_enumeration_object
				    (OVAL_CHECK_AT_LEAST_ONE));
		oval_string_map_put(_oval_check_map, "none exist",
				    _oval_enumeration_object
				    (OVAL_CHECK_NONE_EXIST));
		oval_string_map_put(_oval_check_map, "none satisfy",
				    _oval_enumeration_object
				    (OVAL_CHECK_NONE_SATISFY));
		oval_string_map_put(_oval_check_map, "only one",
				    _oval_enumeration_object
				    (OVAL_CHECK_ONLY_ONE));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname, _oval_check_map);
	if (starval == NULL) {
		char *attval = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
		if (attval != NULL) {
			printf
			    ("NOTICE:oval_check_parse:@%s = %s not resolved\n",
			     attname, attval);
			free(attval);
		}
	}
	return (starval == NULL) ? defval : (oval_syschar_collection_flag_enum)starval->enumval;
}

struct oval_string_map *_oval_datatype_map = NULL;
oval_datatype_enum oval_datatype_parse(xmlTextReaderPtr reader, char *attname,
				       oval_datatype_enum defval)
{
	if (_oval_datatype_map == NULL) {
		_oval_datatype_map = oval_string_map_new();
		oval_string_map_put(_oval_datatype_map, "binary",
				    _oval_enumeration_object
				    (OVAL_DATATYPE_BINARY));
		oval_string_map_put(_oval_datatype_map, "boolean",
				    _oval_enumeration_object
				    (OVAL_DATATYPE_BOOLEAN));
		oval_string_map_put(_oval_datatype_map, "evr_string",
				    _oval_enumeration_object
				    (OVAL_DATATYPE_EVR_STRING));
		oval_string_map_put(_oval_datatype_map, "fileset_revision",
				    _oval_enumeration_object
				    (OVAL_DATATYPE_FILESET_REVISTION));
		oval_string_map_put(_oval_datatype_map, "float",
				    _oval_enumeration_object
				    (OVAL_DATATYPE_FLOAT));
		oval_string_map_put(_oval_datatype_map, "ios_version",
				    _oval_enumeration_object
				    (OVAL_DATATYPE_IOS_VERSION));
		oval_string_map_put(_oval_datatype_map, "version",
				    _oval_enumeration_object
				    (OVAL_DATATYPE_VERSION));
		oval_string_map_put(_oval_datatype_map, "int",
				    _oval_enumeration_object
				    (OVAL_DATATYPE_INTEGER));
		oval_string_map_put(_oval_datatype_map, "string",
				    _oval_enumeration_object
				    (OVAL_DATATYPE_STRING));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname, _oval_datatype_map);
	if (starval == NULL) {
		char *attval = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
		if (attval != NULL) {
			printf
			    ("NOTICE:oval_datatype_parse:@%s = %s not resolved\n",
			     attname, attval);
			free(attval);
		}
	}
	return (starval == NULL) ? defval : (oval_syschar_collection_flag_enum)starval->enumval;
}

struct oval_string_map *_oval_existence_map = NULL;
oval_existence_enum oval_existence_parse(xmlTextReaderPtr reader, char *attname,
					 oval_existence_enum defval)
{
	if (_oval_existence_map == NULL) {
		_oval_existence_map = oval_string_map_new();
		oval_string_map_put(_oval_existence_map, "all_exist",
				    _oval_enumeration_object(ALL_EXIST));
		oval_string_map_put(_oval_existence_map, "any_exist",
				    _oval_enumeration_object(ANY_EXIST));
		oval_string_map_put(_oval_existence_map, "at_least_one_exists",
				    _oval_enumeration_object
				    (AT_LEAST_ONE_EXISTS));
		oval_string_map_put(_oval_existence_map, "only_one_exists",
				    _oval_enumeration_object(ONLY_ONE_EXISTS));
		oval_string_map_put(_oval_existence_map, "none_exist",
				    _oval_enumeration_object(NONE_EXIST));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname, _oval_existence_map);
	oval_syschar_collection_flag_enum returns;
	if (starval == NULL) {
		char *attval =
		    (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "check_existence");
		if (attval != NULL) {
			printf
			    ("NOTICE:oval_existence_parse:@check_existence = %s not resolved\n",
			     attval);
			free(attval);
		}
		returns = defval;
	} else
		returns = (oval_syschar_collection_flag_enum)starval->enumval;
	return returns;
}

struct oval_string_map *_oval_operator_map = NULL;
oval_operator_enum oval_operator_parse(xmlTextReaderPtr reader, char *attname,
				       oval_operator_enum defval)
{
	if (_oval_operator_map == NULL) {
		_oval_operator_map = oval_string_map_new();
		oval_string_map_put(_oval_operator_map, "AND",
				    _oval_enumeration_object(OPERATOR_AND));
		oval_string_map_put(_oval_operator_map, "ONE",
				    _oval_enumeration_object(OPERATOR_ONE));
		oval_string_map_put(_oval_operator_map, "OR",
				    _oval_enumeration_object(OPERATOR_OR));
		oval_string_map_put(_oval_operator_map, "XOR",
				    _oval_enumeration_object(OPERATOR_XOR));
	}
	_oval_enumeration_object_t *starval =
	    _oval_enumeration_attval(reader, attname, _oval_operator_map);
	return (starval == NULL) ? defval : (oval_syschar_collection_flag_enum)starval->enumval;
}

struct oval_string_map *_oval_family_map = NULL;
oval_family_enum oval_family_parse(xmlTextReaderPtr reader)
{
	if (_oval_family_map == NULL) {
		_oval_family_map = oval_string_map_new();
		oval_string_map_put(_oval_family_map, "aix",
				    _oval_enumeration_object(FAMILY_AIX));
		oval_string_map_put(_oval_family_map, "apache",
				    _oval_enumeration_object(FAMILY_APACHE));
		oval_string_map_put(_oval_family_map, "catos",
				    _oval_enumeration_object(FAMILY_CATOS));
		oval_string_map_put(_oval_family_map, "esx",
				    _oval_enumeration_object(FAMILY_ESX));
		oval_string_map_put(_oval_family_map, "freebsd",
				    _oval_enumeration_object(FAMILY_FREEBSD));
		oval_string_map_put(_oval_family_map, "hpux",
				    _oval_enumeration_object(FAMILY_HPUX));
		oval_string_map_put(_oval_family_map, "independent",
				    _oval_enumeration_object
				    (FAMILY_INDEPENDENT));
		oval_string_map_put(_oval_family_map, "ios",
				    _oval_enumeration_object(FAMILY_IOS));
		oval_string_map_put(_oval_family_map, "linux",
				    _oval_enumeration_object(FAMILY_LINUX));
		oval_string_map_put(_oval_family_map, "macos",
				    _oval_enumeration_object(FAMILY_MACOS));
		oval_string_map_put(_oval_family_map, "pixos",
				    _oval_enumeration_object(FAMILY_PIXOS));
		oval_string_map_put(_oval_family_map, "solaris",
				    _oval_enumeration_object(FAMILY_SOLARIS));
		oval_string_map_put(_oval_family_map, "unix",
				    _oval_enumeration_object(FAMILY_UNIX));
		oval_string_map_put(_oval_family_map, "windows",
				    _oval_enumeration_object(FAMILY_WINDOWS));
	}
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int idx;
	for (idx = strlen(namespace); namespace[idx] != '#'; idx--) ;
	char *family_text = namespace + idx + 1;
	_oval_enumeration_object_t *starval =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_family_map, family_text);
	oval_family_enum family;
	if (starval == NULL) {
		printf("NOTICE:oval_family_parse:%s not a registered family\n",
		       family_text);
		family = FAMILY_UNKNOWN;
	} else
		family = starval->enumval;
	free(namespace);
	return family;
}

struct oval_string_map *_oval_subtype_aix_map = NULL;
_oval_enumeration_object_t *_oval_subtype_aix(char *type_text)
{
	if (_oval_subtype_aix_map == NULL) {
		_oval_subtype_aix_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_aix_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_apache_map = NULL;
_oval_enumeration_object_t *_oval_subtype_apache(char *type_text)
{
	if (_oval_subtype_apache_map == NULL) {
		_oval_subtype_apache_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_apache_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_catos_map = NULL;
_oval_enumeration_object_t *_oval_subtype_catos(char *type_text)
{
	if (_oval_subtype_catos_map == NULL) {
		_oval_subtype_catos_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_catos_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_esx_map = NULL;
_oval_enumeration_object_t *_oval_subtype_esx(char *type_text)
{
	if (_oval_subtype_esx_map == NULL) {
		_oval_subtype_esx_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_esx_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_freebsd_map = NULL;
_oval_enumeration_object_t *_oval_subtype_freebsd(char *type_text)
{
	if (_oval_subtype_freebsd_map == NULL) {
		_oval_subtype_freebsd_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_freebsd_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_hpux_map = NULL;
_oval_enumeration_object_t *_oval_subtype_hpux(char *type_text)
{
	if (_oval_subtype_hpux_map == NULL) {
		_oval_subtype_hpux_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_hpux_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_independent_map = NULL;
_oval_enumeration_object_t *_oval_subtype_independent(char *type_text)
{
	if (_oval_subtype_independent_map == NULL) {
		_oval_subtype_independent_map = oval_string_map_new();
		oval_string_map_put(_oval_subtype_independent_map,
				    "environmentvariable",
				    _oval_enumeration_object
				    (INDEPENDENT_ENVIRONMENT_VARIABLE));
		oval_string_map_put(_oval_subtype_independent_map, "family",
				    _oval_enumeration_object
				    (INDEPENDENT_FAMILY));
		oval_string_map_put(_oval_subtype_independent_map, "filehash",
				    _oval_enumeration_object
				    (INDEPENDENT_FILE_HASH));
		oval_string_map_put(_oval_subtype_independent_map, "filemd5",
				    _oval_enumeration_object
				    (INDEPENDENT_FILE_MD5));
		oval_string_map_put(_oval_subtype_independent_map, "sql",
				    _oval_enumeration_object(INDEPENDENT_SQL));
		oval_string_map_put(_oval_subtype_independent_map,
				    "textfilecontent54",
				    _oval_enumeration_object
				    (INDEPENDENT_TEXT_FILE_CONTENT_54));
		oval_string_map_put(_oval_subtype_independent_map,
				    "textfilecontent",
				    _oval_enumeration_object
				    (INDEPENDENT_TEXT_FILE_CONTENT));
		oval_string_map_put(_oval_subtype_independent_map, "variable",
				    _oval_enumeration_object
				    (INDEPENDENT_VARIABLE));
		oval_string_map_put(_oval_subtype_independent_map, "unknown",
				    _oval_enumeration_object
				    (INDEPENDENT_UNKNOWN));
		oval_string_map_put(_oval_subtype_independent_map,
				    "xmlfilecontent",
				    _oval_enumeration_object
				    (INDEPENDENT_XML_FILE_CONTENT));
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_independent_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_ios_map = NULL;
_oval_enumeration_object_t *_oval_subtype_ios(char *type_text)
{
	if (_oval_subtype_ios_map == NULL) {
		_oval_subtype_ios_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_ios_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_linux_map = NULL;
_oval_enumeration_object_t *_oval_subtype_linux(char *type_text)
{
	if (_oval_subtype_linux_map == NULL) {
		_oval_subtype_linux_map = oval_string_map_new();
		oval_string_map_put(_oval_subtype_linux_map, "dpkginfo",
				    _oval_enumeration_object(LINUX_DPKG_INFO));
		oval_string_map_put(_oval_subtype_linux_map,
				    "inetlisteningservers",
				    _oval_enumeration_object
				    (LINUX_INET_LISTENING_SERVERS));
		oval_string_map_put(_oval_subtype_linux_map, "rpminfo",
				    _oval_enumeration_object(LINUX_RPM_INFO));
		oval_string_map_put(_oval_subtype_linux_map, "slackwarepkginfo",
				    _oval_enumeration_object
				    (LINUX_SLACKWARE_PKG_INFO_TEST));
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_linux_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_macos_map = NULL;
_oval_enumeration_object_t *_oval_subtype_macos(char *type_text)
{
	if (_oval_subtype_macos_map == NULL) {
		_oval_subtype_macos_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_macos_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_pixos_map = NULL;
_oval_enumeration_object_t *_oval_subtype_pixos(char *type_text)
{
	if (_oval_subtype_pixos_map == NULL) {
		_oval_subtype_pixos_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_pixos_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_solaris_map = NULL;
_oval_enumeration_object_t *_oval_subtype_solaris(char *type_text)
{
	if (_oval_subtype_solaris_map == NULL) {
		_oval_subtype_solaris_map = oval_string_map_new();
		//TODO
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_solaris_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_unix_map = NULL;
_oval_enumeration_object_t *_oval_subtype_unix(char *type_text)
{
	if (_oval_subtype_unix_map == NULL) {
		_oval_subtype_unix_map = oval_string_map_new();
		oval_string_map_put(_oval_subtype_unix_map, "file",
				    _oval_enumeration_object(UNIX_FILE));
		oval_string_map_put(_oval_subtype_unix_map, "inetd",
				    _oval_enumeration_object(UNIX_INETD));
		oval_string_map_put(_oval_subtype_unix_map, "interface",
				    _oval_enumeration_object(UNIX_INTERFACE));
		oval_string_map_put(_oval_subtype_unix_map, "password",
				    _oval_enumeration_object(UNIX_PASSWORD));
		oval_string_map_put(_oval_subtype_unix_map, "process",
				    _oval_enumeration_object(UNIX_PROCESS));
		oval_string_map_put(_oval_subtype_unix_map, "runlevel",
				    _oval_enumeration_object(UNIX_RUNLEVEL));
		oval_string_map_put(_oval_subtype_unix_map, "sccs",
				    _oval_enumeration_object(UNIX_SCCS));
		oval_string_map_put(_oval_subtype_unix_map, "shadow",
				    _oval_enumeration_object(UNIX_SHADOW));
		oval_string_map_put(_oval_subtype_unix_map, "uname",
				    _oval_enumeration_object(UNIX_UNAME));
		oval_string_map_put(_oval_subtype_unix_map, "xinetd",
				    _oval_enumeration_object(UNIX_XINETD));
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_unix_map, type_text);
	return vstar;
}

struct oval_string_map *_oval_subtype_windows_map = NULL;
_oval_enumeration_object_t *_oval_subtype_windows(char *type_text)
{
	if (_oval_subtype_windows_map == NULL) {
		_oval_subtype_windows_map = oval_string_map_new();
		oval_string_map_put(_oval_subtype_windows_map, "access_token",
				    _oval_enumeration_object
				    (WINDOWS_ACCESS_TOKEN));
		oval_string_map_put(_oval_subtype_windows_map,
				    "active_directory",
				    _oval_enumeration_object
				    (WINDOWS_ACTIVE_DIRECTORY));
		oval_string_map_put(_oval_subtype_windows_map,
				    "audit_event_policy",
				    _oval_enumeration_object
				    (WINDOWS_AUDIT_EVENT_POLICY));
		oval_string_map_put(_oval_subtype_windows_map,
				    "audit_event_subcatagories",
				    _oval_enumeration_object
				    (WINDOWS_AUDIT_EVENT_SUBCATEGORIES));
		oval_string_map_put(_oval_subtype_windows_map, "file",
				    _oval_enumeration_object(WINDOWS_FILE));
		oval_string_map_put(_oval_subtype_windows_map,
				    "file_audited_permissions_53",
				    _oval_enumeration_object
				    (WINDOWS_FILE_AUDITED_PERMISSIONS_53));
		oval_string_map_put(_oval_subtype_windows_map,
				    "file_audited_permissions",
				    _oval_enumeration_object
				    (WINDOWS_FILE_AUDITED_PERMISSIONS));
		oval_string_map_put(_oval_subtype_windows_map,
				    "file_effective_rights_53",
				    _oval_enumeration_object
				    (WINDOWS_FILE_EFFECTIVE_RIGHTS_53));
		oval_string_map_put(_oval_subtype_windows_map,
				    "file_effective_rights",
				    _oval_enumeration_object
				    (WINDOWS_FILE_EFFECTIVE_RIGHTS));
		oval_string_map_put(_oval_subtype_windows_map, "group",
				    _oval_enumeration_object(WINDOWS_GROUP));
		oval_string_map_put(_oval_subtype_windows_map, "group_sid",
				    _oval_enumeration_object
				    (WINDOWS_GROUP_SID));
		oval_string_map_put(_oval_subtype_windows_map, "interface",
				    _oval_enumeration_object
				    (WINDOWS_INTERFACE));
		oval_string_map_put(_oval_subtype_windows_map, "lockout_policy",
				    _oval_enumeration_object
				    (WINDOWS_LOCKOUT_POLICY));
		oval_string_map_put(_oval_subtype_windows_map, "metabase",
				    _oval_enumeration_object(WINDOWS_METABASE));
		oval_string_map_put(_oval_subtype_windows_map,
				    "password_policy",
				    _oval_enumeration_object
				    (WINDOWS_PASSWORD_POLICY));
		oval_string_map_put(_oval_subtype_windows_map, "port",
				    _oval_enumeration_object(WINDOWS_PORT));
		oval_string_map_put(_oval_subtype_windows_map,
				    "printer_effective_rights",
				    _oval_enumeration_object
				    (WINDOWS_PRINTER_EFFECTIVE_RIGHTS));
		oval_string_map_put(_oval_subtype_windows_map, "process",
				    _oval_enumeration_object(WINDOWS_PROCESS));
		oval_string_map_put(_oval_subtype_windows_map, "registry",
				    _oval_enumeration_object(WINDOWS_REGISTRY));
		oval_string_map_put(_oval_subtype_windows_map,
				    "regkey_audited_permissions_53",
				    _oval_enumeration_object
				    (WINDOWS_REGKEY_AUDITED_PERMISSIONS_53));
		oval_string_map_put(_oval_subtype_windows_map,
				    "regkey_audited_permissions",
				    _oval_enumeration_object
				    (WINDOWS_REGKEY_AUDITED_PERMISSIONS));
		oval_string_map_put(_oval_subtype_windows_map,
				    "regkey_effective_rights_53",
				    _oval_enumeration_object
				    (WINDOWS_REGKEY_EFFECTIVE_RIGHTS_53));
		oval_string_map_put(_oval_subtype_windows_map,
				    "regkey_effective_rights",
				    _oval_enumeration_object
				    (WINDOWS_REGKEY_EFFECTIVE_RIGHTS));
		oval_string_map_put(_oval_subtype_windows_map,
				    "shared_resource",
				    _oval_enumeration_object
				    (WINDOWS_SHARED_RESOURCE));
		oval_string_map_put(_oval_subtype_windows_map, "std",
				    _oval_enumeration_object(WINDOWS_SID));
		oval_string_map_put(_oval_subtype_windows_map, "sid_sid",
				    _oval_enumeration_object(WINDOWS_SID_SID));
		oval_string_map_put(_oval_subtype_windows_map,
				    "user_access_control",
				    _oval_enumeration_object
				    (WINDOWS_USER_ACCESS_CONTROL));
		oval_string_map_put(_oval_subtype_windows_map, "user",
				    _oval_enumeration_object(WINDOWS_USER));
		oval_string_map_put(_oval_subtype_windows_map, "user_sid_55",
				    _oval_enumeration_object
				    (WINDOWS_USER_SID_55));
		oval_string_map_put(_oval_subtype_windows_map, "user_sid",
				    _oval_enumeration_object(WINDOWS_USER_SID));
		oval_string_map_put(_oval_subtype_windows_map, "volume",
				    _oval_enumeration_object(WINDOWS_VOLUME));
		oval_string_map_put(_oval_subtype_windows_map, "wmi",
				    _oval_enumeration_object(WINDOWS_WMI));
		oval_string_map_put(_oval_subtype_windows_map,
				    "wua_update_searcher",
				    _oval_enumeration_object
				    (WINDOWS_WUA_UPDATE_SEARCHER));
	}
	_oval_enumeration_object_t *vstar =
	    (_oval_enumeration_object_t *)
	    oval_string_map_get_value(_oval_subtype_windows_map, type_text);
	return vstar;
}

oval_subtype_enum oval_subtype_parse(xmlTextReaderPtr reader)
{
	oval_family_enum family = oval_family_parse(reader);
	oval_subtype_enum subtype;
	if (family == FAMILY_UNKNOWN) {
		subtype = OVAL_SUBTYPE_UNKNOWN;
	} else {
		char *tagname = (char*) xmlTextReaderLocalName(reader);
		int idx;
		for (idx = strlen(tagname); tagname[idx] != '_'; idx--) ;
		tagname[idx] = 0;
		_oval_enumeration_object_t *starval;
		switch (family) {
		case FAMILY_AIX:
			starval = _oval_subtype_aix(tagname);
			break;
		case FAMILY_APACHE:
			starval = _oval_subtype_apache(tagname);
			break;
		case FAMILY_CATOS:
			starval = _oval_subtype_catos(tagname);
			break;
		case FAMILY_ESX:
			starval = _oval_subtype_esx(tagname);
			break;
		case FAMILY_FREEBSD:
			starval = _oval_subtype_freebsd(tagname);
			break;
		case FAMILY_HPUX:
			starval = _oval_subtype_hpux(tagname);
			break;
		case FAMILY_INDEPENDENT:
			starval = _oval_subtype_independent(tagname);
			break;
		case FAMILY_IOS:
			starval = _oval_subtype_ios(tagname);
			break;
		case FAMILY_LINUX:
			starval = _oval_subtype_linux(tagname);
			break;
		case FAMILY_MACOS:
			starval = _oval_subtype_macos(tagname);
			break;
		case FAMILY_PIXOS:
			starval = _oval_subtype_pixos(tagname);
			break;
		case FAMILY_SOLARIS:
			starval = _oval_subtype_solaris(tagname);
			break;
		case FAMILY_UNIX:
			starval = _oval_subtype_unix(tagname);
			break;
		case FAMILY_WINDOWS:
			starval = _oval_subtype_windows(tagname);
			break;
		default:
			starval = NULL;
		}
		if (starval == NULL) {
			printf
			    ("NOTICE:oval_subtype_parse: %s(family = %d) not a registered subtype\n",
			     tagname, family);
			subtype = OVAL_SUBTYPE_UNKNOWN;
		} else
			subtype = starval->enumval;
		free(tagname);
	}
	return subtype;
}
