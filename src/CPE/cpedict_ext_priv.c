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

struct cpe23_item {				///< <cpe23-item> node
	char *name;				///< @name attribute
};

static struct cpe23_item *cpe23_item_new()
{
	return oscap_calloc(1, sizeof(struct cpe23_item));
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
	return item;
}

void cpe23_item_free(struct cpe23_item *item)
{
	oscap_free(item);
}
