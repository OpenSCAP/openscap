/*
 * oval_value.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
	return ((struct oval_value *)value)->text;
}

unsigned char *oval_value_binary(struct oval_value *value)
{
	return NULL;		//TODO
}

char oval_value_boolean(struct oval_value *value)
{
	return 0;		//TODO
}				//datatype==OVAL_DATATYPE_BOOLEAN

float oval_value_float(struct oval_value *value)
{
	return 0;		//TODO
}				//datatype==OVAL_DATATYPE_FLOAT

long oval_value_integer(struct oval_value *value)
{
	return 0;		//TODO
}				//datatype==OVAL_DATATYPE_INTEGER

struct oval_value *oval_value_new()
{
	oval_value_t *value = (oval_value_t *) malloc(sizeof(oval_value_t));
	value->datatype = OVAL_DATATYPE_UNKNOWN;
	value->text = NULL;
	return value;
}

void oval_value_free(struct oval_value *value)
{
	if (value->text != NULL)
		free(value->text);
	free(value);
}

void set_oval_value_datatype(struct oval_value *value,
			     oval_datatype_enum datatype)
{
	value->datatype = datatype;
}

void set_oval_value_text(struct oval_value *value, char *text)
{
	value->text = text;
}

int oval_value_parse_tag(xmlTextReaderPtr reader,
			 struct oval_parser_context *context,
			 oval_value_consumer consumer, void *user)
{
	struct oval_value *value = oval_value_new();
	int return_code;
	oval_datatype_enum datatype =
	    oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
	char *text;
	int isNil = oval_parser_boolean_attribute(reader, "xsi:nil", 0);
	if (isNil) {
		text = NULL;
		return_code = 1;
	} else {
		void consume_text(char *string, void *null) {
			text = string;
		}
		return_code =
		    oval_parser_text_value(reader, context, &consume_text,
					   NULL);
	}
	set_oval_value_datatype(value, datatype);
	set_oval_value_text(value, text);
	(*consumer) (value, user);
	return return_code;
}

void oval_value_to_print(struct oval_value *value, char *indent, int index)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (index == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sVALUE.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sVALUE[%d].", indent, index);


	printf("%sDATATYPE = %d\n", nxtindent, oval_value_datatype(value));
	printf("%sTEXT     = %s\n", nxtindent, oval_value_text(value));
}
