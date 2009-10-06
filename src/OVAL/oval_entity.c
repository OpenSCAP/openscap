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
	oval_entity_type_t type;
	oval_datatype_t datatype;
	oval_operation_t operation;
	int mask;
	oval_entity_varref_type_t varref_type;
	char *name;
	struct oval_variable *variable;
	struct oval_value *value;
};

int oval_entity_iterator_has_more(struct oval_entity_iterator *oc_entity)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_entity);
}

struct oval_entity *oval_entity_iterator_next(struct oval_entity_iterator
					      *oc_entity)
{
	return (struct oval_entity *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_entity);
}

void oval_entity_iterator_free(struct oval_entity_iterator
					      *oc_entity)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_entity);
}

char *oval_entity_get_name(struct oval_entity *entity)
{
	return entity->name;
}

oval_entity_type_t oval_entity_get_type(struct oval_entity * entity)
{
	return entity->type;
}

oval_datatype_t oval_entity_get_datatype(struct oval_entity * entity)
{
	return entity->datatype;
}

oval_operation_t oval_entity_get_operation(struct oval_entity * entity)
{
	return entity->operation;
}

int oval_entity_get_mask(struct oval_entity *entity)
{
	return entity->mask;
}

oval_entity_varref_type_t oval_entity_get_varref_type(struct oval_entity *
						     entity)
{
	return entity->varref_type;
}

struct oval_variable *oval_entity_get_variable(struct oval_entity *entity)
{
	return entity->variable;
}

struct oval_value *oval_entity_get_value(struct oval_entity *entity)
{
	return entity->value;
}

struct oval_entity *oval_entity_new()
{
	struct oval_entity *entity =
	    (struct oval_entity *)malloc(sizeof(struct oval_entity));
	entity->datatype = OVAL_DATATYPE_UNKNOWN;
	entity->mask = 0;
	entity->operation = OVAL_OPERATOR_UNKNOWN;
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
	if (entity->name != NULL)
		free(entity->name);

	entity->name = NULL;
	entity->value = NULL;
	entity->variable = NULL;
	free(entity);
}

void oval_entity_set_type(struct oval_entity *entity,
			  oval_entity_type_t type)
{
	entity->type = type;
}

void oval_entity_set_datatype(struct oval_entity *entity,
			      oval_datatype_t datatype)
{
	entity->datatype = datatype;
}

static void set_oval_entity_operation(struct oval_entity *entity,
			       oval_operation_t operation)
{
	entity->operation = operation;
}

void oval_entity_set_mask(struct oval_entity *entity, int mask)
{
	entity->mask = mask;
}

void oval_entity_set_varref_type(struct oval_entity *entity,
				 oval_entity_varref_type_t type)
{
	entity->varref_type = type;
}

void oval_entity_set_variable(struct oval_entity *entity,
			      struct oval_variable *variable)
{
	entity->variable = variable;
}

void oval_entity_set_value(struct oval_entity *entity, struct oval_value *value)
{
	entity->value = value;
}

static void set_oval_entity_name(struct oval_entity *entity, char *name)
{
	if(entity->name!=NULL)free(entity->name);
	entity->name = name==NULL?NULL:strdup(name);
}

struct oval_consume_varref_context {
	struct oval_definition_model * model;
	struct oval_variable **variable;
};
static void oval_consume_varref(char *varref, void *user) {
	struct oval_consume_varref_context* ctx = user;
	*(ctx->variable) = oval_definition_model_get_variable((struct oval_definition_model *)ctx->model, varref);
}
static void oval_consume_value(struct oval_value *use_value, void *value) {
	*(struct oval_value **)value = use_value;
}

