/**
 * @file oval_agent.c
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
#include "adt/oval_string_map_impl.h"
#include "oval_system_characteristics_impl.h"
#include "results/oval_results_impl.h"
#if defined(OVAL_PROBES_ENABLED)
# include "oval_probe_impl.h"
#endif
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
#if defined(OVAL_PROBES_ENABLED)
	oval_probe_session_t  * psess;
#endif
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
	struct oval_generator *generator;
	int ret;

	dI("Started new OVAL agent.", name);

        /* Optimalization */
        oval_definition_model_optimize_by_filter_propagation(model);

	ag_sess = oscap_talloc(oval_agent_session_t);
        ag_sess->filename = oscap_strdup(name);
	ag_sess->def_model = model;
	ag_sess->cur_var_model = NULL;
	ag_sess->sys_model = oval_syschar_model_new(model);
#if defined(OVAL_PROBES_ENABLED)
	ag_sess->psess     = oval_probe_session_new(ag_sess->sys_model);
#endif

#if defined(OVAL_PROBES_ENABLED)
	/* probe sysinfo */
	ret = oval_probe_query_sysinfo(ag_sess->psess, &sysinfo);
	if (ret != 0) {
		oval_probe_session_destroy(ag_sess->psess);
		oval_syschar_model_free(ag_sess->sys_model);
		oscap_free(ag_sess);
		return NULL;
	}
#else
	/* TODO */
	sysinfo = oval_sysinfo_new(ag_sess->sys_model);
#endif /* OVAL_PROBES_ENABLED */
	oval_syschar_model_set_sysinfo(ag_sess->sys_model, sysinfo);
	oval_sysinfo_free(sysinfo);

	/* one system only */
	ag_sess->sys_models[0] = ag_sess->sys_model;
	ag_sess->sys_models[1] = NULL;
#if defined(OVAL_PROBES_ENABLED)
	ag_sess->res_model = oval_results_model_new_with_probe_session(
			model, ag_sess->sys_models, ag_sess->psess);
#endif
	generator = oval_results_model_get_generator(ag_sess->res_model);
	oval_generator_set_product_version(generator, oscap_get_version());


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

static struct oval_result_system *_oval_agent_get_first_result_system(oval_agent_session_t *ag_sess)
{
	struct oval_results_model *rmodel = oval_agent_get_results_model(ag_sess);
	struct oval_result_system_iterator *rsystem_it = oval_results_model_get_systems(rmodel);
	struct oval_result_system *rsystem = NULL;
	if (oval_result_system_iterator_has_more(rsystem_it)) {
		rsystem = oval_result_system_iterator_next(rsystem_it);
	}
	oval_result_system_iterator_free(rsystem_it);
	return rsystem;
}

int oval_agent_eval_definition(oval_agent_session_t *ag_sess, const char *id)
{
#if defined(OVAL_PROBES_ENABLED)
	int ret;
	struct oval_result_system *rsystem;

	rsystem = _oval_agent_get_first_result_system(ag_sess);
	/* eval */
	ret = oval_result_system_eval_definition(rsystem, id);
	return ret;
#else
	/* TODO */
	return -1;
#endif
}

int oval_agent_get_definition_result(oval_agent_session_t *ag_sess, const char *id, oval_result_t * result)
{
	struct oval_result_system *rsystem;
	struct oval_result_definition *rdef;

	rsystem = _oval_agent_get_first_result_system(ag_sess);
	if (rsystem == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "No results system in agent session.");
                return -1;
	}

	rdef = oval_result_system_get_definition(rsystem, id);
        if (rdef == NULL) {
                oscap_seterr(OSCAP_EFAMILY_OSCAP, "No definition with ID: %s in result model.", id);
                return -1;
        }

	*result =  oval_result_definition_get_result(rdef);

	return 0;
}

struct oval_result_definition * oval_agent_get_result_definition(oval_agent_session_t *ag_sess, const char *id) {
	struct oval_result_system *rsystem;
	struct oval_result_definition *rdef;

	rsystem = _oval_agent_get_first_result_system(ag_sess);
	if (rsystem == NULL)
		return NULL;

	rdef = oval_result_system_get_definition(rsystem, id);

	return rdef;
}

int oval_agent_reset_session(oval_agent_session_t * ag_sess) {
	ag_sess->cur_var_model = NULL;
	oval_definition_model_clear_external_variables(ag_sess->def_model);

	/* We intentionally do not flush out the results model which should
	 * be able to encompass results from multiple evaluations */

	/* Here we temporarily do not flush out the system characteristics model
	 * because there might be tests in the results model which refer to this
	 * sys char (via @tested_item attribute). Later we will need a mechanism
	 * to selectivelly invalidate some of the system characteristics cached
	 * objects (similar to the concept of variable_instance_hint from the
	 * results model. Hooray corner cases! */
	//oval_syschar_model_reset(ag_sess->sys_model);

	/* Apply product name to new results_model */
	if (ag_sess->product_name) {
		struct oval_generator *generator;

	        generator = oval_results_model_get_generator(ag_sess->res_model);
        	oval_generator_set_product_name(generator, ag_sess->product_name);
	}
#if defined(OVAL_PROBES_ENABLED)
	oval_probe_session_destroy(ag_sess->psess);
	ag_sess->psess = oval_probe_session_new(ag_sess->sys_model);
#endif
	return 0;
}

