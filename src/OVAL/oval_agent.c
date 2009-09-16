/**
 * @file oval_agent.c
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

#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"
#include "oval_system_characteristics_impl.h"
#include "oval_results_impl.h"
#include <string.h>
#include <time.h>

int oval_errno;
typedef struct export_target {
	char *filename;
	char *encoding;
} export_target_t;

struct export_target *export_target_new(char *filename, char* encoding){
	export_target_t *target = (export_target_t *)malloc(sizeof(export_target_t));
	target->filename = strdup(filename);
	target->encoding = strdup(encoding);
	return target;
}

void export_target_free(struct export_target *target){
	free(target->filename);
	free(target->encoding);
	target->filename = NULL;
	target->encoding = NULL;
	free(target);
}

typedef struct import_source {
	char *import_source_filename;
} import_source_t;

struct import_source *import_source_file(char *filename)
{
	import_source_t *source =
	    (import_source_t *) malloc(sizeof(import_source_t));
	int namesize = strlen(filename) + 1;
	char *alloc_filename = (char *)malloc(namesize * sizeof(char));
	*alloc_filename = 0;
	strcpy(alloc_filename, filename);
	source->import_source_filename = alloc_filename;
	return source;
}

void import_source_free(struct import_source *source)
{
	free(source->import_source_filename);
	source->import_source_filename = NULL;
	free(source);
}

typedef struct oval_object_model {
	struct oval_string_map *definition_map;
	struct oval_string_map *test_map;
	struct oval_string_map *object_map;
	struct oval_string_map *state_map;
	struct oval_string_map *variable_map;
	struct oval_sysinfo    *sysinfo;
} oval_object_model_t;

struct oval_object_model *oval_object_model_new()
{
	oval_object_model_t *newmodel =
	    (oval_object_model_t *) malloc(sizeof(oval_object_model_t));
	newmodel->definition_map = oval_string_map_new();
	newmodel->object_map = oval_string_map_new();
	newmodel->state_map = oval_string_map_new();
	newmodel->test_map = oval_string_map_new();
	newmodel->variable_map = oval_string_map_new();
	//newmodel->syschar_map = oval_string_map_new();
	newmodel->sysinfo = NULL;
	return newmodel;
}

void oval_object_model_free(struct oval_object_model * model)
{
	oval_string_map_free(model->definition_map, (oscap_destruct_func)oval_definition_free);
	oval_string_map_free(model->object_map,     (oscap_destruct_func)oval_object_free);
	oval_string_map_free(model->state_map,      (oscap_destruct_func)oval_state_free);
	oval_string_map_free(model->test_map,       (oscap_destruct_func)oval_test_free);
	oval_string_map_free(model->variable_map,   (oscap_destruct_func)oval_variable_free);
	if(model->sysinfo)oval_sysinfo_free(model->sysinfo);

	model->definition_map = NULL;
	model->object_map     = NULL;
	model->state_map      = NULL;
	model->test_map       = NULL;
	model->variable_map   = NULL;
	model->sysinfo        = NULL;

	free(model);
}


typedef struct oval_syschar_model{
	struct oval_object_model *object_model;
	struct oval_string_map   *syschar_map;
	struct oval_string_map   *sysdata_map;
	struct oval_string_map   *variable_binding_map;
} oval_syschar_model_t;

struct oval_syschar_model *oval_syschar_model_new(
		struct oval_object_model *object_model,
		struct oval_iterator_variable_binding *bindings){
	oval_syschar_model_t *newmodel =
	    (oval_syschar_model_t *) malloc(sizeof(oval_syschar_model_t));
	newmodel->object_model         = object_model;
	newmodel->syschar_map          = oval_string_map_new();
	newmodel->sysdata_map          = oval_string_map_new();
	newmodel->variable_binding_map = oval_string_map_new();
	if(bindings!=NULL){
		while(oval_iterator_variable_binding_has_more(bindings)){
			struct oval_variable_binding *binding = oval_iterator_variable_binding_next(bindings);
			add_oval_syschar_model_variable_binding(newmodel, binding);
			/*
			struct oval_variable *variable = oval_variable_binding_variable(binding);
			char *varid    = oval_variable_id(variable);
			char *value    = oval_variable_binding_value(binding);
			oval_string_map_put_string(newmodel->variable_binding_map, varid, value);
			*/
		}
	}
	return newmodel;
}
void oval_syschar_model_free(struct oval_syschar_model *model){
	oval_string_map_free(model->syschar_map, (oscap_destruct_func)oval_syschar_free);
	oval_string_map_free(model->sysdata_map, (oscap_destruct_func)oval_sysdata_free);
	oval_string_map_free(model->variable_binding_map, (oscap_destruct_func)oval_variable_binding_free);

	model->object_model         = NULL;
	model->syschar_map          = NULL;
	model->sysdata_map          = NULL;
	model->variable_binding_map = NULL;
	free(model);
}

