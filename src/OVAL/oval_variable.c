/**
 * @file oval_variable.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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

#include "oval_definitions_impl.h"
#include "adt/oval_collection_impl.h"
#include "adt/oval_string_map_impl.h"
#include "oval_agent_api_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/oscap_string.h"
#include "results/oval_cmp_impl.h"
#include "results/oval_results_impl.h"

typedef struct oval_variable {
#define VAR_BASE				\
	struct oval_definition_model *model;	\
	oval_variable_type_t type;		\
	oval_syschar_collection_flag_t flag;	\
	char *id;				\
	int version;				\
	oval_datatype_t datatype;		\
	char *comment;				\
	int deprecated

	VAR_BASE;
} oval_variable_t;

typedef struct {
	VAR_BASE;
	struct oval_collection *values;
} oval_variable_CONSTANT_t;

typedef struct {
	VAR_BASE;
	struct oval_collection *possible_values;
	struct oval_collection *possible_restrictions;
	struct oval_collection *values_ref;
} oval_variable_EXTERNAL_t;

typedef struct {
	VAR_BASE;
	struct oval_component *component;
	struct oval_collection *values;
} oval_variable_LOCAL_t;

typedef struct {
	union {
		oval_variable_t v1;
		oval_variable_CONSTANT_t v2;
		oval_variable_EXTERNAL_t v3;
		oval_variable_LOCAL_t v4;
	} unused;
} oval_variable_UNKNOWN_t;

typedef struct oval_variable_possible_value {
	char *hint;
	char *value;
} oval_variable_possible_value_t;

typedef struct oval_variable_possible_restriction {
	oval_operator_t operator;
	char *hint;
	struct oval_collection *restrictions;
} oval_variable_possible_restriction_t;

typedef struct oval_variable_restriction {
	oval_operation_t operation;
	char *value;
} oval_variable_restriction_t;

struct oval_variable_possible_value *oval_variable_possible_value_new(const char *hint, const char *value)
{
	struct oval_variable_possible_value *pv;
	pv = oscap_alloc(sizeof(oval_variable_possible_value_t));
	pv->hint = oscap_strdup(hint);
	pv->value = oscap_strdup(value);
	return pv;
}

void oval_variable_possible_value_free(struct oval_variable_possible_value *pv)
{
	if (pv != NULL) {
		oscap_free(pv->hint);
		oscap_free(pv->value);
		oscap_free(pv);
	}
}

char* oval_variable_possible_value_get_hint(struct oval_variable_possible_value* pv)
{
    __attribute__nonnull__(pv);

    return pv->hint;
}

char* oval_variable_possible_value_get_value(struct oval_variable_possible_value* pv)
{
    __attribute__nonnull__(pv);

    return pv->value;
}

bool oval_variable_possible_value_iterator_has_more(struct oval_variable_possible_value_iterator* iter)
{
	return oval_collection_iterator_has_more((struct oval_iterator*)iter);
}

struct oval_variable_possible_value* oval_variable_possible_value_iterator_next(struct oval_variable_possible_value_iterator* iter)
{
	return (struct oval_variable_possible_value*)oval_collection_iterator_next((struct oval_iterator*)iter);
}

void oval_variable_possible_value_iterator_free(struct oval_variable_possible_value_iterator* iter)
{
	oval_collection_iterator_free((struct oval_iterator*)iter);
}

int oval_variable_possible_value_iterator_remaining(struct oval_variable_possible_value_iterator* iter)
{
	return oval_collection_iterator_remaining((struct oval_iterator*)iter);
}

struct oval_variable_possible_restriction *oval_variable_possible_restriction_new(oval_operator_t operator, const char *hint)
{
	struct oval_variable_possible_restriction *pr;
	pr = oscap_alloc(sizeof(oval_variable_possible_restriction_t));
	pr->operator = operator;
	pr->hint = oscap_strdup(hint);
	pr->restrictions = oval_collection_new();
	return pr;
}

void oval_variable_possible_restriction_free(struct oval_variable_possible_restriction *pr)
{
	if (pr != NULL) {
		oscap_free(pr->hint);
		oval_collection_free_items(pr->restrictions, (oscap_destruct_func) oval_variable_restriction_free);
		oscap_free(pr);
	}
}

void oval_variable_possible_restriction_add_restriction(struct oval_variable_possible_restriction *pr, struct oval_variable_restriction *r)
{
	__attribute__nonnull__(pr);
	__attribute__nonnull__(r);
	oval_collection_add(pr->restrictions, r);
}

struct oval_variable_restriction_iterator *oval_variable_possible_restriction_get_restrictions(struct oval_variable_possible_restriction *possible_restriction)
{
	return (struct oval_variable_restriction_iterator*)oval_collection_iterator(possible_restriction->restrictions);
}

oval_operator_t oval_variable_possible_restriction_get_operator(struct oval_variable_possible_restriction *possible_restriction)
{
	__attribute__nonnull__(possible_restriction);

	return possible_restriction->operator;
}

char* oval_variable_possible_restriction_get_hint(struct oval_variable_possible_restriction* possible_restriction)
{
	__attribute__nonnull__(possible_restriction);

	return possible_restriction->hint;
}


bool oval_variable_possible_restriction_iterator_has_more(struct oval_variable_possible_restriction_iterator* iter)
{
	return oval_collection_iterator_has_more((struct oval_iterator*)iter);
}

struct oval_variable_possible_restriction* oval_variable_possible_restriction_iterator_next(struct oval_variable_possible_restriction_iterator* iter)
{
	return (struct oval_variable_possible_restriction*)oval_collection_iterator_next((struct oval_iterator*)iter);
}

void oval_variable_possible_restriction_iterator_free(struct oval_variable_possible_restriction_iterator* iter)
{
	oval_collection_iterator_free((struct oval_iterator*)iter);
}

int oval_variable_possible_restriction_iterator_remaining(struct oval_variable_possible_restriction_iterator* iter)
{
	return oval_collection_iterator_remaining((struct oval_iterator*)iter);
}


struct oval_variable_restriction *oval_variable_restriction_new(oval_operation_t operation, const char *value)
{
	struct oval_variable_restriction *r;
	r = oscap_alloc(sizeof(oval_variable_restriction_t));
	r->operation = operation;
	r->value = oscap_strdup(value);
	return r;
}

void oval_variable_restriction_free(struct oval_variable_restriction *r)
{
	if (r != NULL) {
		oscap_free(r->value);
		oscap_free(r);
	}
}

oval_operation_t oval_variable_restriction_get_operation(struct oval_variable_restriction* restriction)
{
    __attribute__nonnull__(restriction);

    return restriction->operation;
}

char* oval_variable_restriction_get_value(struct oval_variable_restriction* restriction)
{
    __attribute__nonnull__(restriction);

    return restriction->value;
}

bool oval_variable_restriction_iterator_has_more(struct oval_variable_restriction_iterator* iter)
{
	return oval_collection_iterator_has_more((struct oval_iterator*)iter);
}

struct oval_variable_restriction* oval_variable_restriction_iterator_next(struct oval_variable_restriction_iterator* iter)
{
	return (struct oval_variable_restriction*)oval_collection_iterator_next((struct oval_iterator*)iter);
}

void oval_variable_restriction_iterator_free(struct oval_variable_restriction_iterator* iter)
{
	oval_collection_iterator_free((struct oval_iterator*)iter);
}

int oval_variable_restriction_iterator_remaining(struct oval_variable_restriction_iterator* iter)
{
	return oval_collection_iterator_remaining((struct oval_iterator*)iter);
}


bool oval_variable_iterator_has_more(struct oval_variable_iterator
				     *oc_variable)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_variable);
}

struct oval_variable *oval_variable_iterator_next(struct
						  oval_variable_iterator
						  *oc_variable)
{
	return (struct oval_variable *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_variable);
}

void oval_variable_iterator_free(struct
				 oval_variable_iterator
				 *oc_variable)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_variable);
}

char *oval_variable_get_id(struct oval_variable *variable)
{
	__attribute__nonnull__(variable);

	return variable->id;
}

char *oval_variable_get_comment(struct oval_variable *variable)
{
	__attribute__nonnull__(variable);

	return variable->comment;
}

int oval_variable_get_version(struct oval_variable *variable)
{
	__attribute__nonnull__(variable);

	return variable->version;
}

bool oval_variable_get_deprecated(struct oval_variable * variable)
{
	__attribute__nonnull__(variable);

	return variable->deprecated;
}

oval_variable_type_t oval_variable_get_type(struct oval_variable * variable)
{
	__attribute__nonnull__(variable);

	return variable->type;
}

oval_datatype_t oval_variable_get_datatype(struct oval_variable * variable)
{
	__attribute__nonnull__(variable);

	return variable->datatype;
}

struct oval_value_iterator *oval_variable_get_values(struct oval_variable *variable)
{
	struct oval_collection *values;

	__attribute__nonnull__(variable);

	switch (variable->type) {
	case OVAL_VARIABLE_EXTERNAL: {
		oval_variable_EXTERNAL_t *var = (oval_variable_EXTERNAL_t *) variable;

		values = var->values_ref;
		break;
	}
	case OVAL_VARIABLE_CONSTANT: {
		oval_variable_CONSTANT_t *var = (oval_variable_CONSTANT_t *) variable;

		values = var->values;
		break;
	}
	case OVAL_VARIABLE_LOCAL: {
		oval_variable_LOCAL_t *var = (oval_variable_LOCAL_t *) variable;

		values = var->values;
		break;
	}
	default:
		values = NULL;
		break;
	}

	return (values) ? (struct oval_value_iterator *) oval_collection_iterator(values) : 
		(struct oval_value_iterator *) oval_collection_iterator_new();
}

struct oval_variable_possible_value_iterator *oval_variable_get_possible_values(struct oval_variable *variable)
{
	if (variable->type == OVAL_VARIABLE_EXTERNAL) {
		oval_variable_EXTERNAL_t *var = (oval_variable_EXTERNAL_t *) variable;
		return (struct oval_variable_possible_value_iterator*)oval_collection_iterator(var->possible_values);
	} else {
		return (struct oval_variable_possible_value_iterator*)oval_collection_iterator_new();
	}
}

struct oval_variable_possible_restriction_iterator *oval_variable_get_possible_restrictions(struct oval_variable *variable)
{
	if (variable->type == OVAL_VARIABLE_EXTERNAL) {
		oval_variable_EXTERNAL_t *var = (oval_variable_EXTERNAL_t *) variable;
		return (struct oval_variable_possible_restriction_iterator*)oval_collection_iterator(var->possible_restrictions);
	} else {
		return (struct oval_variable_possible_restriction_iterator*)oval_collection_iterator_new();
	}
}

bool oval_variable_contains_value(struct oval_variable *variable, const char* o_value_text)
{
	if (variable == NULL)
		return false;
	struct oval_value_iterator *value_it = oval_variable_get_values(variable);
	bool found = false;
	while (!found && oval_value_iterator_has_more(value_it)) {
		struct oval_value *value = oval_value_iterator_next(value_it);
		if (oscap_streq(oval_value_get_text(value), o_value_text))
			found = true;
	}
	oval_value_iterator_free(value_it);
	return found;
}

oval_syschar_collection_flag_t oval_variable_get_collection_flag(struct oval_variable *variable) {
	__attribute__nonnull__(variable);

	return variable->flag;
}

int oval_syschar_model_compute_variable(struct oval_syschar_model *sysmod, struct oval_variable *variable)
{
	oval_variable_LOCAL_t *var;
	struct oval_component *component;
	struct oval_value_iterator *val_itr;

	__attribute__nonnull__(variable);

	if (variable->type != OVAL_VARIABLE_LOCAL)
		return 0;

	var = (oval_variable_LOCAL_t *) variable;
	if (var->flag != SYSCHAR_FLAG_UNKNOWN)
		return 0;

	component = var->component;
        if (component) {
		if (!var->values)
			var->values = oval_collection_new();
		var->flag = oval_component_compute(sysmod, component, var->values);
	} else {
		dW("NULL component bound to a variable, id: %s.", var->id);
		return -1;
        }

	switch (var->flag) {
	case SYSCHAR_FLAG_COMPLETE:
	case SYSCHAR_FLAG_INCOMPLETE:
		break;
	default:
		return 0;
	}

	val_itr = oval_variable_get_values(variable);
	if (!oval_value_iterator_has_more(val_itr))
		var->flag = SYSCHAR_FLAG_ERROR;
	oval_value_iterator_free(val_itr);

        return 0;
}

int oval_probe_query_variable(oval_probe_session_t *sess, struct oval_variable *variable)
{
	oval_variable_LOCAL_t *var;
	struct oval_component *component;
	struct oval_value_iterator *val_itr;
	struct oscap_string *val_dump;

	__attribute__nonnull__(variable);

	if (variable->type != OVAL_VARIABLE_LOCAL)
		return 0;

	var = (oval_variable_LOCAL_t *) variable;
	if (var->flag != SYSCHAR_FLAG_UNKNOWN)
		return 0;

	dI("Querying variable '%s'.", var->id);
	component = var->component;
        if (component) {
		if (!var->values)
			var->values = oval_collection_new();
		var->flag = oval_component_query(sess, component, var->values);
	} else {
		dW("NULL component bound to a variable, id: %s.", var->id);
		return -1;
        }

	switch (var->flag) {
	case SYSCHAR_FLAG_COMPLETE:
	case SYSCHAR_FLAG_INCOMPLETE:
		break;
	default:
		dI("Variable '%s' has no values.", var->id);
		return 0;
	}

	val_itr = oval_variable_get_values(variable);
	if (!oval_value_iterator_has_more(val_itr)) {
		oval_value_iterator_free(val_itr);
		var->flag = SYSCHAR_FLAG_ERROR;
		return 0;
	}

	val_dump = oscap_string_new();
	oscap_string_append_char(val_dump, '\"');
	while(1) {
		struct oval_value *val;

		val = oval_value_iterator_next(val_itr);
		if (oval_value_cast(val, var->datatype) != 0) {
			oval_value_iterator_free(val_itr);
			var->flag = SYSCHAR_FLAG_ERROR;
			oscap_string_free(val_dump);
			return 0;
		}
		oscap_string_append_string(val_dump, oval_value_get_text(val));
		if (!oval_value_iterator_has_more(val_itr)) {
			break;
		}
		oscap_string_append_string(val_dump, "\", \"");
	}
	oscap_string_append_char(val_dump, '\"');
	dI("Variable '%s' has values %s.", var->id, oscap_string_get_cstr(val_dump));
	oscap_string_free(val_dump);
	oval_value_iterator_free(val_itr);

	return 0;
}

struct oval_component *oval_variable_get_component(struct oval_variable *variable)
{
	__attribute__nonnull__(variable);

	/*type==OVAL_VARIABLE_LOCAL */
	struct oval_component *component = NULL;
	if (oval_variable_get_type(variable) == OVAL_VARIABLE_LOCAL) {
		oval_variable_LOCAL_t *local = (oval_variable_LOCAL_t *) variable;
		component = local->component;
	}
	return component;
}

