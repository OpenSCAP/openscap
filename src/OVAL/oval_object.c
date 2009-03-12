/*
 * oval_object.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include <includes/oval_definitions.h>
#include "includes/oval_collection_impl.h"

	typedef struct Oval_object_s{
		Oval_family_enum family          ;
		Oval_subtype_enum subtype        ;
		char* name                       ;
		char** notes                     ;
		char* comment                    ;
		char* id                         ;
		int deprecated                   ;
		int version                      ;
		Oval_objectContent* objectContent;
		Oval_behavior* behaviors         ;
	} Oval_object_t;

	typedef Oval_object_t* Oval_object_ptr;

	OvalCollection_object newOvalCollection_object(Oval_object* object_array){
		return (OvalCollection_object)newOvalCollection((OvalCollection_target*)object_array);
	}
	int   OvalCollection_object_hasMore      (OvalCollection_object oc_object){
		return OvalCollection_hasMore((OvalCollection_ptr)oc_object);
	}
	Oval_object OvalCollection_object_next         (OvalCollection_object oc_object){
		return (Oval_object)OvalCollection_next((OvalCollection_ptr)oc_object);
	}

	Oval_family_enum Oval_object_family                   (Oval_object object){
		return ((Oval_object_ptr)object)->family;
	}
	Oval_subtype_enum Oval_object_subtype                 (Oval_object object){
		return ((Oval_object_ptr)object)->subtype;
	}
	char* Oval_object_name                                (Oval_object object){
		return ((Oval_object_ptr)object)->name;
	}
	OvalCollection_string Oval_object_notes               (Oval_object object){
		char** notes = ((Oval_object_ptr)object)->notes;
		return newOvalCollection_string(notes);
	}
	char* Oval_object_comment                             (Oval_object object){
		return ((Oval_object_ptr)object)->comment;
	}
	char* Oval_object_id                                  (Oval_object object){
		return ((Oval_object_ptr)object)->id;
	}
	int Oval_object_deprecated                            (Oval_object object){
		return ((Oval_object_ptr)object)->deprecated;
	}
	int Oval_object_version                               (Oval_object object){
		return ((Oval_object_ptr)object)->version;
	}
	OvalCollection_objectContent Oval_object_objectContent(Oval_object object){
		Oval_objectContent* objectContent = ((Oval_object_ptr)object)->objectContent;
		return newOvalCollection_objectContent(objectContent);
	}
	OvalCollection_behavior Oval_object_behaviors         (Oval_object object){
		Oval_behavior* behaviors = ((Oval_object_ptr)object)->behaviors;
		return newOvalCollection_behavior(behaviors);
	}