struct oval_object_model *oval_syschar_model_object_model(
		struct oval_syschar_model *model){
	return model->object_model;
}

struct oval_iterator_syschar *oval_syschar_model_syschars(
		struct oval_syschar_model *model){
	return (struct oval_iterator_syschar *)oval_string_map_values(model->syschar_map);
}

struct oval_sysinfo *oval_syschar_model_sysinfo(
		struct oval_syschar_model *model){
	struct oval_iterator_syschar *syschars = oval_syschar_model_syschars(model);
	struct oval_syschar *syschar = oval_iterator_syschar_has_more(syschars)
		?oval_iterator_syschar_next(syschars):NULL;
	return syschar==NULL?NULL:oval_syschar_sysinfo(syschar);
}

bool add_oval_syschar_model_variable_binding(struct oval_syschar_model *model, struct oval_variable_binding *binding)
{
	assert(model != NULL);
	assert(binding != NULL);

	struct oval_variable *variable = oval_variable_binding_variable(binding);
	if (variable == NULL)
		return false;

	char *varid    = oval_variable_id(variable);
	if (varid == NULL || strcmp(varid, "") == 0 || oval_string_map_get_value(model->variable_binding_map, varid) != NULL)
		return false;

	oval_string_map_put(model->variable_binding_map, varid, binding);
	return true;
}

void add_oval_definition(
			struct oval_object_model *model,
			struct oval_definition *definition)
{
	char *key = oval_definition_id(definition);
	oval_string_map_put(model->definition_map, key, (void *)definition);
}

void add_oval_test(struct oval_object_model *model, struct oval_test *test)
{
	char *key = oval_test_id(test);
	oval_string_map_put(model->test_map, key, (void *)test);
}

void add_oval_object(struct oval_object_model *model,
		     struct oval_object *object)
{
	char *key = oval_object_id(object);
	oval_string_map_put(model->object_map, key, (void *)object);
}

void add_oval_state(struct oval_object_model *model, struct oval_state *state)
{
	char *key = oval_state_id(state);
	oval_string_map_put(model->state_map, key, (void *)state);
}

void add_oval_variable(struct oval_object_model *model,
		       struct oval_variable *variable)
{
	char *key = oval_variable_id(variable);
	oval_string_map_put(model->variable_map, key, (void *)variable);
}

void add_oval_syschar(struct oval_syschar_model *model,
		       struct oval_syschar *syschar)
{
	struct oval_object *object = oval_syschar_object(syschar);
	if(object!=NULL){
		char *id = oval_object_id(object);
		oval_string_map_put(model->syschar_map, id, syschar);
	}
}

void add_oval_sysdata(struct oval_syschar_model *model,
		       struct oval_sysdata *sysdata)
{
	char *id = oval_sysdata_id(sysdata);
	if(id!=NULL){
		oval_string_map_put(model->sysdata_map, id, sysdata);
	}
}

void set_oval_sysinfo(struct oval_object_model *model, struct oval_sysinfo *sysinfo)
{
	if(model->sysinfo!=NULL)oval_sysinfo_free(model->sysinfo);
	model->sysinfo = sysinfo;
}

void load_oval_definitions(struct oval_object_model *model,
			   struct import_source *source,
			   oval_xml_error_handler eh, void *user_arg)
{
	xmlDoc *doc = xmlParseFile
		(source->import_source_filename);
	xmlTextReader *reader = xmlNewTextReaderFilename
		(source->import_source_filename);

	xmlTextReaderRead(reader);
	ovaldef_parser_parse
		(model, reader, eh, user_arg);

	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);
}