/* failed   - NULL 
 * success  - oval_variable
 * */
struct oval_variable *oval_variable_new(struct oval_definition_model *model, const char *id, oval_variable_type_t type)
{
	oval_variable_t *variable;

	switch (type) {
	case OVAL_VARIABLE_CONSTANT:{
			oval_variable_CONSTANT_t *cvar;

			variable = (oval_variable_t *) oscap_alloc(sizeof(oval_variable_CONSTANT_t));
			if (variable == NULL)
				return NULL;

			cvar = (oval_variable_CONSTANT_t *) variable;
			cvar->values = NULL;
			cvar->flag = SYSCHAR_FLAG_NOT_COLLECTED;
		}
		break;
	case OVAL_VARIABLE_EXTERNAL:{
			oval_variable_EXTERNAL_t *evar;

			variable = (oval_variable_t *) oscap_alloc(sizeof(oval_variable_EXTERNAL_t));
			if (variable == NULL)
				return NULL;

			evar = (oval_variable_EXTERNAL_t *) variable;
			evar->possible_values = oval_collection_new();
			evar->possible_restrictions = oval_collection_new();
			evar->values_ref = NULL;
			evar->flag = SYSCHAR_FLAG_NOT_COLLECTED;
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			oval_variable_LOCAL_t *lvar;

			variable = (oval_variable_t *) oscap_alloc(sizeof(oval_variable_LOCAL_t));
			if (variable == NULL)
				return NULL;

			lvar = (oval_variable_LOCAL_t *) variable;
			lvar->component = NULL;
			lvar->values = NULL;
			lvar->flag = SYSCHAR_FLAG_UNKNOWN;
		}
		break;
	case OVAL_VARIABLE_UNKNOWN:{
			variable = (oval_variable_t *) oscap_alloc(sizeof(oval_variable_UNKNOWN_t));
			if (variable == NULL)
				return NULL;

			memset(variable, 0, sizeof(oval_variable_UNKNOWN_t));
		};
		break;
	default:
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Variable type is not valid: %d.", type);
		return NULL;
	}

