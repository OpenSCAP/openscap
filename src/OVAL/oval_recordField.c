/*
 * Copyright 2011--2014 Red Hat Inc., Durham, North Carolina.
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
 *      Tomas Heinrich <theinric@redhat.com>
 */

#include <config.h>
#include <libxml/tree.h>
#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_system_characteristics_impl.h"
#include "adt/oval_collection_impl.h"
#include "oval_parser_impl.h"

#include "common/util.h"
#include "common/debug_priv.h"
#include "common/elements.h"

typedef struct oval_record_field {
	oval_record_field_type_t record_field_type;
	char *name;
	char *value;
	oval_datatype_t datatype;
	int mask;
} oval_record_field_t;

typedef struct oval_record_field_STATE {
	oval_record_field_type_t record_field_type;
	char *name;
	char *value;
	oval_datatype_t datatype;
	int mask;
	oval_operation_t operation;
	struct oval_variable *variable;
	oval_check_t var_check;
	oval_check_t ent_check;
} oval_record_field_STATE_t;

typedef struct oval_record_field_ITEM {
	oval_record_field_type_t record_field_type;
	char *name;
	char *value;
	oval_datatype_t datatype;
	int mask;
	oval_syschar_status_t status;
} oval_record_field_ITEM_t;

bool oval_record_field_iterator_has_more(struct oval_record_field_iterator *itr)
{
	return oval_collection_iterator_has_more((struct oval_iterator *) itr);
}

struct oval_record_field *oval_record_field_iterator_next(struct oval_record_field_iterator *itr)
{
	return (struct oval_record_field *)
		oval_collection_iterator_next((struct oval_iterator *) itr);
}

void oval_record_field_iterator_free(struct oval_record_field_iterator *itr)
{
	oval_collection_iterator_free((struct oval_iterator *) itr);
}

struct oval_record_field *oval_record_field_new(oval_record_field_type_t type)
{
	struct oval_record_field *rf;

	switch (type) {
	case OVAL_RECORD_FIELD_STATE:
	{
		struct oval_record_field_STATE *rfs;

		rfs = malloc(sizeof(*rfs));
		if (rfs == NULL)
			return NULL;

		rfs->operation = OVAL_OPERATION_UNKNOWN;
		rfs->variable = NULL;
		rfs->var_check = OVAL_CHECK_UNKNOWN;
		rfs->ent_check = OVAL_CHECK_UNKNOWN;
		rf = (struct oval_record_field *) rfs;
		break;
	}
	case OVAL_RECORD_FIELD_ITEM:
	{
		struct oval_record_field_ITEM *rfi;

		rfi = malloc(sizeof(*rfi));
		if (rfi == NULL)
			return NULL;

		rfi->status = SYSCHAR_STATUS_UNKNOWN;
		rf = (struct oval_record_field *) rfi;
		break;
	}
	default:
		dE("Unsupported record field type: %d.", type);
		return NULL;
	}

	rf->record_field_type = type;
	rf->name = NULL;
	rf->value = NULL;
	rf->datatype = OVAL_DATATYPE_UNKNOWN;
	rf->mask = 0;

	return rf;
}

struct oval_record_field *oval_record_field_clone(struct oval_record_field *old_rf)
{
	struct oval_record_field *new_rf;

	switch (old_rf->record_field_type) {
	case OVAL_RECORD_FIELD_STATE:
	{
		struct oval_record_field_STATE *new_rfs, *old_rfs;

		new_rfs = malloc(sizeof(*new_rfs));
		if (new_rfs == NULL)
			return NULL;

		old_rfs = (struct oval_record_field_STATE *) old_rf;
		new_rfs->operation = old_rfs->operation;
		new_rfs->variable = old_rfs->variable;
		new_rfs->var_check = old_rfs->var_check;
		new_rfs->ent_check = old_rfs->ent_check;
		new_rf = (struct oval_record_field *) new_rfs;
		break;
	}
	case OVAL_RECORD_FIELD_ITEM:
	{
		struct oval_record_field_ITEM *new_rfi, *old_rfi;

		new_rfi = malloc(sizeof(*new_rfi));
		if (new_rfi == NULL)
			return NULL;

		old_rfi = (struct oval_record_field_ITEM *) old_rf;
		new_rfi->status = old_rfi->status;
		new_rf = (struct oval_record_field *) new_rfi;
		break;
	}
	default:
		dE("Unsupported record field type: %d.", old_rf->record_field_type);
		return NULL;
	}

	new_rf->record_field_type = old_rf->record_field_type;
	new_rf->name = oscap_strdup(old_rf->name);
	new_rf->value = oscap_strdup(old_rf->value);
	new_rf->datatype = old_rf->datatype;
	new_rf->mask = old_rf->mask;

	return new_rf;
}

void oval_record_field_free(struct oval_record_field *rf)
{
	if (rf == NULL)
		return;

	if (rf->name != NULL)
		free(rf->name);
	if (rf->value != NULL)
		free(rf->value);

	rf->name = rf->value = NULL;
	free(rf);
}