void load_oval_syschar(struct oval_syschar_model *model,
			struct import_source *source,
			oval_xml_error_handler eh, void *user_arg )
{
	xmlDoc *doc = xmlParseFile
		(source->import_source_filename);
	xmlTextReader *reader = xmlNewTextReaderFilename
		(source->import_source_filename);

	xmlTextReaderRead(reader);
	ovalsys_parser_parse
		(model, reader, eh, user_arg);

	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);
}

struct oval_definition *get_oval_definition(struct oval_object_model *model,
					    char *key)
{
	return (struct oval_definition *)oval_string_map_get_value(model->
								   definition_map,
								   key);
}

struct oval_test *get_oval_test(struct oval_object_model *model, char *key)
{
	return (struct oval_test *)oval_string_map_get_value(model->test_map,
							     key);
}

struct oval_object *get_oval_object(struct oval_object_model *model, char *key)
{
	return (struct oval_object *)oval_string_map_get_value(model->
							       object_map, key);
}

struct oval_state *get_oval_state(struct oval_object_model *model, char *key)
{
	return (struct oval_state *)oval_string_map_get_value(model->state_map,
							      key);
}

struct oval_variable *get_oval_variable(struct oval_object_model *model,
					char *key)
{
	return (struct oval_variable *)oval_string_map_get_value(model->
								 variable_map,
								 key);
}

struct oval_syschar *get_oval_syschar(struct oval_syschar_model *model,
					char *object_id)
{
	return (struct oval_syschar *)oval_string_map_get_value
		(model->syschar_map, object_id);
}

struct oval_sysdata *get_oval_sysdata(struct oval_syschar_model *model,
					char *id)
{
	return (struct oval_sysdata *)oval_string_map_get_value
		(model->sysdata_map, id);
}

struct oval_iterator_definition *get_oval_definitions(struct oval_object_model
						      *model)
{
	return (struct oval_iterator_definition *)oval_string_map_values(model->
									 definition_map);
}

struct oval_iterator_test *get_oval_tests(struct oval_object_model *model)
{
	return (struct oval_iterator_test *)oval_string_map_values(model->
								   test_map);
}

struct oval_iterator_object *get_oval_objects(struct oval_object_model *model)
{
	return (struct oval_iterator_object *)oval_string_map_values(model->
								     object_map);
}

struct oval_iterator_state *get_oval_states(struct oval_object_model *model)
{
	return (struct oval_iterator_state *)oval_string_map_values(model->
								    state_map);
}

struct oval_iterator_variable *get_oval_variables(struct oval_object_model
						  *model)
{
	return (struct oval_iterator_variable *)oval_string_map_values(model->
								       variable_map);
}

struct oval_iterator_syschar *get_oval_syschars(struct oval_syschar_model
						  *model)
{
	return (struct oval_iterator_syschar *)oval_string_map_values(model->
								       syschar_map);
}

struct oval_syschar *get_oval_syschar_new
	(struct oval_syschar_model *model, struct oval_object *object)
{
	char *object_id = oval_object_id(object);
	struct oval_syschar *syschar = get_oval_syschar(model, object_id);
	if (syschar == NULL) {
		syschar = oval_syschar_new(object);
		add_oval_syschar(model, syschar);
	}
	return syschar;
}

struct oval_sysdata *get_oval_sysdata_new(struct oval_syschar_model *model, char *id)
{
	struct oval_sysdata *sysdata = get_oval_sysdata(model, id);
	if (sysdata == NULL) {
		sysdata = oval_sysdata_new(id);
		add_oval_sysdata(model, sysdata);
	}
	return sysdata;
}

struct oval_definition *get_oval_definition_new(struct oval_object_model *model, char *id)
{
	struct oval_definition *definition = get_oval_definition(model, id);
	if (definition == NULL) {
		definition = oval_definition_new(id);
		add_oval_definition(model, definition);
	}
	return definition;
}


struct oval_variable *get_oval_variable_new(struct oval_object_model *model, char *id, oval_variable_type_enum type)
{
	struct oval_variable *variable = get_oval_variable(model, id);
	if (variable == NULL) {
		variable = oval_variable_new(id, type);
		add_oval_variable(model, variable);
	}
	return variable;
}

