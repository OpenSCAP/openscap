/**
 * @file oval_set.c
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
#include "common/elements.h"

typedef struct oval_setobject {
	struct oval_definition_model *model;
	oval_setobject_type_t type;
	oval_setobject_operation_t operation;
	void *extension;
} oval_set_t;

typedef struct oval_set_AGGREGATE {
	struct oval_collection *subsets;	/*type==OVAL_SET_AGGREGATE; */
} oval_set_AGGREGATE_t;

typedef struct oval_set_COLLECTIVE {
	struct oval_collection *objects;	//type==OVAL_SET_COLLECTIVE;
	struct oval_collection *filters;	//type==OVAL_SET_COLLECTIVE;
} oval_set_COLLECTIVE_t;

bool oval_setobject_iterator_has_more(struct oval_setobject_iterator *oc_set)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_set);
}

struct oval_setobject *oval_setobject_iterator_next(struct oval_setobject_iterator *oc_set)
{
	return (struct oval_setobject *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_set);
}

void oval_setobject_iterator_free(struct oval_setobject_iterator *oc_set)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_set);
}

oval_setobject_type_t oval_setobject_get_type(struct oval_setobject *set)
{
	__attribute__nonnull__(set);

	return (set)->type;
}

oval_setobject_operation_t oval_setobject_get_operation(struct oval_setobject * set)
{
	__attribute__nonnull__(set);

	return ((struct oval_setobject *)set)->operation;
}

struct oval_setobject_iterator *oval_setobject_get_subsets(struct oval_setobject *set)
{
	__attribute__nonnull__(set);

	struct oval_setobject_iterator *subsets = NULL;
	if (set->type == OVAL_SET_AGGREGATE) {
		struct oval_set_AGGREGATE *aggregate = (struct oval_set_AGGREGATE *)set->extension;
		subsets = (struct oval_setobject_iterator *)
		    oval_collection_iterator(aggregate->subsets);
	}
	return subsets;
}

struct oval_object_iterator *oval_setobject_get_objects(struct oval_setobject *set)
{
	__attribute__nonnull__(set);

	/* type == OVAL_SET_COLLECTIVE; */
	struct oval_object_iterator *objects = NULL;
	if (set->type == OVAL_SET_COLLECTIVE) {
		struct oval_set_COLLECTIVE *collective = (struct oval_set_COLLECTIVE *)set->extension;
		objects = (struct oval_object_iterator *)
		    oval_collection_iterator(collective->objects);
	}
	return objects;
}

struct oval_filter_iterator *oval_setobject_get_filters(struct oval_setobject *set)
{
	__attribute__nonnull__(set);

	/* type == OVAL_SET_COLLECTIVE; */
	struct oval_filter_iterator *filters = NULL;
	if (set->type == OVAL_SET_COLLECTIVE) {
		struct oval_set_COLLECTIVE *collective = (struct oval_set_COLLECTIVE *)set->extension;
		filters = (struct oval_filter_iterator *)
		    oval_collection_iterator(collective->filters);
	}
	return filters;
}

struct oval_setobject *oval_setobject_new(struct oval_definition_model *model)
{
	oval_set_t *set = (oval_set_t *) oscap_alloc(sizeof(oval_set_t));
	if (set == NULL)
		return NULL;

	set->operation = OVAL_SET_OPERATION_UNKNOWN;
	set->type = OVAL_SET_UNKNOWN;
	set->extension = NULL;
	set->model = model;
	return set;
}

