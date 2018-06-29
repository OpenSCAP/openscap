/**
 * @file cpedict_priv.c
 * \brief Interface to Common Platform Enumeration (CPE) Language
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
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
 *      Maros Barabas <mbarabas@redhat.com>
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/xmlreader.h>
#include <stdlib.h>
// for functions isspace
#include <ctype.h>
// for functins memset, strcpy
#include <string.h>
#include <stdbool.h>

#include "public/cpe_dict.h"
#include "public/cpe_name.h"
#include "cpedict_ext_priv.h"
#include "cpedict_priv.h"
#include "cpe_ctx_priv.h"

#include "common/list.h"
#include "common/elements.h"
#include "common/text_priv.h"
#include "common/util.h"
#include "common/_error.h"
#include "common/xmlns_priv.h"
#include "common/xmltext_priv.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

/***************************************************************************/
/* Variable definitions
 * */

const char *PART_TO_CHAR[] = { NULL, "h", "o", "a" };

/* ****************************************
 * CPE-List structures
 * ***************************************/

/* <cpe-item>
 * */
struct cpe_item {		// the node <cpe-item>

	struct cpe_name *name;	// CPE name as CPE URI
	struct oscap_list *titles;	// titles of cpe-item (can be in various languages)

	bool deprecated;			///< deprecated attribute
	struct cpe_name *deprecated_by;		///< deprecated_by attribute, the CPE name that deprecated this
	char *deprecation_date;	// date of deprecation

	struct oscap_list *references;	// list of references
	struct oscap_list *checks;	// list of checks
	struct oscap_list *notes;	// list of notes - it's the same structure as titles
	struct cpe_item_metadata *metadata;	// element <meta:item-metadata>
	struct cpe23_item *cpe23_item;		///< element <cpe23-item>
	struct {
		bool deprecated:1;		///< Is the deprecated atrtribute specified in XML?
	} export;
};
OSCAP_GETTER(struct cpe_name *, cpe_item, name)
OSCAP_SETTER_GENERIC(cpe_item, const struct cpe_name *, name, cpe_name_free, )
OSCAP_GETTER(struct cpe_name *, cpe_item, deprecated_by)
OSCAP_SETTER_GENERIC(cpe_item, const struct cpe_name *, deprecated_by, cpe_name_free, )
OSCAP_ACCESSOR_STRING(cpe_item, deprecation_date)
OSCAP_GETTER(struct cpe_item_metadata *, cpe_item, metadata)
OSCAP_IGETINS_GEN(cpe_reference, cpe_item, references, reference)
OSCAP_IGETINS_GEN(cpe_check, cpe_item, checks, check)
OSCAP_IGETINS(oscap_text, cpe_item, titles, title)
OSCAP_IGETINS(oscap_text, cpe_item, notes, note)
OSCAP_ITERATOR_REMOVE_F(cpe_reference) OSCAP_ITERATOR_REMOVE_F(cpe_check)

struct cpe_notes {				///< representation of <notes> element
	char *lang;				///< xml:lang attribute
	struct oscap_list *notes;		///< list of inner <note> elements
};

/* <cpe-item><item-metadata>
 * */
struct cpe_item_metadata {
	char *modification_date;
	char *status;
	char *nvd_id;
	char *deprecated_by_nvd_id;
};
OSCAP_ACCESSOR_STRING(cpe_item_metadata, modification_date)
    OSCAP_ACCESSOR_STRING(cpe_item_metadata, status)
    OSCAP_ACCESSOR_STRING(cpe_item_metadata, nvd_id)
    OSCAP_ACCESSOR_STRING(cpe_item_metadata, deprecated_by_nvd_id)

/* <cpe-item><check>
 * */
struct cpe_check {
	char *system;		// system check URI
	char *href;		// external file reference (NULL if not present)
	char *identifier;	// test identifier
};
OSCAP_ACCESSOR_STRING(cpe_check, system)
    OSCAP_ACCESSOR_STRING(cpe_check, href)
    OSCAP_ACCESSOR_STRING(cpe_check, identifier)

/* <cpe-item><references><reference>
 * */
struct cpe_reference {
	char *href;		// reference URL
	char *content;		// reference description
};
OSCAP_ACCESSOR_STRING(cpe_reference, href)
    OSCAP_ACCESSOR_STRING(cpe_reference, content)

/* <generator>
 * */
struct cpe_generator {
	char *product_name;	// generator software name
	char *product_version;	// generator software version
	char *schema_version;	// generator schema version
	char *timestamp;	// generation date and time
};
OSCAP_ACCESSOR_STRING(cpe_generator, product_name)
    OSCAP_ACCESSOR_STRING(cpe_generator, product_version)
    OSCAP_ACCESSOR_STRING(cpe_generator, schema_version)
    OSCAP_ACCESSOR_STRING(cpe_generator, timestamp)

OSCAP_GETTER(struct cpe_generator *, cpe_dict_model, generator)
OSCAP_ACCESSOR_SIMPLE(int, cpe_dict_model, base_version)
OSCAP_IGETINS_GEN(cpe_item, cpe_dict_model, items, item) OSCAP_ITERATOR_REMOVE_F(cpe_item)
OSCAP_IGETINS_GEN(cpe_vendor, cpe_dict_model, vendors, vendor) OSCAP_ITERATOR_REMOVE_F(cpe_vendor)

/* ****************************************
 * Component-tree structures
 * ***************************************/
/* vendor
 * */
struct cpe_vendor {
	char *value;
	struct oscap_list *titles;
	struct oscap_list *products;
};
OSCAP_ACCESSOR_STRING(cpe_vendor, value)
    OSCAP_IGETINS(oscap_text, cpe_vendor, titles, title)
    OSCAP_IGETINS_GEN(cpe_product, cpe_vendor, products, product)
    OSCAP_ITERATOR_REMOVE_F(cpe_product)

