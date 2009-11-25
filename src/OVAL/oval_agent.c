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

struct oval_export_target *oval_export_target_new_file(char *filename, char* encoding){
	oval_export_target_t *target = (oval_export_target_t *)malloc(sizeof(oval_export_target_t));
	target->filename = strdup(filename);
	target->encoding = strdup(encoding);
	return target;
}

void oval_export_target_free(struct oval_export_target *target){
	free(target->filename);
	free(target->encoding);
	target->filename = NULL;
	target->encoding = NULL;
	free(target);
}

struct oval_import_source *oval_import_source_new_file(char *filename)
{
	oval_import_source_t *source =
	    (oval_import_source_t *) malloc(sizeof(oval_import_source_t));
	int namesize = strlen(filename) + 1;
	char *alloc_filename = (char *)malloc(namesize * sizeof(char));
	*alloc_filename = 0;
	strcpy(alloc_filename, filename);
	source->import_source_filename = alloc_filename;
	return source;
}

void oval_import_source_free(struct oval_import_source *source)
{
	free(source->import_source_filename);
	source->import_source_filename = NULL;
	free(source);
}

typedef struct oval_definition_model {
	struct oval_string_map *definition_map;
	struct oval_string_map *test_map;
	struct oval_string_map *object_map;
	struct oval_string_map *state_map;
	struct oval_string_map *variable_map;
	struct oval_sysinfo    *sysinfoX;
} oval_definition_model_t;

struct oval_definition_model *oval_definition_model_new()
{
	oval_definition_model_t *newmodel =
	    (oval_definition_model_t *) malloc(sizeof(oval_definition_model_t));
	newmodel->definition_map = oval_string_map_new();
	newmodel->object_map = oval_string_map_new();
	newmodel->state_map = oval_string_map_new();
	newmodel->test_map = oval_string_map_new();
	newmodel->variable_map = oval_string_map_new();
	return newmodel;
}

typedef void (*_oval_result_system_clone_func)(void *, struct oval_definition_model *);

static void _oval_definition_model_clone
	(struct oval_string_map *oldmap, struct oval_definition_model *newmodel, _oval_result_system_clone_func cloner)
{
	struct oval_string_iterator *keys = (struct oval_string_iterator *)oval_string_map_keys(oldmap);
	while(oval_string_iterator_has_more(keys)){
		char *key = oval_string_iterator_next(keys);
		void *olditem = oval_string_map_get_value(oldmap, key);
		(*cloner)(olditem, newmodel);
	}
	oval_string_iterator_free(keys);
}

struct oval_definition_model *oval_definition_model_clone
	(struct oval_definition_model *oldmodel)
{
	struct oval_definition_model *newmodel = oval_definition_model_new();
	_oval_definition_model_clone
		(oldmodel->definition_map, newmodel,
				(_oval_result_system_clone_func)oval_definition_clone);
	_oval_definition_model_clone
		(oldmodel->object_map, newmodel,
				(_oval_result_system_clone_func)oval_object_clone);
	_oval_definition_model_clone
		(oldmodel->state_map, newmodel,
				(_oval_result_system_clone_func)oval_state_clone);
	_oval_definition_model_clone
		(oldmodel->test_map, newmodel,
				(_oval_result_system_clone_func)oval_test_clone);
	_oval_definition_model_clone
		(oldmodel->variable_map, newmodel,
				(_oval_result_system_clone_func)oval_variable_clone);
	return newmodel;
}

void oval_definition_model_free(struct oval_definition_model * model)
{
	oval_string_map_free(model->definition_map, (oscap_destruct_func)oval_definition_free);
	oval_string_map_free(model->object_map,     (oscap_destruct_func)oval_object_free);
	oval_string_map_free(model->state_map,      (oscap_destruct_func)oval_state_free);
	oval_string_map_free(model->test_map,       (oscap_destruct_func)oval_test_free);
	oval_string_map_free(model->variable_map,   (oscap_destruct_func)oval_variable_free);

	model->definition_map = NULL;
	model->object_map     = NULL;
	model->state_map      = NULL;
	model->test_map       = NULL;
	model->variable_map   = NULL;

	free(model);
}


typedef struct oval_syschar_model{
	struct oval_sysinfo          *sysinfo;
	struct oval_definition_model *definition_model;
	struct oval_string_map       *syschar_map;
	struct oval_string_map       *sysdata_map;
	struct oval_string_map       *variable_binding_map;
} oval_syschar_model_t;

struct oval_syschar_model *oval_syschar_model_new(
		struct oval_definition_model *definition_model)
{
	oval_syschar_model_t *newmodel =
	    (oval_syschar_model_t *) malloc(sizeof(oval_syschar_model_t));
	newmodel->sysinfo              = NULL;
	newmodel->definition_model     = definition_model;
	newmodel->syschar_map          = oval_string_map_new();
	newmodel->sysdata_map          = oval_string_map_new();
	newmodel->variable_binding_map = oval_string_map_new();
	return newmodel;
}