//typedef void (*oval_entity_consumer)(struct oval_entity_node*, void*);
int oval_entity_parse_tag(xmlTextReaderPtr reader,
			  struct oval_parser_context *context,
			  oscap_consumer_func consumer, void *user)
{
	struct oval_entity *entity = oval_entity_new();
	int return_code;
	oval_datatype_t datatype =
	    oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
	oval_operator_t operation =
	    oval_operation_parse(reader, "operation", OVAL_OPERATION_EQUALS);
	int mask = oval_parser_boolean_attribute(reader, "mask", 1);
	oval_entity_type_t type = OVAL_ENTITY_TYPE_UNKNOWN;
	//The value of the type field vs. the complexity of extracting type is arguable
	char *varref = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "var_ref");
	struct oval_value *value = NULL;
	struct oval_variable *variable;
	char *name = (char*) xmlTextReaderLocalName(reader);
	oval_entity_varref_type_t varref_type;
	if (strcmp(name, "var_ref") == 0) {	//special case for <var_ref>
		if (varref == NULL) {
			struct oval_definition_model *model =
			    oval_parser_context_model(context);
			varref_type = OVAL_ENTITY_VARREF_ELEMENT;
			struct oval_consume_varref_context ctx = { .model = model, .variable = &variable };
			return_code =
			    oval_parser_text_value(reader, context,
						   &oval_consume_varref, &ctx);
		} else {
			struct oval_definition_model *model =
			    oval_parser_context_model(context);
			variable = oval_definition_model_get_variable(model, varref);
			varref_type = OVAL_ENTITY_VARREF_ATTRIBUTE;
			return_code = 1;
			free(varref);varref=NULL;
		}
		value = NULL;
	} else if (varref == NULL) {
		variable = NULL;
		varref_type = OVAL_ENTITY_VARREF_NONE;
		return_code =
		    oval_value_parse_tag(reader, context, &oval_consume_value, &value);
	} else {
		struct oval_definition_model *model =
		    oval_parser_context_model(context);
		variable = get_oval_variable_new(model, varref, OVAL_VARIABLE_UNKNOWN);
		varref_type = OVAL_ENTITY_VARREF_ATTRIBUTE;
		value = NULL;
		return_code = 1;
		free(varref);varref = NULL;
	}
	set_oval_entity_name(entity, name);
	oval_entity_set_type(entity, type);
	oval_entity_set_datatype(entity, datatype);
	set_oval_entity_operation(entity, operation);
	oval_entity_set_mask(entity, mask);
	oval_entity_set_varref_type(entity, varref_type);
	oval_entity_set_variable(entity, variable);
	oval_entity_set_value(entity, value);
	(*consumer) (entity, user);
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: oval_entity_parse_tag::parse of <%s> terminated on error line %d\n",
		     name, line);
	}
	free(name);
	return return_code;
}

void oval_entity_to_print(struct oval_entity *entity, char *indent, int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sENTITY.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sENTITY[%d].", indent, idx);

	printf("%sNAME        = %s\n", nxtindent, oval_entity_get_name(entity));
	printf("%sTYPE        = %d\n", nxtindent, oval_entity_get_type(entity));
	if (oval_entity_get_type(entity) > 10) {
		printf("%s<<WARNING::TYPE OUT OF RANGE>>", nxtindent);
	} else {
		printf("%sDATATYPE    = %d\n", nxtindent,
		       oval_entity_get_datatype(entity));
		printf("%sOPERATION   = %d\n", nxtindent,
		       oval_entity_get_operation(entity));
		printf("%sMASK        = %d\n", nxtindent,
		       oval_entity_get_mask(entity));
		printf("%sVARREF_TYPE = %d\n", nxtindent,
		       oval_entity_get_varref_type(entity));

		struct oval_variable *variable = oval_entity_get_variable(entity);
		if (variable == NULL)
			printf("%sVARIABLE    = <<NOT SET>>\n", nxtindent);
		else
			oval_variable_to_print(variable, nxtindent, 0);

		struct oval_value *value = oval_entity_get_value(entity);
		if (value == NULL)
			printf("%sVALUE       = <<NOT SET>>\n", nxtindent);
		else
			oval_value_to_print(value, nxtindent, 0);
	}
}

xmlNode *oval_entity_to_dom
	(struct oval_entity *entity, xmlDoc *doc, xmlNode *parent)
{
	char *tagname = oval_entity_get_name(entity);
	xmlNs *ns_family = *xmlGetNsList(doc, parent);

	struct oval_variable *variable = oval_entity_get_variable(entity);
	oval_entity_varref_type_t vtype = oval_entity_get_varref_type(entity);
	struct oval_value    *value = oval_entity_get_value      (entity);

	char *content = NULL;
	if(variable && vtype==OVAL_ENTITY_VARREF_ELEMENT){
		content = oval_variable_get_id(variable);
	}else if(value){
		content = oval_value_get_text(value);
	}

	xmlNode *entity_node = xmlNewChild(parent, ns_family, tagname, content);

	oval_datatype_t datatype = oval_entity_get_datatype(entity);
	if(datatype!=OVAL_DATATYPE_STRING)
		xmlNewProp(entity_node, BAD_CAST "datatype", oval_datatype_get_text(datatype));
	oval_operation_t operation = oval_entity_get_operation(entity);
	if(operation!=OVAL_OPERATION_EQUALS)
		xmlNewProp(entity_node, BAD_CAST "operation", oval_operation_get_text(operation));
	bool mask = oval_entity_get_mask(entity);
	if(mask)
		xmlNewProp(entity_node, BAD_CAST "mask", BAD_CAST "true");
	if(vtype==OVAL_ENTITY_VARREF_ATTRIBUTE)
		xmlNewProp(entity_node, BAD_CAST "var_ref", oval_variable_get_id(variable));
	return entity_node;
}