/* vendor -> product 
 * */
struct cpe_product {
	char *value;
	cpe_part_t part;
	struct oscap_list *versions;
};
OSCAP_ACCESSOR_STRING(cpe_product, value)
    OSCAP_ACCESSOR_SIMPLE(cpe_part_t, cpe_product, part)
    OSCAP_IGETINS_GEN(cpe_version, cpe_product, versions, version)
    OSCAP_ITERATOR_REMOVE_F(cpe_version)

/* vendor -> product -> version 
 * */
struct cpe_version {
	char *value;
	struct oscap_list *updates;
};
OSCAP_ACCESSOR_STRING(cpe_version, value)
    OSCAP_IGETINS_GEN(cpe_update, cpe_version, updates, update)
    OSCAP_ITERATOR_REMOVE_F(cpe_update)

/* vendor -> product -> version -> update 
 * */
struct cpe_update {
	char *value;
	struct oscap_list *editions;
};
OSCAP_ACCESSOR_STRING(cpe_update, value)
    OSCAP_IGETINS_GEN(cpe_edition, cpe_update, editions, edition)
    OSCAP_ITERATOR_REMOVE_F(cpe_edition)

/* vendor -> product -> version -> update -> edition 
 * */
struct cpe_edition {
	char *value;
	struct oscap_list *languages;
};
OSCAP_ACCESSOR_STRING(cpe_edition, value)
    OSCAP_IGETINS_GEN(cpe_language, cpe_edition, languages, language)
    OSCAP_ITERATOR_REMOVE_F(cpe_language)

/* vendor -> product -> version -> update -> edition -> language
 * */
struct cpe_language {
	char *value;
};
OSCAP_ACCESSOR_STRING(cpe_language, value)

/* End of variable definitions
 * */
/***************************************************************************/
/***************************************************************************/
/* XML string variables definitions
 * */
#define TAG_CHECK_STR               BAD_CAST "check"
#define TAG_NOTES_STR               BAD_CAST "notes"
#define TAG_REFERENCES_STR          BAD_CAST "references"
#define ATTR_DEP_BY_NVDID_STR       BAD_CAST "deprecated-by-nvd-id"
#define ATTR_NVD_ID_STR             BAD_CAST "nvd-id"
#define ATTR_STATUS_STR             BAD_CAST "status"
#define ATTR_MODIFICATION_DATE_STR  BAD_CAST "modification-date"
#define TAG_ITEM_METADATA_STR       BAD_CAST "item-metadata"
#define TAG_REFERENCE_STR           BAD_CAST "reference"
#define TAG_NOTE_STR                BAD_CAST "note"
#define TAG_TITLE_STR               BAD_CAST "title"
#define TAG_CPE_ITEM_STR            BAD_CAST "cpe-item"
#define ATTR_DEPRECATION_DATE_STR   BAD_CAST "deprecation_date"
#define ATTR_DEPRECATED_BY_STR      BAD_CAST "deprecated_by"
#define ATTR_DEPRECATED_STR         BAD_CAST "deprecated"
#define ATTR_NAME_STR               BAD_CAST "name"
/* Generator */
#define TAG_GENERATOR_STR           BAD_CAST "generator"
#define TAG_PRODUCT_STR             BAD_CAST "product"
#define TAG_PRODUCT_NAME_STR        BAD_CAST "product_name"
#define TAG_PRODUCT_VERSION_STR     BAD_CAST "product_version"
#define TAG_SCHEMA_VERSION_STR      BAD_CAST "schema_version"
#define TAG_TIMESTAMP_STR           BAD_CAST "timestamp"
#define TAG_COMPONENT_TREE_STR      BAD_CAST "component-tree"
#define TAG_VENDOR_STR              BAD_CAST "vendor"
#define TAG_CPE_LIST_STR            BAD_CAST "cpe-list"
#define TAG_VERSION_STR             BAD_CAST "version"
#define TAG_UPDATE_STR              BAD_CAST "update"
#define TAG_EDITION_STR             BAD_CAST "edition"
#define TAG_LANGUAGE_STR            BAD_CAST "language"
#define ATTR_VALUE_STR      BAD_CAST "value"
#define ATTR_PART_STR       BAD_CAST "part"
#define ATTR_SYSTEM_STR     BAD_CAST "system"
#define ATTR_HREF_STR       BAD_CAST "href"
#define NS_META_STR         BAD_CAST "meta"
#define ATTR_XML_LANG_STR   BAD_CAST "xml:lang"
#define VAL_TRUE_STR        BAD_CAST "true"
/* End of XML string variables definitions
 * */
/***************************************************************************/
/***************************************************************************/
/* Declaration of static (private to this file) functions
 * These function shoud not be called from outside. For exporting these elements
 * has to call parent element's 
 */
static struct cpe_reference *cpe_reference_parse(xmlTextReaderPtr reader);
static struct cpe_check *cpe_check_parse(xmlTextReaderPtr reader);
static struct cpe_notes *cpe_notes_parse(xmlTextReaderPtr reader);

static void cpe_product_export(const struct cpe_product *product, xmlTextWriterPtr writer);
static void cpe_version_export(const struct cpe_version *version, xmlTextWriterPtr writer);
static void cpe_update_export(const struct cpe_update *update, xmlTextWriterPtr writer);
static void cpe_edition_export(const struct cpe_edition *edition, xmlTextWriterPtr writer);
static void cpe_language_export(const struct cpe_language *language, xmlTextWriterPtr writer);
static void cpe_check_export(const struct cpe_check *check, xmlTextWriterPtr writer);
static void cpe_reference_export(const struct cpe_reference *ref, xmlTextWriterPtr writer);
static void cpe_notes_export(const struct cpe_notes *notes, xmlTextWriterPtr writer);

