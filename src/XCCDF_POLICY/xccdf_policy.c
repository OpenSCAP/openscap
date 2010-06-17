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
#include <math.h> /* For NAN <- TODO */

#include "public/xccdf_policy.h"
#include "../XCCDF/public/xccdf.h"

#include "../common/list.h"
#include "../_error.h"
#include "../common/public/text.h"

/**
 * Typedef of callback structure with system identificator, callback function and usr data (optional) 
 * On evaluation action will be selected checking system and appropriate callback registred by tool
 * for that system.
 */
typedef struct callback_t {

    char * system;                              ///< Identificator of checking engine
    bool (*callback) (struct xccdf_policy_model *,              // Policy model
                      const char *,                             // Rule ID
                      const char *,                             // Definition ID
                      struct xccdf_value_binding_iterator * it, // Value Bindings Iterator
                      void *);                  ///< format of callback function 
    void * usr;                                 ///< User data structure

} callback;

/**
 * XCCDF policy model structure contains xccdf_benchmark as reference
 * to Benchmark element in XML file and list of policies that are
 * abstract structure of Profile element from benchmark file.
 */
struct xccdf_policy_model {

        struct xccdf_benchmark  * benchmark;    ///< Benchmark element (root element of XML file)
	struct oscap_list       * policies;     ///< List of xccdf_policy structures
        struct oscap_list       * callbacks;    ///< Callbacks for checking engines (see callback_t)
        struct oscap_list       * results;      ///< List of XCCDF results
};
/* Macros to generate iterators, getters and setters */
OSCAP_GETTER(struct xccdf_benchmark *, xccdf_policy_model, benchmark)
OSCAP_IGETINS_GEN(xccdf_policy, xccdf_policy_model, policies, policy)
OSCAP_IGETINS(xccdf_result, xccdf_policy_model, results, result)

/**
 * XCCDF policy structure is abstract (class) structure
 * of Profile element from benchmark.
 *
 * Structure contains rules and bound values to abstract
 * these lists from the benchmark file. Can be modified temporaly
 * so changes can be discarded or saved to the existing model.
 */
struct xccdf_policy {

        struct xccdf_policy_model   * model;    ///< XCCDF Policy model
        struct xccdf_profile        * profile;  ///< Profile structure (from benchmark)
        struct oscap_list           * rules;    ///< Selected rules of profile
        struct oscap_list           * values;   ///< Bound values of profile
};

/* Macros to generate iterators, getters and setters */
OSCAP_GETTER(struct xccdf_policy_model *, xccdf_policy, model)
OSCAP_GETTER(struct xccdf_profile *, xccdf_policy, profile)
OSCAP_IGETINS(xccdf_select, xccdf_policy, rules, rule)
OSCAP_IGETINS_GEN(xccdf_value_binding, xccdf_policy, values, value)

/**
 * XCCDF value binding structure is binding between Refine values, Set values, 
 * Value element and Check export element of benchmark. These structures are 
 * binded together for exporting values to checking engine.
 *
 * These bindings are set during the preprocessing of profile, when policies 
 * are beeing created.
 */
struct xccdf_value_binding {

        char * name;                ///< The name of OVAL Variable
        xccdf_value_type_t type;    ///< Type of Variable
        char * value;               ///< Value of variable
        char * setvalue;            ///< Set value if defined or NULL
        xccdf_operator_t operator;  ///< Operator of Value
};
/* Macros to generate iterators, getters and setters */
OSCAP_GETTER(xccdf_value_type_t, xccdf_value_binding, type)
OSCAP_GETTER(char *, xccdf_value_binding, name)
OSCAP_GETTER(char *, xccdf_value_binding, value)
OSCAP_GETTER(char *, xccdf_value_binding, setvalue)
OSCAP_GETTER(xccdf_operator_t, xccdf_value_binding, operator)

/***************************************************************************/
/* Declaration of static (private to this file) functions
 * These function shoud not be called from outside. For exporting these elements
 * has to call parent element's 
 */

/**
 * Get callback from callback structure when system is selected 
 * Parameter policy handle callback structure and sysname the name of system specified in XCCDF file
 */
