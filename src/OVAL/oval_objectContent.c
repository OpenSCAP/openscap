/*
 * oval_object_content.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct oval_object_content_s{
	char* fieldName                   ;
	oval_object_content_type_enum type;
} oval_object_content_t;

typedef struct oval_object_content_ENTITY_s{
	char* fieldName                   ;
	oval_object_content_type_enum type;
	struct oval_entity_s *entity      ;//type == OVAL_OBJECTCONTENT_ENTITY
	oval_check_enum varCheck          ;//type == OVAL_OBJECTCONTENT_ENTITY
} oval_object_content_ENTITY_t;

typedef struct oval_object_content_SET_s{
	char* fieldName                   ;
	oval_object_content_type_enum type;
	struct oval_set_s *set            ;//type == OVAL_OBJECTCONTENT_SET
} oval_object_content_SET_t;

int   oval_iterator_object_content_has_more      (struct oval_iterator_object_content_s *oc_object_content){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_object_content);
}
struct oval_object_content_s *oval_iterator_object_content_next         (struct oval_iterator_object_content_s *oc_object_content){
	return (struct oval_object_content_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_object_content);
}

char* oval_object_content_field_name(struct oval_object_content_s *content){
	return ((struct oval_object_content_s*)content)->fieldName;
}
oval_object_content_type_enum oval_object_content_type(struct oval_object_content_s *content){
	return ((struct oval_object_content_s*)content)->type;
}
struct oval_entity_s *oval_object_content_entity(struct oval_object_content_s *content){
	//type == OVAL_OBJECTCONTENT_ENTITY
	struct oval_entity_s *entity = NULL;
	if(oval_object_content_type(content)==OVAL_OBJECTCONTENT_ENTITY){
		entity = ((struct oval_object_content_ENTITY_s*)content)->entity;
	}
	return entity;
}
oval_check_enum oval_object_content_varCheck         (struct oval_object_content_s *content){
	//type == OVAL_OBJECTCONTENT_ENTITY
	oval_check_enum varCheck = OVAL_CHECK_UNKNOWN;
	if(oval_object_content_type(content)==OVAL_OBJECTCONTENT_ENTITY){
		varCheck = ((struct oval_object_content_ENTITY_s*)content)->varCheck;
	}
	return varCheck;
}
struct oval_set_s *oval_object_content_set                     (struct oval_object_content_s *content){
	//type == OVAL_OBJECTCONTENT_SET
	struct oval_set_s *set = NULL;
	if(oval_object_content_type(content)==OVAL_OBJECTCONTENT_SET){
		set = ((struct oval_object_content_SET_s*)content)->set;
	}
	return set;
}

struct oval_object_content_s *oval_object_content_new(oval_object_content_type_enum type){
	struct oval_object_content_s *content = NULL;
	switch(type){
		case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_object_content_ENTITY_s *entity
			= (oval_object_content_ENTITY_t*)malloc(sizeof(oval_object_content_ENTITY_t));
			content = (oval_object_content_t*)entity;
			entity->entity = NULL;
			entity->varCheck = OVAL_CHECK_UNKNOWN;
		}break;
		case OVAL_OBJECTCONTENT_SET:{
			struct oval_object_content_SET_s *set
			= (oval_object_content_SET_t*)malloc(sizeof(oval_object_content_SET_t));
			set->set = NULL;
			content = (oval_object_content_t*)set;
		}break;
	}
	content->fieldName = NULL;
	content->type = type;
	return content;
}

void oval_object_content_free(struct oval_object_content_s *content){
	switch(content->type){
		case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_object_content_ENTITY_s *entity = (oval_object_content_ENTITY_t*)content;
			if(entity->entity != NULL)oval_entity_free(entity->entity);
		}break;
		case OVAL_OBJECTCONTENT_SET:{
			struct oval_object_content_SET_s *set = (oval_object_content_SET_t*)content;
			if(set->set != NULL)oval_set_free(set->set);
		}break;
	}
	free(content);
}

void set_oval_object_content_field_name(struct oval_object_content_s*, char*);//TODO
void set_oval_object_content_type      (struct oval_object_content_s*, oval_object_content_type_enum);//TODO
void set_oval_object_content_entity    (struct oval_object_content_s*, struct oval_entity_s*);//TODO//type == OVAL_OBJECTCONTENT_ENTITY
void set_oval_object_content_varCheck  (struct oval_object_content_s*, oval_check_enum);//TODO//type == OVAL_OBJECTCONTENT_ENTITY
void set_oval_object_content_set       (struct oval_object_content_s*, struct oval_set_s*);//TODO   //type == OVAL_OBJECTCONTENT_SET

//typedef void (*oval_object_content_consumer)(struct oval_object_content_s*,void*);
int oval_object_content_parse_tag(xmlTextReaderPtr reader ,struct oval_parser_context_s *context, oval_object_content_consumer consumer, void* user){
	xmlChar* tagname   = xmlTextReaderName(reader);
	xmlChar* namespace = xmlTextReaderNamespaceUri(reader);

	oval_object_content_type_enum type = (strcmp(tagname,"set")==0)?OVAL_OBJECTCONTENT_SET:OVAL_OBJECTCONTENT_ENTITY;
	struct oval_object_content_s *content  = oval_object_content_new(type);
	content->fieldName = tagname;
	int return_code;
	switch(type){
		case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_object_content_ENTITY_s *content_entity = (struct oval_object_content_ENTITY_s*)content;
			void consume_entity(struct oval_entity_s *entity, void *null){content_entity->entity = entity;}
			return_code = oval_entity_parse_tag(reader, context, &consume_entity, NULL);
			content_entity->varCheck = oval_check_parse(reader,"var_check");
		};break;
		case OVAL_OBJECTCONTENT_SET:{
			struct oval_object_content_SET_s *content_set = (struct oval_object_content_SET_s*)content;
			void consume_set(struct oval_set_s *set, void *null){content_set->set = set;}
			return_code = 1;//TODO:oval_set_parse_tag(reader, context, &consume_set, NULL);
		};break;
	}
	(*consumer)(content, user);

	free(namespace);
	return return_code;
}

void oval_object_content_to_print(struct oval_object_content_s *content, char* indent, int index){
	char nxtindent[100];*nxtindent = 0;
	strcat(nxtindent,indent);
	if(index==0)strcat(nxtindent,"CONTENT.");
	else{
		strcat(nxtindent,"CONTENT[");
		char itoad[10];*itoad=0;itoa(index,itoad,10);
		strcat(nxtindent,itoad);
		strcat(nxtindent,"].");
	}
	/*
	struct oval_entity_s *oval_object_content_entity      (struct oval_object_content_s*);//type == OVAL_OBJECTCONTENT_ENTITY
	struct oval_set_s *oval_object_content_set            (struct oval_object_content_s*);//type == OVAL_OBJECTCONTENT_SET
	 */
	printf("%sFIELD     = %s\n",nxtindent,oval_object_content_field_name(content));
	printf("%sTYPE      = %d\n",nxtindent,oval_object_content_type(content));
	switch(oval_object_content_type(content)){
		case OVAL_OBJECTCONTENT_ENTITY:{
			printf("%sVAR_CHECK = %d\n",nxtindent,oval_object_content_varCheck(content));
			printf("%sENTITY    <<TODO>>\n",nxtindent);
		}break;
		case OVAL_OBJECTCONTENT_SET:{
			printf("%sSET       <<TODO>>\n",nxtindent);
		}break;
	}
}


