/**
 * @file oval_state.c
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

typedef struct oval_state {
	struct oval_definition_model *model;
	oval_subtype_t subtype;
	char *comment;
	char *id;
	int deprecated;
	int version;
	oval_operator_t operator;
	struct oval_collection *notes;
	struct oval_collection *contents;
} oval_state_t;

bool oval_state_iterator_has_more(struct oval_state_iterator *oc_state)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_state);
}

struct oval_state *oval_state_iterator_next(struct oval_state_iterator
					    *oc_state)
{
	return (struct oval_state *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_state);
}

void oval_state_iterator_free(struct oval_state_iterator
			      *oc_state)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_state);
}

oval_family_t oval_state_get_family(struct oval_state *state)
{
	return (oval_state_get_subtype(state) / 1000) * 1000;
}

oval_subtype_t oval_state_get_subtype(struct oval_state * state)
{
	__attribute__nonnull__(state);

	return ((struct oval_state *)state)->subtype;
}

const char *oval_state_get_name(struct oval_state *state)
{
	return oval_subtype_get_text(oval_state_get_subtype(state));
}

struct oval_string_iterator *oval_state_get_notes(struct oval_state *state)
{
	__attribute__nonnull__(state);

	return (struct oval_string_iterator *)oval_collection_iterator(state->notes);
}

struct oval_state_content_iterator *oval_state_get_contents(struct oval_state *state)
{
	__attribute__nonnull__(state);

	return (struct oval_state_content_iterator *)
	    oval_collection_iterator(state->contents);
}

char *oval_state_get_comment(struct oval_state *state)
{
	__attribute__nonnull__(state);

	return ((struct oval_state *)state)->comment;
}

char *oval_state_get_id(struct oval_state *state)
{
	__attribute__nonnull__(state);

	return ((struct oval_state *)state)->id;
}

bool oval_state_get_deprecated(struct oval_state *state)
{
	__attribute__nonnull__(state);

	return ((struct oval_state *)state)->deprecated;
}

int oval_state_get_version(struct oval_state *state)
{
	__attribute__nonnull__(state);

	return state->version;
}

int oval_state_get_operator(struct oval_state *state)
{
	__attribute__nonnull__(state);

	return state->operator;
}

struct oval_state *oval_state_new(struct oval_definition_model *model, const char *id)
{
	__attribute__nonnull__(model);
	oval_state_t *state;

	state = (oval_state_t *) malloc(sizeof(oval_state_t));
	if (state == NULL)
		return NULL;

	state->deprecated = 0;
	state->version = 0;
	state->operator = OVAL_OPERATOR_UNKNOWN;
	state->subtype = OVAL_SUBTYPE_UNKNOWN;
	state->comment = NULL;
	state->id = oscap_strdup(id);
	state->notes = oval_collection_new();
	state->contents = oval_collection_new();
	state->model = model;

	oval_definition_model_add_state(model, state);

	return state;
}

struct oval_state *oval_state_clone(struct oval_definition_model *new_model, struct oval_state *old_state) {
	__attribute__nonnull__(old_state);

	oval_state_t *new_state = oval_definition_model_get_state(new_model, old_state->id);
	if (new_state == NULL) {
		new_state = oval_state_new(new_model, old_state->id);
		oval_state_set_deprecated(new_state, old_state->deprecated);
		oval_state_set_version(new_state, old_state->version);
		oval_state_set_operator(new_state, old_state->operator);
		oval_state_set_subtype(new_state, old_state->subtype);
		oval_state_set_comment(new_state, old_state->comment);

		struct oval_string_iterator *notes = oval_state_get_notes(old_state);
		while (oval_string_iterator_has_more(notes)) {
			char *note = oval_string_iterator_next(notes);
			oval_state_add_note(new_state, note);
		}
		oval_string_iterator_free(notes);
		struct oval_state_content_iterator *contents = oval_state_get_contents(old_state);
		while (oval_state_content_iterator_has_more(contents)) {
			struct oval_state_content *content = oval_state_content_iterator_next(contents);
			oval_state_add_content(new_state, oval_state_content_clone(new_model, content));
		}
		oval_state_content_iterator_free(contents);
	}
	return new_state;
}

void oval_state_free(struct oval_state *state)
{
	__attribute__nonnull__(state);

	if (state->comment != NULL)
		free(state->comment);
	if (state->id != NULL)
		free(state->id);
	oval_collection_free_items(state->notes, &free);
	oval_collection_free_items(state->contents, (oscap_destruct_func) oval_state_content_free);

	state->comment = NULL;
	state->contents = NULL;
	state->id = NULL;
	state->notes = NULL;
	free(state);
}

void oval_state_set_subtype(struct oval_state *state, oval_subtype_t subtype)
{
	__attribute__nonnull__(state);
	state->subtype = subtype;
}

void oval_state_add_note(struct oval_state *state, char *notes)
{
	__attribute__nonnull__(state);
	oval_collection_add(state->notes, (void *)oscap_strdup(notes));
}

void oval_state_set_comment(struct oval_state *state, char *comm)
{
	__attribute__nonnull__(state);
	if (state->comment != NULL)
		free(state->comment);
	state->comment = comm == NULL ? NULL : oscap_strdup(comm);
}

void oval_state_set_deprecated(struct oval_state *state, bool deprecated)
{
	__attribute__nonnull__(state);
	state->deprecated = deprecated;
}

void oval_state_set_version(struct oval_state *state, int version)
{
	__attribute__nonnull__(state);
	state->version = version;
}

void oval_state_set_operator(struct oval_state *state, oval_operator_t operator)
{
	__attribute__nonnull__(state);
	state->operator = operator;
}

void oval_state_add_content(struct oval_state *state, struct oval_state_content *content) 
{
	__attribute__nonnull__(state);
	oval_collection_add(state->contents, content);
}

oval_version_t oval_state_get_schema_version(const struct oval_state *state)
{
	__attribute__nonnull__(state);

	if (state->model == NULL)
		return OVAL_VERSION_INVALID;
	return oval_definition_model_get_schema_version(state->model);
}

oval_schema_version_t oval_state_get_platform_schema_version(const struct oval_state *state)
{
	oval_family_t family = oval_state_get_family((struct oval_state *)state);
	const char *platform = oval_family_get_text(family);
	return oval_definition_model_get_platform_schema_version(state->model, platform);
}

static void _oval_note_consumer(char *text, void *state)
{
	oval_state_add_note(state, text);
}

static int _oval_state_parse_notes(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_state *state = (struct oval_state *)user;
	return oscap_parser_text_value(reader, _oval_note_consumer, state);
}

static void _oval_state_content_consumer(struct oval_state_content *content, struct oval_state *state) {
	oval_state_add_content(state, content);
}

static int _oval_state_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	__attribute__nonnull__(user);

	struct oval_state *state = (struct oval_state *)user;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code = 0;
	if ((strcmp(tagname, "notes") == 0)) {
		return_code = oval_parser_parse_tag(reader, context, &_oval_state_parse_notes, state);
	} else {
		return_code = oval_state_content_parse_tag
		    (reader, context, (oscap_consumer_func) _oval_state_content_consumer, state);
	}
	free(tagname);
	free(namespace);
	return return_code;
}

int oval_state_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr)
{
	struct oval_definition_model *model = context->definition_model;
	char *id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	struct oval_state *state = oval_definition_model_get_new_state(model, id);
	free(id);
	oval_subtype_t subtype = oval_subtype_parse(reader);
	oval_state_set_subtype(state, subtype);
	char *comm = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	if (comm != NULL) {
		oval_state_set_comment(state, comm);
		free(comm);
		comm = NULL;
	}
	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	oval_state_set_deprecated(state, deprecated);
	char *version = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	oval_state_set_version(state, atoi(version));
	free(version);
	oval_operator_t operator = oval_operator_parse(reader, "operator", OVAL_OPERATOR_AND);
	oval_state_set_operator(state, operator);

	int return_code = oval_parser_parse_tag(reader, context, &_oval_state_parse_tag, state);
	return return_code;
}

xmlNode *oval_state_to_dom(struct oval_state *state, xmlDoc * doc, xmlNode * parent)
{
	/* get state name */
	oval_subtype_t subtype = oval_state_get_subtype(state);
	const char *subtype_text = oval_subtype_get_text(subtype);
	char state_name[strlen(subtype_text) + 7];
	sprintf(state_name, "%s_state", subtype_text);

	oval_family_t family = oval_state_get_family(state);

	/* search namespace & create child */ 
	xmlNs *ns_family = oval_family_to_namespace(family, (const char *) OVAL_DEFINITIONS_NAMESPACE, doc, parent);
	xmlNode *state_node = xmlNewTextChild(parent, ns_family, BAD_CAST state_name, NULL);

	char *id = oval_state_get_id(state);
	xmlNewProp(state_node, BAD_CAST "id", BAD_CAST id);

	char version[10];
	*version = '\0';
	snprintf(version, sizeof(version), "%d", oval_state_get_version(state));
	xmlNewProp(state_node, BAD_CAST "version", BAD_CAST version);

	oval_operator_t operator = oval_state_get_operator(state);
	const char *operator_text = oval_operator_get_text(operator);
	if (operator != OVAL_OPERATOR_AND)
		xmlNewProp(state_node, BAD_CAST "operator", BAD_CAST operator_text);

	char *comm = oval_state_get_comment(state);
	if (comm)
		xmlNewProp(state_node, BAD_CAST "comment", BAD_CAST comm);

	bool deprecated = oval_state_get_deprecated(state);
	if (deprecated)
		xmlNewProp(state_node, BAD_CAST "deprecated", BAD_CAST "true");

	struct oval_string_iterator *notes = oval_state_get_notes(state);
	if (oval_string_iterator_has_more(notes)) {
		xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
		xmlNode *notes_node = xmlNewTextChild(state_node, ns_definitions, BAD_CAST "notes", NULL);
		while (oval_string_iterator_has_more(notes)) {
			char *note = oval_string_iterator_next(notes);
			xmlNewTextChild(notes_node, ns_definitions, BAD_CAST "note", BAD_CAST note);
		}
	}
	oval_string_iterator_free(notes);

	struct oval_state_content_iterator *contents = oval_state_get_contents(state);
	while (oval_state_content_iterator_has_more(contents)) {
		struct oval_state_content *content = oval_state_content_iterator_next(contents);
		oval_state_content_to_dom(content, doc, state_node);
	}
	oval_state_content_iterator_free(contents);
	return state_node;
}
