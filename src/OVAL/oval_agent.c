/**
 * @file oval_agent.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
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
 *      "Peter Vrabec" <pvrabec@redhat.com>
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <time.h>
#include <assume.h>

#include "oval_agent_api.h"
#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"
#include "oval_system_characteristics_impl.h"
#include "oval_probe_impl.h"
#include "oval_results_impl.h"
#include "common/list.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "oval_agent_xccdf_api.h"

struct oval_agent_session {
	char *filename;
	char *product_name;

	struct oval_definition_model * def_model;
	struct oval_variable_model *cur_var_model;
	struct oval_syschar_model    * sys_model;
	struct oval_syschar_model    * sys_models[2];
	struct oval_results_model    * res_model;
	oval_probe_session_t  * psess;
};


/**
 * Specification of structure for transformation of OVAL Result type
 * to XCCDF result type.
 */
struct oval_result_to_xccdf_spec {
	oval_result_t oval;
	xccdf_test_result_type_t xccdf;
	xccdf_test_result_type_t reversed_xccdf;
};

/**
 * Array of transformation rules from OVAL Result type to XCCDF result type
 */
static const struct oval_result_to_xccdf_spec XCCDF_OVAL_RESULTS_MAP[] = {
	{OVAL_RESULT_TRUE, XCCDF_RESULT_PASS, XCCDF_RESULT_FAIL},
	{OVAL_RESULT_FALSE, XCCDF_RESULT_FAIL, XCCDF_RESULT_PASS},
	{OVAL_RESULT_UNKNOWN, XCCDF_RESULT_UNKNOWN, XCCDF_RESULT_UNKNOWN},
	{OVAL_RESULT_ERROR, XCCDF_RESULT_ERROR, XCCDF_RESULT_ERROR},
	{OVAL_RESULT_NOT_EVALUATED, XCCDF_RESULT_NOT_CHECKED, XCCDF_RESULT_NOT_CHECKED},
	{OVAL_RESULT_NOT_APPLICABLE, XCCDF_RESULT_NOT_APPLICABLE, XCCDF_RESULT_NOT_APPLICABLE},
	{0, 0, 0}
};

oval_agent_session_t * oval_agent_new_session(struct oval_definition_model *model, const char * name) {
	oval_agent_session_t *ag_sess;
	struct oval_sysinfo *sysinfo;
	int ret;

        /* Optimalization */
        oval_definition_model_optimize_by_filter_propagation(model);

	ag_sess = oscap_talloc(oval_agent_session_t);
        ag_sess->filename = oscap_strdup(name);
	ag_sess->def_model = model;
	ag_sess->cur_var_model = NULL;
	ag_sess->sys_model = oval_syschar_model_new(model);
	ag_sess->psess     = oval_probe_session_new(ag_sess->sys_model);

	/* probe sysinfo */
	ret = oval_probe_query_sysinfo(ag_sess->psess, &sysinfo);
	if (ret != 0) {
		oval_probe_session_destroy(ag_sess->psess);
		oval_syschar_model_free(ag_sess->sys_model);
		oscap_free(ag_sess);
		return NULL;
	}
	oval_syschar_model_set_sysinfo(ag_sess->sys_model, sysinfo);
	oval_sysinfo_free(sysinfo);

	/* one system only */
	ag_sess->sys_models[0] = ag_sess->sys_model;
	ag_sess->sys_models[1] = NULL;
	ag_sess->res_model = oval_results_model_new(model, ag_sess->sys_models);

	ag_sess->product_name = NULL;

	return ag_sess;
}

struct oval_definition_model* oval_agent_get_definition_model(oval_agent_session_t* ag_sess)
{
	return ag_sess->def_model;
}

void oval_agent_set_product_name(oval_agent_session_t *ag_sess, char * product_name)
{
	struct oval_generator *generator;
	ag_sess->product_name = strdup(product_name);

	generator = oval_syschar_model_get_generator(ag_sess->sys_models[0]);
	oval_generator_set_product_name(generator, product_name);

	generator = oval_results_model_get_generator(ag_sess->res_model);
	oval_generator_set_product_name(generator, product_name);
}

