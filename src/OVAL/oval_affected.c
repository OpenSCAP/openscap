/**
 * @file oval_affected.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"

#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"

/***************************************************************************/
/* Variable definitions
 * */

typedef struct oval_affected {
	struct oval_definition_model *model;
	oval_affected_family_t family;
	struct oval_collection *platforms;
	struct oval_collection *products;
} oval_affected_t;

static const struct oscap_string_map OVAL_ODAFAMILY_MAP[] = {
	{OVAL_AFCFML_CATOS, "catos"},
	{OVAL_AFCFML_IOS, "ios"},
	{OVAL_AFCFML_MACOS, "macos"},
	{OVAL_AFCFML_PIXOS, "pixos"},
	{OVAL_AFCFML_UNDEFINED, "undefined"},
	{OVAL_AFCFML_UNIX, "unix"},
	{OVAL_AFCFML_WINDOWS, "windows"},
	{OVAL_CLASS_UNKNOWN, NULL}
};

/* End of variable definitions
 * */
/***************************************************************************/

bool oval_affected_iterator_has_more(struct oval_affected_iterator *oc_affected)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_affected);
}

struct oval_affected *oval_affected_iterator_next(struct oval_affected_iterator
						  *oc_affected)
{
	return (struct oval_affected *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_affected);
}

void oval_affected_iterator_free(struct oval_affected_iterator
				 *oc_affected)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_affected);
}

oval_affected_family_t oval_affected_get_family(struct oval_affected *affected)
{
	__attribute__nonnull__(affected);

	return ((struct oval_affected *)affected)->family;
}

struct oval_string_iterator *oval_affected_get_platforms(struct oval_affected *affected)
{
	__attribute__nonnull__(affected);

	return (struct oval_string_iterator *)
	    oval_collection_iterator(affected->platforms);
}

struct oval_string_iterator *oval_affected_get_products(struct oval_affected *affected)
{
	__attribute__nonnull__(affected);

	return (struct oval_string_iterator *)
	    oval_collection_iterator(affected->products);
}

struct oval_affected *oval_affected_new(struct oval_definition_model *model)
{
	struct oval_affected *affected = (struct oval_affected *)oscap_alloc(sizeof(oval_affected_t));
	if (affected == NULL)
		return NULL;

	affected->model = model;
	affected->family = OVAL_AFCFML_UNKNOWN;
	affected->platforms = oval_collection_new();
	affected->products = oval_collection_new();
	return affected;
}

bool oval_affected_is_valid(struct oval_affected * affected)
{
        oscap_dlprintf(DBG_W, "NOOP.\n");
	return true;		//TODO
}

bool oval_affected_is_locked(struct oval_affected * affected)
{
	__attribute__nonnull__(affected);

	return oval_definition_model_is_locked(affected->model);
}

struct oval_affected *oval_affected_clone(struct oval_definition_model *new_model, struct oval_affected *old_affected)
{
	__attribute__nonnull__(old_affected);

	struct oval_affected *new_affected = oval_affected_new(new_model);

	oval_affected_set_family(new_affected, old_affected->family);

	struct oval_string_iterator *platforms = oval_affected_get_platforms(old_affected);
	while (oval_string_iterator_has_more(platforms)) {
		char *platform = oval_string_iterator_next(platforms);
		oval_affected_add_platform(new_affected, platform);
	}
	oval_string_iterator_free(platforms);

	struct oval_string_iterator *products = oval_affected_get_products(old_affected);
	while (oval_string_iterator_has_more(products)) {
		char *product = oval_string_iterator_next(products);
		oval_affected_add_product(new_affected, product);
	}
	oval_string_iterator_free(products);

	return new_affected;
}

void oval_affected_free(struct oval_affected *affected)
{
	__attribute__nonnull__(affected);

	oval_collection_free_items(affected->platforms, (oscap_destruct_func) & oscap_free);
	affected->platforms = NULL;
	oval_collection_free_items(affected->products, (oscap_destruct_func) & oscap_free);
	affected->products = NULL;
	oscap_free(affected);
}

void oval_affected_set_family(struct oval_affected *affected, oval_affected_family_t family)
{
	__attribute__nonnull__(affected);

	if (!oval_affected_is_locked(affected)) {
		affected->family = family;
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_affected_add_platform(struct oval_affected *affected, char *platform)
{
	__attribute__nonnull__(affected);

	if (affected && !oval_affected_is_locked(affected)) {
		oval_collection_add(affected->platforms, (void *)oscap_strdup(platform));
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_affected_add_product(struct oval_affected *affected, char *product)
{
	__attribute__nonnull__(affected);

	if (affected && !oval_affected_is_locked(affected)) {
		oval_collection_add(affected->products, (void *)oscap_strdup(product));
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

static oval_affected_family_t _odafamily(char *family)
{
	return oscap_string_to_enum(OVAL_ODAFAMILY_MAP, family);
}

const char *oval_affected_family_get_text(oval_affected_family_t family)
{
	return OVAL_ODAFAMILY_MAP[family - 1].string;
}

static int _oval_affected_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_affected *affected = (struct oval_affected *)user;
	int return_code;
	xmlChar *tagname = xmlTextReaderLocalName(reader);
	//xmlChar *namespace = xmlTextReaderNamespaceUri(reader);

	if (strcmp((char *)tagname, "platform") == 0) {
		char *platform = NULL;
		return_code = oval_parser_text_value(reader, context, &oval_text_consumer, &platform);
		if (platform != NULL) {
			oval_affected_add_platform(affected, platform);
			oscap_free(platform);
		}
	} else if (strcmp((char *)tagname, "product") == 0) {
		char *product = NULL;
		return_code = oval_parser_text_value(reader, context, &oval_text_consumer, &product);
		if (product != NULL) {
			oval_affected_add_product(affected, product);
			oscap_free(product);
		}
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

int oval_affected_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context, oval_affected_consumer consumer, void *user)
{
	__attribute__nonnull__(context);

	struct oval_affected *affected = oval_affected_new(context->definition_model);
	if (affected == NULL)
		return -1;

	//xmlChar *tagname = xmlTextReaderName(reader);
	//xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	//
	char *family = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "family");
	oval_affected_set_family(affected, _odafamily(family));
	oscap_free(family);
	(*consumer) (affected, user);
	return oval_parser_parse_tag(reader, context, &_oval_affected_parse_tag, affected);
}

void oval_affected_to_print(struct oval_affected *affected, char *indent, int idx)
{
	__attribute__nonnull__(affected);

	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sAFFECTED.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sAFFECTED[%d].", indent, idx);

	printf("%sFAMILY = %d\n", nxtindent, affected->family);
	struct oval_iterator *platforms = oval_collection_iterator(affected->platforms);
	for (idx = 1; oval_collection_iterator_has_more(platforms); idx++) {
		void *platform = oval_collection_iterator_next(platforms);
		printf("%sPLATFORM[%d] = %s\n", nxtindent, idx, (char *)platform);
	}
	oval_collection_iterator_free(platforms);

	struct oval_iterator *products = oval_collection_iterator(affected->products);
	for (idx = 1; oval_collection_iterator_has_more(products); idx++) {
		void *product = oval_collection_iterator_next(products);
		printf("%sPRODUCT[%d] = %s\n", nxtindent, idx, (char *)product);
	}
	oval_collection_iterator_free(products);
}
