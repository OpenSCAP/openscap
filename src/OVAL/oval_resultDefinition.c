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
#include "public/oval_agent_api.h"

#define OVAL_RESULT_DEFINITION_DEBUG 0

typedef struct oval_result_definition{
	struct oval_definition           *definition;
	oval_result_t                  result;
	struct oval_result_system        *system;
	struct oval_result_criteria_node *criteria;
	struct oval_collection           *messages;
	int                               instance;
} oval_result_definition_t;

int oval_result_definition_iterator_has_more
	(struct oval_result_definition_iterator *definitions)
{
	return oval_collection_iterator_has_more
		((struct oval_iterator *)definitions);
}
struct oval_result_definition *oval_result_definition_iterator_next
	(struct oval_result_definition_iterator *definitions)
{
	return (struct oval_result_definition *)
		oval_collection_iterator_next
			((struct oval_iterator *)definitions);
}
void oval_result_definition_iterator_free
	(struct oval_result_definition_iterator *definitions)
{
	oval_collection_iterator_free
			((struct oval_iterator *)definitions);
}

struct oval_result_definition *oval_result_definition_new
	(struct oval_result_system *sys, char *definition_id)
{
	oval_result_definition_t *definition = (oval_result_definition_t *)
		malloc(sizeof(oval_result_definition_t));
	definition->system = sys;
	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(sys);
	struct oval_definition_model  *definition_model  = oval_syschar_model_get_definition_model (syschar_model);
	definition->definition = get_oval_definition_new(definition_model, definition_id);
	definition->result     = OVAL_RESULT_INVALID;
	definition->criteria   = NULL;
	definition->messages   = oval_collection_new();
	definition->instance   = 1;
	return definition;
}

struct oval_result_definition *oval_result_definition_clone
	(struct oval_result_definition *old_definition, struct oval_result_system *new_system)
{
	struct oval_definition *ovaldef = oval_result_definition_get_definition(old_definition);
	struct oval_result_definition *new_definition = oval_result_definition_new(new_system, oval_definition_get_id(ovaldef));

	struct oval_result_criteria_node *old_crit = oval_result_definition_get_criteria(old_definition);
	struct oval_result_criteria_node *new_crit = oval_result_criteria_node_clone(old_crit, new_system);
	oval_result_definition_set_criteria(new_definition, new_crit);

	struct oval_message_iterator *old_messages = oval_result_definition_get_messages(old_definition);
	while(oval_message_iterator_has_more(old_messages)){
		struct oval_message *old_message = oval_message_iterator_next(old_messages);
		struct oval_message *new_message = oval_message_clone(old_message);
		oval_result_definition_add_message(new_definition, new_message);
	}
	oval_message_iterator_free(old_messages);

	oval_result_definition_set_result(new_definition, oval_result_definition_get_result(old_definition));
	oval_result_definition_set_instance(new_definition, oval_result_definition_get_instance(old_definition));
	return new_definition;
}

void oval_result_definition_free(struct oval_result_definition *definition)
{
	if(definition->criteria)oval_result_criteria_node_free(definition->criteria);
	oval_collection_free_items(definition->messages,(oscap_destruct_func)oval_message_free);

	definition->system = NULL;
	definition->criteria = NULL;
	definition->definition = NULL;
	definition->messages   = NULL;
	definition->result = 0;
	definition->instance = 0;
	free(definition);
}

struct oval_result_definition *make_result_definition_from_oval_definition
	(struct oval_result_system *sys, struct oval_definition *oval_definition)
{
	char *defid = oval_definition_get_id(oval_definition);
	struct oval_result_definition *rslt_definition
		= oval_result_definition_new(sys, defid);
	struct oval_criteria_node *oval_criteria
		= oval_definition_get_criteria(oval_definition);
	struct oval_result_criteria_node *rslt_criteria
		= make_result_criteria_node_from_oval_criteria_node(sys, oval_criteria);
	if(rslt_criteria)
		oval_result_definition_set_criteria(rslt_definition, rslt_criteria);
	else
		oval_result_definition_set_result(rslt_definition, OVAL_RESULT_NOT_EVALUATED);
	return rslt_definition;
}


struct oval_definition *oval_result_definition_get_definition
	(struct oval_result_definition *definition)
{
	return definition->definition;
}

struct oval_result_system *oval_result_definition_get_system
	(struct oval_result_definition *definition)
{
	return definition->system;
}

int oval_result_definition_get_instance
	(struct oval_result_definition *definition)
{
	return definition->instance;
}

oval_result_t oval_result_definition_get_result
	(struct oval_result_definition *definition)
{
	if(definition->result==OVAL_RESULT_INVALID){
		struct oval_result_criteria_node *criteria
			= oval_result_definition_get_criteria(definition);

		definition-> result = (criteria==NULL)
			?OVAL_RESULT_NOT_EVALUATED:oval_result_criteria_node_get_result(criteria);
	}
	return definition->result;
}

struct oval_message_iterator *oval_result_definition_get_messages
	(struct oval_result_definition *definition)
{
	return (struct oval_message_iterator *)
		oval_collection_iterator(definition->messages);
}

struct oval_result_criteria_node *oval_result_definition_get_criteria
	(struct oval_result_definition *definition)
{
	return definition->criteria;
}

void oval_result_definition_set_result
	(struct oval_result_definition *definition, oval_result_t result)
{
	definition->result = result;
}