struct oval_setobject *oval_setobject_clone
    (struct oval_definition_model *new_model, struct oval_setobject *old_setobject) {
	struct oval_setobject *new_setobject = oval_setobject_new(new_model);
	oval_setobject_type_t type = oval_setobject_get_type(old_setobject);
	oval_setobject_set_type(new_setobject, type);
	oval_setobject_operation_t operation = oval_setobject_get_operation(old_setobject);
	oval_setobject_set_operation(new_setobject, operation);
	switch (type) {
	case OVAL_SET_COLLECTIVE:{
			struct oval_filter_iterator *filters = oval_setobject_get_filters(old_setobject);
			while (oval_filter_iterator_has_more(filters)) {
				struct oval_filter *filter = oval_filter_iterator_next(filters);
				oval_setobject_add_filter(new_setobject, oval_filter_clone(new_model, filter));
			}
			oval_filter_iterator_free(filters);
			struct oval_object_iterator *objects = oval_setobject_get_objects(old_setobject);
			while (oval_object_iterator_has_more(objects)) {
				struct oval_object *object = oval_object_iterator_next(objects);
				oval_setobject_add_object(new_setobject, oval_object_clone(new_model, object));
			}
			oval_object_iterator_free(objects);

		} break;
	case OVAL_SET_AGGREGATE:{
			struct oval_setobject_iterator *subsets = oval_setobject_get_subsets(old_setobject);
			while (oval_setobject_iterator_has_more(subsets)) {
				struct oval_setobject *subset = oval_setobject_iterator_next(subsets);
				oval_setobject_add_subset(new_setobject, oval_setobject_clone(new_model, subset));
			}
			oval_setobject_iterator_free(subsets);
		} break;
	default:
		/*NOOP*/;
	}
	return new_setobject;
}

void oval_setobject_free(struct oval_setobject *set)
{
	__attribute__nonnull__(set);

	switch (set->type) {
	case OVAL_SET_AGGREGATE:{
			oval_set_AGGREGATE_t *aggregate = (oval_set_AGGREGATE_t *) set->extension;
			oval_collection_free_items(aggregate->subsets, (oscap_destruct_func) oval_setobject_free);
			aggregate->subsets = NULL;
			oscap_free(set->extension);
			set->extension = NULL;
		}
		break;
	case OVAL_SET_COLLECTIVE:{
			oval_set_COLLECTIVE_t *collective = (oval_set_COLLECTIVE_t *) set->extension;
			oval_collection_free_items(collective->filters, (oscap_destruct_func) oval_filter_free);
			//Objects (and states in filters) are shared and should not be deleted here.
			oval_collection_free_items(collective->objects, NULL);
			collective->filters = NULL;
			collective->objects = NULL;
			oscap_free(set->extension);
			set->extension = NULL;
		}
		break;
	case OVAL_SET_UNKNOWN:
		break;
	}
	oscap_free(set);
}

void oval_setobject_set_type(struct oval_setobject *set, oval_setobject_type_t type)
{
	__attribute__nonnull__(set);

	set->type = type;
	switch (type) {
	case OVAL_SET_AGGREGATE:{
			oval_set_AGGREGATE_t *aggregate =
			    (oval_set_AGGREGATE_t *) (set->extension =
						      oscap_alloc(sizeof(oval_set_AGGREGATE_t)));
			aggregate->subsets = oval_collection_new();
		}
		break;
	case OVAL_SET_COLLECTIVE:{
			oval_set_COLLECTIVE_t *collective =
			    (oval_set_COLLECTIVE_t *) (set->extension =
						       oscap_alloc(sizeof(oval_set_COLLECTIVE_t)));
			collective->filters = oval_collection_new();
			collective->objects = oval_collection_new();
		}
		break;
	case OVAL_SET_UNKNOWN:
		break;
	}
}

void oval_setobject_set_operation(struct oval_setobject *set, oval_setobject_operation_t operation)
{
	__attribute__nonnull__(set);
	set->operation = operation;
}

void oval_setobject_add_subset(struct oval_setobject *set, struct oval_setobject *subset)
{
	__attribute__nonnull__(set);
	oval_set_AGGREGATE_t *aggregate = (oval_set_AGGREGATE_t *) set->extension;
	assert(aggregate != NULL);
	oval_collection_add(aggregate->subsets, (void *)subset);
}

