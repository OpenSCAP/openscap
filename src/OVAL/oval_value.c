/*
 * oval_value.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_value_s{
	Oval_datatype_enum datatype;
	char* text                 ;
} Oval_value_t;

typedef Oval_value_t* Oval_value_ptr;


OvalCollection_value newOvalCollection_value(Oval_value* value_array){
	return (OvalCollection_value)newOvalCollection((OvalCollection_target*)value_array);
}
int   OvalCollection_value_hasMore      (OvalCollection_value oc_value){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_value);
}
Oval_value OvalCollection_value_next         (OvalCollection_value oc_value){
	return (Oval_value)OvalCollection_next((OvalCollection_ptr)oc_value);
}

Oval_datatype_enum Oval_value_datatype(Oval_value value){
	return ((Oval_value_ptr)value)->datatype;
}
char* Oval_value_text                 (Oval_value value){
	return ((Oval_value_ptr)value)->text;
}
unsigned char* Oval_value_binary      (Oval_value value){
	return NULL;//TODO
}
char Oval_value_boolean               (Oval_value value){
	return 0;//TODO
}//datatype==OVAL_DATATYPE_BOOLEAN
float Oval_value_float                (Oval_value value){
	return 0;//TODO
}//datatype==OVAL_DATATYPE_FLOAT
long Oval_value_integer               (Oval_value value){
	return 0;//TODO
}//datatype==OVAL_DATATYPE_INTEGER
