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
	struct oval_syschar_model *syschar_model;
	bool                       definitions_initialized;
} oval_result_system_t;

struct oval_result_system *oval_result_system_new(struct oval_syschar_model *syschar_model)
{
	oval_result_system_t *system = (oval_result_system_t *)malloc(sizeof(oval_result_system_t));
	system->definitions   = oval_string_map_new();
	system->tests         = oval_string_map_new();
	system->syschar_model = syschar_model;
	system->definitions_initialized = false;
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
	system->syschar_model = NULL;
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

void _oval_result_system_initialize
	(struct oval_result_system *system)
{
	system->definitions_initialized = true;
	struct oval_object_model *object_model = oval_syschar_model_object_model(system->syschar_model);

	struct oval_iterator_definition *oval_definitions = get_oval_definitions(object_model);
	int i;for(i=0;oval_iterator_definition_has_more(oval_definitions);i++){
		struct oval_definition *oval_definition = oval_iterator_definition_next(oval_definitions);
		get_oval_result_definition_new(system, oval_definition);
	}

	struct oval_iterator_test *oval_tests = get_oval_tests(object_model);
	while(oval_iterator_test_has_more(oval_tests)){
		struct oval_test *oval_test = oval_iterator_test_next(oval_tests);
		get_oval_result_test_new(system, oval_test);
	}
}

struct oval_iterator_result_definition *oval_result_system_definitions
	(struct oval_result_system *system)
{
	if(!system->definitions_initialized){
		_oval_result_system_initialize(system);
	}
	return (struct oval_iterator_result_definition *)
		oval_string_map_values(system->definitions);
}
struct oval_iterator_result_test *oval_result_system_tests
	(struct oval_result_system *system)
{
	if(!system->definitions_initialized){
		_oval_result_system_initialize(system);
	}
	return (struct oval_iterator_result_test *)
		oval_string_map_values(system->tests);
}
struct oval_result_definition *get_oval_result_definition
	(struct oval_result_system *system, char *id)
{
	if(!system->definitions_initialized){
		_oval_result_system_initialize(system);
	}
	return (struct oval_result_definition *)
		oval_string_map_get_value(system->definitions, id);

}
struct oval_result_test *get_oval_result_test
	(struct oval_result_system *system, char *id)
{
	if(!system->definitions_initialized){
		_oval_result_system_initialize(system);
	}
	return (struct oval_result_test *)
		oval_string_map_get_value(system->tests, id);

}

struct oval_result_definition *get_oval_result_definition_new
	(struct oval_result_system *system, struct oval_definition *oval_definition)
{
	struct oval_result_definition *rslt_definition = NULL;
	if(oval_definition){
		char *id = oval_definition_id(oval_definition);
		rslt_definition = get_oval_result_definition(system, id);
		if (rslt_definition == NULL) {
			rslt_definition
				= make_result_definition_from_oval_definition
					(system, oval_definition);
			add_oval_result_system_definition(system, rslt_definition);
		}
	}
	return rslt_definition;
}

struct oval_result_test *get_oval_result_test_new
	(struct oval_result_system *system, struct oval_test *oval_test)
{
	char *id = oval_test_id(oval_test);
	struct oval_result_test *rslt_testtest = get_oval_result_test(system, id);
	if (rslt_testtest == NULL) {
		//test = oval_result_test_new(system, id);
		rslt_testtest = make_result_test_from_oval_test(system, oval_test);
		add_oval_result_system_test(system, rslt_testtest);
	}
	return rslt_testtest;
}


struct oval_syschar_model *oval_result_system_syschar_model
	(struct oval_result_system *system)
{
	return system->syschar_model;
}

struct oval_sysinfo *oval_result_system_sysinfo
	(struct oval_result_system *system)
{
	struct oval_syschar_model *syschar_model =
		oval_result_system_syschar_model(system);
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
		return_code = ovalsys_parser_parse
			(system->syschar_model, reader, context->error_handler, context->user_data);
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
	,struct oval_syschar_model  *syschar_model
	,oscap_consumer_func         consumer
	,void                       *client)
{
	int return_code = 1;
	struct oval_result_system *system = oval_result_system_new
		(syschar_model);

	return_code = oval_parser_parse_tag
		(reader, context, (oval_xml_tag_parser)_oval_result_system_parse, system);

	(*consumer)(system, client);

	return return_code;
}

xmlNode *oval_result_system_to_dom
	(struct oval_result_system *system,
			struct oval_results_model *results_model,
			struct oval_result_directives *directives,
			xmlDocPtr doc, xmlNode *parent)
{
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *system_node = xmlNewChild(parent, ns_results, "system", NULL);

	xmlNode *definitions_node = xmlNewChild(system_node, ns_results, "definitions", NULL);
	struct oval_object_model *object_model = oval_results_model_object_model(results_model);
	struct oval_iterator_definition *oval_definitions = get_oval_definitions(object_model);
	int i;for(i=0;oval_iterator_definition_has_more(oval_definitions);i++){
		struct oval_definition *oval_definition = oval_iterator_definition_next(oval_definitions);
		struct oval_result_definition *rslt_definition
			= get_oval_result_definition_new(system, oval_definition);
		if(rslt_definition){
			oval_result_enum result = oval_result_definition_result(rslt_definition);
			if(oval_result_directive_reported(directives, result)){
				oval_result_definition_to_dom
					(rslt_definition, doc, definitions_node);
			}
		}
	}

	xmlNode *tests_node = xmlNewChild(system_node, ns_results, "tests", NULL);
	struct oval_iterator_test *oval_tests = get_oval_tests(object_model);
	while(oval_iterator_test_has_more(oval_tests)){
		struct oval_test *oval_test = oval_iterator_test_next(oval_tests);
		struct oval_result_test *rslt_test
			= get_oval_result_test_new(system, oval_test);
		if(rslt_test)
			oval_result_test_to_dom
				(rslt_test, doc, tests_node);
	}

	struct oval_syschar_model *syschar_model = oval_result_system_syschar_model(system);
	oval_characteristics_to_dom(syschar_model, doc, system_node);

	return system_node;
}