void oval_setobject_add_object(struct oval_setobject *set, struct oval_object *object)
{
	__attribute__nonnull__(set);
	oval_set_COLLECTIVE_t *collective = (oval_set_COLLECTIVE_t *) set->extension;
	assert(collective != NULL);
	oval_collection_add(collective->objects, (void *)object);
}

void oval_setobject_add_filter(struct oval_setobject *set, struct oval_filter *filter)
{
	__attribute__nonnull__(set);
	oval_set_COLLECTIVE_t *collective = (oval_set_COLLECTIVE_t *) set->extension;
	assert(collective != NULL);
	oval_collection_add(collective->filters, (void *)filter);
}

//typedef int (*oval_xml_tag_parser)(xmlTextReaderPtr, struct oval_parser_context*, void*);
static void oval_set_consume(struct oval_setobject *subset, void *set)
{
	oval_setobject_add_subset(set, subset);
}

struct oval_set_context {
	struct oval_parser_context *context;
	struct oval_setobject *set;
};
static void oval_consume_object_ref(char *objref, void *user)
{

	__attribute__nonnull__(user);

	struct oval_set_context *ctx = user;
	struct oval_definition_model *model = ctx->context->definition_model;
	struct oval_object *object = oval_definition_model_get_new_object(model, objref);
	oval_setobject_add_object(ctx->set, object);
}

static void oval_set_consume_filter(struct oval_filter *filter, void *set)
{
	oval_setobject_add_filter(set, filter);
}

static int _oval_set_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	__attribute__nonnull__(user);

	struct oval_setobject *set = (struct oval_setobject *)user;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	struct oval_set_context ctx = {.context = context,.set = set };

	int return_code = 0;

	if (strcmp(tagname, "set") == 0) {
		if (set->type == OVAL_SET_UNKNOWN) {
			oval_setobject_set_type(set, OVAL_SET_AGGREGATE);
		}
		return_code = oval_set_parse_tag(reader, context, &oval_set_consume, set);
	} else {
		if (set->type == OVAL_SET_UNKNOWN) {
			oval_setobject_set_type(set, OVAL_SET_COLLECTIVE);
		}
		if (strcmp(tagname, "object_reference") == 0) {
			return_code = oscap_parser_text_value(reader, &oval_consume_object_ref, &ctx);
		} else if (strcmp(tagname, "filter") == 0) {
			return_code = oval_filter_parse_tag(reader, context, &oval_set_consume_filter, set);
		} else {
			dW("Unknown tag: <%s>, line: %d.", tagname,
                                      xmlTextReaderGetParserLineNumber(reader));
			return_code = oval_parser_skip_tag(reader, context);
		}
	}

	if (return_code != 0) {
		dW("Parsing of <%s> terminated by an error at line %d.", tagname, xmlTextReaderGetParserLineNumber(reader));
	}

	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

//typedef void (*oval_set_consumer)(struct oval_set*,void*);
int oval_set_parse_tag(xmlTextReaderPtr reader,
		       struct oval_parser_context *context, oval_set_consumer consumer, void *user)
{
	__attribute__nonnull__(context);

	xmlChar *tagname = xmlTextReaderLocalName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	struct oval_setobject *set = oval_setobject_new(context->definition_model);

	oval_setobject_operation_t operation = oval_set_operation_parse(reader, "set_operator",
									OVAL_SET_OPERATION_UNION);
	oval_setobject_set_operation(set, operation);

	(*consumer) (set, user);

