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

static int DEBUG_OVAL_SYSDATA = 0;

typedef struct oval_sysdata {
	//oval_family_enum family;
	oval_subtype_t subtype;
	oval_message_level_t message_level;
	char* id;
	char* message;
	struct oval_collection *items;
	oval_syschar_status_t status;
} oval_sysdata_t;

struct oval_sysdata *oval_sysdata_new(char *id)
{
	oval_sysdata_t *sysdata = (oval_sysdata_t*)malloc(sizeof(oval_sysdata_t));
	sysdata->id                = strdup(id);
	sysdata->message_level     = OVAL_MESSAGE_LEVEL_NONE;
	sysdata->subtype           = OVAL_SUBTYPE_UNKNOWN;
	sysdata->status            = SYSCHAR_STATUS_UNKNOWN;
	sysdata->message           = NULL;
	sysdata->items             = oval_collection_new();
	return sysdata;
}

void oval_sysdata_free(struct oval_sysdata *sysdata)
{
	if(sysdata->message!=NULL)free(sysdata->message);

	oval_collection_free_items(sysdata->items, (oscap_destruct_func)oval_sysitem_free);
	free(sysdata->id);

	sysdata->id = NULL;
	sysdata->items = NULL;
	sysdata->message = NULL;
	free(sysdata);
}

int oval_sysdata_iterator_has_more(struct oval_sysdata_iterator *oc_sysdata)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysdata);
}

struct oval_sysdata *oval_sysdata_iterator_next(struct oval_sysdata_iterator
						*oc_sysdata)
{
	return (struct oval_sysdata *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysdata);
}

void oval_sysdata_iterator_free(struct oval_sysdata_iterator
						*oc_sysdata)
{
    oval_collection_iterator_free((struct oval_iterator *)oc_sysdata);
}

oval_subtype_t oval_sysdata_get_subtype(struct oval_sysdata *sysdata)
{
	return sysdata->subtype;
}

void oval_sysdata_set_subtype(struct oval_sysdata *sysdata, oval_subtype_t subtype)
{
	sysdata->subtype = subtype;
}

char *oval_sysdata_get_id(struct oval_sysdata *data)
{
	return data->id;
}

char *oval_sysdata_get_message(struct oval_sysdata *data)
{
	return data->message;
}
static void set_oval_sysdata_message(struct oval_sysdata *data, char *message){
	if(data->message!=NULL)free(data->message);
	data->message = message==NULL?NULL:strdup(message);
}
oval_message_level_t oval_sysdata_get_message_level(struct oval_sysdata *data)
{
	return data->message_level;
}
static void set_oval_sysdata_message_level(struct oval_sysdata *data, oval_message_level_t level){
	data->message_level = level;
}
struct oval_sysitem_iterator *oval_sysdata_get_items(struct oval_sysdata *data)
{
	return (struct oval_sysitem_iterator *)oval_collection_iterator(data->items);
}
void oval_sysdata_add_item(struct oval_sysdata *data, struct oval_sysitem* item)
{
	oval_collection_add(data->items, item);
}
oval_syschar_status_t oval_sysdata_get_status(struct oval_sysdata *data)
{
	return data->status;
}
void oval_sysdata_set_status(struct oval_sysdata *data, oval_syschar_status_t status)
{
	data->status = status;
}