	variable->model = model;
	variable->id = oscap_strdup(id);
	variable->comment = NULL;
	variable->datatype = OVAL_DATATYPE_UNKNOWN;
	variable->type = type;

	oval_definition_model_add_variable(model, variable);

	return variable;
}

struct oval_variable *oval_variable_clone(struct oval_definition_model *new_model, struct oval_variable *old_variable) {
	__attribute__nonnull__(old_variable);

	oval_variable_t *new_variable = oval_definition_model_get_variable(new_model, old_variable->id);
	if (new_variable == NULL) {
		new_variable = oval_variable_new(new_model, old_variable->id, old_variable->type);

		oval_variable_set_comment(new_variable, old_variable->comment);
		oval_variable_set_version(new_variable, old_variable->version);
		oval_variable_set_deprecated(new_variable, old_variable->deprecated);
		oval_variable_set_datatype(new_variable, old_variable->datatype);
		new_variable->flag = old_variable->flag;

		switch (old_variable->type) {
		case OVAL_VARIABLE_CONSTANT: {
			oval_variable_CONSTANT_t *cvar;
			struct oval_value_iterator *old_val_itr;

			cvar = (oval_variable_CONSTANT_t *) new_variable;
			old_val_itr = oval_variable_get_values(old_variable);
			if (oval_value_iterator_has_more(old_val_itr))
				cvar->values = oval_collection_new();
			while (oval_value_iterator_has_more(old_val_itr)) {
				struct oval_value *val;

				val = oval_value_iterator_next(old_val_itr);
				val = oval_value_clone(val);
				oval_collection_add(cvar->values, val);
			}
			oval_value_iterator_free(old_val_itr);

			break;
		}
		case OVAL_VARIABLE_EXTERNAL: {
			oval_variable_EXTERNAL_t *evar, *old_evar;

			evar = (oval_variable_EXTERNAL_t *) new_variable;
			old_evar = (oval_variable_EXTERNAL_t *) old_variable;

			struct oval_variable_possible_value_iterator *old_pv_itr;
			old_pv_itr = oval_variable_get_possible_values(old_variable);
			while (oval_variable_possible_value_iterator_has_more(old_pv_itr)) {
				struct oval_variable_possible_value *old_pv, *new_pv;
				old_pv = oval_variable_possible_value_iterator_next(old_pv_itr);
				new_pv = oval_variable_possible_value_new(old_pv->hint, old_pv->value);
				oval_variable_add_possible_value(new_variable, new_pv);
			}
			oval_variable_possible_value_iterator_free(old_pv_itr);

			struct oval_variable_possible_restriction_iterator *old_pr_itr;
			old_pr_itr = oval_variable_get_possible_restrictions(old_variable);
			while (oval_variable_possible_restriction_iterator_has_more(old_pr_itr)) {
				struct oval_variable_possible_restriction *old_pr, *new_pr;
				old_pr = oval_variable_possible_restriction_iterator_next(old_pr_itr);
				new_pr = oval_variable_possible_restriction_new(old_pr->operator, old_pr->hint);
				struct oval_variable_restriction_iterator *old_r_itr;
				old_r_itr = oval_variable_possible_restriction_get_restrictions(old_pr);
				while (oval_variable_restriction_iterator_has_more(old_r_itr)) {
					struct oval_variable_restriction *old_r, *new_r;
					old_r = oval_variable_restriction_iterator_next(old_r_itr);
					new_r = oval_variable_restriction_new(old_r->operation, old_r->value);
					oval_variable_possible_restriction_add_restriction(new_pr, new_r);
				}
				oval_variable_restriction_iterator_free(old_r_itr);
				oval_variable_add_possible_restriction(new_variable, new_pr);
			}
			oval_variable_possible_restriction_iterator_free(old_pr_itr);

			evar->values_ref = old_evar->values_ref;

			break;
		}
		case OVAL_VARIABLE_LOCAL: {
			oval_variable_LOCAL_t *lvar, *old_lvar;
			struct oval_value_iterator *old_val_itr;

			lvar = (oval_variable_LOCAL_t *) new_variable;
			old_val_itr = oval_variable_get_values(old_variable);
			if (oval_value_iterator_has_more(old_val_itr))
				lvar->values = oval_collection_new();
			while (oval_value_iterator_has_more(old_val_itr)) {
				struct oval_value *val;

				val = oval_value_iterator_next(old_val_itr);
				val = oval_value_clone(val);
				oval_collection_add(lvar->values, val);
			}
			oval_value_iterator_free(old_val_itr);

			old_lvar = (oval_variable_LOCAL_t *) old_variable;
			lvar->component = oval_component_clone(new_model, old_lvar->component);

			break;
		}
		default:
			break;
		}
	}
	return new_variable;
}