struct cpe_notes *cpe_notes_new(void);
void cpe_notes_free(struct cpe_notes *notes);

/***************************************************************************/

/* Constructors of CPE structures cpe_*<structure>*_new()
 * More info in representive header file.
 * returns the type of <structure>
 */
struct cpe_dict_model *cpe_dict_model_new()
{

	struct cpe_dict_model *dict;

	dict = malloc(sizeof(struct cpe_dict_model));
	if (dict == NULL)
		return NULL;
	memset(dict, 0, sizeof(struct cpe_dict_model));

	dict->vendors = oscap_list_new();
	dict->items = oscap_list_new();

	dict->base_version = 2; // default to CPE 2.x

	dict->origin_file = 0;

	return dict;
}

struct cpe_item_metadata *cpe_item_metadata_new()
{

	struct cpe_item_metadata *item;

	item = malloc(sizeof(struct cpe_item_metadata));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_item_metadata));

	item->modification_date = NULL;
	item->status = NULL;
	item->nvd_id = NULL;
	item->deprecated_by_nvd_id = NULL;

	return item;
}

struct cpe_item *cpe_item_new()
{

	struct cpe_item *item;

	item = malloc(sizeof(struct cpe_item));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_item));

	item->notes = oscap_list_new();
	item->references = oscap_list_new();
	item->checks = oscap_list_new();
	item->titles = oscap_list_new();

	return item;
}

struct cpe_check *cpe_check_new()
{

	struct cpe_check *item;

	item = malloc(sizeof(struct cpe_check));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_check));

	item->system = NULL;
	item->href = NULL;
	item->identifier = NULL;

	return item;
}

struct cpe_reference *cpe_reference_new()
{

	struct cpe_reference *item;

	item = malloc(sizeof(struct cpe_reference));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_reference));

	item->href = NULL;
	item->content = NULL;

	return item;
}

struct cpe_notes *cpe_notes_new(void)
{
	struct cpe_notes *notes = calloc(1, sizeof(struct cpe_notes));
	notes->notes = oscap_list_new();
	return notes;
}

struct cpe_generator *cpe_generator_new()
{

	struct cpe_generator *item;

	item = malloc(sizeof(struct cpe_generator));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_generator));

	item->product_name = NULL;
	item->product_version = NULL;
	item->schema_version = NULL;
	item->timestamp = NULL;

	return item;
}

struct cpe_vendor *cpe_vendor_new()
{

	struct cpe_vendor *item;

	item = malloc(sizeof(struct cpe_vendor));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_vendor));

	item->value = NULL;
	item->titles = oscap_list_new();
	item->products = oscap_list_new();

	return item;
}

struct cpe_product *cpe_product_new()
{

	struct cpe_product *item;

	item = malloc(sizeof(struct cpe_product));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_product));

	item->versions = oscap_list_new();
	item->value = NULL;

	return item;
}

struct cpe_version *cpe_version_new()
{

	struct cpe_version *item;

	item = malloc(sizeof(struct cpe_version));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_version));

	item->updates = oscap_list_new();
	item->value = NULL;

	return item;
}

struct cpe_update *cpe_update_new()
{

	struct cpe_update *item;

	item = malloc(sizeof(struct cpe_update));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_update));

	item->editions = oscap_list_new();
	item->value = NULL;

	return item;
}

struct cpe_edition *cpe_edition_new()
{

	struct cpe_edition *item;

	item = malloc(sizeof(struct cpe_edition));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_edition));

	item->languages = oscap_list_new();
	item->value = NULL;

	return item;
}

struct cpe_language *cpe_language_new()
{

	struct cpe_language *item;

	item = malloc(sizeof(struct cpe_language));
	if (item == NULL)
		return NULL;
	memset(item, 0, sizeof(struct cpe_language));

	item->value = NULL;

	return item;
}

/* End of CPE structures' contructors
 * */
/***************************************************************************/

