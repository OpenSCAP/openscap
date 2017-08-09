/**
 * @file oval_filter.c
 * @brief OVAL filter data type implementation
 * @author "Tomas Heinrich" <theinric@redhat.com>
 *
 * @addtogroup OVALDEF
 * @{
 */
/*
 * Copyright 2010--2014 Red Hat Inc., Durham, North Carolina.
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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "common/debug_priv.h"
#include "common/elements.h"
#include "oval_agent_api_impl.h"
#include "oval_definitions_impl.h"

struct oval_filter {
	struct oval_definition_model *model;
	struct oval_state *state;
	oval_filter_action_t action;
};

struct oval_filter *oval_filter_new(struct oval_definition_model *model)
{
	struct oval_filter *filter;

	filter = (struct oval_filter *) oscap_alloc(sizeof (struct oval_filter));
	if (filter == NULL)
		return NULL;

	filter->model = model;
	filter->state = NULL;
	filter->action = OVAL_FILTER_ACTION_UNKNOWN;
	return filter;
}

void oval_filter_free(struct oval_filter *filter)
{
	__attribute__nonnull__(filter);

	filter->model = NULL;
	filter->state = NULL;
	free(filter);
}

struct oval_filter *oval_filter_clone(struct oval_definition_model *new_model,
				      struct oval_filter *old_filter)
{
	struct oval_filter *new_filter;
	struct oval_state *ste;
	oval_filter_action_t fa;

	new_filter = oval_filter_new(new_model);
	ste = oval_filter_get_state(old_filter);
	ste = oval_state_clone(new_model, ste);
	oval_filter_set_state(new_filter, ste);
	fa = oval_filter_get_filter_action(old_filter);
	oval_filter_set_filter_action(new_filter, fa);

	return new_filter;
}

bool oval_filter_iterator_has_more(struct oval_filter_iterator *oc_filter)
{
	return oval_collection_iterator_has_more((struct oval_iterator *) oc_filter);
}

struct oval_filter *oval_filter_iterator_next(struct oval_filter_iterator *oc_filter)
{
	return (struct oval_filter *)
		oval_collection_iterator_next((struct oval_iterator *) oc_filter);
}

void oval_filter_iterator_free(struct oval_filter_iterator *oc_filter)
{
	oval_collection_iterator_free((struct oval_iterator *) oc_filter);
}

struct oval_state *oval_filter_get_state(struct oval_filter *filter)
{
	__attribute__nonnull__(filter);

	return filter->state;
}

oval_filter_action_t oval_filter_get_filter_action(struct oval_filter *filter)
{
	__attribute__nonnull__(filter);

	return filter->action;
}

void oval_filter_set_state(struct oval_filter *filter, struct oval_state *state)
{
	__attribute__nonnull__(filter);
	filter->state = state;
}

void oval_filter_set_filter_action(struct oval_filter *filter, oval_filter_action_t action)
{
	__attribute__nonnull__(filter);
	filter->action = action;
}

static void _oval_filter_consume_ste_ref(char *steref, void *user)
{
	struct oval_filter *filter;
	struct oval_state *ste;

	filter = (struct oval_filter *) user;
	ste = oval_definition_model_get_new_state(filter->model, steref);
	oval_filter_set_state(filter, ste);
}

int oval_filter_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context,
			  oval_filter_consumer consumer, void *user)
{
	struct oval_filter *filter;
	oval_filter_action_t fa;
	int return_code;

	filter = oval_filter_new(context->definition_model);
	fa = oval_filter_action_parse(reader, "action", OVAL_FILTER_ACTION_EXCLUDE);
	oval_filter_set_filter_action(filter, fa);
	return_code = oscap_parser_text_value(reader, &_oval_filter_consume_ste_ref, filter);

	(*consumer) (filter, user);

	return return_code;
}

xmlNode *oval_filter_to_dom(struct oval_filter *filter, xmlDoc *doc, xmlNode *parent)
{
	struct oval_state *ste;
	char *ste_id;
	oval_filter_action_t fact;
	xmlNs *ns_definitions;
	xmlNode *filter_node;

	ste = oval_filter_get_state(filter);
	ste_id = oval_state_get_id(ste);
	fact = oval_filter_get_filter_action(filter);
	ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	filter_node = xmlNewTextChild(parent, ns_definitions, BAD_CAST "filter", BAD_CAST ste_id);
	if (fact != OVAL_FILTER_ACTION_EXCLUDE)
		xmlNewProp(filter_node, BAD_CAST "action", BAD_CAST oval_filter_action_get_text(fact));

	return filter_node;
}

/// @}
