/*
 * oval_state.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include <liboval/oval_definitions.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

	typedef struct oval_state_s{
		oval_family_enum family    ;
		oval_subtype_enum subtype  ;
		char* name                 ;
		struct oval_collection_s *notes  ;
		char* comment              ;
		char* id                   ;
		int deprecated             ;
		int version                ;
	} oval_state_t;

	int   oval_iterator_state_has_more(struct oval_iterator_state_s *oc_state){
		return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_state);
	}
	struct oval_state_s *oval_iterator_state_next(struct oval_iterator_state_s *oc_state){
		return (struct oval_state_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_state);
	}

	oval_family_enum oval_state_family    (struct oval_state_s *state){
		return (state)->family;
	}
	oval_subtype_enum oval_state_subtype  (struct oval_state_s *state){
		return ((struct oval_state_s*)state)->subtype;
	}
	char* oval_state_name                 (struct oval_state_s *state){
		return ((struct oval_state_s*)state)->name;
	}
	struct oval_iterator_string_s *oval_state_notes(struct oval_state_s *state){
		return (struct oval_iterator_string_s*)oval_collection_iterator(state->notes);
	}
	char* oval_state_comment(struct oval_state_s *state){
		return ((struct oval_state_s*)state)->comment;
	}
	char* oval_state_id                   (struct oval_state_s *state){
		return ((struct oval_state_s*)state)->id;
	}
	int oval_state_deprecated             (struct oval_state_s *state){
		return ((struct oval_state_s*)state)->deprecated;
	}
	int oval_state_version                (struct oval_state_s *state){
		return state->version;
	}

	struct oval_state_s *oval_state_new(){
		oval_state_t *state = (oval_state_t*)malloc(sizeof(oval_state_t));
		state->deprecated = 0;
		state->version    = 0;
		state->family     = FAMILY_UNKNOWN;
		state->subtype    = OVAL_SUBTYPE_UNKNOWN;
		state->comment    = NULL;
		state->id         = NULL;
		state->name       = NULL;
		state->notes      = oval_collection_new();
		return state;
	}
	void oval_state_free(struct oval_state_s *state){
		if(state->comment != NULL)free(state->comment);
		if(state->id      != NULL)free(state->id     );
		if(state->name    != NULL)free(state->name   );
		void free_notes(struct oval_collection_item_s *note){free(note);}
		oval_collection_free_items(state->notes,&free_notes);
		free(state);
	}
	void set_oval_state_id(struct oval_state_s *state, char* id){state->id = id;}

	void set_oval_state_family    (struct oval_state_s*, oval_family_enum);//TODO
	void set_oval_state_subtype   (struct oval_state_s*, oval_subtype_enum);//TODO
	void set_oval_state_name      (struct oval_state_s*, char*);//TODO
	void add_oval_state_notes     (struct oval_state_s*, char*);//TODO
	void set_oval_state_comment   (struct oval_state_s*, char*);//TODO
	void set_oval_state_deprecated(struct oval_state_s*, int);//TODO
	void set_oval_state_version   (struct oval_state_s*, int);//TODO