typedef void (*_oval_syschar_model_clone_func)(void *, struct oval_syschar_model *);

static void _oval_syschar_model_clone_variable_binding(struct oval_variable_binding *old_binding, struct oval_syschar_model *new_model){
	struct oval_definition_model *new_defmodel = oval_syschar_model_get_definition_model(new_model);
	struct oval_variable_binding *new_binding = oval_variable_binding_clone(old_binding, new_defmodel);
	oval_syschar_model_add_variable_binding(new_model, new_binding);
}

static void _oval_syschar_model_clone
	(struct oval_string_map *oldmap, struct oval_syschar_model *newmodel, _oval_syschar_model_clone_func cloner)
{
	struct oval_string_iterator *keys = (struct oval_string_iterator *)oval_string_map_keys(oldmap);
	while(oval_string_iterator_has_more(keys)){
		char *key = oval_string_iterator_next(keys);
		void *olditem = oval_string_map_get_value(oldmap, key);
		(*cloner)(olditem, newmodel);
	}
	oval_string_iterator_free(keys);
}


struct oval_syschar_model *oval_syschar_model_clone(struct oval_syschar_model *old_model){
	struct oval_syschar_model *new_model = oval_syschar_model_new(old_model->definition_model);
	_oval_syschar_model_clone(old_model->syschar_map, new_model,
			(_oval_syschar_model_clone_func)oval_syschar_clone);
	_oval_syschar_model_clone(old_model->sysdata_map, new_model,
			(_oval_syschar_model_clone_func)oval_sysdata_clone);
	_oval_syschar_model_clone(old_model->variable_binding_map, new_model,
			(_oval_syschar_model_clone_func)_oval_syschar_model_clone_variable_binding);
	struct oval_sysinfo *old_sysinfo = oval_syschar_model_get_sysinfo(old_model);
	struct oval_sysinfo *new_sysinfo = oval_sysinfo_clone(old_sysinfo);
	oval_syschar_model_set_sysinfo(new_model, new_sysinfo);
	return new_model;
}

void oval_syschar_model_free(struct oval_syschar_model *model)
{
	if(model->sysinfo)oval_sysinfo_free(model->sysinfo);
	oval_string_map_free(model->syschar_map, (oscap_destruct_func)oval_syschar_free);
	oval_string_map_free(model->sysdata_map, (oscap_destruct_func)oval_sysdata_free);
	oval_string_map_free(model->variable_binding_map, (oscap_destruct_func)oval_variable_binding_free);

	model->sysinfo              = NULL;
	model->definition_model     = NULL;
	model->syschar_map          = NULL;
	model->sysdata_map          = NULL;
	model->variable_binding_map = NULL;
	free(model);
}

struct oval_definition_model *oval_syschar_model_get_definition_model(
		struct oval_syschar_model *model)
{
	return model->definition_model;
}

struct oval_syschar_iterator *oval_syschar_model_get_syschars(
		struct oval_syschar_model *model)
{
	struct oval_syschar_iterator *iterator = (struct oval_syschar_iterator *)oval_string_map_values(model->syschar_map);
	return iterator;
}

static struct oval_sysinfo *_oval_syschar_model_probe_sysinfo(struct oval_syschar_model *model)
{
        oval_pctx_t *pctx;
	struct oval_sysinfo *sysinfo;

        pctx    = oval_pctx_new (NULL);
        sysinfo = oval_probe_sysinf_eval (pctx);
        oval_pctx_free (pctx);
        
	if(sysinfo==NULL){
		fprintf(stderr, "WARNING: sysinfo probe returns NULL\n    %s(%d)\n", __FILE__, __LINE__);
	}
	return (model->sysinfo=sysinfo);
}

struct oval_sysinfo *oval_syschar_model_get_sysinfo(struct oval_syschar_model *model)
{
	return (model->sysinfo)?model->sysinfo:_oval_syschar_model_probe_sysinfo(model);
}

void oval_syschar_model_set_sysinfo(struct oval_syschar_model *model, struct oval_sysinfo *sysinfo)
{
	if(sysinfo){
		model->sysinfo = oval_sysinfo_clone(sysinfo);
	}
}

void oval_definition_model_add_definition(
			struct oval_definition_model *model,
			struct oval_definition *definition)
{
	char *key = oval_definition_get_id(definition);
	oval_string_map_put(model->definition_map, key, (void *)definition);
}

void oval_definition_model_add_test(struct oval_definition_model *model, struct oval_test *test)
{
	char *key = oval_test_get_id(test);
	oval_string_map_put(model->test_map, key, (void *)test);
}

void oval_definition_model_add_object(struct oval_definition_model *model,
		     struct oval_object *object)
{
	char *key = oval_object_get_id(object);
	oval_string_map_put(model->object_map, key, (void *)object);
}

void oval_definition_model_add_state(struct oval_definition_model *model, struct oval_state *state)
{
	char *key = oval_state_get_id(state);
	oval_string_map_put(model->state_map, key, (void *)state);
}

