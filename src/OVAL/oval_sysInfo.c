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
#include "oval_agent_api_impl.h"

typedef struct oval_sysinfo {
	char *osName;
	char *osVersion;
	char *osArchitecture;
	char *primaryHostName;
	struct oval_collection *interfaces;
} oval_sysinfo_t;

struct oval_sysinfo *oval_sysinfo_new()
{
	oval_sysinfo_t *sysinfo = (oval_sysinfo_t*)malloc(sizeof(oval_sysinfo_t));
	sysinfo->osArchitecture  = NULL;
	sysinfo->osName          = NULL;
	sysinfo->osVersion       = NULL;
	sysinfo->primaryHostName = NULL;
	sysinfo->interfaces      = oval_collection_new();
	return sysinfo;
}

struct oval_sysinfo *oval_sysinfo_clone(struct oval_sysinfo *old_sysinfo)
{
	struct oval_sysinfo *new_sysinfo = oval_sysinfo_new();
	struct oval_sysint_iterator *interfaces = oval_sysinfo_get_interfaces(old_sysinfo);
	while(oval_sysint_iterator_has_more(interfaces)){
		struct oval_sysint *interface = oval_sysint_iterator_next(interfaces);
		oval_sysinfo_add_interface(new_sysinfo, interface);
	}
	oval_sysint_iterator_free(interfaces);
	char *os_architecture = oval_sysinfo_get_os_architecture(old_sysinfo);
	if(os_architecture){
		oval_sysinfo_set_os_architecture(new_sysinfo, os_architecture);
	}
	char *os_name = oval_sysinfo_get_os_name(old_sysinfo);
	if(os_name){
		oval_sysinfo_set_os_name(new_sysinfo, os_name);
	}
	char *os_version = oval_sysinfo_get_os_version(old_sysinfo);
	if(os_version){
		oval_sysinfo_set_os_version(new_sysinfo, os_version);
	}
	char *host_name = oval_sysinfo_get_primary_host_name(old_sysinfo);
	if(host_name){
		oval_sysinfo_set_primary_host_name(new_sysinfo, host_name);
	}
	return new_sysinfo;
}


void oval_sysinfo_free(struct oval_sysinfo *sysinfo)
{
	if (sysinfo) {
		if(sysinfo->osArchitecture  != NULL) free(sysinfo->osArchitecture);
		if(sysinfo->osName          != NULL) free(sysinfo->osName);
		if(sysinfo->osVersion       != NULL) free(sysinfo->osVersion);
		if(sysinfo->primaryHostName != NULL) free(sysinfo->primaryHostName);
		oval_collection_free_items(sysinfo->interfaces, (oscap_destruct_func)oval_sysint_free);

		sysinfo->interfaces = NULL;
		sysinfo->osArchitecture = NULL;
		sysinfo->osName = NULL;
		sysinfo->osVersion = NULL;
		sysinfo->primaryHostName = NULL;

		free(sysinfo);
	}
}

bool oval_sysinfo_iterator_has_more(struct oval_sysinfo_iterator *oc_sysinfo)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysinfo);
}

struct oval_sysinfo *oval_sysinfo_iterator_next(struct oval_sysinfo_iterator
						*oc_sysinfo)
{
	return (struct oval_sysinfo *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysinfo);
}

void oval_sysinfo_iterator_free(struct oval_sysinfo_iterator *oc_sysinfo)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_sysinfo);
}

char *oval_sysinfo_get_os_name(struct oval_sysinfo *sysinfo)
{
	return sysinfo->osName;
}

void oval_sysinfo_set_os_name(struct oval_sysinfo *sysinfo, char *osName)
{
	if(sysinfo->osName!=NULL)free(sysinfo->osName);
	sysinfo->osName = osName==NULL?NULL:strdup(osName);
}

char *oval_sysinfo_get_os_version(struct oval_sysinfo *sysinfo)
{
	return sysinfo->osVersion;
}

void oval_sysinfo_set_os_version(struct oval_sysinfo *sysinfo, char *osVersion)
{
	if(sysinfo->osVersion!=NULL)free(sysinfo->osVersion);
	sysinfo->osVersion = osVersion==NULL?NULL:strdup(osVersion);
}

char *oval_sysinfo_get_os_architecture(struct oval_sysinfo *sysinfo)
{
	return sysinfo->osArchitecture;
}

void oval_sysinfo_set_os_architecture(struct oval_sysinfo *sysinfo, char *osArchitecture)
{
	if(sysinfo->osArchitecture!=NULL)free(sysinfo->osArchitecture);
	sysinfo->osArchitecture = osArchitecture==NULL?NULL:strdup(osArchitecture);
}

char *oval_sysinfo_get_primary_host_name(struct oval_sysinfo *sysinfo)
{
	return sysinfo->primaryHostName;
}

void oval_sysinfo_set_primary_host_name(struct oval_sysinfo *sysinfo, char *primaryHostName)
{
	if(sysinfo->primaryHostName!=NULL)free(sysinfo->primaryHostName);
	sysinfo->primaryHostName = primaryHostName==NULL?NULL:strdup(primaryHostName);
}

struct oval_sysint_iterator *oval_sysinfo_get_interfaces(struct oval_sysinfo
						     *sysinfo)
{
	return (struct oval_sysint_iterator *)oval_collection_iterator(sysinfo->
								       interfaces);
}

