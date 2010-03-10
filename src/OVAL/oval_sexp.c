
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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#ifndef __STUB_PROBE
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <seap.h>
#include <assert.h>

#include "oval_probe_impl.h"
#include "oval_sexp.h"
#include "probes/public/probe-api.h"
#include "oval_definitions_impl.h"
#include "oval_system_characteristics_impl.h"

#ifndef _A
# define _A(x) assert(x)
#endif

static SEXP_t *oval_value_to_sexp(struct oval_value *val, oval_datatype_t dtype)
{
	SEXP_t *val_sexp = NULL;
        char   *val_rptr = NULL;
        
	switch (dtype) {
	case OVAL_DATATYPE_VERSION:
                val_rptr = oval_value_get_text (val);
                
                if (val_rptr != NULL) {
                        val_sexp = SEXP_string_newf("%s", val_rptr);
                        SEXP_datatype_set(val_sexp, "version");
                }
                
		break;
        case OVAL_DATATYPE_EVR_STRING:
                val_rptr = oval_value_get_text (val);
                
                if (val_rptr != NULL) {
                        val_sexp = SEXP_string_newf("%s", val_rptr);
                        SEXP_datatype_set(val_sexp, "evr_str");
                }
                
		break;
	case OVAL_DATATYPE_STRING:
                val_rptr = oval_value_get_text (val);
                
                if (val_rptr != NULL) {
                        val_sexp = SEXP_string_newf("%s", val_rptr);
                }

		break;
	case OVAL_DATATYPE_FLOAT:
		val_sexp = SEXP_number_newf(oval_value_get_float(val));
		break;
	case OVAL_DATATYPE_INTEGER:
		val_sexp = SEXP_number_newi_32(oval_value_get_integer(val));
		break;
	case OVAL_DATATYPE_BOOLEAN:
		val_sexp = SEXP_number_newb(oval_value_get_boolean(val));
		SEXP_datatype_set(val_sexp, "bool");
		break;
	default:
		val_sexp = NULL;
		break;
	}

	return val_sexp;
}

static SEXP_t *oval_entity_to_sexp(struct oval_entity *ent)
{
	SEXP_t *elm, *elm_name;
	SEXP_t *r0, *r1, *r2;

	elm_name = SEXP_list_new(r0 = SEXP_string_newf("%s", oval_entity_get_name(ent)),
				 /* operation */
				 r1 = SEXP_string_new(":operation", 10),
				 r2 = SEXP_number_newu_32(oval_entity_get_operation(ent)), NULL);

	SEXP_free(r0);
	SEXP_free(r1);
	SEXP_free(r2);

	elm = SEXP_list_new(NULL);

	/* var_ref */
	if (oval_entity_get_varref_type(ent) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
		struct oval_variable *var;

		var = oval_entity_get_variable(ent);

		SEXP_list_add(elm_name, r0 = SEXP_string_new(":var_ref", 8));
		SEXP_free(r0);

		SEXP_list_add(elm_name, r0 = SEXP_string_newf("%s", oval_variable_get_id(var)));
		SEXP_free(r0);

		SEXP_list_add(elm, elm_name);
		SEXP_free(elm_name);

	} else {		/* value */
		SEXP_t *val_sexp;

		SEXP_list_add(elm, elm_name);
		SEXP_free(elm_name);

		val_sexp = oval_value_to_sexp(oval_entity_get_value(ent), oval_entity_get_datatype(ent));

		if (val_sexp != NULL) {
			SEXP_list_add(elm, val_sexp);
			SEXP_free(val_sexp);
		}
	}

	return (elm);
}

