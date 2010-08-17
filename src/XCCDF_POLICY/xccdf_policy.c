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
#include <time.h> /* For timestamps in rule results and TestResult */

#include "public/xccdf_policy.h"
#include "../XCCDF/public/xccdf.h"

#include "../common/list.h"
#include "../_error.h"
#include "../common/public/text.h"
#include "../common/debug_priv.h"
#include "../common/reporter_priv.h"

/**
 * Typedef of callback structure with system identificator, callback function and usr data (optional) 
 * On evaluation action will be selected checking system and appropriate callback registred by tool
 * for that system.
 */
typedef struct callback_t {

    char * system;                              ///< Identificator of checking engine
    int (*callback) (struct xccdf_policy *,                     // Policy model
                      const char *,                             // Rule ID
                      const char *,                             // Definition ID
                      const char *,                             // HREF ID
                      struct xccdf_value_binding_iterator * it, // Value Bindings Iterator
                      void *);                  ///< format of callback function 
    void * usr;                                 ///< User data structure

} callback;

/**
 * Typedef of callback structure with callback function and usr data (optional)
 * After evaluation action will be called the callback with user data.
 */
typedef struct callback_out_t {

    char * system;                              ///< Identificator of checking engine (output engine)
    oscap_reporter callback;
    void * usr;                                 ///< User data structure

} callback_out;

/**
 * XCCDF policy model structure contains xccdf_benchmark as reference
 * to Benchmark element in XML file and list of policies that are
 * abstract structure of Profile element from benchmark file.
 */
struct xccdf_policy_model {

        struct xccdf_benchmark  * benchmark;    ///< Benchmark element (root element of XML file)
	struct oscap_list       * policies;     ///< List of xccdf_policy structures
        struct oscap_list       * callbacks;    ///< Callbacks for checking engines (see callback_t)
};
/* Macros to generate iterators, getters and setters */
OSCAP_GETTER(struct xccdf_benchmark *, xccdf_policy_model, benchmark)
OSCAP_IGETINS_GEN(xccdf_policy, xccdf_policy_model, policies, policy)

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
        struct oscap_list           * results;  ///< List of XCCDF results
};

/* Macros to generate iterators, getters and setters */
OSCAP_GETTER(struct xccdf_policy_model *, xccdf_policy, model)
OSCAP_GETTER(struct xccdf_profile *, xccdf_policy, profile)
OSCAP_IGETINS(xccdf_select, xccdf_policy, rules, rule)
OSCAP_IGETINS_GEN(xccdf_value_binding, xccdf_policy, values, value)
OSCAP_IGETINS(xccdf_result, xccdf_policy, results, result)
OSCAP_ITERATOR_RESET(xccdf_policy)

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
OSCAP_ITERATOR_RESET(xccdf_value_binding)

/**
 * XCCDF Default score structure represents Default XCCDF Score model
 * for each rule
 */
typedef struct xccdf_default_score {

        float score;
        float accumulator;
        float weight_score;
        int count;

} xccdf_default_score_t;

/**
 * XCCDF Flat score structure represents Flat XCCDF Score model
 * for each rule
 */
typedef struct xccdf_flat_score {

        float score;
        float weight;

} xccdf_flat_score_t;

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
 * Filter function returning true if given callback is for the given checking engine,
 * false otherwise
 */
static bool xccdf_policy_filter_callback(void *cb, void *sysname)
{
        if ((((callback *)cb)->system == NULL) && ( (const char *) sysname == NULL))
            return true;
        else if ((((callback *)cb)->system == NULL) || ((const char *) sysname == NULL))
            return false;
        else if (!strcmp(((callback *)cb)->system, (const char *) sysname))
            return true;
        else
            return false;
}

/**
 * Get callbacks that match sysname
 */
static struct oscap_iterator * xccdf_policy_get_callbacks_by_sysname(struct xccdf_policy * policy, const char * sysname) {

    return oscap_iterator_new_filter( policy->model->callbacks, (oscap_filter_func) xccdf_policy_filter_callback,
            (void *) sysname);
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
    struct xccdf_profile            * profile = xccdf_policy_get_profile(policy);;

    if (profile == NULL) return NULL;
    s_value_it = xccdf_profile_get_setvalues(profile);
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
    struct xccdf_profile            * profile = xccdf_policy_get_profile(policy);;

    if (profile == NULL) return NULL;
    r_value_it = xccdf_profile_get_refine_values(profile);
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

static xccdf_test_result_type_t _resolve_operation(int A, int B, xccdf_bool_operator_t oper)
{
    /* ***************************************************************
     * AND  P  F  U  E  N    OR  P  F  U  E  N         P  F  U  E  N *
     *   P  P  F  U  E  P     P  P  P  P  P  P    neg  F  P  U  E  N *
     *   F  F  F  F  F  F     F  P  F  U  E  F                       *
     *   U  U  F  U  U  U     U  P  U  U  U  U                       *
     *   E  E  F  U  E  E     E  P  E  U  E  E                       *
     *   N  P  F  U  E  N     N  P  F  U  E  N                       *
     *****************************************************************/

    xccdf_test_result_type_t value = 0;

    xccdf_test_result_type_t RESULT_TABLE_AND[9][9] = {
        /*  P  F  E  U  N  N  N  P */
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 2, 3, 4, 1, 1, 1, 1}, /* P */
        {0, 2, 2, 2, 2, 2, 2, 2, 2}, /* F */
        {0, 3, 2, 3, 4, 3, 3, 3, 3}, /* E */
        {0, 4, 2, 4, 4, 4, 4, 4, 4}, /* U */

        {0, 1, 2, 3, 4, 5, 5, 5, 1}, /* N */
        {0, 1, 2, 3, 4, 5, 5, 5, 1},
        {0, 1, 2, 3, 4, 5, 5, 5, 1},

        {0, 1, 2, 3, 4, 1, 1, 1, 1}  /* P */};