void oval_sysinfo_add_interface(struct oval_sysinfo *sysinfo, struct oval_sysint *interface)
{
	oval_collection_add(sysinfo->interfaces, oval_sysint_clone(interface));
}

		static void _oval_sysinfo_parse_tag_consume_os_name(char* text, void* sysinfo)
		{
			oval_sysinfo_set_os_name(sysinfo, text);
		}
		static void _oval_sysinfo_parse_tag_consume_os_version(char* text, void* sysinfo)
		{
			oval_sysinfo_set_os_version(sysinfo, text);
		}
		static void _oval_sysinfo_parse_tag_consume_os_architecture(char* text, void* sysinfo)
		{
			oval_sysinfo_set_os_architecture(sysinfo, text);
		}
		static void _oval_sysinfo_parse_tag_consume_primary_host_name(char* text, void* sysinfo)
		{
			oval_sysinfo_set_primary_host_name(sysinfo, text);
		}
		static void _oval_sysinfo_parse_tag_consume_int(struct oval_sysint *sysint, void *sysinfo)
		{
			oval_sysinfo_add_interface(sysinfo, sysint);
		}
		static int _oval_sysinfo_parse_tag_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context, void *sysinfo)
		{
			return oval_sysint_parse_tag
					(reader, context, &_oval_sysinfo_parse_tag_consume_int, sysinfo);
		}
static int _oval_sysinfo_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context, void *user)
{
	struct oval_sysinfo *sysinfo = (struct oval_sysinfo *)user;
	char *tagname   = (char*) xmlTextReaderName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int is_ovalsys = strcmp(namespace,NAMESPACE_OVALSYS)==0;
	int return_code;
	if        (is_ovalsys && (strcmp(tagname,"os_name"          )==0)) {
		return_code = oval_parser_text_value(reader, context, &_oval_sysinfo_parse_tag_consume_os_name, sysinfo);
	} else if (is_ovalsys && (strcmp(tagname,"os_version"       )==0)) {
		return_code = oval_parser_text_value(reader, context, &_oval_sysinfo_parse_tag_consume_os_version, sysinfo);
	} else if (is_ovalsys && (strcmp(tagname,"architecture"     )==0)) {
		return_code = oval_parser_text_value(reader, context, &_oval_sysinfo_parse_tag_consume_os_architecture, sysinfo);
	} else if (is_ovalsys && (strcmp(tagname,"primary_host_name")==0)) {
		return_code = oval_parser_text_value(reader, context, &_oval_sysinfo_parse_tag_consume_primary_host_name, sysinfo);
	} else if (is_ovalsys && (strcmp(tagname,"interfaces")==0)) {
		return_code = oval_parser_parse_tag
			(reader, context, &_oval_sysinfo_parse_tag_parse_tag, sysinfo);
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
	oval_syschar_model_set_sysinfo(context->syschar_model, sysinfo);
	oval_sysinfo_free(sysinfo);
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
		printf("%sOS_NAME           = %s\n", nxtindent, oval_sysinfo_get_os_name(sysinfo));
	}
	{//osVersion
		printf("%sOS_VERSION        = %s\n", nxtindent, oval_sysinfo_get_os_version(sysinfo));
	}
	{//osArchitecture
		printf("%sOS_ARCHITECTURE   = %s	\n", nxtindent, oval_sysinfo_get_os_architecture(sysinfo));
	}
	{//host name
		printf("%sPRIMARY_HOST_NAME = %s\n", nxtindent, oval_sysinfo_get_primary_host_name(sysinfo));
	}
	{//interfaces
		struct oval_sysint_iterator *intrfcs = oval_sysinfo_get_interfaces(sysinfo);
		int i;for(i=1;oval_sysint_iterator_has_more(intrfcs);i++){
			struct oval_sysint *intrfc = oval_sysint_iterator_next(intrfcs);
			oval_sysint_to_print(intrfc, nxtindent, i);
		}
		oval_sysint_iterator_free(intrfcs);
	}
}

void oval_sysinfo_to_dom  (struct oval_sysinfo *sysinfo, xmlDoc *doc, xmlNode *tag_parent){
	if(sysinfo){
		xmlNs *ns_syschar = xmlSearchNsByHref(doc, tag_parent, OVAL_SYSCHAR_NAMESPACE);
	    xmlNode *tag_sysinfo = xmlNewChild
			(tag_parent, ns_syschar, BAD_CAST "system_info", NULL);
	    xmlNewChild
			(tag_sysinfo, ns_syschar, BAD_CAST "os_name",
					BAD_CAST oval_sysinfo_get_os_name(sysinfo));
	    xmlNewChild
			(tag_sysinfo, ns_syschar, BAD_CAST "os_version",
					BAD_CAST oval_sysinfo_get_os_version(sysinfo));
	    xmlNewChild
			(tag_sysinfo, ns_syschar, BAD_CAST "architecture",
					BAD_CAST oval_sysinfo_get_os_architecture(sysinfo));
	    xmlNewChild
			(tag_sysinfo, ns_syschar, BAD_CAST "primary_host_name",
					BAD_CAST oval_sysinfo_get_primary_host_name(sysinfo));

	    xmlNode *tag_interfaces = xmlNewChild
			(tag_sysinfo, ns_syschar, BAD_CAST "interfaces", NULL);
		struct oval_sysint_iterator *intrfcs = oval_sysinfo_get_interfaces(sysinfo);
		int i;for(i=1;oval_sysint_iterator_has_more(intrfcs);i++){
			struct oval_sysint *intrfc = oval_sysint_iterator_next(intrfcs);
			oval_sysint_to_dom(intrfc, doc, tag_interfaces);
		}
		oval_sysint_iterator_free(intrfcs);
	}
}

