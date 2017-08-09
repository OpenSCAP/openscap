/**
 * @file oval_definition.c
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
#include "adt/oval_string_map_impl.h"
#include "oval_agent_api_impl.h"

#include "common/assume.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/elements.h"
#include "common/_error.h"


/***************************************************************************/
/* Variable definitions
 * */

typedef struct oval_definition {
	struct oval_definition_model *model;
	char *id;
	int version;
	oval_definition_class_t class;
	int deprecated;
	char *title;
	char *description;
	struct oval_collection *affected;
	struct oval_collection *reference;
	struct oval_collection *notes;
	char *anyxml;
	struct oval_criteria_node *criteria;
} oval_definition_t;


/* End of variable definitions
 * */
/***************************************************************************/

char *oval_definition_get_id(struct oval_definition *definition)
{
	__attribute__nonnull__(definition);

	return ((struct oval_definition *)definition)->id;
}

int oval_definition_get_version(struct oval_definition *definition)
{
	__attribute__nonnull__(definition);

	return ((struct oval_definition *)definition)->version;
}

oval_definition_class_t oval_definition_get_class(struct oval_definition *definition)
{
	__attribute__nonnull__(definition);

	return ((struct oval_definition *)definition)->class;
}

bool oval_definition_get_deprecated(struct oval_definition *definition)
{
	__attribute__nonnull__(definition);

	return ((struct oval_definition *)definition)->deprecated;
}

char *oval_definition_get_title(struct oval_definition *definition)
{
	__attribute__nonnull__(definition);

	return ((struct oval_definition *)definition)->title;
}

char *oval_definition_get_description(struct oval_definition *definition)
{
	__attribute__nonnull__(definition);

	return ((struct oval_definition *)definition)->description;
}

struct oval_affected_iterator *oval_definition_get_affected(struct oval_definition
							    *definition)
{
	__attribute__nonnull__(definition);

	return (struct oval_affected_iterator *)
	    oval_collection_iterator(definition->affected);
}

struct oval_reference_iterator *oval_definition_get_references(struct oval_definition
							       *definition)
{
	__attribute__nonnull__(definition);

	return (struct oval_reference_iterator *)
	    oval_collection_iterator(definition->reference);
}

struct oval_string_iterator *oval_definition_get_notes(struct oval_definition *definition) {
	__attribute__nonnull__(definition);

	return (struct oval_string_iterator *)
	    oval_collection_iterator(definition->notes);
}

struct oval_criteria_node *oval_definition_get_criteria(struct oval_definition
							*definition)
{
	__attribute__nonnull__(definition);

	return ((struct oval_definition *)definition)->criteria;
}

struct oval_definition *oval_definition_new(struct oval_definition_model *model, const char *id)
{
	__attribute__nonnull__(model);
	struct oval_definition *definition;

	definition = (struct oval_definition *)oscap_talloc(oval_definition_t);

        assume_r(definition != NULL, /* return */ NULL);

	definition->id = oscap_strdup(id);
	definition->version = 0;
	definition->class = OVAL_CLASS_UNKNOWN;
	definition->deprecated = 0;
	definition->title = NULL;
	definition->description = NULL;
	definition->affected = oval_collection_new();
	definition->reference = oval_collection_new();
	definition->notes = oval_collection_new();

	definition->anyxml = NULL;
	definition->criteria = NULL;
	definition->model = model;

	oval_definition_model_add_definition(model, definition);

	return definition;
}

