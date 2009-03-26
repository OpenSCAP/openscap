/*
 * oval_agent.c
 *
 *  Created on: Mar 9, 2009
 *      Author: david.niemoller
 */
#include "includes/oval_definitions_impl.h"
#include "includes/oval_agent_api_impl.h"
#include "includes/oval_parser_impl.h"
#include "includes/oval_string_map_impl.h"

	struct export_target_s{
		char* export_target_filename;
	};
	struct import_source_s{
		char* import_source_filename;
	};

	typedef struct oval_object_model_s{
		struct oval_string_map_s *definition_map;
		struct oval_string_map_s *test_map;
		struct oval_string_map_s *object_map;
		struct oval_string_map_s *state_map;
		struct oval_string_map_s *variable_map;
	}oval_object_model_t;

	struct oval_object_model_s *oval_object_model_new(){
		oval_object_model_t *newmodel = (oval_object_model_t*)malloc(sizeof(oval_object_model_t));
		newmodel->definition_map = oval_string_map_new();
		newmodel->object_map     = oval_string_map_new();
		newmodel->state_map      = oval_string_map_new();
		newmodel->test_map       = oval_string_map_new();
		newmodel->variable_map   = oval_string_map_new();
		return newmodel;
	}

	void add_oval_definition(struct oval_object_model_s *model, struct oval_definition_s *definition){
		char* key = oval_definition_id(definition);
		oval_string_map_put(model->definition_map, key, definition);
	}
	void add_oval_test(struct oval_object_model_s *model, struct oval_test_s *test){
		char* key = oval_test_id(test);
		oval_string_map_put(model->test_map, key, test);
	}
	void add_oval_object(struct oval_object_model_s *model, struct oval_object_s *object){
		char* key = oval_object_id(object);
		oval_string_map_put(model->object_map, key, object);
	}
	void add_oval_state(struct oval_object_model_s *model, struct oval_state_s *state     ){
		char* key = oval_state_id(state);
		oval_string_map_put(model->state_map, key, state);
	}
	void add_oval_variable(struct oval_object_model_s *model, struct oval_variable_s *variable){
		char* key = oval_variable_id(variable);
		oval_string_map_put(model->variable_map, key, variable);
	}

	void load_oval_definitions(struct oval_object_model_s *model, struct import_source_s *source, oval_xml_error_handler eh, void* user_arg){
		oval_parser_parse(model, source->import_source_filename, eh, user_arg);
	}

	struct oval_definition_s *get_oval_definition(struct oval_object_model_s *model, char *key){
		return (struct oval_definition_s*)oval_string_map_get_value(model->definition_map,key);
	}
	struct oval_test_s *get_oval_test(struct oval_object_model_s *model, char *key){
		return (struct oval_test_s*)oval_string_map_get_value(model->test_map,key);
	}
	struct oval_object_s *get_oval_object(struct oval_object_model_s *model, char *key){
		return (struct oval_object_s*)oval_string_map_get_value(model->object_map,key);
	}
	struct oval_state_s *get_oval_state(struct oval_object_model_s *model, char *key){
		return (struct oval_state_s*)oval_string_map_get_value(model->state_map,key);
	}
	struct oval_variable_s *get_oval_variable(struct oval_object_model_s *model, char *key){
		return (struct oval_variable_s*)oval_string_map_get_value(model->variable_map,key);
	}

	struct oval_iterator_definition_s *get_oval_definitions(struct oval_object_model_s *model){
		return (struct oval_iterator_definition_s*)oval_string_map_values(model->definition_map);
	}
	struct oval_iterator_test_s *get_oval_tests(struct oval_object_model_s *model){
		return (struct oval_iterator_test_s*)oval_string_map_values(model->test_map);
	}
	struct oval_iterator_object_s *get_oval_objects(struct oval_object_model_s *model){
		return (struct oval_iterator_object_s*)oval_string_map_values(model->object_map);
	}
	struct oval_iterator_state_s *get_oval_states(struct oval_object_model_s *model){
		return (struct oval_iterator_state_s*)oval_string_map_values(model->state_map);
	}
	struct oval_iterator_variable_s *get_oval_variables(struct oval_object_model_s *model){
		return (struct oval_iterator_variable_s*)oval_string_map_values(model->variable_map);
	}

	struct oval_definition_s *get_oval_definition_new(struct oval_object_model_s* model,char* id){
		struct oval_definition_s *definition = get_oval_definition(model,id);
		if(definition==NULL){
			definition = oval_definition_new();
			set_oval_definition_id(definition,id);
			add_oval_definition(model,definition);
		}
		return definition;
	}

	struct oval_variable_s *get_oval_variable_new(struct oval_object_model_s* model,char* id){
		struct oval_variable_s *variable = get_oval_variable(model,id);
		if(variable==NULL){
			variable = oval_variable_new();
			set_oval_variable_id(variable,id);
			add_oval_variable(model,variable);
		}
		return variable;
	}

	struct oval_state_s *get_oval_state_new(struct oval_object_model_s* model,char* id){
		struct oval_state_s *state = get_oval_state(model,id);
		if(state==NULL){
			state = oval_state_new();
			set_oval_state_id(state,id);
			add_oval_state(model,state);
		}
		return state;
	}

	struct oval_object_s *get_oval_object_new(struct oval_object_model_s* model,char* id){
		struct oval_object_s *object = get_oval_object(model,id);
		if(object==NULL){
			object = oval_object_new();
			set_oval_object_id(object,id);
			add_oval_object(model,object);
		}
		return object;
	}

	struct oval_test_s *get_oval_test_new(struct oval_object_model_s* model,char* id){
		struct oval_test_s *test = get_oval_test(model,id);
		if(test==NULL){
			test = oval_test_new();
			set_oval_test_id(test,id);
			add_oval_test(model,test);
		}
		return test;
	}


	/* needs oval_characteristics, export_target_s struct definitions */
	int export_characteristics(struct oval_iterator_syschar_s*,struct export_target_s*);

	/* needs oval_results, export_target_s struct definitions */
	int export_results(struct oval_result_s*,struct export_target_s*);

	struct oval_iterator_syschar_s *probe_object  (struct oval_object_s *   ,                                  struct oval_iterator_variable_binding_s*);
	struct oval_result_test_s *resolve_test       (struct oval_test_s *     , struct oval_iterator_syschar_s*, struct oval_iterator_variable_binding_s*);
	struct oval_result_s *resolve_definition      (struct oval_definition_s*, struct oval_iterator_syschar_s*, struct oval_iterator_variable_binding_s*);
