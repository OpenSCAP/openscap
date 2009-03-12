/*
 * oval_entity.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_entity_s{

	Oval_entity_type_enum type              ;
	Oval_datatype_enum datatype             ;
	Oval_operator_enum operator             ;
	int mask                                ;
	Oval_entity_varref_type_enum varref_type;
	Oval_variable variable                  ;
	Oval_value value                        ;
} Oval_entity_t;

typedef Oval_entity_t* Oval_entity_ptr;


OvalCollection_entity newOvalCollection_entity(Oval_entity* entity_array){
	return (OvalCollection_entity)newOvalCollection((OvalCollection_target*)entity_array);
}
int   OvalCollection_entity_hasMore      (OvalCollection_entity oc_entity){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_entity);
}
Oval_entity OvalCollection_entity_next         (OvalCollection_entity oc_entity){
	return (Oval_entity)OvalCollection_next((OvalCollection_ptr)oc_entity);
}

Oval_entity_type_enum Oval_entity_type              (Oval_entity entity){
	return ((Oval_entity_ptr)entity)->type;
}
Oval_datatype_enum Oval_entity_datatype             (Oval_entity entity){
	return ((Oval_entity_ptr)entity)->datatype;
}
Oval_operator_enum Oval_entity_operator             (Oval_entity entity){
	return ((Oval_entity_ptr)entity)->operator;
}
int Oval_entity_mask                                (Oval_entity entity){
	return ((Oval_entity_ptr)entity)->mask;
}
Oval_entity_varref_type_enum Oval_entity_varref_type(Oval_entity entity){
	return ((Oval_entity_ptr)entity)->varref_type;
}
Oval_variable Oval_entity_variable                  (Oval_entity entity){
	return ((Oval_entity_ptr)entity)->variable;
}
Oval_value Oval_entity_value                        (Oval_entity entity){
	return ((Oval_entity_ptr)entity)->value;
}
