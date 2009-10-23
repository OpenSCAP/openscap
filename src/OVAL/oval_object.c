/**
 * @file oval_object.c
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

typedef struct oval_object {
	oval_subtype_t subtype;
	struct oval_collection *notes;
	char *comment;
	char *id;
	int deprecated;
	int version;
	struct oval_collection *object_content;
	struct oval_collection *behaviors;
} oval_object_t;

int oval_object_iterator_has_more(struct oval_object_iterator *oc_object)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_object);
}

struct oval_object *oval_object_iterator_next(struct oval_object_iterator
					      *oc_object)
{
	return (struct oval_object *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_object);
}

void oval_object_iterator_free(struct oval_object_iterator
					      *oc_object)
{
    oval_collection_iterator_free((struct oval_iterator *)oc_object);
}

oval_family_t oval_object_get_family(struct oval_object *object)
{
	return ((object->subtype) / 1000) * 1000;
}

oval_subtype_t oval_object_get_subtype(struct oval_object * object)
{
	return ((struct oval_object *)object)->subtype;
}

char *oval_object_get_name (struct oval_object *object) { /* TODO */
	return NULL;
}

struct oval_string_iterator *oval_object_get_notes(struct oval_object *object)
{
	return (struct oval_string_iterator *)oval_collection_iterator(object->
								       notes);
}

char *oval_object_get_comment(struct oval_object *object)
{
	return ((struct oval_object *)object)->comment;
}

char *oval_object_get_id(struct oval_object *object)
{
	return ((struct oval_object *)object)->id;
}

int oval_object_get_deprecated(struct oval_object *object)
{
	return ((struct oval_object *)object)->deprecated;
}

int oval_object_get_version(struct oval_object *object)
{
	return ((struct oval_object *)object)->version;
}

struct oval_object_content_iterator *oval_object_get_object_contents(struct
								oval_object
								*object)
{
	return (struct oval_object_content_iterator *)
	    oval_collection_iterator(object->object_content);
}

struct oval_behavior_iterator *oval_object_get_behaviors(struct oval_object *object)
{
	return (struct oval_behavior_iterator *)
	    oval_collection_iterator(object->behaviors);
}

struct oval_object *oval_object_new(char *id)
{
	oval_object_t *object = (oval_object_t *) malloc(sizeof(oval_object_t));
	object->comment = NULL;
	object->id = strdup(id);
	object->subtype = OVAL_SUBTYPE_UNKNOWN;
	object->deprecated = 0;
	object->version = 0;
	object->behaviors = oval_collection_new();
	object->notes = oval_collection_new();
	object->object_content = oval_collection_new();
	return object;
}

struct oval_object *oval_object_clone
	(struct oval_object *old_object, struct oval_definition_model *model)
{
	struct oval_object *new_object = oval_definition_model_get_object(model, old_object->id);
	if(new_object==NULL){
		new_object = oval_object_new(old_object->id);
		oval_object_set_comment   (new_object, old_object->comment);
		oval_object_set_subtype   (new_object, old_object->subtype);
		oval_object_set_deprecated(new_object, old_object->deprecated);
		oval_object_set_version   (new_object, old_object->version);

		struct oval_behavior_iterator *behaviors = oval_object_get_behaviors(old_object);
		while(oval_behavior_iterator_has_more(behaviors)){
			struct oval_behavior *behavior = oval_behavior_iterator_next(behaviors);
			oval_object_add_behavior(new_object, oval_behavior_clone(behavior));
		}
		oval_behavior_iterator_free(behaviors);
		struct oval_string_iterator *notes = oval_object_get_notes(old_object);
		while(oval_string_iterator_has_more(notes)){
			char *note = oval_string_iterator_next(notes);
			oval_object_add_note(new_object, note);
		}
		oval_string_iterator_free(notes);
		struct oval_object_content_iterator *object_contents = oval_object_get_object_contents(old_object);
		while(oval_object_content_iterator_has_more(object_contents)){
			struct oval_object_content *object_content = oval_object_content_iterator_next(object_contents);
			oval_object_add_object_content(new_object, oval_object_content_clone(object_content, model));
		}
		oval_object_content_iterator_free(object_contents);

		oval_definition_model_add_object(model, new_object);
	}
	return new_object;
}

