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

typedef struct oval_variable_s{
	char* id                               ;
	oval_variable_type_enum type           ;
	oval_datatype_enum datatype            ;
	void* extension                        ;
} oval_variable_t;

typedef struct oval_variable_CONSTANT_s{
	char* Id                               ;
	oval_variable_type_enum type           ;
	oval_datatype_enum datatype            ;
	struct oval_collection_s *values       ;//type==OVAL_VARIABLE_CONSTANT
} oval_variable_CONSTANT_t;

typedef struct oval_variable_LOCAL_s{
	char* Id                               ;
	oval_variable_type_enum type           ;
	oval_datatype_enum datatype            ;
	struct oval_component_s *component     ;//type==OVAL_VARIABLE_LOCAL
} oval_variable_LOCAL_t;

int   oval_collection_variable_has_more(struct oval_iterator_variable_s *oc_variable){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_variable);
}
struct oval_variable_s *oval_collection_variable_next(struct oval_iterator_variable_s *oc_variable){
	return (struct oval_variable_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_variable);
}

char* oval_variable_id                               (struct oval_variable_s *variable){
	return ((struct oval_variable_s*)variable)->id;
}
oval_variable_type_enum oval_variable_type           (struct oval_variable_s *variable){
	return ((struct oval_variable_s*)variable)->type;
}
oval_datatype_enum oval_variable_datatype            (struct oval_variable_s *variable){
	return ((struct oval_variable_s*)variable)->datatype;
}
struct oval_iterator_value_s *oval_variable_values            (struct oval_variable_s *variable){
	//type==OVAL_VARIABLE_CONSTANT
	struct oval_variable_CONSTANT_s *constant = (struct oval_variable_CONSTANT_s*)variable;
	return (struct oval_iterator_value_s*)oval_collection_iterator(constant->values);
}
struct oval_component_s *oval_variable_component               (struct oval_variable_s *variable){
	//type==OVAL_VARIABLE_LOCAL
	struct oval_component_s *component = NULL;
	if(oval_variable_type(variable)==OVAL_VARIABLE_LOCAL){
		component = ((struct oval_variable_LOCAL_s*)variable)->component;
	}
	return component;
}

struct oval_variable_s *oval_variable_new(){
	oval_variable_t *variable = (oval_variable_t*)malloc(sizeof(oval_variable_t));
	variable->id        = NULL;
	variable->type      = OVAL_VARIABLE_UNKNOWN;
	variable->datatype  = OVAL_DATATYPE_UNKNOWN;
	variable->extension = NULL;
	return variable;
};
void  oval_variable_free(struct oval_variable_s *variable){
	if(variable->id != NULL)free(variable->id);
	if(variable->extension!=NULL){
		switch(variable->type){
			case OVAL_VARIABLE_LOCAL:{
				oval_component_free(variable->extension);
			}break;
			case OVAL_VARIABLE_CONSTANT:{
				void free_value(struct oval_collection_item_s *value){oval_value_free(value);}
				oval_collection_free_items((struct oval_collection_s*)variable->extension,&free_value);
			}break;
		}
	}
	free(variable);
}
void set_oval_variable_id       (struct oval_variable_s *variable, char* id){variable->id = id;}
void set_oval_variable_type     (struct oval_variable_s *variable, oval_variable_type_enum type){
	variable->type = type;
	if(type==OVAL_VARIABLE_CONSTANT)variable->extension = oval_collection_new();
}

void set_oval_variable_datatype (struct oval_variable_s*, oval_datatype_enum);//TODO
void add_oval_variable_values   (struct oval_variable_s*, struct oval_value_s*);//TODO//type==OVAL_VARIABLE_CONSTANT
void set_oval_variable_component(struct oval_variable_s*, struct oval_component_s*);//TODO//type==OVAL_VARIABLE_LOCAL
