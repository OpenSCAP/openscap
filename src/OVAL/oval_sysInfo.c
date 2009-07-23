/**
 * @file oval_sysInfo.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "oval_system_characteristics_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_sysinfo {
	char *osName;
	char *osVersion;
	char *osArchitecture;
	char *primaryHostName;
	struct oval_collection *interfaces;
} oval_sysinfo_t;

struct oval_sysinfo *oval_sysinfo_new(){
	oval_sysinfo_t *sysinfo = (oval_sysinfo_t*)malloc(sizeof(oval_sysinfo_t));
	sysinfo->osArchitecture  = NULL;
	sysinfo->osName          = NULL;
	sysinfo->osVersion       = NULL;
	sysinfo->primaryHostName = NULL;
	sysinfo->interfaces      = oval_collection_new();
	return sysinfo;
}

void oval_sysinfo_free(struct oval_sysinfo *sysinfo){
	if(sysinfo->osArchitecture  != NULL) free(sysinfo->osArchitecture);
	if(sysinfo->osName          != NULL) free(sysinfo->osName);
	if(sysinfo->osVersion       != NULL) free(sysinfo->osVersion);
	if(sysinfo->primaryHostName != NULL) free(sysinfo->primaryHostName);
	free(sysinfo);
}

int oval_iterator_sysinfo_has_more(struct oval_iterator_sysinfo *oc_sysinfo)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysinfo);
}

struct oval_sysinfo *oval_iterator_sysinfo_next(struct oval_iterator_sysinfo
						*oc_sysinfo)
{
	return (struct oval_sysinfo *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysinfo);
}

char *oval_sysinfo_os_name(struct oval_sysinfo *sysinfo)
{
	return sysinfo->osName;
}

void set_oval_sysinfo_os_name(struct oval_sysinfo *sysinfo, char *osName){
	sysinfo->osName = osName;
}

char *oval_sysinfo_os_version(struct oval_sysinfo *sysinfo)
{
	return sysinfo->osVersion;
}

void set_oval_sysinfo_os_version(struct oval_sysinfo *sysinfo, char *osVersion)
{
	sysinfo->osVersion = osVersion;
}

char *oval_sysinfo_os_architecture(struct oval_sysinfo *sysinfo)
{
	return sysinfo->osArchitecture;
}

void set_oval_sysinfo_os_architecture(struct oval_sysinfo *sysinfo, char *osArchitecture)
{
	sysinfo->osArchitecture = osArchitecture;
}

char *oval_sysinfo_primary_host_name(struct oval_sysinfo *sysinfo)
{
	return sysinfo->primaryHostName;
}

void set_oval_sysinfo_primary_host_name(struct oval_sysinfo *sysinfo, char *primaryHostName)
{
	sysinfo->primaryHostName = primaryHostName;
}

struct oval_iterator_sysint *oval_sysinfo_interfaces(struct oval_sysinfo
						     *sysinfo)
{
	return (struct oval_iterator_sysint *)oval_collection_iterator(sysinfo->
								       interfaces);
}

void add_oval_sysinfo_interfaces(struct oval_sysinfo *sysinfo, struct oval_sysint *interface)
{
	oval_collection_add(sysinfo->interfaces, interface);
}

extern const char* NAMESPACE_OVALSYS;

int _oval_sysinfo_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context, void *user)
{
	struct oval_sysinfo *sysinfo = (struct oval_sysinfo *)user;
	char *tagname   = (char*) xmlTextReaderName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int is_ovalsys = strcmp(namespace,NAMESPACE_OVALSYS)==0;
	int return_code;
	if        (is_ovalsys && (strcmp(tagname,"os_name"          )==0)) {
		void consume(char* text, void* null)
		{
			set_oval_sysinfo_os_name(sysinfo, text);
		}
		return_code = oval_parser_text_value(reader, context, &consume, NULL);
	} else if (is_ovalsys && (strcmp(tagname,"os_version"       )==0)) {
		void consume(char* text, void* null)
		{
			set_oval_sysinfo_os_version(sysinfo, text);
		}
		return_code = oval_parser_text_value(reader, context, &consume, NULL);
	} else if (is_ovalsys && (strcmp(tagname,"architecture"     )==0)) {
		void consume(char* text, void* null)
		{
			set_oval_sysinfo_os_architecture(sysinfo, text);
		}
		return_code = oval_parser_text_value(reader, context, &consume, NULL);
	} else if (is_ovalsys && (strcmp(tagname,"primary_host_name")==0)) {
		void consume(char* text, void* null)
		{
			set_oval_sysinfo_primary_host_name(sysinfo, text);
		}
		return_code = oval_parser_text_value(reader, context, &consume, NULL);
	} else if (is_ovalsys && (strcmp(tagname,"interfaces")==0)) {
		void consume_int(struct oval_sysint *sysint, void *null)
		{
			add_oval_sysinfo_interfaces(sysinfo, sysint);
		}
		int parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context, void *null)
		{
			return oval_sysint_parse_tag
					(reader, context, &consume_int, NULL);
		}
		return_code = oval_parser_parse_tag
			(reader, context, &parse_tag, sysinfo);
	} else {
		char message[200]; *message = 0;
		sprintf(message, "_oval_sysinfo_parse_tag:: skipping <%s:%s>",
				namespace, tagname);
		oval_parser_log_warn(context, message);
		return_code = oval_parser_skip_tag(reader, context);
	}
	free(tagname);
	free(namespace);
	return return_code;
}


int oval_sysinfo_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context)
{
	oval_sysinfo_t *sysinfo = oval_sysinfo_new();
	char *tagname   = (char*) xmlTextReaderName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int is_ovalsys = strcmp(namespace,NAMESPACE_OVALSYS)==0;
	int return_code;
	if(is_ovalsys){
		return_code = oval_parser_parse_tag(reader, context, &_oval_sysinfo_parse_tag, sysinfo);
	}else{
		char message[200]; *message = 0;
		sprintf(message, "oval_sysinfo_parse_tag:: expecting <system_info> got <%s:%s>",
				namespace, tagname);
		oval_parser_log_warn(context, message);
		return_code = oval_parser_skip_tag(reader, context);
	}
	free(tagname);
	free(namespace);
	if(return_code!=1){
		char message[200]; *message = 0;
		sprintf(message, "oval_sysinfo_parse_tag:: return code is not 1::(%d)",return_code);
		oval_parser_log_warn(context, message);
	}
	context->syschar_sysinfo = sysinfo;

	return return_code;
}

void oval_sysinfo_to_print(struct oval_sysinfo *sysinfo, char *indent,
			      int idx)
{

	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sSYSINFO.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sSYSINFO[%d].", indent, idx);

	/*
	char *osName;
	char *osVersion;
	char *osArchitecture;
	char *primaryHostName;
	struct oval_collection *interfaces;
	 */
	{//osName
		printf("%sOS_NAME           = %s\n", nxtindent, oval_sysinfo_os_name(sysinfo));
	}
	{//osVersion
		printf("%sOS_VERSION        = %s\n", nxtindent, oval_sysinfo_os_version(sysinfo));
	}
	{//osArchitecture
		printf("%sOS_ARCHITECTURE   = %s	\n", nxtindent, oval_sysinfo_os_architecture(sysinfo));
	}
	{//host name
		printf("%sPRIMARY_HOST_NAME = %s\n", nxtindent, oval_sysinfo_primary_host_name(sysinfo));
	}
	{//interfaces
		struct oval_iterator_sysint *intrfcs = oval_sysinfo_interfaces(sysinfo);
		int i;for(i=1;oval_iterator_sysint_has_more(intrfcs);i++){
			struct oval_sysint *intrfc = oval_iterator_sysint_next(intrfcs);
			oval_sysint_to_print(intrfc, nxtindent, i);
		}
	}
}
