/*
 * oval_set.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_set_s{
	Oval_set_type_enum type          ;
	Oval_set_operation_enum operation;
} Oval_set_t;

typedef struct Oval_set_AGGREGATE_s{
	Oval_set_type_enum type          ;
	Oval_set_operation_enum operation;
	Oval_set* subsets                ;//type==OVAL_SET_AGGREGATE;
} Oval_set_AGGREGATE_t;

typedef struct Oval_set_COLLECTIVE_s{
	Oval_set_type_enum type          ;
	Oval_set_operation_enum operation;
	Oval_object* objects             ;//type==OVAL_SET_COLLECTIVE;
	Oval_state*  filters             ;//type==OVAL_SET_COLLECTIVE;
} Oval_set_COLLECTIVE_t;

typedef Oval_set_t* Oval_set_ptr;
typedef Oval_set_AGGREGATE_t* Oval_set_AGGREGATE_ptr;
typedef Oval_set_COLLECTIVE_t* Oval_set_COLLECTIVE_ptr;


OvalCollection_set newOvalCollection_set(Oval_set* set_array){
	return (OvalCollection_set)newOvalCollection((OvalCollection_target*)set_array);
}
int   OvalCollection_set_hasMore      (OvalCollection_set oc_set){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_set);
}
Oval_set OvalCollection_set_next         (OvalCollection_set oc_set){
	return (Oval_set)OvalCollection_next((OvalCollection_ptr)oc_set);
}

Oval_set_type_enum Oval_set_type          (Oval_set set){
	return ((Oval_set_ptr)set)->type;
}
Oval_set_operation_enum Oval_set_operation(Oval_set set){
	return ((Oval_set_ptr)set)->operation;
}
OvalCollection_set Oval_set_subsets       (Oval_set set){
	//type==OVAL_SET_AGGREGATE;
	OvalCollection_set subsets = NULL;
	if(Oval_set_type(set)==OVAL_SET_AGGREGATE){
		Oval_set* sets = ((Oval_set_AGGREGATE_ptr)set)->subsets;
		subsets = newOvalCollection_set(sets);
	}
	return subsets;
}
OvalCollection_object Oval_set_objects    (Oval_set set){
	//type==OVAL_SET_COLLECTIVE;
	OvalCollection_object oc_object = NULL;
	if(Oval_set_type(set)==OVAL_SET_COLLECTIVE){
		Oval_object* objects = ((Oval_set_COLLECTIVE_ptr)set)->objects;
		oc_object = newOvalCollection_object(objects);
	}
	return oc_object;
}
OvalCollection_state Oval_set_filters     (Oval_set set){
	//type==OVAL_SET_COLLECTIVE;
	OvalCollection_state oc_state = NULL;
	if(Oval_set_type(set)==OVAL_SET_COLLECTIVE){
		Oval_state* states = ((Oval_set_COLLECTIVE_ptr)set)->filters;
		oc_state = newOvalCollection_state(states);
	}
	return oc_state;
}