static SEXP_t *oval_varref_to_sexp(struct oval_entity *entity, struct oval_syschar_model *syschar_model)
{
	unsigned int val_cnt = 0;
	SEXP_t *val_lst, *val_sexp, *varref, *id_sexp, *val_cnt_sexp;
	oval_datatype_t dt;
	struct oval_variable *var;
	struct oval_value_iterator *vit;
	struct oval_value *val;

	val_lst = SEXP_list_new(NULL);
	dt = oval_entity_get_datatype(entity);

	var = oval_entity_get_variable(entity);
	vit = oval_syschar_model_get_variable_values(syschar_model, var);
	if (vit == NULL) {
		SEXP_free(val_lst);
		return NULL;
	}

	while (oval_value_iterator_has_more(vit)) {
		val = oval_value_iterator_next(vit);

		val_sexp = oval_value_to_sexp(val, dt);
		if (val_sexp == NULL) {
			SEXP_free(val_lst);
			oval_value_iterator_free(vit);
			return NULL;
		}

		SEXP_list_add(val_lst, val_sexp);
		SEXP_free(val_sexp);
		++val_cnt;
	}
	oval_value_iterator_free(vit);

	id_sexp = SEXP_string_newf("%s", oval_variable_get_id(var));
	val_cnt_sexp = SEXP_number_newu(val_cnt);

	varref = SEXP_list_new(id_sexp, val_cnt_sexp, val_lst, NULL);

	SEXP_free(id_sexp);
	SEXP_free(val_cnt_sexp);
	SEXP_free(val_lst);

	return varref;
}

static SEXP_t *oval_set_to_sexp(struct oval_setobject *set)
{
	SEXP_t *elm, *elm_name;
	SEXP_t *r0, *r1, *r2;

	elm_name = SEXP_list_new(r0 = SEXP_string_new("set", 3),
				 /* operation */
				 r1 = SEXP_string_new(":operation", 10),
				 r2 = SEXP_number_newu_32(oval_setobject_get_operation(set)), NULL);

	SEXP_free(r0);
	SEXP_free(r1);
	SEXP_free(r2);

	elm = SEXP_list_new(elm_name, NULL);
	SEXP_free(elm_name);

	switch (oval_setobject_get_type(set)) {
	case OVAL_SET_AGGREGATE:{
			struct oval_setobject_iterator *sit;
			struct oval_setobject *subset;

			sit = oval_setobject_get_subsets(set);

			while (oval_setobject_iterator_has_more(sit)) {
				subset = oval_setobject_iterator_next(sit);
				SEXP_list_add(elm, r0 = oval_set_to_sexp(subset));
				SEXP_free(r0);
			}

			oval_setobject_iterator_free(sit);
		}
		break;
	case OVAL_SET_COLLECTIVE:{
			struct oval_object_iterator *oit;
			struct oval_state_iterator *sit;
			struct oval_object *obj;
			struct oval_state *ste;
			SEXP_t *subelm;

			oit = oval_setobject_get_objects(set);
			sit = oval_setobject_get_filters(set);

			while (oval_object_iterator_has_more(oit)) {
				obj = oval_object_iterator_next(oit);

				subelm = SEXP_list_new(r0 = SEXP_string_new("obj_ref", 7),
						       r1 = SEXP_string_newf("%s", oval_object_get_id(obj)), NULL);
				SEXP_free(r0);
				SEXP_free(r1);

				SEXP_list_add(elm, subelm);

				SEXP_free(subelm);
			}

			oval_object_iterator_free(oit);

			while (oval_state_iterator_has_more(sit)) {
				ste = oval_state_iterator_next(sit);

				subelm = SEXP_list_new(r0 = SEXP_string_new("filter", 6),
						       r1 = SEXP_string_newf("%s", oval_state_get_id(ste)), NULL);
				SEXP_free(r0);
				SEXP_free(r1);

				SEXP_list_add(elm, subelm);

				SEXP_free(subelm);
			}

			oval_state_iterator_free(sit);
		}
		break;
	default:
		abort();
	}

	return (elm);
}

static SEXP_t *oval_behaviors_to_sexp(struct oval_behavior_iterator *bit)
{
	char *attr_name, *attr_val;
	SEXP_t *elm_name;
	SEXP_t *r0;

	struct oval_behavior *behavior;

	elm_name = SEXP_list_new(r0 = SEXP_string_newf("behaviors"), NULL);
	SEXP_free(r0);

	while (oval_behavior_iterator_has_more(bit)) {
		behavior = oval_behavior_iterator_next(bit);
		attr_name = oval_behavior_get_key(behavior);
		attr_val = oval_behavior_get_value(behavior);

		SEXP_list_add(elm_name, r0 = SEXP_string_newf(":%s", attr_name));
		SEXP_free(r0);

		if (attr_val != NULL) {
			SEXP_list_add(elm_name, r0 = SEXP_string_new(attr_val, strlen(attr_val)));
			SEXP_free(r0);
		}
	}

	r0 = SEXP_list_new(elm_name, NULL);
	SEXP_free(elm_name);

	return (r0);
}

