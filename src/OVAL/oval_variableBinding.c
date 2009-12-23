/**
 * @file oval_variableBinding.c
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
#include "../common/util.h"
#include "../common/public/debug.h"

typedef struct oval_variable_binding {
	struct oval_variable *variable;
	char *value;
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

char *oval_variable_binding_get_value(struct oval_variable_binding *binding)
{
	__attribute__nonnull__(binding);

	return ((struct oval_variable_binding *)binding)->value;
}

void oval_variable_binding_set_variable(struct oval_variable_binding *binding, struct oval_variable *variable)
{
	__attribute__nonnull__(binding);

	binding->variable = variable;
}

void oval_variable_binding_set_value(struct oval_variable_binding *binding, char *value)
{
	__attribute__nonnull__(binding);

	if (binding->value != NULL)
		oscap_free(binding->value);
	binding->value = oscap_strdup(value);
}

struct oval_variable_binding *oval_variable_binding_new(struct oval_variable *variable, char *value)
{
	oval_variable_binding_t *binding = (oval_variable_binding_t *) oscap_alloc(sizeof(oval_variable_binding_t));
	if (binding == NULL)
		return NULL;

	binding->variable = variable;
	binding->value = value;
	return binding;
}

struct oval_variable_binding *oval_variable_binding_clone(struct oval_variable_binding *old_binding,
							  struct oval_definition_model *def_model)
{
	struct oval_variable *old_variable = oval_variable_binding_get_variable(old_binding);
	char *varid = oval_variable_get_id(old_variable);
	struct oval_variable *new_variable = oval_definition_model_get_variable(def_model, varid);
	if (new_variable == NULL)
		oval_variable_clone(def_model, old_variable);

	char *old_value = oval_variable_binding_get_value(old_binding);
	char *new_value = oscap_strdup(old_value);

	return oval_variable_binding_new(new_variable, new_value);
}

static struct oval_variable_binding *_oval_variable_binding_new()
{
	oval_variable_binding_t *binding = (oval_variable_binding_t *) oscap_alloc(sizeof(oval_variable_binding_t));
	if (binding == NULL)
		return NULL;

	binding->variable = NULL;
	binding->value = NULL;
	return binding;
}

void oval_variable_binding_free(struct oval_variable_binding *binding)
{
	if (binding) {
		if (binding->value != NULL)
			oscap_free(binding->value);

		binding->value = NULL;
		binding->variable = NULL;

		oscap_free(binding);
	}
}

static void _oval_variable_binding_value_consumer(char *value, void *user)
{
	oval_variable_binding_set_value((struct oval_variable_binding *)user, value);
}

int oval_variable_binding_parse_tag(xmlTextReaderPtr reader,
				    struct oval_parser_context *context, oval_variable_binding_consumer consumer,
				    void *client)
{
	__attribute__nonnull__(context);

	int return_code = 1;
	struct oval_variable_binding *binding = _oval_variable_binding_new();
	{			//variable
		char *variableId = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "variable_id");
		struct oval_variable *variable =
		    oval_variable_get_new(context->definition_model, variableId, OVAL_VARIABLE_UNKNOWN);
		oval_variable_binding_set_variable(binding, variable);
		oscap_free(variableId);
		variableId = NULL;
	}
	{			//bound value
		return_code = oval_parser_text_value(reader, context, &_oval_variable_binding_value_consumer, binding);
	}
	if (return_code != 1) {
		oscap_dprintf("WARNING: oval_warning_parse_tag:: return code is not 1::(%d)", return_code);
	} else {
		oscap_dprintf("DEBUG: oval_variable_binding_parse_tag::"
			      "\n    binding->variable = %s"
			      "\n    binding->value    = %s",
			      oval_variable_get_id(oval_variable_binding_get_variable(binding)),
			      oval_variable_binding_get_value(binding));
		(*consumer) (binding, client);
	}
	return return_code;
}

void oval_variable_binding_to_dom(struct oval_variable_binding *binding, xmlDoc * doc, xmlNode * tag_parent)
{
	if (binding) {
		xmlNs *ns_syschar = xmlSearchNsByHref(doc, tag_parent, OVAL_SYSCHAR_NAMESPACE);
		xmlNode *tag_variable_binding = xmlNewChild(tag_parent, ns_syschar, BAD_CAST "variable_value",
							    BAD_CAST oval_variable_binding_get_value(binding));

		{		//attributes
			struct oval_variable *variable = oval_variable_binding_get_variable(binding);
			xmlNewProp(tag_variable_binding, BAD_CAST "variable_id",
				   BAD_CAST oval_variable_get_id(variable));
		}
	}
}