void oval_object_free(struct oval_object *object)
{
	if (object->comment != NULL)
		free(object->comment);
	if (object->id != NULL)
		free(object->id);
	oval_collection_free_items(object->behaviors, (oscap_destruct_func)oval_behavior_free);
	oval_collection_free_items(object->notes, free);
	oval_collection_free_items(object->object_content, (oscap_destruct_func)oval_object_content_free);

	object->comment = NULL;
	object->id = NULL;
	object->behaviors = NULL;
	object->notes = NULL;
	object->object_content = NULL;
	free(object);
}

void oval_object_set_family(struct oval_object *obj, oval_family_t family) { /* TODO */
	return NULL;
}

void oval_object_set_subtype(struct oval_object *object,
			     oval_subtype_t subtype)
{
	object->subtype = subtype;
}

void oval_object_add_note(struct oval_object *object, char *note)
{
	oval_collection_add(object->notes, (void *)strdup(note));
}

void oval_object_set_comment(struct oval_object *object, char *comm)
{
	if(object->comment!=NULL)free(object->comment);
	object->comment = comm==NULL?NULL:strdup(comm);
}

void oval_object_set_deprecated(struct oval_object *object, int deprecated)
{
	object->deprecated = deprecated;
}

void oval_object_set_version(struct oval_object *object, int version)
{
	object->version = version;
}

void oval_object_add_object_content(struct oval_object *object,
				    struct oval_object_content *content)
{
	oval_collection_add(object->object_content, (void *)content);
}

void oval_object_add_behavior(struct oval_object *object,
			       struct oval_behavior *behavior)
{
	oval_collection_add(object->behaviors, (void *)behavior);
}

static void oval_note_consume(char *text, void *object) {
	oval_object_add_note(object, text);
}
static int _oval_object_parse_notes(xmlTextReaderPtr reader,
			     struct oval_parser_context *context, void *user)
{
	struct oval_object *object = (struct oval_object *)user;
	return oval_parser_text_value(reader, context, &oval_note_consume, object);
}

static void oval_behavior_consume(struct oval_behavior *behavior,
				  void *object) {
	oval_object_add_behavior(object, behavior);
}
static void oval_content_consume(struct oval_object_content *content,
				 void *object) {
	oval_object_add_object_content(object, content);
}
static int _oval_object_parse_tag(xmlTextReaderPtr reader,
				  struct oval_parser_context *context, void *user)
{
	struct oval_object *object = (struct oval_object *)user;
	char *tagname = (char*) xmlTextReaderName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code = 1;
	if ((strcmp(tagname, "notes") == 0)) {
		return_code =
		    oval_parser_parse_tag(reader, context,
					  &_oval_object_parse_notes, object);
	} else if (strcmp(tagname, "behaviors") == 0) {
		return_code =
		    oval_behavior_parse_tag(reader, context,
					    oval_object_get_family(object),
					    &oval_behavior_consume, object);
	} else {
		return_code =
		    oval_object_content_parse_tag(reader, context,
						  &oval_content_consume, object);
	}
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: oval_object_parse_tag::parse of %s terminated on error at <%s> line %d\n",
		     object->id, tagname, line);
	}
	free(tagname);
	free(namespace);
	return return_code;
}

#define DEBUG_OVAL_OBJECT 0
#define  STUB_OVAL_OBJECT 0

int oval_object_parse_tag(xmlTextReaderPtr reader,
			  struct oval_parser_context *context)
{
	struct oval_definition_model *model = oval_parser_context_model(context);
	char *id = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	if(DEBUG_OVAL_OBJECT)printf("DEBUG::oval_object_parse_tag::id = %s\n", id);
	struct oval_object *object = get_oval_object_new(model, id);
	free(id);id=NULL;
	oval_subtype_t subtype = oval_subtype_parse(reader);
	oval_object_set_subtype(object, subtype);
	char *comm = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	if(comm!=NULL){
		oval_object_set_comment(object, comm);
		free(comm);comm=NULL;
	}
	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	oval_object_set_deprecated(object, deprecated);
	char *version = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	oval_object_set_version(object, atoi(version));
	free(version);

	int return_code = (STUB_OVAL_OBJECT)
		?oval_parser_skip_tag(reader, context)
	    :oval_parser_parse_tag(reader, context, &_oval_object_parse_tag,
				  object);
	return return_code;
}

