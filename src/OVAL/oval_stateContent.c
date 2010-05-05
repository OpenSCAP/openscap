/**
 * @file oval_stateContent.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
#include "common/util.h"
#include "common/debug_priv.h"

typedef struct oval_state_content {
	struct oval_definition_model *model;
	struct oval_entity *entity;
	oval_check_t ent_check;
	oval_check_t var_check;
} oval_state_content_t;

bool oval_state_content_iterator_has_more(struct oval_state_content_iterator
					  *oc_state_content)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_state_content);
}

struct oval_state_content *oval_state_content_iterator_next(struct
							    oval_state_content_iterator
							    *oc_state_content)
{
	return (struct oval_state_content *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_state_content);
}

void oval_state_content_iterator_free(struct
				      oval_state_content_iterator
				      *oc_state_content)
{
	oval_collection_iterator_free((struct oval_iterator *)
				      oc_state_content);
}

struct oval_entity *oval_state_content_get_entity(struct oval_state_content
						  *content)
{
	__attribute__nonnull__(content);

	return content->entity;
}

oval_check_t oval_state_content_get_var_check(struct oval_state_content * content)
{
	__attribute__nonnull__(content);

	return content->var_check;
}

oval_check_t oval_state_content_get_ent_check(struct oval_state_content * content)
{
	__attribute__nonnull__(content);

	return content->ent_check;
}

struct oval_state_content *oval_state_content_new(struct oval_definition_model *model)
{
	oval_state_content_t *content = (oval_state_content_t *)
	    oscap_alloc(sizeof(oval_state_content_t));
	if (content == NULL)
		return NULL;

	content->entity = NULL;
	content->ent_check = OVAL_CHECK_UNKNOWN;
	content->var_check = OVAL_CHECK_UNKNOWN;
	content->model = model;
	return content;
}

/* TODO: don't know if this should be in API -> commenting now
bool oval_state_content_is_valid(struct oval_state_content *state_content)
{
	return true;//TODO
}*/

bool oval_state_content_is_locked(struct oval_state_content * state_content)
{
	__attribute__nonnull__(state_content);

	return oval_definition_model_is_locked(state_content->model);
}

struct oval_state_content *oval_state_content_clone
    (struct oval_definition_model *new_model, struct oval_state_content *old_content) {
	struct oval_state_content *new_content = oval_state_content_new(new_model);
	oval_check_t echeck = oval_state_content_get_ent_check(old_content);
	oval_state_content_set_entcheck(new_content, echeck);
	struct oval_entity *entity = oval_state_content_get_entity(old_content);
	oval_state_content_set_entity(new_content, oval_entity_clone(new_model, entity));
	oval_check_t vcheck = oval_state_content_get_var_check(old_content);
	oval_state_content_set_varcheck(new_content, vcheck);
	return new_content;
}

void oval_state_content_free(struct oval_state_content *content)
{
	__attribute__nonnull__(content);

	if (content->entity)
		oval_entity_free(content->entity);
	oscap_free(content);
}

void oval_state_content_set_entity(struct oval_state_content *content, struct oval_entity *entity)
{
	if (content && !oval_state_content_is_locked(content)) {
		if (content->entity)
			oval_entity_free(content->entity);
		content->entity = entity;
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_state_content_set_varcheck(struct oval_state_content *content, oval_check_t check)
{
	if (content && !oval_state_content_is_locked(content)) {
		content->var_check = check;
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_state_content_set_entcheck(struct oval_state_content *content, oval_check_t check)
{
	if (content && !oval_state_content_is_locked(content)) {
		content->ent_check = check;
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

static void _oval_state_content_entity_consumer(struct oval_entity *entity, struct oval_state_content *content) {
	oval_state_content_set_entity(content, entity);
}

int oval_state_content_parse_tag(xmlTextReaderPtr reader,
				 struct oval_parser_context *context, oscap_consumer_func consumer, void *user)
{
	__attribute__nonnull__(context);

	struct oval_state_content *content = oval_state_content_new(context->definition_model);
	int retcode = oval_entity_parse_tag
	    (reader, context, (oscap_consumer_func) _oval_state_content_entity_consumer, content);

	oval_check_t var_check = oval_check_parse(reader, "var_check", OVAL_CHECK_ALL);
	oval_check_t ent_check = oval_check_parse(reader, "entity_check", OVAL_CHECK_ALL);

	oval_state_content_set_varcheck(content, var_check);
	oval_state_content_set_entcheck(content, ent_check);

	(*consumer) (content, user);
	return retcode;
}

xmlNode *oval_state_content_to_dom(struct oval_state_content * content, xmlDoc * doc, xmlNode * parent) {
	__attribute__nonnull__(content);

	xmlNode *content_node = oval_entity_to_dom(content->entity, doc, parent);

	oval_check_t var_check = oval_state_content_get_var_check(content);
	if (var_check != OVAL_CHECK_ALL)
		xmlNewProp(content_node, BAD_CAST "var_check", BAD_CAST oval_check_get_text(var_check));

	oval_check_t ent_check = oval_state_content_get_ent_check(content);
	if (ent_check != OVAL_CHECK_ALL)
		xmlNewProp(content_node, BAD_CAST "entity_check", BAD_CAST oval_check_get_text(ent_check));

	return content_node;
}
