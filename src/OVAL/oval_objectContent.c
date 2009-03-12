/*
 * oval_objectContent.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_objectContent_s{
	char* fieldName                  ;
	Oval_objectContent_type_enum type;
} Oval_objectContent_t;

typedef struct Oval_objectContent_ENTITY_s{
	char* fieldName                  ;
	Oval_objectContent_type_enum type;
	Oval_entity entity               ;//type == OVAL_OBJECTCONTENT_ENTITY
	Oval_check_enum varCheck         ;//type == OVAL_OBJECTCONTENT_ENTITY
} Oval_objectContent_ENTITY_t;

typedef struct Oval_objectContent_SET_s{
	char* fieldName                  ;
	Oval_objectContent_type_enum type;
	Oval_set set                     ;//type == OVAL_OBJECTCONTENT_SET
} Oval_objectContent_SET_t;

typedef Oval_objectContent_t* Oval_objectContent_ptr;
typedef Oval_objectContent_ENTITY_t* Oval_objectContent_ENTITY_ptr;
typedef Oval_objectContent_SET_t* Oval_objectContent_SET_ptr;


OvalCollection_objectContent newOvalCollection_objectContent(Oval_objectContent* objectContent_array){
	return (OvalCollection_objectContent)newOvalCollection((OvalCollection_target*)objectContent_array);
}
int   OvalCollection_objectContent_hasMore      (OvalCollection_objectContent oc_objectContent){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_objectContent);
}
Oval_objectContent OvalCollection_objectContent_next         (OvalCollection_objectContent oc_objectContent){
	return (Oval_objectContent)OvalCollection_next((OvalCollection_ptr)oc_objectContent);
}


char* Oval_objectContent_fieldName                  (Oval_objectContent content){
	return ((Oval_objectContent_ptr)content)->fieldName;
}
Oval_objectContent_type_enum Oval_objectContent_type(Oval_objectContent content){
	return ((Oval_objectContent_ptr)content)->type;
}
Oval_entity Oval_objectContent_entity               (Oval_objectContent content){
	//type == OVAL_OBJECTCONTENT_ENTITY
	Oval_entity entity = NULL;
	if(Oval_objectContent_type(content)==OVAL_OBJECTCONTENT_ENTITY){
		entity = ((Oval_objectContent_ENTITY_ptr)content)->entity;
	}
	return entity;
}
Oval_check_enum Oval_objectContent_varCheck         (Oval_objectContent content){
	//type == OVAL_OBJECTCONTENT_ENTITY
	Oval_check_enum varCheck = OVAL_CHECK_UNKNOWN;
	if(Oval_objectContent_type(content)==OVAL_OBJECTCONTENT_ENTITY){
		varCheck = ((Oval_objectContent_ENTITY_ptr)content)->varCheck;
	}
	return varCheck;
}
Oval_set Oval_objectContent_set                     (Oval_objectContent content){
	//type == OVAL_OBJECTCONTENT_SET
	Oval_set set = NULL;
	if(Oval_objectContent_type(content)==OVAL_OBJECTCONTENT_SET){
		set = ((Oval_objectContent_SET_ptr)content)->set;
	}
	return set;
}
