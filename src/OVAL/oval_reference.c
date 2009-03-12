/*
 * oval_reference.c
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_reference_s{
	char* source;
	char* id    ;
	char* url   ;
} Oval_reference_t;

typedef Oval_reference_t* Oval_reference_ptr;


OvalCollection_reference newOvalCollection_reference(Oval_reference* reference_array){
	return (OvalCollection_reference)newOvalCollection((OvalCollection_target*)reference_array);
}
int   OvalCollection_reference_hasMore      (OvalCollection_reference oc_reference){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_reference);
}
Oval_reference OvalCollection_reference_next         (OvalCollection_reference oc_reference){
	return (Oval_reference)OvalCollection_next((OvalCollection_ptr)oc_reference);
}

char* Oval_reference_source(Oval_reference reference){
	return ((Oval_reference_ptr)reference)->source;
}
char* Oval_reference_id    (Oval_reference reference){
	return ((Oval_reference_ptr)reference)->id;
}
char* Oval_reference_url   (Oval_reference reference){
	return ((Oval_reference_ptr)reference)->url;
}
