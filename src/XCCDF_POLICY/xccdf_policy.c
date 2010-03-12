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

        struct xccdf_profile    * profile;  //< Profile structure
        struct oscap_list       * rules;    //< Rules from profile
        struct oscap_list       * values;   //< Bound values of profile
};

/* Macros to generate iterators, getters and setters */
OSCAP_GETTER(struct xccdf_profile *, xccdf_policy, profile)
OSCAP_IGETINS(xccdf_select, xccdf_policy, rules, rule)
OSCAP_IGETINS(xccdf_value_binding, xccdf_policy, values, value)


/**
 * XCCDF value binding structure is abstract (class) structure
 * of Value element from Profile.
 */
struct xccdf_value_binding {

        struct oscap_list   * refine_rules;     //< Changing rules
        struct oscap_list   * refine_values;    //< Changing values
        struct oscap_list   * setvalues;       //< Setting values
};
/* Macros to generate iterators, getters and setters */
OSCAP_IGETINS(xccdf_refine_rule, xccdf_value_binding, refine_rules, refine_rule)
OSCAP_IGETINS(xccdf_refine_value, xccdf_value_binding, refine_values, refine_value)
OSCAP_IGETINS(xccdf_setvalue, xccdf_value_binding, setvalues, setvalue)


/***************************************************************************/
/* Declaration of static (private to this file) functions
 * These function shoud not be called from outside. For exporting these elements
 * has to call parent element's 
 */

/**
 * Filter function returning true if the rule is selected, false otherwise
 */
static bool xccdf_policy_filter_selected(void *item, void *foo)
{
        if( xccdf_select_get_selected((struct xccdf_select *) item) )
            return true;
        else 
            return false;
}

/**
 * Filter function returning true if the rule match ruleid, false otherwise
 */
static bool xccdf_policy_filter_rule(void *item, void *ruleid)
{
        if( !strcmp(xccdf_select_get_item((struct xccdf_select *) item), (char *) ruleid) )
            return true;
        else 
            return false;
}

/**
 * Resolve profile to policy:
 * get all selects from all rules / groups if they're selected and
 * if not add them with selected="0"
 */
/*static xccdf_ * xccdf_policy_model_resolve() {
}*/

static bool xccdf_policy_has_rule(struct xccdf_policy * policy, const char *rule_id)
{
        __attribute__nonnull__(policy);
        
        struct xccdf_select_iterator    * sel_it;
        struct xccdf_select             * sel;

        sel_it = xccdf_policy_get_rules(policy);
        while (xccdf_select_iterator_has_more(sel_it)) {
                sel = xccdf_select_iterator_next(sel_it);
                if (!strcmp(xccdf_select_get_item(sel), rule_id)) return true;
        }
        return false;
}

static void xccdf_policy_resolve_rule(struct xccdf_policy * policy, struct xccdf_item * item)
{
        __attribute__nonnull__(policy);
        __attribute__nonnull__(item);

        struct xccdf_item_iterator  * child_it;
        struct xccdf_item           * child;
        struct xccdf_select         * sel;

        xccdf_type_t itype = xccdf_item_get_type(item);

        /* Add current item */
        switch (itype) {
            case XCCDF_RULE:{
                if (!xccdf_policy_has_rule(policy, xccdf_rule_get_id((const struct xccdf_rule *)item))) {
                        sel = xccdf_select_new();
                        xccdf_select_set_selected(sel, xccdf_rule_get_selected((const struct xccdf_rule *)item));
                        xccdf_select_set_item(sel, xccdf_rule_get_id((const struct xccdf_rule *)item));
                        xccdf_policy_add_rule(policy, sel);
                } /* else it has a rule already and a policy 
                     priority inherited from the profile is higher */
            } break;
            case XCCDF_GROUP:{
                if (!xccdf_policy_has_rule(policy, xccdf_group_get_id((const struct xccdf_group *)item))) {
                        sel = xccdf_select_new();
                        xccdf_select_set_selected(sel, xccdf_group_get_selected((const struct xccdf_group *)item));
                        xccdf_select_set_item(sel, xccdf_group_get_id((const struct xccdf_group *)item));
                        xccdf_policy_add_rule(policy, sel);
                } /* else it has a rule already and a policy 
                     priority inherited from the profile is higher */
                /* It is a group, if selected is 0, then group will be not processed, but if selected is 1 or nothing 
                 * by default, group will be processed */
                if (xccdf_group_get_selected((const struct xccdf_group *)item)) { /* it's selected */
                        child_it = xccdf_group_get_content((const struct xccdf_group *)item);
                        while (xccdf_item_iterator_has_more(child_it)) {
                                child = xccdf_item_iterator_next(child_it);
                                xccdf_policy_resolve_rule(policy, child);
                        }
                        xccdf_item_iterator_free(child_it);
                }
            } break;
            
            default: 
              /* TODO: set warning bad argument and return ? */
              break;

        } 
}

