/*
 * oval_behavior.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"
#include "includes/oval_string_map_impl.h"

typedef struct oval_behavior_s{
	struct oval_value_s *   value          ;
	struct oval_string_map_s *att_values   ;
} oval_behavior_t;

int   oval_iterator_behavior_has_more      (struct oval_iterator_behavior_s *oc_behavior){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_behavior);
}
struct oval_behavior_s *oval_iterator_behavior_next         (struct oval_iterator_behavior_s *oc_behavior){
	return (struct oval_behavior_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_behavior);
}

struct oval_value_s *oval_behavior_value (struct oval_behavior_s *behavior){
	return (behavior)->value;
}
struct oval_iterator_string_s *oval_behavior_attributeKeys (struct oval_behavior_s *behavior){
	return (struct oval_iterator_string_s*)oval_string_map_keys(behavior->att_values);
}
struct oval_value_s *oval_behavior_value_forKey(struct oval_behavior_s *behavior, char* attributeKey){
	return (struct oval_value_s*)oval_string_map_get_value(behavior->att_values,attributeKey);
}

struct oval_behavior_s *oval_behavior_new(){
	oval_behavior_t *behavior = (oval_behavior_t*)malloc(sizeof(oval_behavior_t));
	behavior->value         = NULL;
	behavior->att_values    = oval_string_map_new();
	return behavior;
}
void oval_behavior_free(struct oval_behavior_s *behavior){
	void free_value(struct oval_collection_item_s *value){oval_value_free(behavior->value);}
	if(behavior->value != NULL)free_value(behavior->value);
	oval_string_map_free(behavior->att_values,&free_value);
	free(behavior);
}

void set_oval_behavior_value_for_key (struct oval_behavior_s *behavior, struct oval_value_s *value, char *key){
	oval_string_map_put(behavior->att_values,key,value);
}
