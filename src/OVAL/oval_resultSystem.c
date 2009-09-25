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
int oval_result_system_iterator_has_more
	(struct oval_result_system_iterator *system)
{
	return oval_collection_iterator_has_more
		((struct oval_iterator *)system);
}
struct oval_result_system *oval_result_system_iterator_next
	(struct oval_result_system_iterator *system)
{
	return (struct oval_result_system *)
		oval_collection_iterator_next
			((struct oval_iterator *)system);
}

void oval_result_system_iterator_free
	(struct oval_result_system_iterator *system)
{
	oval_collection_iterator_free
			((struct oval_iterator *)system);
}

void _oval_result_system_initialize
	(struct oval_result_system *system)
{
	system->definitions_initialized = true;
	struct oval_object_model *object_model = oval_syschar_model_get_object_model(system->syschar_model);

	struct oval_definition_iterator *oval_definitions = oval_object_model_get_definitions(object_model);
	int i;for(i=0;oval_definition_iterator_has_more(oval_definitions);i++){
		struct oval_definition *oval_definition = oval_definition_iterator_next(oval_definitions);
		get_oval_result_definition_new(system, oval_definition);
	}
	oval_definition_iterator_free(oval_definitions);

	struct oval_test_iterator *oval_tests = oval_object_model_get_tests(object_model);
	while(oval_test_iterator_has_more(oval_tests)){
		struct oval_test *oval_test = oval_test_iterator_next(oval_tests);
		get_oval_result_test_new(system, oval_test);
	}
	oval_test_iterator_free(oval_tests);
}

struct oval_result_definition_iterator *oval_result_system_get_definitions
	(struct oval_result_system *system)
{
	if(!system->definitions_initialized){
		_oval_result_system_initialize(system);
	}
	struct oval_result_definition_iterator *iterator = (struct oval_result_definition_iterator *)
		oval_string_map_values(system->definitions);
	return iterator;
}
struct oval_result_test_iterator *oval_result_system_get_tests
	(struct oval_result_system *system)
{
	if(!system->definitions_initialized){
		_oval_result_system_initialize(system);
	}
	struct oval_result_test_iterator *iterator = (struct oval_result_test_iterator *)
		oval_string_map_values(system->tests);
	return iterator;
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
		char *id = oval_definition_get_id(oval_definition);
		rslt_definition = get_oval_result_definition(system, id);
		if (rslt_definition == NULL) {
			rslt_definition
				= make_result_definition_from_oval_definition
					(system, oval_definition);
			oval_result_system_add_definition_(system, rslt_definition);
		}
	}
	return rslt_definition;
}

struct oval_result_test *get_oval_result_test_new
	(struct oval_result_system *system, struct oval_test *oval_test)
{
	char *id = oval_test_get_id(oval_test);
	struct oval_result_test *rslt_testtest = get_oval_result_test(system, id);
	if (rslt_testtest == NULL) {
		//test = oval_result_test_new(system, id);
		rslt_testtest = make_result_test_from_oval_test(system, oval_test);
		oval_result_system_add_test(system, rslt_testtest);
	}
	return rslt_testtest;
}


struct oval_syschar_model *oval_result_system_get_syschar_model
	(struct oval_result_system *system)
{
	return system->syschar_model;
}

struct oval_sysinfo *oval_result_system_get_sysinfo
	(struct oval_result_system *system)
{
	struct oval_syschar_model *syschar_model =
		oval_result_system_get_syschar_model(system);
	return (syschar_model)
		?oval_syschar_model_get_sysinfo(syschar_model):NULL;
}

void oval_result_system_add_definition_
	(struct oval_result_system *system, struct oval_result_definition *definition)
{
	if(definition){
		struct oval_definition *ovaldef = oval_result_definition_get_definition(definition);
		char *id = oval_definition_get_id(ovaldef);
		oval_string_map_put(system->definitions, id, definition);
	}
}

void oval_result_system_add_test
	(struct oval_result_system *system, struct oval_result_test *test)
{
	if(test){
		struct oval_test *ovaldef = oval_result_test_get_test(test);
		char *id = oval_test_get_id(ovaldef);
		oval_string_map_put(system->tests, id, test);
	}
}

