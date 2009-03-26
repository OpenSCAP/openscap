/*
 * oval_component.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"
#include "includes/oval_string_map_impl.h"

typedef struct oval_component_s{
	oval_component_type_enum type               ;
} oval_component_t;

typedef struct oval_component_LITERAL_s{
	oval_component_type_enum type               ;
	struct oval_value_s *literalValue                 ;//type==OVAL_COMPONENT_LITERAL
} oval_component_LITERAL_t;

typedef struct oval_component_OBJECTREF_s{
	oval_component_type_enum type               ;
	struct oval_object_s *object                      ;//type==OVAL_COMPONENT_OBJECTREF
	char* object_field                          ;//type==OVAL_COMPONENT_OBJECTREF
} oval_component_OBJECTREF_t;

typedef struct oval_component_VARREF_s{
	oval_component_type_enum type               ;
	struct oval_variable_s *variable                      ;//type==OVAL_COMPONENT_VARREF
} oval_component_VARREF_t;

typedef struct oval_component_FUNCTION_s{
	oval_component_type_enum type               ;
	oval_function_type_enum function_type       ;//type==OVAL_COMPONENT_FUNCTION
	struct oval_string_map_s *function_components     ;//type==OVAL_COMPONENT_FUNCTION
	struct oval_value_s *function_value               ;//type==OVAL_COMPONENT_FUNCTION
} oval_component_FUNCTION_t;

int   oval_iterator_component_has_more      (struct oval_iterator_component_s *oc_component){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_component);
}
struct oval_component_s *oval_iterator_component_next         (struct oval_iterator_component_s *oc_component){
	return (struct oval_component_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_component);
}

oval_component_type_enum oval_component_type               (struct oval_component_s *component){
	return ((struct oval_component_s*)component)->type;
}
struct oval_value_s *oval_component_literalValue                     (struct oval_component_s *component){
	//type==OVAL_COMPONENT_LITERAL
	struct oval_value_s *literalValue = NULL;
	if(oval_component_type(component)==OVAL_COMPONENT_LITERAL){
		literalValue = ((struct oval_component_LITERAL_s*)component)->literalValue;
	}
	return literalValue;
}
struct oval_object_s *oval_component_object                          (struct oval_component_s *component){
	//type==OVAL_COMPONENT_OBJECTREF
	struct oval_object_s *object = NULL;
	if(oval_component_type(component)==OVAL_COMPONENT_OBJECTREF){
		object = ((struct oval_component_OBJECTREF_s*)component)->object;
	}
	return object;
}
char* oval_component_object_field                          (struct oval_component_s *component){
	//type==OVAL_COMPONENT_OBJECTREF
	char* field = NULL;
	if(oval_component_type(component)==OVAL_COMPONENT_OBJECTREF){
		field = ((struct oval_component_OBJECTREF_s*)component)->object_field;
	}
	return field;
}
struct oval_variable_s *oval_component_variable                      (struct oval_component_s *component){
	//type==OVAL_COMPONENT_VARREF
	struct oval_variable_s *variable = NULL;
	if(oval_component_type(component)==OVAL_COMPONENT_VARREF){
		variable = ((struct oval_component_VARREF_s*)component)->variable;
	}
	return variable;
}
oval_function_type_enum oval_component_function_type       (struct oval_component_s *component){
	//type==OVAL_COMPONENT_FUNCTION
	oval_function_type_enum function_type = OVAL_FUNCTION_UNKNOWN;
	if(oval_component_type(component)==OVAL_COMPONENT_FUNCTION){
		function_type = ((struct oval_component_FUNCTION_s*)component)->function_type;
	}
	return function_type;
}

struct oval_iterator_component_s *oval_component_function_components(struct oval_component_s *component){
	//type==OVAL_COMPONENT_FUNCTION
	struct oval_component_FUNCTION_s *function = (struct oval_component_FUNCTION_s*)component;
	return (struct oval_iterator_component_s*)oval_string_map_values(function->function_components);
}
struct oval_iterator_string_s *oval_component_function_keys         (struct oval_component_s *component){
	//type==OVAL_COMPONENT_FUNCTION
	struct oval_component_FUNCTION_s *function = (struct oval_component_FUNCTION_s*)component;
	return (struct oval_iterator_string_s*)oval_string_map_keys(function->function_components);
}
struct oval_value_s *oval_component_function_value                   (struct oval_component_s *component, char* key){
	//type==OVAL_COMPONENT_FUNCTION
	struct oval_value_s *value = NULL;
	if(oval_component_type(component)==OVAL_COMPONENT_FUNCTION){
		//TODO
	}
	return value;
}

struct oval_component_s *oval_component_new(oval_component_type_enum type){
	oval_component_t *component;
	switch(type){
		case OVAL_COMPONENT_FUNCTION:{
			oval_component_FUNCTION_t *function
			= (oval_component_FUNCTION_t*)malloc(sizeof(oval_component_FUNCTION_t));
			component = (oval_component_t*)function;
			function->function_value      = NULL;
			function->function_components = oval_string_map_new();
			function->function_type       = OVAL_FUNCTION_UNKNOWN;
		}break;
		case OVAL_COMPONENT_LITERAL:{
			oval_component_LITERAL_t *literal
			= (oval_component_LITERAL_t*)malloc(sizeof(oval_component_LITERAL_t));
			component = (oval_component_t*)literal;
			literal->literalValue = NULL;
		}break;
		case OVAL_COMPONENT_OBJECTREF:{
			oval_component_OBJECTREF_t *objectref
			= (oval_component_OBJECTREF_t*)malloc(sizeof(oval_component_OBJECTREF_t));
			component = (oval_component_t*)objectref;
			objectref->object       = NULL;
			objectref->object_field = NULL;
		}break;
		case OVAL_COMPONENT_VARREF:{
			oval_component_VARREF_t *varref
			= (oval_component_VARREF_t*)malloc(sizeof(oval_component_VARREF_t));
			component = (oval_component_t*)varref;
			varref->variable = NULL;
		}break;
	}
	component->type = type;
	return component;
}
void oval_component_free(struct oval_component_s *component){
	switch(component->type){
		case OVAL_COMPONENT_FUNCTION:{
			oval_component_FUNCTION_t *function = (oval_component_FUNCTION_t*)component;
			if(function->function_value != NULL)oval_value_free(function->function_value);
			void free_subcomp(struct oval_collection_item_s *subcomp){oval_component_free(subcomp);}
			oval_string_map_free(function->function_components,&free_subcomp);
		}break;
		case OVAL_COMPONENT_LITERAL:{
			oval_component_LITERAL_t *literal = (oval_component_LITERAL_t*)component;
			if(literal->literalValue != NULL)oval_value_free(literal->literalValue);
		}break;
		case OVAL_COMPONENT_OBJECTREF:{
			oval_component_OBJECTREF_t *objectref = (oval_component_OBJECTREF_t*)component;
			if(objectref->object_field != NULL)free(objectref->object_field);
		}break;
	}
	free(component);
}

void set_oval_component_type               (struct oval_component_s*, oval_component_type_enum);//TODO
void set_oval_component_literal_value      (struct oval_component_s*, struct oval_value_s*);//TODO         //type==OVAL_COMPONENT_LITERAL
void set_oval_component_object             (struct oval_component_s*, struct oval_object_s*);//TODO        //type==OVAL_COMPONENT_OBJECTREF
void set_oval_component_object_field       (struct oval_component_s*, char*);//TODO                  //type==OVAL_COMPONENT_OBJECTREF
void set_oval_component_variable           (struct oval_component_s*, struct oval_variable_s*);//TODO      //type==OVAL_COMPONENT_VARREF
void set_oval_component_function_type      (struct oval_component_s*, oval_function_type_enum);//TODO//type==OVAL_COMPONENT_FUNCTION
void add_oval_component_function_components(struct oval_component_s*, struct oval_component_s*);//TODO     //type==OVAL_COMPONENT_FUNCTION
void set_oval_component_function_keys      (struct oval_component_s*, char*);//TODO                  //type==OVAL_COMPONENT_FUNCTION
