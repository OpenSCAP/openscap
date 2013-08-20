/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/xmlreader.h>

#include "public/cpe_name.h"
#include "cpe_ctx_priv.h"
#include "cpedict_ext_priv.h"

#include "common/_error.h"
#include "common/list.h"
#include "common/xmlns_priv.h"
#include "common/xmltext_priv.h"

#define ATTR_NAME_STR				"name"
#define ATTR_DATE_STR				"date"
#define ATTR_TYPE_STR				"type"
#define TAG_CPE_EXT_DEPRECATION_STR		"deprecation"
#define TAG_CPE_EXT_DEPRECATEDBY_STR		"deprecated-by"

enum cpe_deprecation_type {			///< cpe_dict_ext:deprecationTypeType
	CPE_DEP_NAME_CORRECTION		= 1,
	CPE_DEP_NAME_REMOVAL		= 2,
	CPE_DEP_ADDITIONAL_INFORMATION	= 3,
};

struct cpe_ext_deprecatedby {			///< <cpe_dict_ext:deprecated-by> node
	char *name;				///< @name attribute
	enum cpe_deprecation_type type;		///< @type attribute
};

struct cpe_ext_deprecation {			///< <cpe_dict_ext:deprecation> node
	char *date;				///< @date attribute
	struct oscap_list *deprecatedbys;	///< <deprecated-by> sub-nodes
};

struct cpe23_item {				///< <cpe23-item> node
	char *name;				///< @name attribute
	struct oscap_list *deprecations;	///< <deprecation> sub-nodes
};

static const struct oscap_string_map CPE_EXT_DEPRECATION_MAP[] = {
	{CPE_DEP_NAME_CORRECTION, "NAME_CORRECTION"},
	{CPE_DEP_NAME_REMOVAL, "NAME_REMOVAL"},
	{CPE_DEP_ADDITIONAL_INFORMATION, "ADDITIONAL_INFORMATION"},
	{0, NULL}
};

static void cpe_ext_deprecatedby_free(struct cpe_ext_deprecatedby *deprecatedby);
static void cpe_ext_deprecation_free(struct cpe_ext_deprecation *deprecation);

static struct cpe_ext_deprecatedby *cpe_ext_deprecatedby_new()
{
	return oscap_calloc(1, sizeof(struct cpe_ext_deprecatedby));
}

static struct cpe_ext_deprecation *cpe_ext_deprecation_new()
{
	struct cpe_ext_deprecation *deprecation = oscap_calloc(1, sizeof(struct cpe_ext_deprecation));
	deprecation->deprecatedbys = oscap_list_new();
	return deprecation;
}

static struct cpe23_item *cpe23_item_new()
{
	struct cpe23_item *item = oscap_calloc(1, sizeof(struct cpe23_item));
	item->deprecations = oscap_list_new();
	return item;
}

static struct cpe_ext_deprecatedby *cpe_ext_deprecatedby_parse(xmlTextReaderPtr reader)
{
	__attribute__nonnull__(reader);

	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST TAG_CPE_EXT_DEPRECATEDBY_STR) != 0 ||
				xmlTextReaderNodeType(reader) != 1) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Found '%s' node when expecting: '%s'!",
				xmlTextReaderConstLocalName(reader), TAG_CPE_EXT_DEPRECATEDBY_STR);
	}
	const xmlChar* nsuri = xmlTextReaderConstNamespaceUri(reader);
	if (nsuri && xmlStrcmp(nsuri, BAD_CAST XMLNS_CPE2D3_EXTENSION) != 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Found '%s' namespace when expecting: '%s'!",
				nsuri, XMLNS_CPE2D3_EXTENSION);
		return NULL;
	}

	struct cpe_ext_deprecatedby *deprecatedby = cpe_ext_deprecatedby_new();
	deprecatedby->name = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST ATTR_NAME_STR);
	const char *type = (const char *) xmlTextReaderGetAttribute(reader, BAD_CAST ATTR_TYPE_STR);
	if (type == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Compulsory attribute '%s' missing at '%s' element.",
			ATTR_TYPE_STR, TAG_CPE_EXT_DEPRECATEDBY_STR);
		cpe_ext_deprecatedby_free(deprecatedby);
		return NULL;
	}
	deprecatedby->type = oscap_string_to_enum(CPE_EXT_DEPRECATION_MAP, type);
	return deprecatedby;
}

