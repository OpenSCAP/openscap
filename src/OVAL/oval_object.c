/**
 * @file oval_object.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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
#include "common/debug_priv.h"
#include "common/elements.h"
#include "public/oval_version.h"
#include "public/oval_schema_version.h"

typedef struct oval_object {
	struct oval_definition_model *model;
	oval_subtype_t subtype;
	struct oval_object *base_obj_ref;
	struct oval_collection *notes;
	char *comment;
	char *id;
	int deprecated;
	int version;
	struct oval_collection *object_content;
	struct oval_collection *behaviors;
} oval_object_t;

bool oval_object_iterator_has_more(struct oval_object_iterator *oc_object)
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
	__attribute__nonnull__(object);

	return ((object->subtype) / 1000) * 1000;
}

oval_subtype_t oval_object_get_subtype(struct oval_object * object)
{
	__attribute__nonnull__(object);

	return ((struct oval_object *)object)->subtype;
}

const char *oval_object_get_name(struct oval_object *object)
{

	__attribute__nonnull__(object);

	return oval_subtype_get_text(object->subtype);
}

struct oval_string_iterator *oval_object_get_notes(struct oval_object *object)
{
	__attribute__nonnull__(object);

	return (struct oval_string_iterator *)oval_collection_iterator(object->notes);
}

char *oval_object_get_comment(struct oval_object *object)
{
	__attribute__nonnull__(object);

	return ((struct oval_object *)object)->comment;
}

char *oval_object_get_id(struct oval_object *object)
{
	__attribute__nonnull__(object);

	return ((struct oval_object *)object)->id;
}

bool oval_object_get_deprecated(struct oval_object *object)
{
	__attribute__nonnull__(object);

	return ((struct oval_object *)object)->deprecated;
}

int oval_object_get_version(struct oval_object *object)
{
	__attribute__nonnull__(object);

	return ((struct oval_object *)object)->version;
}

oval_version_t oval_object_get_schema_version(struct oval_object *object)
{
	__attribute__nonnull__(object);

	if (object->model == NULL)
		return OVAL_VERSION_INVALID;
	return oval_definition_model_get_schema_version(object->model);
}

oval_schema_version_t oval_object_get_platform_schema_version(struct oval_object *object)
{
	__attribute__nonnull__(object);
	if (object->model == NULL) {
		return OVAL_SCHEMA_VERSION_INVALID;
	}
	oval_family_t family = oval_object_get_family(object);
	const char *platform = oval_family_get_text(family);
	return oval_definition_model_get_platform_schema_version(object->model, platform);
}

struct oval_object_content_iterator *oval_object_get_object_contents(struct
								     oval_object
								     *object)
{
	__attribute__nonnull__(object);

	return (struct oval_object_content_iterator *)
	    oval_collection_iterator(object->object_content);
}

struct oval_behavior_iterator *oval_object_get_behaviors(struct oval_object *object)
{
	__attribute__nonnull__(object);

	return (struct oval_behavior_iterator *)
	    oval_collection_iterator(object->behaviors);
}

struct oval_object *oval_object_new(struct oval_definition_model *model, const char *id)
{
	__attribute__nonnull__(model);
	oval_object_t *object;

	object = (oval_object_t *) malloc(sizeof(oval_object_t));
	if (object == NULL)
		return NULL;

	object->comment = NULL;
	object->id = oscap_strdup(id);
	object->subtype = OVAL_SUBTYPE_UNKNOWN;
	object->base_obj_ref = NULL;
	object->deprecated = 0;
	object->version = 0;
	object->behaviors = oval_collection_new();
	object->notes = oval_collection_new();
	object->object_content = oval_collection_new();
	object->model = model;

	oval_definition_model_add_object(model, object);

	return object;
}

struct oval_object *oval_object_clone2(struct oval_definition_model *new_model, struct oval_object *old_object, char *new_id)
{
	__attribute__nonnull__(old_object);

	struct oval_object *new_object;

	if (new_id == NULL)
		new_id = old_object->id;

	new_object = oval_definition_model_get_object(new_model, new_id);
	if (new_object == NULL) {
		new_object = oval_object_new(new_model, new_id);
		oval_object_set_comment(new_object, old_object->comment);
		oval_object_set_subtype(new_object, old_object->subtype);
		oval_object_set_deprecated(new_object, old_object->deprecated);
		oval_object_set_version(new_object, old_object->version);

		struct oval_behavior_iterator *behaviors = oval_object_get_behaviors(old_object);
		while (oval_behavior_iterator_has_more(behaviors)) {
			struct oval_behavior *behavior = oval_behavior_iterator_next(behaviors);
			oval_object_add_behavior(new_object, oval_behavior_clone(new_model, behavior));
		}
		oval_behavior_iterator_free(behaviors);
		struct oval_string_iterator *notes = oval_object_get_notes(old_object);
		while (oval_string_iterator_has_more(notes)) {
			char *note = oval_string_iterator_next(notes);
			oval_object_add_note(new_object, note);
		}
		oval_string_iterator_free(notes);
		struct oval_object_content_iterator *object_contents = oval_object_get_object_contents(old_object);
		while (oval_object_content_iterator_has_more(object_contents)) {
			struct oval_object_content *object_content = oval_object_content_iterator_next(object_contents);
			oval_object_add_object_content(new_object,
						       oval_object_content_clone(new_model, object_content));
		}
		oval_object_content_iterator_free(object_contents);
	}
	return new_object;
}

struct oval_object *oval_object_clone(struct oval_definition_model *new_model, struct oval_object *old_object)
{
	return oval_object_clone2(new_model, old_object, NULL);
}

void oval_object_free(struct oval_object *object)
{
	if (object == NULL)
		return;

	if (object->comment != NULL)
		free(object->comment);
	if (object->id != NULL)
		free(object->id);
	oval_collection_free_items(object->behaviors, (oscap_destruct_func) oval_behavior_free);
	oval_collection_free_items(object->notes, (oscap_destruct_func) free);
	oval_collection_free_items(object->object_content, (oscap_destruct_func) oval_object_content_free);

	object->comment = NULL;
	object->id = NULL;
	object->behaviors = NULL;
	object->notes = NULL;
	object->object_content = NULL;
	free(object);
}

void oval_object_set_subtype(struct oval_object *object, oval_subtype_t subtype)
{
	__attribute__nonnull__(object);
	object->subtype = subtype;
}

void oval_object_add_note(struct oval_object *object, char *note)
{
	__attribute__nonnull__(object);
	oval_collection_add(object->notes, (void *)oscap_strdup(note));
}

void oval_object_set_comment(struct oval_object *object, char *comm)
{
	__attribute__nonnull__(object);
	if (object->comment != NULL)
		free(object->comment);
	object->comment = (comm == NULL) ? NULL : oscap_strdup(comm);
}

void oval_object_set_deprecated(struct oval_object *object, bool deprecated)
{
	__attribute__nonnull__(object);
	object->deprecated = deprecated;
}

void oval_object_set_version(struct oval_object *object, int version)
{
	__attribute__nonnull__(object);
	object->version = version;
}

void oval_object_add_object_content(struct oval_object *object, struct oval_object_content *content)
{
	__attribute__nonnull__(object);
	oval_collection_add(object->object_content, (void *)content);
}

void oval_object_add_behavior(struct oval_object *object, struct oval_behavior *behavior)
{
	__attribute__nonnull__(object);
	oval_collection_add(object->behaviors, (void *)behavior);
}

static void oval_note_consume(char *text, void *object)
{
	oval_object_add_note(object, text);
}

static int _oval_object_parse_notes(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_object *object = (struct oval_object *)user;
	return oscap_parser_text_value(reader, &oval_note_consume, object);
}

static void oval_behavior_consume(struct oval_behavior *behavior, void *object)
{
	oval_object_add_behavior(object, behavior);
}

static void oval_content_consume(struct oval_object_content *content, void *object)
{
	oval_object_add_object_content(object, content);
}

static int _oval_object_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_object *object = (struct oval_object *)user;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code = 0;
	if ((strcmp(tagname, "notes") == 0)) {
		return_code = oval_parser_parse_tag(reader, context, &_oval_object_parse_notes, object);
	} else if (strcmp(tagname, "behaviors") == 0) {
		return_code = oval_behavior_parse_tag(reader, context, oval_object_get_family(object), &oval_behavior_consume, object);
	} else {
		return_code = oval_object_content_parse_tag(reader, context, &oval_content_consume, object);
	}

	if (return_code != 0) {
		dW("Parsing of <%s> terminated by an error at line %d.", tagname, xmlTextReaderGetParserLineNumber(reader));
	}

	free(tagname);
	free(namespace);
	return return_code;
}

/* -1 = error; 0 = OK; 1 = warning */
int oval_object_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr)
{
	int ret;
	char *comm = NULL;
	char *version = NULL;
	struct oval_definition_model *model = context->definition_model;

	char *id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	struct oval_object *object = oval_definition_model_get_new_object(model, id);

	oval_subtype_t subtype = oval_subtype_parse(reader);
	if ( subtype == OVAL_SUBTYPE_UNKNOWN) {
		dE("Unknown object %s.", id);
		ret = -1;
		goto cleanup;
	}
	oval_object_set_subtype(object, subtype);

	comm = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	if (comm != NULL) {
		oval_object_set_comment(object, comm);
	}

	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	oval_object_set_deprecated(object, deprecated);

	version = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	oval_object_set_version(object, atoi(version));

	ret = oval_parser_parse_tag(reader, context, &_oval_object_parse_tag, object);

cleanup:
	free(id);
	free(comm);
	free(version);
	return ret;
}

