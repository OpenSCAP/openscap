/*
 * oval_state.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include <includes/oval_definitions.h>
#include "includes/oval_collection_impl.h"

	typedef struct Oval_state_s{
		Oval_family_enum family    ;
		Oval_subtype_enum subtype  ;
		char* name                 ;
		char** notes               ;
		char* comment              ;
		char* id                   ;
		int deprecated             ;
		int version                ;
	} Oval_state_t;

	typedef Oval_state_t* Oval_state_ptr;

	OvalCollection_state newOvalCollection_state(Oval_state* state_array){
		return (OvalCollection_state)newOvalCollection((OvalCollection_target*)state_array);
	}
	int   OvalCollection_state_hasMore      (OvalCollection_state oc_state){
		return OvalCollection_hasMore((OvalCollection_ptr)oc_state);
	}
	Oval_state OvalCollection_state_next         (OvalCollection_state oc_state){
		return (Oval_state)OvalCollection_next((OvalCollection_ptr)oc_state);
	}

	Oval_family_enum Oval_state_family    (Oval_state state){
		return ((Oval_state_ptr)state)->family;
	}
	Oval_subtype_enum Oval_state_subtype  (Oval_state state){
		return ((Oval_state_ptr)state)->subtype;
	}
	char* Oval_state_name                 (Oval_state state){
		return ((Oval_state_ptr)state)->name;
	}
	OvalCollection_string Oval_state_notes(Oval_state state){
		char** notes = ((Oval_state_ptr)state)->notes;
		return newOvalCollection_string(notes);
	}
	char* Oval_state_comment              (Oval_state state){
		return ((Oval_state_ptr)state)->comment;
	}
	char* Oval_state_id                   (Oval_state state){
		return ((Oval_state_ptr)state)->id;
	}
	int Oval_state_deprecated             (Oval_state state){
		return ((Oval_state_ptr)state)->deprecated;
	}
	int Oval_state_version                (Oval_state state){
		return ((Oval_state_ptr)state)->version;
	}
