/**
 * @file oval_definition.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
#include "oval_string_map_impl.h"
#include "oval_agent_api_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"

#define OVAL_SUPPORTED "5.5"

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
	xmlNode *metadata;
	struct oval_criteria_node *criteria;
} oval_definition_t;

static const struct oscap_string_map OVAL_DEFINITION_CLASS_MAP[] = {
	{OVAL_CLASS_COMPLIANCE, "compliance"},
	{OVAL_CLASS_INVENTORY, "inventory"},
	{OVAL_CLASS_MISCELLANEOUS, "miscellaneous"},
	{OVAL_CLASS_PATCH, "patch"},
	{OVAL_CLASS_VULNERABILITY, "vulnerability"},
	{OVAL_CLASS_UNKNOWN, NULL}
};

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

oval_definition_class_t oval_definition_get_class(struct oval_definition
						  *definition)
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

xmlNode *oval_definition_get_metadata(struct oval_definition *definition)
{
	__attribute__nonnull__(definition);

	return definition->metadata;
}

struct oval_criteria_node *oval_definition_get_criteria(struct oval_definition
							*definition)
{
	__attribute__nonnull__(definition);

	return ((struct oval_definition *)definition)->criteria;
}

struct oval_definition *oval_definition_new(struct oval_definition_model *model, const char *id)
{
	struct oval_definition *definition;
	xmlNode *root;

	if (model && oval_definition_model_is_locked(model)) {
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
		return NULL;
	}

	definition = (struct oval_definition *)oscap_alloc(sizeof(oval_definition_t));;
	if (definition == NULL)
		return NULL;

	definition->id = oscap_strdup(id);
	definition->version = 0;
	definition->class = OVAL_CLASS_UNKNOWN;
	definition->deprecated = 0;
	definition->title = NULL;
	definition->description = NULL;
	definition->affected = oval_collection_new();
	definition->reference = oval_collection_new();
	definition->notes = oval_collection_new();
	root = xmlDocGetRootElement(oval_definition_model_get_metadata_doc(model));
	definition->metadata = xmlNewChild(root, NULL, BAD_CAST "metadata", NULL);
	definition->criteria = NULL;
	definition->model = model;

	oval_definition_model_add_definition(model, definition);

	return definition;
}

bool oval_definition_is_valid(struct oval_definition * definition)
{
	struct oval_criteria_node *criteria_node;

	if (definition == NULL) {
		oscap_dlprintf(DBG_W, "Argument is not valid: NULL.\n");
		return false;
	}

	if (oval_definition_get_class(definition) == OVAL_CLASS_UNKNOWN) {
		oscap_dlprintf(DBG_W, "Argument is not valid: class == OVAL_CLASS_UNKNOWN.\n");
		return false;
	}

	criteria_node = oval_definition_get_criteria(definition);
	if (oval_criteria_node_is_valid(criteria_node) != true)
		return false;

	return true;
}

bool oval_definition_is_locked(struct oval_definition * definition)
{
	__attribute__nonnull__(definition);

	return oval_definition_model_is_locked(definition->model);
}

struct oval_definition *oval_definition_clone
    (struct oval_definition_model *new_model, struct oval_definition *old_definition) {
	__attribute__nonnull__(old_definition);

	struct oval_definition *new_definition = oval_definition_model_get_definition(new_model, old_definition->id);
	if (new_definition == NULL) {
		xmlNode *root, *metadata;

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

		metadata = xmlDocCopyNode(old_definition->metadata, oval_definition_model_get_metadata_doc(new_model), 1);
		root = xmlDocGetRootElement(oval_definition_model_get_metadata_doc(new_model));
		xmlAddChild(root, metadata);

		oval_definition_set_criteria(new_definition,
					     oval_criteria_node_clone(new_model, old_definition->criteria));
	}
	return new_definition;
}

void oval_definition_free(struct oval_definition *definition)
{
	__attribute__nonnull__(definition);

	if (definition->id != NULL)
		oscap_free(definition->id);
	if (definition->title != NULL)
		oscap_free(definition->title);
	if (definition->description != NULL)
		oscap_free(definition->description);
	if (definition->criteria != NULL)
		oval_criteria_node_free(definition->criteria);
	oval_collection_free_items(definition->affected, (oscap_destruct_func) oval_affected_free);
	oval_collection_free_items(definition->reference, (oscap_destruct_func) oval_reference_free);
	oval_collection_free_items(definition->notes, (oscap_destruct_func) oscap_free);
	xmlUnlinkNode(definition->metadata);
	xmlFreeNode(definition->metadata);

	definition->affected = NULL;
	definition->criteria = NULL;
	definition->description = NULL;
	definition->id = NULL;
	definition->reference = NULL;
	definition->notes = NULL;
	definition->metadata = NULL;
	definition->title = NULL;
	oscap_free(definition);
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
	if (definition && !oval_definition_is_locked(definition)) {
		definition->version = version;
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_definition_set_class(struct oval_definition *definition, oval_definition_class_t class)
{
	if (definition && !oval_definition_is_locked(definition)) {
		definition->class = class;
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_definition_set_deprecated(struct oval_definition *definition, bool deprecated)
{
	if (definition && !oval_definition_is_locked(definition)) {
		definition->deprecated = deprecated;
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_definition_set_title(struct oval_definition *definition, char *title)
{
	if (definition && !oval_definition_is_locked(definition)) {
		if (definition->title != NULL)
			oscap_free(definition->title);
		definition->title = (title == NULL) ? NULL : oscap_strdup(title);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_definition_set_description(struct oval_definition *definition, char *description)
{
	if (definition && !oval_definition_is_locked(definition)) {
		if (definition->description)
			oscap_free(definition->description);
		definition->description = (description == NULL) ? NULL : oscap_strdup(description);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_definition_set_criteria(struct oval_definition *definition, struct oval_criteria_node *criteria)
{
	if (definition && !oval_definition_is_locked(definition)) {
		definition->criteria = criteria;
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_definition_add_affected(struct oval_definition *definition, struct oval_affected *affected)
{
	if (definition && !oval_definition_is_locked(definition)) {
		oval_collection_add(definition->affected, affected);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_definition_add_reference(struct oval_definition *definition, struct oval_reference *ref)
{
	if (definition && !oval_definition_is_locked(definition)) {
		oval_collection_add(definition->reference, ref);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_definition_add_note(struct oval_definition *definition, char *note) {
	if (definition && !oval_definition_is_locked(definition)) {
		oval_collection_add(definition->notes, note);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

static oval_definition_class_t _odaclass(char *class)
{
	return oscap_string_to_enum(OVAL_DEFINITION_CLASS_MAP, class);
}

static const char *oval_definition_class_text(oval_definition_class_t class)
{
	return OVAL_DEFINITION_CLASS_MAP[class - 1].string;
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
		oscap_free(title);
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
		oscap_free(description);
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
		return_code = oval_parser_text_value(reader, context, &_oval_definition_title_consumer, definition);
	} else if (strcmp(tagname, "description") == 0) {
		return_code =
		    oval_parser_text_value(reader, context, &_oval_definition_description_consumer, definition);
	} else if (strcmp(tagname, "affected") == 0) {
		return_code = oval_affected_parse_tag(reader, context, &_oval_definition_affected_consumer, definition);
	} else if (strcmp(tagname, "oval_repository") == 0) {	/* NOOP */
		return_code = oval_parser_skip_tag(reader, context);
	} else if (strcmp(tagname, "reference") == 0) {
		return_code = oval_reference_parse_tag(reader, context, &oval_reference_consume, definition);
	} else {
		char *str;
		xmlDoc *doc;
		xmlNode *node;

		str = (char *) xmlTextReaderReadOuterXml(reader);
		doc = xmlReadDoc(BAD_CAST str, NULL, NULL, 0);
		xmlFree(str);

		node = xmlDocGetRootElement(doc);
		// todo: reattach instead of copying?
		node = xmlDocCopyNode(node, oval_definition_model_get_metadata_doc(definition->model), 1);
		xmlAddChild(definition->metadata, node);
		xmlFreeDoc(doc);

		return_code = oval_parser_skip_tag(reader, context);
	}
	oscap_free(tagname);
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
		int depth = xmlTextReaderDepth(reader);
		if (depth == -1)
			oscap_setxmlerr(xmlGetLastError());
		oscap_dlprintf(DBG_I, "Skipping tag: %s, depth: %d, line: %d.\n", tagname, depth,
                              xmlTextReaderGetParserLineNumber(reader));
		return_code = oval_parser_skip_tag(reader, context);
	}
	oscap_free(tagname);
	return return_code;
}

