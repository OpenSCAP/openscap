/*
 * oval_result.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_results_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_result_s{
	Oval_definition definition      ;
	Oval_result_enum result         ;
	char* message                   ;
	Oval_resultDirectives directives;
	Oval_resultCriteriaNode criteria;
} Oval_result_t;

typedef Oval_result_t* Oval_result_ptr;

OvalCollection_result newOvalCollection_result(Oval_result* result_array){
	return (OvalCollection_result)newOvalCollection((OvalCollection_target*)result_array);
}
int   OvalCollection_result_hasMore      (OvalCollection_result oc_result){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_result);
}
Oval_result OvalCollection_result_next         (OvalCollection_result oc_result){
	return (Oval_result)OvalCollection_next((OvalCollection_ptr)oc_result);
}

Oval_definition Oval_result_definition      (Oval_result result){
	return ((Oval_result_ptr)result)->definition;
}
Oval_result_enum Oval_result_result         (Oval_result result){
	return ((Oval_result_ptr)result)->result;
}
char* Oval_result_message                   (Oval_result result){
	return ((Oval_result_ptr)result)->message;
}
Oval_resultDirectives Oval_result_directives(Oval_result result){
	return ((Oval_result_ptr)result)->directives;
}
Oval_resultCriteriaNode Oval_result_criteria(Oval_result result){
	return ((Oval_result_ptr)result)->criteria;
}