/***************************************************************************/
/* Private parsing functions cpe_*<structure>*_parse( xmlTextReaderPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
struct cpe_dict_model *cpe_dict_model_parse(struct cpe_parser_ctx *ctx)
{

	__attribute__nonnull__(ctx);
	xmlTextReaderPtr reader = cpe_parser_ctx_get_reader(ctx);

	struct cpe_dict_model *ret = NULL;
	struct cpe_item *item = NULL;
	struct cpe_vendor *vendor = NULL;
	int next_ret = 1;

	// let's find "<cpe-list>" element
	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CPE_LIST_STR) && (next_ret == 1)) {
		next_ret = xmlTextReaderNextElement(reader);
		// There is no "<cpe-list>" element :(( and we are at the end of file !
		if (next_ret == 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP,
					"There is no \"cpe-list\" element in the provided xml tree !\n");
			return NULL;
		}
	}

	// make sure we exit when we reach this depth again
	int entry_depth = xmlTextReaderDepth(reader);

	// we found cpe-list element, let's roll !
	// allocate memory for cpe_dict so we can fill items and vendors and general structures
	ret = cpe_dict_model_new();
	if (ret == NULL)
		return NULL;

	// let us figure out the base version based on the namespace of cpe-list
	const xmlChar* nsuri = xmlTextReaderConstNamespaceUri(reader);

	if (nsuri && !xmlStrcmp(nsuri, BAD_CAST XMLNS_CPE1D))
	{
		cpe_dict_model_set_base_version(ret, 1);
	}
	else if (nsuri && !xmlStrcmp(nsuri, BAD_CAST XMLNS_CPE2D)) {
		cpe_dict_model_set_base_version(ret, 2);
	}
	else
	{
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't figure out CPE version from namespace URI '%s'. Assuming CPE 2.x.", nsuri);
		// unknown version, let us default to CPE 2.x and hope for the best :-/
		cpe_dict_model_set_base_version(ret, 2);
	}

	// go through elements and switch through actions till end of file..
	next_ret = xmlTextReaderNextElementWE(reader, TAG_CPE_LIST_STR);
	while (next_ret != 0) {
		if (xmlTextReaderDepth(reader) <= entry_depth) {
			// we have reached the end of <cpe-list>
			// this is necessary to make XCCDF CPE integration to work
			break;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR_STR)) {	// <generator> | count = 1
			ret->generator = cpe_generator_parse(ctx);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CPE_ITEM_STR)) {	// <cpe-item> | cout = 0-n
			if ((item = cpe_item_parse(ctx)) == NULL) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to parse cpe-item");
				cpe_dict_model_free(ret);
				return NULL;
			}
			// We got an item !
			if (!cpe_dict_model_add_item(ret, item)) {
				cpe_item_free(item);
				cpe_dict_model_free(ret);
				return NULL;
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VENDOR_STR)) {	// <vendor> | count = 0-n
			vendor = cpe_vendor_parse(reader);
			if (vendor)
				oscap_list_add(ret->vendors, vendor);
		} else
			// TODO: we need to store meta xml data of <component-tree> element
		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_COMPONENT_TREE_STR)) {	// <vendor> | count = 0-n
			// we just need to jump over this element
		} else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unknown XML element in CPE dictionary, local name is '%s'.", xmlTextReaderConstLocalName(reader));
		}

		next_ret = xmlTextReaderNextElementWE(reader, TAG_CPE_LIST_STR);
	}

	return ret;
}

struct cpe_generator *cpe_generator_parse(struct cpe_parser_ctx *ctx)
{

	__attribute__nonnull__(ctx);
	xmlTextReaderPtr reader = cpe_parser_ctx_get_reader(ctx);

	struct cpe_generator *ret = NULL;

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR_STR) &&
	    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

		// we are on "<generator>" element, let's alloc structure
		ret = cpe_generator_new();
		if (ret == NULL)
			return NULL;

		// skip nodes until new element
		xmlTextReaderNextElement(reader);

		while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR_STR) != 0) {

			if ((xmlStrcmp(xmlTextReaderConstLocalName(reader),
				       TAG_PRODUCT_NAME_STR) == 0) &&
			    (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)) {
				ret->product_name = oscap_element_string_copy(reader);
			} else
			    if ((xmlStrcmp(xmlTextReaderConstLocalName(reader),
					   TAG_PRODUCT_VERSION_STR) == 0) &&
				(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)) {
				ret->product_version = oscap_element_string_copy(reader);
			} else
			    if ((xmlStrcmp(xmlTextReaderConstLocalName(reader),
					   TAG_SCHEMA_VERSION_STR) == 0) &&
				(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)) {
				ret->schema_version = oscap_element_string_copy(reader);
				cpe_parser_ctx_set_schema_version(ctx, ret->schema_version);
			} else
			    if ((xmlStrcmp(xmlTextReaderConstLocalName(reader),
					   TAG_TIMESTAMP_STR) == 0) &&
				(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)) {
				ret->timestamp = oscap_element_string_copy(reader);
			} else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP,
						"Unknown XML element in CPE dictionary generator, local name is '%s'.",
						xmlTextReaderConstLocalName(reader));
			}
			// element saved. Let's jump on the very next one node (not element, because we need to 
			// find XML_READER_TYPE_END_ELEMENT node, see "while" condition and the condition below "while"
			xmlTextReaderNextNode(reader);

		}
	}

	return ret;

}

struct cpe_item *cpe_item_parse(struct cpe_parser_ctx *ctx)
{
	__attribute__nonnull__(ctx);
	xmlTextReaderPtr reader = cpe_parser_ctx_get_reader(ctx);

	struct cpe_item *ret = NULL;
	struct cpe_check *check = NULL;
	struct cpe_reference *ref = NULL;
	char *data;

	__attribute__nonnull__(reader);
	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CPE_ITEM_STR) != 0 ||
			xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Found '%s'(%d) node when expecting: '%s'(%d)!",
				xmlTextReaderConstLocalName(reader), xmlTextReaderNodeType(reader),
				TAG_CPE_ITEM_STR, XML_READER_TYPE_ELEMENT);
		return NULL;
	}
	else {
		// we are on "<cpe-item>" element, let's alloc structure
		ret = cpe_item_new();
		if (ret == NULL)
			return NULL;

		// Get a name attribute of cpe-item
		data = (char *)xmlTextReaderGetAttribute(reader, ATTR_NAME_STR);
		if (data != NULL)
			ret->name = cpe_name_new(data);
		free(data);

		// if there is "deprecated", "deprecated_by" and "deprecation_date" in cpe-item element
		// ************************************************************************************
		data = (char *)xmlTextReaderGetAttribute(reader, ATTR_DEPRECATED_STR);
		if (data != NULL) {	// we have a deprecation here !
			ret->deprecated = oscap_string_to_enum(OSCAP_BOOL_MAP, data);
			ret->export.deprecated = true;
			free(data);
		}
		data = (char *)xmlTextReaderGetAttribute(reader, ATTR_DEPRECATED_BY_STR);
		if (data != NULL) {
			if ((ret->deprecated_by = cpe_name_new(data)) == NULL) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to initialize CPE name with '%s'", data);
				free(data);
				free(ret);
				return NULL;
			}
		}
		free(data);

		data = (char *)xmlTextReaderGetAttribute(reader, ATTR_DEPRECATION_DATE_STR);
		ret->deprecation_date = oscap_strdup(data);
		free(data);
		// ************************************************************************************

		xmlTextReaderNextElementWE(reader, TAG_CPE_ITEM_STR);
		// Now it's time to go deaply to cpe-item element and parse it's children
		// Do while there is another cpe-item element. Then return.
		while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CPE_ITEM_STR) != 0) {

			if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
				xmlTextReaderNextNode(reader);
				continue;
			}

			if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TITLE_STR) == 0) {
				oscap_list_add(ret->titles, oscap_text_new_parse(OSCAP_TEXT_TRAITS_PLAIN, reader));
			} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_NOTES_STR) == 0) {
				struct cpe_notes *notes = cpe_notes_parse(reader);
				if (notes != NULL)
					oscap_list_add(ret->notes, notes);
			} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CHECK_STR) == 0) {
				check = cpe_check_parse(reader);
				if (check)
					oscap_list_add(ret->checks, check);
			} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCE_STR) == 0) {
				ref = cpe_reference_parse(reader);
				if (ref)
					oscap_list_add(ret->references, ref);
			} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ITEM_METADATA_STR) == 0) {
				data = (char *)xmlTextReaderGetAttribute(reader, ATTR_MODIFICATION_DATE_STR);
				if ((data == NULL) || ((ret->metadata = cpe_item_metadata_new()) == NULL)) {
					oscap_seterr(OSCAP_EFAMILY_OSCAP,
							"Failed to parse item-metadata element within cpe-item/@name='%s'",
							cpe_name_get_as_str(ret->name));
					cpe_item_free(ret);
					free(data);
					return NULL;
				}
				ret->metadata->modification_date = data;

				data = (char *)xmlTextReaderGetAttribute(reader, ATTR_STATUS_STR);
				if (data)
					ret->metadata->status = data;
				data = (char *)xmlTextReaderGetAttribute(reader, ATTR_NVD_ID_STR);
				if (data)
					ret->metadata->nvd_id = (char *)data;
				data = (char *)xmlTextReaderGetAttribute(reader, ATTR_DEP_BY_NVDID_STR);
				if (data)
					ret->metadata->deprecated_by_nvd_id = (char *)data;
				else
					ret->metadata->deprecated_by_nvd_id = NULL;
				data = NULL;

			} else
			    if ((xmlStrcmp(xmlTextReaderConstLocalName(reader),
					   TAG_REFERENCES_STR) == 0)) {
				// we just need to jump over this element
			} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST TAG_CPE23_ITEM_STR) == 0) {
				if ((ret->cpe23_item = cpe23_item_parse(reader)) == NULL) {
					cpe_item_free(ret);
					return NULL;
				}
			} else {
				return ret;	// <-- we need to return here, because we don't want to jump to next element 
			}
			xmlTextReaderNextElementWE(reader, TAG_CPE_ITEM_STR);
		}
	}

	return ret;
}

static struct cpe_check *cpe_check_parse(xmlTextReaderPtr reader)
{

	struct cpe_check *ret;

	__attribute__nonnull__(reader);

	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CHECK_STR) != 0)
		return NULL;

	if ((ret = malloc(sizeof(struct cpe_check))) == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct cpe_check));

	ret->system = (char *)xmlTextReaderGetAttribute(reader, ATTR_SYSTEM_STR);
	ret->href = (char *)xmlTextReaderGetAttribute(reader, ATTR_HREF_STR);
	ret->identifier = oscap_trim(oscap_element_string_copy(reader));

	return ret;
}

static struct cpe_reference *cpe_reference_parse(xmlTextReaderPtr reader)
{

	struct cpe_reference *ret;

	__attribute__nonnull__(reader);

	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCE_STR) != 0)
		return NULL;

	if ((ret = malloc(sizeof(struct cpe_reference))) == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct cpe_reference));

	ret->href = (char *)xmlTextReaderGetAttribute(reader, ATTR_HREF_STR);
	ret->content = oscap_trim(oscap_element_string_copy(reader));

	return ret;
}

static struct cpe_notes *cpe_notes_parse(xmlTextReaderPtr reader)
{
	__attribute__nonnull__(reader);

	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_NOTES_STR) != 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Found '%s' node when expecting: '%s'!",
				xmlTextReaderConstLocalName(reader), TAG_NOTES_STR);
	}

	struct cpe_notes *notes = cpe_notes_new();
	notes->lang = (char *) xmlTextReaderXmlLang(reader);
	if (xmlTextReaderIsEmptyElement(reader) == 0) { // element contains child nodes
		xmlTextReaderNextNode(reader);
		while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_NOTES_STR) != 0) {
			if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
				xmlTextReaderNextNode(reader);
				continue;
			}

			if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_NOTE_STR) == 0) {
				char *note_text = oscap_element_string_copy(reader);
				oscap_list_add(notes->notes, note_text);
			} else {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unexpected element within notes element: '%s'",
						xmlTextReaderConstLocalName(reader));
				cpe_notes_free(notes);
				return NULL;
			}
			xmlTextReaderNextNode(reader);
		}
	}

	return notes;
}

struct cpe_vendor *cpe_vendor_parse(xmlTextReaderPtr reader)
{

	struct cpe_vendor *ret = NULL;
	struct cpe_product *product = NULL;
	struct cpe_version *version = NULL;
	struct cpe_update *update = NULL;
	struct cpe_edition *edition = NULL;
	struct cpe_language *language = NULL;
	char *data;

	__attribute__nonnull__(reader);

	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VENDOR_STR) != 0)
		return NULL;

	ret = cpe_vendor_new();
	if (ret == NULL)
		return NULL;

	ret->value = (char *)xmlTextReaderGetAttribute(reader, ATTR_VALUE_STR);
	// jump to next element (which should be product)
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VENDOR_STR) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TITLE_STR) == 0) {
			oscap_list_add(ret->titles, oscap_text_new_parse(OSCAP_TEXT_TRAITS_PLAIN, reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STR) == 0) {
			// initialization
			product = cpe_product_new();

			if (product) {
			    product->value = (char *)xmlTextReaderGetAttribute(reader, ATTR_VALUE_STR);

			    data = (char *)xmlTextReaderGetAttribute(reader, ATTR_PART_STR);
			    if (data) {
				if (oscap_strcasecmp((const char *)data, "h") == 0)
				    product->part = CPE_PART_HW;
				else if (oscap_strcasecmp((const char *)data, "o") == 0)
				    product->part = CPE_PART_OS;
				else if (oscap_strcasecmp((const char *)data, "a") == 0)
				    product->part = CPE_PART_APP;
				else {
					oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unknown attribute value of vendor/@part='%s'", data);
				    free(ret);
				    free(data);
				    return NULL;
				}
			    } else {
				product->part = CPE_PART_NONE;
			    }
			    free(data);
			    oscap_list_add(ret->products, product);
			}
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VERSION_STR) == 0) {
			// initialization
			version = cpe_version_new();
			version->value = (char *)xmlTextReaderGetAttribute(reader, ATTR_VALUE_STR);
			oscap_list_add(product->versions, version);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_UPDATE_STR) == 0) {
			// initialization
			update = cpe_update_new();
			update->value = (char *)xmlTextReaderGetAttribute(reader, ATTR_VALUE_STR);
			oscap_list_add(version->updates, update);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_EDITION_STR) == 0) {
			// initialization
			edition = cpe_edition_new();
			edition->value = (char *)xmlTextReaderGetAttribute(reader, ATTR_VALUE_STR);
			oscap_list_add(update->editions, edition);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_LANGUAGE_STR) == 0) {
			// initialization
			language = cpe_language_new();
			language->value = (char *)xmlTextReaderGetAttribute(reader, ATTR_VALUE_STR);
			oscap_list_add(edition->languages, language);
		} else {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unknown XML element withinin CPE vendor element, local name is '%s'.",
				xmlTextReaderConstLocalName(reader));
		}
		xmlTextReaderNextNode(reader);
	}
	return ret;

}

/* End of private parsing functions
 * */
