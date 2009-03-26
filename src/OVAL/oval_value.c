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

typedef struct oval_value_s{
	oval_datatype_enum datatype;
	char* text                 ;
} oval_value_t;

int   oval_iterator_value_has_more      (struct oval_iterator_value_s *oc_value){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_value);
}
struct oval_value_s *oval_iterator_value_next         (struct oval_iterator_value_s *oc_value){
	return (struct oval_value_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_value);
}

oval_datatype_enum oval_value_datatype(struct oval_value_s *value){
	return (value)->datatype;
}
char* oval_value_text                 (struct oval_value_s *value){
	return ((struct oval_value_s*)value)->text;
}
unsigned char* oval_value_binary      (struct oval_value_s *value){
	return NULL;//TODO
}
char oval_value_boolean               (struct oval_value_s *value){
	return 0;//TODO
}//datatype==OVAL_DATATYPE_BOOLEAN
float oval_value_float                (struct oval_value_s *value){
	return 0;//TODO
}//datatype==OVAL_DATATYPE_FLOAT
long oval_value_integer               (struct oval_value_s *value){
	return 0;//TODO
}//datatype==OVAL_DATATYPE_INTEGER


struct oval_value_s *oval_value_new(){
	oval_value_t *value = (oval_value_t*)malloc(sizeof(oval_value_t));
	value->datatype = OVAL_DATATYPE_UNKNOWN;
	value->text     = NULL;
	return value;
}
void oval_value_free(struct oval_value_s *value){
	if(value->text!=NULL)free(value->text);
	free(value);
}

void set_oval_value_datatype(struct oval_value_s*, oval_datatype_enum);
void set_oval_value_text    (struct oval_value_s*, char*);
void set_oval_value_binary  (struct oval_value_s*, unsigned char*);//datatype==OVAL_DATATYPE_BINARY
void set_oval_value_boolean (struct oval_value_s*, int);           //datatype==OVAL_DATATYPE_BOOLEAN
void set_oval_value_float   (struct oval_value_s*, float);         //datatype==OVAL_DATATYPE_FLOAT
void set_oval_value_integer (struct oval_value_s*, long);          //datatype==OVAL_DATATYPE_INTEGER