void oval_definition_model_add_variable(struct oval_definition_model *model,
		       struct oval_variable *variable)
{
	char *key = oval_variable_get_id(variable);
	oval_string_map_put(model->variable_map, key, (void *)variable);
}

void oval_syschar_model_add_syschar(struct oval_syschar_model *model,
		       struct oval_syschar *syschar)
{
	struct oval_object *object = oval_syschar_get_object(syschar);
	if(object!=NULL){
		char *id = oval_object_get_id(object);
		oval_string_map_put(model->syschar_map, id, syschar);
	}
}

void oval_syschar_model_add_variable_binding(struct oval_syschar_model *model, struct oval_variable_binding *binding)
{
	struct oval_variable *variable = oval_variable_binding_get_variable(binding);
	char *varid = oval_variable_get_id(variable);
	oval_string_map_put(model->variable_binding_map, varid, binding);
}

void oval_syschar_model_probe_objects(struct oval_syschar_model *syschar_model)
{
	if(syschar_model->sysinfo==NULL)oval_syschar_model_get_sysinfo(syschar_model);
	struct oval_definition_model *definition_model = oval_syschar_model_get_definition_model(syschar_model);
	if(definition_model){
                oval_pctx_t *pctx;
		struct oval_object_iterator *objects = oval_definition_model_get_objects(definition_model);
                
                pctx = oval_pctx_new (syschar_model);
                
		while(oval_object_iterator_has_more(objects)) {
			struct oval_object *object = oval_object_iterator_next(objects);
			char *objid = oval_object_get_id(object);
			struct oval_syschar *syschar = oval_syschar_model_get_syschar(syschar_model, objid);
                        
			if(syschar==NULL) {
                                syschar = oval_probe_object_eval (pctx, object);
                                
				if(syschar == NULL) {
					syschar = oval_syschar_new(object);
					oval_syschar_set_flag(syschar,SYSCHAR_FLAG_NOT_COLLECTED);
				}
                                oval_syschar_model_add_syschar(syschar_model, syschar);
			}
		}
                
                oval_pctx_free (pctx);
		oval_object_iterator_free(objects);
	}
}

void oval_syschar_model_add_sysdata(struct oval_syschar_model *model, struct oval_sysdata *sysdata)
{
	char *id = oval_sysdata_get_id(sysdata);
	if(id!=NULL){
		oval_string_map_put(model->sysdata_map, id, sysdata);
	}
}

