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
 * Copyright 2009-2011 Red Hat Inc., Durham, North Carolina.
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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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

	/* todo: remove calls to SEXP_datatype_set() */

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
	oval_datatype_t datatype;

	elm_name = SEXP_list_new(r0 = SEXP_string_newf("%s", oval_entity_get_name(ent)),
				 /* operation */
				 r1 = SEXP_string_new(":operation", 10),
				 r2 = SEXP_number_newu_32(oval_entity_get_operation(ent)), NULL);

	SEXP_free(r0);
	SEXP_free(r1);
	SEXP_free(r2);

	elm = SEXP_list_new(NULL);
	datatype = oval_entity_get_datatype(ent);
	probe_ent_setdatatype(elm, datatype);

	if (oval_entity_get_varref_type(ent) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
		/* var_ref */
		struct oval_variable *var;

		var = oval_entity_get_variable(ent);

		SEXP_list_add(elm_name, r0 = SEXP_string_new(":var_ref", 8));
		SEXP_free(r0);

		SEXP_list_add(elm_name, r0 = SEXP_string_newf("%s", oval_variable_get_id(var)));
		SEXP_free(r0);

		SEXP_list_add(elm, elm_name);
		SEXP_free(elm_name);

	} else {
		/* value */
		SEXP_list_add(elm, elm_name);
		SEXP_free(elm_name);

		if (datatype != OVAL_DATATYPE_RECORD) {
			SEXP_t *val_sexp;

			val_sexp = oval_value_to_sexp(oval_entity_get_value(ent), datatype);
			if (val_sexp != NULL) {
				SEXP_list_add(elm, val_sexp);
				SEXP_free(val_sexp);
			}
		}
	}

	return (elm);
}