static callback * xccdf_policy_get_callback(struct xccdf_policy * policy, const char * sysname) {

        struct oscap_list * cbs = policy->model->callbacks;

        struct oscap_iterator * callback_it = oscap_iterator_new(cbs);
        while (oscap_iterator_has_more(callback_it)) {
            callback * cb = oscap_iterator_next(callback_it);

            if ((cb->system == NULL) && (sysname == NULL)) {
                oscap_iterator_free(callback_it);
                return cb;
            }
            else if ((cb->system == NULL) || (sysname == NULL)) continue;

            else if (!strcmp(cb->system, sysname)) {
                oscap_iterator_free(callback_it);
                return cb;
            }
        }
        return NULL;
        
}

/**
 * Filter function returning true if the item is selected, false otherwise
 * This function is only called from iterator
 */
static bool xccdf_policy_filter_selected(void *item, void *foo)
{
        if (xccdf_select_get_selected((struct xccdf_select *) item))
            return true;
        else 
            return false;
}

/**
 * Filter function returning true if the rule match ruleid, false otherwise
 * This function is only called from iterator
 */
static bool xccdf_policy_filter_rule(void *item, void *ruleid)
{
        if( !strcmp(xccdf_select_get_item((struct xccdf_select *) item), (char *) ruleid) )
            return true;
        else 
            return false;
}

/**
 * Return true if given policy has the rule, false otherwise
 */
static bool xccdf_policy_has_rule(struct xccdf_policy * policy, const char *rule_id)
{
        __attribute__nonnull__(policy);
        
        struct xccdf_select_iterator    * sel_it;
        struct xccdf_select             * sel;

        sel_it = xccdf_policy_get_rules(policy);
        while (xccdf_select_iterator_has_more(sel_it)) {
                sel = xccdf_select_iterator_next(sel_it);
                if (!strcmp(xccdf_select_get_item(sel), rule_id)) {
                    xccdf_select_iterator_free(sel_it);
                    return true;
                }
        }
        xccdf_select_iterator_free(sel_it);
        return false;
}

static struct xccdf_setvalue * xccdf_policy_get_setvalue(struct xccdf_policy * policy, const char * id)
{
    if (id == NULL) return NULL;
    if (policy == NULL) return NULL;

    struct xccdf_setvalue_iterator  * s_value_it;
    struct xccdf_setvalue           * s_value;

    s_value_it = xccdf_profile_get_setvalues(xccdf_policy_get_profile(policy));
    while (xccdf_setvalue_iterator_has_more(s_value_it)) {
        s_value = xccdf_setvalue_iterator_next(s_value_it);
        if (!strcmp(id, xccdf_setvalue_get_item(s_value))) {
            xccdf_setvalue_iterator_free(s_value_it);
            return s_value;
        }
    }
    xccdf_setvalue_iterator_free(s_value_it);

    return NULL;
}

static struct xccdf_refine_value * xccdf_policy_get_refine_value(struct xccdf_policy * policy, const char * id)
{
    if (id == NULL) return NULL;
    if (policy == NULL) return NULL;

    struct xccdf_refine_value_iterator  * r_value_it;
    struct xccdf_refine_value           * r_value;

    r_value_it = xccdf_profile_get_refine_values(xccdf_policy_get_profile(policy));
    while (xccdf_refine_value_iterator_has_more(r_value_it)) {
        r_value = xccdf_refine_value_iterator_next(r_value_it);
        if (!strcmp(id, xccdf_refine_value_get_item(r_value))) {
            xccdf_refine_value_iterator_free(r_value_it);
            return r_value;
        }
    }
    if (r_value_it != NULL) xccdf_refine_value_iterator_free(r_value_it);
    return NULL;
}