int oval_agent_eval_definition(oval_agent_session_t *ag_sess, const char *id)
{
	int ret;
	struct oval_result_system_iterator *rsystem_it;
	struct oval_result_system *rsystem;

	/* probe */
	ret = oval_probe_query_definition(ag_sess->psess, id);
	if (ret == -1)
		return ret;

	/* take the first system */
	rsystem_it = oval_results_model_get_systems(ag_sess->res_model);
	rsystem = oval_result_system_iterator_next(rsystem_it);
        oval_result_system_iterator_free(rsystem_it);
	/* eval */
	ret = oval_result_system_eval_definition(rsystem, id);

	return ret;
}

int oval_agent_get_definition_result(oval_agent_session_t *ag_sess, const char *id, oval_result_t * result)
{
	struct oval_results_model *rmodel;
	struct oval_result_system_iterator *rsystem_it;
	struct oval_result_system *rsystem;
	struct oval_result_definition *rdef;

	rmodel = oval_agent_get_results_model(ag_sess);
	rsystem_it = oval_results_model_get_systems(rmodel);
	if (!oval_result_system_iterator_has_more(rsystem_it)) {
		oval_result_system_iterator_free(rsystem_it);
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "No results system in agent session.");
                return -1;
	}

	rsystem = oval_result_system_iterator_next(rsystem_it);
	oval_result_system_iterator_free(rsystem_it);
	rdef = oval_result_system_get_definition(rsystem, id);
        if (rdef == NULL) {
                oscap_seterr(OSCAP_EFAMILY_OSCAP, "No definition with ID: %s in result model.", id);
                return -1;
        }

	*result =  oval_result_definition_get_result(rdef);

	return 0;
}

struct oval_result_definition * oval_agent_get_result_definition(oval_agent_session_t *ag_sess, const char *id) {
	struct oval_results_model *rmodel;
	struct oval_result_system_iterator *rsystem_it;
	struct oval_result_system *rsystem;
	struct oval_result_definition *rdef;

	rmodel = oval_agent_get_results_model(ag_sess);
	rsystem_it = oval_results_model_get_systems(rmodel);
	if (!oval_result_system_iterator_has_more(rsystem_it)) {
		oval_result_system_iterator_free(rsystem_it);
                return NULL;
	}

	rsystem = oval_result_system_iterator_next(rsystem_it);
	oval_result_system_iterator_free(rsystem_it);
	rdef = oval_result_system_get_definition(rsystem, id);

	return rdef;
}

int oval_agent_reset_session(oval_agent_session_t * ag_sess) {
	ag_sess->cur_var_model = NULL;
	oval_definition_model_clear_external_variables(ag_sess->def_model);

	/* Reset syschar model */
	oval_syschar_model_reset(ag_sess->sys_model);

	/* Replace result model */
	oval_results_model_free(ag_sess->res_model);
	ag_sess->res_model = oval_results_model_new(ag_sess->def_model, ag_sess->sys_models);

	/* Apply product name to new results_model */
	if (ag_sess->product_name) {
		struct oval_generator *generator;

	        generator = oval_results_model_get_generator(ag_sess->res_model);
        	oval_generator_set_product_name(generator, ag_sess->product_name);
	}

	oval_probe_session_destroy(ag_sess->psess);
	ag_sess->psess = oval_probe_session_new(ag_sess->sys_model);

	return 0;
}

int oval_agent_abort_session(oval_agent_session_t *ag_sess)
{
	assume_d(ag_sess != NULL, -1);
	assume_d(ag_sess->psess != NULL, -1);

	return oval_probe_session_abort(ag_sess->psess);
}

