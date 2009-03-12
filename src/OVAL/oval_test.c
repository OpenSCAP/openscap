/*
 * oval_test.c
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_test_s{
	Oval_family_enum family      ;
	Oval_subtype_enum subtype    ;
	char* name                   ;
	char** notes                 ;
	char* comment                ;
	char* id                     ;
	int deprecated               ;
	int version                  ;
	Oval_operator_enum operator  ;
	Oval_existence_enum existence;
	Oval_check_enum check        ;
	Oval_object object           ;
	Oval_state state             ;
} Oval_test_t;

typedef Oval_test_t* Oval_test_ptr;


OvalCollection_test newOvalCollection_test(Oval_test* test_array){
	return (OvalCollection_test)newOvalCollection((OvalCollection_target*)test_array);
}
int   OvalCollection_test_hasMore      (OvalCollection_test oc_test){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_test);
}
Oval_test OvalCollection_test_next         (OvalCollection_test oc_test){
	return (Oval_test)OvalCollection_next((OvalCollection_ptr)oc_test);
}

Oval_family_enum Oval_test_family      (Oval_test test){
	return ((Oval_test_ptr)test)->family;
}
Oval_subtype_enum Oval_test_subtype    (Oval_test test){
	return ((Oval_test_ptr)test)->subtype;
}
char* Oval_test_name                   (Oval_test test){
	return ((Oval_test_ptr)test)->name;
}
OvalCollection_string Oval_test_notes  (Oval_test test){
	return newOvalCollection_string(((Oval_test_ptr)test)->notes);
}
char* Oval_test_comment                (Oval_test test){
	return ((Oval_test_ptr)test)->comment;
}
char* Oval_test_id                     (Oval_test test){
	return ((Oval_test_ptr)test)->id;
}
int Oval_test_deprecated               (Oval_test test){
	return ((Oval_test_ptr)test)->deprecated;
}
int Oval_test_version                  (Oval_test test){
	return ((Oval_test_ptr)test)->version;
}
Oval_operator_enum Oval_test_operator  (Oval_test test){
	return ((Oval_test_ptr)test)->operator;
}
Oval_existence_enum Oval_test_existence(Oval_test test){
	return ((Oval_test_ptr)test)->existence;
}
Oval_check_enum Oval_test_check        (Oval_test test){
	return ((Oval_test_ptr)test)->check;
}
Oval_object Oval_test_object           (Oval_test test){
	return ((Oval_test_ptr)test)->object;
}
Oval_state Oval_test_state             (Oval_test test){
	return ((Oval_test_ptr)test)->state;
}
