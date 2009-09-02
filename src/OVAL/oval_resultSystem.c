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
#include "oval_string_map_impl.h"
#include "api/oval_definitions.h"
#include "api/oval_agent_api.h"

#define OVAL_RESULT_SYSTEM_DEBUG 0

typedef struct oval_result_system {
	struct oval_string_map    *definitions;
	struct oval_string_map    *tests;
	struct oval_results_model *results_model;
} oval_result_system_t;

struct oval_result_system *oval_result_system_new(struct oval_results_model *model)
{
	oval_result_system_t *system = (oval_result_system_t *)malloc(sizeof(oval_result_system_t));
	system->definitions   = oval_string_map_new();
	system->tests         = oval_string_map_new();
	system->results_model = model;
	return system;
}

void oval_result_system_free(struct oval_result_system *system)
{
	oval_string_map_free
		(system->definitions,
				(oscap_destruct_func)oval_result_definition_free);
	oval_string_map_free
		(system->tests,
				(oscap_destruct_func)oval_result_test_free);

	system->definitions   = NULL;
	system->results_model = NULL;
	system->tests         = NULL;

	free(system);
}
int oval_iterator_result_system_has_more
	(struct oval_iterator_result_system *system)
{
	return oval_collection_iterator_has_more
		((struct oval_iterator *)system);
}
struct oval_result_system *oval_iterator_result_system_next
	(struct oval_iterator_result_system *system)
{
	return (struct oval_result_system *)
		oval_collection_iterator_next
			((struct oval_iterator *)system);
}

struct oval_iterator_result_definition *oval_result_system_definitions
	(struct oval_result_system *system)
{
	return (struct oval_iterator_result_definition *)
		oval_string_map_values(system->definitions);
}
struct oval_iterator_result_test *oval_result_system_tests
	(struct oval_result_system *system)
{
	return (struct oval_iterator_result_test *)
		oval_string_map_values(system->tests);
}
struct oval_result_definition *get_oval_result_definition
	(struct oval_result_system *system, char *id)
{
	return (struct oval_result_definition *)
		oval_string_map_get_value(system->definitions, id);

}
struct oval_result_test *get_oval_result_test
	(struct oval_result_system *system, char *id)
{
	return (struct oval_result_test *)
		oval_string_map_get_value(system->tests, id);

}

struct oval_result_definition *get_oval_result_definition_new
	(struct oval_result_system *system, char *id)
{
	struct oval_result_definition *definition = get_oval_result_definition(system, id);
	if (definition == NULL) {
		definition = oval_result_definition_new(system, id);
		add_oval_result_system_definition(system, definition);
	}
	return definition;
}

struct oval_result_test *get_oval_result_test_new
	(struct oval_result_system *system, char *id)
{
	struct oval_result_test *test = get_oval_result_test(system, id);
	if (test == NULL) {
		test = oval_result_test_new(system, id);
		add_oval_result_system_test(system, test);
	}
	return test;
}


struct oval_results_model *oval_result_system_results_model
	(struct oval_result_system *system)
{
	return system->results_model;
}
struct oval_sysinfo *oval_result_system_sysinfo
	(struct oval_result_system *system)
{
	struct oval_results_model *results_model =
		system->results_model;
	struct oval_syschar_model *syschar_model =
		oval_results_model_syschar_model(results_model);
	return (syschar_model)
		?oval_syschar_model_sysinfo(syschar_model):NULL;
}

void add_oval_result_system_definition
	(struct oval_result_system *system, struct oval_result_definition *definition)
{
	if(definition){
		struct oval_definition *ovaldef = oval_result_definition_definition(definition);
		char *id = oval_definition_id(ovaldef);
		oval_string_map_put(system->definitions, id, definition);
	}
}

void add_oval_result_system_test
	(struct oval_result_system *system, struct oval_result_test *test)
{
	if(test){
		struct oval_test *ovaldef = oval_result_test_test(test);
		char *id = oval_test_id(ovaldef);
		oval_string_map_put(system->tests, id, test);
	}
}

