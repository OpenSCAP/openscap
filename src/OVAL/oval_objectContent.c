/**
 * @file oval_objectContent.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oval_definitions_impl.h"
#include "adt/oval_collection_impl.h"
#include "oval_agent_api_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"

typedef struct oval_object_content {
	struct oval_definition_model *model;
	char *fieldName;
	oval_object_content_type_t type;
} oval_object_content_t;

typedef struct oval_object_content_ENTITY {
	struct oval_definition_model *model;
	char *fieldName;
	oval_object_content_type_t type;
	struct oval_entity *entity;	/*type == OVAL_OBJECTCONTENT_ENTITY */
	oval_check_t varCheck;	/*type == OVAL_OBJECTCONTENT_ENTITY */
} oval_object_content_ENTITY_t;

typedef struct oval_object_content_SET {
	struct oval_definition_model *model;
	char *fieldName;
	oval_object_content_type_t type;
	struct oval_setobject *set;	/*type == OVAL_OBJECTCONTENT_SET */
} oval_object_content_SET_t;

typedef struct oval_object_content_FILTER {
	struct oval_definition_model *model;
	char *fieldName;
	oval_object_content_type_t type;
	struct oval_filter *filter;	/*type == OVAL_OBJECTCONTENT_FILTER */
} oval_object_content_FILTER_t;

bool oval_object_content_iterator_has_more(struct oval_object_content_iterator
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
	__attribute__nonnull__(content);

	return ((struct oval_object_content *)content)->type;
}

struct oval_entity *oval_object_content_get_entity(struct oval_object_content
						   *content)
{
	__attribute__nonnull__(content);

	/*type == OVAL_OBJECTCONTENT_ENTITY */
	struct oval_entity *entity = NULL;
	if (oval_object_content_get_type(content) == OVAL_OBJECTCONTENT_ENTITY) {
		entity = ((struct oval_object_content_ENTITY *)content)->entity;
	}
	return entity;
}

oval_check_t oval_object_content_get_varCheck(struct oval_object_content * content)
{
	__attribute__nonnull__(content);

	/*type == OVAL_OBJECTCONTENT_ENTITY */
	oval_check_t varCheck = OVAL_CHECK_UNKNOWN;
	if (oval_object_content_get_type(content) == OVAL_OBJECTCONTENT_ENTITY) {
		varCheck = ((struct oval_object_content_ENTITY *)content)->varCheck;
	}
	return varCheck;
}

struct oval_setobject *oval_object_content_get_setobject(struct oval_object_content *content)
{
	__attribute__nonnull__(content);

	/*type == OVAL_OBJECTCONTENT_SET */
	struct oval_setobject *set = NULL;
	if (oval_object_content_get_type(content) == OVAL_OBJECTCONTENT_SET) {
		set = ((struct oval_object_content_SET *)content)->set;
	}
	return set;
}

struct oval_filter *oval_object_content_get_filter(struct oval_object_content *content)
{
	__attribute__nonnull__(content);

	/* type == OVAL_OBJECTCONTENT_FILTER */
	struct oval_filter *filter = NULL;
	if (oval_object_content_get_type(content) == OVAL_OBJECTCONTENT_FILTER) {
		filter = ((struct oval_object_content_FILTER *)content)->filter;
	}
	return filter;
}

struct oval_object_content
*oval_object_content_new(struct oval_definition_model *model, oval_object_content_type_t type)
{
	struct oval_object_content *content = NULL;
	switch (type) {
	case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_object_content_ENTITY *entity =
			    (oval_object_content_ENTITY_t *) malloc(sizeof(oval_object_content_ENTITY_t));
			if (entity == NULL)
				return NULL;

			content = (oval_object_content_t *) entity;
			entity->entity = NULL;
			entity->varCheck = OVAL_CHECK_UNKNOWN;
		}
		break;
	case OVAL_OBJECTCONTENT_SET:{
			struct oval_object_content_SET *set =
			    (oval_object_content_SET_t *) malloc(sizeof(oval_object_content_SET_t));
			if (set == NULL)
				return NULL;

			set->set = NULL;
			content = (oval_object_content_t *) set;
		}
		break;
	case OVAL_OBJECTCONTENT_FILTER:{
			struct oval_object_content_FILTER *filter =
			    (oval_object_content_FILTER_t *) malloc(sizeof(oval_object_content_FILTER_t));
			if (filter == NULL)
				return NULL;

			filter->filter = NULL;
			content = (oval_object_content_t *) filter;
		}
		break;
	default:
		dE("Unsupported object content type: %d.", type);
		return NULL;
	}
	content->model = model;
	content->fieldName = NULL;
	content->type = type;
	return content;
}

