/**
 * @file   oval_sexp.c
 * @brief  OVAL to/from S-exp conversion functions - implementation
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 * @author "Lukas Kuklinek" <lkukline@redhat.com>
 * @author "Tomas Heinrich" <theinric@redhat.com>
 *
 * @addtogroup PROBEOVALSEXP
 * @{
 */
/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
 *      "Lukas Kuklinek" <lkuklinek@redhat.com>
 */
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
#include "common/debug_priv.h"
#include "common/_error.h"

#ifndef _A
# define _A(x) assert(x)
#endif

SEXP_t *oval_value_to_sexp(struct oval_value *val, oval_datatype_t dtype)
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
		break;
	case OVAL_DATATYPE_BINARY:
	case OVAL_DATATYPE_FILESET_REVISION:
	case OVAL_DATATYPE_IOS_VERSION:
		// todo:
		oscap_dlprintf(DBG_E, "Unsupported datatype: %s.\n", dtype);
		oscap_seterr(OSCAP_EFAMILY_OVAL, OVAL_EOVALINT, "Unsupported datatype");
		val_sexp = NULL;
		break;
	default:
		oscap_dlprintf(DBG_E, "Unknown datatype: %s.\n", dtype);
		oscap_seterr(OSCAP_EFAMILY_OVAL, OVAL_EOVALINT, "Unknown datatype");
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

static SEXP_t *oval_varref_to_sexp(struct oval_entity *entity, struct oval_syschar_model *syschar_model, void *sess)
{
	unsigned int val_cnt = 0;
	SEXP_t *val_lst, *val_sexp, *varref, *id_sexp, *val_cnt_sexp;
	oval_datatype_t dt;
	struct oval_variable *var;
	struct oval_value_iterator *vit;
	struct oval_value *val;
	oval_syschar_collection_flag_t flag;

	val_lst = SEXP_list_new(NULL);
	dt = oval_entity_get_datatype(entity);

	var = oval_entity_get_variable(entity);
	if (oval_probe_query_variable(sess, var) != 0) {
		SEXP_free(val_lst);
		return NULL;
	}

	flag = oval_variable_get_collection_flag(var);
	switch (flag) {
	case SYSCHAR_FLAG_COMPLETE:
	case SYSCHAR_FLAG_INCOMPLETE:
		break;
	default:
		SEXP_free(val_lst);
		return NULL;
	}

	vit = oval_variable_get_values(var);
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

static SEXP_t *oval_filter_to_sexp(struct oval_filter *filter)
{
	SEXP_t *elm, *attr, *r0, *r1;
	oval_filter_action_t act;
	struct oval_state *ste;
	char *ste_id;

	act = oval_filter_get_filter_action(filter);
	ste = oval_filter_get_state(filter);
	ste_id = oval_state_get_id(ste);
	attr = probe_attr_creat("action", r0 = SEXP_number_newu(act), NULL);
	elm = probe_ent_creat1("filter",
			       attr,
			       r1 = SEXP_string_newf("%s", ste_id));
	SEXP_vfree(attr, r0, r1, NULL);

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

SEXP_t *oval_object2sexp(const char *typestr, struct oval_object *object, struct oval_syschar_model *syschar_model, void *sess)
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
						   r0 = SEXP_number_newu_32(ochk));
				SEXP_free(r0);
			}

			if (oval_entity_get_varref_type(entity) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
				stmp = oval_varref_to_sexp(entity, syschar_model, sess);

				if (stmp == NULL) {
					SEXP_free(elm);
					elm = NULL;
					break;
				}

				if (varrefs == NULL) {
					varrefs = SEXP_list_new(NULL);
				}
				SEXP_list_add(varrefs, stmp);
                                SEXP_free(stmp);
				// todo: don't add duplicates
				++varref_cnt;

				lst = obj_sexp;
				++ent_cnt;
			}
			break;

		case OVAL_OBJECTCONTENT_SET:
			elm = oval_set_to_sexp(oval_object_content_get_setobject(content));
			break;

		case OVAL_OBJECTCONTENT_FILTER:
			elm = oval_filter_to_sexp(oval_object_content_get_filter(content));
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