xmlNode *oval_object_to_dom(struct oval_object *object, xmlDoc * doc, xmlNode * parent)
{
	xmlNode *object_node = NULL;

	/* skip unknown object */
	oval_subtype_t subtype = oval_object_get_subtype(object);
        if ( subtype == OVAL_SUBTYPE_UNKNOWN ) {
                dE("Unknown Object %s.", oval_object_get_id(object));
                return object_node;
        }

	/* get object name */
	const char *subtype_text = oval_subtype_get_text(subtype);
	char object_name[strlen(subtype_text) + 8];
	sprintf(object_name, "%s_object", subtype_text);

	oval_family_t family = oval_object_get_family(object);

	/* search namespace & create child */
	xmlNs *ns_family = oval_family_to_namespace(family, (const char *) OVAL_DEFINITIONS_NAMESPACE, doc, parent);
	object_node = xmlNewTextChild(parent, ns_family, BAD_CAST object_name, NULL);

	char *id = oval_object_get_id(object);
	xmlNewProp(object_node, BAD_CAST "id", BAD_CAST id);

	char version[10];
	*version = '\0';
	snprintf(version, sizeof(version), "%d", oval_object_get_version(object));
	xmlNewProp(object_node, BAD_CAST "version", BAD_CAST version);

	char *comm = oval_object_get_comment(object);
	if (comm)
		xmlNewProp(object_node, BAD_CAST "comment", BAD_CAST comm);

	bool deprecated = oval_object_get_deprecated(object);
	if (deprecated)
		xmlNewProp(object_node, BAD_CAST "deprecated", BAD_CAST "true");

	struct oval_string_iterator *notes = oval_object_get_notes(object);
	if (oval_string_iterator_has_more(notes)) {
		xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
		xmlNode *notes_node = xmlNewTextChild(object_node, ns_definitions, BAD_CAST "notes", NULL);
		while (oval_string_iterator_has_more(notes)) {
			char *note = oval_string_iterator_next(notes);
			xmlNewTextChild(notes_node, ns_definitions, BAD_CAST "note", BAD_CAST note);
		}
	}
	oval_string_iterator_free(notes);

	struct oval_behavior_iterator *behaviors = oval_object_get_behaviors(object);
	if (oval_behavior_iterator_has_more(behaviors)) {
		xmlNode *behaviors_node = xmlNewTextChild(object_node, ns_family, BAD_CAST "behaviors", NULL);
		while (oval_behavior_iterator_has_more(behaviors)) {
			struct oval_behavior *behavior = oval_behavior_iterator_next(behaviors);
			char *key = oval_behavior_get_key(behavior);
			char *value = oval_behavior_get_value(behavior);
			xmlNewProp(behaviors_node, BAD_CAST key, BAD_CAST value);
		}
	}
	oval_behavior_iterator_free(behaviors);

	struct oval_object_content_iterator *contents = oval_object_get_object_contents(object);
	int i;
	for (i = 0; oval_object_content_iterator_has_more(contents); i++) {
		struct oval_object_content *content = oval_object_content_iterator_next(contents);
		oval_object_content_to_dom(content, doc, object_node);
	}
	oval_object_content_iterator_free(contents);

	return object_node;
}

struct oval_object *oval_object_create_internal(struct oval_object *obj, char *set_id)
{
	struct oval_object *new_obj;
	size_t oid_len, sid_len;
	char *new_obj_id;

	oid_len = strlen(obj->id);
	set_id = strrchr(set_id, ':') + 1;
	sid_len = strlen(set_id);
	new_obj_id = malloc(oid_len + sid_len + 2);
	memcpy(new_obj_id, obj->id, oid_len);
	new_obj_id[oid_len] = 'i';
	memcpy(new_obj_id + oid_len + 1, set_id, sid_len);
	new_obj_id[oid_len + sid_len + 1] = '\0';
	new_obj = oval_object_clone2(obj->model, obj, new_obj_id);
	new_obj->base_obj_ref = obj;
	free(new_obj_id);

	return new_obj;
}

struct oval_object *oval_object_get_base_obj(struct oval_object *obj)
{
	return obj->base_obj_ref;
}