int oval_definition_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context)
{
	struct oval_definition_model *model = oval_parser_context_model(context);
	char *id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	struct oval_definition *definition = oval_definition_get_new(model, id);
	oscap_free(id);
	id = NULL;

	char *version = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	oval_definition_set_version(definition, atoi(version));
	oscap_free(version);
	version = NULL;

	char *class = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "class");
	oval_definition_set_class(definition, _odaclass(class));
	oscap_free(class);
	class = NULL;

	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	oval_definition_set_deprecated(definition, deprecated);
	int return_code = oval_parser_parse_tag(reader, context, &_oval_definition_parse_tag,
						definition);
	return return_code;
}

void oval_definition_to_print(struct oval_definition *definition, char *indent, int idx)
{
	__attribute__nonnull__(definition);

	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sDEFINITION.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sDEFINITION[%d].", indent, idx);

	oscap_dprintf("%sID          = %s\n", nxtindent, definition->id);
	oscap_dprintf("%sVERSION     = %d\n", nxtindent, definition->version);
	oscap_dprintf("%sCLASS       = %d\n", nxtindent, definition->class);
	oscap_dprintf("%sDEPRECATED  = %d\n", nxtindent, definition->deprecated);
	oscap_dprintf("%sTITLE       = %s\n", nxtindent, definition->title);
	oscap_dprintf("%sDESCRIPTION = %s\n", nxtindent, definition->description);
	struct oval_iterator *affecteds = oval_collection_iterator(definition->affected);
	for (idx = 1; oval_collection_iterator_has_more(affecteds); idx++) {
		void *affected = oval_collection_iterator_next(affecteds);
		oval_affected_to_print(affected, nxtindent, idx);
	}
	oval_collection_iterator_free(affecteds);
	struct oval_iterator *references = oval_collection_iterator(definition->reference);
	for (idx = 1; oval_collection_iterator_has_more(references); idx++) {
		void *ref = oval_collection_iterator_next(references);
		oval_reference_to_print(ref, nxtindent, idx);
	}
	oval_collection_iterator_free(references);
	if (definition->criteria != NULL)
		oval_criteria_node_to_print(definition->criteria, nxtindent, 0);
}

xmlNode *oval_definition_to_dom(struct oval_definition *definition, xmlDoc * doc, xmlNode * parent)
{
	xmlNode *nodelst;
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

	nodelst = xmlDocCopyNodeList(doc, definition->metadata->children);
	xmlAddChildList(metadata_node, nodelst);

	struct oval_criteria_node *criteria = oval_definition_get_criteria(definition);
	if (criteria)
		oval_criteria_node_to_dom(criteria, doc, definition_node);

	return definition_node;
}

const char * oval_definition_model_supported(void)
{
        return OVAL_SUPPORTED;
}
