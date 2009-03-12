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

typedef struct Oval_component_s{
	Oval_component_type_enum type               ;
} Oval_component_t;

typedef struct Oval_component_LITERAL_s{
	Oval_component_type_enum type               ;
	Oval_value literalValue                     ;//type==OVAL_COMPONENT_LITERAL
} Oval_component_LITERAL_t;

typedef struct Oval_component_OBJECTREF_s{
	Oval_component_type_enum type               ;
	Oval_object object                          ;//type==OVAL_COMPONENT_OBJECTREF
	char* object_field                          ;//type==OVAL_COMPONENT_OBJECTREF
} Oval_component_OBJECTREF_t;

typedef struct Oval_component_VARREF_s{
	Oval_component_type_enum type               ;
	Oval_variable variable                      ;//type==OVAL_COMPONENT_VARREF
} Oval_component_VARREF_t;

typedef struct Oval_component_FUNCTION_s{
	Oval_component_type_enum type               ;
	Oval_function_type_enum function_type       ;//type==OVAL_COMPONENT_FUNCTION
	Oval_component* function_components         ;//type==OVAL_COMPONENT_FUNCTION
	char** function_keys                        ;//type==OVAL_COMPONENT_FUNCTION
	Oval_value function_value                   ;//type==OVAL_COMPONENT_FUNCTION
} Oval_component_FUNCTION_t;

typedef Oval_component_t* Oval_component_ptr;
typedef Oval_component_LITERAL_t* Oval_component_LITERAL_ptr;
typedef Oval_component_OBJECTREF_t* Oval_component_OBJECTREF_ptr;
typedef Oval_component_VARREF_t* Oval_component_VARREF_ptr;
typedef Oval_component_FUNCTION_t* Oval_component_FUNCTION_ptr;

OvalCollection_component newOvalCollection_component(Oval_component* component_array){
	return (OvalCollection_component)newOvalCollection((OvalCollection_target*)component_array);
}
int   OvalCollection_component_hasMore      (OvalCollection_component oc_component){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_component);
}
Oval_component OvalCollection_component_next         (OvalCollection_component oc_component){
	return (Oval_component)OvalCollection_next((OvalCollection_ptr)oc_component);
}

Oval_component_type_enum Oval_component_type               (Oval_component component){
	return ((Oval_component_ptr)component)->type;
}
Oval_value Oval_component_literalValue                     (Oval_component component){
	//type==OVAL_COMPONENT_LITERAL
	Oval_value literalValue = NULL;
	if(Oval_component_type(component)==OVAL_COMPONENT_LITERAL){
		literalValue = ((Oval_component_LITERAL_ptr)component)->literalValue;
	}
	return literalValue;
}
Oval_object Oval_component_object                          (Oval_component component){
	//type==OVAL_COMPONENT_OBJECTREF
	Oval_object object = NULL;
	if(Oval_component_type(component)==OVAL_COMPONENT_OBJECTREF){
		object = ((Oval_component_OBJECTREF_ptr)component)->object;
	}
	return object;
}
char* Oval_component_object_field                          (Oval_component component){
	//type==OVAL_COMPONENT_OBJECTREF
	char* field = NULL;
	if(Oval_component_type(component)==OVAL_COMPONENT_OBJECTREF){
		field = ((Oval_component_OBJECTREF_ptr)component)->object_field;
	}
	return field;
}
Oval_variable Oval_component_variable                      (Oval_component component){
	//type==OVAL_COMPONENT_VARREF
	Oval_variable variable = NULL;
	if(Oval_component_type(component)==OVAL_COMPONENT_VARREF){
		variable = ((Oval_component_VARREF_ptr)component)->variable;
	}
	return variable;
}
Oval_function_type_enum Oval_component_function_type       (Oval_component component){
	//type==OVAL_COMPONENT_FUNCTION
	Oval_function_type_enum function_type = OVAL_FUNCTION_UNKNOWN;
	if(Oval_component_type(component)==OVAL_COMPONENT_FUNCTION){
		function_type = ((Oval_component_FUNCTION_ptr)component)->function_type;
	}
	return function_type;
}
OvalCollection_component Oval_component_function_components(Oval_component component){
	//type==OVAL_COMPONENT_FUNCTION
	OvalCollection_component oc_component = NULL;
	if(Oval_component_type(component)==OVAL_COMPONENT_FUNCTION){
		Oval_component* components = ((Oval_component_FUNCTION_ptr)component)->function_components;
		oc_component = newOvalCollection_component(components);
	}
	return oc_component;
}
OvalCollection_string Oval_component_function_keys         (Oval_component component){
	//type==OVAL_COMPONENT_FUNCTION
	OvalCollection_string oc_key = NULL;
	if(Oval_component_type(component)==OVAL_COMPONENT_FUNCTION){
		char** keys = ((Oval_component_FUNCTION_ptr)component)->function_keys;
		oc_key = newOvalCollection_string(keys);
	}
	return oc_key;
}
Oval_value Oval_component_function_value                   (Oval_component component, char* key){
	//type==OVAL_COMPONENT_FUNCTION
	Oval_value value = NULL;
	if(Oval_component_type(component)==OVAL_COMPONENT_FUNCTION){
		//TODO
	}
	return value;
}
