/*
 * oval_resultTest.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_results_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_resultTest_s{
	Oval_test test                 ;
	Oval_check_enum check          ;
	Oval_result_enum result        ;
	char* message                  ;
	Oval_resultItem* items         ;
} Oval_resultTest_t;

typedef Oval_resultTest_t* Oval_resultTest_ptr;

OvalCollection_resultTest newOvalCollection_resultTest(Oval_resultTest* resultTest_array){
	return (OvalCollection_resultTest)newOvalCollection((OvalCollection_target*)resultTest_array);
}
int   OvalCollection_resultTest_hasMore      (OvalCollection_resultTest oc_resultTest){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_resultTest);
}
Oval_resultTest OvalCollection_resultTest_next         (OvalCollection_resultTest oc_resultTest){
	return (Oval_resultTest)OvalCollection_next((OvalCollection_ptr)oc_resultTest);
}

Oval_test Oval_resultTest_test                 (Oval_resultTest rtest){
	return ((Oval_resultTest_ptr)rtest)->test;
}
Oval_check_enum Oval_resultTest_check          (Oval_resultTest rtest){
	return ((Oval_resultTest_ptr)rtest)->check;
}
Oval_result_enum Oval_resultTest_result        (Oval_resultTest rtest){
	return ((Oval_resultTest_ptr)rtest)->result;
}
char* Oval_resultTest_message                  (Oval_resultTest rtest){
	return ((Oval_resultTest_ptr)rtest)->message;
}
OvalCollection_resultItem Oval_resultTest_items(Oval_resultTest rtest){
	Oval_resultItem* items = ((Oval_resultTest_ptr)rtest)->items;
	return newOvalCollection_resultItem(items);
}
