/**
 * @file oval_value.c
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
#include <stdbool.h>

#include "oval_definitions_impl.h"
#include "adt/oval_collection_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/elements.h"

typedef struct oval_value {
	oval_datatype_t datatype;
	char *text;
} oval_value_t;

bool oval_value_iterator_has_more(struct oval_value_iterator *oc_value)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_value);
}

struct oval_value *oval_value_iterator_next(struct oval_value_iterator
					    *oc_value)
{
	return (struct oval_value *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_value);
}

void oval_value_iterator_free(struct oval_value_iterator
			      *oc_value)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_value);
}

int oval_value_iterator_remaining(struct oval_value_iterator *iterator)
{
	return oval_collection_iterator_remaining((struct oval_iterator *)iterator);
}

oval_datatype_t oval_value_get_datatype(struct oval_value *value)
{
	__attribute__nonnull__(value);

	return (value)->datatype;
}

char *oval_value_get_text(struct oval_value *value)
{
	__attribute__nonnull__(value);

	return value->text;
}

unsigned char *oval_value_get_binary(struct oval_value *value)
{
	return NULL;		//TODO: implement oval_value_binary
}

bool oval_value_get_boolean(struct oval_value * value)
{
	__attribute__nonnull__(value);

	if (strcmp("false", (value)->text) == 0
	    || strcmp("0", (value)->text) == 0)
		return false;
	return true;
}

float oval_value_get_float(struct oval_value *value)
{
	__attribute__nonnull__(value);

	char *endptr;
	return strtof((const char *)value->text, &endptr);
}

long long oval_value_get_integer(struct oval_value *value)
{
	__attribute__nonnull__(value);

	char *endptr;
	return strtoll((const char *)value->text, &endptr, 10);
}

struct oval_value *oval_value_new(oval_datatype_t datatype, char *text_value)
{
	oval_value_t *value = (oval_value_t *) malloc(sizeof(oval_value_t));
	if (value == NULL)
		return NULL;

	value->datatype = datatype;
	value->text = oscap_strdup(text_value);
	return value;
}


struct oval_value *oval_value_clone(struct oval_value *old_value)
{
	__attribute__nonnull__(old_value);

	struct oval_value *new_value = oval_value_new(old_value->datatype, old_value->text);
	return new_value;
}

void oval_value_free(struct oval_value *value)
{
    if (value == NULL)
        return;

    free(value->text);
    free(value);
}

int oval_value_cast(struct oval_value *value, oval_datatype_t new_dt)
{
	/*
          oval_datatype_t old_dt;

          old_dt = oval_value_get_datatype(value);
        */
	// todo: attempt a proper cast here
	value->datatype = new_dt;

	return 0;
}

void oval_value_set_datatype(struct oval_value *value, oval_datatype_t datatype)
{
	value->datatype = datatype;
}

/*
void oval_value_set_text(struct oval_value *value, char *text)
{
	if(value->text!=NULL)
		free(value->text);
	value->text = oscap_strdup(text);
}
*/

static void oval_value_parse_tag_consume_text(char *string, void *text)
{
	*(char **)text = oscap_strdup(string);
}

int oval_value_parse_tag(xmlTextReaderPtr reader,
			 struct oval_parser_context *context, oval_value_consumer consumer, void *user)
{
	int return_code;
	oval_datatype_t datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
	char *text = NULL;
	int isNil = oval_parser_boolean_attribute(reader, "xsi:nil", 0);
	if (isNil) {
		return_code = 0;
	} else {
		return_code = oscap_parser_text_value(reader, &oval_value_parse_tag_consume_text, &text);
	}
	struct oval_value *value = oval_value_new(datatype, text ? text : "");
	free(text);
	(*consumer) (value, user);
	return return_code;
}

xmlNode *oval_value_to_dom(struct oval_value *value, xmlDoc * doc, xmlNode * parent) {
	return NULL;		//TODO: implement oval_value_to_dom
}