static struct cpe_ext_deprecation *cpe_ext_deprecation_parse(xmlTextReaderPtr reader)
{
	__attribute__nonnull__(reader);

	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST TAG_CPE_EXT_DEPRECATION_STR) != 0 ||
			xmlTextReaderNodeType(reader) != 1) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Found '%s' node when expecting: '%s'!",
				xmlTextReaderConstLocalName(reader), TAG_CPE_EXT_DEPRECATION_STR);
	}
	const xmlChar* nsuri = xmlTextReaderConstNamespaceUri(reader);
	if (nsuri && xmlStrcmp(nsuri, BAD_CAST XMLNS_CPE2D3_EXTENSION) != 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Found '%s' namespace when expecting: '%s'!",
				nsuri, XMLNS_CPE2D3_EXTENSION);
		return NULL;
	}

	struct cpe_ext_deprecation *deprecation = cpe_ext_deprecation_new();
	deprecation->date = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST ATTR_DATE_STR);
	if (xmlTextReaderIsEmptyElement(reader) == 0) { // the element contains child nodes
		xmlTextReaderNextNode(reader);
		while (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST TAG_CPE_EXT_DEPRECATION_STR) != 0) {
			if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
				xmlTextReaderNextNode(reader);
				continue;
			}

			if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST TAG_CPE_EXT_DEPRECATEDBY_STR) == 0) {
				struct cpe_ext_deprecatedby *deprecatedby = cpe_ext_deprecatedby_parse(reader);
				if (deprecatedby == NULL) {
					cpe_ext_deprecation_free(deprecation);
					return NULL;
				}
				oscap_list_add(deprecation->deprecatedbys, deprecatedby);
			}
			else {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unexpected element within deprecation element: '%s'",
						xmlTextReaderConstLocalName(reader));
				cpe_ext_deprecation_free(deprecation);
				return NULL;
			}
			xmlTextReaderNextNode(reader);
		}
	}
	return deprecation;
}

struct cpe23_item *cpe23_item_parse(xmlTextReaderPtr reader)
{
	__attribute__nonnull__(reader);

	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST TAG_CPE23_ITEM_STR) != 0 ||
			xmlTextReaderNodeType(reader) != 1) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Found '%s' node when expecting: '%s'!",
				xmlTextReaderConstLocalName(reader), TAG_CPE23_ITEM_STR);
		return NULL;
	}
	const xmlChar* nsuri = xmlTextReaderConstNamespaceUri(reader);
	if (nsuri && xmlStrcmp(nsuri, BAD_CAST XMLNS_CPE2D3_EXTENSION) != 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Found '%s' namespace when expecting: '%s'!",
				nsuri, XMLNS_CPE2D3_EXTENSION);
		return NULL;
	}

	struct cpe23_item *item = cpe23_item_new();
	item->name = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST ATTR_NAME_STR);
	if (xmlTextReaderIsEmptyElement(reader) == 0) { // the element contains child nodes
		xmlTextReaderNextNode(reader);
		while (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST TAG_CPE23_ITEM_STR) != 0) {
			if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
				xmlTextReaderNextNode(reader);
				continue;
			}

			if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST TAG_CPE_EXT_DEPRECATION_STR) == 0) {
				struct cpe_ext_deprecation *deprecation = cpe_ext_deprecation_parse(reader);
				if (deprecation == NULL) {
					cpe23_item_free(item);
					return NULL;
				}
				oscap_list_add(item->deprecations, deprecation);
			}
			else {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unexpected element within cpe23-item[@name='%s']: '%s'",
						item->name, xmlTextReaderConstLocalName(reader));
				cpe23_item_free(item);
				return NULL;
			}
			xmlTextReaderNextNode(reader);
		}
	}

	return item;
}

static void cpe_ext_deprecatedby_free(struct cpe_ext_deprecatedby *deprecatedby)
{
	oscap_free(deprecatedby);
}

static void cpe_ext_deprecation_free(struct cpe_ext_deprecation *deprecation)
{
	if (deprecation != NULL) {
		oscap_free(deprecation->date);
		oscap_list_free(deprecation->deprecatedbys, (oscap_destruct_func) cpe_ext_deprecatedby_free);
		oscap_free(deprecation);
	}
}

void cpe23_item_free(struct cpe23_item *item)
{
	if (item != NULL) {
		oscap_list_free(item->deprecations, (oscap_destruct_func) cpe_ext_deprecation_free);
		oscap_free(item);
	}
}