int oval_definition_model_import(struct oval_definition_model *model,
			   struct oval_import_source *source,
			   oval_xml_error_handler eh, void *user_arg)
{
	xmlTextReader *reader = xmlNewTextReaderFilename
		(source->import_source_filename);
	int retcode = 0;
	if(reader){
		if(xmlTextReaderRead(reader)>-1){
			retcode = ovaldef_parser_parse
				(model, reader, eh, user_arg);
		}
		xmlFreeTextReader(reader);
	}else
		fprintf(stderr, "ERROR: oval_definition_model_import: "
		"cannot open XML reader for %s\n"
		"    Code Location = %s(%d)\n",
		source->import_source_filename,
		__FILE__, __LINE__);
	return retcode;
}
void oval_syschar_model_import(struct oval_syschar_model *model,
			struct oval_import_source *source,
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

struct oval_definition *oval_definition_model_get_definition(struct oval_definition_model *model,
					    char *key)
{
	return (struct oval_definition *)oval_string_map_get_value(model->
								   definition_map,
								   key);
}

struct oval_test *oval_definition_model_get_test(struct oval_definition_model *model, char *key)
{
	return (struct oval_test *)oval_string_map_get_value(model->test_map,
							     key);
}

struct oval_object *oval_definition_model_get_object(struct oval_definition_model *model, char *key)
{
	return (struct oval_object *)oval_string_map_get_value(model->
							       object_map, key);
}

struct oval_state *oval_definition_model_get_state(struct oval_definition_model *model, char *key)
{
	return (struct oval_state *)oval_string_map_get_value(model->state_map,
							      key);
}

struct oval_variable *oval_definition_model_get_variable(struct oval_definition_model *model,
					char *key)
{
	return (struct oval_variable *)oval_string_map_get_value(model->
								 variable_map,
								 key);
}

void oval_definition_model_bind_variable_model
	(struct oval_definition_model *sysmodel, struct oval_variable_model *varmodel)
{
	//Bind values to all external variables specified in the variable model.
	struct oval_variable_iterator *variables = oval_definition_model_get_variables(sysmodel);
	while(oval_variable_iterator_has_more(variables)){
		struct oval_variable *variable = oval_variable_iterator_next(variables);
		if(oval_variable_get_type(variable)==OVAL_VARIABLE_EXTERNAL){
			char *varid = oval_variable_get_id(variable);
			oval_datatype_t var_datatype = oval_variable_model_get_datatype(varmodel, varid);
			if(var_datatype){//values are bound in the variable model
				oval_datatype_t def_datatype = oval_variable_get_datatype(variable);
				if(def_datatype==var_datatype){
					struct oval_string_iterator *values = oval_variable_model_get_values(varmodel, varid);
					while(oval_string_iterator_has_more(values)){
						char *text = oval_string_iterator_next(values);
						struct oval_value *value = oval_value_new(var_datatype, text);
						oval_variable_add_value(variable, value);
					}
					oval_string_iterator_free(values);
				}else{
					fprintf(stderr,
							"WARNING: Unmatched variable datatypes\n"
							"                             varid = %s\n"
							"         definition_model datatype = %s\n"
							"           variable_model datatype = %s\n",
							varid,
							oval_datatype_get_text(def_datatype),
							oval_datatype_get_text(var_datatype));
				}
			}
		}
	}
	oval_variable_iterator_free(variables);
}

void oval_syschar_model_bind_variable_model
	(struct oval_syschar_model *sysmodel, struct oval_variable_model *varmodel)
{
	oval_definition_model_bind_variable_model(sysmodel->definition_model, varmodel);
}


struct oval_syschar *oval_syschar_model_get_syschar(struct oval_syschar_model *model,
					char *object_id)
{
	return (struct oval_syschar *)oval_string_map_get_value
		(model->syschar_map, object_id);
}

struct oval_sysdata *oval_syschar_model_get_sysdata(struct oval_syschar_model *model, char* id)
{
	return (struct oval_sysdata *)oval_string_map_get_value
		(model->sysdata_map, id);
}

struct oval_definition_iterator *oval_definition_model_get_definitions(struct oval_definition_model
						      *model)
{
	struct oval_definition_iterator *iterator = (struct oval_definition_iterator *)oval_string_map_values(model->
									 definition_map);
	return iterator;
}

struct oval_test_iterator *oval_definition_model_get_tests(struct oval_definition_model *model)
{
	struct oval_test_iterator *iterator = (struct oval_test_iterator *)oval_string_map_values(model->
								   test_map);
	return iterator;
}

struct oval_object_iterator *oval_definition_model_get_objects(struct oval_definition_model *model)
{
	struct oval_object_iterator *iterator = (struct oval_object_iterator *)oval_string_map_values(model->
								     object_map);
	return iterator;
}

struct oval_state_iterator *oval_definition_model_get_states(struct oval_definition_model *model)
{
	struct oval_state_iterator *iterator = (struct oval_state_iterator *)oval_string_map_values(model->
								    state_map);
	return iterator;
}

struct oval_variable_iterator *oval_definition_model_get_variables(struct oval_definition_model
						  *model)
{
	struct oval_variable_iterator *iterator = (struct oval_variable_iterator *)oval_string_map_values(model->
								       variable_map);
	return iterator;
}

struct oval_syschar *get_oval_syschar_new
	(struct oval_syschar_model *model, struct oval_object *object)
{
	char *object_id = oval_object_get_id(object);
	struct oval_syschar *syschar = oval_syschar_model_get_syschar(model, object_id);
	if (syschar == NULL) {
		syschar = oval_syschar_new(object);
		oval_syschar_model_add_syschar(model, syschar);
	}
	return syschar;
}

struct oval_sysdata *get_oval_sysdata_new(struct oval_syschar_model *model, char *id)
{
	struct oval_sysdata *sysdata = oval_syschar_model_get_sysdata(model, id);
	if (sysdata == NULL) {
		sysdata = oval_sysdata_new(id);
		oval_syschar_model_add_sysdata(model, sysdata);
	}
	return sysdata;
}

struct oval_definition *get_oval_definition_new(struct oval_definition_model *model, char *id)
{
	struct oval_definition *definition = oval_definition_model_get_definition(model, id);
	if (definition == NULL) {
		definition = oval_definition_new(id);
		oval_definition_model_add_definition(model, definition);
	}
	return definition;
}


struct oval_variable *get_oval_variable_new(struct oval_definition_model *model, char *id, oval_variable_type_t type)
{
	struct oval_variable *variable = oval_definition_model_get_variable(model, id);
	if (variable == NULL) {
		variable = oval_variable_new(id, type);
		oval_definition_model_add_variable(model, variable);
	}
	return variable;
}

struct oval_state *get_oval_state_new(struct oval_definition_model *model, char *id)
{
	struct oval_state *state = oval_definition_model_get_state(model, id);
	if (state == NULL) {
		state = oval_state_new(id);
		oval_definition_model_add_state(model, state);
	}
	return state;
}

struct oval_object *get_oval_object_new(struct oval_definition_model *model, char *id)
{
	struct oval_object *object = oval_definition_model_get_object(model, id);
	if (object == NULL) {
		object = oval_object_new(id);
		oval_definition_model_add_object(model, object);
	}
	return object;
}

struct oval_test *get_oval_test_new(struct oval_definition_model *model, char *id)
{
	struct oval_test *test = oval_definition_model_get_test(model, id);
	if (test == NULL) {
		test = oval_test_new(id);
		oval_definition_model_add_test(model, test);
	}
	return test;
}

static int _generator_to_dom(xmlDocPtr doc, xmlNode *tag_generator)
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
		(tag_generator, ns_common, BAD_CAST "timestamp", BAD_CAST timestamp);
	return 1;
}



