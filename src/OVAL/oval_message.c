/**
 * @file oval_message.c
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

int DEBUG_OVAL_MESSAGE = 0;

typedef struct oval_message {
	char*                   text;
	oval_message_level_enum level;
} oval_message_t;

struct oval_message *oval_message_new(){
	oval_message_t *message = (oval_message_t*)malloc(sizeof(oval_message_t));
	message->text              = NULL;
	message->level             = OVAL_MESSAGE_LEVEL_NONE;
	return message;
}

void oval_message_free(struct oval_message *message){
	if(message->text   !=NULL)free(message->text);
	free(message);
}

int oval_iterator_message_has_more(struct oval_iterator_message *oc_message)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_message);
}

struct oval_message *oval_iterator_message_next(struct oval_iterator_message
						*oc_message)
{
	return (struct oval_message *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_message);
}

char *oval_message_text(struct oval_message *message){return message->text;}
oval_message_level_enum oval_message_level(struct oval_message *message){return message->level;}

void set_oval_message_text(struct oval_message *message, char *text)
{
	if(message->text!=NULL)free(message->text);
	message->text = (text==NULL)?NULL:malloc_string(text);
}
void set_oval_message_level(struct oval_message *message, oval_message_level_enum level)
{
	message->level = level;
}

void oval_message_parse_tag_consumer(char* text, void* message){
	set_oval_message_text(message, text);
}
int oval_message_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context, oval_message_consumer consumer, void* client)
{
	int return_code = 1;
	struct oval_message *message = oval_message_new();
	{//message->lever
		set_oval_message_level(message, oval_message_level_parse(reader,"level",OVAL_MESSAGE_LEVEL_INFO));
	}
	{//message->text
		return_code = oval_parser_text_value(reader, context, &oval_message_parse_tag_consumer, message);
	}
	if(return_code!=1){
		char warning[200]; *warning = 0;
		sprintf(warning, "oval_warning_parse_tag:: return code is not 1::(%d)",return_code);
		oval_parser_log_warn(context, warning);
	}else{
		if(DEBUG_OVAL_MESSAGE){
			int numchars = 0;
			char debug[2000];debug[numchars]='\0';
			numchars = numchars + sprintf(debug+numchars,"oval_message_parse_tag::");
			numchars = numchars + sprintf(debug+numchars,"\n    message->level    = %d",oval_message_level   (message));
			numchars = numchars + sprintf(debug+numchars,"\n    message->text     = %s",oval_message_text    (message));
			oval_parser_log_debug(context, debug);
		}
		(*consumer)(message, client);
	}
	return return_code;
}

void oval_message_to_print(struct oval_message *message, char *indent,
			      int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sMESSAGE.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sMESSAGE[%d].", indent, idx);

	/*
	char*                   text;
	oval_message_level_enum level;
	 */
	printf("%sLEVEL = %d\n", nxtindent, oval_message_level(message));
	printf("%sTEXT  = %s\n", nxtindent, oval_message_text (message));
}