struct oval_state *get_oval_state_new(struct oval_object_model *model, char *id)
{
	struct oval_state *state = get_oval_state(model, id);
	if (state == NULL) {
		state = oval_state_new(id);
		add_oval_state(model, state);
	}
	return state;
}

struct oval_object *get_oval_object_new(struct oval_object_model *model, char *id)
{
	struct oval_object *object = get_oval_object(model, id);
	if (object == NULL) {
		object = oval_object_new(id);
		add_oval_object(model, object);
	}
	return object;
}

struct oval_test *get_oval_test_new(struct oval_object_model *model, char *id)
{
	struct oval_test *test = get_oval_test(model, id);
	if (test == NULL) {
		test = oval_test_new(id);
		add_oval_test(model, test);
	}
	return test;
}

int _generator_to_dom(xmlDocPtr doc, xmlNode *tag_generator)
{
	xmlNs *ns_common  = xmlSearchNsByHref(doc, tag_generator, OVAL_COMMON_NAMESPACE);
	xmlNewChild
		(tag_generator, ns_common, BAD_CAST "product_name", BAD_CAST "OPEN SCAP");
	xmlNewChild
		(tag_generator, ns_common, BAD_CAST "schema_version", BAD_CAST "5.5");
	time_t epoch_time[] = {0};time(epoch_time);
	struct tm *lt = localtime(epoch_time);
	char timestamp[] = "yyyy-mm-ddThh:mm:ss";
	snprintf(timestamp, sizeof(timestamp),"%4d-%02d-%02dT%02d:%02d:%02d",
	1900+lt->tm_year, 1+lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
	xmlNewChild
		(tag_generator, ns_common, BAD_CAST "timestamp", timestamp);
	return 1;
}



struct oval_results_model{
	struct oval_object_model      *object_model;
	struct oval_collection        *systems;
};

typedef struct oval_results_model oval_results_model_t;

struct oval_results_model *oval_results_model_new
	(struct oval_object_model *object_model,
			struct oval_syschar_model **syschar_models)
{
	oval_results_model_t *model = (oval_results_model_t *)
		malloc(sizeof(oval_results_model_t));
	model->systems       = oval_collection_new();
	model->object_model  = object_model;
	if(syschar_models){
		struct oval_syschar_model *syschar_model;
		for(syschar_model = *syschar_models;syschar_model;
		    syschar_model = *(++syschar_models)){
			struct oval_result_system *system = oval_result_system_new(syschar_model);
			add_oval_results_model_system(model, system);
		}
	}
	return model;
}

void oval_results_model_free(struct oval_results_model *model)
{
	oval_collection_free_items
	(model->systems, (oscap_destruct_func)oval_result_system_free);
	model->object_model  = NULL;
	model->systems       = NULL;
	free(model);
}

struct oval_object_model *oval_results_model_object_model
	(struct oval_results_model *model)
{
	return model->object_model;
}

struct oval_iterator_result_system *oval_results_model_systems
	(struct oval_results_model *model){
	return (struct oval_iterator_result_system *)
			oval_collection_iterator(model->systems);
}

void add_oval_results_model_system
	(struct oval_results_model *model, struct oval_result_system *system)
{
	if(system)oval_collection_add(model->systems, system);
}

struct oval_result_directives *load_oval_results
	(struct oval_results_model *model, struct import_source *source,
			oval_xml_error_handler handler, void *client_data)
{
	xmlDoc *doc = xmlParseFile(source->import_source_filename);
	xmlTextReader *reader = xmlNewTextReaderFilename(source->import_source_filename);

	xmlTextReaderRead(reader);
	struct oval_result_directives *directives = ovalres_parser_parse
		(model, reader, handler, client_data);

	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);
	return directives;
}