/* This shouldn't be here at all */
static bool xccdf_policy_evaluate_oval(const char * id) 
{
    /* struct oval_result_system *sys;
    struct oval_result_definition *rdef = oval_result_system_get_definition(sys, id)
    */
    return false;
}

/* Name collision with xccdf_check -> changed to xccdf_policy_check */
static bool xccdf_policy_check_evaluate(struct xccdf_check * check)
{
    struct xccdf_check_iterator             * child_it;
    struct xccdf_check                      * child;
    struct xccdf_check_content_ref_iterator * content_it;
    struct xccdf_check_content_ref          * content;
    bool                                      ret       = false;
    const char                              * content_name;

    /* At least one of check-content or check-content-ref must
        * appear in each check element. */
    // TODO: ask lkuklinek how is this implemented to get all contents here
    if (xccdf_check_get_complex(check)) { /* we have complex subtree */
            child_it = xccdf_check_get_children(check);
            while (xccdf_check_iterator_has_more(child_it)) {
                child = xccdf_check_iterator_next(child_it);
                ret = xccdf_policy_check_evaluate(child);
                if (ret == false) break;
            }
            xccdf_check_iterator_free(child_it);
    } else { /* This is <check> element */
            /* It depends on what operation we process - we do only Compliance Check */
            content_it = xccdf_check_get_content_refs(check);
            while (xccdf_check_content_ref_iterator_has_more(content_it)) {
                content = xccdf_check_content_ref_iterator_next(content_it);
                content_name = xccdf_check_content_ref_get_name(content);
                /* Check if this is OVAL ? Never mind. Added to TODO */
                ret = xccdf_policy_evaluate_oval(content_name);
            }
            xccdf_check_content_ref_iterator_free(content_it);
    }
    /* Process check-import and check-export elements here */
    return false;
}


/* Name collision with xccdf_item -> changed to xccdf_policy_item */
static bool xccdf_policy_item_evaluate(struct xccdf_item * item)
{
    struct xccdf_check_iterator     * check_it;
    struct xccdf_check              * check;
    struct xccdf_item_iterator      * child_it;
    struct xccdf_item               * child;
    bool                              ret       = false;

    xccdf_type_t itype = xccdf_item_get_type(item);

    switch (itype) {
        case XCCDF_RULE:{
                    /* Get all checks of rule */
                    check_it = xccdf_rule_get_checks((const struct xccdf_rule *)item);
                    /* we need to evaluate all checks in rule, iteration begin */
                    while(xccdf_check_iterator_has_more(check_it)) {
                            check = xccdf_check_iterator_next(check_it);
                            ret = xccdf_policy_check_evaluate(check);

                            if (ret == false) /* we got item that can't be processed */
                                break;
                    }
                    xccdf_check_iterator_free(check_it);
                    /* iteration thorugh checks ends here */;
        } break;

        case XCCDF_GROUP:{
                    child_it = xccdf_group_get_content((const struct xccdf_group *)item);
                    while (xccdf_item_iterator_has_more(child_it)) {
                            child = xccdf_item_iterator_next(child_it);
                            ret = xccdf_policy_item_evaluate(item);

                            if (ret == false) /* we got item that can't be processed */
                                break;
                    }
                    xccdf_item_iterator_free(child_it);
        } break;
        
        default: 
                    /* TODO: set warning bad argument and return ? */
                    ret=false;
            break;
    } 

    return ret;
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
            policy = xccdf_policy_new(profile);

            /* Should we set the error code and return NULL here ? */
            if (policy != NULL) oscap_list_add(model->policies, policy);
        }
        xccdf_profile_iterator_free(profile_it);

	return model;
}

