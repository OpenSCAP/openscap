/**
 * @file oval_sysData.c
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

int DEBUG_OVAL_SYSDATA = 0;

extern const char* NAMESPACE_OVALSYS;

typedef struct oval_sysdata {
	//oval_family_enum family;
	oval_subtype_enum subtype;
	oval_message_level_enum message_level;
	char* subtype_name;
	char* id;
	char* message;
	struct oval_collection *items;
	oval_syschar_status_enum status;
} oval_sysdata_t;

struct oval_sysdata *oval_sysdata_new(char *id){
	oval_sysdata_t *sysdata = (oval_sysdata_t*)malloc(sizeof(oval_sysdata_t));
	sysdata->id                = id;
	sysdata->message_level     = OVAL_MESSAGE_LEVEL_NONE;
	sysdata->subtype           = OVAL_SUBTYPE_UNKNOWN;
	sysdata->subtype_name      = NULL;
	sysdata->status            = SYSCHAR_STATUS_UNKNOWN;
	sysdata->message           = NULL;
	sysdata->items             = oval_collection_new();
	return sysdata;
}

void oval_sysdata_free(struct oval_sysdata *sysdata){
	if(sysdata->message!=NULL)free(sysdata->message);
	if(sysdata->subtype_name!=NULL)free(sysdata->subtype_name);

	oval_collection_free_items(sysdata->items, (oscap_destruct_func)oval_sysitem_free);
	free(sysdata->id);
	free(sysdata);
}

int oval_iterator_sysdata_has_more(struct oval_iterator_sysdata *oc_sysdata)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysdata);
}

struct oval_sysdata *oval_iterator_sysdata_next(struct oval_iterator_sysdata
						*oc_sysdata)
{
	return (struct oval_sysdata *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysdata);
}

/*
oval_family_enum oval_sysdata_family(struct oval_sysdata *sysdata)
{
	return ((struct oval_sysdata *)sysdata)->family;
}
*/

oval_subtype_enum oval_sysdata_subtype(struct oval_sysdata *sysdata)
{
	return sysdata->subtype;
}

void set_oval_sysdata_subtype(struct oval_sysdata *sysdata, oval_subtype_enum subtype)
{
	sysdata->subtype = subtype;
}

char *oval_sysdata_id(struct oval_sysdata *data){
	return data->id;
}
char *oval_sysdata_subtype_name(struct oval_sysdata *data){
	return data->subtype_name;
}
void set_oval_sysdata_subtype_name(struct oval_sysdata *data, char *name){
	data->subtype_name = malloc_string(name);
}
char *oval_sysdata_message(struct oval_sysdata *data){
	return data->message;
}
void set_oval_sysdata_message(struct oval_sysdata *data, char *message){
	data->message = malloc_string(message);
}
oval_message_level_enum oval_sysdata_message_level(struct oval_sysdata *data){
	return data->message_level;
}
void set_oval_sysdata_message_level(struct oval_sysdata *data, oval_message_level_enum level){
	data->message_level = level;
}
struct oval_iterator_sysitem *oval_sysdata_items(struct oval_sysdata *data){
	return (struct oval_iterator_sysitem *)oval_collection_iterator(data->items);
}
void add_oval_sysdata_item(struct oval_sysdata *data, struct oval_sysitem* item){
	oval_collection_add(data->items, item);
}
oval_syschar_status_enum oval_sysdata_status(struct oval_sysdata *data){
	return data->status;
}
void set_oval_sysdata_status(struct oval_sysdata *data, oval_syschar_status_enum status){
	data->status = status;
}

void _oval_sysdata_parse_subtag_consume(char* message, void* sysdata) {
			set_oval_sysdata_message(sysdata, message);
}
void _oval_sysdata_parse_subtag_item_consumer(struct oval_sysitem *item, void* sysdata) {
	add_oval_sysdata_item(sysdata, item);
}
int _oval_sysdata_parse_subtag(
		xmlTextReaderPtr reader,
		struct oval_parser_context *context,
		void *client){
	struct oval_sysdata *sysdata = client;
	char *tagname   = (char*) xmlTextReaderLocalName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int return_code;
	if(strcmp(NAMESPACE_OVALSYS,namespace)==0){
		//This is a message
		set_oval_sysdata_message_level(sysdata, oval_message_level_parse(reader, "level", OVAL_MESSAGE_LEVEL_INFO));
		return_code = oval_parser_text_value(reader, context, _oval_sysdata_parse_subtag_consume, sysdata);
	}else{
		//typedef *(oval_sysitem_consumer)(struct oval_sysitem *, void* client);
		return_code = oval_sysitem_parse_tag(reader, context, _oval_sysdata_parse_subtag_item_consumer, sysdata);
	}
	free(tagname);
	free(namespace);
	return return_code;
}