struct oval_definition *oval_definition_clone
    (struct oval_definition_model *new_model, struct oval_definition *old_definition) {
	__attribute__nonnull__(old_definition);

	struct oval_definition *new_definition = oval_definition_model_get_definition(new_model, old_definition->id);
	if (new_definition == NULL) {

		new_definition = oval_definition_new(new_model, old_definition->id);
		oval_definition_set_version(new_definition, old_definition->version);
		oval_definition_set_class(new_definition, old_definition->class);
		oval_definition_set_deprecated(new_definition, old_definition->deprecated);
		oval_definition_set_title(new_definition, old_definition->title);
		oval_definition_set_description(new_definition, old_definition->description);

		struct oval_affected_iterator *affecteds = oval_definition_get_affected(old_definition);
		while (oval_affected_iterator_has_more(affecteds)) {
			struct oval_affected *old_affected = oval_affected_iterator_next(affecteds);
			oval_definition_add_affected(new_definition, oval_affected_clone(new_model, old_affected));
		}
		oval_affected_iterator_free(affecteds);
		struct oval_reference_iterator *references = oval_definition_get_references(old_definition);
		while (oval_reference_iterator_has_more(references)) {
			struct oval_reference *old_reference = oval_reference_iterator_next(references);
			oval_definition_add_reference(new_definition, oval_reference_clone(new_model, old_reference));
		}
		oval_reference_iterator_free(references);
		struct oval_string_iterator *notes = oval_definition_get_notes(old_definition);
		while (oval_string_iterator_has_more(notes)) {
			char *old_note = oval_string_iterator_next(notes);
			oval_definition_add_note(new_definition, old_note);
		}
		oval_string_iterator_free(notes);

		new_definition->anyxml = strdup(old_definition->anyxml);

		oval_definition_set_criteria(new_definition, oval_criteria_node_clone(new_model, old_definition->criteria));
	}
	return new_definition;
}

void oval_definition_free(struct oval_definition *definition)
{
	__attribute__nonnull__(definition);

	if (definition->id != NULL)
		free(definition->id);
	if (definition->title != NULL)
		free(definition->title);
	if (definition->description != NULL)
		free(definition->description);
	if (definition->criteria != NULL)
		oval_criteria_node_free(definition->criteria);
	oval_collection_free_items(definition->affected, (oscap_destruct_func) oval_affected_free);
	oval_collection_free_items(definition->reference, (oscap_destruct_func) oval_reference_free);
	oval_collection_free_items(definition->notes, (oscap_destruct_func) free);

	free(definition->anyxml);

	definition->affected = NULL;
	definition->criteria = NULL;
	definition->description = NULL;
	definition->id = NULL;
	definition->reference = NULL;
	definition->notes = NULL;
	definition->anyxml = NULL;
	definition->title = NULL;
	free(definition);
}

bool oval_definition_iterator_has_more(struct oval_definition_iterator
				       *oc_definition)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_definition);
}

struct oval_definition *oval_definition_iterator_next(struct
						      oval_definition_iterator
						      *oc_definition)
{
	return (struct oval_definition *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_definition);
}

void oval_definition_iterator_free(struct
				   oval_definition_iterator
				   *oc_definition)
{
	oval_collection_iterator_free((struct oval_iterator *)
				      oc_definition);
}

void oval_definition_set_version(struct oval_definition *definition, int version)
{
	__attribute__nonnull__(definition);
	definition->version = version;
}

void oval_definition_set_class(struct oval_definition *definition, oval_definition_class_t class)
{
	__attribute__nonnull__(definition);
	definition->class = class;
}

void oval_definition_set_deprecated(struct oval_definition *definition, bool deprecated)
{
	__attribute__nonnull__(definition);
	definition->deprecated = deprecated;
}

void oval_definition_set_title(struct oval_definition *definition, char *title)
{
	__attribute__nonnull__(definition);
	if (definition->title != NULL)
		free(definition->title);
	definition->title = (title == NULL) ? NULL : oscap_strdup(title);
}

void oval_definition_set_description(struct oval_definition *definition, char *description)
{
	__attribute__nonnull__(definition);
	if (definition->description)
		free(definition->description);
	definition->description = (description == NULL) ? NULL : oscap_strdup(description);
}

void oval_definition_set_criteria(struct oval_definition *definition, struct oval_criteria_node *criteria)
{
	__attribute__nonnull__(definition);
	definition->criteria = criteria;
}

void oval_definition_add_affected(struct oval_definition *definition, struct oval_affected *affected)
{
	__attribute__nonnull__(definition);
	oval_collection_add(definition->affected, affected);
}

void oval_definition_add_reference(struct oval_definition *definition, struct oval_reference *ref)
{
	__attribute__nonnull__(definition);
	oval_collection_add(definition->reference, ref);
}

void oval_definition_add_note(struct oval_definition *definition, char *note) {
	__attribute__nonnull__(definition);
	oval_collection_add(definition->notes, note);
}