    xccdf_test_result_type_t RESULT_TABLE_OR[9][9] = {
        /*  P  F  E  U  N  N  N  P */
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1}, /* P */
        {0, 1, 2, 3, 4, 2, 2, 2, 1}, /* F */
        {0, 1, 3, 3, 4, 3, 3, 3, 1}, /* E */
        {0, 1, 4, 4, 4, 4, 4, 4, 1}, /* U */

        {0, 1, 2, 3, 4, 5, 5, 5, 1}, /* N */
        {0, 1, 2, 3, 4, 5, 5, 5, 1},
        {0, 1, 2, 3, 4, 5, 5, 5, 1},

        {0, 1, 1, 1, 1, 1, 1, 1, 1}  /* P */};

    if ((A == 0) || (B == 0)) {
	oscap_dlprintf(DBG_E, "Bad test results %d, %d.\n", A, B);
	return 0;
    }

    switch (oper) {
        case XCCDF_OPERATOR_AND: /* AND */
        case XCCDF_OPERATOR_NAND:
            value = (xccdf_test_result_type_t) RESULT_TABLE_AND[A][B];
            break;

        case XCCDF_OPERATOR_OR: /* OR */
        case XCCDF_OPERATOR_NOR:
            value = (xccdf_test_result_type_t) RESULT_TABLE_OR[A][B];
            break;
        case XCCDF_OPERATOR_NOT:
        case XCCDF_OPERATOR_MASK:
	    oscap_dlprintf(DBG_E, "Operation not supported.\n");
            return 0;
            break;
    }
    if ((oper == XCCDF_OPERATOR_NOR) || (oper == XCCDF_OPERATOR_NAND)) {
        if (value == XCCDF_RESULT_PASS) value = XCCDF_RESULT_FAIL;
        else if (value == XCCDF_RESULT_FAIL) value = XCCDF_RESULT_PASS;
    }

    return value;
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
static int
xccdf_policy_evaluate_cb(struct xccdf_policy * policy, const char * sysname, const char * content, const char * href,
        const char * rule_id, struct oscap_list * bindings) 
{
    xccdf_test_result_type_t retval = XCCDF_RESULT_NOT_CHECKED;
    struct oscap_iterator * cb_it = xccdf_policy_get_callbacks_by_sysname(policy, sysname);
    oscap_iterator_reset(cb_it);
    while (oscap_iterator_has_more(cb_it)) {
        callback * cb = (callback *) oscap_iterator_next(cb_it);
        if (cb == NULL) { /* No callback found - checking system not registered */
            oscap_seterr(OSCAP_EFAMILY_XCCDF, XCCDF_EUNKNOWNCB, 
                    "Unknown callback for given checking system. Set callback first");
            oscap_iterator_free(cb_it);
            return XCCDF_RESULT_NOT_CHECKED;
        }

        struct xccdf_value_binding_iterator * binding_it = (struct xccdf_value_binding_iterator *) oscap_iterator_new(bindings);

        retval = cb->callback(policy, rule_id, content, href, binding_it, cb->usr);

        if (binding_it != NULL) xccdf_value_binding_iterator_free(binding_it);
        if (retval != XCCDF_RESULT_NOT_CHECKED) break;
    }
    oscap_iterator_free(cb_it);

    return retval;
}

static struct oscap_list * xccdf_policy_check_get_value_bindings(struct xccdf_policy * policy, struct xccdf_check_export_iterator * check_it)
{
        __attribute__nonnull__(check_it);

        struct xccdf_check_export   * check;
        struct xccdf_value          * value;
        struct xccdf_benchmark      * benchmark;
        struct xccdf_value_binding  * binding;
        struct xccdf_policy_model   * model;
        struct xccdf_refine_value   * r_value;
        struct xccdf_setvalue       * s_value;
        struct oscap_list           * list = oscap_list_new();

        model = xccdf_policy_get_model(policy);
        benchmark = xccdf_policy_model_get_benchmark(model);

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
            