void _oval_result_system_test_consume
	(struct oval_result_test *test, struct oval_result_system *system)
{
	oval_result_system_add_test(system, test);
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
	oval_result_system_add_definition_(system, definition);
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

void _oval_result_system_scan_criteria_for_references
	(struct oval_result_criteria_node *node, struct oval_string_map *testmap)
{
	struct oval_result_criteria_node_iterator *subnodes
		= oval_result_criteria_node_get_subnodes(node);
	while(subnodes && oval_result_criteria_node_iterator_has_more(subnodes)){
		struct oval_result_criteria_node *subnode
			= oval_result_criteria_node_iterator_next(subnodes);
		_oval_result_system_scan_criteria_for_references(subnode, testmap);
	}
	oval_result_criteria_node_iterator_free(subnodes);
	struct oval_result_test *result_test = oval_result_criteria_node_get_test(node);
	if(result_test){
		struct oval_test *oval_test = oval_result_test_get_test(result_test);
		char *testid = oval_test_get_id(oval_test);
		void *value = oval_string_map_get_value(testmap, testid);
		if(value==NULL){
			oval_string_map_put(testmap, testid, result_test);
		}
	}
}

void _oval_result_system_scan_entity_for_references
	(struct oval_syschar_model *syschar_model, struct oval_entity *entity,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap,
	 struct oval_string_map *sysmap);

void _oval_result_system_scan_set_for_references
	(struct oval_syschar_model *syschar_model, struct oval_setobject *set,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap,
	 struct oval_string_map *sysmap);

void _oval_result_system_scan_object_for_references
	(struct oval_syschar_model *syschar_model, struct oval_object *object,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap,
	 struct oval_string_map *sysmap)
{
	char *objid = oval_object_get_id(object);
	struct oval_syschar *syschar = oval_syschar_model_get_syschar(syschar_model, objid);
	if(syschar)oval_string_map_put(sysmap, objid, syschar);
	struct oval_object_content_iterator *contents = oval_object_get_object_content(object);
	while(oval_object_content_iterator_has_more(contents)){
		struct oval_object_content *content = oval_object_content_iterator_next(contents);
		struct oval_entity *entity = oval_object_content_get_entity(content);
		if(entity)_oval_result_system_scan_entity_for_references(syschar_model, entity, objmap, sttmap, varmap, sysmap);
		struct oval_setobject *set = oval_object_content_get_setobject(content);
		if(set)_oval_result_system_scan_set_for_references(syschar_model, set, objmap, sttmap, varmap, sysmap);
	}
	oval_object_content_iterator_free(contents);
}
void _oval_result_system_scan_state_for_references
	(struct oval_syschar_model *syschar_model, struct oval_state *state,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap,
	 struct oval_string_map *sysmap)
{
	struct oval_state_content_iterator *contents = oval_state_get_contents(state);
	while(oval_state_content_iterator_has_more(contents)){
		struct oval_state_content *content = oval_state_content_iterator_next(contents);
		struct oval_entity *entity = oval_state_content_get_entity(content);
		if(entity)_oval_result_system_scan_entity_for_references(syschar_model, entity, objmap, sttmap, varmap, sysmap);
	}
	oval_state_content_iterator_free(contents);
}

void _oval_result_system_scan_component_for_references
	(struct oval_syschar_model *syschar_model, struct oval_component *component,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap,
	 struct oval_string_map *sysmap)
{
	struct oval_variable *variable = oval_component_get_variable(component);
	if(variable){
		char *varid = oval_variable_get_id(variable);
		void *value = oval_string_map_get_value(varmap, varid);
		if(value==NULL){
			oval_string_map_put(varmap, varid, variable);
			struct oval_component *component = oval_variable_get_component(variable);
			if(component){
				_oval_result_system_scan_component_for_references(syschar_model, component, objmap, sttmap, varmap, sysmap);
			}
		}
	}else{
		struct oval_component_iterator *fcomponents = oval_component_get_function_components(component);
		if(fcomponents)while(oval_component_iterator_has_more(fcomponents)){
			struct oval_component *fcomponent = oval_component_iterator_next(fcomponents);
			_oval_result_system_scan_component_for_references(syschar_model, fcomponent, objmap, sttmap, varmap, sysmap);
		}
		oval_component_iterator_free(fcomponents);

		struct oval_object *object = oval_component_get_object(component);
		if(object){
			char *objid = oval_object_get_id(object);
			void *value = oval_string_map_get_value(objmap, objid);
			if(value==NULL){
				oval_string_map_put(objmap, objid, object);
				_oval_result_system_scan_object_for_references(syschar_model, object, objmap, sttmap, varmap, sysmap);
			}
		}
	}
}

void _oval_result_system_scan_entity_for_references
	(struct oval_syschar_model *syschar_model, struct oval_entity *entity,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap,
	 struct oval_string_map *sysmap)
{
	struct oval_variable *variable = oval_entity_get_variable(entity);
	if(variable){
		char *varid = oval_variable_get_id(variable);
		void *value = oval_string_map_get_value(varmap, varid);
		if(value==NULL){
			oval_string_map_put(varmap, varid, variable);
			struct oval_component *component = oval_variable_get_component(variable);
			if(component){
				_oval_result_system_scan_component_for_references(syschar_model, component, objmap, sttmap, varmap, sysmap);
			}
		}
	}
}

void _oval_result_system_scan_set_for_references
	(struct oval_syschar_model *syschar_model, struct oval_setobject *set,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap,
	 struct oval_string_map *sysmap)
{
	struct oval_object_iterator *objects = oval_setobject_get_objects(set);
	if(objects)while(oval_object_iterator_has_more(objects)){
		struct oval_object *object = oval_object_iterator_next(objects);
		char *objid = oval_object_get_id(object);
		void *value = oval_string_map_get_value(objmap, objid);
		if(value==NULL){
			oval_string_map_put(objmap, objid, object);
			_oval_result_system_scan_object_for_references(syschar_model, object, objmap, sttmap, varmap, sysmap);
		}
	}
	oval_object_iterator_free(objects);
	struct oval_state_iterator *states = oval_setobject_get_filters(set);
	if(states)while(oval_state_iterator_has_more(states)){
		struct oval_state *state = oval_state_iterator_next(states);
		char *sttid = oval_state_get_id(state);
		void *value = oval_string_map_get_value(sttmap, sttid);
		if(value==NULL){
			oval_string_map_put(sttmap, sttid, state);
			_oval_result_system_scan_state_for_references(syschar_model, state, objmap, sttmap, varmap, sysmap);
		}
	}
	oval_state_iterator_free(states);
	struct oval_setobject_iterator *subsets = oval_setobject_get_subsets(set);
	if(subsets)while(oval_setobject_iterator_has_more(subsets)){
		struct oval_setobject *subset = oval_setobject_iterator_next(subsets);
		_oval_result_system_scan_set_for_references(syschar_model, subset, objmap, sttmap, varmap, sysmap);
	}
	oval_setobject_iterator_free(subsets);
}

bool _oval_result_system_resolve_syschar
	(struct oval_syschar *syschar, struct oval_string_map *sysmap)
{
	struct oval_object *object = oval_syschar_get_object(syschar);
	char *objid = oval_object_get_id(object);
	return oval_string_map_get_value(sysmap, objid)!=NULL;
}

xmlNode *oval_result_system_to_dom
	(struct oval_result_system *system,
			struct oval_results_model *results_model,
			struct oval_result_directives *directives,
			xmlDocPtr doc, xmlNode *parent)
{
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *system_node = xmlNewChild(parent, ns_results, "system", NULL);

	struct oval_string_map *tstmap = oval_string_map_new();

	xmlNode *definitions_node = xmlNewChild(system_node, ns_results, "definitions", NULL);
	struct oval_object_model *object_model = oval_results_model_get_object_model(results_model);
	struct oval_definition_iterator *oval_definitions = oval_object_model_get_definitions(object_model);
	int i;for(i=0;oval_definition_iterator_has_more(oval_definitions);i++){
		struct oval_definition *oval_definition = oval_definition_iterator_next(oval_definitions);
		struct oval_result_definition *rslt_definition
			= get_oval_result_definition_new(system, oval_definition);
		if(rslt_definition){
			oval_result_t result = oval_result_definition_get_result(rslt_definition);
			if(oval_result_directive_get_reported(directives, result)){
				oval_result_directive_content_t content
					= oval_result_directive_get_content(directives, result);
				oval_result_definition_to_dom
					(rslt_definition, content, doc, definitions_node);
				if(content==OVAL_DIRECTIVE_CONTENT_FULL){
					struct oval_result_criteria_node *criteria
						= oval_result_definition_criteria(rslt_definition);
					if(criteria)_oval_result_system_scan_criteria_for_references(criteria, tstmap);
				}
			}
		}
	}
	oval_definition_iterator_free(oval_definitions);

	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(system);
	struct oval_string_map *sysmap = oval_string_map_new();
	struct oval_string_map *objmap = oval_string_map_new();
	struct oval_string_map *sttmap = oval_string_map_new();
	struct oval_string_map *varmap = oval_string_map_new();

	struct oval_result_test_iterator *result_tests = (struct oval_result_test_iterator *)
		oval_string_map_values(tstmap);
	if(oval_result_test_iterator_has_more(result_tests)){
		xmlNode *tests_node = xmlNewChild(system_node, ns_results, "tests", NULL);
		while(oval_result_test_iterator_has_more(result_tests)){
			struct oval_result_test *result_test = oval_result_test_iterator_next(result_tests);
			oval_result_test_to_dom(result_test, doc, tests_node);
			struct oval_test *oval_test = oval_result_test_get_test(result_test);
			struct oval_object *object = oval_test_get_object(oval_test);
			if(object){
				char* objid = oval_object_get_id(object);
				void* value = oval_string_map_get_value(objmap, objid);
				if(value==NULL){
					struct oval_syschar *syschar = oval_syschar_model_get_syschar(syschar_model, objid);
					if(syschar){
						oval_string_map_put(objmap, objid, object);
						_oval_result_system_scan_object_for_references
							(syschar_model, object, objmap, sttmap, varmap, sysmap);
					}
				}
			}
		}
	}
	oval_result_test_iterator_free(result_tests);

	oval_characteristics_to_dom
		(syschar_model, doc, system_node,
				(oval_syschar_resolver *)_oval_result_system_resolve_syschar, sysmap);

	oval_string_map_free(sysmap, NULL);
	oval_string_map_free(objmap, NULL);
	oval_string_map_free(sttmap, NULL);
	oval_string_map_free(varmap, NULL);
	oval_string_map_free(tstmap, NULL);

	return system_node;
}