void _oval_agent_scan_for_extensions_tests
	(struct oval_criteria_node *node,
	 struct oval_string_map *extmap,
	 struct oval_string_map *tstmap,
	 oval_definitions_resolver resolver, void *user_arg)
{
	oval_criteria_node_type_enum type = oval_criteria_node_type(node);
	switch(type)
	{
	case NODETYPE_CRITERIA:{
		struct oval_iterator_criteria_node *subnodes = oval_criteria_node_subnodes(node);
		while(oval_iterator_criteria_node_has_more(subnodes)){
			struct oval_criteria_node *subnode = oval_iterator_criteria_node_next(subnodes);
			_oval_agent_scan_for_extensions_tests(subnode, extmap, tstmap, resolver, user_arg);
		}
	};break;
	case NODETYPE_CRITERION:{
		struct oval_test *test = oval_criteria_node_test(node);
		char *tstid = oval_test_id(test);
		void *value = oval_string_map_get_value(tstmap,tstid);
		if(value==NULL){
			oval_string_map_put(tstmap, tstid, test);
		}
	};break;
	case NODETYPE_EXTENDDEF:{
		struct oval_definition *extends = oval_criteria_node_definition(node);
		if(resolver && !(*resolver)(extends, user_arg)){
			char *extid = oval_definition_id(extends);
			void *value = oval_string_map_get_value(extmap, extid);
			if(value==NULL){
				oval_string_map_put(extmap, extid, extends);
			}
		}
	};break;
	default: break;
	}
}

void _oval_agent_scan_entity_for_references
	(struct oval_entity *entity,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap);

void _oval_agent_scan_set_for_references
	(struct oval_set *set,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap);

void _oval_agent_scan_object_for_references
	(struct oval_object *object,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap)
{
	struct oval_iterator_object_content *contents = oval_object_object_content(object);
	while(oval_iterator_object_content_has_more(contents)){
		struct oval_object_content *content = oval_iterator_object_content_next(contents);
		struct oval_entity *entity = oval_object_content_entity(content);
		if(entity)_oval_agent_scan_entity_for_references(entity, objmap, sttmap, varmap);
		struct oval_set *set = oval_object_content_set(content);
		if(set)_oval_agent_scan_set_for_references(set, objmap, sttmap, varmap);
	}
}
void _oval_agent_scan_state_for_references
	(struct oval_state *state,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap)
{
	struct oval_iterator_state_content *contents = oval_state_contents(state);
	while(oval_iterator_state_content_has_more(contents)){
		struct oval_state_content *content = oval_iterator_state_content_next(contents);
		struct oval_entity *entity = oval_state_content_entity(content);
		if(entity)_oval_agent_scan_entity_for_references(entity, objmap, sttmap, varmap);
	}
}

void _oval_agent_scan_component_for_references
	(struct oval_component *component,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap)
{
	struct oval_variable *variable = oval_component_variable(component);
	if(variable){
		char *varid = oval_variable_id(variable);
		void *value = oval_string_map_get_value(varmap, varid);
		if(value==NULL){
			oval_string_map_put(varmap, varid, variable);
			struct oval_component *component = oval_variable_component(variable);
			if(component){
				_oval_agent_scan_component_for_references(component, objmap, sttmap, varmap);
			}
		}
	}else{
		struct oval_iterator_component *fcomponents = oval_component_function_components(component);
		if(fcomponents)while(oval_iterator_component_has_more(fcomponents)){
			struct oval_component *fcomponent = oval_iterator_component_next(fcomponents);
			_oval_agent_scan_component_for_references(fcomponent, objmap, sttmap, varmap);
		}

		struct oval_object *object = oval_component_object(component);
		if(object){
			char *objid = oval_object_id(object);
			void *value = oval_string_map_get_value(objmap, objid);
			if(value==NULL){
				oval_string_map_put(objmap, objid, object);
				_oval_agent_scan_object_for_references(object, objmap, sttmap, varmap);
			}
		}
	}
}

void _oval_agent_scan_entity_for_references
	(struct oval_entity *entity,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap)
{
	struct oval_variable *variable = oval_entity_variable(entity);
	if(variable){
		char *varid = oval_variable_id(variable);
		void *value = oval_string_map_get_value(varmap, varid);
		if(value==NULL){
			oval_string_map_put(varmap, varid, variable);
			struct oval_component *component = oval_variable_component(variable);
			if(component){
				_oval_agent_scan_component_for_references(component, objmap, sttmap, varmap);
			}
		}
	}
}