struct oval_object_content *oval_object_content_clone
    (struct oval_definition_model *new_model, struct oval_object_content *old_content) {
	struct oval_object_content *new_content = oval_object_content_new(new_model, old_content->type);
	char *name = oval_object_content_get_field_name(old_content);
	oval_object_content_set_field_name(new_content, name);
	switch (new_content->type) {
	case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_entity *entity = oval_entity_clone(new_model, oval_object_content_get_entity(old_content));
			oval_object_content_set_entity(new_content, entity);
			oval_check_t check = oval_object_content_get_varCheck(old_content);
			oval_object_content_set_varCheck(new_content, check);
		} break;
	case OVAL_OBJECTCONTENT_SET:{
			struct oval_setobject *set = oval_object_content_get_setobject(old_content);
			oval_object_content_set_setobject(new_content, oval_setobject_clone(new_model, set));
		} break;
	case OVAL_OBJECTCONTENT_FILTER:{
			struct oval_filter *filter = oval_object_content_get_filter(old_content);
			oval_object_content_set_filter(new_content, oval_filter_clone(new_model, filter));
		} break;
	default:
		/*NOOP*/;
	}
	return new_content;
}

void oval_object_content_free(struct oval_object_content *content)
{
	__attribute__nonnull__(content);

	if (content->fieldName != NULL)
		free(content->fieldName);
	content->fieldName = NULL;
	switch (content->type) {
	case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_object_content_ENTITY *entity = (oval_object_content_ENTITY_t *) content;
			if (entity->entity != NULL)
				oval_entity_free(entity->entity);
			entity->entity = NULL;
		}
		break;
	case OVAL_OBJECTCONTENT_SET:{
			struct oval_object_content_SET *set = (oval_object_content_SET_t *) content;
			if (set->set != NULL)
				oval_setobject_free(set->set);
			set->set = NULL;
		}
		break;
	case OVAL_OBJECTCONTENT_FILTER:{
			struct oval_object_content_FILTER *filter = (oval_object_content_FILTER_t *) content;
			if (filter->filter != NULL)
				oval_filter_free(filter->filter);
			filter->filter = NULL;
		}
		break;
	case OVAL_OBJECTCONTENT_UNKNOWN:
		break;
	}
	free(content);
}

void oval_object_content_set_type(struct oval_object_content *content, oval_object_content_type_t type)
{
	__attribute__nonnull__(content);
	content->type = type;
}

void oval_object_content_set_field_name(struct oval_object_content *content, char *name)
{
	__attribute__nonnull__(content);
	if (content->fieldName != NULL)
		free(content->fieldName);
	content->fieldName = (name == NULL) ? NULL : oscap_strdup(name);
}

void oval_object_content_set_entity(struct oval_object_content *content, struct oval_entity *entity)
{				/*type == OVAL_OBJECTCONTENT_ENTITY */
	__attribute__nonnull__(content);
	if (content->type == OVAL_OBJECTCONTENT_ENTITY) {
		oval_object_content_ENTITY_t *content_ENTITY = (oval_object_content_ENTITY_t *) content;
		content_ENTITY->entity = entity;
	}
}

void oval_object_content_set_varCheck(struct oval_object_content *content, oval_check_t check)
{				/*type == OVAL_OBJECTCONTENT_ENTITY */
	__attribute__nonnull__(content);
	if (content->type == OVAL_OBJECTCONTENT_ENTITY) {
		oval_object_content_ENTITY_t *content_ENTITY = (oval_object_content_ENTITY_t *) content;
		content_ENTITY->varCheck = check;
	}
}

void oval_object_content_set_setobject(struct oval_object_content *content, struct oval_setobject *set)
{				/*type == OVAL_OBJECTCONTENT_SET */
	__attribute__nonnull__(content);
	if (content->type == OVAL_OBJECTCONTENT_SET) {
		oval_object_content_SET_t *content_SET = (oval_object_content_SET_t *) content;
		content_SET->set = set;
	}
}

