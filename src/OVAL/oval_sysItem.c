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

int DEBUG_OVAL_SYSITEM = 0;

typedef struct oval_sysitem {
	char*              name;
	char*              value;
	int                mask;
	oval_datatype_enum datatype;
	oval_syschar_status_enum status;
} oval_sysitem_t;

struct oval_sysitem *oval_sysitem_new(){
	oval_sysitem_t *sysitem = (oval_sysitem_t*)malloc(sizeof(oval_sysitem_t));
	sysitem->name              = NULL;
	sysitem->value             = NULL;
	sysitem->status            = SYSCHAR_STATUS_UNKNOWN;
	sysitem->datatype          = OVAL_DATATYPE_UNKNOWN;
	sysitem->mask              = 0;
	return sysitem;
}

void oval_sysitem_free(struct oval_sysitem *sysitem){
	if(sysitem->name   !=NULL)free(sysitem->name);
	if(sysitem->value  !=NULL)free(sysitem->value);
	free(sysitem);
}

int oval_iterator_sysitem_has_more(struct oval_iterator_sysitem *oc_sysitem)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysitem);
}

struct oval_sysitem *oval_iterator_sysitem_next(struct oval_iterator_sysitem
						*oc_sysitem)
{
	return (struct oval_sysitem *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysitem);
}

char *oval_sysitem_name(struct oval_sysitem *sysitem){return sysitem->name;}
oval_syschar_status_enum oval_sysitem_status(struct oval_sysitem *sysitem){return sysitem->status;}
char *oval_sysitem_value(struct oval_sysitem *sysitem){return sysitem->value;}
oval_datatype_enum oval_sysitem_datatype(struct oval_sysitem *sysitem){return sysitem->datatype;}
int oval_sysitem_mask(struct oval_sysitem *sysitem){return sysitem->mask;}

void set_oval_sysitem_name(struct oval_sysitem *sysitem, char *name)
{
	sysitem->name = malloc_string(name);
}
void set_oval_sysitem_status(struct oval_sysitem *sysitem, oval_syschar_status_enum status)
{
	sysitem->status = status;
}
void set_oval_sysitem_datatype(struct oval_sysitem *sysitem, oval_datatype_enum datatype)
{
	sysitem->datatype = datatype;
}
void set_oval_sysitem_mask(struct oval_sysitem *sysitem, int mask)
{
	sysitem->mask = mask;
}
void set_oval_sysitem_value(struct oval_sysitem *sysitem, char *value)
{
	sysitem->value = malloc_string(value);
}

void oval_sysitem_value_consumer_(char* value, void* sysitem){
	set_oval_sysitem_value(sysitem, value);
	free(value);
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
			set_oval_sysitem_name(sysitem, tagname);
		}
		{//sysitem->mask
			int mask = oval_parser_boolean_attribute(reader, "mask", 0);
			set_oval_sysitem_mask(sysitem, mask);
		}
		{//sysitem->datatype
			oval_datatype_enum datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
			set_oval_sysitem_datatype(sysitem, datatype);
		}
		{//sysitem->status
			oval_syschar_status_enum status = oval_syschar_status_parse(reader, "status", SYSCHAR_STATUS_EXISTS);
			set_oval_sysitem_status(sysitem, status);
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
				numchars = numchars + sprintf(message+numchars,"\n    sysitem->name     = %s",oval_sysitem_name    (sysitem));
				numchars = numchars + sprintf(message+numchars,"\n    sysitem->mask     = %d",oval_sysitem_mask    (sysitem));
				numchars = numchars + sprintf(message+numchars,"\n    sysitem->datatype = %d",oval_sysitem_datatype(sysitem));
				numchars = numchars + sprintf(message+numchars,"\n    sysitem->status   = %d",oval_sysitem_status  (sysitem));
				numchars = numchars + sprintf(message+numchars,"\n    sysitem->value    = %s",oval_sysitem_value   (sysitem));
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
		printf("%sNAME          = %s\n", nxtindent, oval_sysitem_name(sysitem));
	}
	{//id
		printf("%sVALUE         = %s\n", nxtindent, oval_sysitem_value(sysitem));
	}
	{//mask
		printf("%sMASK          = %d\n", nxtindent, oval_sysitem_mask(sysitem));
	}
	{//datatype
		printf("%sDATATYPE      = %d\n", nxtindent, oval_sysitem_datatype(sysitem));
	}
	{//status
		printf("%sSTATUS        = %d\n", nxtindent, oval_sysitem_status(sysitem));
	}
}