/***************************************************************************/

/***************************************************************************/
/* Private exporting functions cpe_*<structure>*_export( xmlTextWriterPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
void cpe_dict_model_export_xml(const struct cpe_dict_model *dict, const char *file)
{

	__attribute__nonnull__(dict);
	__attribute__nonnull__(file);

	// TODO: add macro to check return value from xmlTextWriter* functions
	xmlTextWriterPtr writer;

	writer = xmlNewTextWriterFilename(file, 0);
	if (writer == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return;
	}

	xmlTextWriterSetIndent(writer, 1);
	xmlTextWriterSetIndentString(writer, BAD_CAST "    ");

	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

	cpe_dict_export(dict, writer);
	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cpe_dict_export(const struct cpe_dict_model *dict, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(dict);
	__attribute__nonnull__(writer);

	const int base_version = cpe_dict_model_get_base_version(dict);

	switch (base_version) {
		case 1:
			xmlTextWriterStartElementNS(writer, NULL, TAG_CPE_LIST_STR, BAD_CAST XMLNS_CPE1D);
			break;
		case 2:
			xmlTextWriterStartElementNS(writer, NULL, TAG_CPE_LIST_STR, BAD_CAST XMLNS_CPE2D);
			break;
		default:
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unknown CPE base version '%i'.", base_version);
			xmlTextWriterStartElementNS(writer, NULL, TAG_CPE_LIST_STR, BAD_CAST "http://open-scap.org/CPE/unknown");
			break;
	}

	xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:meta", BAD_CAST XMLNS_CPE2D_METADATA);
	xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:xsi", BAD_CAST "http://www.w3.org/2001/XMLSchema-instance");
	xmlTextWriterWriteAttribute(writer, BAD_CAST "xsi:schemaLocation", BAD_CAST
			XMLNS_CPE2D_METADATA " http://nvd.nist.gov/schema/cpe-dictionary-metadata_0.2.xsd "
			XMLNS_CPE2D " http://cpe.mitre.org/files/cpe-dictionary_2.1.xsd");

	if (dict->generator) cpe_generator_export(dict->generator, writer);
	OSCAP_FOREACH(cpe_item, item, cpe_dict_model_get_items(dict),
		      // dump its contents to XML tree
		      cpe_item_export(item, writer, base_version);)

	if (base_version >= 2) {
	    // TODO: NEED TO HAVE COMPONENT-TREE STRUCTURE TO GET XML-NAMESPACE 
		if (oscap_list_get_itemcount(dict->vendors) != 0) {
			xmlTextWriterStartElementNS(writer, NULL, TAG_COMPONENT_TREE_STR, BAD_CAST XMLNS_CPE2D_METADATA);
			OSCAP_FOREACH(cpe_vendor, vendor, cpe_dict_model_get_vendors(dict), cpe_vendor_export(vendor, writer);)
			xmlTextWriterEndElement(writer);	//</component-tree>
		}
	}

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cpe_generator_export(const struct cpe_generator *generator, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(generator);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_GENERATOR_STR, NULL);
	if (generator->product_name != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_NAME_STR, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST generator->product_name);
		xmlTextWriterEndElement(writer);
	}
	if (generator->product_version != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_VERSION_STR, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST generator->product_version);
		xmlTextWriterEndElement(writer);
	}
	if (generator->schema_version != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_SCHEMA_VERSION_STR, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST generator->schema_version);
		xmlTextWriterEndElement(writer);
	}
	if (generator->timestamp != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_TIMESTAMP_STR, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST generator->timestamp);
		xmlTextWriterEndElement(writer);
	}
	xmlTextWriterEndElement(writer);	//</gnerator>
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());

}

void cpe_item_export(const struct cpe_item *item, xmlTextWriterPtr writer, int base_version)
{

	char *temp;
	struct oscap_iterator *it;

	__attribute__nonnull__(item);
	__attribute__nonnull__(writer);

	switch (base_version) {
		case 1:
			xmlTextWriterStartElementNS(writer, NULL, TAG_CPE_ITEM_STR, BAD_CAST XMLNS_CPE1D);
			break;
		case 2:
			xmlTextWriterStartElementNS(writer, NULL, TAG_CPE_ITEM_STR, BAD_CAST XMLNS_CPE2D);
			break;
		default:
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unknown CPE base version '%i'.", base_version);
			xmlTextWriterStartElementNS(writer, NULL, TAG_CPE_ITEM_STR, BAD_CAST "http://open-scap.org/CPE/unknown");
			break;
	}

	if (item->name != NULL) {
		temp = cpe_name_get_as_format(item->name, CPE_FORMAT_URI);
		xmlTextWriterWriteAttribute(writer, ATTR_NAME_STR, BAD_CAST temp);
		free(temp);
	}
	if (item->export.deprecated == true) {
		xmlTextWriterWriteAttribute(writer, ATTR_DEPRECATED_STR,
			BAD_CAST oscap_enum_to_string(OSCAP_BOOL_MAP, item->deprecated));
	}
	if (item->deprecated_by != NULL) {
		temp = cpe_name_get_as_format(item->deprecated_by, CPE_FORMAT_URI);
		xmlTextWriterWriteAttribute(writer, ATTR_DEPRECATED_BY_STR, BAD_CAST temp);
		free(temp);
	}
	if (item->deprecation_date != NULL) {
		xmlTextWriterWriteAttribute(writer, ATTR_DEPRECATION_DATE_STR, BAD_CAST item->deprecation_date);
	}

	oscap_textlist_export(cpe_item_get_titles(item), writer, "title");

	it = oscap_iterator_new(item->notes);
	if (oscap_iterator_has_more(it)) {
		cpe_notes_export(oscap_iterator_next(it), writer);
	}

	oscap_iterator_free(it);

	    if (item->metadata != NULL) {
		xmlTextWriterStartElementNS(writer, NS_META_STR, TAG_ITEM_METADATA_STR, NULL);
		if (item->metadata->modification_date != NULL)
			xmlTextWriterWriteAttribute(writer, ATTR_MODIFICATION_DATE_STR,
						    BAD_CAST item->metadata->modification_date);
		if (item->metadata->status != NULL)
			xmlTextWriterWriteAttribute(writer, ATTR_STATUS_STR, BAD_CAST item->metadata->status);
		if (item->metadata->nvd_id != NULL)
			xmlTextWriterWriteAttribute(writer, ATTR_NVD_ID_STR, BAD_CAST item->metadata->nvd_id);
		if (item->metadata->deprecated_by_nvd_id != NULL)
			xmlTextWriterWriteAttribute(writer, ATTR_DEP_BY_NVDID_STR,
						    BAD_CAST item->metadata->deprecated_by_nvd_id);
		xmlTextWriterEndElement(writer);
	}

	it = oscap_iterator_new(item->references);
	if (oscap_iterator_has_more(it)) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_REFERENCES_STR, NULL);
		OSCAP_FOREACH(cpe_reference, ref, cpe_item_get_references(item), cpe_reference_export(ref, writer);)
		    xmlTextWriterEndElement(writer);
	}
	oscap_iterator_free(it);

	OSCAP_FOREACH(cpe_check, check, cpe_item_get_checks(item), cpe_check_export(check, writer);)
	if (item->cpe23_item != NULL) {
		cpe23_item_export(item->cpe23_item, writer);
	}
	    xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());

}

void cpe_vendor_export(const struct cpe_vendor *vendor, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(vendor);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_VENDOR_STR, NULL);
	if (vendor->value != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_VALUE_STR, BAD_CAST vendor->value);

	oscap_textlist_export(cpe_vendor_get_titles(vendor), writer, "title");

	    OSCAP_FOREACH(cpe_product, product, cpe_vendor_get_products(vendor), cpe_product_export(product, writer);)

	    xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

static void cpe_product_export(const struct cpe_product *product, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(product);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_STR, NULL);
	if (product->value != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_VALUE_STR, BAD_CAST product->value);
	if (product->part != CPE_PART_NONE)
		xmlTextWriterWriteAttribute(writer, ATTR_PART_STR, BAD_CAST PART_TO_CHAR[product->part]);

	OSCAP_FOREACH(cpe_version, version, cpe_product_get_versions(product), cpe_version_export(version, writer);)

	    xmlTextWriterEndElement(writer);
}

static void cpe_version_export(const struct cpe_version *version, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(version);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_VERSION_STR, NULL);
	if (version->value != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_VALUE_STR, BAD_CAST version->value);

	OSCAP_FOREACH(cpe_update, update, cpe_version_get_updates(version), cpe_update_export(update, writer);)

	    xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());

}

static void cpe_update_export(const struct cpe_update *update, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(update);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_UPDATE_STR, NULL);
	if (update->value != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_VALUE_STR, BAD_CAST update->value);

	OSCAP_FOREACH(cpe_edition, edition, cpe_update_get_editions(update), cpe_edition_export(edition, writer);)

	    xmlTextWriterEndElement(writer);
}

static void cpe_edition_export(const struct cpe_edition *edition, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(edition);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_EDITION_STR, NULL);
	if (edition->value != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_VALUE_STR, BAD_CAST edition->value);

	OSCAP_FOREACH(cpe_language, language, cpe_edition_get_languages(edition),
		      cpe_language_export(language, writer);)

	    xmlTextWriterEndElement(writer);
}

static void cpe_language_export(const struct cpe_language *language, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(language);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_LANGUAGE_STR, NULL);
	if (language->value != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_VALUE_STR, BAD_CAST language->value);

	xmlTextWriterEndElement(writer);
}

static void cpe_check_export(const struct cpe_check *check, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(check);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_CHECK_STR, NULL);
	if (check->system != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_SYSTEM_STR, BAD_CAST check->system);
	if (check->href != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_HREF_STR, BAD_CAST check->href);
	if (check->identifier != NULL)
		xmlTextWriterWriteString(writer, BAD_CAST check->identifier);
	xmlTextWriterEndElement(writer);
}

static void cpe_reference_export(const struct cpe_reference *ref, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(ref);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_REFERENCE_STR, NULL);
	if (ref->href != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_HREF_STR, BAD_CAST ref->href);
	if (ref->content != NULL)
		xmlTextWriterWriteString(writer, BAD_CAST ref->content);
	xmlTextWriterEndElement(writer);
}

static void cpe_notes_export(const struct cpe_notes *notes, xmlTextWriterPtr writer)
{
	__attribute__nonnull__(notes);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_NOTES_STR, NULL);
	if (notes->lang != NULL)
		xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST notes->lang);
	struct oscap_iterator *it = oscap_iterator_new(notes->notes);
	while (oscap_iterator_has_more(it)) {
		const char *note_text = oscap_iterator_next(it);
		xmlTextWriterStartElementNS(writer, NULL, TAG_NOTE_STR, NULL);
		if (note_text != NULL)
			xmlTextWriterWriteString(writer, BAD_CAST note_text);
		xmlTextWriterEndElement(writer);
	}
	oscap_iterator_free(it);
	xmlTextWriterEndElement(writer);
}

/* End of private export functions
 * */