void _oval_agent_scan_set_for_references
	(struct oval_set *set,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap)
{
	struct oval_iterator_object *objects = oval_set_objects(set);
	if(objects)while(oval_iterator_object_has_more(objects)){
		struct oval_object *object = oval_iterator_object_next(objects);
		char *objid = oval_object_id(object);
		void *value = oval_string_map_get_value(objmap, objid);
		if(value==NULL){
			oval_string_map_put(objmap, objid, object);
			_oval_agent_scan_object_for_references(object, objmap, sttmap, varmap);
		}
	}
	struct oval_iterator_state *states = oval_set_filters(set);
	if(states)while(oval_iterator_state_has_more(states)){
		struct oval_state *state = oval_iterator_state_next(states);
		char *sttid = oval_state_id(state);
		void *value = oval_string_map_get_value(sttmap, sttid);
		if(value==NULL){
			oval_string_map_put(sttmap, sttid, state);
			_oval_agent_scan_state_for_references(state, objmap, sttmap, varmap);
		}
	}
	struct oval_iterator_set *subsets = oval_set_subsets(set);
	if(subsets)while(oval_iterator_set_has_more(subsets)){
		struct oval_set *subset = oval_iterator_set_next(subsets);
		_oval_agent_scan_set_for_references(subset, objmap, sttmap, varmap);
	}
}

xmlNode *oval_definitions_to_dom
	(struct oval_object_model *object_model, xmlDocPtr doc, xmlNode *parent,
	 oval_definitions_resolver resolver, void *user_arg)
{
	xmlNodePtr root_node;
	if(parent){
		root_node = xmlNewChild(parent, NULL, BAD_CAST "oval_definitions", NULL);
	}else{
		root_node = xmlNewNode(NULL, BAD_CAST "oval_definitions");
	    xmlDocSetRootElement(doc, root_node);
	}
	xmlNs *ns_common  = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE,BAD_CAST "oval");
	xmlNs *ns_defntns = xmlNewNs(root_node, OVAL_DEFINITIONS_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_defntns);

    xmlNode *tag_generator = xmlNewChild
		(root_node, ns_defntns, BAD_CAST "generator", NULL);

    _generator_to_dom(doc, tag_generator);

    struct oval_string_map *tstmap = oval_string_map_new();
    struct oval_iterator_definition *definitions = get_oval_definitions(object_model);
    if(oval_iterator_definition_has_more(definitions)){
        struct oval_string_map *extmap = oval_string_map_new();
    	xmlNode *definitions_node = NULL;
    	while(oval_iterator_definition_has_more(definitions)){
    		struct oval_definition *definition = oval_iterator_definition_next(definitions);
			if(resolver==NULL || (*resolver)(definition, user_arg)){
				if(definitions_node==NULL)definitions_node
					= xmlNewChild(root_node, ns_defntns, "definitions", NULL);
				oval_definition_to_dom
					(definition, doc, definitions_node);
				struct oval_criteria_node *criteria = oval_definition_criteria(definition);
				_oval_agent_scan_for_extensions_tests(criteria, extmap, tstmap, resolver, user_arg);
			}
    	}
        definitions = (struct oval_iterator_definition *)oval_string_map_values(extmap);
    	while(oval_iterator_definition_has_more(definitions)){
    		struct oval_definition *definition = oval_iterator_definition_next(definitions);
    		oval_definition_to_dom
    			(definition, doc, definitions_node);
    	}
        oval_string_map_free(extmap, NULL);
    }

    struct oval_string_map *objmap = oval_string_map_new();
    struct oval_string_map *sttmap = oval_string_map_new();
    struct oval_string_map *varmap = oval_string_map_new();

    struct oval_iterator_test *tests = (struct oval_iterator_test *)oval_string_map_values(tstmap);
    if(oval_iterator_test_has_more(tests)){
    	xmlNode *tests_node = xmlNewChild(root_node, ns_defntns, "tests", NULL);
    	while(oval_iterator_test_has_more(tests)){
    		struct oval_test *test = oval_iterator_test_next(tests);
    		oval_test_to_dom(test, doc, tests_node);
    		struct oval_object *object = oval_test_object(test);
    		if(object){
    			char *object_id = oval_object_id(object);
    			void *value = oval_string_map_get_value(objmap, object_id);
    			if(value==NULL){
        			oval_string_map_put(objmap, object_id, object);
        			_oval_agent_scan_object_for_references(object, objmap, sttmap, varmap);
    			}
    		}
    		struct oval_state *state = oval_test_state(test);
    		if(state){
    			char *state_id = oval_state_id(state);
    			void *value = oval_string_map_get_value(sttmap, state_id);
    			if(value==NULL){
        			oval_string_map_put(sttmap, state_id, state);
        			_oval_agent_scan_state_for_references(state, objmap, sttmap, varmap);
    			}
    		}
    	}
    }

    struct oval_iterator_object *objects = (struct oval_iterator_object *)oval_string_map_values(objmap);
    if(oval_iterator_object_has_more(objects)){
    	xmlNode *objects_node = xmlNewChild(root_node, ns_defntns, "objects", NULL);
    	int index;for(index=0;oval_iterator_object_has_more(objects); index++){
    		struct oval_object *object = oval_iterator_object_next(objects);
    		oval_object_to_dom(object, doc, objects_node);
    	}
    }
    struct oval_iterator_state *states = (struct oval_iterator_state *)oval_string_map_values(sttmap);
    if(oval_iterator_state_has_more(states)){
    	xmlNode *states_node = xmlNewChild(root_node, ns_defntns, "states", NULL);
    	while(oval_iterator_state_has_more(states)){
    		struct oval_state *state = oval_iterator_state_next(states);
    		oval_state_to_dom(state, doc, states_node);
    	}
    }
    struct oval_iterator_variable *variables = (struct oval_iterator_variable *)oval_string_map_values(varmap);
    if(oval_iterator_variable_has_more(variables)){
    	xmlNode *variables_node = xmlNewChild(root_node, ns_defntns, "variables", NULL);
    	while(oval_iterator_variable_has_more(variables)){
    		struct oval_variable *variable = oval_iterator_variable_next(variables);
    		oval_variable_to_dom(variable, doc, variables_node);
    	}
    }

    oval_string_map_free(tstmap, NULL);
    oval_string_map_free(objmap, NULL);
    oval_string_map_free(sttmap, NULL);
    oval_string_map_free(varmap, NULL);

	return root_node;
}

