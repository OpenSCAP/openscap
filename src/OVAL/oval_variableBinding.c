/**
 * @file oval_variableBinding.c
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

#include "oval_agent_api_impl.h"
#include "oval_definitions_impl.h"
#include "adt/oval_collection_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/elements.h"

typedef struct oval_variable_binding {
	struct oval_variable *variable;
	struct oval_collection *values;
} oval_variable_binding_t;

bool oval_variable_binding_iterator_has_more(struct
					     oval_variable_binding_iterator
					     *oc_variable_binding)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_variable_binding);
}

struct oval_variable_binding *oval_variable_binding_iterator_next(struct
								  oval_variable_binding_iterator
								  *oc_variable_binding)
{
	return (struct oval_variable_binding *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_variable_binding);
}

void oval_variable_binding_iterator_free(struct
					 oval_variable_binding_iterator
					 *oc_variable_binding)
{
	oval_collection_iterator_free((struct oval_iterator *)
				      oc_variable_binding);
}

struct oval_variable *oval_variable_binding_get_variable(struct
							 oval_variable_binding
							 *binding)
{
	__attribute__nonnull__(binding);

	return ((struct oval_variable_binding *)binding)->variable;
}

struct oval_string_iterator *oval_variable_binding_get_values(struct oval_variable_binding *binding)
{
	__attribute__nonnull__(binding);

	return (struct oval_string_iterator *)
		oval_collection_iterator(binding->values);
}

void oval_variable_binding_set_variable(struct oval_variable_binding *binding, struct oval_variable *variable)
{
	__attribute__nonnull__(binding);

	binding->variable = variable;
}

void oval_variable_binding_add_value(struct oval_variable_binding *binding, char *value)
{
	__attribute__nonnull__(binding);
	__attribute__nonnull__(value);

	oval_collection_add(binding->values, value);
}

struct oval_variable_binding *oval_variable_binding_new(struct oval_variable *variable, char *value)
{
	oval_variable_binding_t *binding = (oval_variable_binding_t *) oscap_alloc(sizeof(oval_variable_binding_t));
	if (binding == NULL)
		return NULL;

	binding->variable = variable;
	binding->values = oval_collection_new();
	if (value != NULL)
		oval_collection_add(binding->values, value);
	return binding;
}

struct oval_variable_binding *oval_variable_binding_clone(struct oval_variable_binding *old_binding,
							  struct oval_definition_model *def_model)
{
	struct oval_variable *old_variable = oval_variable_binding_get_variable(old_binding);
	char *varid = oval_variable_get_id(old_variable);
	struct oval_variable *new_variable = oval_definition_model_get_variable(def_model, varid);
	struct oval_string_iterator *str_itr;
	struct oval_variable_binding *new_binding;

	if (new_variable == NULL)
		oval_variable_clone(def_model, old_variable);
	new_binding = oval_variable_binding_new(new_variable, NULL);

	str_itr = oval_variable_binding_get_values(old_binding);
	while (oval_string_iterator_has_more(str_itr)) {
		char *s;

		s = oval_string_iterator_next(str_itr);
		s = oscap_strdup(s);
		oval_variable_binding_add_value(new_binding, s);
	}
	oval_string_iterator_free(str_itr);

	return new_binding;
}

void oval_variable_binding_free(struct oval_variable_binding *binding)
{
	if (binding) {
		oval_collection_free_items(binding->values, (oscap_destruct_func) free);
		binding->values = NULL;
		binding->variable = NULL;

		free(binding);
	}
}

static void _oval_variable_binding_value_consumer(char *value, void *user)
{
	struct oval_variable_binding *binding = (struct oval_variable_binding *) user;

	value = oscap_strdup(value);
	oval_variable_binding_add_value(binding, value);
}

int oval_variable_binding_parse_tag(xmlTextReaderPtr reader,
				    struct oval_parser_context *context, oval_variable_binding_consumer consumer,
				    void *client)
{
	__attribute__nonnull__(context);

	int return_code = 0;
	struct oval_variable_binding *binding = oval_variable_binding_new(NULL, NULL);
	/* variable */
	char *variableId = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "variable_id");
	struct oval_variable *variable = oval_definition_model_get_new_variable(context->definition_model, variableId, OVAL_VARIABLE_UNKNOWN);
	oval_variable_binding_set_variable(binding, variable);
	free(variableId);

	/* bound value */
	return_code = oscap_parser_text_value(reader, &_oval_variable_binding_value_consumer, binding);

	if (return_code == 0)
		(*consumer) (binding, client);

	return return_code;
}

void oval_variable_binding_to_dom(struct oval_variable_binding *binding, xmlDoc *doc, xmlNode *parent_tag)
{
	struct oval_string_iterator *val_itr;
	xmlNs *ns_syschar;

	__attribute__nonnull__(binding);

	ns_syschar = xmlSearchNsByHref(doc, parent_tag, OVAL_SYSCHAR_NAMESPACE);

	val_itr = oval_variable_binding_get_values(binding);
	while (oval_string_iterator_has_more(val_itr)) {
		char *val;
		xmlNode *binding_tag;
		struct oval_variable *var;

		val = oval_string_iterator_next(val_itr);
		binding_tag = xmlNewTextChild(parent_tag, ns_syschar, BAD_CAST "variable_value", BAD_CAST val);
		var = oval_variable_binding_get_variable(binding);
		xmlNewProp(binding_tag, BAD_CAST "variable_id", BAD_CAST oval_variable_get_id(var));
	}
	oval_string_iterator_free(val_itr);
}
