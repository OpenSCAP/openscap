/**
 * @file oval_test.c
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

#include "public/oval_types.h"
#include "oval_definitions_impl.h"
#include "adt/oval_collection_impl.h"
#include "oval_agent_api_impl.h"
#include "common/oscap_string.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/elements.h"
#include "common/_error.h"

typedef struct oval_test {
	struct oval_definition_model *model;
	oval_subtype_t subtype;
	struct oval_collection *notes;
	char *comment;
	char *id;
	int deprecated;
	int version;
	oval_existence_t existence;
	oval_check_t check;
	oval_operator_t state_operator;
	struct oval_object *object;
	struct oval_collection *states;
} oval_test_t;

bool oval_test_iterator_has_more(struct oval_test_iterator *oc_test)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_test);
}

struct oval_test *oval_test_iterator_next(struct oval_test_iterator *oc_test)
{
	return (struct oval_test *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_test);
}

void oval_test_iterator_free(struct oval_test_iterator *oc_test)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_test);
}

oval_family_t oval_test_get_family(struct oval_test *test)
{
	__attribute__nonnull__(test);

	return ((test->subtype) / 1000) * 1000;
}

oval_subtype_t oval_test_get_subtype(struct oval_test * test)
{
	__attribute__nonnull__(test);

	return test->subtype;
}

struct oval_string_iterator *oval_test_get_notes(struct oval_test *test)
{
	__attribute__nonnull__(test);

	return (struct oval_string_iterator *)oval_collection_iterator(test->notes);
}

char *oval_test_get_comment(struct oval_test *test)
{
	__attribute__nonnull__(test);

	return test->comment;
}

char *oval_test_get_id(struct oval_test *test)
{
	__attribute__nonnull__(test);

	return test->id;
}

bool oval_test_get_deprecated(struct oval_test * test)
{
	__attribute__nonnull__(test);

	return test->deprecated;
}

int oval_test_get_version(struct oval_test *test)
{
	__attribute__nonnull__(test);

	return test->version;
}

oval_existence_t oval_test_get_existence(struct oval_test * test)
{
	__attribute__nonnull__(test);

	return test->existence;
}

oval_check_t oval_test_get_check(struct oval_test * test)
{
	__attribute__nonnull__(test);

	return test->check;
}

oval_operator_t oval_test_get_state_operator(struct oval_test *test)
{
	__attribute__nonnull__(test);

	return test->state_operator;
}

struct oval_object *oval_test_get_object(struct oval_test *test)
{
	__attribute__nonnull__(test);

	return test->object;
}

struct oval_state_iterator *oval_test_get_states(struct oval_test *test)
{
	__attribute__nonnull__(test);

	return (struct oval_state_iterator *) oval_collection_iterator(test->states);
}

char *oval_test_get_state_names(struct oval_test *test)
{
	__attribute__nonnull__(test);

	struct oval_state_iterator *ste_itr = oval_test_get_states(test);
	if (!oval_state_iterator_has_more(ste_itr)) {
		oval_state_iterator_free(ste_itr);
		return NULL;
	}
	struct oscap_string *state_list = oscap_string_new();
	oscap_string_append_char(state_list, '\'');
	while (1) {
		struct oval_state *ste = oval_state_iterator_next(ste_itr);
		const char *ste_id = oval_state_get_id(ste);
		oscap_string_append_string(state_list, ste_id);
		if (!oval_state_iterator_has_more(ste_itr)) {
			break;
		}
		oscap_string_append_string(state_list, "', '");
	}
	oscap_string_append_char(state_list, '\'');
	char *state_names = oscap_strdup(oscap_string_get_cstr(state_list));
	oscap_string_free(state_list);
	oval_state_iterator_free(ste_itr);
	return state_names;
}

struct oval_test *oval_test_new(struct oval_definition_model *model, const char *id)
{
	__attribute__nonnull__(model);
	oval_test_t *test;

	test = (oval_test_t *) malloc(sizeof(oval_test_t));
	if (test == NULL)
		return NULL;

	test->deprecated = 0;
	test->version = 0;
	test->check = OVAL_CHECK_UNKNOWN;
	test->existence = OVAL_EXISTENCE_UNKNOWN;
	test->state_operator = OVAL_OPERATOR_AND;
	test->subtype = OVAL_SUBTYPE_UNKNOWN;
	test->comment = NULL;
	test->id = oscap_strdup(id);
	test->object = NULL;
	test->states = oval_collection_new();
	test->notes = oval_collection_new();
	test->model = model;

	oval_definition_model_add_test(model, test);

	return test;
}

struct oval_test *oval_test_clone(struct oval_definition_model *new_model, struct oval_test *old_test) {
	__attribute__nonnull__(old_test);

	struct oval_state_iterator *ste_itr;
	struct oval_test *new_test = oval_definition_model_get_test(new_model, old_test->id);
	if (new_test == NULL) {
		new_test = oval_test_new(new_model, old_test->id);
		oval_test_set_deprecated(new_test, old_test->deprecated);
		oval_test_set_version(new_test, old_test->version);
		oval_test_set_check(new_test, old_test->check);
		oval_test_set_existence(new_test, old_test->existence);
		oval_test_set_state_operator(new_test, old_test->state_operator);
		oval_test_set_subtype(new_test, old_test->subtype);
		oval_test_set_comment(new_test, old_test->comment);

		if (old_test->object) {
			struct oval_object *object = oval_object_clone(new_model, old_test->object);
			oval_test_set_object(new_test, object);
		}

		ste_itr = oval_test_get_states(old_test);
		while (oval_state_iterator_has_more(ste_itr)) {
			struct oval_state *ste;

			ste = oval_state_iterator_next(ste_itr);
			ste = oval_state_clone(new_model, ste);
			oval_test_add_state(new_test, ste);
		}
		oval_state_iterator_free(ste_itr);

		struct oval_string_iterator *notes = oval_test_get_notes(old_test);
		while (oval_string_iterator_has_more(notes)) {
			char *note = oval_string_iterator_next(notes);
			oval_test_add_note(new_test, note);
		}
		oval_string_iterator_free(notes);
	}
	return new_test;
}

void oval_test_free(struct oval_test *test)
{
	if (test == NULL)
		return;

	free(test->comment);
	free(test->id);
	oval_collection_free_items(test->notes, free);
	oval_collection_free(test->states);

	free(test);
}

void oval_test_set_deprecated(struct oval_test *test, bool deprecated)
{
	__attribute__nonnull__(test);
	test->deprecated = deprecated;
}

void oval_test_set_version(struct oval_test *test, int version)
{
	__attribute__nonnull__(test);
	test->version = version;
}

void oval_test_set_subtype(struct oval_test *test, oval_subtype_t subtype)
{
	__attribute__nonnull__(test);
	test->subtype = subtype;
}

void oval_test_set_comment(struct oval_test *test, char *comm)
{
	__attribute__nonnull__(test);
	if (test->comment != NULL)
		free(test->comment);
	test->comment = oscap_strdup(comm);
}

void oval_test_set_existence(struct oval_test *test, oval_existence_t existence)
{
	__attribute__nonnull__(test);
	test->existence = existence;
}

void oval_test_set_state_operator(struct oval_test *test, oval_operator_t state_operator)
{
	__attribute__nonnull__(test);
	test->state_operator = state_operator;
}

void oval_test_set_check(struct oval_test *test, oval_check_t check)
{
	__attribute__nonnull__(test);
	test->check = check;
}

void oval_test_set_object(struct oval_test *test, struct oval_object *object)
{
	__attribute__nonnull__(test);
	test->object = object;
}

void oval_test_add_state(struct oval_test *test, struct oval_state *state)
{
	__attribute__nonnull__(test);
	oval_collection_add(test->states, state);
}

void oval_test_add_note(struct oval_test *test, char *note)
{
	__attribute__nonnull__(test);
	oval_collection_add(test->notes, (void *)oscap_strdup(note));
}

static void _oval_test_parse_notes_consumer(char *text, void *test)
{
	oval_test_add_note(test, text);
}

static int _oval_test_parse_notes(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_test *test = (struct oval_test *)user;
	return oscap_parser_text_value(reader, &_oval_test_parse_notes_consumer, test);
}

static int _oval_test_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_test *test = (struct oval_test *)user;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	int return_code = 0;
	if ((strcmp(tagname, "notes") == 0)) {
		return_code = oval_parser_parse_tag(reader, context, &_oval_test_parse_notes, test);
	} else if ((strcmp(tagname, "object") == 0)) {
		char *object_ref = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "object_ref");
		if (object_ref != NULL) {
			struct oval_definition_model *model = context->definition_model;
			struct oval_object *object = oval_definition_model_get_new_object(model, object_ref);
			free(object_ref);
			object_ref = NULL;
			oval_test_set_object(test, object);
		}
	} else if ((strcmp(tagname, "state") == 0)) {
		char *state_ref = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "state_ref");
		if (state_ref != NULL) {
			struct oval_definition_model *model = context->definition_model;
			struct oval_state *state = oval_definition_model_get_new_state(model, state_ref);
			oval_test_add_state(test, state);
			free(state_ref);
			state_ref = NULL;
		}
	} else {
		dW("Skipping tag <%s>.", tagname);
		return_code = oval_parser_skip_tag(reader, context);
	}

	free(tagname);
	return return_code;

}

int oval_test_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr)
{
	int ret = 0;
	char *comm = NULL;
	char *version = NULL;
	struct oval_definition_model *model = context->definition_model;

	char *id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
        if (id == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Found test element without id attribute.");
		return -1;
	}
	struct oval_test *test = oval_definition_model_get_new_test(model, id);

	oval_subtype_t subtype = oval_subtype_parse(reader);
        if ( subtype == OVAL_SUBTYPE_UNKNOWN) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Unknown test type %s.", id);
		ret = -1;
		goto cleanup;
        }
	oval_test_set_subtype(test, subtype);

	oval_operator_t ste_operator = oval_operator_parse(reader, "state_operator", OVAL_OPERATOR_AND);
	oval_test_set_state_operator(test, ste_operator);

	oval_check_t check = oval_check_parse(reader, "check", OVAL_CHECK_UNKNOWN);
	if (check == OVAL_CHECK_NONE_EXIST) {
		dW("The 'none exist' CheckEnumeration value has been deprecated. "
		   "Converted to check='none satisfy' and check_existence='none exist'.");
		oval_test_set_check(test, OVAL_CHECK_NONE_SATISFY);
		oval_test_set_existence(test, OVAL_NONE_EXIST);
	} else {
		oval_existence_t existence;

		oval_test_set_check(test, check);
		existence = oval_existence_parse(reader, "check_existence", OVAL_AT_LEAST_ONE_EXISTS);
		oval_test_set_existence(test, existence);
	}

	comm = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	if (comm != NULL) {
		oval_test_set_comment(test, comm);
	}

	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	oval_test_set_deprecated(test, deprecated);

	version = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	if (version != NULL) {
		oval_test_set_version(test, atoi(version));
	}


	ret = oval_parser_parse_tag(reader, context, &_oval_test_parse_tag, test);

cleanup:
	free(version);
	free(comm);
	free(id);
	return ret;
}


xmlNode *oval_test_to_dom(struct oval_test *test, xmlDoc * doc, xmlNode * parent)
{
	xmlNode * test_node=NULL;

	/* skip unknown test */
	oval_subtype_t subtype = oval_test_get_subtype(test);
	if ( subtype == OVAL_SUBTYPE_UNKNOWN ) {
		dE("Unknown Test %s.", oval_test_get_id(test));
		return test_node;
	}

	/* get test name */
	const char *subtype_text = oval_subtype_get_text(subtype);
	char *test_name = malloc(strlen(subtype_text) + 6);
	sprintf(test_name, "%s_test", subtype_text);

	oval_family_t family = oval_test_get_family(test);

	/* search namespace & create child */
	xmlNs *ns_family = oval_family_to_namespace(family, (const char *) OVAL_DEFINITIONS_NAMESPACE, doc, parent);
	test_node = xmlNewTextChild(parent, ns_family, BAD_CAST test_name, NULL);
	free(test_name);

	char *id = oval_test_get_id(test);
	xmlNewProp(test_node, BAD_CAST "id", BAD_CAST id);

	char version[10];
	*version = '\0';
	snprintf(version, sizeof(version), "%d", oval_test_get_version(test));
	xmlNewProp(test_node, BAD_CAST "version", BAD_CAST version);

	oval_existence_t existence = oval_test_get_existence(test);
	if (existence != OVAL_AT_LEAST_ONE_EXISTS)
		xmlNewProp(test_node, BAD_CAST "check_existence", BAD_CAST oval_existence_get_text(existence));

	oval_check_t check = oval_test_get_check(test);
	xmlNewProp(test_node, BAD_CAST "check", BAD_CAST oval_check_get_text(check));

	oval_operator_t ste_operator = oval_test_get_state_operator(test);
	if (ste_operator != OVAL_OPERATOR_AND)
		xmlNewProp(test_node, BAD_CAST "state_operator", BAD_CAST oval_operator_get_text(ste_operator));

	char *comm = oval_test_get_comment(test);
	xmlNewProp(test_node, BAD_CAST "comment", BAD_CAST comm);

	bool deprecated = oval_test_get_deprecated(test);
	if (deprecated)
		xmlNewProp(test_node, BAD_CAST "deprecated", BAD_CAST "true");

	struct oval_string_iterator *notes = oval_test_get_notes(test);
	if (oval_string_iterator_has_more(notes)) {
		xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
		xmlNode *notes_node = xmlNewTextChild(test_node, ns_definitions, BAD_CAST "notes", NULL);
		while (oval_string_iterator_has_more(notes)) {
			char *note = oval_string_iterator_next(notes);
			xmlNewTextChild(notes_node, ns_definitions, BAD_CAST "note", BAD_CAST note);
		}
	}
	oval_string_iterator_free(notes);

	struct oval_object *object = oval_test_get_object(test);
	if (object) {
		xmlNode *object_node = xmlNewTextChild(test_node, ns_family, BAD_CAST "object", NULL);
		xmlNewProp(object_node, BAD_CAST "object_ref", BAD_CAST oval_object_get_id(object));
	}

	struct oval_state_iterator *ste_itr = oval_test_get_states(test);
	while (oval_state_iterator_has_more(ste_itr)) {
		struct oval_state *state;

		state = oval_state_iterator_next(ste_itr);
		xmlNode *state_node = xmlNewTextChild(test_node, ns_family, BAD_CAST "state", NULL);
		xmlNewProp(state_node, BAD_CAST "state_ref", BAD_CAST oval_state_get_id(state));
	}
	oval_state_iterator_free(ste_itr);

	return test_node;
}
