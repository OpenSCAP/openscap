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
	oval_datatype_enum datatype;
	char *text;
} oval_value_t;

int oval_iterator_value_has_more(struct oval_iterator_value *oc_value)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_value);
}

struct oval_value *oval_iterator_value_next(struct oval_iterator_value
					    *oc_value)
{
	return (struct oval_value *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_value);
}

oval_datatype_enum oval_value_datatype(struct oval_value *value)
{
	return (value)->datatype;
}

char *oval_value_text(struct oval_value *value)
{
	return value->text;
}

unsigned char *oval_value_binary(struct oval_value *value)
{
	return NULL;		//TODO
}

bool oval_value_boolean(struct oval_value *value)
{
	if( strncmp("false", (value)->text, 5) )
		return true;
	return false;
}

float oval_value_float(struct oval_value *value)
{
	char *endptr;
	return strtof( (const char *) value->text, &endptr);
}

long oval_value_integer(struct oval_value *value)
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

void oval_value_free(struct oval_value *value)
{
	if (value) {
		free(value->text);
		value->text = NULL;
		free(value);
	}
}

void set_oval_value_datatype(struct oval_value *value,
			     oval_datatype_enum datatype)
{
	value->datatype = datatype;
}

void set_oval_value_text(struct oval_value *value, char *text)
{
	if(value->text!=NULL)free(value->text);
	value->text = text;
}

void oval_value_parse_tag_consume_text(char *string, void *text) {
	*(char**)text = strdup(string);
}

int oval_value_parse_tag(xmlTextReaderPtr reader,
			 struct oval_parser_context *context,
			 oval_value_consumer consumer, void *user)
{
	struct oval_value *value = oval_value_new();
	int return_code;
	oval_datatype_enum datatype =
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
	set_oval_value_datatype(value, datatype);
	set_oval_value_text(value, text);
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


	printf("%sDATATYPE = %d\n", nxtindent, oval_value_datatype(value));
	printf("%sTEXT     = %s\n", nxtindent, oval_value_text(value));
}
