/**
 * @file oval_set.c
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

typedef struct oval_set {
	oval_set_type_enum type;
	oval_set_operation_enum operation;
	void *extension;
} oval_set_t;

typedef struct oval_set_AGGREGATE {
	struct oval_collection *subsets;	//type==OVAL_SET_AGGREGATE;
} oval_set_AGGREGATE_t;

typedef struct oval_set_COLLECTIVE {
	struct oval_collection *objects;	//type==OVAL_SET_COLLECTIVE;
	struct oval_collection *filters;	//type==OVAL_SET_COLLECTIVE;
} oval_set_COLLECTIVE_t;

int oval_iterator_set_has_more(struct oval_iterator_set *oc_set)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_set);
}

struct oval_set *oval_iterator_set_next(struct oval_iterator_set *oc_set)
{
	return (struct oval_set *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_set);
}

oval_set_type_enum oval_set_type(struct oval_set *set)
{
	return (set)->type;
}

oval_set_operation_enum oval_set_operation(struct oval_set * set)
{
	return ((struct oval_set *)set)->operation;
}

struct oval_iterator_set *oval_set_subsets(struct oval_set *set)
{
	//type==OVAL_SET_AGGREGATE;
	struct oval_set_AGGREGATE *aggregate =
	    (struct oval_set_AGGREGATE *)set->extension;
	return (struct oval_iterator_set *)oval_collection_iterator(aggregate->
								    subsets);
}

struct oval_iterator_object *oval_set_objects(struct oval_set *set)
{
	//type==OVAL_SET_COLLECTIVE;
	struct oval_set_COLLECTIVE *collective =
	    (struct oval_set_COLLECTIVE *)set->extension;
	return (struct oval_iterator_object *)
	    oval_collection_iterator(collective->objects);
}

struct oval_iterator_state *oval_set_filters(struct oval_set *set)
{
	//type==OVAL_SET_COLLECTIVE;
	struct oval_set_COLLECTIVE *collective =
	    (struct oval_set_COLLECTIVE *)set->extension;
	return (struct oval_iterator_state *)
	    oval_collection_iterator(collective->filters);
}

struct oval_set *oval_set_new()
{
	oval_set_t *set = (oval_set_t *) malloc(sizeof(oval_set_t));
	set->operation = OVAL_SET_OPERATION_UNKNOWN;
	set->type = OVAL_SET_UNKNOWN;
	set->extension = NULL;
	return set;
}

void oval_set_free(struct oval_set *set)
{
	switch (set->type) {
	case OVAL_SET_AGGREGATE:{
			oval_set_AGGREGATE_t *aggregate =
			    (oval_set_AGGREGATE_t *) set->extension;
			oval_collection_free_items(aggregate->subsets,
						   (oscap_destruct_func)oval_set_free);
			aggregate->subsets = NULL;
 			free(set->extension);
			set->extension = NULL;
		} break;
	case OVAL_SET_COLLECTIVE:{
			oval_set_COLLECTIVE_t *collective =
			    (oval_set_COLLECTIVE_t *) set->extension;
			//States and objects are shared and should not be deleted here.
			oval_collection_free_items(collective->filters,NULL);
			oval_collection_free_items(collective->objects,NULL);
			collective->filters = NULL;
			collective->objects = NULL;
			free(set->extension);
			set->extension = NULL;
		} break;
	case OVAL_SET_UNKNOWN: break;
	}
	free(set);
}

void set_oval_set_type(struct oval_set *set, oval_set_type_enum type)
{
	set->type = type;
	switch (type) {
	case OVAL_SET_AGGREGATE:{
			oval_set_AGGREGATE_t *aggregate =
			    (oval_set_AGGREGATE_t *) (set->extension =
						      malloc(sizeof
							     (oval_set_AGGREGATE_t)));
			aggregate->subsets = oval_collection_new();
		}
		break;
	case OVAL_SET_COLLECTIVE:{
			oval_set_COLLECTIVE_t *collective =
			    (oval_set_COLLECTIVE_t *) (set->extension =
						       malloc(sizeof
							      (oval_set_COLLECTIVE_t)));
			collective->filters = oval_collection_new();
			collective->objects = oval_collection_new();
		}
		break;
	case OVAL_SET_UNKNOWN: break;
	}
}

void set_oval_set_operation(struct oval_set *set,
			    oval_set_operation_enum operation)
{
	set->operation = operation;
}

void add_oval_set_subsets(struct oval_set *set, struct oval_set *subset)
{
	oval_set_AGGREGATE_t *aggregate =
	    (oval_set_AGGREGATE_t *) set->extension;
	oval_collection_add(aggregate->subsets, (void *)subset);
}

void add_oval_set_objects(struct oval_set *set, struct oval_object *object)
{
	oval_set_COLLECTIVE_t *collective =
	    (oval_set_COLLECTIVE_t *) set->extension;
	oval_collection_add(collective->objects, (void *)object);
}

void add_oval_set_filters(struct oval_set *set, struct oval_state *filter)
{
	oval_set_COLLECTIVE_t *collective =
	    (oval_set_COLLECTIVE_t *) set->extension;
	oval_collection_add(collective->filters, (void *)filter);
}

//typedef int (*oval_xml_tag_parser)(xmlTextReaderPtr, struct oval_parser_context*, void*);
void oval_set_consume(struct oval_set *subset, void *set) {
	add_oval_set_subsets(set, subset);
}
struct oval_set_context {
	struct oval_parser_context *context;
	struct oval_set *set;
};
void oval_consume_object_ref(char *objref, void *user) {
	struct oval_set_context *ctx = user;
	struct oval_object_model *model =
		oval_parser_context_model(ctx->context);
	struct oval_object *object =
		get_oval_object_new(model, objref);
	add_oval_set_objects(ctx->set, object);
}
void oval_consume_state_ref(char *steref, void *user) {
	struct oval_set_context *ctx = user;
	struct oval_object_model *model =
		oval_parser_context_model(ctx->context);
	struct oval_state *state =
		get_oval_state_new(model, steref);
	add_oval_set_filters(ctx->set, state);
}

int _oval_set_parse_tag(xmlTextReaderPtr reader,
			struct oval_parser_context *context, void *user)
{
	struct oval_set *set = (struct oval_set *)user;
	char *tagname = (char*) xmlTextReaderName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	struct oval_set_context ctx = { .context = context, .set = set };

	int return_code = 0;

	if (strcmp(tagname, "set") == 0) {
		if (set->type == OVAL_SET_UNKNOWN) {
			set_oval_set_type(set, OVAL_SET_AGGREGATE);
		}
		return_code =
		    oval_set_parse_tag(reader, context, &oval_set_consume, set);
	} else {
		if (set->type == OVAL_SET_UNKNOWN) {
			set_oval_set_type(set, OVAL_SET_COLLECTIVE);
		}
		if (strcmp(tagname, "object_reference") == 0) {
			return_code =
			    oval_parser_text_value(reader, context,
						   &oval_consume_object_ref, &ctx);
		} else if (strcmp(tagname, "filter") == 0) {
			return_code =
			    oval_parser_text_value(reader, context,
						   &oval_consume_state_ref, &ctx);
		} else {
			int line = xmlTextReaderGetParserLineNumber(reader);
			printf
			    ("NOTICE: oval_set_parse_tag::unhandled component <%s> %d\n",
			     tagname, line);
			return_code = oval_parser_skip_tag(reader, context);
		}
	}
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: oval_set_parse_tag::parse of <%s> terminated on error line %d\n",
		     tagname, line);
	}
	free(tagname);
	free(namespace);
	return return_code;
}

//typedef void (*oval_set_consumer)(struct oval_set*,void*);
int oval_set_parse_tag(xmlTextReaderPtr reader,
		       struct oval_parser_context *context,
		       oval_set_consumer consumer, void *user)
{
	xmlChar *tagname = xmlTextReaderName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	struct oval_set *set = oval_set_new();

	oval_set_operation_enum operation =
	    oval_set_operation_parse(reader, "set_operator",
				     OVAL_SET_OPERATION_UNION);
	set_oval_set_operation(set, operation);

	(*consumer) (set, user);

	int return_code =
	    oval_parser_parse_tag(reader, context, &_oval_set_parse_tag, set);

	free(tagname);
	free(namespace);
	return return_code;
}

void oval_set_to_print(struct oval_set *set, char *indent, int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sSET.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sSET[%d].", indent, idx);


	printf("%sOPERATOR    = %d\n", nxtindent, oval_set_operation(set));
	printf("%sTYPE        = %d\n", nxtindent, oval_set_type(set));

	switch (oval_set_type(set)) {
	case OVAL_SET_AGGREGATE:{
			struct oval_iterator_set *subsets =
			    oval_set_subsets(set);
			int i;
			for (i = 1; oval_iterator_set_has_more(subsets); i++) {
				struct oval_set *subset =
				    oval_iterator_set_next(subsets);
				oval_set_to_print(subset, nxtindent, i);
			}
		} break;
	case OVAL_SET_COLLECTIVE:{
			struct oval_iterator_object *objects =
			    oval_set_objects(set);
			int i;
			for (i = 1; oval_iterator_object_has_more(objects); i++) {
				struct oval_object *object =
				    oval_iterator_object_next(objects);
				oval_object_to_print(object, nxtindent, i);
			}
			struct oval_iterator_state *states =
			    oval_set_filters(set);
			for (i = 1; oval_iterator_state_has_more(states); i++) {
				struct oval_state *state =
				    oval_iterator_state_next(states);
				oval_state_to_print(state, nxtindent, i);
			}
		} break;
	case OVAL_SET_UNKNOWN: break;
	}
}
