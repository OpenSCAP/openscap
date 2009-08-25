/**
 * @file oval_resultSystem.c
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
#include "oval_results_impl.h"
#include "oval_collection_impl.h"
#include "api/oval_agent_api.h"

#define OVAL_RESULT_DEFINITION_DEBUG 0

typedef struct oval_result_definition{
	struct oval_definition           *definition;
	oval_result_enum                  result;
	struct oval_result_system        *system;
	struct oval_result_criteria_node *criteria;
	struct oval_collection           *messages;
} oval_result_definition_t;

int oval_iterator_result_definition_has_more
	(struct oval_iterator_result_definition *definitions)
{
	return oval_collection_iterator_has_more
		((struct oval_iterator *)definitions);
}
struct oval_result_definition *oval_iterator_result_definition_next
	(struct oval_iterator_result_definition *definitions)
{
	return (struct oval_result_definition *)
		oval_collection_iterator_next
			((struct oval_iterator *)definitions);
}

struct oval_result_definition *oval_result_definition_new
	(struct oval_result_system *system, char *definition_id)
{
	oval_result_definition_t *definition = (oval_result_definition_t *)
		malloc(sizeof(oval_result_definition_t));
	definition->system = system;
	struct oval_results_model *results_model = oval_result_system_results_model(system);
	struct oval_syschar_model *syschar_model = oval_results_model_syschar_model(results_model);
	struct oval_object_model  *object_model  = oval_syschar_model_object_model (syschar_model);
	definition->definition = get_oval_definition_new(object_model, definition_id);
	definition->result     = OVAL_RESULT_UNKNOWN;
	definition->criteria   = NULL;
	definition->messages   = oval_collection_new();
	return definition;
}
void oval_result_definition_free(struct oval_result_definition *definition){
	if(definition->criteria)oval_result_criteria_node_free(definition->criteria);
	oval_collection_free_items(definition->messages,(oscap_destruct_func)oval_message_free);

	definition->system = NULL;
	definition->criteria = NULL;
	definition->definition = NULL;
	definition->messages   = NULL;
	definition->result = 0;
	free(definition);
}


struct oval_definition *oval_result_definition_definition
	(struct oval_result_definition *definition)
{
	return definition->definition;
}

struct oval_result_system *oval_result_definition_system
	(struct oval_result_definition *definition)
{
	return definition->system;
}

oval_result_enum oval_result_definition_result
	(struct oval_result_definition *definition)
{
	return definition->result;
}

struct oval_iterator_message *oval_result_definition_messages
	(struct oval_result_definition *definition)
{
	return (struct oval_iterator_message *)
		oval_collection_iterator(definition->messages);
}

struct oval_result_criteria_node *oval_result_definition_criteria
	(struct oval_result_definition *definition)
{
	return definition->criteria;
}

void set_oval_result_definition_result
	(struct oval_result_definition *definition, oval_result_enum result)
{
	definition->result = result;
}

void set_oval_result_definition_criteria
	(struct oval_result_definition *definition,
		struct oval_result_criteria_node *criteria)
{
	if(definition->criteria){
		if(oval_result_criteria_node_type(criteria)==NODETYPE_CRITERIA){
			oval_result_criteria_node_free(definition->criteria);
		}
	}
	definition->criteria = criteria;
}

void add_oval_result_definition_message
	(struct oval_result_definition *definition,
		struct oval_message *message)
{
	if(message)oval_collection_add(definition->messages, message);
}

void _oval_result_definition_consume_criteria
	(struct oval_result_criteria_node *node,
		struct oval_result_definition *definition)
{
	set_oval_result_definition_criteria(definition, node);
}
void _oval_result_definition_consume_message
	(struct oval_message *message,
		struct oval_result_definition *definition)
{
	add_oval_result_definition_message(definition, message);
}

int _oval_result_definition_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
		struct oval_result_definition *definition)
{
	int return_code = 1;
	xmlChar *localName = xmlTextReaderLocalName(reader);

	if(OVAL_RESULT_DEFINITION_DEBUG){

	}
	if       (strcmp(localName, "criteria")==0){
		return_code = oval_result_criteria_node_parse
			(reader, context,oval_result_definition_system(definition),
				(oscap_consumer_func)_oval_result_definition_consume_criteria, definition);
	}else if (strcmp(localName, "message")==0){
		return_code = oval_message_parse_tag
			(reader, context,
				(oscap_consumer_func)_oval_result_definition_consume_message, definition);
	}

	return return_code;
}

/*
	OVAL_RESULT_TRUE           = 1,
	OVAL_RESULT_FALSE          = 2,
	OVAL_RESULT_UNKNOWN        = 3,
	OVAL_RESULT_ERROR          = 4,
	OVAL_RESULT_NOT_EVALUATED  = 5,
	OVAL_RESULT_NOT_APPLICABLE = 6
 */
char* _oval_result_definition_results[] =
{
		NULL, "true", "false", "unknown", "error", "not evaluated", "not applicable"
};

int oval_result_definition_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
			struct oval_result_system *system,
			oscap_consumer_func consumer, void *client)
{
	int return_code = 1;
	if(OVAL_RESULT_DEFINITION_DEBUG){
		oval_parser_log_debug(context, "oval_result_definition_parse: BEGIN");
	}

	xmlChar *definition_id = xmlTextReaderGetAttribute(reader, BAD_CAST "definition_id");
	xmlChar *definition_version = xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	int resvsn = atoi(definition_version);
	xmlChar *definition_result = xmlTextReaderGetAttribute(reader, BAD_CAST "result");

	struct oval_result_definition *definition = oval_result_definition_new
		(system, (char *)definition_id);

	int defvsn = oval_definition_version(definition->definition);
	if(defvsn && resvsn!=defvsn){
		char message[200];*message = '\0';
		sprintf(message,
				"oval_result_definition_parse: definition versions don't match\n"
				"    definition id = %s\n"
				"    ovaldef vsn = %d resdef vsn = %d",
				definition_id, defvsn, resvsn);
		oval_parser_log_warn(context, message);
	}
	set_oval_definition_version(definition->definition, resvsn);
	oval_result_enum result = 0;
	int i;
	for(i=1;i<7 && result==0;i++){
		result = strcmp
			(_oval_result_definition_results[i],definition_result)==0
			?i:0;
	}
	if(result){
		set_oval_result_definition_result(definition, result);
	}else{
		char message[200];*message = '\0';
		sprintf(message,
				"oval_result_definition_parse: can't resolve result attribute\n"
				"    definition id = %s\n"
				"    result attribute = %s",
				definition_id, definition_result);
		oval_parser_log_warn(context, message);
	}

	//Process tag contents
	if(OVAL_RESULT_DEFINITION_DEBUG){
		char message[200];*message = '\0';
		sprintf(message,
				"oval_result_definition_parse: processing <definition> contents\n"
				"    definition id = %s vsn = %d\n"
				"    definition result = %s(%d)",
				definition_id, defvsn, definition_result, result);
		oval_parser_log_debug(context, message);
	}
	return_code = oval_parser_parse_tag
		(reader, context,
				(oval_xml_tag_parser)_oval_result_definition_parse, definition);

	free(definition_id);
	free(definition_version);
	free(definition_result);

	(*consumer)(definition, client);
	if(OVAL_RESULT_DEFINITION_DEBUG){
		oval_parser_log_debug(context, "oval_result_definition_parse: END");
	}
	return return_code;
}