void oval_record_field_set_name(struct oval_record_field *rf, char *name)
{
	rf->name = oscap_strdup(name);
}

void oval_record_field_set_value(struct oval_record_field *rf, char *value)
{
	rf->value = oscap_strdup(value);
}

void oval_record_field_set_datatype(struct oval_record_field *rf, oval_datatype_t dt)
{
	rf->datatype = dt;
}

void oval_record_field_set_mask(struct oval_record_field *rf, int mask)
{
	rf->mask = mask;
}

void oval_record_field_set_operation(struct oval_record_field *rf, oval_operation_t operation)
{
	if (rf->record_field_type != OVAL_RECORD_FIELD_STATE) {
		dE("Wrong record field type: %d.", rf->record_field_type);
		return;
	}

	((struct oval_record_field_STATE *) rf)->operation = operation;
}

void oval_record_field_set_variable(struct oval_record_field *rf, struct oval_variable *var)
{
	if (rf->record_field_type != OVAL_RECORD_FIELD_STATE) {
		dE("Wrong record field type: %d.", rf->record_field_type);
		return;
	}

	((struct oval_record_field_STATE *) rf)->variable = var;
}

void oval_record_field_set_var_check(struct oval_record_field *rf, oval_check_t var_check)
{
	if (rf->record_field_type != OVAL_RECORD_FIELD_STATE) {
		dE("Wrong record field type: %d.", rf->record_field_type);
		return;
	}

	((struct oval_record_field_STATE *) rf)->var_check = var_check;
}

void oval_record_field_set_ent_check(struct oval_record_field *rf, oval_check_t ent_check)
{
	if (rf->record_field_type != OVAL_RECORD_FIELD_STATE) {
		dE("Wrong record field type: %d.", rf->record_field_type);
		return;
	}

	((struct oval_record_field_STATE *) rf)->ent_check = ent_check;
}

void oval_record_field_set_status(struct oval_record_field *rf, oval_syschar_status_t status)
{
	if (rf->record_field_type != OVAL_RECORD_FIELD_ITEM) {
		dE("Wrong record field type: %d.", rf->record_field_type);
		return;
	}

	((struct oval_record_field_ITEM *) rf)->status = status;
}

oval_record_field_type_t oval_record_field_get_type(struct oval_record_field *rf)
{
	return rf->record_field_type;
}

char *oval_record_field_get_name(struct oval_record_field *rf)
{
	return rf->name;
}

char *oval_record_field_get_value(struct oval_record_field *rf)
{
	return rf->value;
}

oval_datatype_t oval_record_field_get_datatype(struct oval_record_field *rf)
{
	return rf->datatype;
}

int oval_record_field_get_mask(struct oval_record_field *rf)
{
	return rf->mask;
}

oval_operation_t oval_record_field_get_operation(struct oval_record_field *rf)
{
	if (rf->record_field_type != OVAL_RECORD_FIELD_STATE) {
		dE("Wrong record field type: %d.", rf->record_field_type);
		return OVAL_OPERATION_UNKNOWN;
	}

	return ((struct oval_record_field_STATE *) rf)->operation;
}

struct oval_variable *oval_record_field_get_variable(struct oval_record_field *rf)
{
	if (rf->record_field_type != OVAL_RECORD_FIELD_STATE) {
		dE("Wrong record field type: %d.", rf->record_field_type);
		return NULL;
	}

	return ((struct oval_record_field_STATE *) rf)->variable;
}

oval_check_t oval_record_field_get_var_check(struct oval_record_field *rf)
{
	if (rf->record_field_type != OVAL_RECORD_FIELD_STATE) {
		dE("Wrong record field type: %d.", rf->record_field_type);
		return OVAL_CHECK_UNKNOWN;
	}

	return ((struct oval_record_field_STATE *) rf)->var_check;
}

oval_check_t oval_record_field_get_ent_check(struct oval_record_field *rf)
{
	if (rf->record_field_type != OVAL_RECORD_FIELD_STATE) {
		dE("Wrong record field type: %d.", rf->record_field_type);
		return OVAL_CHECK_UNKNOWN;
	}

	return ((struct oval_record_field_STATE *) rf)->ent_check;
}

oval_syschar_status_t oval_record_field_get_status(struct oval_record_field *rf)
{
	if (rf->record_field_type != OVAL_RECORD_FIELD_ITEM) {
		dE("Wrong record field type: %d.", rf->record_field_type);
		return SYSCHAR_STATUS_UNKNOWN;
	}

	return ((struct oval_record_field_ITEM *) rf)->status;
}

static void _oval_record_field_value_consumer(char *value, void *rf)
{
	oval_record_field_set_value(rf, value);
}