int export_definitions(
		struct oval_object_model *model, struct export_target *target){

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	oval_definitions_to_dom(model, doc, NULL, NULL, NULL);
	/*
	 * Dumping document to stdio or file
	 */
	int retcode = xmlSaveFormatFileEnc(target->filename, doc, target->encoding, 1);

	xmlFreeDoc(doc);
    return retcode;
}

xmlNode *oval_characteristics_to_dom
	(struct oval_syschar_model *syschar_model, xmlDocPtr doc, xmlNode *parent)
{
	xmlNodePtr root_node;
	if(parent){
		root_node = xmlNewChild(parent, NULL, BAD_CAST "oval_system_characteristics",NULL);
	}else{
		root_node = xmlNewNode(NULL, BAD_CAST "oval_system_characteristics");
	    xmlDocSetRootElement(doc, root_node);
	}
	xmlNs *ns_common  = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE,BAD_CAST "oval");
	xmlNs *ns_syschar = xmlNewNs(root_node, OVAL_SYSCHAR_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_syschar);


    xmlNode *tag_generator = xmlNewChild
		(root_node, ns_syschar, BAD_CAST "generator", NULL);

    _generator_to_dom(doc, tag_generator);

	oval_sysinfo_to_dom(oval_syschar_model_sysinfo(syschar_model), doc, root_node);

	xmlNode *tag_objects = xmlNewChild
		(root_node, ns_syschar, BAD_CAST "collected_objects",NULL);

	struct oval_string_map *sysdata_map = oval_string_map_new();
	struct oval_iterator_syschar *syschars = oval_syschar_model_syschars(syschar_model);
	while(oval_iterator_syschar_has_more(syschars)){
		struct oval_syschar *syschar =oval_iterator_syschar_next(syschars);
		oval_syschar_to_dom(syschar, doc, tag_objects);
		struct oval_iterator_sysdata *sysdatas = oval_syschar_sysdata(syschar);
		while(oval_iterator_sysdata_has_more(sysdatas)){
			struct oval_sysdata *sysdata = oval_iterator_sysdata_next(sysdatas);
			oval_string_map_put(sysdata_map, oval_sysdata_id(sysdata), sysdata);
		}
	}

	struct oval_iterator *sysdatas = oval_string_map_values(sysdata_map);
	if(oval_collection_iterator_has_more(sysdatas)){
		xmlNode *tag_items = xmlNewChild
			(root_node, ns_syschar, BAD_CAST "system_data", NULL);
		while(oval_collection_iterator_has_more(sysdatas)){
			struct oval_sysdata *sysdata = (struct oval_sysdata *)
			oval_collection_iterator_next(sysdatas);
			oval_sysdata_to_dom(sysdata, doc, tag_items);
		}
	}
	oval_string_map_free(sysdata_map, NULL);

	return root_node;
}