int oval_agent_eval_system(oval_agent_session_t * ag_sess, agent_reporter cb, void *arg) {
	struct oval_definition *oval_def;
	struct oval_definition_iterator *oval_def_it;
	char   *id;
	int ret = 0;

	oval_def_it = oval_definition_model_get_definitions(ag_sess->def_model);
	while (oval_definition_iterator_has_more(oval_def_it)) {
		oval_def = oval_definition_iterator_next(oval_def_it);
		id = oval_definition_get_id(oval_def);

		/* probe and eval */
		ret = oval_agent_eval_definition(ag_sess, id);
		if (ret==-1) {
			goto cleanup;
		}

		/* callback */
                if (cb != NULL) {
			struct oval_result_definition * res_def = oval_agent_get_result_definition(ag_sess, id);
			ret = cb(res_def,arg);
			/* stop? */
			if (ret!=0)
				goto cleanup;
		}

		/* probe evaluation terminated by signal */
		if (ret == -2) {
			ret = 1;
			break;
		}
	}

cleanup:
	oval_definition_iterator_free(oval_def_it);
	return ret;
}

struct oval_results_model * oval_agent_get_results_model(oval_agent_session_t * ag_sess) {
	__attribute__nonnull__(ag_sess);

	return ag_sess->res_model;
}

const char * oval_agent_get_filename(oval_agent_session_t * ag_sess) {
	__attribute__nonnull__(ag_sess);

	return ag_sess->filename;
}

void oval_agent_destroy_session(oval_agent_session_t * ag_sess) {
	if (ag_sess->product_name)
		oscap_free(ag_sess->product_name);
	oval_probe_session_destroy(ag_sess->psess);
	oval_syschar_model_free(ag_sess->sys_model);
	oval_results_model_free(ag_sess->res_model);
        oscap_free(ag_sess->filename);
	oscap_free(ag_sess);
	ag_sess=NULL;
}


/**
 * Function for OVAL Result type -> XCCDF result type transformation
 * @param id OVAL_RESULT_* type
 * @return xccdf_test_result_type_t
 */
static xccdf_test_result_type_t xccdf_get_result_from_oval(oval_definition_class_t class, oval_result_t id)
{

	const struct oval_result_to_xccdf_spec *mapptr;

	for (mapptr = XCCDF_OVAL_RESULTS_MAP; mapptr->oval != 0; ++mapptr) {
		if (id == mapptr->oval)
			// SP800-126r2: Deriving XCCDF Check Results from OVAL Definition Results
			return (class == OVAL_CLASS_VULNERABILITY || class == OVAL_CLASS_PATCH) ? mapptr->reversed_xccdf : mapptr->xccdf;
	}

	return XCCDF_RESULT_UNKNOWN;
}

/**
 * Transform the value_bindings to intermediary mapping.
 * @param it XCCDF value binding iterator
 * @return dict structure which for each OVAL variable name contains a list of binded values
 */
static struct oscap_htable *_binding_iterator_to_dict(struct xccdf_value_binding_iterator *it)
{
	struct oscap_htable *dict = oscap_htable_new();
	while (xccdf_value_binding_iterator_has_more(it)) {
		struct xccdf_value_binding *binding = xccdf_value_binding_iterator_next(it);
		const char *var_name = xccdf_value_binding_get_name(binding);
		const char *var_val = xccdf_value_binding_get_setvalue(binding);
		if (var_val == NULL)
			var_val = xccdf_value_binding_get_value(binding);
		struct oscap_stringlist *list = (struct oscap_stringlist *) oscap_htable_get(dict, var_name);
		if (list == NULL) {
			list = oscap_stringlist_new();
			oscap_htable_add(dict, var_name, list);
		}

		if (!oscap_list_contains((struct oscap_list *) list, (char *) var_val, (oscap_cmp_func) oscap_streq)) {
			oscap_stringlist_add_string(list, var_val);
		}
	}
	xccdf_value_binding_iterator_reset(it);
	return dict;
}

/**
 * Ensure that the newly binded values do not pose conflict with existing value set.
 * The lists should be equal or the existing must be empty.
 */
static bool _stringlist_conflicts_with_value_it(struct oscap_stringlist *slist, struct oval_value_iterator *val_it)
{
	int multival_count = 0;
	if (!oval_value_iterator_has_more(val_it))
		return false;
	while (oval_value_iterator_has_more(val_it)) {
		struct oval_value *o_value = oval_value_iterator_next(val_it);
		char *o_value_text = oval_value_get_text(o_value);

		if (!oscap_list_contains((struct oscap_list *) slist, o_value_text, (oscap_cmp_func) oscap_streq))
			return true;
		multival_count++;
	}
	// It is conflict if the numbers do not match
	return oscap_list_get_itemcount((struct oscap_list *) slist) != multival_count;
}