int oval_record_field_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context,
				oscap_consumer_func consumer, void *user,
				oval_record_field_type_t record_field_type)
{
	int ret = 0, mask;
	char *name;
	oval_datatype_t datatype;
	struct oval_record_field *rf;

	rf = oval_record_field_new(record_field_type);
	if (rf == NULL)
		return -1;

	name = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "name");
	datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
	mask = oval_parser_boolean_attribute(reader, "mask", 0);

	oval_record_field_set_name(rf, name);
	xmlFree(name);
	oval_record_field_set_datatype(rf, datatype);
	oval_record_field_set_mask(rf, mask);

	switch (record_field_type) {
	case OVAL_RECORD_FIELD_STATE:
	{
		oval_operation_t opr;
		oval_check_t var_check;
		oval_check_t ent_check;
		char *var_ref;

		opr = oval_operation_parse(reader, "operation", OVAL_OPERATION_EQUALS);
		oval_record_field_set_operation(rf, opr);
		var_check = oval_check_parse(reader, "var_check", OVAL_CHECK_ALL);
		oval_record_field_set_var_check(rf, var_check);
		ent_check = oval_check_parse(reader, "entity_check", OVAL_CHECK_ALL);
		oval_record_field_set_ent_check(rf, ent_check);

		var_ref = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "var_ref");
		if (var_ref == NULL) {
			ret = oscap_parser_text_value(reader, &_oval_record_field_value_consumer, rf);
		} else {
			struct oval_definition_model *model;
			struct oval_variable *var;

			model = context->definition_model;
			var = oval_definition_model_get_new_variable(model, var_ref, OVAL_VARIABLE_UNKNOWN);
			oval_record_field_set_variable(rf, var);
			xmlFree(var_ref);
		}
		break;
	}
	case OVAL_RECORD_FIELD_ITEM:
	{
		oval_syschar_status_t status;

		status = oval_syschar_status_parse(reader, "status", SYSCHAR_STATUS_EXISTS);
		oval_record_field_set_status(rf, status);
		ret = oscap_parser_text_value(reader, &_oval_record_field_value_consumer, rf);
		break;
	}
	default:
		dE("Impossible happened.");
	}

	(*consumer) (rf, user);

	return ret;
}

xmlNode *oval_record_field_to_dom(struct oval_record_field *rf, bool parent_mask, xmlDoc *doc, xmlNode *parent, xmlNs *namespace)
{
	char *name, *value;
	bool rf_mask, masked;
	xmlNode *node, *root_node;
	oval_datatype_t datatype;

	if (rf->record_field_type != OVAL_RECORD_FIELD_STATE
	    && rf->record_field_type != OVAL_RECORD_FIELD_ITEM) {
		dE("Unsupported record field type: %d.", rf->record_field_type);
		return NULL;
	}



	root_node = xmlDocGetRootElement(doc);
	name = oval_record_field_get_name(rf);
	rf_mask = oval_record_field_get_mask(rf);
	if (!xmlStrcmp(root_node->name, BAD_CAST OVAL_ROOT_ELM_RESULTS)
	    && (rf_mask || parent_mask)) {
		value = NULL;
		masked = true;
	} else {
		value = oval_record_field_get_value(rf);
		masked = false;
	}

	node = xmlNewTextChild(parent, namespace, BAD_CAST "field", BAD_CAST value);
	xmlNewProp(node, BAD_CAST "name", BAD_CAST name);
	datatype = oval_record_field_get_datatype(rf);
	if (datatype != OVAL_DATATYPE_STRING)
		xmlNewProp(node, BAD_CAST "datatype", BAD_CAST oval_datatype_get_text(datatype));
	if (rf_mask)
		xmlNewProp(node, BAD_CAST "mask", BAD_CAST "true");

	switch (rf->record_field_type) {
	case OVAL_RECORD_FIELD_STATE:
	{
		struct oval_variable *var;
		oval_check_t var_check;
		oval_check_t ent_check;

		if (!masked) {
			oval_operation_t opr;

			opr = oval_record_field_get_operation(rf);
			if (opr != OVAL_OPERATION_EQUALS)
				xmlNewProp(node, BAD_CAST "operation", BAD_CAST oval_operation_get_text(opr));
		}
		var = oval_record_field_get_variable(rf);
		if (var != NULL)
			xmlNewProp(node, BAD_CAST "var_ref", BAD_CAST oval_variable_get_id(var));
		var_check = oval_record_field_get_var_check(rf);
		if (var_check != OVAL_CHECK_ALL)
			xmlNewProp(node, BAD_CAST "var_check", BAD_CAST oval_check_get_text(var_check));
		ent_check = oval_record_field_get_ent_check(rf);
		if (ent_check != OVAL_CHECK_ALL)
			xmlNewProp(node, BAD_CAST "entity_check", BAD_CAST oval_check_get_text(ent_check));
		break;
	}
	case OVAL_RECORD_FIELD_ITEM:
	{
		oval_syschar_status_t status;

		status = oval_record_field_get_status(rf);
		if (status != SYSCHAR_STATUS_EXISTS)
			xmlNewProp(node, BAD_CAST "status", BAD_CAST oval_syschar_status_get_text(status));
		break;
	}
	default:
		dE("Impossible happened.");
	}

	return node;
}