static void _oval_sysdata_parse_subtag_consume(char* message, void* sysdata) {
			set_oval_sysdata_message(sysdata, message);
}
static void _oval_sysdata_parse_subtag_item_consumer(struct oval_sysitem *item, void* sysdata) {
	oval_sysdata_add_item(sysdata, item);
}
static int _oval_sysdata_parse_subtag(
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
	oval_subtype_t subtype = oval_subtype_parse(reader);
	int return_code;
	if(subtype!=OVAL_SUBTYPE_UNKNOWN){
		char *item_id = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
		struct oval_sysdata *sysdata = get_oval_sysdata_new(context->syschar_model, item_id);
		free(item_id);item_id=NULL;
		oval_subtype_t sub = oval_subtype_parse(reader);
		oval_sysdata_set_subtype(sysdata, sub);
		oval_syschar_status_t  status_enum
			= oval_syschar_status_parse(reader, "status", SYSCHAR_STATUS_EXISTS);
		oval_sysdata_set_status(sysdata, status_enum);
		return_code = oval_parser_parse_tag(reader, context, &_oval_sysdata_parse_subtag, sysdata);
		if(DEBUG_OVAL_SYSDATA){
			int numchars = 0;
			char message[2000];message[numchars]='\0';
			numchars = numchars + sprintf(message+numchars,"oval_sysdata_parse_tag::");
			numchars = numchars + sprintf(message+numchars,"\n    sysdata->id            = %s",oval_sysdata_get_id    (sysdata));
			numchars = numchars + sprintf(message+numchars,"\n    sysdata->status        = %d",oval_sysdata_get_status(sysdata));
			oval_message_level_t level = oval_sysdata_get_message_level(sysdata);
			if(level>OVAL_MESSAGE_LEVEL_NONE){
				numchars = numchars + sprintf(message+numchars,"\n    sysdata->message_level = %d",level);
				numchars = numchars + sprintf(message+numchars,"\n    sysdata->message       = %s",oval_sysdata_get_message(sysdata));
			}
			struct oval_sysitem_iterator *items = oval_sysdata_get_items(sysdata);
			int numItems;for(numItems=0;oval_sysitem_iterator_has_more(items);numItems++)oval_sysitem_iterator_next(items);
			oval_sysitem_iterator_free(items);
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
		printf("%sID            = %s\n", nxtindent, oval_sysdata_get_id(sysdata));
	}
	{//subtype
		printf("%sSUBTYPE       = %d\n", nxtindent, oval_sysdata_get_subtype(sysdata));
	}
	{//status
		printf("%sSTATUS        = %d\n", nxtindent, oval_sysdata_get_status(sysdata));
	}
	oval_message_level_t level = oval_sysdata_get_message_level(sysdata);
	{//level
		printf("%sMESSAGE_LEVEL = %d\n", nxtindent, level);
	}
	if(level!=OVAL_MESSAGE_LEVEL_NONE){//message
		printf("%sMESSAGE       = %s\n", nxtindent, oval_sysdata_get_message(sysdata));
	}
	{//items
		struct oval_sysitem_iterator *items = oval_sysdata_get_items(sysdata);
		int i;for(i=1;oval_sysitem_iterator_has_more(items);i++){
			struct oval_sysitem *item = oval_sysitem_iterator_next(items);
			oval_sysitem_to_print(item, nxtindent, i);
		}
		oval_sysitem_iterator_free(items);
	}
}

void oval_sysdata_to_dom  (struct oval_sysdata *sysdata, xmlDoc *doc, xmlNode *tag_parent){
	if(sysdata){
		xmlNs *ns_syschar = xmlSearchNsByHref(doc, tag_parent, OVAL_SYSCHAR_NAMESPACE);

		char syschar_namespace[] = "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5";
		oval_subtype_t subtype = oval_sysdata_get_subtype(sysdata);
		if(subtype){
			const char* family = oval_family_get_text(oval_subtype_get_family(subtype));
			char  family_namespace[sizeof(syschar_namespace)+strlen(family)+2];*family_namespace = '\0';
			sprintf(family_namespace,"%s#%s",OVAL_SYSCHAR_NAMESPACE,family);
			const char* subtype_text = oval_subtype_get_text(subtype);
			char tagname[strlen(subtype_text)+6];*tagname = '\0';
			sprintf(tagname, "%s_item", subtype_text);
		    xmlNode *tag_sysdata = xmlNewChild
				(tag_parent, NULL, BAD_CAST tagname, NULL);
		    xmlNs *ns_family = xmlNewNs(tag_sysdata, BAD_CAST family_namespace, NULL);
		    xmlSetNs(tag_sysdata, ns_family);

		    {//attributes
		    	xmlNewProp(tag_sysdata, BAD_CAST "id", BAD_CAST oval_sysdata_get_id(sysdata));
		    	oval_syschar_status_t status_index = oval_sysdata_get_status(sysdata);
		    	char* status = oval_syschar_status_text(status_index);
		    	xmlNewProp(tag_sysdata, BAD_CAST "status", BAD_CAST status);
		    }
			{//message
				char *message = oval_sysdata_get_message(sysdata);
				if(message!=NULL){
					xmlNode *tag_message = xmlNewChild
						(tag_sysdata, ns_syschar, BAD_CAST "message", BAD_CAST message);
					oval_message_level_t idx = oval_sysdata_get_message_level(sysdata);
					const char* level = oval_message_level_text(idx);
					xmlNewProp(tag_message, BAD_CAST "level", BAD_CAST level);
				}
			}

			{//items
				struct oval_sysitem_iterator *items = oval_sysdata_get_items(sysdata);
				while(oval_sysitem_iterator_has_more(items)){
					struct oval_sysitem *item = oval_sysitem_iterator_next(items);
					oval_sysitem_to_dom(item, doc, tag_sysdata);
				}
				oval_sysitem_iterator_free(items);
			}
		}else{
			fprintf(stderr,"WARNING: Skipping XML generation of oval_sysdata with subtype OVAL_SUBTYPE_UNKNOWN");
		}
	}
}
