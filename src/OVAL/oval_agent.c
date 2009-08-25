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
			struct oval_variable *variable = oval_variable_binding_variable(binding);
			char *varid    = oval_variable_id(variable);
			char *value    = oval_variable_binding_value(binding);
			oval_string_map_put_string(newmodel->variable_binding_map, varid, value);
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
	oval_parser_parse(model, source->import_source_filename, eh, user_arg);
}

void load_oval_syschar(struct oval_syschar_model *model,
			struct import_source *source,
			oval_xml_error_handler eh, void *user_arg )
{
	ovalsys_parser_parse(model, source->import_source_filename, eh, user_arg);
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

int export_characteristics(
		struct oval_syschar_model *model, struct export_target *target){

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "oval_system_characteristics");
	xmlNs *ns_common  = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE,BAD_CAST "oval");
	xmlNs *ns_syschar = xmlNewNs(root_node, OVAL_SYSCHAR_NAMESPACE, "oval-sc");
	xmlNs *ns_digsig  = xmlNewNs(root_node, OVAL_DIGSIG_NAMESPACE , "ds");

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_digsig);
	xmlSetNs(root_node, ns_syschar);

    xmlDocSetRootElement(doc, root_node);

    xmlNode *tag_generator = xmlNewChild
		(root_node, ns_syschar, BAD_CAST "generator", NULL);

    xmlNewChild
		(tag_generator, ns_common, BAD_CAST "product_name", BAD_CAST "OPEN SCAP");
    xmlNewChild
		(tag_generator, ns_common, BAD_CAST "schema_version", BAD_CAST "5.5");
    xmlNewChild
		(tag_generator, ns_common, BAD_CAST "timestamp", NULL);//TODO

    oval_sysinfo_to_dom(oval_syschar_model_sysinfo(model), doc, root_node);

    xmlNode *tag_objects = xmlNewChild
		(root_node, ns_syschar, BAD_CAST "collected_objects",NULL);

    struct oval_string_map *sysdata_map = oval_string_map_new();
    struct oval_iterator_syschar *syschars = oval_syschar_model_syschars(model);
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

    /*
     * Dumping document to stdio or file
     */
    int retcode = xmlSaveFormatFileEnc(target->filename, doc, target->encoding, 1);

	xmlFreeDoc(doc);
	xmlFreeNs(ns_common );
	xmlFreeNs(ns_digsig );
    xmlFreeNs(ns_syschar);

    return retcode;
}

struct oval_results_model{
	struct oval_syschar_model     *syschar_model;
	struct oval_result_directives *directives;
	struct oval_collection        *systems;
	struct oval_string_map        *definition_map;
	struct oval_string_map        *test_map;
};

typedef struct oval_results_model oval_results_model_t;

struct oval_results_model *oval_results_model_new(
		struct oval_syschar_model *syschar_model)
{
	oval_results_model_t *model = (oval_results_model_t *)
		malloc(sizeof(oval_results_model_t));
	model->syschar_model = syschar_model;
	model->directives    = oval_result_directives_new();
	model->systems       = oval_collection_new();
	model->definition_map= oval_string_map_new();
	model->test_map      = oval_string_map_new();
	return model;
}

void oval_results_model_free(struct oval_results_model *model)
{
	oval_collection_free_items
	(model->systems, (oscap_destruct_func)oval_result_system_free);
	oval_result_directives_free(model->directives);
	model->directives    = NULL;
	model->systems       = NULL;
	model->syschar_model = NULL;
	free(model);
}

struct oval_syschar_model *oval_results_model_syschar_model(
		struct oval_results_model *model)
{
	return model->syschar_model;
}

struct oval_result_directives *oval_results_model_directives(
		struct oval_results_model *model)
{
	return model->directives;
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

struct oval_iterator_results *oval_results_model_results(
		struct oval_results_model *model);//TODO: implement

struct oval_result *get_oval_result(
		struct oval_results_model *model,
		char *object_id);//TODO: implement

void load_oval_results(struct oval_results_model *model, struct import_source *source,
			oval_xml_error_handler handler, void *client_data)
{

	ovalres_parser_parse(model, source->import_source_filename, handler, client_data);
}