void oval_result_definition_set_instance
	(struct oval_result_definition *definition, int instance)
{
	definition->instance = instance;
}

void oval_result_definition_set_criteria
	(struct oval_result_definition *definition,
		struct oval_result_criteria_node *criteria)
{
	if(definition->criteria){
		if(oval_result_criteria_node_get_type(criteria)==OVAL_NODETYPE_CRITERIA){
			oval_result_criteria_node_free(definition->criteria);
		}
	}
	definition->criteria = criteria;
}

void oval_result_definition_add_message
	(struct oval_result_definition *definition,
		struct oval_message *message)
{
	if(message)oval_collection_add(definition->messages, message);
}

static void _oval_result_definition_consume_criteria
	(struct oval_result_criteria_node *node,
		struct oval_result_definition *definition)
{
	oval_result_definition_set_criteria(definition, node);
}
static void _oval_result_definition_consume_message
	(struct oval_message *message,
		struct oval_result_definition *definition)
{
	oval_result_definition_add_message(definition, message);
}

static int _oval_result_definition_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
		struct oval_result_definition *definition)
{
	int return_code = 1;
	xmlChar *localName = xmlTextReaderLocalName(reader);

	if(OVAL_RESULT_DEFINITION_DEBUG){

	}
	if       (strcmp((const char *)localName, "criteria")==0){
		return_code = oval_result_criteria_node_parse
			(reader, context,oval_result_definition_get_system(definition),
				(oscap_consumer_func)_oval_result_definition_consume_criteria, definition);
	}else if (strcmp((const char *)localName, "message")==0){
		return_code = oval_message_parse_tag
			(reader, context,
				(oscap_consumer_func)_oval_result_definition_consume_message, definition);
	}

	return return_code;
}

static char* _oval_result_definition_results[] =
{
		NULL, "true", "false", "unknown", "error", "not evaluated", "not applicable"
};

int oval_result_definition_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
			struct oval_result_system *sys,
			oscap_consumer_func consumer, void *client)
{
	int return_code = 1;
	if(OVAL_RESULT_DEFINITION_DEBUG){
		oval_parser_log_debug(context, "oval_result_definition_parse: BEGIN");
	}

	xmlChar *definition_id = xmlTextReaderGetAttribute(reader, BAD_CAST "definition_id");
	xmlChar *definition_version = xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	int resvsn = atoi( (char *) definition_version);
	xmlChar *definition_result = xmlTextReaderGetAttribute(reader, BAD_CAST "result");
	int instance = oval_parser_int_attribute(reader, "variable_instance", 1);

	struct oval_result_definition *definition = oval_result_definition_new
		(sys, (char *)definition_id);

	int defvsn = oval_definition_get_version(definition->definition);
	if(defvsn && resvsn!=defvsn){
		char message[200];*message = '\0';
		sprintf(message,
				"oval_result_definition_parse: definition versions don't match\n"
				"    definition id = %s\n"
				"    ovaldef vsn = %d resdef vsn = %d",
				definition_id, defvsn, resvsn);
		oval_parser_log_warn(context, message);
	}
	oval_definition_set_version(definition->definition, resvsn);
	oval_result_definition_set_instance(definition, instance);
	oval_result_t result = 0;
	int i;
	for(i=1;i<7 && result==0;i++){
		result = strcmp
		  (_oval_result_definition_results[i],(const char *)definition_result)==0
			?i:0;
	}
	if(result){
		oval_result_definition_set_result(definition, result);
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

xmlNode *oval_result_definition_to_dom
	(struct oval_result_definition *definition, oval_result_directive_content_t content,
			xmlDocPtr doc, xmlNode *parent)
{
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *definition_node = xmlNewChild(parent, ns_results, BAD_CAST "definition", NULL);

	struct oval_definition *oval_definition
		= oval_result_definition_get_definition(definition);
	char *definition_id = oval_definition_get_id(oval_definition);
	xmlNewProp(definition_node, BAD_CAST "definition_id", BAD_CAST definition_id);

	oval_result_t result
		= oval_result_definition_get_result(definition);
	const char* result_att = oval_result_get_text(result);
	xmlNewProp(definition_node, BAD_CAST "result", BAD_CAST result_att);

	int version = oval_definition_get_version(oval_definition);
	char version_att[10]; *version_att = '\0';
	snprintf(version_att, sizeof(version_att), "%d", version);
	xmlNewProp(definition_node, BAD_CAST "version", BAD_CAST version_att);

	int instance = oval_result_definition_get_instance(definition);
	if(instance!=1){
		char instance_att[10]; *instance_att = '\0';
		snprintf(instance_att, sizeof(instance_att), "%d", instance);
		xmlNewProp(definition_node, BAD_CAST "variable_instance", BAD_CAST instance_att);
	}

	struct oval_message_iterator *messages
		= oval_result_definition_get_messages(definition);
	while(oval_message_iterator_has_more(messages)){
		oval_message_to_dom
			(oval_message_iterator_next(messages),
				doc, definition_node);
	}
	oval_message_iterator_free(messages);

	if(content==OVAL_DIRECTIVE_CONTENT_FULL){
		struct oval_result_criteria_node *criteria
			= oval_result_definition_get_criteria(definition);
		if(criteria){
			oval_result_criteria_node_to_dom
				(criteria, doc, definition_node);
		}
	}

	return definition_node;
}