SEXP_t *oval_object2sexp(const char *typestr, struct oval_object * object, struct oval_syschar_model * syschar_model)
{
	unsigned int ent_cnt, varref_cnt;
	SEXP_t *obj_sexp, *obj_name, *elm, *varrefs, *ent_lst, *lst, *stmp;
	SEXP_t *r0, *r1, *r2;

	struct oval_object_content_iterator *cit;
	struct oval_behavior_iterator *bit;
	struct oval_object_content *content;
	struct oval_entity *entity;

	/*
	 * Object name & attributes (id)
	 */
	obj_name = SEXP_list_new(r0 = SEXP_string_newf("%s_object", typestr),
				 r1 = SEXP_string_new(":id", 3),
				 r2 = SEXP_string_newf("%s", oval_object_get_id(object)), NULL);
	SEXP_free(r0);
	SEXP_free(r1);
	SEXP_free(r2);

	obj_sexp = SEXP_list_new(obj_name, NULL);

	SEXP_free(obj_name);

	/*
	 * Object content
	 */

	ent_lst = SEXP_list_new(NULL);
	varrefs = NULL;
	ent_cnt = varref_cnt = 0;

	cit = oval_object_get_object_contents(object);
	while (oval_object_content_iterator_has_more(cit)) {
		oval_check_t ochk;

		content = oval_object_content_iterator_next(cit);
		elm = NULL;
		lst = ent_lst;

		switch (oval_object_content_get_type(content)) {
		case OVAL_OBJECTCONTENT_ENTITY:
			entity = oval_object_content_get_entity(content);
			elm = oval_entity_to_sexp(entity);

			if (elm == NULL)
				break;

			ochk = oval_object_content_get_varCheck(content);
			if (ochk != OVAL_CHECK_UNKNOWN) {
				probe_ent_attr_add(elm, "var_check",
						   r0 = SEXP_string_newf("%s", oval_check_get_text(ochk)));
				SEXP_free(r0);
			}

			if (oval_entity_get_varref_type(entity) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
				stmp = oval_varref_to_sexp(entity, syschar_model);

				if (stmp == NULL) {
					SEXP_free(elm);
					elm = NULL;
					break;
				}

				if (varrefs == NULL) {
					varrefs = SEXP_list_new(NULL);
				}
				SEXP_list_add(varrefs, stmp);
				// todo: don't add duplicates
				++varref_cnt;

				lst = obj_sexp;
				++ent_cnt;
			}
			break;

		case OVAL_OBJECTCONTENT_SET:
			elm = oval_set_to_sexp(oval_object_content_get_setobject(content));
			break;

		case OVAL_OBJECTCONTENT_UNKNOWN:
			break;
		}

		if (elm == NULL) {
			SEXP_free(obj_sexp);
			SEXP_free(ent_lst);
			if (varrefs != NULL)
				SEXP_free(varrefs);
			oval_object_content_iterator_free(cit);

			return (NULL);
		}

		SEXP_list_add(lst, elm);
		SEXP_free(elm);
	}

	if (varrefs != NULL) {
		// todo: SEXP_list_push()
		stmp = SEXP_list_new(r0 = SEXP_string_new("varrefs", 7),
				     r1 = SEXP_number_newu(varref_cnt), r2 = SEXP_number_newu(ent_cnt), NULL);
		SEXP_vfree(r0, r1, r2, NULL);

		r0 = SEXP_list_join(stmp, varrefs);
		SEXP_list_add(obj_sexp, r0);
		SEXP_vfree(stmp, varrefs, r0, NULL);
	}
	stmp = SEXP_list_join(obj_sexp, ent_lst);
	SEXP_free(obj_sexp);
	SEXP_free(ent_lst);
	obj_sexp = stmp;

	oval_object_content_iterator_free(cit);

	/*
	 * Object behaviors
	 */

	bit = oval_object_get_behaviors(object);
	if (oval_behavior_iterator_has_more(bit)) {
		elm = oval_behaviors_to_sexp(bit);
		SEXP_list_add(obj_sexp, elm);
		SEXP_free(elm);
	}
	oval_behavior_iterator_free(bit);

	return (obj_sexp);
}