/***************************************************************************/

/***************************************************************************/
/* Free functions - all are static private, do not use them outside this file
 */
void cpe_dict_model_free(struct cpe_dict_model *dict)
{
	if (dict == NULL)
		return;

	oscap_list_free(dict->items, (oscap_destruct_func) cpe_item_free);
	oscap_list_free(dict->vendors, (oscap_destruct_func) cpe_vendor_free);
	cpe_generator_free(dict->generator);
	free(dict->origin_file);
	free(dict);
}

void cpe_item_free(struct cpe_item *item)
{

	if (item == NULL)
		return;
	cpe_name_free(item->name);
	cpe_name_free(item->deprecated_by);
	free(item->deprecation_date);
	oscap_list_free(item->references, (oscap_destruct_func) cpe_reference_free);
	oscap_list_free(item->checks, (oscap_destruct_func) cpe_check_free);
	oscap_list_free(item->notes, (oscap_destruct_func) cpe_notes_free);
	oscap_list_free(item->titles, (oscap_destruct_func) oscap_text_free);
	cpe_itemmetadata_free(item->metadata);
	cpe23_item_free(item->cpe23_item);
	free(item);
}

void cpe_generator_free(struct cpe_generator *generator)
{

	if (generator == NULL)
		return;

	free(generator->product_name);
	free(generator->product_version);
	free(generator->schema_version);
	free(generator->timestamp);
	free(generator);
}

