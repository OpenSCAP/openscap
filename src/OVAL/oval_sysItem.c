/**
 * @file oval_sysItem.c
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
#include "oval_agent_api_impl.h"
#include "oval_system_characteristics_impl.h"
#include "oval_collection_impl.h"

static int DEBUG_OVAL_SYSITEM = 0;

typedef struct oval_sysitem {
	char*              name;
	char*              value;
	int                mask;
	oval_datatype_t datatype;
	oval_syschar_status_t status;
} oval_sysitem_t;

struct oval_sysitem *oval_sysitem_new()
{
	oval_sysitem_t *sysitem = (oval_sysitem_t*)malloc(sizeof(oval_sysitem_t));
	sysitem->name              = NULL;
	sysitem->value             = NULL;
	sysitem->status            = SYSCHAR_STATUS_UNKNOWN;
	sysitem->datatype          = OVAL_DATATYPE_UNKNOWN;
	sysitem->mask              = 0;
	return sysitem;
}

struct oval_sysitem *oval_sysitem_clone(struct oval_sysitem *old_item)
{
	struct oval_sysitem *new_item = oval_sysitem_new();


	char *old_value = oval_sysitem_get_value(old_item);
	if(old_value){
		oval_sysitem_set_value(new_item, strdup(old_value));
	}

	char* old_name = oval_sysitem_get_name(old_item);
	if(old_name){
		oval_sysitem_set_name(new_item, strdup(old_name));
	}

	oval_sysitem_set_datatype(new_item, oval_sysitem_get_datatype(old_item));
	oval_sysitem_set_mask(new_item, oval_sysitem_get_mask(old_item));
	oval_sysitem_set_status(new_item, oval_sysitem_get_status(old_item));

	return new_item;
}

void oval_sysitem_free(struct oval_sysitem *sysitem)
{
	if(sysitem->name   !=NULL)free(sysitem->name);
	if(sysitem->value  !=NULL)free(sysitem->value);

	sysitem->name = NULL;
	sysitem->value = NULL;

	free(sysitem);
}

int oval_sysitem_iterator_has_more(struct oval_sysitem_iterator *oc_sysitem)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysitem);
}

struct oval_sysitem *oval_sysitem_iterator_next(struct oval_sysitem_iterator *oc_sysitem)
{
	return (struct oval_sysitem *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysitem);
}

void oval_sysitem_iterator_free(struct oval_sysitem_iterator *oc_sysitem)
{
    oval_collection_iterator_free((struct oval_iterator *)oc_sysitem);
}

char *oval_sysitem_get_name(struct oval_sysitem *sysitem)
{return sysitem->name;}
oval_syschar_status_t oval_sysitem_get_status(struct oval_sysitem *sysitem)
{return sysitem->status;}
char *oval_sysitem_get_value(struct oval_sysitem *sysitem)
{return sysitem->value;}
oval_datatype_t oval_sysitem_get_datatype(struct oval_sysitem *sysitem)
{return sysitem->datatype;}
int oval_sysitem_get_mask(struct oval_sysitem *sysitem)
{return sysitem->mask;}

void oval_sysitem_set_name(struct oval_sysitem *sysitem, char *name)
{
	if(sysitem->name!=NULL)free(sysitem->name);
	sysitem->name = name==NULL?NULL:strdup(name);
}
void oval_sysitem_set_status(struct oval_sysitem *sysitem, oval_syschar_status_t status)
{
	sysitem->status = status;
}
void oval_sysitem_set_datatype(struct oval_sysitem *sysitem, oval_datatype_t datatype)
{
	sysitem->datatype = datatype;
}
void oval_sysitem_set_mask(struct oval_sysitem *sysitem, int mask)
{
	sysitem->mask = mask;
}
void oval_sysitem_set_value(struct oval_sysitem *sysitem, char *value)
{
	if(sysitem->value!=NULL)free(sysitem->value);
	sysitem->value = value==NULL?NULL:strdup(value);
}

static void oval_sysitem_value_consumer_(char* value, void* sysitem){
	oval_sysitem_set_value(sysitem, value);
}

int oval_sysitem_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context, oval_sysitem_consumer consumer, void* client)
{
	/*
	char*              name;
	int                mask;
	oval_datatype_enum datatype;
	oval_syschar_status_enum status;
	char*              value;
	 */
	int return_code = 1;
	char *tagname   = (char*) xmlTextReaderName(reader);
	if(strcmp("#text", tagname)){
		struct oval_sysitem *sysitem = oval_sysitem_new();
		{//sysitem->name
			oval_sysitem_set_name(sysitem, tagname);
		}
		{//sysitem->mask
			int mask = oval_parser_boolean_attribute(reader, "mask", 0);
			oval_sysitem_set_mask(sysitem, mask);
		}
		{//sysitem->datatype
			oval_datatype_t datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
			oval_sysitem_set_datatype(sysitem, datatype);
		}
		{//sysitem->status
			oval_syschar_status_t status = oval_syschar_status_parse(reader, "status", SYSCHAR_STATUS_EXISTS);
			oval_sysitem_set_status(sysitem, status);
		}
		{//sysitem->value
			return_code = oval_parser_text_value(reader, context, &oval_sysitem_value_consumer_, sysitem);
		}
		if(return_code!=1){
			char message[200]; *message = 0;
			sprintf(message, "oval_sysitem_parse_tag:: return code is not 1::(%d)",return_code);
			oval_parser_log_warn(context, message);
		}else{
			if(DEBUG_OVAL_SYSITEM){
				int numchars = 0;
				char message[2000];message[numchars]='\0';
				numchars = numchars + sprintf(message+numchars,"oval_sysitem_parse_tag::");
				numchars = numchars + sprintf(message+numchars,"\n    sysitem->name     = %s",oval_sysitem_get_name    (sysitem));
				numchars = numchars + sprintf(message+numchars,"\n    sysitem->mask     = %d",oval_sysitem_get_mask    (sysitem));
				numchars = numchars + sprintf(message+numchars,"\n    sysitem->datatype = %d",oval_sysitem_get_datatype(sysitem));
				numchars = numchars + sprintf(message+numchars,"\n    sysitem->status   = %d",oval_sysitem_get_status  (sysitem));
				numchars = numchars + sprintf(message+numchars,"\n    sysitem->value    = %s",oval_sysitem_get_value   (sysitem));
				oval_parser_log_debug(context, message);
			}
			(*consumer)(sysitem, client);
		}
	}
	free(tagname);
	return return_code;
}