SEXP_t *oval_state2sexp(struct oval_state * state)
{
	SEXP_t *ste, *ste_name, *ste_ent;
	SEXP_t *r0, *r1, *r2;
	char buffer[128];
	size_t buflen;
	const oval_pdsc_t *probe;
	struct oval_state_content_iterator *contents;

	probe = oval_pdsc_lookup(oval_state_get_subtype(state));

	if (probe == NULL) {
		_D("FAIL: unknown subtype: %d\n", oval_state_get_subtype(state));
		return (NULL);
	}

	buflen = snprintf(buffer, sizeof buffer, "%s_state", probe->subtype_name);
	_A(buflen < sizeof buffer);

	ste_name = SEXP_list_new(r0 = SEXP_string_new(buffer, buflen),
				 r1 = SEXP_string_new(":id", 3),
				 r2 = SEXP_string_newf("%s", oval_state_get_id(state)), NULL);

	SEXP_free(r0);
	SEXP_free(r1);
	SEXP_free(r2);

	ste = SEXP_list_new(ste_name, NULL);

	SEXP_free(ste_name);

	//entities = oval_state_entities (state);

	contents = oval_state_get_contents(state);

	/*
	   while (oval_entity_iterator_has_more (entities)) {
	   ste_ent = oval_entity_to_sexp (oval_entity_iterator_next (entities));
	   SEXP_list_add (ste, ste_ent);
	   }
	 */
	while (oval_state_content_iterator_has_more(contents)) {
		oval_check_t ochk;
		struct oval_state_content *content = oval_state_content_iterator_next(contents);

		ste_ent = oval_entity_to_sexp(oval_state_content_get_entity(content));

		if (ste_ent == NULL) {
			SEXP_free(ste);
			ste = NULL;
			break;
		}

		ochk = oval_state_content_get_var_check(content);
		if (ochk != OVAL_CHECK_UNKNOWN) {
			probe_ent_attr_add(ste_ent, "var_check", r0 = SEXP_number_newu_32(ochk));
			SEXP_free(r0);
		}

		ochk = oval_state_content_get_ent_check(content);
		if (ochk != OVAL_CHECK_UNKNOWN) {
			probe_ent_attr_add(ste_ent, "entity_check", r0 = SEXP_number_newu_32(ochk));
			SEXP_free(r0);
		}

		SEXP_list_add(ste, ste_ent);
		SEXP_free(ste_ent);
	}

	oval_state_content_iterator_free(contents);

	return (ste);
}

static struct oval_sysitem *oval_sysitem_from_sexp(struct oval_syschar_model *model, SEXP_t * sexp)
{
	_A(sexp);
	SEXP_t *sval;
	char *key;
	char *val;

	key = probe_ent_getname(sexp);

	if (!key)
		return NULL;

	sval = probe_ent_getval(sexp);

	if (sval == NULL) {
		oscap_free(key);
		return NULL;
	}

	switch (SEXP_typeof(sval)) {
	case SEXP_TYPE_STRING:{
			val = SEXP_string_cstr(sval);
			break;
		}
	case SEXP_TYPE_NUMBER:{
			size_t allocsize = 64;
			val = oscap_alloc(allocsize * sizeof(char));
			*val = '\0';

			switch (SEXP_number_type(sval)) {
			case SEXP_NUM_DOUBLE:
				snprintf(val, allocsize, "%f", SEXP_number_getf(sval));
				break;
			case SEXP_NUM_INT8:
			case SEXP_NUM_INT16:
			case SEXP_NUM_INT32:
			case SEXP_NUM_INT64:
				snprintf(val, allocsize, "%" PRId64, SEXP_number_geti_64(sval));
				break;
			case SEXP_NUM_UINT8:
			case SEXP_NUM_UINT16:
			case SEXP_NUM_UINT32:
			case SEXP_NUM_UINT64:
				snprintf(val, allocsize, "%" PRIu64, SEXP_number_getu_64(sval));
				break;
                        case SEXP_NUM_BOOL:
                                snprintf(val, allocsize, "%s", SEXP_number_getb (sval) ? "true" : "false");
                                break;
			case SEXP_NUM_NONE:
                        default:
				_A(false);
				break;
			}

			val[allocsize - 1] = '\0';
			val = oscap_realloc(val, strlen(val) + 1);
			break;
		}
	default:{
			_D("Unsupported type: %u", SEXP_typeof(sval));
			SEXP_free(sval);
			oscap_free(key);
			return NULL;
		}
	}

