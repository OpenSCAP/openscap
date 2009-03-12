/*
 * oval_definition.c
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_definition_s{
	char* id                          ;
	int version                       ;
	definition_class_enum class       ;
	int deprecated                    ;
	char* title                       ;
	char* description                 ;
	Oval_affected* affected           ;
	Oval_reference* reference         ;
	Oval_criteriaNode criteria        ;
} Oval_definition_t;

typedef Oval_definition_t* Oval_definition_ptr;


OvalCollection_definition newOvalCollection_definition(Oval_definition* definition_array){
	return (OvalCollection_definition)newOvalCollection((OvalCollection_target*)definition_array);
}
int   OvalCollection_definition_hasMore      (OvalCollection_definition oc_definition){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_definition);
}
Oval_definition OvalCollection_definition_next         (OvalCollection_definition oc_definition){
	return (Oval_definition)OvalCollection_next((OvalCollection_ptr)oc_definition);
}

char* Oval_definition_id                          (Oval_definition definition){
	return ((Oval_definition_ptr)definition)->id;
}
int Oval_definition_version                       (Oval_definition definition){
	return ((Oval_definition_ptr)definition)->version;
}
Oval_definition_class_enum Oval_definition_class  (Oval_definition definition){
	return ((Oval_definition_ptr)definition)->class;
}
int Oval_definition_deprecated                    (Oval_definition definition){
	return ((Oval_definition_ptr)definition)->deprecated;
}
char* Oval_definition_title                       (Oval_definition definition){
	return ((Oval_definition_ptr)definition)->title;
}
char* Oval_definition_description                 (Oval_definition definition){
	return ((Oval_definition_ptr)definition)->description;
}
OvalCollection_affected Oval_definition_affected  (Oval_definition definition){
	return newOvalCollection_affected(((Oval_definition_ptr)definition)->affected);
}
OvalCollection_reference Oval_definition_reference(Oval_definition definition){
	return newOvalCollection_reference(((Oval_definition_ptr)definition)->reference);
}
Oval_criteriaNode Oval_definition_criteria        (Oval_definition definition){
	return ((Oval_definition_ptr)definition)->criteria;
}