struct oval_results_model{
	struct oval_definition_model  *definition_model;
	struct oval_collection        *systems;
};

typedef struct oval_results_model oval_results_model_t;

struct oval_results_model *oval_results_model_new
	(struct oval_definition_model *definition_model,
			struct oval_syschar_model **syschar_models)
{
	oval_results_model_t *model = (oval_results_model_t *)
		malloc(sizeof(oval_results_model_t));
	model->systems       = oval_collection_new();
	model->definition_model  = definition_model;
	if(syschar_models){
		struct oval_syschar_model *syschar_model;
		for(syschar_model = *syschar_models;syschar_model;
		    syschar_model = *(++syschar_models)){
			struct oval_result_system *sys = oval_result_system_new(syschar_model);
			oval_results_model_add_system(model, sys);
		}
	}
	return model;
}


struct oval_results_model *oval_results_model_clone(struct oval_results_model * old_resmodel)
{
	struct oval_definition_model *old_defmodel = oval_results_model_get_definition_model(old_resmodel);
	struct oval_results_model *new_resmodel = oval_results_model_new(old_defmodel, NULL);
	struct oval_result_system_iterator *old_systems = oval_results_model_get_systems(old_resmodel);
	while(oval_result_system_iterator_has_more(old_systems)){
		struct oval_result_system *old_system = oval_result_system_iterator_next(old_systems);
		oval_result_system_clone(old_system, new_resmodel);
	}
	oval_result_system_iterator_free(old_systems);

	return new_resmodel;
}


void oval_results_model_free(struct oval_results_model *model)
{
	oval_collection_free_items
	(model->systems, (oscap_destruct_func)oval_result_system_free);
	model->definition_model  = NULL;
	model->systems       = NULL;
	free(model);
}

struct oval_definition_model *oval_results_model_get_definition_model
	(struct oval_results_model *model)
{
	return model->definition_model;
}

struct oval_result_system_iterator *oval_results_model_get_systems
	(struct oval_results_model *model)
{
	return (struct oval_result_system_iterator *)
			oval_collection_iterator(model->systems);
}

void oval_results_model_add_system
	(struct oval_results_model *model, struct oval_result_system *sys)
{
	if(sys)oval_collection_add(model->systems, sys);
}