void _oval_result_system_test_consume
	(struct oval_result_test *test, struct oval_result_system *system)
{
	add_oval_result_system_test(system, test);
}

int _oval_result_system_test_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
			struct oval_result_system *system)
{
	return oval_result_test_parse_tag
		(reader, context, system,
				(oscap_consumer_func)_oval_result_system_test_consume, system);

}

void _oval_result_system_definition_consume
	(struct oval_result_definition *definition, struct oval_result_system *system)
{
	add_oval_result_system_definition(system, definition);
}

int _oval_result_system_definition_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
			struct oval_result_system *system)
{
	return oval_result_definition_parse
		(reader, context, system,
				(oscap_consumer_func)_oval_result_system_definition_consume, system);
}

int _oval_result_system_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
			struct oval_result_system *system)
{
	xmlChar *localName = xmlTextReaderLocalName(reader);
	if(OVAL_RESULT_SYSTEM_DEBUG){
		char message[200];*message='\0';
		sprintf(message, "_oval_result_system_parse: parse <%s>",localName);
		oval_parser_log_debug(context,message);
	}
	int return_code = 0;
	if      (strcmp(localName, "definitions")==0){
		return_code = oval_parser_parse_tag
			(reader, context,
				(oval_xml_tag_parser)_oval_result_system_definition_parse, system);
	}else if(strcmp(localName, "tests")==0){
		return_code = oval_parser_parse_tag
			(reader, context,
				(oval_xml_tag_parser)_oval_result_system_test_parse, system);
	}else if(strcmp(localName, "oval_system_characteristics")==0){
		struct oval_syschar_model *syschar_model
			= oval_results_model_syschar_model(system->results_model);
		return_code = ovalsys_parser_parse
			(syschar_model, reader, context->error_handler, context->user_data);
		//return_code = oval_parser_skip_tag(reader, context);
	}else{
		return_code = 0;
		char message[200];*message='\0';
		sprintf(message, "_oval_result_system_parse: TODO: <%s> not handled",localName);
		oval_parser_log_warn(context,message);
	}
	free(localName);
	return return_code;
}

int oval_result_system_parse
	(xmlTextReaderPtr            reader
	,struct oval_parser_context *context
	,oscap_consumer_func         consumer
	,void                       *client)
{
	int return_code = 1;
	struct oval_result_system *system = oval_result_system_new(context->results_model);

	return_code = oval_parser_parse_tag
		(reader, context, (oval_xml_tag_parser)_oval_result_system_parse, system);

	(*consumer)(system, client);

	return return_code;
}

xmlNode *oval_result_system_to_dom
	(struct oval_result_system *system, xmlDocPtr doc, xmlNode *parent)
{
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *system_node = xmlNewChild(parent, ns_results, "system", NULL);

	xmlNode *definitions_node = xmlNewChild(system_node, ns_results, "definitions", NULL);
	struct oval_iterator_result_definition *definitions =
		oval_result_system_definitions(system);
	while(oval_iterator_result_definition_has_more(definitions)){
		oval_result_definition_to_dom
			(oval_iterator_result_definition_next(definitions),
				doc, definitions_node);
	}
	xmlNode *tests_node = xmlNewChild(system_node, ns_results, "tests", NULL);
	struct oval_iterator_result_test *tests
		= oval_result_system_tests(system);
	while(oval_iterator_result_test_has_more(tests)){
		oval_result_test_to_dom
			(oval_iterator_result_test_next(tests),
				doc, tests_node);
	}

	struct oval_results_model *rslt_model = oval_result_system_results_model(system);
	struct oval_syschar_model *syschar_model = oval_results_model_syschar_model(rslt_model);
	oval_characteristics_to_dom(syschar_model, doc, system_node);

	return system_node;
}
