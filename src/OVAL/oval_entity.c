/**
 * @file oval_entity.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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

#include <libxml/tree.h>

#include "oval_definitions_impl.h"
#include "adt/oval_collection_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"

#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"

/***************************************************************************/
/* Variable definitions
 * */

struct oval_entity {
	struct oval_definition_model *model;
	oval_entity_type_t type;
	oval_datatype_t datatype;
	oval_operation_t operation;
	int mask;
	oval_entity_varref_type_t varref_type;
	char *name;
	struct oval_variable *variable;
	struct oval_value *value;
	bool xsi_nil;				///< @xsi:nil boolean attribute
};

struct oval_consume_varref_context {
	struct oval_definition_model *model;
	struct oval_variable **variable;
	struct oval_value **value;
};

/* End of variable definitions
 * */
/***************************************************************************/
static inline bool oval_entity_get_xsi_nil(const struct oval_entity *entity);
static inline void oval_entity_set_xsi_nil(struct oval_entity *entity, bool xsi_nil);


bool oval_entity_iterator_has_more(struct oval_entity_iterator *oc_entity)
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
	__attribute__nonnull__(entity);

	return entity->name;
}

oval_entity_type_t oval_entity_get_type(struct oval_entity * entity)
{
	__attribute__nonnull__(entity);

	return entity->type;
}

oval_datatype_t oval_entity_get_datatype(struct oval_entity * entity)
{
	__attribute__nonnull__(entity);

	return entity->datatype;
}

oval_operation_t oval_entity_get_operation(struct oval_entity * entity)
{
	__attribute__nonnull__(entity);

	return entity->operation;
}

int oval_entity_get_mask(struct oval_entity *entity)
{
	__attribute__nonnull__(entity);

	return entity->mask;
}

static inline bool oval_entity_get_xsi_nil(const struct oval_entity *entity)
{
	__attribute__nonnull__(entity);
	return entity->xsi_nil;
}

oval_entity_varref_type_t oval_entity_get_varref_type(struct oval_entity * entity)
{
	__attribute__nonnull__(entity);

	return entity->varref_type;
}

struct oval_variable *oval_entity_get_variable(struct oval_entity *entity)
{
	__attribute__nonnull__(entity);

	return entity->variable;
}

struct oval_value *oval_entity_get_value(struct oval_entity *entity)
{
	__attribute__nonnull__(entity);

	return entity->value;
}

struct oval_entity *oval_entity_new(struct oval_definition_model *model)
{
	struct oval_entity *entity = (struct oval_entity *)oscap_alloc(sizeof(struct oval_entity));
	if (entity == NULL)
		return NULL;

	entity->datatype = OVAL_DATATYPE_UNKNOWN;
	entity->mask = 0;
	entity->xsi_nil = 0;
	entity->operation = OVAL_OPERATION_UNKNOWN;
	entity->type = OVAL_ENTITY_TYPE_UNKNOWN;
	entity->name = NULL;
	entity->value = NULL;
	entity->variable = NULL;
	entity->model = model;
	return entity;
}

struct oval_entity *oval_entity_clone(struct oval_definition_model *new_model, struct oval_entity *old_entity) {
	struct oval_entity *new_entity = oval_entity_new(new_model);
	oval_datatype_t datatype = oval_entity_get_datatype(old_entity);
	oval_entity_set_datatype(new_entity, datatype);
	int mask = oval_entity_get_mask(old_entity);
	oval_entity_set_mask(new_entity, mask);
	bool xsi_nil = oval_entity_get_xsi_nil(old_entity);
	oval_entity_set_xsi_nil(new_entity, xsi_nil);
	char *name = oval_entity_get_name(old_entity);
	oval_entity_set_name(new_entity, name);
	oval_operation_t operation = oval_entity_get_operation(old_entity);
	oval_entity_set_operation(new_entity, operation);
	oval_entity_type_t type = oval_entity_get_type(old_entity);
	oval_entity_set_type(new_entity, type);
	struct oval_value *value = oval_entity_get_value(old_entity);
	if (value) {
		oval_entity_set_value(new_entity, oval_value_clone(value));
	}
	struct oval_variable *old_variable = oval_entity_get_variable(old_entity);
	if (old_variable) {
		oval_entity_set_variable(new_entity, oval_variable_clone(new_model, old_variable));
	}
	oval_entity_varref_type_t reftype = oval_entity_get_varref_type(old_entity);
	oval_entity_set_varref_type(new_entity, reftype);
	return new_entity;
}

