/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "oval_definitions_impl.h"

static void _oval_definition_fill_vardef(struct oval_definition *definition, struct oval_string_map *vardef);
static void _oval_criteria_fill_vardef(struct oval_criteria_node *cnode, struct oval_string_map *vardef, const char *definition_id);
static void _oval_test_fill_vardef(struct oval_test *test, struct oval_string_map *vardef, const char *definition_id);
static void _oval_object_fill_vardef(struct oval_object *object, struct oval_string_map *vardef, const char *definition_id);
static void _oval_setobject_fill_vardef(struct oval_setobject *set, struct oval_string_map *vardef, const char *definition_id);
static void _oval_state_fill_vardef(struct oval_state *state, struct oval_string_map *vardef, const char *definition_id);
static void _oval_entity_fill_vardef(struct oval_entity *entity, struct oval_string_map *vardef, const char *definition_id);
static void _vardef_insert(struct oval_string_map *vardef, const char *definition_id, const char *variable_id);

struct oval_string_map *oval_definition_model_build_vardef_mapping(struct oval_definition_model *model)
{
	struct oval_string_map *vardef = oval_string_map_new();
	struct oval_definition_iterator *def_it = oval_definition_model_get_definitions(model);
	while (oval_definition_iterator_has_more(def_it)) {
		struct oval_definition *definition = oval_definition_iterator_next(def_it);
		_oval_definition_fill_vardef(definition, vardef);
	}
	oval_definition_iterator_free(def_it);
	return vardef;
}

void _oval_definition_fill_vardef(struct oval_definition *definition, struct oval_string_map *vardef)
{
	struct oval_criteria_node *cnode = oval_definition_get_criteria(definition);
	if (cnode != NULL)
		_oval_criteria_fill_vardef(cnode, vardef, oval_definition_get_id(definition));
}

void _oval_criteria_fill_vardef(struct oval_criteria_node *cnode, struct oval_string_map *vardef, const char *definition_id)
{
	switch (oval_criteria_node_get_type(cnode)) {
	case OVAL_NODETYPE_CRITERION:{
		struct oval_test *test = oval_criteria_node_get_test(cnode);
		if (test != NULL)
			_oval_test_fill_vardef(test, vardef, definition_id);
		} break;
	case OVAL_NODETYPE_CRITERIA:{
		struct oval_criteria_node_iterator *cnode_it = oval_criteria_node_get_subnodes(cnode);
		if (cnode_it != NULL) {
			while (oval_criteria_node_iterator_has_more(cnode_it)) {
				struct oval_criteria_node *node = oval_criteria_node_iterator_next(cnode_it);
				if (node != NULL)
					_oval_criteria_fill_vardef(node, vardef, definition_id);
			}
			oval_criteria_node_iterator_free(cnode_it);
		}
		} break;
	case OVAL_NODETYPE_EXTENDDEF:{
		struct oval_definition *definition = oval_criteria_node_get_definition(cnode);
		if (definition != NULL) {
			struct oval_criteria_node *node = oval_definition_get_criteria(definition);
			if (node != NULL)
				_oval_criteria_fill_vardef(node, vardef, definition_id);
		}
		} break;
	case OVAL_NODETYPE_UNKNOWN:
	default:
		assert(false);
		break;
	}
}

void _oval_test_fill_vardef(struct oval_test *test, struct oval_string_map *vardef, const char *definition_id)
{
	struct oval_object *object = oval_test_get_object(test);
	if (object != NULL)
		_oval_object_fill_vardef(object, vardef, definition_id);
	struct oval_state_iterator *ste_it = oval_test_get_states(test);
	while (oval_state_iterator_has_more(ste_it)) {
		struct oval_state *state = oval_state_iterator_next(ste_it);
		if (state != NULL)
			_oval_state_fill_vardef(state, vardef, definition_id);
	}
	oval_state_iterator_free(ste_it);
}