static void _oval_definition_title_consumer(char *string, void *user)
{
	__attribute__nonnull__(user);

	struct oval_definition *definition = (struct oval_definition *)user;
	char *title = definition->title;
	if (title == NULL)
		title = oscap_strdup(string);
	else {
		int newsize = strlen(title) + strlen(string) + 1;
		char *newtitle = (char *)oscap_alloc(newsize * sizeof(char));
		if (newtitle == NULL)
			return;

		strcpy(newtitle, title);
		strcat(newtitle, string);
		free(title);
		title = newtitle;
	}
	definition->title = title;
}

static void _oval_definition_description_consumer(char *string, void *user)
{
	__attribute__nonnull__(user);

	struct oval_definition *definition = (struct oval_definition *)user;
	char *description = definition->description;
	if (description == NULL)
		description = oscap_strdup(string);
	else {
		int newsize = strlen(description) + strlen(string) + 1;
		char *newdescription = (char *)oscap_alloc(newsize * sizeof(char));
		if (newdescription == NULL)
			return;

		*newdescription = '\0';
		strcpy(newdescription, description);
		strcat(newdescription, string);
		free(description);
		description = newdescription;
	}
	definition->description = description;
}

static void _oval_definition_affected_consumer(struct oval_affected *affected, void *user)
{
	__attribute__nonnull__(user);

	struct oval_definition *definition = (struct oval_definition *)user;
	oval_collection_add(definition->affected, (void *)affected);
}

static void oval_reference_consume(struct oval_reference *ref, void *def)
{

	__attribute__nonnull__(def);

	struct oval_definition *definition = def;
	oval_collection_add(definition->reference, (void *)ref);
}

static int _oval_definition_parse_metadata(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_definition *definition = (struct oval_definition *)user;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	int return_code;
	if ((strcmp(tagname, "title") == 0)) {
		return_code = oscap_parser_text_value(reader, &_oval_definition_title_consumer, definition);
	} else if (strcmp(tagname, "description") == 0) {
		return_code = oscap_parser_text_value(reader, &_oval_definition_description_consumer, definition);
	} else if (strcmp(tagname, "affected") == 0) {
		return_code = oval_affected_parse_tag(reader, context, &_oval_definition_affected_consumer, definition);
	} else if (strcmp(tagname, "oval_repository") == 0) {	/* NOOP */
		return_code = oval_parser_skip_tag(reader, context);
	} else if (strcmp(tagname, "reference") == 0) {
		return_code = oval_reference_parse_tag(reader, context, &oval_reference_consume, definition);
	} else {
		definition->anyxml = (char *) xmlTextReaderReadOuterXml(reader);
		return_code = oval_parser_skip_tag(reader, context);
	}
	free(tagname);
	return return_code;
}

static void _oval_definition_criteria_consumer(struct oval_criteria_node *criteria, void *user)
{
	__attribute__nonnull__(user);

	struct oval_definition *definition = (struct oval_definition *)user;
	definition->criteria = criteria;
}

static int _oval_definition_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_definition *definition = (struct oval_definition *)user;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	int return_code;
	if ((strcmp(tagname, "metadata") == 0)) {
		return_code = oval_parser_parse_tag(reader, context, &_oval_definition_parse_metadata, definition);
	} else if ((strcmp(tagname, "criteria") == 0)) {
		return_code = oval_criteria_parse_tag(reader, context, &_oval_definition_criteria_consumer, definition);
	} else {
		dI("Skipping tag: %s.", tagname);
		return_code = oval_parser_skip_tag(reader, context);
	}
	free(tagname);
	return return_code;
}

int oval_definition_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr)
{
	struct oval_definition_model *model = context->definition_model;
	char *id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	struct oval_definition *definition = oval_definition_model_get_new_definition(model, id);
	free(id);
	id = NULL;

	char *version = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	oval_definition_set_version(definition, atoi(version));
	free(version);
	version = NULL;

	char *class = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "class");
	oval_definition_set_class(definition, oval_definition_class_enum(class));
	free(class);
	class = NULL;

	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	oval_definition_set_deprecated(definition, deprecated);
	int return_code = oval_parser_parse_tag(reader, context, &_oval_definition_parse_tag, definition);
	return return_code;
}