/**
 * Finds out, if the new batch of variable bindings compel new variable model
 * (so-called multiset). Creates new variable model if needed.
 */
static void _oval_agent_resolve_variables_conflict(struct oval_agent_session *session, struct xccdf_value_binding_iterator *it)
{
	const char *var_name = NULL;
	struct oscap_stringlist *value_list = NULL;
	bool conflict = false;
	struct oscap_htable *dict = _binding_iterator_to_dict(it);
	struct oscap_htable_iterator *hit = oscap_htable_iterator_new(dict);
	struct oval_definition_model *def_model =
			oval_results_model_get_definition_model(oval_agent_get_results_model(session));
	while (!conflict && oscap_htable_iterator_has_more(hit)) {
		oscap_htable_iterator_next_kv(hit, &var_name, (void*) &value_list);
		struct oval_variable *variable = oval_definition_model_get_variable(def_model, var_name);
		if (variable != NULL) {
			struct oval_value_iterator *value_it = oval_variable_get_values(variable);
			if (_stringlist_conflicts_with_value_it(value_list, value_it))
				conflict = true;
			oval_value_iterator_free(value_it);
		}
	}
	oscap_htable_iterator_free(hit);
	oscap_htable_free(dict, (oscap_destruct_func) oscap_stringlist_free);

    if (conflict) {
        /* We have a conflict, clear session and external variables */
        oval_agent_reset_session(session);
    }

    if (!session->cur_var_model) {
	    session->cur_var_model = oval_variable_model_new();
	    oval_definition_model_bind_variable_model(def_model, session->cur_var_model);
	    /* Apply generator template to new model */
	    if (session->product_name) {
			struct oval_generator *generator;

			generator = oval_variable_model_get_generator(session->cur_var_model);
			oval_generator_set_product_name(generator, session->product_name);
	    }
    }
}

int oval_agent_resolve_variables(struct oval_agent_session * session, struct xccdf_value_binding_iterator *it)
{
	int retval = 0;

	if (!xccdf_value_binding_iterator_has_more(it))
		return 0;

	_oval_agent_resolve_variables_conflict(session, it);

	/* Get the definition model from OVAL agent session */
	struct oval_definition_model *def_model =
		oval_results_model_get_definition_model(oval_agent_get_results_model(session));

    /* Iterate through variable bindings and add variables into the variable model */
    while (xccdf_value_binding_iterator_has_more(it)) {
        struct xccdf_value_binding *binding = xccdf_value_binding_iterator_next(it);
        char *name = xccdf_value_binding_get_name(binding);
        char * value = xccdf_value_binding_get_setvalue(binding);
        if (value == NULL) value = xccdf_value_binding_get_value(binding);
        struct oval_variable *variable = oval_definition_model_get_variable(def_model, name);
        if (variable != NULL) {
                oval_datatype_t o_type = oval_variable_get_datatype(variable);
		if (!oval_variable_contains_value(variable, value)) {
			/* Add variable to variable model */
			oval_variable_model_add(session->cur_var_model, name, "Unknown", o_type, value);
			oval_variable_bind_ext_var(variable, session->cur_var_model, name);
			oscap_dlprintf(DBG_I, "Adding external variable %s.\n", name);
		} else {
			/* Skip this variable (we assume it has same values otherwise conflict was detected) */
			oscap_dlprintf(DBG_W, "Skipping external variable %s.\n", name);
		}
        } else {
                oscap_dlprintf(DBG_W, "Variable %s does not exist, skipping.\n", name);
        }
    }

    return retval;
}

