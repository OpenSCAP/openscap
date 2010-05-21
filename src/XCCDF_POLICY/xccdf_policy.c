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
#include "../OVAL/public/oval_variables.h"

#include "../common/list.h"
#include "../_error.h"
#include "../common/public/text.h"

/**
 * Typedef of callback structure with system identificator, callback function and usr data (optional) 
 * On evaluation action will be selected checking system and appropriate callback registred by tool
 * for that system.
 */
typedef struct callback_t {

    char * system;                                                                      ///< Identificator of checking engine
    bool (*callback) (struct xccdf_policy_model *, const char *, const char *, void *); ///< format of callback function 
    void * usr;                                                                         ///< User data structure

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

        struct oscap_list           * check_exports;    ///< Check export elements of benchmark
        struct xccdf_refine_value   * refine_value;     ///< Refine value element of Profile
        struct xccdf_setvalue       * set_value;        ///< Set value element of Profile
        struct xccdf_value          * rule_value;       ///< Value element of benchmark (TODO:should be here more then once ?)
};
/* Macros to generate iterators, getters and setters */
OSCAP_IGETINS(xccdf_check_export, xccdf_value_binding, check_exports, check_export)
OSCAP_GETTER(struct xccdf_refine_value *, xccdf_value_binding, refine_value)
OSCAP_GETTER(struct xccdf_setvalue *, xccdf_value_binding, set_value)
OSCAP_GETTER(struct xccdf_value *, xccdf_value_binding, rule_value)

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
            if (!strcmp(cb->system, sysname)) {
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

/**
 * Return true if given OSCAP list of check exports has the item that points to the variable
 * with item_id name, false otherwise
 */
static bool xccdf_check_export_list_has_item(struct oscap_list * list, const char *item_id)
{
        if (list == NULL) return false;

        struct xccdf_check_export_iterator  * check_it;
        struct xccdf_check_export           * check;

        check_it = (struct xccdf_check_export_iterator *) oscap_iterator_new(list);
        while (xccdf_check_export_iterator_has_more(check_it)) {
            check = xccdf_check_export_iterator_next(check_it);
            if (!strcmp( item_id, xccdf_check_export_get_name(check) )) {
                xccdf_check_export_iterator_free(check_it);
                return true;
            }
        }
        xccdf_check_export_iterator_free(check_it);
        return false;
}

/**
 * Return true if given OSCAP list of values has the item that points to the variable
 * with item_id name, false otherwise
 */
static bool xccdf_value_list_has_item(struct oscap_list * list, const char *item_id)
{
        if (list == NULL) return false;

        struct xccdf_value_iterator * value_it;
        struct xccdf_value          * value;

        value_it = (struct xccdf_value_iterator *) oscap_iterator_new(list);
        while (xccdf_value_iterator_has_more(value_it)) {
            value = xccdf_value_iterator_next(value_it);
            if (!strcmp( item_id, xccdf_value_get_id(value) )) {
                xccdf_value_iterator_free(value_it);
                return true;
            }
        }
        xccdf_value_iterator_free(value_it);
        return false;
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
                        xccdf_policy_add_rule(policy, sel);
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
 * Static (private) function for collecting exports - this function is called from 
 * xccdf_policy_model_collect_check_exports function to allow recurse. 
 */
static void __xccdf_policy_model_collect_check_exports(struct xccdf_item * item, struct oscap_list * collection)
{
    struct xccdf_item_iterator          * child_it          = NULL;
    struct xccdf_item                   * child             = NULL;
    struct xccdf_check_iterator         * check_it          = NULL;
    struct xccdf_check                  * check             = NULL;
    struct xccdf_check_export_iterator  * check_export_it   = NULL;


    if (xccdf_item_get_type(item) == XCCDF_GROUP) {

        child_it = xccdf_item_get_content(item);
        if (child_it == NULL) return; /* TODO: this should be in oscap_iterator_has_more */
        while(xccdf_item_iterator_has_more(child_it)) {
            child = xccdf_item_iterator_next(child_it);
            __xccdf_policy_model_collect_check_exports(child, collection);
        }
        xccdf_item_iterator_free(child_it);
    }
    else if (xccdf_item_get_type(item) == XCCDF_RULE) {
        check_it = xccdf_rule_get_checks((const struct xccdf_rule *) item);
        /* we need to go throught all checks in rule, iteration begin */
        while(xccdf_check_iterator_has_more(check_it)) {
                check = xccdf_check_iterator_next(check_it);
                /* Go throught check-exports and add them to collection */
                check_export_it = xccdf_check_get_exports(check);
                while (xccdf_check_export_iterator_has_more(check_export_it)) {
                    oscap_list_add(collection, xccdf_check_export_iterator_next(check_export_it));
                }
                xccdf_check_export_iterator_free(check_export_it);
        }
        xccdf_check_iterator_free(check_it);
    }

    return;

}

/**
 * Collect all check exports from benchmark and return them in OSCAP list structure
 */
static struct oscap_list * xccdf_policy_model_collect_check_exports(struct xccdf_benchmark * benchmark, struct xccdf_policy * policy)
{
    struct xccdf_item                   * item;
    struct xccdf_select_iterator        * sel_it;
    struct xccdf_select                 * sel;
    struct oscap_list                   * collection = oscap_list_new();

    //child_it = xccdf_benchmark_get_content(benchmark);
    sel_it = xccdf_policy_get_rules(policy);
    while(xccdf_select_iterator_has_more(sel_it)) {
            sel = xccdf_select_iterator_next(sel_it);
            /* If the rule/group is not selected, just do not pass variables */
            if (!xccdf_select_get_selected(sel)) continue;
            item = xccdf_benchmark_get_item(benchmark, xccdf_select_get_item(sel));
            if (item == NULL) {
                oscap_seterr(OSCAP_EFAMILY_XCCDF, XCCDF_EBADID, "Check-exports collection: ID of selector does not exist in Benchmark !");
                continue; /* TODO: Should we just skip that selector ? XCCDF is not valid here !! */
            }
            __xccdf_policy_model_collect_check_exports(item, collection);
    }
    xccdf_select_iterator_free(sel_it);

    return collection;
}


/**
 * Proccess value bindings - fill the list of structures that contains refine values, set values 
 * check exports and value elements from XCCDF benchmark
 */
static void xccdf_value_binding_proccess(struct xccdf_policy * policy, struct xccdf_benchmark * benchmark)
{
        struct xccdf_refine_value_iterator  * refine_value_it;
        struct xccdf_refine_value           * refine_value;
        struct xccdf_setvalue_iterator      * setvalue_it;
        struct xccdf_setvalue               * setvalue;
        struct xccdf_value_binding          * binding;
        struct xccdf_profile                * profile;
        struct oscap_list                   * check_exports;
        struct xccdf_check_export_iterator  * check_it;
        struct xccdf_check_export           * check;
        struct xccdf_value_iterator         * value_it;
        struct xccdf_value                  * value;

        profile = xccdf_policy_get_profile(policy);

        /* Get all check export elements from benchmark - just once */
        check_exports = xccdf_policy_model_collect_check_exports(benchmark, policy);

        /* Get all variables that will be passed, cause they are related to selected rules in Policy */
        //value_it = xccdf_benchmark_get_values(benchmark); // TODO: this does not work !
        /* ---- workaround start */
        struct oscap_list * values = oscap_list_new();
        check_it = (struct xccdf_check_export_iterator *) oscap_iterator_new(check_exports);
        while (xccdf_check_export_iterator_has_more(check_it)) {
            check = xccdf_check_export_iterator_next(check_it);
            value = (struct xccdf_value *) xccdf_benchmark_get_item(benchmark, xccdf_check_export_get_value(check));
            if (!xccdf_value_list_has_item(values, xccdf_value_get_id(value))) {
                oscap_list_add(values, value);
            }
        }
        xccdf_check_export_iterator_free(check_it);

        value_it = (struct xccdf_value_iterator *) oscap_iterator_new(values);
        /* ---- workaround ends */

        while (xccdf_value_iterator_has_more(value_it)) {

            value = xccdf_value_iterator_next(value_it);
            binding = xccdf_value_binding_new();

            // Go throught check_exports and find out if this variable we use in Policy 
            check_it = (struct xccdf_check_export_iterator *) oscap_iterator_new(check_exports);
            while (xccdf_check_export_iterator_has_more(check_it)) {
                check = xccdf_check_export_iterator_next(check_it);
                if (!strcmp( xccdf_check_export_get_value(check), xccdf_value_get_id(value) )) {
                    // Check if we have in the list same check export which points to the same check engine variable 
                    if (!xccdf_check_export_list_has_item(binding->check_exports, xccdf_check_export_get_name(check))) {
                        oscap_list_add(binding->check_exports, check);
                    }
                }
            }
            xccdf_check_export_iterator_free(check_it);

            /*if (oscap_list_get_itemcount(binding->check_exports) == 0) {
                xccdf_value_binding_free(binding);
                continue;
            }*/

            /* Get value from binding */
            binding->rule_value = value;

            /* Get setvalue for value binding */
            setvalue_it = xccdf_profile_get_setvalues(profile);
            while (xccdf_setvalue_iterator_has_more(setvalue_it)) {

                setvalue = xccdf_setvalue_iterator_next(setvalue_it);
                if ((setvalue != NULL) && 
                   (!strcmp( xccdf_setvalue_get_item(setvalue), xccdf_value_get_id(value) ))) {
                        binding->set_value = setvalue; /* TODO: clone */
                }
            }
            xccdf_setvalue_iterator_free(setvalue_it);


            refine_value_it = xccdf_profile_get_refine_values(profile);
            while (xccdf_refine_value_iterator_has_more(refine_value_it)) {
                refine_value = xccdf_refine_value_iterator_next(refine_value_it); /* TODO: clone */
                if (!strcmp( xccdf_refine_value_get_item(refine_value), xccdf_value_get_id(value) )) {
                    binding->refine_value = refine_value;
                }
            }
            xccdf_refine_value_iterator_free(refine_value_it);

            /* Add binding to value bindings */
            xccdf_policy_add_value(policy, binding);
        }

        oscap_free(check_exports);

}

/**
 * Evaluate the policy check with given checking system
 */
static bool xccdf_policy_evaluate_cb(struct xccdf_policy * policy, const char * sysname, const char * href, const char * id) 
{
    callback * cb = xccdf_policy_get_callback(policy, sysname);
    if (cb == NULL) { /* No callback found - checking system not registered */
        oscap_seterr(OSCAP_EFAMILY_XCCDF, XCCDF_EUNKNOWNCB, 
                "Unknown callback for given checking system. Set callback first");
        return false;
    }

    /* Each callback has format: "bool callback(struct xccdf_policy_model * model, const char * href, const char *id)" */
    /* Don't proccess results here, tool should set them to Policy Model ad hoc */
    return cb->callback(xccdf_policy_get_model(policy), href, id, cb->usr);
}

/** 
 * Evaluate the XCCDF check. 
 * Name collision with xccdf_check -> changed to xccdf_policy_check 
 */
static bool xccdf_policy_check_evaluate(struct xccdf_policy * policy, struct xccdf_check * check)
{
    struct xccdf_check_iterator             * child_it;
    struct xccdf_check                      * child;
    struct xccdf_check_content_ref_iterator * content_it;
    struct xccdf_check_content_ref          * content;
    bool                                      ret       = false;
    const char                              * content_name;
    const char                              * system_name;
    const char                              * href;

    /* At least one of check-content or check-content-ref must
        * appear in each check element. */
    if (xccdf_check_get_complex(check)) { /* we have complex subtree */
            child_it = xccdf_check_get_children(check);
            while (xccdf_check_iterator_has_more(child_it)) {
                child = xccdf_check_iterator_next(child_it);
                ret = xccdf_policy_check_evaluate(policy, child);
                if (ret == false) break;
            }
            xccdf_check_iterator_free(child_it);
    } else { /* This is <check> element */
            /* It depends on what operation we process - we do only Compliance Check */
            content_it = xccdf_check_get_content_refs(check);
            system_name = xccdf_check_get_system(check);
            while (xccdf_check_content_ref_iterator_has_more(content_it)) {
                content = xccdf_check_content_ref_iterator_next(content_it);
                content_name = xccdf_check_content_ref_get_name(content);
                href = xccdf_check_content_ref_get_href(content);
                /* Check if this is OVAL ? Never mind. Added to TODO */
                ret = xccdf_policy_evaluate_cb(policy, system_name, href, content_name);
            }
            xccdf_check_content_ref_iterator_free(content_it);
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

    xccdf_type_t itype = xccdf_item_get_type(item);

    switch (itype) {
        case XCCDF_RULE:{
                    /* Get all checks of rule */
                    check_it = xccdf_rule_get_checks((const struct xccdf_rule *)item);
                    /* we need to evaluate all checks in rule, iteration begin */
                    while(xccdf_check_iterator_has_more(check_it)) {
                            check = xccdf_check_iterator_next(check_it);
                            ret = xccdf_policy_check_evaluate(policy, check);

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

/**
 * Tailoring function for rule preprocessing, use this function on cloned item before processing item 
 * with value exporting or result exporting.
 * returns false if no refine rule match item id
 * returns true if tailoring procces for this rule is complete
 */
static bool xccdf_policy_tailor_item(struct xccdf_item * item, struct xccdf_value_binding * values) {

    /* Proccess set values */
    /* set-value element override Value object */

    struct xccdf_refine_rule_iterator   * r_rule_it;
    struct xccdf_refine_rule            * r_rule    = NULL;

    bool found = false;

    //TODO:r_rule_it = xccdf_value_binding_get_refine_rules(values);
    while (xccdf_refine_rule_iterator_has_more(r_rule_it)) {
        r_rule = xccdf_refine_rule_iterator_next(r_rule_it);
        if (!strcmp(xccdf_item_get_id(item), xccdf_refine_rule_get_item(r_rule))) {
            if (found == true) {
                /* Only one refine rule can match the rule */
                oscap_seterr(OSCAP_EFAMILY_XCCDF, XCCDF_EREFIDCONFLICT, 
                        "The 'idref' attribute values of sibling refine-rule element children of a Profile must be different");
                xccdf_refine_rule_iterator_free(r_rule_it);
                return false;            
            }
            found = true;
        }
    }
    xccdf_refine_rule_iterator_free(r_rule_it);

    /* No refine rule match this item, reutrn false */
    if (found == false) return false;

    /* In r_rule we have refine rule that match  - no more then one !*/
    if (xccdf_item_get_type(item) == XCCDF_GROUP) { 
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
        if (xccdf_refine_rule_get_selector(r_rule) != NULL)
            //xccdf_rule_set_selector((struct xccdf_rule *) item, xccdf_refine_rule_get_selector(r_rule));
            {};
        if (xccdf_refine_rule_get_severity(r_rule) != NAN)
            xccdf_rule_set_severity((struct xccdf_rule *) item, xccdf_refine_rule_get_severity(r_rule));

   } else {}/* TODO oscap_err ? */;

    return true;
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

struct oval_variable_model * xccdf_policy_get_variables(struct xccdf_policy * policy, struct oval_definition_model * def_model) {

    struct xccdf_value_binding_iterator     * binding_it;
    struct xccdf_value_binding              * binding;
    struct xccdf_value                      * value;
    struct oscap_text_iterator              * text_it;
    struct oscap_text                       * text;
    const char                              * com;
    struct xccdf_check_export_iterator      * check_it;
    struct xccdf_check_export               * check;
    struct oval_variable                    * o_variable;
    struct oval_variable_model              * var_model;
    oval_datatype_t                           o_type;

    /* Create and fill the OVAL variable model */
    var_model = oval_variable_model_new();

    binding_it = xccdf_policy_get_values(policy);
    while (xccdf_value_binding_iterator_has_more(binding_it)) {
            binding = xccdf_value_binding_iterator_next(binding_it);
            value = xccdf_value_binding_get_rule_value(binding);

            /* Assume some comment of variable - required in OVAL v>5.5 TODO: improve this (what language and so) */
            text_it = xccdf_item_get_description((const struct xccdf_item *)value);
            if (oscap_text_iterator_has_more(text_it)) {
                    text = oscap_text_iterator_next(text_it);
                    com = oscap_text_get_text(text);
            } else {
                    oscap_text_iterator_free(text_it);
                    text_it = xccdf_item_get_title((const struct xccdf_item *)value);
                    if (oscap_text_iterator_has_more(text_it)) {
                            text = oscap_text_iterator_next(text_it);
                            com = oscap_text_get_text(text);
                    } else {
                            com = "Unknown";
                    }
            }
            oscap_text_iterator_free(text_it);

            /* Get all check export OVAL variables and pass it to model */
            check_it = xccdf_value_binding_get_check_exports(binding);
            while (xccdf_check_export_iterator_has_more(check_it)) {
                    check = xccdf_check_export_iterator_next(check_it);
                    /* Get the type of variable in OVAL context */
                    o_variable = oval_definition_model_get_variable(def_model, (char *)xccdf_check_export_get_name(check));
                    if (o_variable == NULL) {
                            /* TODO: What should I do when variable is missing on OVAL side ? */
                            continue;
                    }
                    o_type = oval_variable_get_datatype(o_variable);

                    oval_variable_model_add(var_model,
                                            (char *) xccdf_check_export_get_name(check),
                                            com, o_type, xccdf_value_get_selected_value(value));
            }
            xccdf_check_export_iterator_free(check_it);
    }
    xccdf_value_binding_iterator_free(binding_it);

    return var_model;
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

        /* Fill value bindings */
        xccdf_value_binding_proccess(policy, benchmark);

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

        binding->refine_value   = NULL;
        binding->set_value      = NULL;
        binding->rule_value     = NULL;
        binding->check_exports  = oscap_list_new();

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
 * Resolve benchmark - apply all refine_rules and all refine_values to the benchmark items.
 * Beware ! Benchmark properties will be changed ! For discarding changes you have to load
 * benchmark from XML file again.
 */
bool xccdf_policy_resolve(struct xccdf_policy * policy)
{

    struct xccdf_refine_rule_iterator   * r_rule_it;
    struct xccdf_refine_rule            * r_rule;
    struct xccdf_refine_value           * r_value;
    struct xccdf_value_binding          * binding;
    struct xccdf_value                  * value;
    struct xccdf_item                   * item;

    struct xccdf_policy_model           * policy_model  = xccdf_policy_get_model(policy);
    struct xccdf_benchmark              * benchmark     = xccdf_policy_model_get_benchmark(policy_model);
    struct xccdf_value_binding_iterator * binding_it    = xccdf_policy_get_values(policy);

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

    /* Proccess refine values; Changing Values */
    while (xccdf_value_binding_iterator_has_more(binding_it)) {
        binding = xccdf_value_binding_iterator_next(binding_it);
        value = xccdf_value_binding_get_rule_value(binding);
        r_value = xccdf_value_binding_get_refine_value(binding);

        /* Perform all changes in value */
        if (xccdf_refine_value_get_oper(r_value) != NAN) {
            xccdf_value_set_oper((struct xccdf_item *) value, xccdf_refine_value_get_oper(r_value));
        }
        if (xccdf_refine_value_get_selector(r_value) != NULL) {
            xccdf_value_get_set_selector((struct xccdf_item *) value, xccdf_refine_value_get_selector(r_value));
        }
    }
    xccdf_value_binding_iterator_free(binding_it);

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

    /* Step 1: From policy get all selected rules */
    struct xccdf_select_iterator    * sel_it;
    struct xccdf_select             * sel;
    struct xccdf_item               * item;
    struct xccdf_benchmark          * benchmark;
    bool                              ret       = false;

    __attribute__nonnull__(policy);

    /* Get all constant information */
    benchmark = xccdf_profile_get_benchmark(xccdf_policy_get_profile(policy));


    //sel_it = xccdf_policy_get_selected_rules(policy);
    sel_it = xccdf_policy_get_rules(policy);
    while (xccdf_select_iterator_has_more(sel_it)) {
        sel = xccdf_select_iterator_next(sel_it);

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

void xccdf_policy_export_variables(struct xccdf_policy *policy, char *export_namespace, struct oscap_export_target *target) {
}

void xccdf_policy_export_controls (struct xccdf_policy *policy, char *export_namespace, struct oscap_export_target *target) {
}

void xccdf_policy_import_results(struct xccdf_policy *policy, char *import_namespace, struct oscap_import_source *source) {
}

void xccdf_policy_export_results(struct xccdf_policy *policy, char *scoring_model_namespace, struct oscap_export_target *target) {
}


bool xccdf_value_binding_add_refine_rule(struct xccdf_value_binding *binding, struct xccdf_refine_rule *rule) {
	//TODO
}

bool xccdf_value_binding_add_refine_value(struct xccdf_value_binding *binding, struct xccdf_refine_value *value) {
	//TODO
}

bool xccdf_value_binding_add_setvalue(struct xccdf_value_binding *binding, struct xccdf_setvalue *value) {
	//TODO
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

        /* TODO: after implementing clone functions, they will have to be freed */
	//xccdf_refine_value_free(binding->refine_values);
	//xccdf_setvalue_free(binding->set_value);
	//xccdf_check_export_free(binding->check_export);
	//xccdf_vaule_free(binding->rule_value);
        oscap_free(binding);
}


