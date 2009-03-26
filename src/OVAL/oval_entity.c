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

struct oval_entity_s{
	oval_entity_type_enum type              ;
	oval_datatype_enum datatype             ;
	oval_operator_enum operator             ;
	int mask                                ;
	oval_entity_varref_type_enum varref_type;
	struct oval_variable_s *variable        ;
	struct oval_value_s *value              ;
};


int   oval_iterator_entity_has_more      (struct oval_iterator_entity_s *oc_entity){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_entity);
}
struct oval_entity_s *oval_iterator_entity_next         (struct oval_iterator_entity_s *oc_entity){
	return (struct oval_entity_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_entity);
}

oval_entity_type_enum oval_entity_type(struct oval_entity_s *entity){
	return ((struct oval_entity_s*)entity)->type;
}
oval_datatype_enum oval_entity_datatype             (struct oval_entity_s *entity){
	return ((struct oval_entity_s*)entity)->datatype;
}
oval_operator_enum oval_entity_operator             (struct oval_entity_s *entity){
	return ((struct oval_entity_s*)entity)->operator;
}
int oval_entity_mask                                (struct oval_entity_s *entity){
	return ((struct oval_entity_s*)entity)->mask;
}
oval_entity_varref_type_enum oval_entity_varref_type(struct oval_entity_s *entity){
	return ((struct oval_entity_s*)entity)->varref_type;
}
struct oval_variable_s *oval_entity_variable               (struct oval_entity_s *entity){
	return ((struct oval_entity_s*)entity)->variable;
}
struct oval_value_s *oval_entity_value                     (struct oval_entity_s *entity){
	return ((struct oval_entity_s*)entity)->value;
}

struct oval_entity_s *oval_entity_new(){
	struct oval_entity_s *entity = (struct oval_entity_s*)malloc(sizeof(struct oval_entity_s));
	entity->datatype = OVAL_DATATYPE_UNKNOWN;
	entity->mask     = 0;
	entity->operator = OPERATOR_UNKNOWN;
	entity->type     = OVAL_ENTITY_TYPE_UNKNOWN;
	entity->value    = NULL;
	entity->variable = NULL;
	return entity;
}
void oval_entity_free(struct oval_entity_s *entity){
	if(entity->value    != NULL)oval_value_free(entity->value);
	if(entity->variable != NULL)oval_variable_free(entity->variable);
	free(entity);
}

void set_oval_entity_type       (struct oval_entity_s *entity, oval_entity_type_enum type);//TODO
void set_oval_entity_datatype   (struct oval_entity_s *entity, oval_datatype_enum);//TODO
void set_oval_entity_operator   (struct oval_entity_s *entity, oval_operator_enum);//TODO
void set_oval_entity_mask       (struct oval_entity_s *entity, int);//TODO
void set_oval_entity_varref_type(struct oval_entity_s *entity, oval_entity_varref_type_enum);//TODO
void set_oval_entity_variable   (struct oval_entity_s *entity, struct oval_variable_s*);//TODO
void set_oval_entity_value      (struct oval_entity_s *entity, struct oval_value_s*);//TODO


//typedef void (*oval_entity_consumer)(struct oval_entity_node_s*, void*);
int oval_entity_parse_tag (xmlTextReaderPtr reader, struct oval_parser_context_s *context, oval_entity_consumer consumer, void* user){
	int linno = xmlTextReaderGetParserLineNumber(reader);
	printf("NOTICE::(oval_entity_parse_tag)skipping <%s> depth = %d line = %d\n",xmlTextReaderName(reader),xmlTextReaderDepth(reader),linno);
	return oval_parser_skip_tag(reader,context);
}

