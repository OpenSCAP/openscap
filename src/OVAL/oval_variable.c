/*
 * oval_variable.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_variable_s{
	char* id                               ;
	Oval_variable_type_enum type           ;
	Oval_datatype_enum datatype            ;
} Oval_variable_t;

typedef struct Oval_variable_CONSTANT_s{
	char* Id                               ;
	Oval_variable_type_enum type           ;
	Oval_datatype_enum datatype            ;
	Oval_value*  values                    ;//type==OVAL_VARIABLE_CONSTANT
} Oval_variable_CONSTANT_t;

typedef struct Oval_variable_LOCAL_s{
	char* Id                               ;
	Oval_variable_type_enum type           ;
	Oval_datatype_enum datatype            ;
	Oval_component component               ;//type==OVAL_VARIABLE_LOCAL
} Oval_variable_LOCAL_t;

typedef Oval_variable_t* Oval_variable_ptr;
typedef Oval_variable_CONSTANT_t* Oval_variable_CONSTANT_ptr;
typedef Oval_variable_LOCAL_t* Oval_variable_LOCAL_ptr;

OvalCollection_variable newOvalCollection_variable(Oval_variable* variable_array){
	return (OvalCollection_variable)newOvalCollection((OvalCollection_target*)variable_array);
}
int   OvalCollection_variable_hasMore      (OvalCollection_variable oc_variable){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_variable);
}
Oval_variable OvalCollection_variable_next         (OvalCollection_variable oc_variable){
	return (Oval_variable)OvalCollection_next((OvalCollection_ptr)oc_variable);
}

char* Oval_variable_Id                               (Oval_variable variable){
	return ((Oval_variable_ptr)variable)->id;
}
Oval_variable_type_enum Oval_variable_type           (Oval_variable variable){
	return ((Oval_variable_ptr)variable)->type;
}
Oval_datatype_enum Oval_variable_datatype            (Oval_variable variable){
	return ((Oval_variable_ptr)variable)->datatype;
}
OvalCollection_value Oval_variable_values            (Oval_variable variable){
	//type==OVAL_VARIABLE_CONSTANT
	OvalCollection_value oc_value = NULL;
	if(Oval_variable_type(variable)==OVAL_VARIABLE_CONSTANT){
		Oval_value* values = ((Oval_variable_CONSTANT_ptr)variable)->values;
		oc_value = newOvalCollection_value(values);
	}
	return oc_value;
}
Oval_component Oval_variable_component               (Oval_variable variable){
	//type==OVAL_VARIABLE_LOCAL
	Oval_component component = NULL;
	if(Oval_variable_type(variable)==OVAL_VARIABLE_LOCAL){
		component = ((Oval_variable_LOCAL_ptr)variable)->component;
	}
	return component;
}