	int return_code = oval_parser_parse_tag(reader, context, &_oval_set_parse_tag, set);

	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

xmlNode *oval_set_to_dom(struct oval_setobject *set, xmlDoc * doc, xmlNode * parent) {
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *set_node = xmlNewTextChild(parent, ns_definitions, BAD_CAST "set", NULL);

	oval_setobject_operation_t operation = oval_setobject_get_operation(set);
	if (operation != OVAL_SET_OPERATION_UNION)
		xmlNewProp(set_node, BAD_CAST "set_operator", BAD_CAST oval_set_operation_get_text(operation));

	switch (oval_setobject_get_type(set)) {
	case OVAL_SET_AGGREGATE:{
			struct oval_setobject_iterator *subsets = oval_setobject_get_subsets(set);
			while (oval_setobject_iterator_has_more(subsets)) {
				struct oval_setobject *subset = oval_setobject_iterator_next(subsets);
				oval_set_to_dom(subset, doc, set_node);
			}
			oval_setobject_iterator_free(subsets);
		} break;
	case OVAL_SET_COLLECTIVE:{
			struct oval_object_iterator *objects = oval_setobject_get_objects(set);
			while (oval_object_iterator_has_more(objects)) {
				struct oval_object *object = oval_object_iterator_next(objects);
				if (oval_object_get_base_obj(object))
					/* Skip internal objects */
					object = oval_object_get_base_obj(object);
				char *id = oval_object_get_id(object);
				xmlNewTextChild(set_node, ns_definitions, BAD_CAST "object_reference", BAD_CAST id);
			}
			oval_object_iterator_free(objects);

			struct oval_filter_iterator *filters = oval_setobject_get_filters(set);
			while (oval_filter_iterator_has_more(filters)) {
				struct oval_filter *filter;

				filter = oval_filter_iterator_next(filters);
				oval_filter_to_dom(filter, doc, set_node);
			}
			oval_filter_iterator_free(filters);
		} break;
	default:
		break;
	}

	return set_node;
}

void oval_set_propagate_filters(struct oval_definition_model *model, struct oval_setobject *set, char *set_id)
{
	struct oval_object_iterator *obj_itr;
	struct oval_collection *new_objects;
	struct oval_set_COLLECTIVE *ext_col;

	new_objects = oval_collection_new();

	obj_itr = oval_setobject_get_objects(set);
	while (oval_object_iterator_has_more(obj_itr)) {
		struct oval_object *obj, *new_obj;
		struct oval_object_content_iterator *cont_itr;
		struct oval_object_content *cont;
		struct oval_filter_iterator *filter_itr;

		obj = oval_object_iterator_next(obj_itr);
		cont_itr = oval_object_get_object_contents(obj);
		if (!oval_object_content_iterator_has_more(cont_itr)) {
			oval_object_content_iterator_free(cont_itr);
			oval_collection_add(new_objects, obj);
			continue;
		}

		cont = oval_object_content_iterator_next(cont_itr);
		oval_object_content_iterator_free(cont_itr);
		if (oval_object_content_get_type(cont) == OVAL_OBJECTCONTENT_SET) {
			oval_collection_add(new_objects, obj);
			continue;
		}

		new_obj = oval_object_create_internal(obj, set_id);

		filter_itr = oval_setobject_get_filters(set);
		while (oval_filter_iterator_has_more(filter_itr)) {
			struct oval_filter *filter, *new_filter;
			struct oval_state *ste;
			oval_filter_action_t act;
			struct oval_object_content *content;

			filter = oval_filter_iterator_next(filter_itr);
			ste = oval_filter_get_state(filter);
			act = oval_filter_get_filter_action(filter);
			new_filter = oval_filter_new(model);
			oval_filter_set_state(new_filter, ste);
			oval_filter_set_filter_action(new_filter, act);
			content = oval_object_content_new(model, OVAL_OBJECTCONTENT_FILTER);
			oval_object_content_set_filter(content, new_filter);
			oval_object_add_object_content(new_obj, content);
		}
		oval_filter_iterator_free(filter_itr);

		oval_collection_add(new_objects, new_obj);
	}
	oval_object_iterator_free(obj_itr);

	ext_col = (struct oval_set_COLLECTIVE *) set->extension;
	oval_collection_free_items(ext_col->objects, NULL);
	ext_col->objects = new_objects;
}