void oval_object_to_print(struct oval_object *object, char *indent, int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sOBJECT.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sOBJECT[%d].", indent, idx);

	printf("%sID         = %s\n", nxtindent, oval_object_get_id(object));
	printf("%sFAMILY     = %d\n", nxtindent, oval_object_get_family(object));
	printf("%sSUBTYPE    = %d\n", nxtindent, oval_object_get_subtype(object));
	printf("%sVERSION    = %d\n", nxtindent, oval_object_get_version(object));
	printf("%sCOMMENT    = %s\n", nxtindent, oval_object_get_comment(object));
	printf("%sDEPRECATED = %d\n", nxtindent,
	       oval_object_get_deprecated(object));
	struct oval_string_iterator *notes = oval_object_get_notes(object);
	for (idx = 1; oval_string_iterator_has_more(notes); idx++) {
		printf("%sNOTE[%d]    = %s\n", nxtindent, idx,
		       oval_string_iterator_next(notes));
	}
	oval_string_iterator_free(notes);
	struct oval_behavior_iterator *behaviors =
	    oval_object_get_behaviors(object);
	for (idx = 1; oval_behavior_iterator_has_more(behaviors); idx++) {
		struct oval_behavior *behavior =
		    oval_behavior_iterator_next(behaviors);
		oval_behavior_to_print(behavior, nxtindent, idx);
	}
	oval_behavior_iterator_free(behaviors);
	struct oval_object_content_iterator *contents =
	    oval_object_get_object_contents(object);
	for (idx = 1; oval_object_content_iterator_has_more(contents);
	     idx++) {
		struct oval_object_content *content =
		    oval_object_content_iterator_next(contents);
		oval_object_content_to_print(content, nxtindent, idx);
	}
	oval_object_content_iterator_free(contents);
}

xmlNode *oval_object_to_dom
	(struct oval_object *object, xmlDoc *doc, xmlNode *parent)
{
	oval_subtype_t subtype = oval_object_get_subtype(object);
	const char *subtype_text = oval_subtype_get_text(subtype);
	char  object_name[strlen(subtype_text)+8]; *object_name = '\0';
	strcat(strcat(object_name, subtype_text), "_object");
	xmlNode *object_node = xmlNewChild(parent, NULL, object_name, NULL);

	oval_family_t family = oval_object_get_family(object);
	const char *family_text = oval_family_get_text(family);
	char family_uri[strlen((const char *)OVAL_DEFINITIONS_NAMESPACE)+strlen(family_text)+2];
	*family_uri = '\0';
	strcat(strcat(strcat(family_uri, (const char *)OVAL_DEFINITIONS_NAMESPACE),"#"),family_text);
	xmlNs *ns_family = xmlNewNs(object_node, family_uri, NULL);

	xmlSetNs(object_node, ns_family);

	char *id = oval_object_get_id(object);
	xmlNewProp(object_node, BAD_CAST "id", id);

	char version[10]; *version = '\0';
	snprintf(version, sizeof(version), "%d", oval_object_get_version(object));
	xmlNewProp(object_node, BAD_CAST "version", BAD_CAST version);

	char *comm = oval_object_get_comment(object);
	if(comm)xmlNewProp(object_node, BAD_CAST "comment", comm);

	bool deprecated = oval_object_get_deprecated(object);
	if(deprecated)
		xmlNewProp(object_node, BAD_CAST "deprecated", BAD_CAST "true");

	struct oval_string_iterator *notes = oval_object_get_notes(object);
	if(oval_string_iterator_has_more(notes)){
		xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
		xmlNode *notes_node = xmlNewChild(object_node, ns_definitions, BAD_CAST "notes", NULL);
		while(oval_string_iterator_has_more(notes)){
			char *note = oval_string_iterator_next(notes);
			xmlNewChild(notes_node, ns_definitions, BAD_CAST "note", note);
		}
	}
	oval_string_iterator_free(notes);

	struct oval_behavior_iterator *behaviors = oval_object_get_behaviors(object);
	if(oval_behavior_iterator_has_more(behaviors)){
		xmlNode *behaviors_node = xmlNewChild(object_node, ns_family, BAD_CAST "behaviors", NULL);
		while(oval_behavior_iterator_has_more(behaviors)){
			struct oval_behavior *behavior = oval_behavior_iterator_next(behaviors);
			char *key   = oval_behavior_get_key  (behavior);
			char *value = oval_behavior_get_value(behavior);
			xmlNewProp(behaviors_node, key, value);
		}
	}
	oval_behavior_iterator_free(behaviors);

	struct oval_object_content_iterator *contents = oval_object_get_object_contents(object);
	int i;for(i=0;oval_object_content_iterator_has_more(contents); i++){
		struct oval_object_content *content = oval_object_content_iterator_next(contents);
		oval_object_content_to_dom(content, doc, object_node);
	}
	oval_object_content_iterator_free(contents);

	return object_node;
}