struct oval_result_directives *oval_results_model_import
	(struct oval_results_model *model, struct oval_import_source *source,
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

static void _oval_agent_scan_for_extensions_tests
	(struct oval_criteria_node *node,
	 struct oval_string_map *extmap,
	 struct oval_string_map *tstmap,
	 oval_definitions_resolver resolver, void *user_arg)
{
	oval_criteria_node_type_t type = oval_criteria_node_get_type(node);
	switch(type)
	{
	case OVAL_NODETYPE_CRITERIA:{
		struct oval_criteria_node_iterator *subnodes = oval_criteria_node_get_subnodes(node);
		while(oval_criteria_node_iterator_has_more(subnodes)){
			struct oval_criteria_node *subnode = oval_criteria_node_iterator_next(subnodes);
			_oval_agent_scan_for_extensions_tests(subnode, extmap, tstmap, resolver, user_arg);
		}
		oval_criteria_node_iterator_free(subnodes);
	};break;
	case OVAL_NODETYPE_CRITERION:{
		struct oval_test *test = oval_criteria_node_get_test(node);
		char *tstid = oval_test_get_id(test);
		void *value = oval_string_map_get_value(tstmap,tstid);
		if(value==NULL){
			oval_string_map_put(tstmap, tstid, test);
		}
	};break;
	case OVAL_NODETYPE_EXTENDDEF:{
		struct oval_definition *extends = oval_criteria_node_get_definition(node);
		if(resolver && !(*resolver)(extends, user_arg)){
			char *extid = oval_definition_get_id(extends);
			void *value = oval_string_map_get_value(extmap, extid);
			if(value==NULL){
				oval_string_map_put(extmap, extid, extends);
			}
		}
	};break;
	default: break;
	}
}

static void _oval_agent_scan_entity_for_references
	(struct oval_entity *entity,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap);

static void _oval_agent_scan_set_for_references
	(struct oval_setobject *set,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap);

static void _oval_agent_scan_object_for_references
	(struct oval_object *object,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap)
{
	struct oval_object_content_iterator *contents = oval_object_get_object_contents(object);
	while(oval_object_content_iterator_has_more(contents)){
		struct oval_object_content *content = oval_object_content_iterator_next(contents);
		struct oval_entity *entity = oval_object_content_get_entity(content);
		if(entity)_oval_agent_scan_entity_for_references(entity, objmap, sttmap, varmap);
		struct oval_setobject *set = oval_object_content_get_setobject(content);
		if(set)_oval_agent_scan_set_for_references(set, objmap, sttmap, varmap);
	}
	oval_object_content_iterator_free(contents);
}
static void _oval_agent_scan_state_for_references
	(struct oval_state *state,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap)
{
	struct oval_state_content_iterator *contents = oval_state_get_contents(state);
	while(oval_state_content_iterator_has_more(contents)){
		struct oval_state_content *content = oval_state_content_iterator_next(contents);
		struct oval_entity *entity = oval_state_content_get_entity(content);
		if(entity)_oval_agent_scan_entity_for_references(entity, objmap, sttmap, varmap);
	}
	oval_state_content_iterator_free(contents);
}

static void _oval_agent_scan_component_for_references
	(struct oval_component *component,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap)
{
	struct oval_variable *variable = oval_component_get_variable(component);
	if(variable){
		char *varid = oval_variable_get_id(variable);
		void *value = oval_string_map_get_value(varmap, varid);
		if(value==NULL){
			oval_string_map_put(varmap, varid, variable);
			struct oval_component *component2 = oval_variable_get_component(variable);
			if(component2){
				_oval_agent_scan_component_for_references(component2, objmap, sttmap, varmap);
			}
		}
	}else{
		struct oval_component_iterator *fcomponents = oval_component_get_function_components(component);
		if(fcomponents)while(oval_component_iterator_has_more(fcomponents)){
			struct oval_component *fcomponent = oval_component_iterator_next(fcomponents);
			_oval_agent_scan_component_for_references(fcomponent, objmap, sttmap, varmap);
		}
		oval_component_iterator_free(fcomponents);

		struct oval_object *object = oval_component_get_object(component);
		if(object){
			char *objid = oval_object_get_id(object);
			void *value = oval_string_map_get_value(objmap, objid);
			if(value==NULL){
				oval_string_map_put(objmap, objid, object);
				_oval_agent_scan_object_for_references(object, objmap, sttmap, varmap);
			}
		}
	}
}

static void _oval_agent_scan_entity_for_references
	(struct oval_entity *entity,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap)
{
	struct oval_variable *variable = oval_entity_get_variable(entity);
	if(variable){
		char *varid = oval_variable_get_id(variable);
		void *value = oval_string_map_get_value(varmap, varid);
		if(value==NULL){
			oval_string_map_put(varmap, varid, variable);
			struct oval_component *component = oval_variable_get_component(variable);
			if(component){
				_oval_agent_scan_component_for_references(component, objmap, sttmap, varmap);
			}
		}
	}
}

static void _oval_agent_scan_set_for_references
	(struct oval_setobject *set,
	 struct oval_string_map *objmap,
	 struct oval_string_map *sttmap,
	 struct oval_string_map *varmap)
{
	struct oval_object_iterator *objects = oval_setobject_get_objects(set);
	if(objects)while(oval_object_iterator_has_more(objects)){
		struct oval_object *object = oval_object_iterator_next(objects);
		char *objid = oval_object_get_id(object);
		void *value = oval_string_map_get_value(objmap, objid);
		if(value==NULL){
			oval_string_map_put(objmap, objid, object);
			_oval_agent_scan_object_for_references(object, objmap, sttmap, varmap);
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
			_oval_agent_scan_state_for_references(state, objmap, sttmap, varmap);
		}
	}
	oval_state_iterator_free(states);
	struct oval_setobject_iterator *subsets = oval_setobject_get_subsets(set);
	if(subsets)while(oval_setobject_iterator_has_more(subsets)){
		struct oval_setobject *subset = oval_setobject_iterator_next(subsets);
		_oval_agent_scan_set_for_references(subset, objmap, sttmap, varmap);
	}
	oval_setobject_iterator_free(subsets);
}

xmlNode *oval_definitions_to_dom
	(struct oval_definition_model *definition_model, xmlDocPtr doc, xmlNode *parent,
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
    struct oval_definition_iterator *definitions = oval_definition_model_get_definitions(definition_model);
    if(oval_definition_iterator_has_more(definitions)){
        struct oval_string_map *extmap = oval_string_map_new();
    	xmlNode *definitions_node = NULL;
    	int i;for(i=0;oval_definition_iterator_has_more(definitions); i++){
    		struct oval_definition *definition = oval_definition_iterator_next(definitions);
			if(resolver==NULL || (*resolver)(definition, user_arg)){
				if(definitions_node==NULL)definitions_node
					= xmlNewChild(root_node, ns_defntns, BAD_CAST "definitions", NULL);
				oval_definition_to_dom
					(definition, doc, definitions_node);
				struct oval_criteria_node *criteria = oval_definition_get_criteria(definition);
				if(criteria)_oval_agent_scan_for_extensions_tests(criteria, extmap, tstmap, resolver, user_arg);
			}
    	}
    	oval_definition_iterator_free(definitions);
        definitions = (struct oval_definition_iterator *)oval_string_map_values(extmap);
    	while(oval_definition_iterator_has_more(definitions)){
    		struct oval_definition *definition = oval_definition_iterator_next(definitions);
    		oval_definition_to_dom
    			(definition, doc, definitions_node);
    	}
        oval_string_map_free(extmap, NULL);
    	oval_definition_iterator_free(definitions);
    }

    struct oval_string_map *objmap = oval_string_map_new();
    struct oval_string_map *sttmap = oval_string_map_new();
    struct oval_string_map *varmap = oval_string_map_new();

    struct oval_test_iterator *tests = (struct oval_test_iterator *)oval_string_map_values(tstmap);
    if(oval_test_iterator_has_more(tests)){
    	xmlNode *tests_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "tests", NULL);
    	while(oval_test_iterator_has_more(tests)){
    		struct oval_test *test = oval_test_iterator_next(tests);
    		oval_test_to_dom(test, doc, tests_node);
    		struct oval_object *object = oval_test_get_object(test);
    		if(object){
    			char *object_id = oval_object_get_id(object);
    			void *value = oval_string_map_get_value(objmap, object_id);
    			if(value==NULL){
        			oval_string_map_put(objmap, object_id, object);
        			_oval_agent_scan_object_for_references(object, objmap, sttmap, varmap);
    			}
    		}
    		struct oval_state *state = oval_test_get_state(test);
    		if(state){
    			char *state_id = oval_state_get_id(state);
    			void *value = oval_string_map_get_value(sttmap, state_id);
    			if(value==NULL){
        			oval_string_map_put(sttmap, state_id, state);
        			_oval_agent_scan_state_for_references(state, objmap, sttmap, varmap);
    			}
    		}
    	}
    }
    oval_test_iterator_free(tests);

    struct oval_object_iterator *objects = (struct oval_object_iterator *)oval_string_map_values(objmap);
    if(oval_object_iterator_has_more(objects)){
    	xmlNode *objects_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "objects", NULL);
    	int i;for(i=0;oval_object_iterator_has_more(objects); i++){
    		struct oval_object *object = oval_object_iterator_next(objects);
    		oval_object_to_dom(object, doc, objects_node);
    	}
    }
    oval_object_iterator_free(objects);
    struct oval_state_iterator *states = (struct oval_state_iterator *)oval_string_map_values(sttmap);
    if(oval_state_iterator_has_more(states)){
    	xmlNode *states_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "states", NULL);
    	while(oval_state_iterator_has_more(states)){
    		struct oval_state *state = oval_state_iterator_next(states);
    		oval_state_to_dom(state, doc, states_node);
    	}
    }
    oval_state_iterator_free(states);
    struct oval_variable_iterator *variables = (struct oval_variable_iterator *)oval_string_map_values(varmap);
    if(oval_variable_iterator_has_more(variables)){
    	xmlNode *variables_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "variables", NULL);
    	while(oval_variable_iterator_has_more(variables)){
    		struct oval_variable *variable = oval_variable_iterator_next(variables);
    		oval_variable_to_dom(variable, doc, variables_node);
    	}
    }
    oval_variable_iterator_free(variables);

    oval_string_map_free(objmap, NULL);
    oval_string_map_free(sttmap, NULL);
    oval_string_map_free(varmap, NULL);
    oval_string_map_free(tstmap, NULL);

    return root_node;
}

