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

typedef struct oval_set_s{
	oval_set_type_enum type          ;
	oval_set_operation_enum operation;
} oval_set_t;

typedef struct oval_set_AGGREGATE_s{
	oval_set_type_enum type          ;
	oval_set_operation_enum operation;
	struct oval_collection_s *subsets      ;//type==OVAL_SET_AGGREGATE;
} oval_set_AGGREGATE_t;

typedef struct oval_set_COLLECTIVE_s{
	oval_set_type_enum type          ;
	oval_set_operation_enum operation;
	struct oval_collection_s *objects      ;//type==OVAL_SET_COLLECTIVE;
	struct oval_collection_s *filters      ;//type==OVAL_SET_COLLECTIVE;
} oval_set_COLLECTIVE_t;

int   oval_iterator_set_has_more      (struct oval_iterator_set_s *oc_set){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_set);
}
struct oval_set_s *oval_iterator_set_next         (struct oval_iterator_set_s *oc_set){
	return (struct oval_set_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_set);
}

oval_set_type_enum oval_set_type          (struct oval_set_s *set){
	return (set)->type;
}
oval_set_operation_enum oval_set_operation(struct oval_set_s *set){
	return ((struct oval_set_s*)set)->operation;
}
struct oval_iterator_set_s *oval_set_subsets       (struct oval_set_s *set){
	//type==OVAL_SET_AGGREGATE;
	struct oval_set_AGGREGATE_s *aggregate = (struct oval_set_AGGREGATE_s*)set;
	return (struct oval_iterator_set_s*)oval_collection_iterator(aggregate->subsets);
}
struct oval_iterator_object_s *oval_set_objects    (struct oval_set_s *set){
	//type==OVAL_SET_COLLECTIVE;
	struct oval_set_COLLECTIVE_s *collective = (struct oval_set_COLLECTIVE_s*)set;
	return (struct oval_iterator_object_s*)oval_collection_iterator(collective->objects);
}
struct oval_iterator_state_s *oval_set_filters     (struct oval_set_s *set){
	//type==OVAL_SET_COLLECTIVE;
	struct oval_set_COLLECTIVE_s *collective = (struct oval_set_COLLECTIVE_s*)set;
	return (struct oval_iterator_state_s*)oval_collection_iterator(collective->filters);
}

struct oval_set_s *oval_set_new(oval_set_type_enum type){
	oval_set_t *set;
	switch(type){
		case OVAL_SET_AGGREGATE:{
			oval_set_AGGREGATE_t *aggregate
			= (oval_set_AGGREGATE_t*)malloc(sizeof(oval_set_AGGREGATE_t));
			set = (oval_set_t*)aggregate;
			aggregate->subsets = oval_collection_new();
		}break;
		case OVAL_SET_COLLECTIVE:{
			oval_set_COLLECTIVE_t *collective
			= (oval_set_COLLECTIVE_t*)malloc(sizeof(oval_set_COLLECTIVE_t));
			set = (oval_set_t*)collective;
			collective->filters = oval_collection_new();
			collective->objects = oval_collection_new();
		}break;
	}
	set->operation = OVAL_SET_OPERATION_UNKNOWN;
	set->type      = type;
	return set;
}
void oval_set_free(struct oval_set_s *set){
	switch(set->type){
		case OVAL_SET_AGGREGATE:{
			oval_set_AGGREGATE_t *aggregate = (oval_set_AGGREGATE_t*)set;
			void free_set(struct oval_collection_item_s *subset){oval_set_free(subset);}
			oval_collection_free_items(aggregate->subsets,&free_set);
		}break;
		case OVAL_SET_COLLECTIVE:{
			oval_set_COLLECTIVE_t *collective = (oval_set_COLLECTIVE_t*)set;
			void free_state(struct oval_collection_item_s *state){oval_state_free(state);}
			oval_collection_free_items(collective->filters, &free_state);
			void free_object(struct oval_collection_item_s *object){oval_object_free(object);}
			oval_collection_free_items(collective->objects, &free_object);
		}break;
	}
	free(set);
}

void set_oval_set_type     (struct oval_set_s*, oval_set_type_enum);//TODO
void set_oval_set_operation(struct oval_set_s*, oval_set_operation_enum);//TODO
void add_oval_set_subsets  (struct oval_set_s*, struct oval_set_s*);//TODO   //type==OVAL_SET_AGGREGATE;//TODO
void add_oval_set_objects  (struct oval_set_s*, struct oval_object_s*);//TODO//type==OVAL_SET_COLLECTIVE;//TODO
void add_oval_set_filters  (struct oval_set_s*, struct oval_state_s*);//TODO //type==OVAL_SET_COLLECTIVE;//TODO