/**
 * Resolve the xccdf item 
 */
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
                        oscap_list_add(policy->rules, sel);
                } /* else it has a rule already and a policy 
                     priority inherited from the profile is higher */
            } break;
            case XCCDF_GROUP:{
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

/**
 * Evaluate the policy check with given checking system
 */
static bool xccdf_policy_evaluate_cb(struct xccdf_policy * policy, const char * sysname, const char * content, const char * rule_id, struct oscap_list * bindings) 
{
    bool retval = false;

    callback * cb = xccdf_policy_get_callback(policy, sysname);
    if (cb == NULL) { /* No callback found - checking system not registered */
        oscap_seterr(OSCAP_EFAMILY_XCCDF, XCCDF_EUNKNOWNCB, 
                "Unknown callback for given checking system. Set callback first");
        return false;
    }

    struct xccdf_value_binding_iterator * binding_it = (struct xccdf_value_binding_iterator *) oscap_iterator_new(bindings);

    /* Each callback has format: "bool callback(struct xccdf_policy_model * model, const char * href, const char * id, const char *id)" */
    /* Don't proccess results here, tool should set them to Policy Model ad hoc */
    retval = cb->callback(xccdf_policy_get_model(policy), rule_id, content, binding_it, cb->usr);

    /* Clear */
    if (binding_it != NULL) xccdf_value_binding_iterator_free(binding_it);

    return retval;
}

static struct oscap_list * xccdf_policy_check_get_value_bindings(struct xccdf_policy * policy, struct xccdf_check_export_iterator * check_it)
{
        __attribute__nonnull__(check_it);

        struct xccdf_check_export   * check;
        struct xccdf_value          * value;
        struct xccdf_benchmark      * benchmark;
        struct xccdf_value_binding  * binding;
        struct xccdf_profile        * profile;
        struct xccdf_refine_value   * r_value;
        struct xccdf_setvalue       * s_value;
        struct oscap_list           * list = oscap_list_new();

        profile = xccdf_policy_get_profile(policy);
        benchmark = xccdf_profile_get_benchmark(profile);

        while (xccdf_check_export_iterator_has_more(check_it)) {
            check = xccdf_check_export_iterator_next(check_it);

            binding = xccdf_value_binding_new();
            value = (struct xccdf_value *) xccdf_benchmark_get_item(benchmark, xccdf_check_export_get_value(check));

            /* Apply related setvalue from policy profile */
            s_value = xccdf_policy_get_setvalue(policy, xccdf_value_get_id(value));
            if (s_value != NULL) binding->setvalue = (char *) xccdf_setvalue_get_value(s_value);

            /* Apply related refine value from policy profile */
            const char * selector = NULL;
            r_value = xccdf_policy_get_refine_value(policy, xccdf_value_get_id(value));
            if (r_value != NULL) {
                selector = xccdf_refine_value_get_selector(r_value);
                /* This refine value changes the value content */
                if (xccdf_refine_value_get_oper(r_value) != NAN) {
                    binding->operator = xccdf_refine_value_get_oper(r_value);
                } else binding->operator = xccdf_value_get_oper(value);

            } else binding->operator = xccdf_value_get_oper(value);
            
            binding->value = xccdf_value_instance_get_value(xccdf_value_get_instance_by_selector(value, selector));
            binding->name = (char *) xccdf_check_export_get_name(check);
            binding->type = xccdf_value_get_type(value);
            oscap_list_add(list, binding);
        }
        xccdf_check_export_iterator_free(check_it);

        return list;

}

/** 
 * Evaluate the XCCDF check. 
 * Name collision with xccdf_check -> changed to xccdf_policy_check 
 */
static bool xccdf_policy_check_evaluate(struct xccdf_policy * policy, struct xccdf_check * check, char * rule_id)
{
    struct xccdf_check_iterator             * child_it;
    struct xccdf_check                      * child;
    struct xccdf_check_content_ref_iterator * content_it;
    struct xccdf_check_content_ref          * content;
    bool                                      ret       = false;
    const char                              * content_name;
    const char                              * system_name;
    struct oscap_list                       * bindings;

    /* At least one of check-content or check-content-ref must
        * appear in each check element. */
    if (xccdf_check_get_complex(check)) { /* we have complex subtree */
            child_it = xccdf_check_get_children(check);
            while (xccdf_check_iterator_has_more(child_it)) {
                child = xccdf_check_iterator_next(child_it);
                ret = xccdf_policy_check_evaluate(policy, child, rule_id);
                if (ret == false) break;
            }
            xccdf_check_iterator_free(child_it);
    } else { /* This is <check> element */
            /* It depends on what operation we process - we do only Compliance Check */
            content_it = xccdf_check_get_content_refs(check);
            system_name = xccdf_check_get_system(check);
            bindings = xccdf_policy_check_get_value_bindings(policy, xccdf_check_get_exports(check));
            while (xccdf_check_content_ref_iterator_has_more(content_it)) {
                content = xccdf_check_content_ref_iterator_next(content_it);
                content_name = xccdf_check_content_ref_get_name(content);
                /* Check if this is OVAL ? Never mind. Added to TODO */
                ret = xccdf_policy_evaluate_cb(policy, system_name, content_name, rule_id, bindings);
            }
            xccdf_check_content_ref_iterator_free(content_it);
            oscap_list_free(bindings, (oscap_destruct_func) xccdf_value_binding_free);
    }
    return ret;
}


/** 
 * Evaluate the XCCDF item. If it is group, start recursive cycle, otherwise get XCCDF check
 * and evaluate it.
 * Name collision with xccdf_item -> changed to xccdf_policy_item 
 */
static bool xccdf_policy_item_evaluate(struct xccdf_policy * policy, struct xccdf_item * item)
{
    struct xccdf_check_iterator     * check_it;
    struct xccdf_check              * check;
    struct xccdf_item_iterator      * child_it;
    struct xccdf_item               * child;
    bool                              ret       = false;
    const char                      * rule_id;

    xccdf_type_t itype = xccdf_item_get_type(item);

    switch (itype) {
        case XCCDF_RULE:{
                    /* Get all checks of rule */
                    check_it = xccdf_rule_get_checks((struct xccdf_rule *)item);
                    rule_id = xccdf_rule_get_id((struct xccdf_rule *)item);
                    /* we need to evaluate all checks in rule, iteration begin */
                    while(xccdf_check_iterator_has_more(check_it)) {
                            check = xccdf_check_iterator_next(check_it);

                            ret = xccdf_policy_check_evaluate(policy, check, (char *) rule_id);

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
                            ret = xccdf_policy_item_evaluate(policy, child);

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
/* Public functions.
 */

/**
 * Get ID of XCCDF Profile that belongs to XCCDF Policy
 */
const char * xccdf_policy_get_id(struct xccdf_policy * policy)
{
    return xccdf_profile_get_id(xccdf_policy_get_profile(policy));
}

/**
 * Funtion to register callback for particular checking system. System is used for evaluating content
 * of rules.
 */
bool xccdf_policy_model_register_callback(struct xccdf_policy_model * model, char * sys, void * func, void * usr) {

        __attribute__nonnull__(model);
        callback * cb = oscap_alloc(sizeof(callback));
        if (cb == NULL) return false;

        cb->system   = sys;
        cb->callback = func;
        cb->usr      = usr;

        return oscap_list_add(model->callbacks, cb);
}

struct xccdf_result * xccdf_policy_model_get_result_by_id(struct xccdf_policy_model * model, const char * id) {

    struct xccdf_result_iterator    * result_it;
    struct xccdf_result             * result;

    result_it = xccdf_policy_model_get_results(model);
    while (xccdf_result_iterator_has_more(result_it)){
        result = xccdf_result_iterator_next(result_it);
        if (!strcmp(xccdf_result_get_id(result), id) ){
            xccdf_result_iterator_free(result_it);
            return result;
        }
    }
    return NULL;
}

/***************************************************************************/
/* Constructors of XCCDF POLICY structures xccdf_policy_*<structure>*_new()
 * More info in representive header file.
 * returns the type of <structure>
 */

/**
 * New XCCDF Policy model. Create new structure and fill the policies list with 
 * policy entries that are inherited from XCCDF benchmark Profile elements. For each 
 * profile element call xccdf_policy_new function.
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
        model->callbacks = oscap_list_new();
        model->results   = oscap_list_new();

        /* Create policies from benchmark model */
        profile_it = xccdf_benchmark_get_profiles(benchmark);
        /* Iterate through profiles and create policies */
        while (xccdf_profile_iterator_has_more(profile_it)) {

            profile = xccdf_profile_iterator_next(profile_it);
            policy = xccdf_policy_new(model, profile);

            /* Should we set the error code and return NULL here ? */
            if (policy != NULL) oscap_list_add(model->policies, policy);
        }
        xccdf_profile_iterator_free(profile_it);

	return model;
}

/**
 * Constructor for structure XCCDF Policy. Create the structure and resolve all rules
 * from benchmark that are not present in selectors. This step is necessary because of 
 * default values of groups / rules that can be added to Policy whether or not they have
 * their selector. Last step is filling the list of value bindings (see more in 
 * xccdf_value_binding_proccess).
 */
struct xccdf_policy * xccdf_policy_new(struct xccdf_policy_model * model, struct xccdf_profile * profile) {

	__attribute__nonnull__(profile);

	struct xccdf_policy             * policy;
        struct xccdf_select_iterator    * sel_it;
        struct xccdf_select             * sel;
        struct xccdf_benchmark          * benchmark;
        struct xccdf_item_iterator      * item_it;
        struct xccdf_item               * item;

	policy = oscap_alloc(sizeof(struct xccdf_policy));
	if (policy == NULL)
		return NULL;
	memset(policy, 0, sizeof(struct xccdf_policy));

	policy->profile = profile;
	policy->rules   = oscap_list_new();
	policy->values  = oscap_list_new();

        policy->model = model;

        /* Create selects from benchmark model */
        sel_it = xccdf_profile_get_selects(profile);
        /* Iterate through selects in profile */
        while (xccdf_select_iterator_has_more(sel_it)) {

            sel = xccdf_select_iterator_next(sel_it);
            /* Should we set the error code and return NULL here ? */
            if (sel != NULL) oscap_list_add(policy->rules, xccdf_select_clone(sel));
        }
        xccdf_select_iterator_free(sel_it);

        /* Iterate through items in benchmark and resolve rules */
        benchmark = xccdf_profile_get_benchmark(profile);
        item_it = xccdf_benchmark_get_content(benchmark);
        while (xccdf_item_iterator_has_more(item_it)) {
            
            item = xccdf_item_iterator_next(item_it);
            xccdf_policy_resolve_rule(policy, item);
        }
        xccdf_item_iterator_free(item_it);

	return policy;
}

/**
 * Constructor for structure XCCDF Policy Value bindings
 */
struct xccdf_value_binding * xccdf_value_binding_new() {

        struct xccdf_value_binding          * binding;

        /* Initialization */
	binding = oscap_alloc(sizeof(struct xccdf_value_binding));
	if (binding == NULL)
		return NULL;
	memset(binding, 0, sizeof(struct xccdf_value_binding));

        binding->name       = NULL;
        binding->type       = 0;
        binding->value      = NULL;
        binding->setvalue   = NULL;
        binding->operator   = XCCDF_OPERATOR_EQUALS;

        return binding;
}

/***************************************************************************/


struct xccdf_select_iterator * xccdf_policy_get_selected_rules(struct xccdf_policy * policy) {

    return (struct xccdf_select_iterator *) oscap_iterator_new_filter( policy->rules, 
                                                                       (oscap_filter_func) xccdf_policy_filter_selected, 
                                                                       NULL);
}

/**
 * Make the rule from benchmark selected in Policy
 */
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

/**
 * Get Policy from Policy model by it's id.
 */
struct xccdf_policy * xccdf_policy_model_get_policy_by_id(struct xccdf_policy_model * policy_model, const char * id)
{
    struct xccdf_policy_iterator * policy_it;
    struct xccdf_policy          * policy;

    policy_it = xccdf_policy_model_get_policies(policy_model);
    while (xccdf_policy_iterator_has_more(policy_it)) {
        policy = xccdf_policy_iterator_next(policy_it);
        if (!strcmp(xccdf_policy_get_id(policy), id)) {
            xccdf_policy_iterator_free(policy_it);
            return policy;
        }
    }
    xccdf_policy_iterator_free(policy_it);

    return NULL;
}

/**
 * Resolve benchmark - apply all refine_rules to the benchmark items.
 * Beware ! Benchmark properties will be changed ! For discarding changes you have to load
 * benchmark from XML file again.
 */
bool xccdf_policy_resolve(struct xccdf_policy * policy)
{

    struct xccdf_refine_rule_iterator   * r_rule_it;
    struct xccdf_refine_rule            * r_rule;
    struct xccdf_item                   * item;

    struct xccdf_policy_model           * policy_model  = xccdf_policy_get_model(policy);
    struct xccdf_benchmark              * benchmark     = xccdf_policy_model_get_benchmark(policy_model);

    /* Proccess refine rules; Changing Rules and Groups */
    r_rule_it = xccdf_profile_get_refine_rules(xccdf_policy_get_profile(policy));
    while (xccdf_refine_rule_iterator_has_more(r_rule_it)) {
        r_rule = xccdf_refine_rule_iterator_next(r_rule_it);
        item = xccdf_benchmark_get_item(benchmark, xccdf_refine_rule_get_item(r_rule));
        if (item != NULL) {
            /* Proccess refine rule appliement */
            /* In r_rule we have refine rule that match  - no more then one !*/
            if (xccdf_item_get_type(item) == XCCDF_VALUE) { 
                /* Perform check of weight attribute  - ignore other attributes */
                if (xccdf_refine_rule_get_weight(r_rule) == NAN) {
                        oscap_seterr(OSCAP_EFAMILY_XCCDF, XCCDF_EREFGROUPATTR, 
                                "'Weight' attribute not specified, only 'weight' attribute applies to groups items");
                        return false;            
                }
                else {
                    /* Apply the rule changes */
                    xccdf_group_set_weight((struct xccdf_group *) item, xccdf_refine_rule_get_weight(r_rule) );
                }
                
            } else if (xccdf_item_get_type(item) == XCCDF_RULE) {
                /* Perform all changes in rule */
                if (xccdf_refine_rule_get_role(r_rule) != NAN)
                    xccdf_rule_set_role((struct xccdf_rule *) item, xccdf_refine_rule_get_role(r_rule));
                //if (xccdf_refine_rule_get_selector(r_rule) != NULL)
                    //TODO: xccdf_rule_set_selector((struct xccdf_rule *) item, xccdf_refine_rule_get_selector(r_rule));
                if (xccdf_refine_rule_get_severity(r_rule) != NAN)
                    xccdf_rule_set_severity((struct xccdf_rule *) item, xccdf_refine_rule_get_severity(r_rule));

            } else {}/* TODO oscap_err ? */;

        } else {
            oscap_seterr(OSCAP_EFAMILY_XCCDF, XCCDF_EREFIDCONFLICT, 
                    "Refine rule item points to nonexisting XCCDF item");
            xccdf_refine_rule_iterator_free(r_rule_it);
            return false;            
        }
    }
    xccdf_refine_rule_iterator_free(r_rule_it);

    return true;
}

/**
 * Evaluate XCCDF Policy
 * Iterate through Benchmark items and evalute one by one by calling 
 * callback for checking system that is defined by particular rules
 * Callbacks for checking systems have to be defined before calling this function, otherwise 
 * rules would not be evaluated and process ends with error.
 */
bool xccdf_policy_evaluate(struct xccdf_policy * policy) 
{

    struct xccdf_select_iterator    * sel_it;
    struct xccdf_select             * sel;
    struct xccdf_item               * item;
    struct xccdf_benchmark          * benchmark;
    bool                              ret       = false;

    __attribute__nonnull__(policy);

    /* Get all constant information */
    benchmark = xccdf_profile_get_benchmark(xccdf_policy_get_profile(policy));

    sel_it = xccdf_policy_get_rules(policy);
    while (xccdf_select_iterator_has_more(sel_it)) {
        sel = xccdf_select_iterator_next(sel_it);
        if (!xccdf_select_get_selected(sel)) continue;

        /* Get the refid string and find xccdf_item in benchmark */
        /* TODO: we need to check if every requirement is met - some of required Item has to be sleected too */

        item = xccdf_benchmark_get_item(benchmark, xccdf_select_get_item(sel));
        if (item == NULL) {
            oscap_seterr(OSCAP_EFAMILY_XCCDF, XCCDF_EBADID, "Check-exports collection: ID of selector does not exist in Benchmark !");
            continue; /* TODO: Should we just skip that selector ? XCCDF is not valid here !! */
        }

        if (xccdf_item_get_type(item) == XCCDF_GROUP) continue;
        ret = xccdf_policy_item_evaluate(policy, item);
    }
    xccdf_select_iterator_free(sel_it);

    return ret;
}

void xccdf_policy_export_variables(struct xccdf_policy *policy, char *export_namespace, const char *file) {
}

void xccdf_policy_export_controls (struct xccdf_policy *policy, char *export_namespace, const char *file) {
}

void xccdf_policy_import_results(struct xccdf_policy *policy, char *import_namespace, const char *file) {
}

void xccdf_policy_export_results(struct xccdf_policy *policy, char *scoring_model_namespace, const char *file) {
}


void xccdf_policy_model_free(struct xccdf_policy_model * model) {

        xccdf_benchmark_free(model->benchmark);
	oscap_list_free(model->policies, (oscap_destruct_func) xccdf_policy_free);
	oscap_list_free(model->results, (oscap_destruct_func) xccdf_result_free);
	oscap_list_free(model->callbacks, (oscap_destruct_func) oscap_free);
        oscap_free(model);
}

void xccdf_policy_free(struct xccdf_policy * policy) {

	xccdf_item_free((struct xccdf_item *)policy->profile);
	oscap_list_free(policy->rules, (oscap_destruct_func) xccdf_select_free);
	oscap_list_free(policy->values, (oscap_destruct_func) xccdf_value_binding_free);
        oscap_free(policy);
}

void xccdf_value_binding_free(struct xccdf_value_binding * binding) {

        oscap_free(binding->name);
        oscap_free(binding->value);
        oscap_free(binding);
}
