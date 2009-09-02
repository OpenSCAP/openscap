/**
 * @file oval_test.c
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

typedef struct oval_test {
	oval_subtype_enum subtype;
	char *name;
	struct oval_collection *notes;
	char *comment;
	char *id;
	int deprecated;
	int version;
	oval_operator_enum operator    ;
	oval_existence_enum existence;
	oval_check_enum check;
	struct oval_object *object;
	struct oval_state *state;
} oval_test_t;

int oval_iterator_test_has_more(struct oval_iterator_test *oc_test)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_test);
}

struct oval_test *oval_iterator_test_next(struct oval_iterator_test *oc_test)
{
	return (struct oval_test *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_test);
}

oval_family_enum oval_test_family(struct oval_test *test)
{
	return ((test->subtype) / 1000) * 1000;
}

oval_subtype_enum oval_test_subtype(struct oval_test * test)
{
	return test->subtype;
}

char *oval_test_name(struct oval_test *test)
{
	return test->name;
}

struct oval_iterator_string *oval_test_notes(struct oval_test *test)
{
	return (struct oval_iterator_string *)oval_collection_iterator(test->
								       notes);
}

char *oval_test_comment(struct oval_test *test)
{
	return test->comment;
}

char *oval_test_id(struct oval_test *test)
{
	return test->id;
}

int oval_test_deprecated(struct oval_test *test)
{
	return test->deprecated;
}

int oval_test_version(struct oval_test *test)
{
	return test->version;
}

oval_operator_enum oval_test_operator(struct oval_test * test)
{
	return test->operator;
}

oval_existence_enum oval_test_existence(struct oval_test * test)
{
	return test->existence;
}

oval_check_enum oval_test_check(struct oval_test * test)
{
	return test->check;
}

struct oval_object *oval_test_object(struct oval_test *test)
{
	return test->object;
}

struct oval_state *oval_test_state(struct oval_test *test)
{
	return test->state;
}

struct oval_test *oval_test_new(char *id)
{
	oval_test_t *test = (oval_test_t *) malloc(sizeof(oval_test_t));
	test->deprecated = 0;
	test->version = 0;
	test->check = OVAL_CHECK_UNKNOWN;
	test->existence = EXISTENCE_UNKNOWN;
	test->operator   = OPERATOR_UNKNOWN;
	test->subtype = OVAL_SUBTYPE_UNKNOWN;
	test->comment = NULL;
	test->id = strdup(id);
	test->name = NULL;
	test->object = NULL;
	test->state = NULL;
	test->notes = oval_collection_new();
	return test;
}

void oval_test_free(struct oval_test *test)
{
	if (test->comment != NULL)
		free(test->comment);
	if (test->id != NULL)
		free(test->id);
	if (test->name != NULL)
		free(test->name);
	oval_collection_free_items(test->notes, &free);

	test->comment = NULL;
	test->id = NULL;
	test->name = NULL;
	test->notes = NULL;
	test->object = NULL;
	test->state = NULL;

	free(test);
}

void set_oval_test_deprecated(struct oval_test *test, int deprecated)
{
	test->deprecated = deprecated;
}

void set_oval_test_version(struct oval_test *test, int version)
{
	test->version = version;
}

void set_oval_test_subtype(struct oval_test *test, oval_subtype_enum subtype)
{
	test->subtype = subtype;
}

void set_oval_test_comment(struct oval_test *test, char *comm)
{
	if(test->comment!=NULL)free(test->comment);
	test->comment = comm==NULL?NULL:strdup(comm);
}

void set_oval_test_existence(struct oval_test *test,
			     oval_existence_enum existence)
{
	test->existence = existence;
}

void set_oval_test_check(struct oval_test *test, oval_check_enum check)
{
	test->check = check;
}

void set_oval_test_object(struct oval_test *test, struct oval_object *object)
{
	test->object = object;
}

void set_oval_test_state(struct oval_test *test, struct oval_state *state)
{
	test->state = state;
}

void add_oval_test_notes(struct oval_test *test, char *note)
{
	oval_collection_add(test->notes, (void *)strdup(note));
}

void _oval_test_parse_notes_consumer(char *text, void *test) {
	add_oval_test_notes(test, text);
}
int _oval_test_parse_notes(xmlTextReaderPtr reader,
			   struct oval_parser_context *context, void *user)
{
	struct oval_test *test = (struct oval_test *)user;
	return oval_parser_text_value(reader, context, &_oval_test_parse_notes_consumer, test);
}

int _oval_test_parse_tag(xmlTextReaderPtr reader,
			 struct oval_parser_context *context, void *user)
{
	struct oval_test *test = (struct oval_test *)user;
	char *tagname = (char*) xmlTextReaderName(reader);
	//xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code = 1;
	if ((strcmp(tagname, "notes") == 0)) {
		return_code =
		    oval_parser_parse_tag(reader, context,
					  &_oval_test_parse_notes, test);
	} else if ((strcmp(tagname, "object") == 0)) {
		char *object_ref =
		    (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "object_ref");
		if (object_ref != NULL) {
			struct oval_object_model *model =
			    oval_parser_context_model(context);
			struct oval_object *object =
			    get_oval_object_new(model, object_ref);
			free(object_ref);object_ref=NULL;
			set_oval_test_object(test, object);
		}
	} else if ((strcmp(tagname, "state") == 0)) {
		char *state_ref =
		    (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "state_ref");
		if (state_ref != NULL) {
			struct oval_object_model *model =
			    oval_parser_context_model(context);
			struct oval_state *state =
			    get_oval_state_new(model, state_ref);
			set_oval_test_state(test, state);
			free(state_ref);state_ref=NULL;
		}
	} else {
		int linno = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE::(oval_test)skipping <%s> depth = %d line = %d\n",
		     tagname, xmlTextReaderDepth(reader), linno);
		return_code = oval_parser_skip_tag(reader, context);
	}

	free(tagname);
	return return_code;

}

int oval_test_parse_tag(xmlTextReaderPtr reader,
			struct oval_parser_context *context)
{
	struct oval_object_model *model = oval_parser_context_model(context);
	char *id = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	struct oval_test *test = get_oval_test_new(model, id);
	free(id);id=test->id;
	oval_subtype_enum subtype = oval_subtype_parse(reader);
	set_oval_test_subtype(test, subtype);
	oval_existence_enum existence =
	    oval_existence_parse(reader, "check_existence",
				 AT_LEAST_ONE_EXISTS);
	set_oval_test_existence(test, existence);
	oval_check_enum check =
	    oval_check_parse(reader, "check", OVAL_CHECK_UNKNOWN);
	set_oval_test_check(test, check);
	char *comm = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	if(comm!=NULL){
		set_oval_test_comment(test, comm);
		free(comm);comm=NULL;
	}
	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	set_oval_test_deprecated(test, deprecated);
	char *version = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	set_oval_test_version(test, atoi(version));
	free(version);version=NULL;

	int return_code =
	    oval_parser_parse_tag(reader, context, &_oval_test_parse_tag, test);
	return return_code;
}

void oval_test_to_print(struct oval_test *test, char *indent, int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sTEST.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sTEST[%d].", indent, idx);

	printf("%sID         = %s\n", nxtindent, oval_test_id(test));
	printf("%sFAMILY     = %d\n", nxtindent, oval_test_family(test));
	printf("%sSUBTYPE    = %d\n", nxtindent, oval_test_subtype(test));
	printf("%sVERSION    = %d\n", nxtindent, oval_test_version(test));
	printf("%sCOMMENT    = %s\n", nxtindent, oval_test_comment(test));
	printf("%sDEPRECATED = %d\n", nxtindent, oval_test_deprecated(test));
	printf("%sEXISTENCE  = %d\n", nxtindent, oval_test_existence(test));
	printf("%sCHECK      = %d\n", nxtindent, oval_test_check(test));
	struct oval_iterator_string *notes = oval_test_notes(test);
	for (idx = 1; oval_iterator_string_has_more(notes); idx++) {
		printf("%sNOTE[%d]    = %s\n", nxtindent, idx,
		       oval_iterator_string_next(notes));
	}
	struct oval_object *object = oval_test_object(test);
	if (object == NULL)
		printf("%sOBJECT     = <<NONE>>\n", nxtindent);
	else
		oval_object_to_print(object, nxtindent, 0);
	struct oval_state *state = oval_test_state(test);
	if (state == NULL)
		printf("%sSTATE      = <<NONE>>\n", nxtindent);
	else
		oval_state_to_print(state, nxtindent, 0);
}

xmlNode *oval_test_to_dom (struct oval_test *test, xmlDoc *doc, xmlNode *parent)
{
	oval_subtype_enum subtype = oval_test_subtype(test);
	const char *subtype_text = oval_subtype_text(subtype);
	char  test_name[strlen(subtype_text)+6]; *test_name = '\0';
	strcat(strcat(test_name, subtype_text), "_test");
	xmlNode *test_node = xmlNewChild(parent, NULL, test_name, NULL);

	oval_family_enum family = oval_test_family(test);
	const char *family_text = oval_family_text(family);
	char family_uri[strlen(OVAL_DEFINITIONS_NAMESPACE)+strlen(family_text)+2];
	*family_uri = '\0';
	strcat(strcat(strcat(family_uri, OVAL_DEFINITIONS_NAMESPACE),"#"),family_text);
	xmlNs *ns_family = xmlNewNs(test_node, family_uri, NULL);

	xmlSetNs(test_node, ns_family);

	char *id = oval_test_id(test);
	xmlNewProp(test_node, "id", id);

	char version[10]; *version = '\0';
	snprintf(version, sizeof(version), "%d", oval_test_version(test));
	xmlNewProp(test_node, "version", version);

	oval_existence_enum existence = oval_test_existence(test);
	if(existence!=AT_LEAST_ONE_EXISTS)
		xmlNewProp(test_node, "check_existence", oval_existence_text(existence));

	oval_check_enum check = oval_test_check(test);
	xmlNewProp(test_node, "check", oval_check_text(check));

	char *comment = oval_test_comment(test);
	xmlNewProp(test_node, "comment", comment);

	bool deprecated = oval_test_deprecated(test);
	if(deprecated)
		xmlNewProp(test_node, "deprecated", "true");

	struct oval_iterator_string *notes = oval_test_notes(test);
	if(oval_iterator_string_has_more(notes)){
		xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
		xmlNode *notes_node = xmlNewChild(test_node, ns_definitions, "notes", NULL);
		while(oval_iterator_string_has_more(notes)){
			char *note = oval_iterator_string_next(notes);
			xmlNewChild(notes_node, ns_definitions, "note", note);
		}
	}

	struct oval_object *object = oval_test_object(test);
	if(object){
		xmlNode *object_node = xmlNewChild(test_node, ns_family, "object", NULL);
		xmlNewProp(object_node, "object_ref", oval_object_id(object));
	}

	struct oval_state *state = oval_test_state(test);
	if(state){
		xmlNode *state_node = xmlNewChild(test_node, ns_family, "state", NULL);
		xmlNewProp(state_node, "state_ref", oval_state_id(state));
	}

	return test_node;
}