static int
oval_agent_eval_multi_check(oval_agent_session_t *sess)
{
	// This is special handling of evaluation when the process is driven by XCCDF
	// and xccdf:check/@multi-check=false. We need extra function for this to apply
	// the XCCDF Truth Table for AND.
	struct oval_definition *oval_def;
	struct oval_definition_iterator *oval_def_it;
	const char *id;
	oval_result_t oval_result;
	xccdf_test_result_type_t xccdf_result;
	xccdf_test_result_type_t final_result = 0;

	oval_def_it = oval_definition_model_get_definitions(sess->def_model);
	if (!oval_definition_iterator_has_more(oval_def_it)) {
		// We are evaluating oval, which has no definitions. We are in state
		// which is not explicitly covered in SCAP 1.2 Specification, we are
		// better to report error.
		final_result = XCCDF_RESULT_ERROR;
	}
	while (oval_definition_iterator_has_more(oval_def_it)) {
		oval_def = oval_definition_iterator_next(oval_def_it);
		id = oval_definition_get_id(oval_def);

		// Evaluate definition.
		assume_r(oval_agent_eval_definition(sess, id) != -1, -1);
		assume_r(oval_agent_get_definition_result(sess, id, &oval_result) != -1, -1);
		// Get XCCDF equivalent of the oval result.
		xccdf_result = xccdf_get_result_from_oval(oval_definition_get_class(oval_def), oval_result);
		// AND as described in (NISTIR-7275r4): Table 12: Truth Table for AND
		final_result = (final_result == 0) ? xccdf_result :
			xccdf_test_result_resolve_and_operation(final_result, xccdf_result);
	}
	oval_definition_iterator_free(oval_def_it);
	return final_result;
}

xccdf_test_result_type_t oval_agent_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id,
			       const char * href, struct xccdf_value_binding_iterator *it,
			       struct xccdf_check_import_iterator * check_import_it,
			       void *usr)
{
        __attribute__nonnull__(usr);

        oval_result_t result;
        int retval = 0;
	struct oval_agent_session * sess = (struct oval_agent_session *) usr;
        if (strcmp(sess->filename, href))
            return XCCDF_RESULT_NOT_CHECKED;

        /* Resolve variables */
        retval = oval_agent_resolve_variables(sess, it);
        if (retval != 0) return XCCDF_RESULT_UNKNOWN;

        if (id != NULL) {
		struct oval_definition *definition = oval_definition_model_get_definition(oval_results_model_get_definition_model(oval_agent_get_results_model(sess)), id);
            /* If there is no such OVAL definition, return XCCDF_RESUL_NOT_CHECKED. XDCCDF should look for alternative definition in this case. */
            if (definition == NULL)
                    return XCCDF_RESULT_NOT_CHECKED;
            /* Evaluate OVAL definition */
	    oval_agent_eval_definition(sess, id);
	    oval_agent_get_definition_result(sess, id, &result);
		return xccdf_get_result_from_oval(oval_definition_get_class(definition), result);
        } else {
		return oval_agent_eval_multi_check(sess);
        }
}

static void *
_oval_agent_list_definitions(void *usr, xccdf_policy_engine_query_t query_type, void *query_data)
{
	__attribute__nonnull__(usr);
	struct oval_agent_session *sess = (struct oval_agent_session *) usr;
	if (query_type != POLICY_ENGINE_QUERY_NAMES_FOR_HREF || (query_data != NULL && strcmp(sess->filename, (const char *) query_data)))
		return NULL;
	struct oval_definition_iterator *iterator = oval_definition_model_get_definitions(sess->def_model);
	struct oscap_stringlist *result = oscap_stringlist_new();
	struct oval_definition *oval_def;

	while (oval_definition_iterator_has_more(iterator)) {
		oval_def = oval_definition_iterator_next(iterator);
		oscap_stringlist_add_string(result, oval_definition_get_id(oval_def));
	}

	oval_definition_iterator_free(iterator);
	return result;
}

bool xccdf_policy_model_register_engine_oval(struct xccdf_policy_model * model, struct oval_agent_session * usr)
{

    return xccdf_policy_model_register_engine_and_query_callback(model, "http://oval.mitre.org/XMLSchema/oval-definitions-5",
		oval_agent_eval_rule, (void *) usr, _oval_agent_list_definitions);
}

void oval_agent_export_sysinfo_to_xccdf_result(struct oval_agent_session * sess, struct xccdf_result * ritem)
{
	// just a delegate to maintain API and ABI stability,
	// this has been implemented elsewhere and this function is deprecated!

	xccdf_result_fill_sysinfo(ritem);
}