int oval_sysdata_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context)
{
	char *tagname = (char*) xmlTextReaderLocalName(reader);
	oval_subtype_enum subtype = oval_subtype_parse(reader);
	int return_code;
	if(subtype!=OVAL_SUBTYPE_UNKNOWN){
		char *item_id = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
		struct oval_sysdata *sysdata = get_oval_sysdata_new(context->syschar_model, item_id);
		oval_subtype_enum sub = oval_subtype_parse(reader);
		set_oval_sysdata_subtype(sysdata, sub);
		set_oval_sysdata_subtype_name(sysdata, tagname);
		oval_syschar_status_enum  status_enum
			= oval_syschar_status_parse(reader, "status", SYSCHAR_STATUS_UNKNOWN);
		set_oval_sysdata_status(sysdata, status_enum);
		return_code = oval_parser_parse_tag(reader, context, &_oval_sysdata_parse_subtag, sysdata);
		if(DEBUG_OVAL_SYSDATA){
			int numchars = 0;
			char message[2000];message[numchars]='\0';
			numchars = numchars + sprintf(message+numchars,"oval_sysdata_parse_tag::");
			numchars = numchars + sprintf(message+numchars,"\n    sysdata->id            = %s",oval_sysdata_id    (sysdata));
			numchars = numchars + sprintf(message+numchars,"\n    sysdata->status        = %d",oval_sysdata_status(sysdata));
			oval_message_level_enum level = oval_sysdata_message_level(sysdata);
			if(level>OVAL_MESSAGE_LEVEL_NONE){
				numchars = numchars + sprintf(message+numchars,"\n    sysdata->message_level = %d",level);
				numchars = numchars + sprintf(message+numchars,"\n    sysdata->message       = %s",oval_sysdata_message(sysdata));
			}
			struct oval_iterator_sysitem *items = oval_sysdata_items(sysdata);
			int numItems;for(numItems=0;oval_iterator_sysitem_has_more(items);numItems++)oval_iterator_sysitem_next(items);
			numchars = numchars + sprintf(message+numchars,"\n    sysdata->items.length  = %d",numItems);
			oval_parser_log_debug(context, message);
		}
	}else{
		char message[200]; *message = 0;
		char *tagnm     = (char*) xmlTextReaderName(reader);
		char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
		sprintf(message, "oval_sysdata_parse_tag:: expecting <item> got <%s:%s>",
				namespace, tagnm);
		oval_parser_log_warn(context, message);
		return_code = oval_parser_skip_tag(reader, context);
		free(tagnm);
		free(namespace);
	}
	if(return_code!=1){
		char message[200]; *message = 0;
		sprintf(message, "oval_sysdata_parse_tag:: return code is not 1::(%d)",return_code);
		oval_parser_log_warn(context, message);
	}
	free(tagname);

	return return_code;
}

void oval_sysdata_to_print(struct oval_sysdata *sysdata, char *indent,
			      int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sSYSDATA.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sSYSDATA[%d].", indent, idx);

	/*
	char* id;
	oval_subtype_enum subtype;
	oval_syschar_status_enum status;
	oval_message_level_enum message_level;
	char* message;
	struct oval_collection *items;
	 */
	{//id
		printf("%sID            = %s\n", nxtindent, oval_sysdata_id(sysdata));
	}
	{//subtype name
		printf("%sSUBTYPE_NAME  = %s\n", nxtindent, oval_sysdata_subtype_name(sysdata));
	}
	{//subtype
		printf("%sSUBTYPE       = %d\n", nxtindent, oval_sysdata_subtype(sysdata));
	}
	{//status
		printf("%sSTATUS        = %d\n", nxtindent, oval_sysdata_status(sysdata));
	}
	oval_message_level_enum level = oval_sysdata_message_level(sysdata);
	{//level
		printf("%sMESSAGE_LEVEL = %d\n", nxtindent, level);
	}
	if(level!=OVAL_MESSAGE_LEVEL_NONE){//message
		printf("%sMESSAGE       = %s\n", nxtindent, oval_sysdata_message(sysdata));
	}
	{//items
		struct oval_iterator_sysitem *items = oval_sysdata_items(sysdata);
		int i;for(i=1;oval_iterator_sysitem_has_more(items);i++){
			struct oval_sysitem *item = oval_iterator_sysitem_next(items);
			oval_sysitem_to_print(item, nxtindent, i);
		}
	}
}
