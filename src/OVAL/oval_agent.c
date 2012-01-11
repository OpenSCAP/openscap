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
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/reporter_priv.h"

#ifdef ENABLE_XCCDF
#include "oval_agent_xccdf_api.h"
#endif

struct oval_agent_session {
	char *filename;
	struct oval_definition_model * def_model;
	struct oval_variable_model *cur_var_model;
	struct oval_syschar_model    * sys_model;
	struct oval_syschar_model    * sys_models[2];
	struct oval_results_model    * res_model;
	oval_probe_session_t  * psess;
	struct oval_generator *gen_tpl;
};


#ifdef ENABLE_XCCDF
/**
 * Specification of structure for transformation of OVAL Result type
 * to XCCDF result type.
 */
struct oval_result_to_xccdf_spec {
	oval_result_t oval;
	xccdf_test_result_type_t xccdf;
};

/**
 * Array of transformation rules from OVAL Result type to XCCDF result type
 */
static const struct oval_result_to_xccdf_spec XCCDF_OVAL_RESULTS_MAP[] = {
	{OVAL_RESULT_TRUE, XCCDF_RESULT_PASS},
	{OVAL_RESULT_FALSE, XCCDF_RESULT_FAIL},
	{OVAL_RESULT_UNKNOWN, XCCDF_RESULT_UNKNOWN},
	{OVAL_RESULT_ERROR, XCCDF_RESULT_ERROR},
	{OVAL_RESULT_NOT_EVALUATED, XCCDF_RESULT_NOT_CHECKED},
	{OVAL_RESULT_NOT_APPLICABLE, XCCDF_RESULT_NOT_APPLICABLE},
	{0, 0}
};

#endif

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

	/* todo: accept 'gen_tpl' as an arg
	if (gen_tpl)
		oval_agent_set_generator_template(ag_sess, gen_tpl);
	else
	*/
		ag_sess->gen_tpl = NULL;

	return ag_sess;
}

void oval_agent_set_generator_template(oval_agent_session_t *ag_sess, struct oval_generator *gen_tpl)
{
	ag_sess->gen_tpl = gen_tpl;
	gen_tpl = oval_generator_clone(gen_tpl);
	oval_syschar_model_set_generator(ag_sess->sys_models[0], gen_tpl);
	gen_tpl = oval_generator_clone(gen_tpl);
	oval_results_model_set_generator(ag_sess->res_model, gen_tpl);
}

struct oval_generator *oval_agent_get_generator_template(oval_agent_session_t *ag_sess)
{
	return ag_sess->gen_tpl;
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
                dE("No results system in agent sessin.");
                oscap_seterr(OSCAP_EFAMILY_OSCAP, OVAL_EOVALINT, "Missing result system.");
                return -1;
	}

	rsystem = oval_result_system_iterator_next(rsystem_it);
	oval_result_system_iterator_free(rsystem_it);
	rdef = oval_result_system_get_definition(rsystem, id);
        if (rdef == NULL) {
                char msg[100];
                snprintf(msg, sizeof(msg), "No definition with ID: %s in result model.", id);
                dE(msg);
                oscap_seterr(OSCAP_EFAMILY_OSCAP, OVAL_EOVALINT, msg);
                return -1;
        }

	*result =  oval_result_definition_get_result(rdef);

	return 0;
}

