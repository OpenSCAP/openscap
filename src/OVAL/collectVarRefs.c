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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "collectVarRefs_impl.h"
#include "oval_definitions_impl.h"
#include "adt/oval_string_map_impl.h"

static void _var_collect_var_refs(struct oval_variable *var, struct oval_string_map *vm);

static void _comp_collect_var_refs(struct oval_component *comp, struct oval_string_map *vm)
{
	struct oval_object *obj;
	struct oval_variable *var;
	struct oval_component_iterator *cmp_itr;

	switch (oval_component_get_type(comp)) {
	case OVAL_COMPONENT_OBJECTREF:
		obj = oval_component_get_object(comp);
		oval_obj_collect_var_refs(obj, vm);
		break;
	case OVAL_COMPONENT_VARREF:
		var = oval_component_get_variable(comp);
		_var_collect_var_refs(var, vm);
		break;
	case OVAL_FUNCTION_ARITHMETIC:
	case OVAL_FUNCTION_BEGIN:
	case OVAL_FUNCTION_CONCAT:
	case OVAL_FUNCTION_END:
	case OVAL_FUNCTION_ESCAPE_REGEX:
	case OVAL_FUNCTION_REGEX_CAPTURE:
	case OVAL_FUNCTION_SPLIT:
	case OVAL_FUNCTION_SUBSTRING:
	case OVAL_FUNCTION_TIMEDIF:
		cmp_itr = oval_component_get_function_components(comp);
		while (oval_component_iterator_has_more(cmp_itr)) {
			struct oval_component *cmp;

			cmp = oval_component_iterator_next(cmp_itr);
			_comp_collect_var_refs(cmp, vm);
		}
		oval_component_iterator_free(cmp_itr);
		break;
	default:
		break;
	}
}

static void _var_collect_var_refs(struct oval_variable *var, struct oval_string_map *vm)
{
	char *var_id;

	var_id = oval_variable_get_id(var);
	oval_string_map_put(vm, var_id, var);

	if (oval_variable_get_type(var) == OVAL_VARIABLE_LOCAL) {
		struct oval_component *comp;

		comp = oval_variable_get_component(var);
		_comp_collect_var_refs(comp, vm);
	}
}

static void _ent_collect_var_refs(struct oval_entity *ent, struct oval_string_map *vm)
{
	oval_entity_varref_type_t vrt;

	vrt = oval_entity_get_varref_type(ent);
	if (vrt == OVAL_ENTITY_VARREF_ATTRIBUTE
	    || vrt == OVAL_ENTITY_VARREF_ELEMENT) {
		struct oval_variable *var;

		var = oval_entity_get_variable(ent);
		_var_collect_var_refs(var, vm);
	}
}

void oval_ste_collect_var_refs(struct oval_state *ste, struct oval_string_map *vm)
{
	struct oval_state_content_iterator *cont_itr;

	cont_itr = oval_state_get_contents(ste);
	while (oval_state_content_iterator_has_more(cont_itr)) {
		struct oval_state_content *cont;
		struct oval_entity *ent;

		cont = oval_state_content_iterator_next(cont_itr);
		ent = oval_state_content_get_entity(cont);
		_ent_collect_var_refs(ent, vm);
	}
	oval_state_content_iterator_free(cont_itr);
}

static void _set_collect_var_refs(struct oval_setobject *set, struct oval_string_map *vm)
{
	struct oval_setobject_iterator *subset_itr;
	struct oval_object_iterator *obj_itr;
	struct oval_filter_iterator *fil_itr;

	switch (oval_setobject_get_type(set)) {
	case OVAL_SET_AGGREGATE:
		subset_itr = oval_setobject_get_subsets(set);
		while (oval_setobject_iterator_has_more(subset_itr)) {
			struct oval_setobject *subset;

			subset = oval_setobject_iterator_next(subset_itr);
			_set_collect_var_refs(subset, vm);
		}
		oval_setobject_iterator_free(subset_itr);
		break;
	case OVAL_SET_COLLECTIVE:
		obj_itr = oval_setobject_get_objects(set);
		while (oval_object_iterator_has_more(obj_itr)) {
			struct oval_object *obj;

			obj = oval_object_iterator_next(obj_itr);
			oval_obj_collect_var_refs(obj, vm);
		}
		oval_object_iterator_free(obj_itr);
		fil_itr = oval_setobject_get_filters(set);
		while (oval_filter_iterator_has_more(fil_itr)) {
			struct oval_filter *fil;
			struct oval_state *ste;

			fil = oval_filter_iterator_next(fil_itr);
			ste = oval_filter_get_state(fil);
			oval_ste_collect_var_refs(ste, vm);
		}
		oval_filter_iterator_free(fil_itr);
		break;
	default:
		break;
	}
}

void oval_obj_collect_var_refs(struct oval_object *obj, struct oval_string_map *vm)
{
	struct oval_object_content_iterator *cont_itr;

	cont_itr = oval_object_get_object_contents(obj);
	while (oval_object_content_iterator_has_more(cont_itr)) {
		struct oval_object_content *cont;
		struct oval_entity *ent;
		struct oval_setobject *set;

		cont = oval_object_content_iterator_next(cont_itr);

		switch (oval_object_content_get_type(cont)) {
		case OVAL_OBJECTCONTENT_ENTITY:
			ent = oval_object_content_get_entity(cont);
			_ent_collect_var_refs(ent, vm);
			break;
		case OVAL_OBJECTCONTENT_SET:
			set = oval_object_content_get_setobject(cont);
			_set_collect_var_refs(set, vm);
			break;
		default:
			break;
		}
	}
	oval_object_content_iterator_free(cont_itr);
}