void oval_variable_free(struct oval_variable *variable)
{
	if (variable) {
		if (variable->id)
			oscap_free(variable->id);
		if (variable->comment)
			oscap_free(variable->comment);
		variable->id = variable->comment = NULL;

		switch (variable->type) {
		case OVAL_VARIABLE_CONSTANT: {
			oval_variable_CONSTANT_t *cvar;

			cvar = (oval_variable_CONSTANT_t *) variable;
			if (cvar->values)
				oval_collection_free_items(cvar->values, (oscap_destruct_func) oval_value_free);
			cvar->values = NULL;

			break;
		}
		case OVAL_VARIABLE_EXTERNAL: {
			oval_variable_EXTERNAL_t *evar;

			evar = (oval_variable_EXTERNAL_t *) variable;
			oval_collection_free_items(evar->possible_values, (oscap_destruct_func) oval_variable_possible_value_free);
			oval_collection_free_items(evar->possible_restrictions, (oscap_destruct_func) oval_variable_possible_restriction_free);
			evar->values_ref = NULL;

			break;
		}
		case OVAL_VARIABLE_LOCAL: {
			oval_variable_LOCAL_t *lvar;

			lvar = (oval_variable_LOCAL_t *) variable;
			if (lvar->values)
				oval_collection_free_items(lvar->values, (oscap_destruct_func) oval_value_free);
			lvar->values = NULL;
			oval_component_free(lvar->component);

			break;
		}
		default:
			break;
		}

		oscap_free(variable);
	}
}