void oval_entity_free(struct oval_entity *entity)
{
	__attribute__nonnull__(entity);

	if (entity->value != NULL)
		oval_value_free(entity->value);
	if (entity->name != NULL)
		oscap_free(entity->name);

	entity->name = NULL;
	entity->value = NULL;
	entity->variable = NULL;
	oscap_free(entity);
}

void oval_entity_set_type(struct oval_entity *entity, oval_entity_type_t type)
{
	__attribute__nonnull__(entity);
	entity->type = type;
}

void oval_entity_set_datatype(struct oval_entity *entity, oval_datatype_t datatype)
{
	__attribute__nonnull__(entity);
	entity->datatype = datatype;
}

void oval_entity_set_operation(struct oval_entity *entity, oval_operation_t operation)
{
	__attribute__nonnull__(entity);
	entity->operation = operation;
}

void oval_entity_set_mask(struct oval_entity *entity, int mask)
{
	__attribute__nonnull__(entity);
	entity->mask = mask;
}

static inline void oval_entity_set_xsi_nil(struct oval_entity *entity, bool xsi_nil)
{
	__attribute__nonnull__(entity);
	entity->xsi_nil = xsi_nil;
}

void oval_entity_set_varref_type(struct oval_entity *entity, oval_entity_varref_type_t type)
{
	__attribute__nonnull__(entity);
	entity->varref_type = type;
}

void oval_entity_set_variable(struct oval_entity *entity, struct oval_variable *variable)
{
	__attribute__nonnull__(entity);
	entity->variable = variable;
}

void oval_entity_set_value(struct oval_entity *entity, struct oval_value *value)
{
	__attribute__nonnull__(entity);
	entity->value = value;
}

void oval_entity_set_name(struct oval_entity *entity, char *name)
{
	__attribute__nonnull__(entity);
	if (entity->name != NULL)
		oscap_free(entity->name);
	entity->name = (name == NULL) ? NULL : oscap_strdup(name);
}

static void oval_consume_varref(char *varref, void *user)
{

	__attribute__nonnull__(user);

	struct oval_consume_varref_context *ctx = user;
	*(ctx->variable) = oval_definition_model_get_new_variable((struct oval_definition_model *)ctx->model, varref, OVAL_VARIABLE_UNKNOWN);
	*(ctx->value) = oval_value_new(OVAL_DATATYPE_STRING, varref);
}

static void oval_consume_value(struct oval_value *use_value, void *value)
{

	*(struct oval_value **)value = use_value;
}

//typedef void (*oval_entity_consumer)(struct oval_entity_node*, void*);
int oval_entity_parse_tag(xmlTextReaderPtr reader,
			  struct oval_parser_context *context, oscap_consumer_func consumer, void *user)
{
	__attribute__nonnull__(context);

	struct oval_entity *entity = oval_entity_new(context->definition_model);
	int return_code = 0;
	oval_datatype_t datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
	oval_operation_t operation = oval_operation_parse(reader, "operation", OVAL_OPERATION_EQUALS);
	int mask = oval_parser_boolean_attribute(reader, "mask", 0);

	char *nil_attr = (char *) xmlTextReaderGetAttributeNs(reader, BAD_CAST "nil", OVAL_XMLNS_XSI);
	int xsi_nil = oscap_streq(nil_attr, "true") != 0;
	xmlFree(nil_attr);

