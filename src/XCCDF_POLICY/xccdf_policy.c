/*
 * xccdf_policy.c
 *
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
 *  Created on: Dec 16, 2009
 *      Author: David Niemoller
 *              Maros Barabas <mbarabas@redhat.com>
 */

#include <stdlib.h>
#include <string.h>

#include "public/xccdf_policy.h"
#include "../XCCDF/public/xccdf.h"

#include "../common/list.h"
#include "../_error.h"

/**
 * XCCDF policy model structure contains xccdf_benchmark as reference
 * to Benchmark element in XML file and list of policies that are
 * abstract structure of Profile element from benchmark file.
 */
struct xccdf_policy_model {

        struct xccdf_benchmark  * benchmark;    //< Benchmark element (root element of XML file)
	struct oscap_list       * policies;     //< List of xccdf_policy structures
};
/* Macros to generate iterators, getters and setters */
OSCAP_GETTER(struct xccdf_benchmark *, xccdf_policy_model, benchmark)
OSCAP_IGETINS(xccdf_policy, xccdf_policy_model, policies, policy)


/**
 * XCCDF policy structure is abstract (class) structure
 * of Profile element from benchmark.
 *
 * Structure contains rules and bound values to abstract
 * these lists from the benchmark file. Can be modified temporaly
 * so changes can be discarded or saved to the existing model.
 */
struct xccdf_policy {

        struct xccdf_item   * item;     //< Profile structure
        struct oscap_list   * selects;  //< Rules from profile
        struct oscap_list   * values;   //< Bound values of profile
};

/* Macros to generate iterators, getters and setters */
OSCAP_GETTER(struct xccdf_item *, xccdf_policy, item)
OSCAP_IGETINS(xccdf_item, xccdf_policy, selects, select)
OSCAP_IGETINS(xccdf_value_binding, xccdf_policy, values, value)


/**
 * XCCDF value binding structure is abstract (class) structure
 * of Value element from Profile.
 */
struct xccdf_value_binding {

        struct oscap_list   * refine_rules;     //< Changing rules
        struct oscap_list   * refine_values;    //< Changing values
        struct oscap_list   * set_values;       //< Setting values
};
/* Macros to generate iterators, getters and setters */
OSCAP_IGETINS(xccdf_refine_rule, xccdf_value_binding, refine_rules, refine_rule)
OSCAP_IGETINS(xccdf_refine_value, xccdf_value_binding, refine_values, refine_value)
OSCAP_IGETINS(xccdf_setvalue, xccdf_value_binding, set_values, set_value)


/***************************************************************************/
/* Declaration of static (private to this file) functions
 * These function shoud not be called from outside. For exporting these elements
 * has to call parent element's 
 */

/**
 * Filter function returning true if the rule is selected, false otherwise
 */
static bool xccdf_policy_is_selected(void *item, void *foo)
{
        if( xccdf_select_get_selected((struct xccdf_select *) item) )
            return true;
        else 
            return false;
}

/**
 * Filter function returning true if the rule match ruleid, false otherwise
 */
static bool xccdf_policy_is_rule(void *item, void *ruleid)
{
        if( !strcmp(xccdf_select_get_item((struct xccdf_select *) item), (char *) ruleid) )
            return true;
        else 
            return false;
}

/***************************************************************************/

/***************************************************************************/
/* Constructors of XCCDF POLICY structures xccdf_policy_*<structure>*_new()
 * More info in representive header file.
 * returns the type of <structure>
 */
struct xccdf_policy_model * xccdf_policy_model_new(struct xccdf_benchmark * benchmark) {

	__attribute__nonnull__(benchmark);

	struct xccdf_policy_model       * model;
        struct xccdf_profile_iterator   * profile_it;
        struct xccdf_profile            * profile;
        struct xccdf_policy             * policy;

	model = oscap_alloc(sizeof(struct xccdf_policy_model));
	if (model == NULL)
		return NULL;
	memset(model, 0, sizeof(struct xccdf_policy_model));

	model->benchmark = benchmark;
	model->policies  = oscap_list_new();

        /* Create policies from benchmark model */
        profile_it = xccdf_benchmark_get_profiles(benchmark);
        /* Iterate through profiles */
        while (xccdf_profile_iterator_has_more(profile_it)) {

            profile = xccdf_profile_iterator_next(profile_it);
            policy = xccdf_policy_new((struct xccdf_item *) profile);

            /* Should we set the error code and return NULL here ? */
            if (policy != NULL) oscap_list_add(model->policies, policy);
        }
        xccdf_profile_iterator_free(profile_it);

	return model;
}