void oval_variable_set_datatype(struct oval_variable *variable, oval_datatype_t datatype)
{
	variable->datatype = datatype;
}

void oval_variable_set_type(struct oval_variable *variable, oval_variable_type_t new_type)
{
	if (variable->type != OVAL_VARIABLE_UNKNOWN) {
		dE("Attempt to reset valid variable type, oldtype: %s, newtype: %s.",
			       oval_variable_type_get_text(variable->type), oval_variable_type_get_text(new_type));
		return;
	}

	variable->type = new_type;

	switch (new_type) {
	case OVAL_VARIABLE_CONSTANT: {
		oval_variable_CONSTANT_t *cvar;

		cvar = (oval_variable_CONSTANT_t *) variable;
		cvar->values = NULL;
		cvar->flag = SYSCHAR_FLAG_NOT_COLLECTED;

		break;
	}
	case OVAL_VARIABLE_EXTERNAL: {
		oval_variable_EXTERNAL_t *evar;

		evar = (oval_variable_EXTERNAL_t *) variable;
		evar->values_ref = NULL;
		evar->possible_values = oval_collection_new();
		evar->possible_restrictions = oval_collection_new();
		evar->flag = SYSCHAR_FLAG_NOT_COLLECTED;

		break;
	}
	case OVAL_VARIABLE_LOCAL: {
		oval_variable_LOCAL_t *lvar;

		lvar = (oval_variable_LOCAL_t *) variable;
		lvar->component = NULL;
		lvar->values = NULL;
		lvar->flag = SYSCHAR_FLAG_UNKNOWN;

		break;
	}
	default:
		break;
	}
}

void oval_variable_set_comment(struct oval_variable *variable, char *comm)
{
	__attribute__nonnull__(variable);

	if (variable->comment != NULL)
		oscap_free(variable->comment);
	variable->comment = oscap_strdup(comm);

}

void oval_variable_set_deprecated(struct oval_variable *variable, bool deprecated)
{
	__attribute__nonnull__(variable);
	variable->deprecated = deprecated;
}

void oval_variable_set_version(struct oval_variable *variable, int version)
{
	__attribute__nonnull__(variable);
	variable->version = version;
}

void oval_variable_add_value(struct oval_variable *variable, struct oval_value *value)
{
	__attribute__nonnull__(variable);
	oval_variable_CONSTANT_t *cvar;

	if (variable->type != OVAL_VARIABLE_CONSTANT)
		return;

	cvar = (oval_variable_CONSTANT_t *) variable;
	if (!cvar->values)
		cvar->values = oval_collection_new();
	oval_collection_add(cvar->values, value);
	cvar->flag = SYSCHAR_FLAG_COMPLETE;
}

void oval_variable_add_possible_value(struct oval_variable *variable, struct oval_variable_possible_value *pv)
{
	if (variable->type == OVAL_VARIABLE_EXTERNAL) {
		oval_variable_EXTERNAL_t *var = (oval_variable_EXTERNAL_t *) variable;
		oval_collection_add(var->possible_values, pv);
	}
}

void oval_variable_add_possible_restriction(struct oval_variable *variable, struct oval_variable_possible_restriction *pr)
{
	if (variable->type == OVAL_VARIABLE_EXTERNAL) {
		oval_variable_EXTERNAL_t *var = (oval_variable_EXTERNAL_t *) variable;
		oval_collection_add(var->possible_restrictions, pr);
	}
}