xmlNode *oval_definition_to_dom(struct oval_definition *definition, xmlDoc * doc, xmlNode * parent)
{
	xmlNode *nodestr, *nodelst;
	xmlDoc * docstr;
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *definition_node = xmlNewTextChild(parent, ns_definitions, BAD_CAST "definition", NULL);

	char *id = oval_definition_get_id(definition);
	xmlNewProp(definition_node, BAD_CAST "id", BAD_CAST id);

	char version[10];
	*version = '\0';
	snprintf(version, sizeof(version), "%d", oval_definition_get_version(definition));
	xmlNewProp(definition_node, BAD_CAST "version", BAD_CAST version);

	oval_definition_class_t class = oval_definition_get_class(definition);
	xmlNewProp(definition_node, BAD_CAST "class", BAD_CAST oval_definition_class_text(class));

	bool deprecated = oval_definition_get_deprecated(definition);
	if (deprecated)
		xmlNewProp(definition_node, BAD_CAST "deprecated", BAD_CAST "true");

	xmlNode *metadata_node = xmlNewTextChild(definition_node, ns_definitions, BAD_CAST "metadata", NULL);

	char *title = oval_definition_get_title(definition);
	xmlNewTextChild(metadata_node, ns_definitions, BAD_CAST "title", BAD_CAST title);

	struct oval_affected_iterator *affecteds = oval_definition_get_affected(definition);
	while (oval_affected_iterator_has_more(affecteds)) {
		xmlNode *affected_node = xmlNewTextChild(metadata_node, ns_definitions, BAD_CAST "affected", NULL);
		struct oval_affected *affected = oval_affected_iterator_next(affecteds);
		oval_affected_family_t family = oval_affected_get_family(affected);
		xmlNewProp(affected_node, BAD_CAST "family", BAD_CAST oval_affected_family_get_text(family));
		struct oval_string_iterator *platforms = oval_affected_get_platforms(affected);
		while (oval_string_iterator_has_more(platforms)) {
			char *platform = oval_string_iterator_next(platforms);
			xmlNewTextChild(affected_node, ns_definitions, BAD_CAST "platform", BAD_CAST platform);
		}
		oval_string_iterator_free(platforms);
		struct oval_string_iterator *products = oval_affected_get_products(affected);
		while (oval_string_iterator_has_more(products)) {
			char *product = oval_string_iterator_next(products);
			xmlNewTextChild(affected_node, ns_definitions, BAD_CAST "product", BAD_CAST product);
		}
		oval_string_iterator_free(products);
	}
	oval_affected_iterator_free(affecteds);

	struct oval_reference_iterator *references = oval_definition_get_references(definition);
	while (oval_reference_iterator_has_more(references)) {
		struct oval_reference *ref = oval_reference_iterator_next(references);
		xmlNode *referenceNode = xmlNewTextChild(metadata_node, ns_definitions, BAD_CAST "reference", NULL);
		char *source = oval_reference_get_source(ref);
		char *ref_id = oval_reference_get_id(ref);
		char *ref_url = oval_reference_get_url(ref);
		xmlNewProp(referenceNode, BAD_CAST "source", BAD_CAST source);
		xmlNewProp(referenceNode, BAD_CAST "ref_id", BAD_CAST ref_id);
		if (ref_url)
			xmlNewProp(referenceNode, BAD_CAST "ref_url", BAD_CAST ref_url);
	}
	oval_reference_iterator_free(references);

	char *description = oval_definition_get_description(definition);
	xmlNewTextChild(metadata_node, ns_definitions, BAD_CAST "description", BAD_CAST description);

	struct oval_string_iterator *notes = oval_definition_get_notes(definition);
	if (oval_string_iterator_has_more(notes)) {
		xmlNode *notes_node = xmlNewTextChild(definition_node, ns_definitions, BAD_CAST "notes", NULL);
		while (oval_string_iterator_has_more(notes)) {
			char *note = oval_string_iterator_next(notes);
			xmlNewTextChild(notes_node, ns_definitions, BAD_CAST "note", BAD_CAST note);
		}
	}
	oval_string_iterator_free(notes);

	docstr = xmlReadDoc(BAD_CAST definition->anyxml, NULL, NULL, 0);
	nodestr = xmlDocGetRootElement(docstr);
	nodelst = xmlDocCopyNode(nodestr, doc, 1);
	xmlAddChildList(metadata_node, nodelst);
	xmlFreeDoc(docstr);

	struct oval_criteria_node *criteria = oval_definition_get_criteria(definition);
	if (criteria)
		oval_criteria_node_to_dom(criteria, doc, definition_node);

	return definition_node;
}

const char * oval_definition_model_supported(void)
{
        return OVAL_SUPPORTED;
}