	oval_entity_type_t type = OVAL_ENTITY_TYPE_UNKNOWN;
	//The value of the type field vs. the complexity of extracting type is arguable
	char *varref = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "var_ref");
	struct oval_value *value = NULL;
	struct oval_variable *variable;
	char *name = (char *)xmlTextReaderLocalName(reader);
	oval_entity_varref_type_t varref_type;
	if (strcmp(name, "var_ref") == 0) {	//special case for <var_ref>
		if (varref == NULL) {
			struct oval_definition_model *model = context->definition_model;
			varref_type = OVAL_ENTITY_VARREF_ELEMENT;
			struct oval_consume_varref_context ctx = {.model = model, .variable = &variable, .value = &value};
			return_code = oval_parser_text_value(reader, context, &oval_consume_varref, &ctx);
		} else {
			struct oval_definition_model *model = context->definition_model;
			if (oval_version_cmp(oval_definition_model_get_schema_version(model), OVAL_VERSION(5.6)) > 0) {
				oscap_seterr(OSCAP_EFAMILY_OVAL, "The var_ref attribute for the var_ref entity "
						"of a variable_object is prohibited since OVAL 5.6. Use plain "
						"var_ref instead.");
			}
			variable = oval_definition_model_get_variable(model, varref);
			if (variable == NULL) {
				oscap_seterr(OSCAP_EFAMILY_OVAL,
						"Could not found variable '%s' referenced by var_ref element.", varref);
				return_code = 1;
			} else {
				varref_type = OVAL_ENTITY_VARREF_ATTRIBUTE;
				oscap_free(varref);
				varref = NULL;
				value = NULL;
			}
		}
	} else if (varref == NULL) {
		variable = NULL;
		varref_type = OVAL_ENTITY_VARREF_NONE;
		if (datatype == OVAL_DATATYPE_RECORD) {
			value = NULL;
		} else {
			if (xsi_nil)
				value = NULL;
			else
				return_code = oval_value_parse_tag(reader, context, &oval_consume_value, &value);
		}
	} else {
		struct oval_definition_model *model = context->definition_model;
		variable = oval_definition_model_get_new_variable(model, varref, OVAL_VARIABLE_UNKNOWN);
		varref_type = OVAL_ENTITY_VARREF_ATTRIBUTE;
		value = NULL;
		oscap_free(varref);
		varref = NULL;
	}
	oval_entity_set_name(entity, name);
	oval_entity_set_type(entity, type);
	oval_entity_set_datatype(entity, datatype);
	oval_entity_set_operation(entity, operation);
	oval_entity_set_mask(entity, mask);
	oval_entity_set_varref_type(entity, varref_type);
	oval_entity_set_variable(entity, variable);
	oval_entity_set_value(entity, value);
	oval_entity_set_xsi_nil(entity, xsi_nil);
	(*consumer) (entity, user);

	if (return_code != 0) {
		dW("Parsing of <%s> terminated by an error at line %d.\n", name, xmlTextReaderGetParserLineNumber(reader));
	}

	oscap_free(name);
	return return_code;
}

xmlNode *oval_entity_to_dom(struct oval_entity *entity, xmlDoc * doc, xmlNode * parent) {
	
	xmlNsPtr ent_ns = parent->ns;
	xmlNodePtr root_node = xmlDocGetRootElement(doc);

	xmlNode *entity_node = NULL;
	char *content = NULL;

	struct oval_variable *variable = oval_entity_get_variable(entity);
	oval_entity_varref_type_t vtype = oval_entity_get_varref_type(entity);
	struct oval_value *value = oval_entity_get_value(entity);

	if (vtype == OVAL_ENTITY_VARREF_ELEMENT) {
		content = oval_variable_get_id(variable);
	} else if (value) {
		content = oval_value_get_text(value);
	}

	char *tagname = oval_entity_get_name(entity);
	bool mask = oval_entity_get_mask(entity);

	/* omit the value and operation used for testing in oval_results if mask=true */
	if (mask && !xmlStrcmp(root_node->name, BAD_CAST OVAL_ROOT_ELM_RESULTS)) {
		entity_node = xmlNewTextChild(parent, ent_ns, BAD_CAST tagname, BAD_CAST "");
	} else {
		entity_node = xmlNewTextChild(parent, ent_ns, BAD_CAST tagname, BAD_CAST content);
		oval_operation_t operation = oval_entity_get_operation(entity);
		if (operation != OVAL_OPERATION_EQUALS)
			xmlNewProp(entity_node, BAD_CAST "operation", BAD_CAST oval_operation_get_text(operation));
		if (oscap_streq(content, "") && oval_entity_get_xsi_nil(entity)) {
			// Look-up xsi namespace pointer. We can be pretty sure that this namespace
			// is defined in root element, because it usually carries xsi:schemaLocation
			// attribute.
			xmlNsPtr ns_xsi = xmlSearchNsByHref(doc, xmlDocGetRootElement(doc), OVAL_XMLNS_XSI);
			if (ns_xsi == NULL) {
				assert(ns_xsi != NULL); // Spot this in testing
				ns_xsi = xmlNewNs(xmlDocGetRootElement(doc), OVAL_XMLNS_XSI, BAD_CAST "xsi");
			}
			// Export @xsi:nil="true" only if it was imported and the content is still empty
			xmlNewNsProp(entity_node, ns_xsi, BAD_CAST "nil", BAD_CAST "true");
		}
	}

	oval_datatype_t datatype = oval_entity_get_datatype(entity);
	if (datatype != OVAL_DATATYPE_STRING)
		xmlNewProp(entity_node, BAD_CAST "datatype", BAD_CAST oval_datatype_get_text(datatype));
	if (mask)
		xmlNewProp(entity_node, BAD_CAST "mask", BAD_CAST "true");
	if (vtype == OVAL_ENTITY_VARREF_ATTRIBUTE)
		xmlNewProp(entity_node, BAD_CAST "var_ref", BAD_CAST oval_variable_get_id(variable));

	return entity_node;
}