void oval_variable_clear_values(struct oval_variable *variable)
{
	__attribute__nonnull__(variable);

	if (variable->type != OVAL_VARIABLE_CONSTANT && variable->type != OVAL_VARIABLE_EXTERNAL) {
		dW("Wrong variable type for this operation: %d.", variable->type);
		return;
        }

	switch (variable->type) {
	case OVAL_VARIABLE_CONSTANT: {
		oval_variable_CONSTANT_t *cvar;

		cvar = (oval_variable_CONSTANT_t *) variable;
		if (cvar->values) {
			oval_collection_free_items(cvar->values, (oscap_destruct_func) oval_value_free);
			cvar->values = NULL;
		}
		cvar->flag = SYSCHAR_FLAG_NOT_COLLECTED;

		break;
	}
	case OVAL_VARIABLE_EXTERNAL: {
		oval_variable_EXTERNAL_t *evar;

		evar = (oval_variable_EXTERNAL_t *) variable;
		evar->values_ref = NULL;
		evar->flag = SYSCHAR_FLAG_NOT_COLLECTED;

		break;
	}
	default:
		break;
	}
}

static int oval_value_satisfies_possible_restriction(struct oval_value *value, struct oval_variable_possible_restriction *pr)
{
	oval_datatype_t datatype = oval_value_get_datatype(value);
	char *text = oval_value_get_text(value);
	oval_operator_t operator = pr->operator;
	struct oresults results;
	ores_clear(&results);
	struct oval_variable_restriction_iterator *restrictions = oval_variable_possible_restriction_get_restrictions(pr);
	while (oval_variable_restriction_iterator_has_more(restrictions)) {
		struct oval_variable_restriction *r = oval_variable_restriction_iterator_next(restrictions);
		oval_result_t result = oval_str_cmp_str(r->value, datatype, text, r->operation);
		ores_add_res(&results, result);
	}
	oval_variable_restriction_iterator_free(restrictions);
	return (ores_get_result_byopr(&results, operator) == OVAL_RESULT_TRUE);
}

static int oval_variable_validate_ext_var(oval_variable_EXTERNAL_t *var, struct oval_collection *oval_values)
{
	if (oval_values == NULL)
		return 1;

	int retval = 0;
	if (!oval_collection_is_empty(var->possible_values) ||
		!oval_collection_is_empty(var->possible_restrictions)) {
		/* Check that the value of variable is allowed */
		struct oval_iterator *values = oval_collection_iterator(oval_values);
		while (!retval && oval_collection_iterator_has_more(values)) {
			struct oval_value *value = oval_collection_iterator_next(values);
			const char *text = oval_value_get_text(value);
			int found = 0;
			struct oval_iterator *possible_values = oval_collection_iterator(var->possible_values);
			while(!found && oval_collection_iterator_has_more(possible_values)) {
				struct oval_variable_possible_value *pv = oval_collection_iterator_next(possible_values);
				const char *constraint = pv->value;
				if (strcmp(text, constraint) == 0) {
					found = 1;
				}
			}
			oval_collection_iterator_free(possible_values);
			struct oval_iterator *possible_restrictions = oval_collection_iterator(var->possible_restrictions);
			while(!found && oval_collection_iterator_has_more(possible_restrictions)) {
				struct oval_variable_possible_restriction *pr = oval_collection_iterator_next(possible_restrictions);
				if (oval_value_satisfies_possible_restriction(value, pr)) {
					found = 1;
				}
			}
			oval_collection_iterator_free(possible_restrictions);
			if (!found) {
				retval = 1;
			}
		}
		oval_collection_iterator_free(values);
	}
	return retval;
}

int oval_variable_bind_ext_var(struct oval_variable *var, struct oval_variable_model *varmod, char *extvar_id)
{
	oval_variable_EXTERNAL_t *evar;

	if (var->type != OVAL_VARIABLE_EXTERNAL) {
		dW("Attemp to bind a non-external variable, id: %s, type: %s.", var->id, oval_variable_type_get_text(var->type));
		return 2;
	}

	evar = (oval_variable_EXTERNAL_t *) var;
	struct oval_collection *values_ref = oval_variable_model_get_values_ref(varmod, extvar_id);
	if (oval_variable_validate_ext_var(evar, values_ref)) {
		evar->flag = SYSCHAR_FLAG_DOES_NOT_EXIST;
		return 1;
	} else {
		evar->values_ref = values_ref;
		evar->flag = SYSCHAR_FLAG_COMPLETE;
		return 0;
	}
}

void oval_variable_set_component(struct oval_variable *variable, struct oval_component *component)
{
	__attribute__nonnull__(variable);

	if (variable->type == OVAL_VARIABLE_LOCAL) {
		oval_variable_LOCAL_t *local = (oval_variable_LOCAL_t *) variable;
		local->component = component;
	}
}

static void _oval_variable_parse_local_tag_component_consumer(struct oval_component *component, void *variable)
{
	oval_variable_set_component(variable, component);
}

static int _oval_variable_parse_local_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_variable *variable = (struct oval_variable *)user;
	xmlChar *tagname = xmlTextReaderLocalName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code = oval_component_parse_tag(reader, context, &_oval_variable_parse_local_tag_component_consumer, variable);
	if (return_code != 0) {
		dW("Parsing of %s terminated by an error at <%s>, line %d.", variable->id, tagname, xmlTextReaderGetParserLineNumber(reader));
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}