void cpe_check_free(struct cpe_check *check)
{

	if (check == NULL)
		return;

	free(check->identifier);
	free(check->system);
	free(check->href);
	free(check);
}

void cpe_reference_free(struct cpe_reference *ref)
{

	if (ref == NULL)
		return;

	free(ref->href);
	free(ref->content);
	free(ref);
}

void cpe_notes_free(struct cpe_notes *notes)
{
	if (notes != NULL) {
		oscap_list_free(notes->notes, (oscap_destruct_func) free);
		free(notes->lang);
		free(notes);
	}
}

void cpe_vendor_free(struct cpe_vendor *vendor)
{

	if (vendor == NULL)
		return;

	free(vendor->value);
	oscap_list_free(vendor->titles, (oscap_destruct_func) oscap_text_free);
	oscap_list_free(vendor->products, (oscap_destruct_func) cpe_product_free);
	free(vendor);
}

void cpe_product_free(struct cpe_product *product)
{

	if (product == NULL)
		return;

	free(product->value);
	oscap_list_free(product->versions, (oscap_destruct_func) cpe_version_free);
	free(product);
}

void cpe_version_free(struct cpe_version *version)
{

	if (version == NULL)
		return;

	free(version->value);
	oscap_list_free(version->updates, (oscap_destruct_func) cpe_update_free);
	free(version);
}

void cpe_update_free(struct cpe_update *update)
{

	if (update == NULL)
		return;

	free(update->value);
	oscap_list_free(update->editions, (oscap_destruct_func) cpe_edition_free);
	free(update);
}

void cpe_edition_free(struct cpe_edition *edition)
{

	if (edition == NULL)
		return;

	free(edition->value);
	oscap_list_free(edition->languages, (oscap_destruct_func) cpe_language_free);
	free(edition);
}

void cpe_language_free(struct cpe_language *language)
{

	if (language == NULL)
		return;

	free(language->value);
	free(language);
}

void cpe_itemmetadata_free(struct cpe_item_metadata *meta)
{

	if (meta == NULL)
		return;

	free(meta->modification_date);
	free(meta->status);
	free(meta->nvd_id);
	free(meta->deprecated_by_nvd_id);
	free(meta);
}

/* End of free functions
 * */
/***************************************************************************/