            const struct xccdf_value_instance * val = xccdf_value_get_instance_by_selector(value, selector);
            if (val == NULL) {
                oscap_seterr(OSCAP_EFAMILY_XCCDF, XCCDF_EVALUE, "Value instance does not exist\n");
                oscap_dlprintf(DBG_E, "Attempt to get non-existent selector \"%s\" from variable \"%s\"\n", selector, xccdf_value_get_id(value));
                return NULL;
            }
            binding->value = oscap_strdup(xccdf_value_instance_get_value(val));
            binding->name = oscap_strdup((char *) xccdf_check_export_get_name(check));
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
static int xccdf_policy_check_evaluate(struct xccdf_policy * policy, struct xccdf_check * check, char * rule_id)
{
    struct xccdf_check_iterator             * child_it;
    struct xccdf_check                      * child;
    struct xccdf_check_content_ref_iterator * content_it;
    struct xccdf_check_content_ref          * content;
    const char                              * content_name;
    const char                              * system_name;
    const char                              * href;
    struct oscap_list                       * bindings;
    int                                       ret = 0;
    int                                       ret2 = 0;

    /* At least one of check-content or check-content-ref must
        * appear in each check element. */
    if (xccdf_check_get_complex(check)) { /* we have complex subtree */
            child_it = xccdf_check_get_children(check);
            while (xccdf_check_iterator_has_more(child_it)) {
                child = xccdf_check_iterator_next(child_it);
                ret2 = xccdf_policy_check_evaluate(policy, child, rule_id);
                if (ret2 == -1) return -1;
                if (ret == 0) ret = ret2;
                else {
                    ret = (int) _resolve_operation((xccdf_test_result_type_t) ret, (xccdf_test_result_type_t) ret2, xccdf_check_get_oper(check));
                }
            }
            xccdf_check_iterator_free(child_it);
    } else { /* This is <check> element */
            /* It depends on what operation we process - we do only Compliance Check */
            content_it = xccdf_check_get_content_refs(check);
            system_name = xccdf_check_get_system(check);
            bindings = xccdf_policy_check_get_value_bindings(policy, xccdf_check_get_exports(check));
            if (bindings == NULL)
                return XCCDF_RESULT_UNKNOWN;
            while (xccdf_check_content_ref_iterator_has_more(content_it)) {
                content = xccdf_check_content_ref_iterator_next(content_it);
                content_name = xccdf_check_content_ref_get_name(content);
                href = xccdf_check_content_ref_get_href(content);
                /* Check if this is OVAL ? Never mind. Added to TODO */
                ret = xccdf_policy_evaluate_cb(policy, system_name, content_name, href, rule_id, bindings);
                if ((xccdf_test_result_type_t) ret != XCCDF_RESULT_NOT_CHECKED) break;
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
static int xccdf_policy_item_evaluate(struct xccdf_policy * policy, struct xccdf_item * item, struct xccdf_result * result)
{
    struct xccdf_check_iterator     * check_it;
    struct xccdf_check              * check;
    struct xccdf_item_iterator      * child_it;
    struct xccdf_item               * child;
    const char                      * rule_id;
    int                               ret = XCCDF_RESULT_UNKNOWN;

    xccdf_type_t itype = xccdf_item_get_type(item);

    switch (itype) {
        case XCCDF_RULE:{
                    /* Get all checks of rule */
                    check_it = xccdf_rule_get_checks((struct xccdf_rule *)item);
                    rule_id = xccdf_rule_get_id((struct xccdf_rule *)item);
                    /* we need to evaluate all checks in rule, iteration begin */
                    while(xccdf_check_iterator_has_more(check_it)) {
                            check = xccdf_check_iterator_next(check_it);

                            /************** Evaluation  **************/
                            ret = xccdf_policy_check_evaluate(policy, check, (char *) rule_id);
                            /*****************************************/
                            if (ret == -1) return -1;

                            if (ret == false) /* we got item that can't be processed */
                                break;
                    }
                    xccdf_check_iterator_free(check_it);
                    /* iteration thorugh checks ends here */

                    callback_out * cb = (callback_out *) xccdf_policy_get_callback(policy, "urn:xccdf:system:callback:output");
                    int retval = 0;
                    if (cb != NULL) {
                            struct oscap_text_iterator * dsc_it = xccdf_rule_get_description((struct xccdf_rule *) item);
                            struct oscap_text_iterator * title_it = xccdf_rule_get_title((struct xccdf_rule *) item);
                            const char * description = NULL;
                            const char * title = NULL;
                            if (oscap_text_iterator_has_more(dsc_it))
                                description = oscap_text_get_text(oscap_text_iterator_next(dsc_it));
                            oscap_text_iterator_free(dsc_it);     
                            if (oscap_text_iterator_has_more(title_it))
                                title = oscap_text_get_text(oscap_text_iterator_next(title_it));
                            oscap_text_iterator_free(title_it);     
                            /*retval = cb->callback(rule_id, ret, cb->usr);*/
                            struct oscap_reporter_message * msg = oscap_reporter_message_new_fmt(
                                    OSCAP_REPORTER_FAMILY_XCCDF, /* FAMILY */
                                    0,                           /* CODE */
                                    description);
                            oscap_reporter_message_set_user1str(msg, rule_id);
                            oscap_reporter_message_set_user2num(msg, (xccdf_test_result_type_t) ret);
                            oscap_reporter_message_set_user3str(msg, title);
                            retval = oscap_reporter_report(cb->callback, msg, cb->usr);
                            if (retval != 0) return retval;
                    }

                    /* Add result to policy */
                    if (result != NULL) {
                            struct xccdf_rule_result *rule_ritem = xccdf_rule_result_new();
                            /* --Set rule-- */
                            xccdf_rule_result_set_result(rule_ritem, (xccdf_test_result_type_t) ret);
                            xccdf_rule_result_set_idref(rule_ritem, rule_id);
                            xccdf_rule_result_set_weight(rule_ritem, xccdf_item_get_weight(item));
                            xccdf_rule_result_set_version(rule_ritem, xccdf_rule_get_version((struct xccdf_rule *) item));
                            xccdf_rule_result_set_severity(rule_ritem, xccdf_rule_get_severity((struct xccdf_rule *) item));
                            xccdf_rule_result_set_role(rule_ritem, xccdf_rule_get_role((struct xccdf_rule *) item));
                            xccdf_rule_result_set_time(rule_ritem, time(NULL));
                            /* --Fix --*/
                            struct xccdf_fix_iterator * fix_it = xccdf_rule_get_fixes((struct xccdf_rule *) item);
                            while (xccdf_fix_iterator_has_more(fix_it)){
                                struct xccdf_fix * fix = xccdf_fix_iterator_next(fix_it);
                                xccdf_rule_result_add_fix(rule_ritem, xccdf_fix_clone(fix));
                            }
                            xccdf_fix_iterator_free(fix_it);
                            /* --Ident-- */
                            struct xccdf_ident_iterator * ident_it = xccdf_rule_get_idents((struct xccdf_rule *) item);
                            while (xccdf_ident_iterator_has_more(ident_it)){
                                struct xccdf_ident * ident = xccdf_ident_iterator_next(ident_it);
                                xccdf_rule_result_add_ident(rule_ritem, xccdf_ident_clone(ident));
                            }
                            xccdf_ident_iterator_free(ident_it);
                            /* TODO: Check, override, message, instance */
                            /* --Add rule-- */
                            xccdf_result_add_rule_result(result, rule_ritem);
                    } else {
                            /* Should be an error here ? */
                            return -1;
                    }
        } break;

        case XCCDF_GROUP:{
                    child_it = xccdf_group_get_content((const struct xccdf_group *)item);
                    while (xccdf_item_iterator_has_more(child_it)) {
                            child = xccdf_item_iterator_next(child_it);
                            ret = xccdf_policy_item_evaluate(policy, child, result);
                            if (ret == -1) return -1;

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

    return 0;
}

static struct xccdf_default_score * xccdf_item_get_default_score(struct xccdf_item * item, struct xccdf_result * test_result)
{

    struct xccdf_default_score  * score;
    struct xccdf_default_score  * ch_score;
    struct xccdf_rule_result    * rule_result;
    struct xccdf_item           * child;

    xccdf_type_t itype = xccdf_item_get_type(item);

    switch (itype) {
        case XCCDF_RULE:{
                    /* Rule */
                    rule_result = xccdf_result_get_rule_result_by_id(test_result, xccdf_rule_get_id((const struct xccdf_rule *) item));
                    if (rule_result == NULL) {
			    /*oscap_dlprintf(DBG_E, "No result of rule %s.\n", xccdf_rule_get_id((const struct xccdf_rule *) item));
                            No result for this rule, just skip */
			    return NULL;
                    }

                    score = oscap_alloc(sizeof(struct xccdf_default_score));
                    /* If the node is a Rule, then assign a count of 1 */
                    score->count = 1;
                    /* If the test result is 'pass', assign the node a score of 100, otherwise assign a score of 0 */
                    if (xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_PASS) score->score = 100.0;
                    else score->score = 0.0;
                    /* Add weight */
                    score->weight_score = xccdf_item_get_weight(item) * score->score;
        } break;

        case XCCDF_BENCHMARK:
        case XCCDF_GROUP:{
                    /* Init */
                    score = oscap_alloc(sizeof(struct xccdf_default_score));
                    score->count = 0; score->score = 0.0; score->accumulator = 0.0;
                    /* Recurse */
                    struct xccdf_item_iterator * child_it;
                    if (itype == XCCDF_GROUP) child_it = xccdf_group_get_content((const struct xccdf_group *)item);
                    else child_it = xccdf_benchmark_get_content((const struct xccdf_benchmark *)item);
                    while (xccdf_item_iterator_has_more(child_it)) {
                            child = xccdf_item_iterator_next(child_it);
                            ch_score = xccdf_item_get_default_score(child, test_result);
                            if (ch_score == NULL) /* we got item that can't be processed */
                                continue; /* TODO */
                            if (ch_score->count == 0) {  /* we got item that has no selected items */
                                oscap_free(ch_score);
                                continue;
                            }
                            /* If child's count value is not 0, then add the child's wighted score to this node's score */
                            if (ch_score->count != 0) { 
                                score->score += ch_score->weight_score;
                                score->count++;
                                score->accumulator += xccdf_item_get_weight(child);
                            }
                            oscap_free(ch_score);
                            /* Normalize */
                            if (score->accumulator != 0) /* Division by zero */
                                score->score = score->score / score->accumulator;
                            else
				    oscap_dlprintf(DBG_E, "Avoided division by zero - Score accumulator is 0!\n");
                            score->weight_score = score->score * xccdf_item_get_weight(item);
                    }
                    xccdf_item_iterator_free(child_it);
        } break;

        
        default: 
                    /* TODO: set warning bad argument and return ? */
                    score=NULL;
            break;
    } 

    return score;
}

static struct xccdf_flat_score * xccdf_item_get_flat_score(struct xccdf_item * item, struct xccdf_result * test_result, bool unweighted)
{

    struct xccdf_flat_score     * score;
    struct xccdf_flat_score     * ch_score;
    struct xccdf_rule_result    * rule_result;
    struct xccdf_item           * child;

    xccdf_type_t itype = xccdf_item_get_type(item);

    switch (itype) {
        case XCCDF_RULE:{
                    /* Rule */
                    rule_result = xccdf_result_get_rule_result_by_id(test_result, xccdf_rule_get_id((const struct xccdf_rule *) item));
                    if (rule_result == NULL) {
			    /*oscap_dlprintf(DBG_E, "No result of rule %s.\n", xccdf_rule_get_id((const struct xccdf_rule *) item));
                            No result for this rule, just skip  */
			    return NULL;
                    }

                    score = oscap_alloc(sizeof(struct xccdf_flat_score));
                    if (unweighted) score->weight = 1.0;
                    else score->weight = xccdf_item_get_weight(item);
                    if ((xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_PASS) ||
                        (xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_FIXED)) {
                            if (unweighted) score->score = 1.0;
                            else score->score = xccdf_item_get_weight(item);
                    } else score->score = 0.0;
        } break;

        case XCCDF_BENCHMARK:
        case XCCDF_GROUP:{
                    /* Init */
                    score = oscap_alloc(sizeof(struct xccdf_flat_score));
                    score->score = 0; score->weight = 0.0;
                    /* Recurse */
                    struct xccdf_item_iterator * child_it;
                    if (itype == XCCDF_GROUP) child_it = xccdf_group_get_content((const struct xccdf_group *)item);
                    else child_it = xccdf_benchmark_get_content((const struct xccdf_benchmark *)item);
                    while (xccdf_item_iterator_has_more(child_it)) {
                            child = xccdf_item_iterator_next(child_it);

                            ch_score = xccdf_item_get_flat_score(child, test_result, unweighted);
                            if (ch_score == NULL) /* we got item that can't be processed */
                                continue; /* TODO */
                            if (ch_score->weight == 0) {  /* we got item that has no selected items */
                                oscap_free(ch_score);
                                continue;
                            }
                            /* If child's count value is not 0, then add the child's wighted score to this node's score */
                            score->score += ch_score->score;
                            score->weight += ch_score->weight;
                            oscap_free(ch_score);
                    }
                    xccdf_item_iterator_free(child_it);
        } break;

        
        default: 
                    /* TODO: set warning bad argument and return ? */
                    score=NULL;
            break;
    } 

    return score;
}

static bool xccdf_cmp_func(const char *s1, const char *s2)
{
    if (!oscap_strcmp(s1, s2)) return true;
    else return false;
}

static struct oscap_stringlist * xccdf_check_get_files(struct xccdf_check * check)
{
    struct xccdf_check_iterator             * child_it;
    struct xccdf_check                      * child;
    struct xccdf_check_content_ref_iterator * content_it;
    struct xccdf_check_content_ref          * content;
    char                                    * href;
    struct oscap_stringlist                 * names;
    struct oscap_stringlist                 * sub_names;

    names = oscap_stringlist_new();
    if (xccdf_check_get_complex(check)) {
        child_it = xccdf_check_get_children(check);
        while (xccdf_check_iterator_has_more(child_it)) {
            child = xccdf_check_iterator_next(child_it);
            sub_names = xccdf_check_get_files(child);

            struct oscap_string_iterator *name_it = oscap_stringlist_get_strings(sub_names);
            while (oscap_string_iterator_has_more(name_it)) {
                href = (char *) oscap_string_iterator_next(name_it);
                if (!oscap_list_contains((struct oscap_list *) names, (void *) href, (oscap_cmp_func) xccdf_cmp_func))
                    oscap_stringlist_add_string(names, href);
            }
            oscap_string_iterator_free(name_it);
        }
        xccdf_check_iterator_free(child_it);
    } else {
        content_it = xccdf_check_get_content_refs(check);
        while (xccdf_check_content_ref_iterator_has_more(content_it)) {
            content = xccdf_check_content_ref_iterator_next(content_it);
            href = (char *) xccdf_check_content_ref_get_href(content);
            if (!oscap_list_contains((struct oscap_list *) names, href, (oscap_cmp_func) xccdf_cmp_func))
                oscap_stringlist_add_string(names, href);
        }
        xccdf_check_content_ref_iterator_free(content_it);
    }

    return names;
}

static struct oscap_stringlist * xccdf_item_get_files(struct xccdf_item * item)
{

    struct xccdf_item_iterator  * child_it;
    struct xccdf_item           * child;
    struct xccdf_check_iterator * check_it;
    struct xccdf_check          * check;
    struct oscap_stringlist     * names;
    struct oscap_stringlist     * sub_names;
    char                        * href;

    xccdf_type_t itype = xccdf_item_get_type(item);
    names = oscap_stringlist_new();

    switch (itype) {
        case XCCDF_RULE:
            check_it = xccdf_rule_get_checks((struct xccdf_rule *) item);
            while(xccdf_check_iterator_has_more(check_it)) {
                check = xccdf_check_iterator_next(check_it);
                sub_names = xccdf_check_get_files(check);

                struct oscap_string_iterator *name_it = oscap_stringlist_get_strings(sub_names);
                while (oscap_string_iterator_has_more(name_it)) {
                    href = (char *) oscap_string_iterator_next(name_it);
                    if (!oscap_list_contains((struct oscap_list *)names, href, (oscap_cmp_func) xccdf_cmp_func))
                        oscap_stringlist_add_string(names, href);
                }
                oscap_string_iterator_free(name_it);
            }
            xccdf_check_iterator_free(check_it);
            break;

        case XCCDF_BENCHMARK:
        case XCCDF_GROUP:
            if (itype == XCCDF_GROUP) child_it = xccdf_group_get_content((const struct xccdf_group *)item);
            else child_it = xccdf_benchmark_get_content((const struct xccdf_benchmark *)item);
            while (xccdf_item_iterator_has_more(child_it)) {
                    child = xccdf_item_iterator_next(child_it);
                    sub_names = xccdf_item_get_files(child);

                    struct oscap_string_iterator *name_it = oscap_stringlist_get_strings(sub_names);
                    while (oscap_string_iterator_has_more(name_it)) {
                        href = (char *) oscap_string_iterator_next(name_it);
                        if (!oscap_list_contains((struct oscap_list *)names, href, (oscap_cmp_func) xccdf_cmp_func))
                            oscap_stringlist_add_string(names, href);
                    }
                    oscap_string_iterator_free(name_it);
            }
            xccdf_item_iterator_free(child_it);
        break;
        
        default: 
            oscap_stringlist_free(names);
            names = NULL;
            break;
    } 

    return names;
}




/***************************************************************************/
/* Public functions.
 */

/**
 * Get ID of XCCDF Profile that belongs to XCCDF Policy
 */
const char * xccdf_policy_get_id(struct xccdf_policy * policy)
{
    if (policy->profile != NULL)
        return xccdf_profile_get_id(xccdf_policy_get_profile(policy));
    else return NULL;
}

/**
 * Funtion to register callback for particular checking system. System is used for evaluating content
 * of rules.
 */
bool xccdf_policy_model_register_engine_callback(struct xccdf_policy_model * model, char * sys, void * func, void * usr)
{

        __attribute__nonnull__(model);
        callback * cb = oscap_alloc(sizeof(callback));
        if (cb == NULL) return false;

        cb->system   = sys;
        cb->callback = func;
        cb->usr      = usr;

        return oscap_list_add(model->callbacks, cb);
}

bool xccdf_policy_model_register_output_callback(struct xccdf_policy_model * model, oscap_reporter func, void * usr)
{

        __attribute__nonnull__(model);
        callback_out * cb = oscap_alloc(sizeof(callback_out));
        if (cb == NULL) return false;

        cb->system   = "urn:xccdf:system:callback:output";
        cb->callback = func;
        cb->usr      = usr;

        return oscap_list_add(model->callbacks, (callback *) cb);
}

struct xccdf_result * xccdf_policy_get_result_by_id(struct xccdf_policy * policy, const char * id) {

    struct xccdf_result_iterator    * result_it;
    struct xccdf_result             * result;

    result_it = xccdf_policy_get_results(policy);
    while (xccdf_result_iterator_has_more(result_it)){
        result = xccdf_result_iterator_next(result_it);
        if (!strcmp(xccdf_result_get_id(result), id) ){
            xccdf_result_iterator_free(result_it);
            return result;
        }
    }
    xccdf_result_iterator_free(result_it);
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

        /* Resolve document */
        xccdf_benchmark_resolve(benchmark);

        /* Create policy without profile */
        policy = xccdf_policy_new(model, NULL); 
        if (policy != NULL) oscap_list_add(model->policies, policy);

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

	struct xccdf_policy             * policy;
        struct xccdf_benchmark          * benchmark;
        struct xccdf_item_iterator      * item_it;
        struct xccdf_item               * item;
        struct xccdf_select             * sel;
        struct xccdf_select_iterator    * sel_it = NULL;

	policy = oscap_alloc(sizeof(struct xccdf_policy));
	if (policy == NULL)
		return NULL;
	memset(policy, 0, sizeof(struct xccdf_policy));

	policy->profile = profile;
	policy->rules   = oscap_list_new();
	policy->values  = oscap_list_new();
        policy->results = oscap_list_new();

        policy->model = model;

        /* Create selects from benchmark model */
        if (profile != NULL) sel_it = xccdf_profile_get_selects(profile);
        /* Iterate through selects in profile */
        while (xccdf_select_iterator_has_more(sel_it)) {

            sel = xccdf_select_iterator_next(sel_it);
            /* Should we set the error code and return NULL here ? */
            if (sel != NULL) oscap_list_add(policy->rules, xccdf_select_clone(sel));
        }
        xccdf_select_iterator_free(sel_it);

        /* Iterate through items in benchmark and resolve rules */
        benchmark = xccdf_policy_model_get_benchmark(model);
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

    if (id == NULL) return NULL;

    policy_it = xccdf_policy_model_get_policies(policy_model);
    while (xccdf_policy_iterator_has_more(policy_it)) {
        policy = xccdf_policy_iterator_next(policy_it);
        if (xccdf_policy_get_id(policy) == NULL) continue;
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
    struct xccdf_profile                * profile       = xccdf_policy_get_profile(policy);

    /* Proccess refine rules; Changing Rules and Groups */
    if (profile == NULL) return false;
    r_rule_it = xccdf_profile_get_refine_rules(profile);
    while (xccdf_refine_rule_iterator_has_more(r_rule_it)) {
        r_rule = xccdf_refine_rule_iterator_next(r_rule_it);
        item = xccdf_benchmark_get_item(benchmark, xccdf_refine_rule_get_item(r_rule));
        if (item != NULL) {
            /* Proccess refine rule appliement */
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
struct xccdf_result * xccdf_policy_evaluate(struct xccdf_policy * policy)
{

    struct xccdf_select_iterator    * sel_it;
    struct xccdf_select             * sel;
    struct xccdf_item               * item;
    struct xccdf_benchmark          * benchmark;
    int                               ret       = -1;

    __attribute__nonnull__(policy);

    /* Add result to policy */
    struct xccdf_result * result = xccdf_result_new();
    xccdf_result_set_start_time(result, time(NULL));

    /** Set ID of TestResult */
    const char * id = NULL;
    if (xccdf_policy_get_profile(policy) != NULL)
        id = oscap_strdup(xccdf_profile_get_id(xccdf_policy_get_profile(policy)));
    else
        id = oscap_strdup("default-profile");

    char rid[11+strlen(id)];
    sprintf(rid, "OSCAP-Test-%s", id);
    xccdf_result_set_id(result, rid);
    oscap_free(id);
    /** */

    /* Get all constant information */
    benchmark = xccdf_policy_model_get_benchmark(xccdf_policy_get_model(policy));

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
        ret = xccdf_policy_item_evaluate(policy, item, result);
        if (ret == -1) return NULL;
        if (ret != 0) break;
    }
    xccdf_select_iterator_free(sel_it);
    xccdf_policy_add_result(policy, result);
    xccdf_result_set_end_time(result, time(NULL));

    return result;
}

struct xccdf_score * xccdf_policy_get_score(struct xccdf_policy * policy, struct xccdf_result * test_result, const char * scsystem)
{
    struct xccdf_score * score = NULL;
    struct xccdf_benchmark * benchmark = xccdf_policy_model_get_benchmark(xccdf_policy_get_model(policy));

    score = xccdf_score_new();
    xccdf_score_set_system(score, scsystem);
    /* Default XCCDF score system */
    if (!strcmp(scsystem, "urn:xccdf:scoring:default")) {
        struct xccdf_default_score * item_score = xccdf_item_get_default_score((struct xccdf_item *) benchmark, test_result);
        xccdf_score_set_score(score, item_score->score);
        oscap_free(item_score);
    }
    else if (!strcmp(scsystem, "urn:xccdf:scoring:flat")) {
        struct xccdf_flat_score * item_score = xccdf_item_get_flat_score((struct xccdf_item *) benchmark, test_result, false);
        xccdf_score_set_maximum(score, item_score->weight);
        xccdf_score_set_score(score, item_score->score);
        oscap_free(item_score);
    }
    else if (!strcmp(scsystem, "urn:xccdf:scoring:flat-unweighted")) {
        struct xccdf_flat_score * item_score = xccdf_item_get_flat_score((struct xccdf_item *) benchmark, test_result, true);
        xccdf_score_set_maximum(score, item_score->weight);
        xccdf_score_set_score(score, item_score->score);
        oscap_free(item_score);
    }
    else if (!strcmp(scsystem, "urn:xccdf:scoring:absolute")) {
        int absolute;
        struct xccdf_flat_score * item_score = xccdf_item_get_flat_score((struct xccdf_item *) benchmark, test_result, false);
        xccdf_score_set_maximum(score, item_score->weight);
        absolute = (item_score->score == item_score->weight);
        xccdf_score_set_score(score, absolute);
        oscap_free(item_score);
    } else {
        xccdf_score_free(score);
        oscap_dlprintf(DBG_E, "Scoring system \"%s\" is not supported.\n", scsystem);
        return NULL;
    }

    return score;
}

static struct xccdf_refine_rule * xccdf_policy_get_refine_rules_by_rule(struct xccdf_policy * policy, struct xccdf_item * item)
{
    struct xccdf_refine_rule * r_rule = NULL;
    struct xccdf_profile * profile = xccdf_policy_get_profile(policy);
    if (profile == NULL) return NULL;

    /* Get refine-rule for this item */
    struct xccdf_refine_rule_iterator * r_rule_it = xccdf_profile_get_refine_rules(profile);
    while (xccdf_refine_rule_iterator_has_more(r_rule_it)) {
        r_rule = xccdf_refine_rule_iterator_next(r_rule_it);
        if (!strcmp(xccdf_refine_rule_get_item(r_rule), xccdf_rule_get_id((struct xccdf_rule *) item)))
            break;
        else r_rule = NULL;
    }
    xccdf_refine_rule_iterator_free(r_rule_it);

    return r_rule;
}

static const char * xccdf_policy_get_value_of_item(struct xccdf_policy * policy, struct xccdf_item * item)
{
    struct xccdf_refine_value * r_value = NULL;
    struct xccdf_setvalue * s_value = NULL;
    const char * selector = NULL;
    struct xccdf_profile * profile = xccdf_policy_get_profile(policy);
    if (profile == NULL) return NULL;

    /* Get set_value for this item */
    struct xccdf_setvalue_iterator * s_value_it = xccdf_profile_get_setvalues(profile);
    while (xccdf_setvalue_iterator_has_more(s_value_it)) {
        s_value = xccdf_setvalue_iterator_next(s_value_it);
        if (!strcmp(xccdf_setvalue_get_item(s_value), xccdf_value_get_id((struct xccdf_value *) item)))
            break;
        else s_value = NULL;
    }
    xccdf_setvalue_iterator_free(s_value_it);
    if (s_value != NULL) return xccdf_setvalue_get_value(s_value);

    /* We don't have set-value in profile, look for refine-value */
    struct xccdf_refine_value_iterator * r_value_it = xccdf_profile_get_refine_values(profile);
    while (xccdf_refine_value_iterator_has_more(r_value_it)) {
        r_value = xccdf_refine_value_iterator_next(r_value_it);
        if (!strcmp(xccdf_refine_value_get_item(r_value), xccdf_value_get_id((struct xccdf_value *) item)))
            break;
        else r_value = NULL;
    }
    xccdf_refine_value_iterator_free(r_value_it);
    if (r_value != NULL) {
        selector = xccdf_refine_value_get_selector(r_value);
        struct xccdf_value_instance * instance = xccdf_value_get_instance_by_selector((struct xccdf_value *) item, selector);
        return xccdf_value_instance_get_value(instance);
    }

    return NULL;
}

static int xccdf_policy_get_refine_value_oper(struct xccdf_policy * policy, struct xccdf_item * item)
{
    struct xccdf_refine_value * r_value = NULL;
    struct xccdf_profile * profile = xccdf_policy_get_profile(policy);
    if (profile == NULL) return -1;

    /* We don't have set-value in profile, look for refine-value */
    struct xccdf_refine_value_iterator * r_value_it = xccdf_profile_get_refine_values(profile);
    while (xccdf_refine_value_iterator_has_more(r_value_it)) {
        r_value = xccdf_refine_value_iterator_next(r_value_it);
        if (!strcmp(xccdf_refine_value_get_item(r_value), xccdf_value_get_id((struct xccdf_value *) item)))
            break;
        else r_value = NULL;
    }
    xccdf_refine_value_iterator_free(r_value_it);
    if (r_value != NULL) {
        return xccdf_refine_value_get_oper(r_value);
    }
    return -1;
}

struct xccdf_item * xccdf_policy_tailor_item(struct xccdf_policy * policy, struct xccdf_item * item)
{
    struct xccdf_item * new_item = NULL;

    xccdf_type_t type = xccdf_item_get_type(item);
    switch (type) {
        case XCCDF_RULE: {
            struct xccdf_refine_rule * r_rule = xccdf_policy_get_refine_rules_by_rule(policy, item);
            if (r_rule == NULL) return item;

            new_item = (struct xccdf_item *) xccdf_rule_clone((struct xccdf_rule *) item);
            if (xccdf_refine_rule_get_role(r_rule) != NAN)
                xccdf_rule_set_role((struct xccdf_rule *) new_item, xccdf_refine_rule_get_role(r_rule));
            if (xccdf_refine_rule_get_severity(r_rule) != NAN)
                xccdf_rule_set_severity((struct xccdf_rule *) new_item, xccdf_refine_rule_get_severity(r_rule));
            break;
        }
        case XCCDF_GROUP: {
            struct xccdf_refine_rule * r_rule = xccdf_policy_get_refine_rules_by_rule(policy, item);
            if (r_rule == NULL) return item;

            new_item = (struct xccdf_item *) xccdf_group_clone((struct xccdf_group *) item);
            if (xccdf_refine_rule_get_weight(r_rule) != NAN)
                xccdf_group_set_weight((struct xccdf_group *) new_item, xccdf_refine_rule_get_weight(r_rule));
            else {
                xccdf_group_free(new_item);
                return item;
            }
            break;
        }
        case XCCDF_VALUE: {
            const char * value = xccdf_policy_get_value_of_item(policy, item);
            if (value == NULL) 
                return NULL;

            const char * selector = NULL;
            struct xccdf_value_instance * instance = NULL;
            new_item = (struct xccdf_item *) xccdf_value_clone((struct xccdf_value *) item);

            struct xccdf_value_instance_iterator * instance_it = xccdf_value_get_instances((struct xccdf_value *) new_item);
            while (xccdf_value_instance_iterator_has_more(instance_it)) {
                instance = xccdf_value_instance_iterator_next(instance_it);
                if (!strcmp(xccdf_value_instance_get_value(instance), value))
                    selector = xccdf_value_instance_get_selector(instance);
            }
            xccdf_value_instance_iterator_free(instance_it);
            instance_it = xccdf_value_get_instances((struct xccdf_value *) new_item);
            while (xccdf_value_instance_iterator_has_more(instance_it)) {
                instance = xccdf_value_instance_iterator_next(instance_it);
                if (strcmp(xccdf_value_instance_get_selector(instance), selector))
                    xccdf_value_instance_iterator_remove(instance_it);
            }
            xccdf_value_instance_iterator_free(instance_it);
            if (selector == NULL) {
                instance = xccdf_value_get_instance_by_selector((struct xccdf_value *) new_item, NULL);
                xccdf_value_instance_set_defval_string(instance, value);
            }

            int oper = xccdf_policy_get_refine_value_oper(policy, item);
            if (oper == -1) break;
            xccdf_value_set_oper(item, (xccdf_operator_t) oper);
            break;
        }
        default:
            return NULL;
    }
    return new_item;
}

struct oscap_stringlist * xccdf_policy_model_get_files(struct xccdf_policy_model * policy_model)
{
    return xccdf_item_get_files((struct xccdf_item *) xccdf_policy_model_get_benchmark(policy_model));
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
	oscap_list_free(model->callbacks, (oscap_destruct_func) oscap_free);
        oscap_free(model);
}

void xccdf_policy_free(struct xccdf_policy * policy) {

	oscap_list_free(policy->rules, (oscap_destruct_func) xccdf_select_free);
	oscap_list_free(policy->values, (oscap_destruct_func) xccdf_value_binding_free);
	oscap_list_free(policy->results, (oscap_destruct_func) xccdf_result_free);
        oscap_free(policy);
}

void xccdf_value_binding_free(struct xccdf_value_binding * binding) {

        oscap_free(binding->name);
        oscap_free(binding->value);
        oscap_free(binding->setvalue);
        oscap_free(binding);
}