static int _oval_variable_parse_restriction_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_variable_possible_restriction *pr = (struct oval_variable_possible_restriction *)user;
	int return_code;
	oval_operation_t operation = oval_operation_parse(reader, "operation", OVAL_OPERATION_EQUALS);
	if (xmlTextReaderRead(reader) == 1) {
		char *value = (char *)xmlTextReaderValue(reader);
		struct oval_variable_restriction *restriction = oval_variable_restriction_new(operation, value);
		oval_variable_possible_restriction_add_restriction(pr, restriction);
		return_code = 0;
		oscap_free(value);
	} else {
		return_code = 1;
	}
	return return_code;
}

static int _oval_variable_parse_external_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_variable *variable = (struct oval_variable *)user;
	char *tagname = (char *) xmlTextReaderLocalName(reader);
	char *namespace = (char *) xmlTextReaderNamespaceUri(reader);
	int return_code;
	if (strcmp(tagname, "possible_value") == 0) {
		char *hint = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "hint");
		if (xmlTextReaderRead(reader) == 1) {
			char *value = (char *)xmlTextReaderValue(reader);
			struct oval_variable_possible_value *pv = oval_variable_possible_value_new(hint, value);
			oval_variable_add_possible_value(variable, pv);
			oscap_free(value);
			return_code = 0;
		} else {
			return_code = 1;
		}
		oscap_free(hint);
	} else if (strcmp(tagname, "possible_restriction") == 0) {
		oval_operator_t operator = oval_operator_parse(reader, "operator", OVAL_OPERATOR_AND);
		char *hint = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "hint");
		struct oval_variable_possible_restriction *pr = oval_variable_possible_restriction_new(operator, hint);
		oval_variable_add_possible_restriction(variable, pr);
		return_code = oval_parser_parse_tag(reader, context, _oval_variable_parse_restriction_tag, pr);
		oscap_free(hint);
	} else {
		return_code = 1;
	}
	if (return_code != 0) {
		dW("Parsing of %s terminated by an error at <%s>, line %d.", variable->id, tagname, xmlTextReaderGetParserLineNumber(reader));
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

#define DEFINITION_NAMESPACE "http://oval.mitre.org/XMLSchema/oval-definitions-5"

static void _const_tag_consumer(struct oval_value *val, void *user)
{
	oval_datatype_t dt;

	dt = oval_variable_get_datatype(user);
	oval_value_set_datatype(val, dt);
	oval_variable_add_value(user, val);
}

static int _oval_variable_parse_constant_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	xmlChar *tagname = xmlTextReaderLocalName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	struct oval_variable *variable = (struct oval_variable *)user;

	if (strcmp("value", (char *) tagname) || strcmp(DEFINITION_NAMESPACE, (char *) namespace)) {
		dW("Invalid element <%s:%s> in constant variable %s on line %d.",
		   namespace, tagname, variable->id, xmlTextReaderGetParserLineNumber(reader));
		goto out;
	}

	oval_value_parse_tag(reader, context, _const_tag_consumer, variable);

 out:
	oscap_free(tagname);
	oscap_free(namespace);
	return 0;
}

int oval_variable_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr)
{
	struct oval_definition_model *model = context->definition_model;
	char *tagname = NULL;
	char *id = NULL;
	char *comm = NULL;
	char *version = NULL;
	int return_code = 0;
	oval_variable_type_t type;

	tagname = (char *)xmlTextReaderLocalName(reader);
	if (strcmp(tagname, "constant_variable") == 0)
		type = OVAL_VARIABLE_CONSTANT;
	else if (strcmp(tagname, "external_variable") == 0)
		type = OVAL_VARIABLE_EXTERNAL;
	else if (strcmp(tagname, "local_variable") == 0)
		type = OVAL_VARIABLE_LOCAL;
	else {
		type = OVAL_VARIABLE_UNKNOWN;
		dW("Unhandled variable type: %s, line: %d.", tagname, xmlTextReaderGetParserLineNumber(reader));
	}
	id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	struct oval_variable *variable = oval_definition_model_get_new_variable(model, id, type);

	comm = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	if (comm != NULL) {
		oval_variable_set_comment(variable, comm);
	}

	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	oval_variable_set_deprecated(variable, deprecated);

	version = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	oval_variable_set_version(variable, atoi(version));

	oval_datatype_t datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_UNKNOWN);
	oval_variable_set_datatype(variable, datatype);

	switch (type) {
	case OVAL_VARIABLE_CONSTANT:{
			return_code = oval_parser_parse_tag(reader, context, _oval_variable_parse_constant_tag, variable);
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			return_code = oval_parser_parse_tag(reader, context, _oval_variable_parse_local_tag, variable);
		}
		break;
	case OVAL_VARIABLE_EXTERNAL:{
			return_code = oval_parser_parse_tag(reader, context, _oval_variable_parse_external_tag, variable);
			break;
		}
	default:
		return_code = 1;
	}

	oscap_free(tagname);
	oscap_free(id);
	oscap_free(comm);
	oscap_free(version);

	return return_code;
}

