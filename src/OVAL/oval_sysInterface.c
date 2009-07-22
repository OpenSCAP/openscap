/**
 * @file oval_sysInterface.c
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
#include "oval_system_characteristics_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_sysint {
	char *name;
	char *ipAddress;
	char *macAddress;
} oval_sysint_t;

int oval_iterator_sysint_has_more(struct oval_iterator_sysint *oc_sysint)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysint);
}

struct oval_sysint *oval_iterator_sysint_next(struct oval_iterator_sysint
					      *oc_sysint)
{
	return (struct oval_sysint *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysint);
}

char *oval_sysint_name(struct oval_sysint *sysint)
{
	return sysint->name;
}

void set_oval_sysint_name(struct oval_sysint *sysint, char *name)
{
	sysint->name = name;
}

char *oval_sysint_ip_address(struct oval_sysint *sysint)
{
	return ((struct oval_sysint *)sysint)->ipAddress;
}

void set_oval_sysint_ip_address(struct oval_sysint *sysint, char *ip_address)
{
	sysint->ipAddress = ip_address;
}

char *oval_sysint_mac_address(struct oval_sysint *sysint)
{
	return ((struct oval_sysint *)sysint)->macAddress;
}

void set_oval_sysint_mac_address(struct oval_sysint *sysint, char *mac_address)
{
	sysint->macAddress = mac_address;
}

oval_sysint_t *oval_sysint_new()
{
	oval_sysint_t *sysint = (oval_sysint_t*)malloc(sizeof(oval_sysint_t));
	sysint->ipAddress  = NULL;
	sysint->macAddress = NULL;
	sysint->name       = NULL;
	return sysint;
}

void oval_sysint_free(struct oval_sysint *sysint)
{
	if(sysint->ipAddress !=NULL)free(sysint->ipAddress);
	if(sysint->macAddress!=NULL)free(sysint->macAddress);
	if(sysint->name      !=NULL)free(sysint->name);
	free(sysint);
}
extern const char* NAMESPACE_OVALSYS;

int _oval_sysint_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context , void *user){
	struct oval_sysint *sysint = (struct oval_sysint *)user;
	char *tagname   = xmlTextReaderName(reader);
	char *namespace = xmlTextReaderNamespaceUri(reader);
	int is_ovalsys = strcmp(namespace,NAMESPACE_OVALSYS)==0;
	int return_code;
	if        (is_ovalsys && (strcmp(tagname,"interface_name")==0)) {
		void consume(char* text, void* null)
		{
			set_oval_sysint_name(sysint, text);
		}
		return_code = oval_parser_text_value(reader, context, &consume, NULL);
	} else if(is_ovalsys && (strcmp(tagname,"ip_address")==0)) {
		void consume(char* text, void* null)
		{
			set_oval_sysint_ip_address(sysint, text);
		}
		return_code = oval_parser_text_value(reader, context, &consume, NULL);
	} else if(is_ovalsys && (strcmp(tagname,"mac_address")==0)) {
		void consume(char* text, void* null)
		{
			set_oval_sysint_mac_address(sysint, text);
		}
		return_code = oval_parser_text_value(reader, context, &consume, NULL);
	} else {
		char message[200]; *message = 0;
		sprintf(message, "_oval_sysint_parse_tag:: skipping <%s:%s>",
				namespace, tagname);
		oval_parser_log_warn(context, message);
		return_code = oval_parser_skip_tag(reader, context);
	}
	free(tagname);
	free(namespace);
	return return_code;
}


int oval_sysint_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context, oval_sysint_consumer consumer , void *user){
	struct oval_sysint *sysint = oval_sysint_new();
	char *tagname   = xmlTextReaderName(reader);
	char *namespace = xmlTextReaderNamespaceUri(reader);
	int is_ovalsys = strcmp(namespace,NAMESPACE_OVALSYS)==0;
	int return_code;
	if        (is_ovalsys && (strcmp(tagname,"interface")==0)) {
		return_code = oval_parser_parse_tag(reader, context, &_oval_sysint_parse_tag, sysint);
	} else {
		char message[200]; *message = 0;
		sprintf(message, "oval_sysint_parse_tag:: expecting <interface> skipping <%s:%s>",
				namespace, tagname);
		oval_parser_log_warn(context, message);
		return_code = oval_parser_skip_tag(reader, context);
	}
	free(tagname);
	free(namespace);
	(*consumer)(sysint, user);
	return return_code;
}

void oval_sysint_to_print(struct oval_sysint *sysint, char *indent,
			      int index)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (index == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sINTERFACE.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sINTERFACE[%d].", indent, index);
	/*
	char *name;
	char *ipAddress;
	char *macAddress;
	 */
	{//name
		printf("%sNAME           = %s\n", nxtindent, oval_sysint_name(sysint));
	}
	char* ipadd = oval_sysint_ip_address(sysint);
	if(ipadd!=NULL){//ipaddress
		printf("%sIP_ADDRESS      = %s\n", nxtindent, ipadd);
	}
	char* macadd = oval_sysint_mac_address(sysint);
	if(macadd!=NULL){//mac address
		printf("%sMAC_ADDRESS     = %s\n", nxtindent, macadd);
	}
}