int oval_agent_abort_session(oval_agent_session_t *ag_sess)
{
	assume_d(ag_sess != NULL, -1);
	assume_d(ag_sess->psess != NULL, -1);
#if defined(OVAL_PROBES_ENABLED)
	return oval_probe_session_abort(ag_sess->psess);
#else
	/* TODO */
	return 0;
#endif
}

int oval_agent_eval_system(oval_agent_session_t * ag_sess, agent_reporter cb, void *arg) {
	struct oval_definition *oval_def;
	struct oval_definition_iterator *oval_def_it;
	char   *id;
	int ret = 0;

	dI("OVAL agent started to evaluate OVAL definitions on your system.");
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
	dI("OVAL agent finished evaluation.");
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
	if (ag_sess != NULL) {
		oscap_free(ag_sess->product_name);
#if defined(OVAL_PROBES_ENABLED)
		oval_probe_session_destroy(ag_sess->psess);
#endif
		oval_syschar_model_free(ag_sess->sys_model);
		oval_results_model_free(ag_sess->res_model);
	        oscap_free(ag_sess->filename);
		oscap_free(ag_sess);
	}
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
		if (var_val == NULL) {
			var_val = xccdf_value_binding_get_value(binding);
			if (var_val == NULL) {
				var_val = "";
			}
		}
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
			if (_stringlist_conflicts_with_value_it(value_list, value_it)) {
				// You don't want to touch this code. There are also other means to waste your
				// life. Now please proceed by reading the previous line again.
				//
				// Now, we have found that the variable we are trying to bind into the session
				// is already there with different values. This is the rise of concept which
				// is often referred as variable_instance or simply multiset.
				//
				// As per OVAL 5.10.1, the Variable Schema does not allow multisets. Therefore,
				// we will later create new variable model and export multiple variables docs.
				conflict = true;
				// Next, in the results model, there might be already some definitions, tests
				// states, or objects. These might be dependent on the previous value of the
				// given variable.
				//
				// The 'latest' result-definition for each such definition (whose result depends
				// on the value) needs to be marked by 'variable_instance_hint'. The hint has
				// meaning that any possible future evaluation of the given definition needs
				// to create new result-definition and not re-use the old one.
				//
				// Both (or all) such result-definitions are then distinguished by different
				// @variable_instance attribute. And each result-definition refers to different
				// set of tests. These tests might have same @id but differ in @variable_instance
				// attribute. Further, some of these tests will differ in tested_variable element.
				struct oval_result_system *r_system = _oval_agent_get_first_result_system(session);
				if (r_system == NULL)
					continue;

				struct oval_string_iterator *def_it =
					oval_definition_model_get_definitions_dependent_on_variable(def_model, variable);
				while (oval_string_iterator_has_more(def_it)) {
					char *definition_id = oval_string_iterator_next(def_it);

					struct oval_result_definition *r_definition = oval_result_system_get_definition(r_system, definition_id);
					if (r_definition != NULL) {
						// Here we simply increase the variable_instance_hint, however
						// in future we might want to do better and have a single session wide
						// counter and set the variable_instance_hints to this given counter.
						// That would allow the one-to-one mapping of variable_instance attributes
						// to the oval_variable files.
						int instance = oval_result_definition_get_instance(r_definition);
						oval_result_definition_set_variable_instance_hint(r_definition, instance + 1);
						struct oval_definition *definition = oval_result_definition_get_definition(r_definition);
#if defined(OVAL_PROBES_ENABLED)
						oval_probe_hint_definition(session->psess, definition, instance + 1);
#endif
					}
					else {
						// TODO: We really need oval_agent_session wide variable_instance attribute
						// to be able to correctly handle syschars even when there is no result-definition.
					}
				}
				oval_string_iterator_free(def_it);
			}
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
        if (value == NULL) {
		value = xccdf_value_binding_get_value(binding);
		if (value == NULL) {
			value = "";
		}
	}
        struct oval_variable *variable = oval_definition_model_get_variable(def_model, name);
        if (variable != NULL) {
                oval_datatype_t o_type = oval_variable_get_datatype(variable);
		if (!oval_variable_contains_value(variable, value)) {
			/* Add variable to variable model */
			oval_variable_model_add(session->cur_var_model, name, "Unknown", o_type, value);
			oval_variable_bind_ext_var(variable, session->cur_var_model, name);
			dI("Adding external variable %s.", name);
		} else {
			/* Skip this variable (we assume it has same values otherwise conflict was detected) */
			dW("Skipping external variable %s.", name);
		}
        } else {
                dW("Variable %s does not exist, skipping.", name);
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