struct xccdf_policy * xccdf_policy_new(struct xccdf_profile * profile) {

	__attribute__nonnull__(profile);

	struct xccdf_policy             * policy;
        struct xccdf_select_iterator    * sel_it;
        struct xccdf_select             * sel;
        struct xccdf_value_binding      * binding;
        struct xccdf_benchmark          * benchmark;
        struct xccdf_item_iterator      * item_it;
        struct xccdf_item               * item;

	policy = oscap_alloc(sizeof(struct xccdf_policy));
	if (policy == NULL)
		return NULL;
	memset(policy, 0, sizeof(struct xccdf_policy));

	policy->profile = profile;
	policy->rules = oscap_list_new();
	policy->values  = oscap_list_new();

        /* Create selects from benchmark model */
        sel_it = xccdf_profile_get_selects(profile);
        /* Iterate through selects in profile */
        while (xccdf_select_iterator_has_more(sel_it)) {

            sel = xccdf_select_iterator_next(sel_it);
            /* Should we set the error code and return NULL here ? */
            if (sel != NULL) oscap_list_add(policy->rules, xccdf_select_clone(sel));
            //if (sel != NULL) oscap_list_add(policy->rules, oscap_strdup(xccdf_select_get_item(sel)));
        }
        xccdf_select_iterator_free(sel_it);

        /* Iterate through items in benchmark */
        benchmark = xccdf_profile_get_benchmark(profile);
        item_it = xccdf_benchmark_get_content(benchmark);
        while (xccdf_item_iterator_has_more(item_it)) {
            
            item = xccdf_item_iterator_next(item_it);
            xccdf_policy_resolve_rule(policy, item);
        }
        xccdf_item_iterator_free(item_it);

        /* Create values from benchmark model */
        binding = xccdf_value_binding_new(profile);
        /* TODO: Clone */
        if (binding) oscap_list_add(policy->values, binding);

	return policy;
}

struct xccdf_value_binding * xccdf_value_binding_new(const struct xccdf_profile * profile) {

        __attribute__nonnull__(profile);

        struct xccdf_refine_rule_iterator   * refine_rule_it;
        struct xccdf_refine_rule            * refine_rule;
        struct xccdf_refine_value_iterator  * refine_value_it;
        struct xccdf_refine_value           * refine_value;
        struct xccdf_setvalue_iterator      * setvalue_it;
        struct xccdf_setvalue               * setvalue;
        struct xccdf_value_binding          * binding;

        /* Initialization */
	binding = oscap_alloc(sizeof(struct xccdf_value_binding));
	if (binding == NULL)
		return NULL;
	memset(binding, 0, sizeof(struct xccdf_value_binding));

	binding->refine_rules   = oscap_list_new();
	binding->refine_values  = oscap_list_new();
	binding->setvalues     = oscap_list_new();

        /* Get all refine rules */
        refine_rule_it = xccdf_profile_get_refine_rules(profile);
        while (xccdf_refine_rule_iterator_has_more(refine_rule_it)) {

            refine_rule = xccdf_refine_rule_iterator_next(refine_rule_it);
            if (refine_rule != NULL) oscap_list_add(binding->refine_rules, refine_rule);
        }
        xccdf_refine_rule_iterator_free(refine_rule_it);

        /* Get all refine values */
        refine_value_it = xccdf_profile_get_refine_values(profile);
        while (xccdf_refine_value_iterator_has_more(refine_value_it)) {

            refine_value = xccdf_refine_value_iterator_next(refine_value_it);
            if (refine_value != NULL) oscap_list_add(binding->refine_values, refine_value);
        }
        xccdf_refine_value_iterator_free(refine_value_it);

