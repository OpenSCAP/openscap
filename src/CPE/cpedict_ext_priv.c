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
#include "common/xmlns_priv.h"

#define ATTR_NAME_STR				"name"
#define ATTR_DATE_STR				"date"
#define TAG_CPE_EXT_DEPRECATION_STR		"deprecation"

struct cpe_ext_deprecation {			///< <cpe_dict_ext:deprecation> node
	char *date;				///< @date attribute
};

struct cpe23_item {				///< <cpe23-item> node
	char *name;				///< @name attribute
	struct oscap_list *deprecations;	///< <deprecation> sub-nodes
};

static struct cpe_ext_deprecation_new()
{
	return oscap_calloc(1, sizeof(struct cpe_ext_deprecation));
}

static struct cpe23_item *cpe23_item_new()
{
	struct cpe23_item *item = oscap_calloc(1, sizeof(struct cpe23_item));
	item->deprecations = oscap_list_new();
	return item;
}

struct cpe_ext_deprecation cpe_ext_deprecation_parse(xmlTextReaderPtr reader)
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

static void cpe_ext_deprecation_free(struct cpe_ext_deprecation *deprecation)
{
	if (deprecation != NULL) {
		oscap_free(deprecation->date);
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

