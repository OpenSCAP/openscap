/*
 * oval_behavior.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_test_s{
	Oval_value value                    ;
	char** attributeKeys                ;
	Oval_value* att_values              ;
} Oval_behavior_t;

typedef Oval_behavior_t* Oval_behavior_ptr;

OvalCollection_behavior newOvalCollection_behavior(Oval_behavior* behavior_array){
	return (OvalCollection_behavior)newOvalCollection((OvalCollection_target*)behavior_array);
}
int   OvalCollection_behavior_hasMore      (OvalCollection_behavior oc_behavior){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_behavior);
}
Oval_behavior OvalCollection_behavior_next         (OvalCollection_behavior oc_behavior){
	return (Oval_behavior)OvalCollection_next((OvalCollection_ptr)oc_behavior);
}

Oval_value Oval_behavior_value (Oval_behavior behavior){
	return ((Oval_behavior_ptr)behavior)->value;
}
OvalCollection_string Oval_behavior_attributeKeys (Oval_behavior behavior){
	char** attributeKeys = ((Oval_behavior_ptr)behavior)->attributeKeys;
	return newOvalCollection_string(attributeKeys);
}
Oval_value Oval_behavior_value_forKey(Oval_behavior behavior, char* attributeKey){
	return NULL;//TODO
}