int export_characteristics(
		struct oval_syschar_model *model, struct export_target *target){

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	oval_characteristics_to_dom(model, doc, NULL);
	/*
	 * Dumping document to stdio or file
	 */
	int retcode = xmlSaveFormatFileEnc(target->filename, doc, target->encoding, 1);

	xmlFreeDoc(doc);
    return retcode;
}

void _scan_for_viewable_definitions
	(struct oval_results_model *results_model, struct oval_result_directives *directives, struct oval_string_map *defids)
{
	struct oval_iterator_result_system *systems = oval_results_model_systems(results_model);
	while(oval_iterator_result_system_has_more(systems)){
		struct oval_result_system *system = oval_iterator_result_system_next(systems);
		struct oval_iterator_result_definition *rslt_definitions = oval_result_system_definitions(system);
		while(oval_iterator_result_definition_has_more(rslt_definitions)){
			struct oval_result_definition *rslt_definition = oval_iterator_result_definition_next(rslt_definitions);
			oval_result_enum result = oval_result_definition_result(rslt_definition);
			if(oval_result_directive_reported(directives, result)){
				struct oval_definition *oval_definition = oval_result_definition_definition(rslt_definition);
				if(oval_definition){
					char *defid = oval_definition_id(oval_definition);
					oval_string_map_put(defids, defid, oval_definition);
				}
			}
		}
	}
}

bool _resolve_oval_definition_from_map
	(struct oval_definition *oval_definition, struct oval_string_map *defids)
{
	char *defid = oval_definition_id(oval_definition);
	return oval_string_map_get_value(defids, defid)!=NULL;
}

xmlNode *oval_results_to_dom
	(struct oval_results_model *results_model, struct oval_result_directives *directives,
			xmlDocPtr doc, xmlNode *parent)
{
	xmlNode *root_node;
	if(parent){
		root_node = xmlNewChild(parent, NULL, "oval_results", NULL);
	}else{
		root_node = xmlNewNode(NULL, BAD_CAST "oval_results");
	    xmlDocSetRootElement(doc, root_node);
	}
	xmlNs *ns_common  = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE , BAD_CAST "oval");
	xmlNs *ns_results = xmlNewNs(root_node, OVAL_RESULTS_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_results);

    xmlNode *tag_generator = xmlNewChild
		(root_node, ns_results, BAD_CAST "generator", NULL);

    _generator_to_dom(doc, tag_generator);
	oval_result_directives_to_dom(directives, doc, root_node);

	//Scan for viewable definitions.
	struct oval_string_map *defids = oval_string_map_new();
	_scan_for_viewable_definitions(results_model, directives, defids);

	struct oval_object_model *object_model = oval_results_model_object_model(results_model);
	oval_definitions_to_dom
		(object_model, doc, root_node,
				(oval_definitions_resolver *)_resolve_oval_definition_from_map, defids);

	xmlNode *results_node = xmlNewChild(root_node, ns_results, "results", NULL);

	struct oval_iterator_result_system *systems = oval_results_model_systems(results_model);
	while(oval_iterator_result_system_has_more(systems)){
		struct oval_result_system *system
			= oval_iterator_result_system_next(systems);
		oval_result_system_to_dom
			(system, results_model, directives,
				doc, results_node);
	}
    return root_node;
}

int export_results
	(struct oval_results_model *results_model, struct oval_result_directives *directives,
			struct export_target *target)
{
	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	oval_results_to_dom(results_model, directives, doc, NULL);
	xmlSaveFormatFileEnc
		(target->filename, doc, target->encoding, 1);

	xmlFreeDoc(doc);

	return 1;
}