	int datatype = probe_ent_getdatatype(sexp);
	if (datatype < 0)
		datatype = 0;

	int status = probe_ent_getstatus(sexp);

	struct oval_sysitem *item = oval_sysitem_new(model);

	oval_sysitem_set_status(item, status);
	oval_sysitem_set_name(item, key);
	oval_sysitem_set_mask(item, probe_ent_getmask(sexp));

	if (status == OVAL_STATUS_EXISTS)
		oval_sysitem_set_value(item, val);

	oval_sysitem_set_datatype(item, datatype);

	SEXP_free(sval);
	/* oscap_free (key); ? */

	return item;
}

static struct oval_sysdata *oval_sysdata_from_sexp(struct oval_syschar_model *model, SEXP_t * sexp)
{
	_A(sexp);

	static int id_counter = 1;	/* TODO better ID generator */

	char *name;
	struct oval_sysdata *sysdata = NULL;

	name = probe_ent_getname(sexp);

	if (name == NULL)
		return NULL;
	else {
		char *endptr = strrchr(name, '_');

		if (strcmp(endptr, "_item") != 0)
			goto cleanup;

		*endptr = '\0';	// cut off the '_item' part
	}

	int type = oval_pdsc_lookup_type(name);

	_D("Syschar entry type: %d '%s' => %s\n", type, name, (type ? "OK" : "FAILED to decode"));

	char *id = oscap_alloc(sizeof(char) * 16);
	SEXP_t *sub;
	struct oval_sysitem *sysitem;

	int status = probe_ent_getstatus(sexp);

	sprintf(id, "%d", id_counter++);
	sysdata = oval_sysdata_get_new(model, id);
	oval_sysdata_set_status(sysdata, status);
	oval_sysdata_set_subtype(sysdata, type);
	//oval_sysdata_set_subtype_name(sysdata, name);

	if (status == OVAL_STATUS_EXISTS) {
		for (int i = 2; (sub = SEXP_list_nth(sexp, i)) != NULL; ++i) {
			if ((sysitem = oval_sysitem_from_sexp(model, sub)) != NULL)
				oval_sysdata_add_item(sysdata, sysitem);
			SEXP_free(sub);
		}
	}

 cleanup:
	oscap_free(name);
	return sysdata;
}

struct oval_syschar *oval_sexp2sysch(const SEXP_t * s_exp, struct oval_syschar_model *model, struct oval_object *object)
{
	struct oval_syschar *sysch;

	_A(s_exp != NULL);

	sysch = oval_syschar_new(model, object);

	if (oval_sysch_apply_sexp(sysch, s_exp, object) != 0) {
		oval_syschar_free(sysch);
		return (NULL);
	}

	return (sysch);
}

int oval_sysch_apply_sexp(struct oval_syschar *sysch, const SEXP_t *cobj, struct oval_object *object)
{
	_A(cobj != NULL);
	_A(sysch != NULL);

	SEXP_t *s_exp, *items;
	struct oval_sysdata *sysdata;
	struct oval_syschar_model *model;
	oval_syschar_collection_flag_t flag;

	if (oval_syschar_get_object(sysch) == NULL) {
		if (object != NULL)
			oval_syschar_set_object(sysch, object);
		else
			return (-1);

	} else if (object == NULL) {
		object = oval_syschar_get_object(sysch);

		if (object == NULL)
			return (-1);
	}

	_A(object == oval_syschar_get_object(sysch));

	model = oval_syschar_get_model(sysch);
	flag = _probe_cobj_get_flag(cobj);
	oval_syschar_set_flag(sysch, flag);

	items = _probe_cobj_get_items(cobj);
	SEXP_list_foreach(s_exp, items) {
		sysdata = oval_sysdata_from_sexp(model, s_exp);

		if (sysdata != NULL)
			oval_syschar_add_sysdata(sysch, sysdata);
	}
	SEXP_free(items);

	return (0);
}
#endif				/* __STUB_PROBE */
