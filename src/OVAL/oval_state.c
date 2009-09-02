/**
 * @file oval_state.c
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

typedef struct oval_state {
	oval_subtype_enum subtype;
	char *name;
	char *comment;
	char *id;
	int deprecated;
	int version;
	struct oval_collection *notes;
	struct oval_collection *contents;
} oval_state_t;

int oval_iterator_state_has_more(struct oval_iterator_state *oc_state)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_state);
}

struct oval_state *oval_iterator_state_next(struct oval_iterator_state
					    *oc_state)
{
	return (struct oval_state *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_state);
}

oval_family_enum oval_state_family(struct oval_state *state)
{
	return (oval_state_subtype(state)/1000)*1000;
}

oval_subtype_enum oval_state_subtype(struct oval_state * state)
{
	return ((struct oval_state *)state)->subtype;
}

char *oval_state_name(struct oval_state *state)
{
	return ((struct oval_state *)state)->name;
}

struct oval_iterator_string *oval_state_notes(struct oval_state *state)
{
	return (struct oval_iterator_string *)oval_collection_iterator(state->
								       notes);
}

struct oval_iterator_state_content *oval_state_contents(struct oval_state *state)
{
	return (struct oval_iterator_state_content *)
		oval_collection_iterator(state->contents);
}

char *oval_state_comment(struct oval_state *state)
{
	return ((struct oval_state *)state)->comment;
}

char *oval_state_id(struct oval_state *state)
{
	return ((struct oval_state *)state)->id;
}

int oval_state_deprecated(struct oval_state *state)
{
	return ((struct oval_state *)state)->deprecated;
}

int oval_state_version(struct oval_state *state)
{
	return state->version;
}

struct oval_state *oval_state_new(char* id)
{
	oval_state_t *state = (oval_state_t *) malloc(sizeof(oval_state_t));
	state->deprecated = 0;
	state->version = 0;
	state->subtype = OVAL_SUBTYPE_UNKNOWN;
	state->comment = NULL;
	state->id = strdup(id);
	state->name = NULL;
	state->notes = oval_collection_new();
	state->contents = oval_collection_new();
	return state;
}

void oval_state_free(struct oval_state *state)
{
	if (state->comment != NULL)
		free(state->comment);
	if (state->id != NULL)
		free(state->id);
	if (state->name != NULL)
		free(state->name);
	oval_collection_free_items(state->notes, &free);
	oval_collection_free_items(state->contents, (oscap_destruct_func)oval_state_content_free);

	state->comment =NULL;
	state->contents =NULL;
	state->id =NULL;
	state->name =NULL;
	state->notes =NULL;
	free(state);
}

void set_oval_state_subtype(struct oval_state *state, oval_subtype_enum subtype)
{
	state->subtype = subtype;
}

void set_oval_state_name(struct oval_state *state, char *name)
{
	if(state->name!=NULL)free(state->name);
	state->name = name==NULL?NULL:strdup(name);
}

void add_oval_state_notes(struct oval_state *state, char *notes)
{
	oval_collection_add(state->notes, (void *)strdup(notes));
}

void set_oval_state_comment(struct oval_state *state, char *comm)
{
	if(state->comment!=NULL)free(state->comment);
	state->comment = comm==NULL?NULL:strdup(comm);
}

void set_oval_state_deprecated(struct oval_state *state, int deprecated)
{
	state->deprecated = deprecated;
}

void set_oval_state_version(struct oval_state *state, int version)
{
	state->version = version;
}

void add_oval_state_content
	(struct oval_state *state, struct oval_state_content *content)
{
	oval_collection_add(state->contents, content);
}

void _oval_note_consumer(char *text, void *state) {
	add_oval_state_notes(state, text);
}

int _oval_state_parse_notes(xmlTextReaderPtr reader,
			    struct oval_parser_context *context, void *user)
{
	struct oval_state *state = (struct oval_state *)user;
	return oval_parser_text_value(reader, context, _oval_note_consumer, state);
}

void _oval_state_content_consumer
	(struct oval_state_content *content, struct oval_state *state) {
	add_oval_state_content(state, content);
}

int _oval_state_parse_tag(xmlTextReaderPtr reader,
			  struct oval_parser_context *context, void *user)
{
	struct oval_state *state = (struct oval_state *)user;
	char *tagname = (char*) xmlTextReaderName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code = 1;
	if ((strcmp(tagname, "notes") == 0)) {
		return_code =
		    oval_parser_parse_tag(reader, context,
					  &_oval_state_parse_notes, state);
	} else {
		return_code =
		    oval_state_content_parse_tag
				(reader, context, (oscap_consumer_func)_oval_state_content_consumer, state);
	}
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: oval_state_parse_tag::parse of %s terminated on error at <%s> line %d\n",
		     state->id, tagname, line);
	}
	free(tagname);
	free(namespace);
	return return_code;
}

int oval_state_parse_tag(xmlTextReaderPtr reader,
			 struct oval_parser_context *context)
{
	struct oval_object_model *model = oval_parser_context_model(context);
	char *id = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	struct oval_state *state = get_oval_state_new(model, id);
	free(id);
	oval_subtype_enum subtype = oval_subtype_parse(reader);
	set_oval_state_subtype(state, subtype);
	char *comm = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	if(comm!=NULL){
		set_oval_state_comment(state, comm);
		free(comm);comm=NULL;
	}
	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	set_oval_state_deprecated(state, deprecated);
	char *version = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	set_oval_state_version(state, atoi(version));
	free(version);

	int return_code =
	    oval_parser_parse_tag(reader, context, &_oval_state_parse_tag,
				  state);
	return return_code;
}

void oval_state_to_print(struct oval_state *state, char *indent, int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sSTATE.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sSTATE[%d].", indent, idx);

	printf("%sID         = %s\n", nxtindent, oval_state_id(state));
	printf("%sFAMILY     = %d\n", nxtindent, oval_state_family(state));
	printf("%sSUBTYPE    = %d\n", nxtindent, oval_state_subtype(state));
	printf("%sVERSION    = %d\n", nxtindent, oval_state_version(state));
	printf("%sCOMMENT    = %s\n", nxtindent, oval_state_comment(state));
	printf("%sDEPRECATED = %d\n", nxtindent, oval_state_deprecated(state));
	struct oval_iterator_string *notes = oval_state_notes(state);
	for (idx = 1; oval_iterator_string_has_more(notes); idx++) {
		printf("%sNOTE[%d]    = %s\n", nxtindent, idx,
		       oval_iterator_string_next(notes));
	}
}

xmlNode *oval_state_to_dom (struct oval_state *state, xmlDoc *doc, xmlNode *parent)
{
	oval_subtype_enum subtype = oval_state_subtype(state);
	const char *subtype_text = oval_subtype_text(subtype);
	char  state_name[strlen(subtype_text)+7]; *state_name = '\0';
	strcat(strcat(state_name, subtype_text), "_state");
	xmlNode *state_node = xmlNewChild(parent, NULL, state_name, NULL);

	oval_family_enum family = oval_state_family(state);
	const char *family_text = oval_family_text(family);
	char family_uri[strlen(OVAL_DEFINITIONS_NAMESPACE)+strlen(family_text)+2];
	*family_uri = '\0';
	strcat(strcat(strcat(family_uri, OVAL_DEFINITIONS_NAMESPACE),"#"),family_text);
	xmlNs *ns_family = xmlNewNs(state_node, family_uri, NULL);

	xmlSetNs(state_node, ns_family);

	char *id = oval_state_id(state);
	xmlNewProp(state_node, "id", id);

	char version[10]; *version = '\0';
	snprintf(version, sizeof(version), "%d", oval_state_version(state));
	xmlNewProp(state_node, "version", version);

	char *comment = oval_state_comment(state);
	if(comment)xmlNewProp(state_node, "comment", comment);

	bool deprecated = oval_state_deprecated(state);
	if(deprecated)
		xmlNewProp(state_node, "deprecated", "true");

	struct oval_iterator_string *notes = oval_state_notes(state);
	if(oval_iterator_string_has_more(notes)){
		xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
		xmlNode *notes_node = xmlNewChild(state_node, ns_definitions, "notes", NULL);
		while(oval_iterator_string_has_more(notes)){
			char *note = oval_iterator_string_next(notes);
			xmlNewChild(notes_node, ns_definitions, "note", note);
		}
	}

	struct oval_iterator_state_content *contents = oval_state_contents(state);
	while(oval_iterator_state_content_has_more(contents))
	{
		struct oval_state_content *content = oval_iterator_state_content_next(contents);
		oval_state_content_to_dom(content, doc, state_node);
	}
	return state_node;
}
