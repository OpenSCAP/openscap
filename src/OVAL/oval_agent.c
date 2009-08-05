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
#include <string.h>

typedef struct export_target {
	char *export_target_filename;
} export_target_t;

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

struct oval_object_model *oval_syschar_model_object_model(
		struct oval_syschar_model *model){
	return model->object_model;
}

struct oval_iterator_syschar *oval_syschar_model_syschars(
		struct oval_syschar_model *model){
	return (struct oval_iterator_syschar *)oval_string_map_values(model->syschar_map);
}

void add_oval_definition(struct oval_object_model *model,
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

struct oval_syschar *get_oval_syschar_new(struct oval_syschar_model *model, struct oval_object *object)
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


struct oval_variable *get_oval_variable_new(struct oval_object_model *model, char *id)
{
	struct oval_variable *variable = get_oval_variable(model, id);
	if (variable == NULL) {
		variable = oval_variable_new(id);
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

	/* needs oval_characteristics, export_target  struct definitions */
int export_characteristics(struct oval_iterator_syschar *,
			   struct export_target *);

	/* needs oval_results, export_target  struct definitions */
int export_results(struct oval_result *, struct export_target *);

struct oval_iterator_syschar *probe_object(struct oval_object *,
					   struct oval_iterator_variable_binding
					   *);
struct oval_result_test *resolve_test(struct oval_test *,
				      struct oval_iterator_syschar *,
				      struct oval_iterator_variable_binding *);
struct oval_result *resolve_definition(struct oval_definition *,
				       struct oval_iterator_syschar *,
				       struct oval_iterator_variable_binding *);