int oval_agent_reset_session(oval_agent_session_t * ag_sess) {
	ag_sess->cur_var_model = NULL;
	oval_definition_model_clear_external_variables(ag_sess->def_model);

	/* Reset syschar model */
	oval_syschar_model_reset(ag_sess->sys_model);

	/* Replace result model */
	oval_results_model_free(ag_sess->res_model);
	ag_sess->res_model = oval_results_model_new(ag_sess->def_model, ag_sess->sys_models);

	/* Apply generator template to new model */
	if (ag_sess->gen_tpl) {
		struct oval_generator *gen_tpl;

		gen_tpl = oval_generator_clone(ag_sess->gen_tpl);
		oval_results_model_set_generator(ag_sess->res_model, gen_tpl);
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

int oval_agent_eval_system(oval_agent_session_t * ag_sess, oscap_reporter cb, void *arg) {
	struct oval_definition *oval_def;
	struct oval_definition_iterator *oval_def_it;
	char   *id;
	int ret = 0;
	oval_result_t result;

	oval_def_it = oval_definition_model_get_definitions(ag_sess->def_model);
	while (oval_definition_iterator_has_more(oval_def_it)) {
		oval_def = oval_definition_iterator_next(oval_def_it);
		id = oval_definition_get_id(oval_def);

		/* probe and eval */
		ret = oval_agent_eval_definition(ag_sess, id);
		if (ret==-1) {
			return -1;
		}

		/* callback */
                if (cb != NULL) {
			struct oscap_reporter_message * msg = oscap_reporter_message_new_fmt(
				OSCAP_REPORTER_FAMILY_OVAL, /* FAMILY */
				0,                           /* CODE */
				"%s",
				oval_definition_get_description(oval_def));
			oscap_reporter_message_set_user1str(msg, id);
			ret = oval_agent_get_definition_result(ag_sess, id, &result);
			if (ret == -1) {
				oscap_reporter_message_free(msg);
				goto cleanup;
			}

			oscap_reporter_message_set_user2num(msg, result);
			oscap_reporter_message_set_user3str(msg, oval_definition_get_title(oval_def));
			ret = oscap_reporter_report(cb, msg, arg);
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
	if (ag_sess->gen_tpl)
		oval_generator_free(ag_sess->gen_tpl);
	oval_probe_session_destroy(ag_sess->psess);
	oval_syschar_model_free(ag_sess->sys_model);
	oval_results_model_free(ag_sess->res_model);
        oscap_free(ag_sess->filename);
	oscap_free(ag_sess);
	ag_sess=NULL;
}


#ifdef ENABLE_XCCDF
/**
 * Function for OVAL Result type -> XCCDF result type transformation
 * @param id OVAL_RESULT_* type
 * @return xccdf_test_result_type_t
 */
static xccdf_test_result_type_t xccdf_get_result_from_oval(oval_result_t id)
{

	const struct oval_result_to_xccdf_spec *mapptr;

	for (mapptr = XCCDF_OVAL_RESULTS_MAP; mapptr->oval != 0; ++mapptr) {
		if (id == mapptr->oval)
			return mapptr->xccdf;
	}

	return XCCDF_RESULT_UNKNOWN;
}

int oval_agent_resolve_variables(struct oval_agent_session * session, struct xccdf_value_binding_iterator *it)
{
    bool conflict = false;
    int retval = 0;
    struct oval_value_iterator * value_it;

    if (!xccdf_value_binding_iterator_has_more(it))
        return 0;

    /* Get the definition model from OVAL agent session */
    struct oval_definition_model *def_model =
        oval_results_model_get_definition_model(oval_agent_get_results_model(session));

    /* Check the conflict */
    while (xccdf_value_binding_iterator_has_more(it)) {
        struct xccdf_value_binding *binding = xccdf_value_binding_iterator_next(it);
        struct oval_variable *variable = oval_definition_model_get_variable(def_model, xccdf_value_binding_get_name(binding));
        /* Do we have comflict ? */
        if (variable != NULL) {
            value_it = oval_variable_get_values(variable);
            char * value = xccdf_value_binding_get_setvalue(binding);
            if (value == NULL) value = xccdf_value_binding_get_value(binding);
            if (oval_value_iterator_has_more(value_it)) {
                struct oval_value * o_value = oval_value_iterator_next(value_it);
                if (strcmp(oval_value_get_text(o_value), value)) {
                    conflict = true;
                    oscap_dlprintf(DBG_W, "Variable conflict: %s has different values %s != %s\n", xccdf_value_binding_get_name(binding), oval_value_get_text(o_value), value);
                }
                else oscap_dlprintf(DBG_W, "Variable %s has the same value, skipping\n", xccdf_value_binding_get_name(binding));
            }
            oval_value_iterator_free(value_it);
        }
    }

    xccdf_value_binding_iterator_reset(it);
    if (conflict) {
        /* We have a conflict, clear session and external variables */
        oval_agent_reset_session(session);
    }

    if (!session->cur_var_model) {
	    session->cur_var_model = oval_variable_model_new();
	    oval_definition_model_bind_variable_model(def_model, session->cur_var_model);
	    /* Apply generator template to new model */
	    if (session->gen_tpl) {
		    struct oval_generator *gen_tpl;

		    gen_tpl = oval_generator_clone(session->gen_tpl);
		    oval_variable_model_set_generator(session->cur_var_model, gen_tpl);
	    }
    }

    /* Iterate through variable bindings and add variables into the variable model */
    while (xccdf_value_binding_iterator_has_more(it)) {
        struct xccdf_value_binding *binding = xccdf_value_binding_iterator_next(it);
        char *name = xccdf_value_binding_get_name(binding);
        char * value = xccdf_value_binding_get_setvalue(binding);
        if (value == NULL) value = xccdf_value_binding_get_value(binding);
        struct oval_variable *variable = oval_definition_model_get_variable(def_model, name);
        if (variable != NULL) {
                oval_datatype_t o_type = oval_variable_get_datatype(variable);
                /* TODO: check of variable type ? */
                if (oval_variable_model_has_variable(session->cur_var_model, name))
			oscap_dlprintf(DBG_E, "External variable %s in conflict! Probably content failure.\n", name);
                /* Add variable to variable model */
                value_it = oval_variable_get_values(variable);
		if (!oval_value_iterator_has_more(value_it)) {
			oval_variable_model_add(session->cur_var_model, name, "Unknown", o_type, value);
			oval_variable_bind_ext_var(variable, session->cur_var_model, name);
		} else {
			oscap_dlprintf(DBG_W, "External variable %s in conflict but with same value.\n", name);
		}
                oval_value_iterator_free(value_it);
        } else {
                oscap_dlprintf(DBG_W, "Variable %s does not exist, skipping.\n", name);
        }
    }

    return retval;
}

static int oval_agent_callback(const struct oscap_reporter_message *msg, void *arg)
{

        /*printf("Evalutated definition %s: %s\n",
                oscap_reporter_message_get_user1str(msg),
                oval_result_get_text(oscap_reporter_message_get_user2num(msg)));*/
	switch ((oval_result_t) oscap_reporter_message_get_user2num(msg)) {
            case OVAL_RESULT_FALSE:
            case OVAL_RESULT_UNKNOWN:
                    (*((int *)arg))++;
                    break;
            default:
                    break;
	}

	return 0;
}

xccdf_test_result_type_t oval_agent_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id,
			       const char * href, struct xccdf_value_binding_iterator *it, void *usr)
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
            /* If there is no such OVAL definition, return XCCDF_RESUL_NOT_CHECKED. XDCCDF should look for alternative definition in this case. */
            if (oval_definition_model_get_definition(oval_results_model_get_definition_model(oval_agent_get_results_model(sess)), id) == NULL)
                    return XCCDF_RESULT_NOT_CHECKED;
            /* Evaluate OVAL definition */
	    oval_agent_eval_definition(sess, id);
	    oval_agent_get_definition_result(sess, id, &result);
        } else {
            int res = 0;
            oval_agent_eval_system(sess, oval_agent_callback, (void *) &res);
            if (res == 0) return XCCDF_RESULT_PASS;
            else return XCCDF_RESULT_FAIL;
        }

	return xccdf_get_result_from_oval(result);
}


bool xccdf_policy_model_register_engine_oval(struct xccdf_policy_model * model, struct oval_agent_session * usr)
{

    return xccdf_policy_model_register_engine_callback(model, "http://oval.mitre.org/XMLSchema/oval-definitions-5", oval_agent_eval_rule, (void *) usr);
}

#endif