int oval_definition_model_export(
		struct oval_definition_model *model, struct oval_export_target *target){

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

xmlNode *oval_syschar_model_to_dom
	(struct oval_syschar_model *syschar_model, xmlDocPtr doc, xmlNode *parent,
			oval_syschar_resolver resolver, void *user_arg)
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

	oval_sysinfo_to_dom(oval_syschar_model_get_sysinfo(syschar_model), doc, root_node);

	struct oval_syschar_iterator *syschars = oval_syschar_model_get_syschars(syschar_model);
	if(resolver){
		struct oval_collection *collection = oval_collection_new();
		while(oval_syschar_iterator_has_more(syschars)){
			struct oval_syschar *syschar = oval_syschar_iterator_next(syschars);
			if((*resolver)(syschar, user_arg)){
				oval_collection_add(collection, syschar);
			}
		}
		oval_syschar_iterator_free(syschars);
		syschars = (struct oval_syschar_iterator *)oval_collection_iterator(collection);
	}

	struct oval_string_map *sysdata_map = oval_string_map_new();
	if(oval_syschar_iterator_has_more(syschars)){
		xmlNode *tag_objects = xmlNewChild
			(root_node, ns_syschar, BAD_CAST "collected_objects",NULL);

		while(oval_syschar_iterator_has_more(syschars)){
			struct oval_syschar *syschar =oval_syschar_iterator_next(syschars);
			oval_syschar_to_dom(syschar, doc, tag_objects);
			struct oval_sysdata_iterator *sysdatas = oval_syschar_get_sysdata(syschar);
			while(oval_sysdata_iterator_has_more(sysdatas)){
				struct oval_sysdata *sysdata = oval_sysdata_iterator_next(sysdatas);
				oval_string_map_put(sysdata_map, oval_sysdata_get_id(sysdata), sysdata);
			}
			oval_sysdata_iterator_free(sysdatas);
		}
	}
	oval_syschar_iterator_free(syschars);

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
	oval_collection_iterator_free(sysdatas);
	oval_string_map_free(sysdata_map, NULL);

	return root_node;
}

