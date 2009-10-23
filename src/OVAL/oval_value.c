/**
 * @file oval_value.c
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
#include <stdbool.h>
#include "oval_definitions_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_value {
	oval_datatype_t datatype;
	char *text;
} oval_value_t;

int oval_value_iterator_has_more(struct oval_value_iterator *oc_value)
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

oval_datatype_t oval_value_get_datatype(struct oval_value *value)
{
	return (value)->datatype;
}

char *oval_value_get_text(struct oval_value *value)
{
	return value->text;
}

unsigned char *oval_value_get_binary(struct oval_value *value)
{
	return NULL;		//TODO: implement oval_value_binary
}

bool oval_value_get_boolean(struct oval_value *value)
{
	if( strncmp("false", (value)->text, 5) )
		return true;
	return false;
}

float oval_value_get_float(struct oval_value *value)
{
	char *endptr;
	return strtof( (const char *) value->text, &endptr);
}

long oval_value_get_integer(struct oval_value *value)
{
        char *endptr;
        return strtol( (const char *) value->text, &endptr, 10);
}

struct oval_value *oval_value_new()
{
	oval_value_t *value = (oval_value_t *) malloc(sizeof(oval_value_t));
	value->datatype = OVAL_DATATYPE_UNKNOWN;
	value->text = NULL;
	return value;
}

struct oval_value *oval_value_clone(struct oval_value *old_value)
{
	struct oval_value *new_value = oval_value_new();
	oval_datatype_t datatype = oval_value_get_datatype(old_value);
	oval_value_set_datatype(new_value, datatype);
	char *text = oval_value_get_text(old_value);
	oval_value_set_text(new_value, text);
	return new_value;
}

void oval_value_free(struct oval_value *value)
{
 	if (value) {
		free(value->text);
		value->text = NULL;
		free(value);
	}
}

void oval_value_set_datatype(struct oval_value *value,
			     oval_datatype_t datatype)
{
	value->datatype = datatype;
}

void oval_value_set_text(struct oval_value *value, char *text)
{
	if(value->text!=NULL)free(value->text);
	value->text = ((text==NULL)?NULL:strdup(text));
}

void oval_value_set_float(struct oval_value *value, float num) { /* TODO */
}

void oval_value_set_boolean(struct oval_value *value, int b) { /* TODO */
	return;
}

void oval_value_set_binary(struct oval_value *value, unsigned char *num) { /* TODO */
	return;
}

void oval_value_set_integer(struct oval_value *value, long num) { /* TODO */
	return;
}

static void oval_value_parse_tag_consume_text(char *string, void *text) {
	*(char**)text = strdup(string);
}

int oval_value_parse_tag(xmlTextReaderPtr reader,
			 struct oval_parser_context *context,
			 oval_value_consumer consumer, void *user)
{
	struct oval_value *value = oval_value_new();
	int return_code;
	oval_datatype_t datatype =
	    oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
	char *text = NULL;
	int isNil = oval_parser_boolean_attribute(reader, "xsi:nil", 0);
	if (isNil) {
		text = NULL;
		return_code = 1;
	} else {
		return_code =
		    oval_parser_text_value(reader, context, &oval_value_parse_tag_consume_text, &text);
	}
	oval_value_set_datatype(value, datatype);
	oval_value_set_text(value, text);
	free(text);
	(*consumer) (value, user);
	return return_code;
}

void oval_value_to_print(struct oval_value *value, char *indent, int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sVALUE.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sVALUE[%d].", indent, idx);


	printf("%sDATATYPE = %d\n", nxtindent, oval_value_get_datatype(value));
	printf("%sTEXT     = %s\n", nxtindent, oval_value_get_text(value));
}

xmlNode *oval_value_to_dom
	(struct oval_value *value, xmlDoc *doc, xmlNode *parent)
{
	return NULL;//TODO: implement oval_value_to_dom
}