static xmlNode *_oval_VARIABLE_CONSTANT_to_dom(struct oval_variable *variable, xmlDoc * doc, xmlNode * parent) {
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *variable_node = xmlNewTextChild(parent, ns_definitions, BAD_CAST "constant_variable", NULL);

	struct oval_value_iterator *values = oval_variable_get_values(variable);
	while (oval_value_iterator_has_more(values)) {
		struct oval_value *value = oval_value_iterator_next(values);
		char *text = oval_value_get_text(value);
		xmlNewTextChild(variable_node, ns_definitions, BAD_CAST "value", BAD_CAST text);
	}
	oval_value_iterator_free(values);

	return variable_node;
}

static xmlNode *_oval_VARIABLE_EXTERNAL_to_dom(struct oval_variable *variable, xmlDoc * doc, xmlNode * parent) {

	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *variable_node = xmlNewTextChild(parent, ns_definitions, BAD_CAST "external_variable", NULL);

	struct oval_variable_possible_value_iterator *possible_values = oval_variable_get_possible_values(variable);
	while (oval_variable_possible_value_iterator_has_more(possible_values)) {
		struct oval_variable_possible_value *pv = oval_variable_possible_value_iterator_next(possible_values);
		xmlNode *possible_value_node = xmlNewTextChild(variable_node, ns_definitions, BAD_CAST "possible_value", BAD_CAST pv->value);
		xmlNewProp(possible_value_node, BAD_CAST "hint", BAD_CAST pv->hint);
	}
	oval_variable_possible_value_iterator_free(possible_values);

	oval_schema_version_t schema_version = oval_definition_model_get_core_schema_version(variable->model);
	bool serialize_operator = oval_schema_version_cmp(schema_version, OVAL_SCHEMA_VERSION(5.11)) >= 0;
	struct oval_variable_possible_restriction_iterator *possible_restrictions = oval_variable_get_possible_restrictions(variable);
	while (oval_variable_possible_restriction_iterator_has_more(possible_restrictions)) {
		struct oval_variable_possible_restriction *pr = oval_variable_possible_restriction_iterator_next(possible_restrictions);
		struct oval_variable_restriction_iterator *restrictions = oval_variable_possible_restriction_get_restrictions(pr);
		/* Create "possible_restriction" node only if there will be some
		 * "restriction" children, because each "possible_restriction"
		 * node must have at least one "restriction" child.
		 */
		if (oval_variable_restriction_iterator_has_more(restrictions)) {
			xmlNode *possible_restriction_node = xmlNewTextChild(variable_node, ns_definitions, BAD_CAST "possible_restriction", NULL);
			/* Attribute "operator" is new in OVAL 5.11, we don't serialize it in older OVAL versions */
			if (serialize_operator) {
				xmlNewProp(possible_restriction_node, BAD_CAST "operator", BAD_CAST oval_operator_get_text(pr->operator));
			}
			xmlNewProp(possible_restriction_node, BAD_CAST "hint", BAD_CAST pr->hint);
			while (oval_variable_restriction_iterator_has_more(restrictions)) {
				struct oval_variable_restriction *r = oval_variable_restriction_iterator_next(restrictions);
				xmlNode *restriction_node = xmlNewTextChild(possible_restriction_node, ns_definitions, BAD_CAST "restriction", BAD_CAST r->value);
				xmlNewProp(restriction_node, BAD_CAST "operation", BAD_CAST oval_operation_get_text(r->operation));
			}
		}
		oval_variable_restriction_iterator_free(restrictions);
	}
	oval_variable_possible_restriction_iterator_free(possible_restrictions);

	return variable_node;
}

static xmlNode *_oval_VARIABLE_LOCAL_to_dom(struct oval_variable *variable, xmlDoc * doc, xmlNode * parent) {
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *variable_node = xmlNewTextChild(parent, ns_definitions, BAD_CAST "local_variable", NULL);

	struct oval_component *component = oval_variable_get_component(variable);
	oval_component_to_dom(component, doc, variable_node);

	return variable_node;
}

xmlNode *oval_variable_to_dom(struct oval_variable * variable, xmlDoc * doc, xmlNode * parent)
{
	xmlNode *variable_node = NULL;
	switch (oval_variable_get_type(variable)) {
	case OVAL_VARIABLE_CONSTANT:{
			variable_node = _oval_VARIABLE_CONSTANT_to_dom(variable, doc, parent);
		}
		break;
	case OVAL_VARIABLE_EXTERNAL:{
			variable_node = _oval_VARIABLE_EXTERNAL_to_dom(variable, doc, parent);
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			variable_node = _oval_VARIABLE_LOCAL_to_dom(variable, doc, parent);
		}
		break;
	default:
		break;
	};

	char *id = oval_variable_get_id(variable);
	xmlNewProp(variable_node, BAD_CAST "id", BAD_CAST id);

	char version[10];
	*version = '\0';
	snprintf(version, sizeof(version), "%d", oval_variable_get_version(variable));
	xmlNewProp(variable_node, BAD_CAST "version", BAD_CAST version);

	oval_datatype_t datatype = oval_variable_get_datatype(variable);
	xmlNewProp(variable_node, BAD_CAST "datatype", BAD_CAST oval_datatype_get_text(datatype));

	char *comm = oval_variable_get_comment(variable);
	xmlNewProp(variable_node, BAD_CAST "comment", BAD_CAST comm);

	bool deprecated = oval_variable_get_deprecated(variable);
	if (deprecated)
		xmlNewProp(variable_node, BAD_CAST "deprecated", BAD_CAST "true");

	return variable_node;
}