int oval_syschar_model_export(
		struct oval_syschar_model *model, struct oval_export_target *target)
{

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	oval_syschar_model_to_dom(model, doc, NULL, NULL, NULL);
	/*
	 * Dumping document to stdio or file
	 */
	int retcode = xmlSaveFormatFileEnc(target->filename, doc, target->encoding, 1);

	xmlFreeDoc(doc);
    return retcode;
}

static void _scan_for_viewable_definitions
	(struct oval_results_model *results_model, struct oval_result_directives *directives, struct oval_string_map *defids)
{
	struct oval_result_system_iterator *systems = oval_results_model_get_systems(results_model);
	while(oval_result_system_iterator_has_more(systems)){
		struct oval_result_system *sys = oval_result_system_iterator_next(systems);
		struct oval_result_definition_iterator *rslt_definitions = oval_result_system_get_definitions(sys);
		int i;for(i=0;oval_result_definition_iterator_has_more(rslt_definitions); i++){
			struct oval_result_definition *rslt_definition = oval_result_definition_iterator_next(rslt_definitions);
			oval_result_t result = oval_result_definition_get_result(rslt_definition);
			if(oval_result_directives_get_reported(directives, result)){
				struct oval_definition *oval_definition = oval_result_definition_get_definition(rslt_definition);
				if(oval_definition){
					char *defid = oval_definition_get_id(oval_definition);
					oval_string_map_put(defids, defid, oval_definition);
				}
			}
		}
		oval_result_definition_iterator_free(rslt_definitions);
	}
	oval_result_system_iterator_free(systems);
}

static bool _resolve_oval_definition_from_map
	(struct oval_definition *oval_definition, struct oval_string_map *defids)
{
	char *defid = oval_definition_get_id(oval_definition);
	return oval_string_map_get_value(defids, defid)!=NULL;
}

static xmlNode *oval_results_to_dom
	(struct oval_results_model *results_model, struct oval_result_directives *directives,
			xmlDocPtr doc, xmlNode *parent)
{
	xmlNode *root_node;
	if(parent){
		root_node = xmlNewChild(parent, NULL, BAD_CAST "oval_results", NULL);
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

	struct oval_definition_model *definition_model = oval_results_model_get_definition_model(results_model);
	oval_definitions_to_dom
		(definition_model, doc, root_node,
				(oval_definitions_resolver *)_resolve_oval_definition_from_map, defids);

	xmlNode *results_node = xmlNewChild(root_node, ns_results, BAD_CAST "results", NULL);

	struct oval_result_system_iterator *systems = oval_results_model_get_systems(results_model);
	while(oval_result_system_iterator_has_more(systems)){
		struct oval_result_system *sys
			= oval_result_system_iterator_next(systems);
		oval_result_system_to_dom
			(sys, results_model, directives,
				doc, results_node);
	}
	oval_result_system_iterator_free(systems);

	return root_node;
}

int oval_results_model_export
	(struct oval_results_model *results_model, struct oval_result_directives *directives,
			struct oval_export_target *target)
{
	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");

	oval_results_to_dom(results_model, directives, doc, NULL);
	int xmlCode = xmlSaveFormatFileEnc
		(target->filename, doc, target->encoding, 1);
	if(xmlCode<=0){
		fprintf(stderr, "WARNING: No bytes exported: xmlCode = %d\n", xmlCode);
	}


	xmlFreeDoc(doc);

	return 1;
}

#if 0
#ifdef __STUB_PROBE
//STUB for oval_object_probe
static int item_id = 1;
struct oval_syschar *oval_probe_object_eval (struct oval_object *object, struct oval_definition_model *model)
{
	struct oval_syschar *syschar = oval_syschar_new(object);
	oval_syschar_set_flag(syschar, SYSCHAR_STATUS_NOT_COLLECTED);
	char itemid[10];
	snprintf(itemid, sizeof(item_id), "%d", (++item_id));
	struct oval_sysdata *sysdata = oval_sysdata_new(itemid);
	oval_sysdata_set_status(sysdata, SYSCHAR_STATUS_NOT_COLLECTED);
	oval_sysdata_set_subtype(sysdata, oval_object_get_subtype(object));
	oval_syschar_add_sysdata(syschar, sysdata);
	return syschar;
}

struct oval_sysinfo *oval_probe_sysinf_eval (void)
{
        return (NULL);
}
#endif
#endif /* 0 */