void oval_object_content_set_filter(struct oval_object_content *content, struct oval_filter *filter)
{				/*type == OVAL_OBJECTCONTENT_FILTER */
	__attribute__nonnull__(content);
	if (content->type == OVAL_OBJECTCONTENT_FILTER) {
		oval_object_content_FILTER_t *content_FILTER = (oval_object_content_FILTER_t *) content;
		content_FILTER->filter = filter;
	}
}

/*typedef void (*oval_object_content_consumer)(struct oval_object_content*,void*);*/
static void oval_consume_entity(struct oval_entity *entity, void *content_entity)
{
	__attribute__nonnull__(entity);

	((struct oval_object_content_ENTITY *)content_entity)->entity = entity;
}

static void oval_consume_set(struct oval_setobject *set, void *content_set)
{
	__attribute__nonnull__(content_set);

	((struct oval_object_content_SET *)content_set)->set = set;
}

static void oval_consume_filter(struct oval_filter *filter, void *content_filter)
{
	__attribute__nonnull__(content_filter);

	((struct oval_object_content_FILTER *)content_filter)->filter = filter;
}

/* prevent gcc false warning */
//#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
int oval_object_content_parse_tag(xmlTextReaderPtr reader,
				  struct oval_parser_context *context,
				  oval_object_content_consumer consumer, void *user)
{
	__attribute__nonnull__(context);

	char *tagname = (char *)xmlTextReaderLocalName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	oval_object_content_type_t type = OVAL_OBJECTCONTENT_UNKNOWN;
	struct oval_object_content *content;
	int return_code = 0;

	if (!strcmp(tagname, "set")) {
		type = OVAL_OBJECTCONTENT_SET;
	} else if (!strcmp(tagname, "filter")) {
		type = OVAL_OBJECTCONTENT_FILTER;
	} else {
		type = OVAL_OBJECTCONTENT_ENTITY;
	}

	content = oval_object_content_new(context->definition_model, type);
	if (content == NULL)
		return -1;

	content->fieldName = tagname;
	switch (type) {
	case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_object_content_ENTITY *content_entity =
			    (struct oval_object_content_ENTITY *)content;
			return_code =
			    oval_entity_parse_tag(reader, context,
						  (oscap_consumer_func) oval_consume_entity, content_entity);
			content_entity->varCheck = oval_check_parse(reader, "var_check", OVAL_CHECK_ALL);
		};
		break;
	case OVAL_OBJECTCONTENT_SET:{
			struct oval_object_content_SET *content_set = (struct oval_object_content_SET *)content;
			return_code = oval_set_parse_tag(reader, context, &oval_consume_set, content_set);
		};
		break;
	case OVAL_OBJECTCONTENT_FILTER:{
			struct oval_object_content_FILTER *content_filter =
			    (struct oval_object_content_FILTER *) content;
			return_code = oval_filter_parse_tag(reader, context, &oval_consume_filter, content_filter);
		};
		break;
	}

	(*consumer) (content, user);

	if (return_code != 0)
		dW("Parsing of <%s> terminated by an error at line %d.",tagname, xmlTextReaderGetParserLineNumber(reader));

	free(namespace);
	return return_code;
}
//#pragma GCC diagnostic pop

xmlNode *oval_object_content_to_dom(struct oval_object_content *content, xmlDoc * doc, xmlNode * parent) {
	xmlNode *content_node;
	switch (oval_object_content_get_type(content)) {
	case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_entity *entity = oval_object_content_get_entity(content);
			content_node = oval_entity_to_dom(entity, doc, parent);
			oval_check_t check = oval_object_content_get_varCheck(content);

			if (check != OVAL_CHECK_ALL || xmlHasProp(content_node, BAD_CAST "var_ref"))
				xmlNewProp(content_node, BAD_CAST "var_check", BAD_CAST oval_check_get_text(check));
		}
		break;
	case OVAL_OBJECTCONTENT_SET:{
			struct oval_setobject *set = oval_object_content_get_setobject(content);
			content_node = oval_set_to_dom(set, doc, parent);
		} break;
	case OVAL_OBJECTCONTENT_FILTER:{
			struct oval_filter *filter;

			filter = oval_object_content_get_filter(content);
			content_node = oval_filter_to_dom(filter, doc, parent);
		}
		break;
	default:
		content_node = NULL;
	}

	return content_node;
}
