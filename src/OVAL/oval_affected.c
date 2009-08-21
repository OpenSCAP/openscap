/**
 * @file oval_affected.c
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
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"

typedef struct oval_affected {
	oval_affected_family_enum family;
	struct oval_collection *platforms;
	struct oval_collection *products;
} oval_affected_t;

int oval_iterator_affected_has_more(struct oval_iterator_affected *oc_affected)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_affected);
}

struct oval_affected *oval_iterator_affected_next(struct oval_iterator_affected
						  *oc_affected)
{
	return (struct oval_affected *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_affected);
}

oval_affected_family_enum oval_affected_family(struct oval_affected *affected)
{
	return ((struct oval_affected *)affected)->family;
}

struct oval_iterator_string *oval_affected_platform(struct oval_affected
						    *affected)
{
	return (struct oval_iterator_string *)
	    oval_collection_iterator(affected->platforms);
}

struct oval_iterator_string *oval_affected_product(struct oval_affected
						   *affected)
{
	return (struct oval_iterator_string *)
	    oval_collection_iterator(affected->products);
}

struct oval_affected *oval_affected_new()
{
	struct oval_affected *affected =
	    (struct oval_affected *)malloc(sizeof(oval_affected_t));
	affected->family = AFCFML_UNKNOWN;
	affected->platforms = oval_collection_new();
	affected->products = oval_collection_new();
	return affected;
}

void oval_affected_free(struct oval_affected *affected)
{
	oval_collection_free_items(affected->platforms, &free);
	affected->platforms = NULL;
	oval_collection_free_items(affected->products, &free);
	affected->products = NULL;
	free(affected);
}

void set_oval_affected_family(struct oval_affected *affected,
			      oval_affected_family_enum family)
{
	affected->family = family;
}

void add_oval_affected_platform(struct oval_affected *affected, char *platform)
{
	oval_collection_add(affected->platforms, (void *)strdup(platform));
}

void add_oval_affected_product(struct oval_affected *affected, char *product)
{
	oval_collection_add(affected->products, (void *)strdup(product));
}

struct oval_string_map *_odafamilyMap = NULL;
typedef struct _odafamily {
	int value;
} _odafamily_t;
void _odafamily_set(char *name, int val)
{
	_odafamily_t *enumval = (_odafamily_t *) malloc(sizeof(_odafamily_t));
	enumval->value = val;
	oval_string_map_put(_odafamilyMap, name, (void *)enumval);
}

oval_affected_family_enum _odafamily(char *family)
{
	if (_odafamilyMap == NULL) {
		_odafamilyMap = oval_string_map_new();
		_odafamily_set("catos", AFCFML_CATOS);
		_odafamily_set("ios", AFCFML_IOS);
		_odafamily_set("macos", AFCFML_MACOS);
		_odafamily_set("pixos", AFCFML_PIXOS);
		_odafamily_set("undefined", AFCFML_UNDEFINED);
		_odafamily_set("unix", AFCFML_UNIX);
		_odafamily_set("windows", AFCFML_WINDOWS);
	}
	_odafamily_t *valstar =
	    (_odafamily_t *) oval_string_map_get_value(_odafamilyMap, family);
	return (valstar == NULL) ? CLASS_UNKNOWN : valstar->value;
}

int _oval_affected_parse_tag(xmlTextReaderPtr reader,
			     struct oval_parser_context *context, void *user)
{
	struct oval_affected *affected = (struct oval_affected *)user;
	int return_code;
	xmlChar *tagname = xmlTextReaderName(reader);
	//xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	if (strcmp((char *) tagname, "platform") == 0) {
		char *platform = NULL;
		return_code =
		    oval_parser_text_value(reader, context, &oval_text_consumer,&platform);
		if (platform != NULL){
			add_oval_affected_platform(affected, platform);
			free(platform);
		}
	} else if (strcmp((char *) tagname, "product") == 0) {
		char *product = NULL;
		return_code =
		    oval_parser_text_value(reader, context, &oval_text_consumer,&product);
		if (product != NULL){
			add_oval_affected_product(affected, product);
			free(product);
		}
	} else {
		int linno = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE::(oval_affected)skipping <%s> depth = %d line = %d\n",
		     tagname, xmlTextReaderDepth(reader), linno);
		return_code = oval_parser_skip_tag(reader, context);
	}
	return return_code;
}

int oval_affected_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context,
			    oval_affected_consumer consumer, void *user)
{
	struct oval_affected *affected = oval_affected_new();
	//xmlChar *tagname = xmlTextReaderName(reader);
	//xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	char *family = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "family");
	set_oval_affected_family(affected, _odafamily(family));
	free(family);
	(*consumer) (affected, user);
	return oval_parser_parse_tag(reader, context, &_oval_affected_parse_tag,
				     affected);
}

void oval_affected_to_print(struct oval_affected *affected, char *indent,
			    int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sAFFECTED.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sAFFECTED[%d].", indent, idx);

	printf("%sFAMILY = %d\n", nxtindent, affected->family);
	struct oval_iterator *platforms =
	    oval_collection_iterator(affected->platforms);
	for (idx = 1; oval_collection_iterator_has_more(platforms); idx++) {
		void *platform = oval_collection_iterator_next(platforms);
		printf("%sPLATFORM[%d] = %s\n", nxtindent, idx, (char *) platform);
	}
	struct oval_iterator *products =
	    oval_collection_iterator(affected->products);
	for (idx = 1; oval_collection_iterator_has_more(products); idx++) {
		void *product = oval_collection_iterator_next(products);
		printf("%sPRODUCT[%d] = %s\n", nxtindent, idx, (char *) product);
	}
}
