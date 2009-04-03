/**
 * @file oval_entity.c
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
#include "oval_agent_api_impl.h"

struct oval_entity {
	oval_entity_type_enum type;
	oval_datatype_enum datatype;
	oval_operation_enum operation;
	int mask;
	oval_entity_varref_type_enum varref_type;
	char *name;
	struct oval_variable *variable;
	struct oval_value *value;
};

int oval_iterator_entity_has_more(struct oval_iterator_entity *oc_entity)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_entity);
}

struct oval_entity *oval_iterator_entity_next(struct oval_iterator_entity
					      *oc_entity)
{
	return (struct oval_entity *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_entity);
}

char *oval_entity_name(struct oval_entity *entity)
{
	return entity->name;
}

oval_entity_type_enum oval_entity_type(struct oval_entity * entity)
{
	return entity->type;
}

oval_datatype_enum oval_entity_datatype(struct oval_entity * entity)
{
	return entity->datatype;
}

oval_operation_enum oval_entity_operation(struct oval_entity * entity)
{
	return entity->operation;
}

int oval_entity_mask(struct oval_entity *entity)
{
	return entity->mask;
}

oval_entity_varref_type_enum oval_entity_varref_type(struct oval_entity *
						     entity)
{
	return entity->varref_type;
}

struct oval_variable *oval_entity_variable(struct oval_entity *entity)
{
	return entity->variable;
}

struct oval_value *oval_entity_value(struct oval_entity *entity)
{
	return entity->value;
}

struct oval_entity *oval_entity_new()
{
	struct oval_entity *entity =
	    (struct oval_entity *)malloc(sizeof(struct oval_entity));
	entity->datatype = OVAL_DATATYPE_UNKNOWN;
	entity->mask = 0;
	entity->operation = OPERATOR_UNKNOWN;
	entity->type = OVAL_ENTITY_TYPE_UNKNOWN;
	entity->name = NULL;
	entity->value = NULL;
	entity->variable = NULL;
	return entity;
}

void oval_entity_free(struct oval_entity *entity)
{
	if (entity->value != NULL)
		oval_value_free(entity->value);
	if (entity->variable != NULL)
		oval_variable_free(entity->variable);
	if (entity->name != NULL)
		free(entity->name);
	free(entity);
}

void set_oval_entity_type(struct oval_entity *entity,
			  oval_entity_type_enum type)
{
	entity->type = type;
}

void set_oval_entity_datatype(struct oval_entity *entity,
			      oval_datatype_enum datatype)
{
	entity->datatype = datatype;
}

void set_oval_entity_operation(struct oval_entity *entity,
			       oval_operation_enum operation)
{
	entity->operation = operation;
}

void set_oval_entity_mask(struct oval_entity *entity, int mask)
{
	entity->mask = mask;
}

void set_oval_entity_varref_type(struct oval_entity *entity,
				 oval_entity_varref_type_enum type)
{
	entity->varref_type = type;
}

void set_oval_entity_variable(struct oval_entity *entity,
			      struct oval_variable *variable)
{
	entity->variable = variable;
}

void set_oval_entity_value(struct oval_entity *entity, struct oval_value *value)
{
	entity->value = value;
}

void set_oval_entity_name(struct oval_entity *entity, char *name)
{
	entity->name = name;
}

//typedef void (*oval_entity_consumer)(struct oval_entity_node*, void*);
int oval_entity_parse_tag(xmlTextReaderPtr reader,
			  struct oval_parser_context *context,
			  oval_entity_consumer consumer, void *user)
{
	struct oval_entity *entity = oval_entity_new();
	int return_code;
	oval_datatype_enum datatype =
	    oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
	oval_operator_enum operation =
	    oval_operation_parse(reader, "operation", OPERATION_EQUALS);
	int mask = oval_parser_boolean_attribute(reader, "mask", 1);
	oval_entity_type_enum type = OVAL_ENTITY_TYPE_UNKNOWN;	//TODO
	char *varref = xmlTextReaderGetAttribute(reader, "var_ref");
	struct oval_value *value;
	struct oval_variable *variable;
	char *name = xmlTextReaderName(reader);
	oval_entity_varref_type_enum varref_type;
	if (strcmp(name, "var_ref") == 0) {	//special case for <var_ref>
		if (varref == NULL) {
			struct oval_object_model *model =
			    oval_parser_context_model(context);
			void consume_varref(char *varref, void *null) {
				variable = get_oval_variable(model, varref);
			}
			varref_type = OVAL_ENTITY_VARREF_ELEMENT;
			return_code =
			    oval_parser_text_value(reader, context,
						   &consume_varref, NULL);
		} else {
			struct oval_object_model *model =
			    oval_parser_context_model(context);
			variable = get_oval_variable(model, varref);
			varref_type = OVAL_ENTITY_VARREF_ATTRIBUTE;
			return_code = 1;
		}
		value = NULL;
	} else if (varref == NULL) {
		variable = NULL;
		void consume_value(struct oval_value *use_value, void *null) {
			value = use_value;
		}
		varref_type = OVAL_ENTITY_VARREF_NONE;
		return_code =
		    oval_value_parse_tag(reader, context, &consume_value, NULL);
	} else {
		struct oval_object_model *model =
		    oval_parser_context_model(context);
		variable = get_oval_variable_new(model, varref);
		varref_type = OVAL_ENTITY_VARREF_ATTRIBUTE;
		value = NULL;
		return_code = 1;
	}
	set_oval_entity_name(entity, name);
	set_oval_entity_type(entity, type);
	set_oval_entity_datatype(entity, datatype);
	set_oval_entity_operation(entity, operation);
	set_oval_entity_mask(entity, mask);
	set_oval_entity_varref_type(entity, varref_type);
	set_oval_entity_variable(entity, variable);
	set_oval_entity_value(entity, value);
	(*consumer) (entity, user);
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: oval_entity_parse_tag::parse of <%s> terminated on error line %d\n",
		     name, line);
	}
	return return_code;
}

void oval_entity_to_print(struct oval_entity *entity, char *indent, int index)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (index == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sENTITY.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sENTITY[%d].", indent, index);

	printf("%sNAME        = %s\n", nxtindent, oval_entity_name(entity));
	printf("%sTYPE        = %d\n", nxtindent, oval_entity_type(entity));
	if (oval_entity_type(entity) > 10) {
		printf("%s<<WARNING::TYPE OUT OF RANGE>>", nxtindent);
	} else {
		printf("%sDATATYPE    = %d\n", nxtindent,
		       oval_entity_datatype(entity));
		printf("%sOPERATION   = %d\n", nxtindent,
		       oval_entity_operation(entity));
		printf("%sMASK        = %d\n", nxtindent,
		       oval_entity_mask(entity));
		printf("%sVARREF_TYPE = %d\n", nxtindent,
		       oval_entity_varref_type(entity));

		struct oval_variable *variable = oval_entity_variable(entity);
		if (variable == NULL)
			printf("%sVARIABLE    = <<NOT SET>>\n", nxtindent);
		else
			oval_variable_to_print(variable, nxtindent, 0);

		struct oval_value *value = oval_entity_value(entity);
		if (value == NULL)
			printf("%sVALUE       = <<NOT SET>>\n", nxtindent);
		else
			oval_value_to_print(value, nxtindent, 0);
	}
}