SEXP_t *oval_state2sexp(struct oval_state *state, void *sess)
{
	SEXP_t *ste, *ste_name, *ste_ent;
	SEXP_t *r0, *r1, *r2, *r3, *r4;
	char buffer[128];
	size_t buflen;
	const char *subtype_name;
	struct oval_state_content_iterator *contents;

        subtype_name = oval_subtype2str(oval_state_get_subtype(state));

	if (subtype_name == NULL) {
		_D("FAIL: unknown subtype: %d\n", oval_state_get_subtype(state));
		return (NULL);
	}

	buflen = snprintf(buffer, sizeof buffer, "%s_state", subtype_name);
	_A(buflen < sizeof buffer);

	ste_name = SEXP_list_new(r0 = SEXP_string_new(buffer, buflen),
				 r1 = SEXP_string_new(":id", 3),
				 r2 = SEXP_string_newf("%s", oval_state_get_id(state)),
				 r3 = SEXP_string_new(":operator", 9),
				 r4 = SEXP_number_newu(oval_state_get_operator(state)),
				 NULL);

	ste = SEXP_list_new(ste_name, NULL);
	SEXP_vfree(r0, r1, r2, r3, r4, ste_name, NULL);

	contents = oval_state_get_contents(state);
	while (oval_state_content_iterator_has_more(contents)) {
		oval_check_t ochk;
		struct oval_entity *ent;
		struct oval_state_content *content = oval_state_content_iterator_next(contents);

		ent = oval_state_content_get_entity(content);
		ste_ent = oval_entity_to_sexp(ent);
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

		if (oval_entity_get_varref_type(ent) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
			SEXP_t *val_lst;
			struct oval_variable *var;
			struct oval_value_iterator *val_itr;
			oval_datatype_t dt;
			oval_syschar_collection_flag_t flag;

			var = oval_entity_get_variable(ent);
			if (oval_probe_query_variable(sess, var) != 0) {
				goto fail;
			}

			flag = oval_variable_get_collection_flag(var);
			switch (flag) {
			case SYSCHAR_FLAG_COMPLETE:
			case SYSCHAR_FLAG_INCOMPLETE:
				break;
			default:
				goto fail;
			}

			dt = oval_entity_get_datatype(ent);
			val_lst = SEXP_list_new(NULL);

			val_itr = oval_variable_get_values(var);
			while (oval_value_iterator_has_more(val_itr)) {
				struct oval_value *val;
				SEXP_t *vs;

				val = oval_value_iterator_next(val_itr);
				vs = oval_value_to_sexp(val, dt);
				if (vs == NULL) {
					oscap_dlprintf(DBG_E, "Failed to convert OVAL value to SEXP: "
						       "datatype: %s, text: %s.\n",
						       oval_datatype_get_text(dt),
						       oval_value_get_text(val));
					oscap_seterr(OSCAP_EFAMILY_OVAL, OVAL_EOVALINT, "Failed to convert OVAL value to SEXP");
					oval_value_iterator_free(val_itr);
					SEXP_free(val_lst);
					goto fail;
				}
				SEXP_list_add(val_lst, vs);
				SEXP_free(vs);
			}
			oval_value_iterator_free(val_itr);

			SEXP_list_add(ste_ent, val_lst);
			SEXP_free(val_lst);
		}

		SEXP_list_add(ste, ste_ent);
		SEXP_free(ste_ent);
	}
	oval_state_content_iterator_free(contents);

	return (ste);

 fail:
	oval_state_content_iterator_free(contents);
	SEXP_vfree(ste_ent, ste, NULL);
	return NULL;
}

static struct oval_sysent *oval_sysent_from_sexp(struct oval_syschar_model *model, SEXP_t * sexp)
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

	struct oval_sysent *item = oval_sysent_new(model);

	oval_sysent_set_status(item, status);
	oval_sysent_set_name(item, key);
	oval_sysent_set_mask(item, probe_ent_getmask(sexp));

	if (status == OVAL_STATUS_EXISTS)
		oval_sysent_set_value(item, val);

	oval_sysent_set_datatype(item, datatype);

	SEXP_free(sval);
	oscap_free(key);
	oscap_free(val);

	return item;
}

static struct oval_sysitem *oval_sysitem_from_sexp(struct oval_syschar_model *model, SEXP_t * sexp)
{
	_A(sexp);

	static int id_counter = 1;	/* TODO better ID generator */

	char *name;
	struct oval_sysitem *sysitem = NULL;

	name = probe_ent_getname(sexp);

	if (name == NULL)
		return NULL;
	else {
		char *endptr = strrchr(name, '_');

		if (strcmp(endptr, "_item") != 0)
			goto cleanup;

		*endptr = '\0';	// cut off the '_item' part
	}

	int type = oval_str2subtype(name);

	_D("Syschar entry type: %d '%s' => %s\n", type, name, (type ? "OK" : "FAILED to decode"));

	char id[16];
	SEXP_t *sub;
	struct oval_sysent *sysent;

	int status = probe_ent_getstatus(sexp);

	sprintf(id, "%d", id_counter++);
	sysitem = oval_sysitem_get_new(model, id);
	oval_sysitem_set_status(sysitem, status);
	oval_sysitem_set_subtype(sysitem, type);
	//oval_sysitem_set_subtype_name(sysitem, name);

	if (status == OVAL_STATUS_EXISTS) {
		for (int i = 2; (sub = SEXP_list_nth(sexp, i)) != NULL; ++i) {
			if ((sysent = oval_sysent_from_sexp(model, sub)) != NULL)
				oval_sysitem_add_item(sysitem, sysent);
			SEXP_free(sub);
		}
	}

 cleanup:
	oscap_free(name);
	return sysitem;
}

struct oval_syschar *oval_sexp2sysch(const SEXP_t * s_exp, struct oval_syschar_model *model, struct oval_object *object)
{
	struct oval_syschar *sysch;

	_A(s_exp != NULL);

	sysch = oval_syschar_new(model, object);

	if (oval_sysch_apply_sexp(sysch, s_exp, object) != 0) {
		// todo: remove syschar from model
		return (NULL);
	}

	return (sysch);
}

int oval_sysch_apply_sexp(struct oval_syschar *sysch, const SEXP_t *cobj, struct oval_object *object)
{
	_A(cobj != NULL);
	_A(sysch != NULL);

	SEXP_t *s_exp, *items;
	struct oval_sysitem *sysitem;
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
		sysitem = oval_sysitem_from_sexp(model, s_exp);

		if (sysitem != NULL)
			oval_syschar_add_sysitem(sysch, sysitem);
	}
	SEXP_free(items);

	return (0);
}
