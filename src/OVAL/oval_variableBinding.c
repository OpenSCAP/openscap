/*
 * oval_variableBinding.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_variableBinding_s{
	Oval_variable variable ;
	char* value            ;
} Oval_variableBinding_t;

typedef Oval_variableBinding_t* Oval_variableBinding_ptr;

OvalCollection_variableBinding newOvalCollection_variableBinding(Oval_variableBinding* variableBinding_array){
	return (OvalCollection_variableBinding)newOvalCollection((OvalCollection_target*)variableBinding_array);
}
int   OvalCollection_variableBinding_hasMore      (OvalCollection_variableBinding oc_variableBinding){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_variableBinding);
}
Oval_variableBinding OvalCollection_variableBinding_next         (OvalCollection_variableBinding oc_variableBinding){
	return (Oval_variableBinding)OvalCollection_next((OvalCollection_ptr)oc_variableBinding);
}

Oval_variable Oval_variableBinding_variable (Oval_variableBinding binding){
	return ((Oval_variableBinding_ptr)binding)->variable;
}
char* Oval_variableBinding_value            (Oval_variableBinding binding){
	return ((Oval_variableBinding_ptr)binding)->value;
}