void oval_sysitem_to_print(struct oval_sysitem *sysitem, char *indent,
			      int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sSYSITEM.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sSYSITEM[%d].", indent, idx);

	/*
	char*              name;
	char*              value;
	int                mask;
	oval_datatype_enum datatype;
	oval_syschar_status_enum status;
	 */
	{//id
		printf("%sNAME          = %s\n", nxtindent, oval_sysitem_get_name(sysitem));
	}
	{//id
		printf("%sVALUE         = %s\n", nxtindent, oval_sysitem_get_value(sysitem));
	}
	{//mask
		printf("%sMASK          = %d\n", nxtindent, oval_sysitem_get_mask(sysitem));
	}
	{//datatype
		printf("%sDATATYPE      = %d\n", nxtindent, oval_sysitem_get_datatype(sysitem));
	}
	{//status
		printf("%sSTATUS        = %d\n", nxtindent, oval_sysitem_get_status(sysitem));
	}
}

void oval_sysitem_to_dom  (struct oval_sysitem *sysitem, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_parent = xmlGetNsList(doc, parent)[0];
	xmlNode *sysitem_tag = xmlNewChild(parent, ns_parent, oval_sysitem_get_name(sysitem), oval_sysitem_get_value(sysitem));

	bool mask_value = oval_sysitem_get_mask(sysitem);
	if(mask_value){
		xmlNewProp(sysitem_tag, BAD_CAST "mask", BAD_CAST "true");
	}

	oval_datatype_t datatype_index = oval_sysitem_get_datatype(sysitem);
	if(datatype_index != OVAL_DATATYPE_STRING){
		xmlNewProp(sysitem_tag, BAD_CAST "datatype", BAD_CAST oval_datatype_get_text(datatype_index));
	}

	oval_syschar_status_t status_index = oval_sysitem_get_status(sysitem);
	if(status_index!=SYSCHAR_STATUS_EXISTS){
		xmlNewProp(sysitem_tag, BAD_CAST "status", BAD_CAST oval_syschar_status_text(status_index));
	}
}