        /* Get all set-values */
        setvalue_it = xccdf_profile_get_setvalues(profile);
        while (xccdf_setvalue_iterator_has_more(setvalue_it)) {

            setvalue = xccdf_setvalue_iterator_next(setvalue_it);
            if (setvalue != NULL) oscap_list_add(binding->setvalues, setvalue);
        }
        xccdf_setvalue_iterator_free(setvalue_it);

        return binding;
}
/***************************************************************************/


struct xccdf_select_iterator * xccdf_policy_get_selected_rules(struct xccdf_policy * policy) {

    return (struct xccdf_select_iterator *) oscap_iterator_new_filter( policy->rules, 
                                                                       (oscap_filter_func) xccdf_policy_filter_selected, 
                                                                       NULL);
}

bool xccdf_policy_set_selected(struct xccdf_policy * policy, char * idref) {

    struct oscap_iterator *sel_it = 
        oscap_iterator_new_filter( policy->rules, (oscap_filter_func) xccdf_policy_filter_rule, idref);
    if (oscap_iterator_get_itemcount(sel_it) > 0) {
        /* There is rule already, skip */
        return 0;
    }
    else {
        /* There is no such rule, add */
        struct xccdf_select * sel = NULL;
        //TODO: sel = xccdf_select_new <-- missing implementation
        oscap_list_add(policy->rules, sel);
        return 1;

    }
}

bool xccdf_policy_evaluate(struct xccdf_policy * policy) 
{

    /* Step 1: From policy get all selected rules */
    struct xccdf_select_iterator    * sel_it;
    struct xccdf_select             * sel;
    struct xccdf_item               * item;
    struct xccdf_benchmark          * benchmark;
    /*struct xccdf_item_iterator      * child_it;*/
    /*struct xccdf_item               * child;*/
    bool                              ret       = false;

    /* Get all constant information */
    benchmark = xccdf_profile_get_benchmark(xccdf_policy_get_profile(policy));

    sel_it = xccdf_policy_get_selected_rules(policy);
    while (xccdf_select_iterator_has_more(sel_it)) {
        sel = xccdf_select_iterator_next(sel_it);
        /* Step 2: Get all information from rules - each rule should have bindings or be empty */
        
        /* Here we process the item of benchmark - we have 3 options:
         *          1) The processing type is Tailoring             < TODO
         *          2) The processing type is Document Generation   < TODO
         *          3) The processing type is Compliance Checking   < Below
         *  It depends of item types of rule and given profile
         */

        /* Step 3: Compliance Checking */
        /* Get the refid string and find xccdf_item in benchmark */
        /* TODO: we need to check if every requirement is met - some of required Item has to be sleected too */
        
        item = xccdf_benchmark_get_item(benchmark, xccdf_select_get_item(sel));
        
        //xccdf_type_t itype = xccdf_item_get_type(item);
        /* We know that this group selection was overrided by profile. Each child of this group will be
         * evaluated, but could be unselected so it's selection is up to item evaluation function */
        /*if (itype == XCCDF_GROUP) {
            child_it = xccdf_group_get_content((const struct xccdf_group *)item);
            while (xccdf_item_iterator_has_more(child_it)) {
                child = xccdf_item_iterator_next(child_it);
                //TODO: handle more returns here 
                ret = xccdf_policy_item_evaluate(item);
            }
            xccdf_item_iterator_free(child_it);
          } else ret = xccdf_policy_item_evaluate(item);
        */

        ret = xccdf_policy_item_evaluate(item);
    }
    xccdf_select_iterator_free(sel_it);

    return ret;
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

	xccdf_item_free((struct xccdf_item *)policy->profile);
	oscap_list_free(policy->rules, (oscap_destruct_func) xccdf_select_free);
	oscap_list_free(policy->values, (oscap_destruct_func) xccdf_value_binding_free);
        oscap_free(policy);
}

void xccdf_value_binding_free(struct xccdf_value_binding * binding) {

	oscap_list_free(binding->refine_rules, (oscap_destruct_func) xccdf_refine_rule_free);
	oscap_list_free(binding->refine_values, (oscap_destruct_func) xccdf_refine_value_free);
	oscap_list_free(binding->setvalues, (oscap_destruct_func) xccdf_setvalue_free);
        oscap_free(binding);
}


