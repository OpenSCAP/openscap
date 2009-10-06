/**
 * @file oval_objectContent.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "oval_definitions_impl.h"
#include "oval_collection_impl.h"
#include "oval_agent_api_impl.h"

typedef struct oval_object_content {
	char *fieldName;
	oval_object_content_type_t type;
} oval_object_content_t;

typedef struct oval_object_content_ENTITY {
	char *fieldName;
	oval_object_content_type_t type;
	struct oval_entity *entity;	//type == OVAL_OBJECTCONTENT_ENTITY
	oval_check_t varCheck;	//type == OVAL_OBJECTCONTENT_ENTITY
} oval_object_content_ENTITY_t;

typedef struct oval_object_content_SET {
	char *fieldName;
	oval_object_content_type_t type;
	struct oval_setobject *set;	//type == OVAL_OBJECTCONTENT_SET
} oval_object_content_SET_t;

int oval_object_content_iterator_has_more(struct oval_object_content_iterator
					  *oc_object_content)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_object_content);
}

struct oval_object_content *oval_object_content_iterator_next(struct
							      oval_object_content_iterator
							      *oc_object_content)
{
	return (struct oval_object_content *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_object_content);
}

void oval_object_content_iterator_free(struct
							      oval_object_content_iterator
							      *oc_object_content)
{
    oval_collection_iterator_free((struct oval_iterator *)
					  oc_object_content);
}

char *oval_object_content_get_field_name(struct oval_object_content *content)
{
	return ((struct oval_object_content *)content)->fieldName;
}

oval_object_content_type_t oval_object_content_get_type(struct
						       oval_object_content
						       *content)
{
	return ((struct oval_object_content *)content)->type;
}

struct oval_entity *oval_object_content_get_entity(struct oval_object_content
					       *content)
{
	//type == OVAL_OBJECTCONTENT_ENTITY
	struct oval_entity *entity = NULL;
	if (oval_object_content_get_type(content) == OVAL_OBJECTCONTENT_ENTITY) {
		entity = ((struct oval_object_content_ENTITY *)content)->entity;
	}
	return entity;
}

oval_check_t oval_object_content_get_varCheck(struct oval_object_content *
					     content)
{
	//type == OVAL_OBJECTCONTENT_ENTITY
	oval_check_t varCheck = OVAL_CHECK_UNKNOWN;
	if (oval_object_content_get_type(content) == OVAL_OBJECTCONTENT_ENTITY) {
		varCheck =
		    ((struct oval_object_content_ENTITY *)content)->varCheck;
	}
	return varCheck;
}

struct oval_setobject *oval_object_content_get_setobject(struct oval_object_content *content)
{
	//type == OVAL_OBJECTCONTENT_SET
	struct oval_setobject *set = NULL;
	if (oval_object_content_get_type(content) == OVAL_OBJECTCONTENT_SET) {
		set = ((struct oval_object_content_SET *)content)->set;
	}
	return set;
}

struct oval_object_content
    *oval_object_content_new(oval_object_content_type_t type)
{
	struct oval_object_content *content = NULL;
	switch (type) {
	case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_object_content_ENTITY *entity
			    =
			    (oval_object_content_ENTITY_t *)
			    malloc(sizeof(oval_object_content_ENTITY_t));
			content = (oval_object_content_t *) entity;
			entity->entity = NULL;
			entity->varCheck = OVAL_CHECK_UNKNOWN;
		}
		break;
	case OVAL_OBJECTCONTENT_SET:{
			struct oval_object_content_SET *set
			    =
			    (oval_object_content_SET_t *)
			    malloc(sizeof(oval_object_content_SET_t));
			set->set = NULL;
			content = (oval_object_content_t *) set;
		}
		break;
	case OVAL_OBJECTCONTENT_UNKNOWN: break;
	}
	content->fieldName = NULL;
	content->type = type;
	return content;
}

void oval_object_content_free(struct oval_object_content *content)
{
	if(content->fieldName!=NULL)free(content->fieldName);
	content->fieldName = NULL;
	switch (content->type) {
	case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_object_content_ENTITY *entity =
			    (oval_object_content_ENTITY_t *) content;
			if (entity->entity != NULL)
				oval_entity_free(entity->entity);
			entity->entity = NULL;
		}
		break;
	case OVAL_OBJECTCONTENT_SET:{
			struct oval_object_content_SET *set =
			    (oval_object_content_SET_t *) content;
			if (set->set != NULL)
				oval_set_free(set->set);
			set->set = NULL;
		}
		break;
	case OVAL_OBJECTCONTENT_UNKNOWN: break;
	}
	free(content);
}
void oval_object_content_set_type(struct oval_object_content *content, oval_object_content_type_t type)
{
	content->type = type;
}
void oval_object_content_set_field_name(struct oval_object_content *content, char *name)
{
	if(content->fieldName!=NULL)free(content->fieldName);
	content->fieldName = name==NULL?NULL:strdup(name);
}
void oval_object_content_set_entity(struct oval_object_content *content, struct oval_entity *entity)//type == OVAL_OBJECTCONTENT_ENTITY
{
	if(content->type == OVAL_OBJECTCONTENT_ENTITY){
		oval_object_content_ENTITY_t *content_ENTITY = (oval_object_content_ENTITY_t *)content;
		content_ENTITY->entity = entity;
	}
}
void oval_object_content_set_varCheck(struct oval_object_content *content, oval_check_t check)//type == OVAL_OBJECTCONTENT_ENTITY
{
	if(content->type == OVAL_OBJECTCONTENT_ENTITY){
		oval_object_content_ENTITY_t *content_ENTITY = (oval_object_content_ENTITY_t *)content;
		content_ENTITY->varCheck = check;
	}
}
void oval_object_content_set_setobject(struct oval_object_content *content, struct oval_setobject *set)//type == OVAL_OBJECTCONTENT_SET
{
	if(content->type == OVAL_OBJECTCONTENT_SET){
		oval_object_content_SET_t *content_SET = (oval_object_content_SET_t *)content;
		content_SET->set = set;
	}
}

//typedef void (*oval_object_content_consumer)(struct oval_object_content*,void*);
static void oval_consume_entity(struct oval_entity *entity,
			void *content_entity) {
	((struct oval_object_content_ENTITY *)content_entity)->entity = entity;
}
static void oval_consume_set(struct oval_setobject *set, void *content_set) {
	((struct oval_object_content_SET *)content_set)->set = set;
}
int oval_object_content_parse_tag(xmlTextReaderPtr reader,
				  struct oval_parser_context *context,
				  oval_object_content_consumer consumer,
				  void *user)
{
	char *tagname = (char*) xmlTextReaderName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);

	oval_object_content_type_t type =
	    (strcmp(tagname, "set") ==
	     0) ? OVAL_OBJECTCONTENT_SET : OVAL_OBJECTCONTENT_ENTITY;
	struct oval_object_content *content = oval_object_content_new(type);
	content->fieldName = tagname;
	int return_code = 0;
	switch (type) {
	case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_object_content_ENTITY *content_entity =
			    (struct oval_object_content_ENTITY *)content;
			return_code =
			    oval_entity_parse_tag(reader, context,
						  (oscap_consumer_func)oval_consume_entity, content_entity);
			content_entity->varCheck =
			    oval_check_parse(reader, "var_check",
					     OVAL_CHECK_ALL);
		};
		break;
	case OVAL_OBJECTCONTENT_SET:{
			struct oval_object_content_SET *content_set =
			    (struct oval_object_content_SET *)content;
			return_code =
			    oval_set_parse_tag(reader, context, &oval_consume_set,
					       content_set);
		};
		break;
	case OVAL_OBJECTCONTENT_UNKNOWN: break;
	}
	(*consumer) (content, user);
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: oval_object_content_parse_tag::parse of <%s> terminated on error line %d\n",
		     tagname, line);
	}
	free(namespace);
	return return_code;
}

void oval_object_content_to_print(struct oval_object_content *content,
				  char *indent, int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sCONTENT.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sCONTENT[%d].", indent, idx);

	printf("%sFIELD     = %s\n", nxtindent,
	       oval_object_content_get_field_name(content));
	printf("%sTYPE      = %d\n", nxtindent,
	       oval_object_content_get_type(content));
	switch (oval_object_content_get_type(content)) {
	case OVAL_OBJECTCONTENT_ENTITY:{
			printf("%sVAR_CHECK = %d\n", nxtindent,
			       oval_object_content_get_varCheck(content));
			struct oval_entity *entity =
			    oval_object_content_get_entity(content);
			if (entity == NULL)
				printf("%sENTITY    <<NOT SET>>\n", nxtindent);
			else
				oval_entity_to_print(entity, nxtindent, 0);
		}
		break;
	case OVAL_OBJECTCONTENT_SET:{
			struct oval_setobject *set = oval_object_content_get_setobject(content);
			oval_set_to_print(set, nxtindent, 0);
		} break;
	case OVAL_OBJECTCONTENT_UNKNOWN: break;
	}
}

xmlNode *oval_object_content_to_dom
	(struct oval_object_content *content, xmlDoc *doc, xmlNode *parent)
{
	xmlNode *content_node;
	switch(oval_object_content_get_type(content))
	{
	case OVAL_OBJECTCONTENT_ENTITY:{
		struct oval_entity *entity = oval_object_content_get_entity(content);
		content_node = oval_entity_to_dom(entity, doc, parent);
		oval_check_t check = oval_object_content_get_varCheck(content);
		if(check!=OVAL_CHECK_ALL)
			xmlNewProp(content_node, "var_check", oval_check_get_text(check));
	}break;
	case OVAL_OBJECTCONTENT_SET:{
		struct oval_setobject *set = oval_object_content_get_setobject(content);
		content_node = oval_set_to_dom(set, doc, parent);
	}break;
	default: content_node = NULL;
	}

	return content_node;
}