static int oval_varref_to_sexp(void *sess, struct oval_entity *entity, struct oval_syschar *syschar, SEXP_t **out_sexp)
{
	unsigned int val_cnt = 0;
	SEXP_t *val_lst, *val_sexp, *varref, *id_sexp, *val_cnt_sexp;
	oval_datatype_t dt;
	struct oval_variable *var;
	struct oval_value_iterator *vit;
	struct oval_value *val;
	oval_syschar_collection_flag_t flag;
	char msg[100];
	int ret = 0;

	var = oval_entity_get_variable(entity);
	if (oval_probe_query_variable(sess, var) != 0) {
		dE("Can't convert variable reference to SEXP.\n");
		return -1;
	}

	flag = oval_variable_get_collection_flag(var);
	switch (flag) {
	case SYSCHAR_FLAG_COMPLETE:
	case SYSCHAR_FLAG_INCOMPLETE:
		vit = oval_variable_get_values(var);
		if (oval_value_iterator_has_more(vit))
			break;
		/* fall through */
	case SYSCHAR_FLAG_DOES_NOT_EXIST:
		snprintf(msg, sizeof(msg), "Referenced variable has no values (%s).\n",
			 oval_variable_get_id(var));
		ret = 1;
		break;
	default:
		snprintf(msg, sizeof(msg), "There was a problem processing referenced variable (%s).\n",
			 oval_variable_get_id(var));
		ret = 1;
	}

	if (ret) {
		dW(msg);
		oval_syschar_add_new_message(syschar, msg, OVAL_MESSAGE_LEVEL_WARNING);
		oval_syschar_set_flag(syschar, SYSCHAR_FLAG_DOES_NOT_EXIST);
		return ret;
	}

	val_lst = SEXP_list_new(NULL);

	while (oval_value_iterator_has_more(vit)) {
		val = oval_value_iterator_next(vit);

		dt = oval_entity_get_datatype(entity);
		val_sexp = oval_value_to_sexp(val, dt);
		if (val_sexp == NULL) {
			oval_syschar_add_new_message(syschar, "Failed to convert variable value.\n",
						     OVAL_MESSAGE_LEVEL_ERROR);
			oval_syschar_set_flag(syschar, SYSCHAR_FLAG_ERROR);
			SEXP_free(val_lst);
			oval_value_iterator_free(vit);
			return -1;
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

	*out_sexp = varref;
	return 0;
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
			struct oval_filter_iterator *fit;
			struct oval_object *obj;
			SEXP_t *subelm;

			oit = oval_setobject_get_objects(set);
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

			fit = oval_setobject_get_filters(set);
			while (oval_filter_iterator_has_more(fit)) {
				struct oval_filter *fil;

				fil = oval_filter_iterator_next(fit);
				subelm = oval_filter_to_sexp(fil);
				SEXP_list_add(elm, subelm);
				SEXP_free(subelm);
			}
			oval_filter_iterator_free(fit);
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

int oval_object2sexp(void *sess, const char *typestr, struct oval_syschar *syschar, SEXP_t **out_sexp)
{
	unsigned int ent_cnt, varref_cnt;
	int ret = -1;
	SEXP_t *obj_sexp, *obj_name, *elm, *varrefs, *ent_lst, *lst, *stmp;
	SEXP_t *r0, *r1, *r2;

	struct oval_object *object;
	struct oval_object_content_iterator *cit;
	struct oval_behavior_iterator *bit;
	struct oval_object_content *content;
	struct oval_entity *entity;

	object = oval_syschar_get_object(syschar);

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
				ret = oval_varref_to_sexp(sess, entity, syschar, &stmp);

				if (ret != 0) {
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

			return (ret);
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

	*out_sexp = obj_sexp;

	return (0);
}

static SEXP_t *oval_record_field_STATE_to_sexp(struct oval_record_field *rf)
{
	struct oval_entity *rf_ent;
	struct oval_variable *var;
	oval_check_t ochk;
	oval_datatype_t dt;
	SEXP_t *rf_sexp, *r0;

	rf_ent = oval_entity_new(NULL);
	oval_entity_set_name(rf_ent, oval_record_field_get_name(rf));
	oval_entity_set_operation(rf_ent, oval_record_field_get_operation(rf));
	dt = oval_record_field_get_datatype(rf);
	oval_entity_set_datatype(rf_ent, dt);

	var = oval_record_field_get_variable(rf);
	if (var != NULL) {
		oval_entity_set_varref_type(rf_ent, OVAL_ENTITY_VARREF_ATTRIBUTE);
		oval_entity_set_variable(rf_ent, var);
	} else {
		struct oval_value *val;

		val = oval_value_new(dt, oval_record_field_get_value(rf));
		oval_entity_set_value(rf_ent, val);
	}

	rf_sexp = oval_entity_to_sexp(rf_ent);

	ochk = oval_record_field_get_var_check(rf);
	if (ochk != OVAL_CHECK_UNKNOWN) {
		probe_ent_attr_add(rf_sexp, "var_check", r0 = SEXP_number_newu_32(ochk));
		SEXP_free(r0);
	}
	ochk = oval_record_field_get_ent_check(rf);
	if (ochk != OVAL_CHECK_UNKNOWN) {
		probe_ent_attr_add(rf_sexp, "entity_check", r0 = SEXP_number_newu_32(ochk));
		SEXP_free(r0);
	}

	return rf_sexp;
}

static struct oval_sysent *oval_sysent_from_sexp(struct oval_syschar_model *model, SEXP_t * sexp);

static struct oval_record_field *oval_record_field_ITEM_from_sexp(SEXP_t *sexp)
{
	struct oval_sysent *sysent;
	struct oval_record_field *rf;

	sysent = oval_sysent_from_sexp(NULL, sexp);
	if (sysent == NULL)
		return NULL;

	rf = oval_record_field_new(OVAL_RECORD_FIELD_ITEM);
	oval_record_field_set_name(rf, oscap_strdup(oval_sysent_get_name(sysent)));
	oval_record_field_set_value(rf, oscap_strdup(oval_sysent_get_value(sysent)));
	oval_record_field_set_datatype(rf, oval_sysent_get_datatype(sysent));
	oval_record_field_set_mask(rf, oval_sysent_get_mask(sysent));
	oval_record_field_set_status(rf, oval_sysent_get_status(sysent));

	oval_sysent_free(sysent);

	return rf;
}

int oval_state2sexp(void *sess, struct oval_state *state, SEXP_t **out_sexp)
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
		return (-1);
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
		struct oval_record_field_iterator *rf_itr;

		ent = oval_state_content_get_entity(content);
		ste_ent = oval_entity_to_sexp(ent);
		if (ste_ent == NULL) {
			goto fail;
		}

		rf_itr = oval_state_content_get_record_fields(content);
		while (oval_record_field_iterator_has_more(rf_itr)) {
			struct oval_record_field *rf;
			SEXP_t *rf_sexp;

			rf = oval_record_field_iterator_next(rf_itr);
			rf_sexp = oval_record_field_STATE_to_sexp(rf);

			SEXP_list_add(ste_ent, rf_sexp);
			SEXP_free(rf_sexp);
		}
		oval_record_field_iterator_free(rf_itr);

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

			dt = oval_entity_get_datatype(ent);
			val_lst = SEXP_list_new(NULL);

			flag = oval_variable_get_collection_flag(var);
			if (flag == SYSCHAR_FLAG_COMPLETE
			    || flag == SYSCHAR_FLAG_INCOMPLETE) {
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
			}

			SEXP_list_add(ste_ent, val_lst);
			SEXP_free(val_lst);
		}

		SEXP_list_add(ste, ste_ent);
		SEXP_free(ste_ent);
	}
	oval_state_content_iterator_free(contents);

	*out_sexp = ste;
	return (0);

 fail:
	oval_state_content_iterator_free(contents);
	SEXP_vfree(ste, ste_ent, NULL);
	return (-1);
}

static struct oval_message *oval_sexp2msg(const SEXP_t *msg)
{
	struct oval_message *message;
	SEXP_t *r0;
	oval_message_level_t lvl;
	char *str;

	message = oval_message_new();
	r0 = SEXP_list_first(msg);
	lvl = SEXP_number_getu(r0);
	SEXP_free(r0);
	oval_message_set_level(message, lvl);
	r0 = SEXP_list_nth(msg, 2);
	str = SEXP_string_cstr(r0);
	SEXP_free(r0);
	oval_message_set_text(message, str);
	oscap_free(str);

	return message;
}

static struct oval_sysent *oval_sysent_from_sexp(struct oval_syschar_model *model, SEXP_t * sexp)
{
	char *key;
	oval_syschar_status_t status;
	oval_datatype_t dt;
	struct oval_sysent *ent;

	key = probe_ent_getname(sexp);
	if (!key)
		return NULL;

	status = probe_ent_getstatus(sexp);
	dt = probe_ent_getdatatype(sexp);

	ent = oval_sysent_new(model);
	oval_sysent_set_name(ent, key);
	oval_sysent_set_status(ent, status);
	oval_sysent_set_datatype(ent, dt);
	oval_sysent_set_mask(ent, probe_ent_getmask(sexp));

	if (status != OVAL_STATUS_EXISTS)
		return ent;

	if (dt == OVAL_DATATYPE_RECORD) {
		SEXP_t *srf, *srfs;

		probe_ent_getvals(sexp, &srfs);
		SEXP_list_foreach(srf, srfs) {
			struct oval_record_field *rf;

			rf = oval_record_field_ITEM_from_sexp(srf);
			oval_sysent_add_record_field(ent, rf);
		}
		SEXP_free(srfs);
	} else {
		char val[64], *valp = val;
		SEXP_t *sval;
		SEXP_numtype_t sndt;

		sval = probe_ent_getval(sexp);
		if (sval == NULL)
			return ent;

		switch (dt) {
		case OVAL_DATATYPE_BOOLEAN:
			snprintf(val, sizeof(val), "%s", SEXP_number_getb(sval) ? "true" : "false");
			break;
		case OVAL_DATATYPE_FLOAT:
			snprintf(val, sizeof(val), "%f", SEXP_number_getf(sval));
			break;
		case OVAL_DATATYPE_INTEGER:
			sndt = SEXP_number_type(sval);
			switch (sndt) {
			case SEXP_NUM_INT8:
			case SEXP_NUM_INT16:
			case SEXP_NUM_INT32:
			case SEXP_NUM_INT64:
				snprintf(val, sizeof(val), "%" PRId64, SEXP_number_geti_64(sval));
				break;
			case SEXP_NUM_UINT8:
			case SEXP_NUM_UINT16:
			case SEXP_NUM_UINT32:
			case SEXP_NUM_UINT64:
				snprintf(val, sizeof(val), "%" PRIu64, SEXP_number_getu_64(sval));
				break;
			default:
				dE("Unexpected SEXP number datatype: %d, name: '%s'.\n", sndt, key);
				valp = '\0';
				break;
			}
			break;
		case OVAL_DATATYPE_STRING:
			valp = SEXP_string_cstr(sval);
			break;
		default:
			dE("Unexpected OVAL datatype: %d, '%s', name: '%s'.\n",
			   dt, oval_datatype_get_text(dt), key);
			valp = '\0';
			break;
		}

		oval_sysent_set_value(ent, valp);
		if (valp != val)
			oscap_free(valp);
                SEXP_free(sval);
	}

	return ent;
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

int oval_sexp2sysch(const SEXP_t *cobj, struct oval_syschar *syschar)
{
	oval_syschar_collection_flag_t flag;
	SEXP_t *messages, *msg, *items, *item;
	struct oval_syschar_model *model;

	_A(cobj != NULL);

	flag = probe_cobj_get_flag(cobj);
	oval_syschar_set_flag(syschar, flag);

	messages = probe_cobj_get_msgs(cobj);
	SEXP_list_foreach(msg, messages) {
		struct oval_message *omsg;

		omsg = oval_sexp2msg(msg);
		if (omsg != NULL)
			oval_syschar_add_message(syschar, omsg);
	}
	SEXP_free(messages);

	model = oval_syschar_get_model(syschar);
	items = probe_cobj_get_items(cobj);
	SEXP_list_foreach(item, items) {
		struct oval_sysitem *sysitem;

		sysitem = oval_sysitem_from_sexp(model, item);
		if (sysitem != NULL)
			oval_syschar_add_sysitem(syschar, sysitem);
	}
	SEXP_free(items);

	return 0;
}