void _oval_object_fill_vardef(struct oval_object *object, struct oval_string_map *vardef, const char *definition_id)
{
	struct oval_object_content_iterator *content_it = oval_object_get_object_contents(object);
	while (oval_object_content_iterator_has_more(content_it)) {
		struct oval_object_content *content = oval_object_content_iterator_next(content_it);
		switch (oval_object_content_get_type(content)) {
		case OVAL_OBJECTCONTENT_ENTITY:{
			struct oval_entity *entity = oval_object_content_get_entity(content);
			_oval_entity_fill_vardef(entity, vardef, definition_id);
			} break;
		case OVAL_OBJECTCONTENT_SET:{
			struct oval_setobject *set = oval_object_content_get_setobject(content);
			_oval_setobject_fill_vardef(set, vardef, definition_id);
			} break;
		default:
			break;
		}
	}
	oval_object_content_iterator_free(content_it);
}

void _oval_setobject_fill_vardef(struct oval_setobject *set, struct oval_string_map *vardef, const char *definition_id)
{
	switch (oval_setobject_get_type(set)) {
	case OVAL_SET_AGGREGATE:{
		struct oval_setobject_iterator *subset_it = oval_setobject_get_subsets(set);
		while (oval_setobject_iterator_has_more(subset_it)) {
			struct oval_setobject *subset = oval_setobject_iterator_next(subset_it);
			_oval_setobject_fill_vardef(subset, vardef, definition_id);
		}
		oval_setobject_iterator_free(subset_it);
		} break;
	case OVAL_SET_COLLECTIVE:{
		struct oval_object_iterator *object_it = oval_setobject_get_objects(set);
		while (oval_object_iterator_has_more(object_it)) {
			struct oval_object *object = oval_object_iterator_next(object_it);
			_oval_object_fill_vardef(object, vardef, definition_id);
		}
		oval_object_iterator_free(object_it);

		struct oval_filter_iterator *filter_it = oval_setobject_get_filters(set);
		while (oval_filter_iterator_has_more(filter_it)) {
			struct oval_filter *filter = oval_filter_iterator_next(filter_it);
			struct oval_state *state = oval_filter_get_state(filter);
			_oval_state_fill_vardef(state, vardef, definition_id);
		}
		oval_filter_iterator_free(filter_it);
		} break;
	default:
		break;
	}
}

void _oval_state_fill_vardef(struct oval_state *state, struct oval_string_map *vardef, const char *definition_id)
{
	struct oval_state_content_iterator *content_it = oval_state_get_contents(state);
	while (oval_state_content_iterator_has_more(content_it)) {
		struct oval_state_content *content = oval_state_content_iterator_next(content_it);
		struct oval_entity *entity = oval_state_content_get_entity(content);
		if (entity != NULL)
			_oval_entity_fill_vardef(entity, vardef, definition_id);
	}
	oval_state_content_iterator_free(content_it);
}

void _oval_entity_fill_vardef(struct oval_entity *entity, struct oval_string_map *vardef, const char *definition_id)
{
	if (oval_entity_get_varref_type(entity) == OVAL_ENTITY_VARREF_ATTRIBUTE ||
		oval_entity_get_varref_type(entity) == OVAL_ENTITY_VARREF_ELEMENT) {
		struct oval_variable *variable = oval_entity_get_variable(entity);
		if (variable != NULL)
			_vardef_insert(vardef, definition_id, oval_variable_get_id(variable));
	}
}

void _vardef_insert(struct oval_string_map *vardef, const char *definition_id, const char *variable_id)
{
	struct oval_string_map *def_list = (struct oval_string_map *) oval_string_map_get_value(vardef, variable_id);
	if (def_list == NULL) {
		def_list = oval_string_map_new();
		oval_string_map_put(vardef, variable_id, def_list);
	}
	oval_string_map_put(def_list, definition_id, (void *) "");
}