struct xccdf_policy * xccdf_policy_new(struct xccdf_item * profile) {

	__attribute__nonnull__(profile);

	struct xccdf_policy                 * policy;
        struct xccdf_select_iterator        * sel_it;
        struct xccdf_select                 * sel;
        struct xccdf_setvalue_iterator      * value_it;
        struct xccdf_setvalue               * value;

	policy = oscap_alloc(sizeof(struct xccdf_policy));
	if (policy == NULL)
		return NULL;
	memset(policy, 0, sizeof(struct xccdf_policy));

	policy->item    = profile;
	policy->selects   = oscap_list_new();
	policy->values  = oscap_list_new();

        /* Create selects from benchmark model */
        sel_it = xccdf_profile_get_selects( (const struct xccdf_profile *) profile);
        /* Iterate through profiles */
        while (xccdf_select_iterator_has_more(sel_it)) {

            sel = xccdf_select_iterator_next(sel_it);
            /* Should we set the error code and return NULL here ? */
            if (sel != NULL) oscap_list_add(policy->selects, sel);
        }
        xccdf_select_iterator_free(sel_it);

        /* Create values from benchmark model */
        /* TODO: Change this to refine_rules, refine_values, set_values */
        value_it = xccdf_profile_get_setvalues( (const struct xccdf_profile *) profile);
        /* Iterate through profiles */
        while (xccdf_setvalue_iterator_has_more(value_it)) {

            value = xccdf_setvalue_iterator_next(value_it);
            /* Should we set the error code and return NULL here ? */
            if (value != NULL) oscap_list_add(policy->values, value);
        }
        xccdf_setvalue_iterator_free(value_it);

	return policy;
}
/***************************************************************************/


struct xccdf_select_iterator * xccdf_policy_get_selected_rules(struct xccdf_policy * policy) {

    return (struct xccdf_select_iterator *) oscap_iterator_new_filter( policy->selects, 
                                                                       (oscap_filter_func) xccdf_policy_is_selected, 
                                                                       NULL);
}

bool xccdf_policy_set_selected(struct xccdf_policy * policy, char * idref) {

    struct oscap_iterator *sel_it = 
        oscap_iterator_new_filter( policy->selects, (oscap_filter_func) xccdf_policy_is_rule, idref);
    if (oscap_iterator_get_itemcount(sel_it) > 0) {
        /* There is rule already, skip */
        return 0;
    }
    else {
        /* There is no such rule, add */
        struct xccdf_select * sel = NULL;
        //TODO: sel = xccdf_select_new <-- missing implementation
        oscap_list_add(policy->selects, sel);
        return 1;

    }
}

bool xccdf_policy_evaluate(struct xccdf_policy * policy) {

    /* Step 1: From policy get all selected rules */
    struct xccdf_select_iterator * rules;

    rules = xccdf_policy_get_selected_rules(policy);

    /* Step 2: Get all information from rules - each rule should have bindings or empty */
    /* Step 3: preprocces */
    /* Step 4: evaluate */

    return false;
}



struct xccdf_value_binding_iterator * xccdf_policy_get_bound_values(struct xccdf_policy *policy) {
    return NULL;
}

void xccdf_policy_export_variables(struct xccdf_policy *policy, char *export_namespace, struct oscap_export_target *target) {
}

void xccdf_policy_export_controls (struct xccdf_policy *policy, char *export_namespace, struct oscap_export_target *target) {
}

void xccdf_policy_import_results(struct xccdf_policy *policy, char *import_namespace, struct oscap_import_source *source) {
}

void xccdf_policy_export_results(struct xccdf_policy *policy, char *scoring_model_namespace, struct oscap_export_target *target) {
}

void xccdf_policy_model_free(struct xccdf_policy_model * model) {

        xccdf_benchmark_free(model->benchmark);
	oscap_list_free(model->policies, (oscap_destruct_func) xccdf_policy_free);
        oscap_free(model);
}

void xccdf_policy_free(struct xccdf_policy * policy) {

	oscap_free(policy->item);
	oscap_list_free(policy->selects, (oscap_destruct_func) xccdf_item_free);
	oscap_list_free(policy->values, (oscap_destruct_func) xccdf_value_binding_free);
        oscap_free(policy);
}

void xccdf_value_binding_free(struct xccdf_value_binding * binding) {

	oscap_list_free(binding->refine_rules, (oscap_destruct_func) xccdf_item_free);
	oscap_list_free(binding->refine_values, (oscap_destruct_func) xccdf_item_free);
	oscap_list_free(binding->set_values, (oscap_destruct_func) xccdf_item_free);
        oscap_free(binding);
}


