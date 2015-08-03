/*
 * xccdf_policy.c
 *
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "xccdf_policy_priv.h"
#include "xccdf_policy_model_priv.h"
#include "xccdf_policy_engine_priv.h"
#include "reporter_priv.h"
#include "public/xccdf_policy.h"
#include "public/xccdf_benchmark.h"
#include "public/oscap_text.h"

#include "cpe_lang.h"

#include "oval_agent_api.h"

#include "item.h"
#include "common/list.h"
#include "common/_error.h"
#include "common/debug_priv.h"
#include "common/assume.h"
#include "common/text_priv.h"
#include "xccdf_policy_resolve.h"

/* Macros to generate iterators, getters and setters */
OSCAP_GETTER(struct xccdf_benchmark *, xccdf_policy_model, benchmark)
OSCAP_IGETINS_GEN(xccdf_policy, xccdf_policy_model, policies, policy)

/* Macros to generate iterators, getters and setters */
OSCAP_GETTER(struct xccdf_policy_model *, xccdf_policy, model)
OSCAP_GETTER(struct xccdf_profile *, xccdf_policy, profile)
OSCAP_IGETTER(xccdf_select, xccdf_policy, selects)
OSCAP_IGETINS_GEN(xccdf_value_binding, xccdf_policy, values, value)
OSCAP_IGETINS(xccdf_result, xccdf_policy, results, result)

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


/*==========================================================================*/
/* Declaration of static (private to this file) functions
 * These function shoud not be called from outside. For exporting these 
 * elements has to call parent element's 
 */

/**
 * Filter function returning true if the item is selected, false otherwise
 */
static bool xccdf_policy_filter_selected(void *item, void *policy)
{
        struct xccdf_benchmark * benchmark 
            = xccdf_policy_model_get_benchmark(xccdf_policy_get_model((struct xccdf_policy *) policy));

        struct xccdf_item * titem = xccdf_benchmark_get_item(benchmark, xccdf_select_get_item((struct xccdf_select *) item));
        if (titem == NULL) {
            oscap_dlprintf(DBG_E, "Item \"%s\" does not exist. Remove it from Profile !\n", xccdf_select_get_item((struct xccdf_select *) item));
            return false;
        }
	return ((xccdf_item_get_type(titem) == XCCDF_RULE) &&
		(xccdf_select_get_selected((struct xccdf_select *) item)));
}

/**
 * Get callbacks that match sysname. Call this function to get iterator over list of callbacks
 * that have the same system name
 */
static struct oscap_iterator *
_xccdf_policy_get_callbacks_by_sysname(struct xccdf_policy * policy, const char * sysname)
{
	return oscap_iterator_new_filter( policy->model->callbacks, (oscap_filter_func) xccdf_policy_engine_filter, (void *) sysname);
}

/**
 * Get the checking engines that match the sysname.
 */
static struct oscap_iterator *_xccdf_policy_get_engines_by_sysname(struct xccdf_policy *policy, const char *sysname)
{
	return oscap_iterator_new_filter(policy->model->engines, (oscap_filter_func) xccdf_policy_engine_filter, (void *) sysname);
}

char *xccdf_policy_get_readable_item_title(struct xccdf_policy *policy, struct xccdf_item *item, const char *preferred_lang)
{
	struct oscap_text_iterator *title_it = xccdf_item_get_title(item);
	char *unresolved = oscap_textlist_get_preferred_plaintext(title_it, preferred_lang);
	oscap_text_iterator_free(title_it);
	if (!unresolved)
		return oscap_strdup("");
	char *resolved = xccdf_policy_substitute(unresolved, policy);
	oscap_free(unresolved);
	return resolved;
}

char *xccdf_policy_get_readable_item_description(struct xccdf_policy *policy, struct xccdf_item *item, const char *preferred_lang)
{
	/* Get description in prefered language */
	struct oscap_text_iterator *description_it = xccdf_item_get_description(item);
	struct oscap_text *unresolved_text = oscap_textlist_get_preferred_text(description_it, preferred_lang);
	oscap_text_iterator_free(description_it);
	if (!unresolved_text)
		return oscap_strdup("");
	const char *unresolved = oscap_text_get_text(unresolved_text);
	/* Resolve <xccdf:sub> elements */
	const char *resolved = xccdf_policy_substitute(unresolved, policy);
	/* Get a rid of xhtml elements */
	char *plaintext = _xhtml_to_plaintext(resolved);
	oscap_free(resolved);
	return plaintext;
}

/**
 * Get last setvalue from policy that match specified id
 */
static struct xccdf_setvalue * xccdf_policy_get_setvalue(struct xccdf_policy * policy, const char * id)
{
    /* return NULL if id or policy is NULL but don't use
     * __attribute_not_null__ here, it will cause abort
     * which is not desired
     */
    if (id == NULL) return NULL;
    if (policy == NULL) return NULL;

    struct xccdf_setvalue_iterator  * s_value_it;
    struct xccdf_setvalue           * s_value;
    struct xccdf_profile            * profile = xccdf_policy_get_profile(policy);;

    /* If profile is NULL we don't have setvalue's
     * and we return NULL, otherwise we could cause SIGSEG
     * with accessing NULL structure
     */
    if (profile == NULL) return NULL;

    /* We need to return *LAST* setvalue in Profile.
     * and iterate to the end of setvalue list
     */
    struct xccdf_setvalue * final = NULL;
    s_value_it = xccdf_profile_get_setvalues(profile);
    while (xccdf_setvalue_iterator_has_more(s_value_it)) {
        s_value = xccdf_setvalue_iterator_next(s_value_it);
        if (!strcmp(id, xccdf_setvalue_get_item(s_value))) {
            final = s_value;
        }
    }
    xccdf_setvalue_iterator_free(s_value_it);

    return final;
}

static struct xccdf_refine_value * xccdf_policy_get_refine_value(struct xccdf_policy * policy, const char * id)
{
    /* return NULL if id or policy is NULL but don't use
     * __attribute_not_null__ here, it will cause abort
     * which is not desired
     */
    if (id == NULL) return NULL;
    if (policy == NULL) return NULL;

    struct xccdf_refine_value_iterator  * r_value_it;
    struct xccdf_refine_value           * r_value;
    struct xccdf_profile            * profile = xccdf_policy_get_profile(policy);;

    /* If profile is NULL we don't have setvalue's
     * and we return NULL, otherwise we could cause SIGSEG
     * with accessing NULL structure
     */
    if (profile == NULL) return NULL;

    /* We need to return *LAST* setvalue in Profile.
     * and iterate to the end of setvalue list
     */
    struct xccdf_refine_value * final = NULL;
    r_value_it = xccdf_profile_get_refine_values(profile);
    while (xccdf_refine_value_iterator_has_more(r_value_it)) {
        r_value = xccdf_refine_value_iterator_next(r_value_it);
        if (!strcmp(id, xccdf_refine_value_get_item(r_value))) {
            final = r_value;
        }
    }
    xccdf_refine_value_iterator_free(r_value_it);

    return final;
}

/**
 * Function resolves two operations:
 *  P - PASS
 *  F - FAIL
 *  U - UNKNOWN
 *  E - ERROR
 *  N - NOT CHECKED, NOT SELECTED, NOT APPLICABLE
 *
 * ***************************************************************
 * AND  P  F  U  E  N    OR  P  F  U  E  N         P  F  U  E  N *
 *   P  P  F  U  E  P     P  P  P  P  P  P    neg  F  P  U  E  N *
 *   F  F  F  F  F  F     F  P  F  U  E  F                       *
 *   U  U  F  U  U  U     U  P  U  U  U  U                       *
 *   E  E  F  U  E  E     E  P  E  U  E  E                       *
 *   N  P  F  U  E  N     N  P  F  U  E  N                       *
 * ***************************************************************
 *
 */

static xccdf_test_result_type_t _resolve_operation(int A, int B, xccdf_bool_operator_t oper)
{

    xccdf_test_result_type_t value = 0;

    static const xccdf_test_result_type_t RESULT_TABLE_AND[9][9] = {
        /*  P  F  E  U  N  K  S  I */
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 2, 3, 4, 1, 1, 1, 1}, /* P (pass)*/
	{0, 2, 2, 2, 2, 2, 2, 2, 2}, /* F (fail) */
	{0, 4, 2, 4, 4, 4, 4, 4, 4}, /* E (error) */
	{0, 3, 2, 3, 4, 3, 3, 3, 3}, /* U (unknown) */
	{0, 1, 2, 3, 4, 5, 5, 5, 5}, /* N (notapplicable) */
	{0, 1, 2, 3, 4, 5, 6, 6, 6}, /* K (notchecked) */
	{0, 1, 2, 3, 4, 5, 6, 7, 7}, /* S (notselected) */
	{0, 1, 2, 3, 4, 5, 6, 7, 8}};/* I (informational) */

    static const xccdf_test_result_type_t RESULT_TABLE_OR[9][9] = {
        /*  P  F  E  U  N  K  S  I */
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 1, 1, 1, 1, 1, 1, 1}, /* P (pass)*/
	{0, 1, 2, 3, 4, 2, 2, 2, 2}, /* F (fail) */
	{0, 1, 4, 4, 4, 4, 4, 4, 4}, /* E (error) */
	{0, 1, 3, 3, 4, 3, 3, 3, 3}, /* U (unknown) */
	{0, 1, 2, 3, 4, 5, 5, 5, 5}, /* N (notapplicable) */
	{0, 1, 2, 3, 4, 5, 6, 6, 6}, /* K (notchecked) */
	{0, 1, 2, 3, 4, 5, 6, 7, 7}, /* S (notselected) */
	{0, 1, 2, 3, 4, 5, 6, 7, 8}};/* I (informational) */

    /* No test result can end with 0
     */
    if ((A == 0) || (B == 0)
	|| A > XCCDF_RESULT_INFORMATIONAL || B > XCCDF_RESULT_INFORMATIONAL) {
	oscap_dlprintf(DBG_E, "Bad test results %d, %d.\n", A, B);
	return 0;
    }

    switch (oper) {
        case XCCDF_OPERATOR_AND: /* AND */
            value = (xccdf_test_result_type_t) RESULT_TABLE_AND[A][B];
            break;

        case XCCDF_OPERATOR_OR: /* OR */
            value = (xccdf_test_result_type_t) RESULT_TABLE_OR[A][B];
            break;
	default:
	    oscap_dlprintf(DBG_E, "Operation not supported.\n");
            return 0;
            break;
    }

    return value;
}

xccdf_test_result_type_t xccdf_test_result_resolve_and_operation(xccdf_test_result_type_t A, xccdf_test_result_type_t B)
{
	return _resolve_operation((int)A, (int)B, XCCDF_OPERATOR_AND);
}

/**
 * Handle the negation="true" paramter of xccdf:complex-check.
 * Shall be run only once per a complex-check.
 */
static xccdf_test_result_type_t _resolve_negate(xccdf_test_result_type_t value, const struct xccdf_check *check)
{
    if (xccdf_check_get_negate(check)) {
        if (value == XCCDF_RESULT_PASS) return XCCDF_RESULT_FAIL;
        else if (value == XCCDF_RESULT_FAIL) return XCCDF_RESULT_PASS;
    }
    return value;
}

/**
 * Resolve the xccdf item. Parameter selected indicates parents selection attribute
 * It is used to decide the final selection attribute of item
 *
 * If parent's group of rule item has attribute selected = FALSE, It should not be processed, neither
 * its children. This selected attribute is iterativly passed to each child of group to set their selected
 * attribute to false no matter of profile setting.
 */
static void xccdf_policy_resolve_item(struct xccdf_policy * policy, struct xccdf_item * item, bool selected)
{
        __attribute__nonnull__(policy);
        __attribute__nonnull__(item);

	static bool TRUE0= true;
	static bool FALSE0 = false;
	bool result;

	oscap_htable_detach(policy->selected_final, xccdf_item_get_id(item));
	if (!selected)
		result = false;
	else {
		struct xccdf_select *sel = xccdf_policy_get_select_by_id(policy, xccdf_item_get_id(item));
		result = (sel != NULL) ? xccdf_select_get_selected(sel) : xccdf_item_get_selected(item);
	}

	if (xccdf_item_get_type(item) == XCCDF_GROUP) {
		struct xccdf_item_iterator *child_it = xccdf_group_get_content((const struct xccdf_group *)item);
		while (xccdf_item_iterator_has_more(child_it))
			xccdf_policy_resolve_item(policy, xccdf_item_iterator_next(child_it), result);
		xccdf_item_iterator_free(child_it);
	}

	assume_ex(oscap_htable_add(policy->selected_final, xccdf_item_get_id(item), result ? &TRUE0 : &FALSE0), NULL);
}

/**
 * Evaluate the policy check with given checking system
 */
static int
xccdf_policy_evaluate_cb(struct xccdf_policy * policy, const char * sysname, const char * content, const char * href,
		struct oscap_list * bindings, struct xccdf_check_import_iterator * check_import_it)
{
    xccdf_test_result_type_t retval = XCCDF_RESULT_NOT_CHECKED;
    struct oscap_iterator * cb_it = _xccdf_policy_get_engines_by_sysname(policy, sysname);
    while (oscap_iterator_has_more(cb_it)) {
        struct xccdf_policy_engine *engine = (struct xccdf_policy_engine *) oscap_iterator_next(cb_it);
	retval = xccdf_policy_engine_eval(engine, policy, content, href, bindings, check_import_it);
        if (retval != XCCDF_RESULT_NOT_CHECKED) break;
    }
    oscap_iterator_free(cb_it);

    return retval;
}

/**
 * Find all posible names for given check-content-ref/@href, considering also the check/@system.
 * This is usefull for multi-check="true" feature.
 * @return list of names (even empty) if the given href found, NULL otherwise.
 */
static struct oscap_stringlist *
_xccdf_policy_get_namesfor_href(struct xccdf_policy *policy, const char *sysname, const char *href)
{
	struct oscap_iterator *cb_it = _xccdf_policy_get_engines_by_sysname(policy, sysname);
	struct oscap_stringlist *result = NULL;
	while (oscap_iterator_has_more(cb_it) && result == NULL) {
		struct xccdf_policy_engine *engine = (struct xccdf_policy_engine *) oscap_iterator_next(cb_it);
		if (engine == NULL)
			break;
		result = xccdf_policy_engine_query(engine, POLICY_ENGINE_QUERY_NAMES_FOR_HREF, (void *) href);
	}
	oscap_iterator_free(cb_it);
	return result;
}

int xccdf_policy_report_cb(struct xccdf_policy *policy, const char *sysname, void *rule)
{
    int retval = 0;
    struct oscap_iterator * cb_it = _xccdf_policy_get_callbacks_by_sysname(policy, sysname);
    while (oscap_iterator_has_more(cb_it)) {
        struct reporter *cb = (struct reporter *) oscap_iterator_next(cb_it);

	retval = reporter_send_simple(cb, rule);

        /* We still want to stop evaluation if user cancel it
         * TODO: We should have a way to stop evaluation of current item
         */
        if (retval != 0) break;
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

            value = (struct xccdf_value *) xccdf_benchmark_get_item(benchmark, xccdf_check_export_get_value(check));
            if (value == NULL) {
                oscap_seterr(OSCAP_EFAMILY_XCCDF, "Value \"%s\" does not exist in benchmark", xccdf_check_export_get_value(check));
		oscap_list_free(list, oscap_free);
                return NULL;
            }
            binding = xccdf_value_binding_new();

            /* Apply related setvalue from policy profile */
            s_value = xccdf_policy_get_setvalue(policy, xccdf_value_get_id(value));
            if (s_value != NULL) binding->setvalue = oscap_strdup((char *) xccdf_setvalue_get_value(s_value));

            /* Apply related refine value from policy profile */
            const char * selector = NULL;
            r_value = xccdf_policy_get_refine_value(policy, xccdf_value_get_id(value));
            if (r_value != NULL) {
                selector = xccdf_refine_value_get_selector(r_value);
                /* This refine value changes the value content */
                if ((int)xccdf_refine_value_get_oper(r_value) > 0) {
                    binding->operator = xccdf_refine_value_get_oper(r_value);
                } else binding->operator = xccdf_value_get_oper(value);

            } else binding->operator = xccdf_value_get_oper(value);

            const struct xccdf_value_instance * val = xccdf_value_get_instance_by_selector(value, selector);
            if (val == NULL) {
                oscap_seterr(OSCAP_EFAMILY_XCCDF, "Attempt to get non-existent selector \"%s\" from variable \"%s\"", selector, xccdf_value_get_id(value));
		oscap_list_free(list, oscap_free);
		xccdf_value_binding_free(binding);
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

int xccdf_policy_check_evaluate(struct xccdf_policy * policy, struct xccdf_check * check)
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
                ret2 = xccdf_policy_check_evaluate(policy, child);
                if (ret2 == -1) {
                    xccdf_check_iterator_free(child_it);
                    return -1;
		}
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
            if (bindings == NULL) {
                xccdf_check_content_ref_iterator_free(content_it);
                return XCCDF_RESULT_UNKNOWN;
	    }
            while (xccdf_check_content_ref_iterator_has_more(content_it)) {
                content = xccdf_check_content_ref_iterator_next(content_it);
                content_name = xccdf_check_content_ref_get_name(content);
                href = xccdf_check_content_ref_get_href(content);

                struct xccdf_check_import_iterator * check_import_it = xccdf_check_get_imports(check);
                ret = xccdf_policy_evaluate_cb(policy, system_name, content_name, href, bindings, check_import_it);
                // the evaluation has filled check imports at this point, we can simply free the iterator
                xccdf_check_import_iterator_free(check_import_it);

                // the content references are basically alternatives according to the specification
                // we should go through them in the order they are defined and we are done as soon
                // as we process one of them successfully
                if ((xccdf_test_result_type_t) ret != XCCDF_RESULT_NOT_CHECKED) {
			xccdf_check_inject_content_ref(check, content, NULL);
			break;
		}
            }
            xccdf_check_content_ref_iterator_free(content_it);
            oscap_list_free(bindings, (oscap_destruct_func) xccdf_value_binding_free);
    }
    /* Negate only once */
    ret = _resolve_negate(ret, check);
    return ret;
}

static inline bool
_xccdf_policy_is_engine_registered(struct xccdf_policy *policy, char *sysname)
{
	return oscap_list_contains(policy->model->engines, (void *) sysname, (oscap_cmp_func) xccdf_policy_engine_filter);
}

static struct xccdf_check *
_xccdf_policy_rule_get_applicable_check(struct xccdf_policy *policy, struct xccdf_item *rule)
{
	// Citations inline come from NISTIR-7275r4.
	struct xccdf_check *result = NULL;
	{
		// If an <xccdf:Rule> contains an <xccdf:complex-check>, then the benchmark consumer MUST process
		// it and MUST ignore any <xccdf:check> elements that are also contained by the <xccdf:Rule>.
		struct xccdf_check_iterator *check_it = xccdf_rule_get_complex_checks(rule);
		if (xccdf_check_iterator_has_more(check_it))
			result = xccdf_check_iterator_next(check_it);
		xccdf_check_iterator_free(check_it);
	}
	if (result == NULL) {
		// Check Processing Algorithm -- Check.Initialize
		// Check Processing Algorithm -- Check.Selector
		struct xccdf_refine_rule_internal *r_rule = xccdf_policy_get_refine_rule_by_item(policy, rule);
		char *selector = (r_rule == NULL) ? NULL : xccdf_refine_rule_internal_get_selector(r_rule);
		struct xccdf_check_iterator *candidate_it = xccdf_rule_get_checks_filtered(rule, selector);
		if (selector != NULL && !xccdf_check_iterator_has_more(candidate_it)) {
			xccdf_check_iterator_free(candidate_it);
			// If the refined selector does not match, checks without selector shall be used.
			candidate_it = xccdf_rule_get_checks_filtered(rule, NULL);
		}
		// Check Processing Algorithm -- Check.System
		while (xccdf_check_iterator_has_more(candidate_it)) {
			struct xccdf_check *check = xccdf_check_iterator_next(candidate_it);
			if (_xccdf_policy_is_engine_registered(policy, (char *) xccdf_check_get_system(check)))
				result = check;
		}
		xccdf_check_iterator_free(candidate_it);
	}
	// A tool processing the Benchmark for compliance checking must pick at most one check or
	// complex-check element to process for each Rule.
	return result;
}

bool
xccdf_policy_is_item_selected(struct xccdf_policy *policy, const char *id)
{
	const bool *tmp = (const bool*) oscap_htable_get(policy->selected_final, id);
	if (tmp	== NULL) {
		/* This shall really never happen. All valid IDs of any
		 * xccdf:Item shall be stored in the dictionery. However,
		 * we shall not to segfault. */
		assert(false);
		return false;
	}
	return *tmp;
}

int xccdf_policy_get_selected_rules_count(struct xccdf_policy *policy)
{
	int ret = 0;
	struct xccdf_policy_model *model = xccdf_policy_get_model(policy);
	struct xccdf_benchmark *benchmark = xccdf_policy_model_get_benchmark(model);
	struct oscap_htable_iterator *it = oscap_htable_iterator_new(policy->selected_final);

	while (oscap_htable_iterator_has_more(it)) {
		const char *key = NULL;
		void *value = NULL;

		oscap_htable_iterator_next_kv(it, &key, &value);

		if (!value || !*(bool*)value)
			continue;

		if (!key)
			continue;

		struct xccdf_item *item = xccdf_benchmark_get_member(benchmark, XCCDF_ITEM, key);

		if (!item)
			continue;

		if (xccdf_item_get_type(item) == XCCDF_RULE)
			++ret;
	}

	oscap_htable_iterator_free(it);
	return ret;
}

static struct xccdf_rule_result * _xccdf_rule_result_new_from_rule(const struct xccdf_policy *policy, const struct xccdf_rule *rule,
								  struct xccdf_check *check,
								  xccdf_test_result_type_t eval_result,
								  const char *message)
{
	const char* rule_id = xccdf_rule_get_id(rule);
	struct xccdf_rule_result *rule_ritem = xccdf_rule_result_new();
	struct xccdf_refine_rule_internal* r_rule = oscap_htable_get(policy->refine_rules_internal, rule_id);

	/* --Set rule-- */
	xccdf_rule_result_set_result(rule_ritem, eval_result);
	xccdf_rule_result_set_idref(rule_ritem, xccdf_rule_get_id(rule));
	xccdf_rule_result_set_weight(rule_ritem, xccdf_get_final_weight(rule, r_rule));
	xccdf_rule_result_set_version(rule_ritem, xccdf_rule_get_version(rule));
	xccdf_rule_result_set_severity(rule_ritem, xccdf_get_final_severity(rule, r_rule));
	xccdf_rule_result_set_role(rule_ritem, xccdf_get_final_role(rule, r_rule));

	xccdf_rule_result_set_time_current(rule_ritem);

	/* --Ident-- */
	struct xccdf_ident_iterator * ident_it = xccdf_rule_get_idents(rule);
	while (xccdf_ident_iterator_has_more(ident_it)){
		struct xccdf_ident * ident = xccdf_ident_iterator_next(ident_it);
		xccdf_rule_result_add_ident(rule_ritem, xccdf_ident_clone(ident));
	}
	xccdf_ident_iterator_free(ident_it);
	if (check != NULL) {
		xccdf_rule_result_add_check(rule_ritem, check);
	} else {
		/* Workaround: No applicable or candidate check was found. However,
		 * the ARF schematron requires us to include at least one check.
		 *
		 * For more info see thread on xccdf-dev mailing list named:
		 *     [Xccdf-dev] xccdf:rule-result element properties
		 */
		struct xccdf_check_iterator *check_it = xccdf_rule_get_checks(rule);
		while (xccdf_check_iterator_has_more(check_it)) {
			struct xccdf_check *orig_check = xccdf_check_iterator_next(check_it);
			struct xccdf_check *new_check = xccdf_check_clone(orig_check);
			xccdf_rule_result_add_check(rule_ritem, new_check);
		}
		xccdf_check_iterator_free(check_it);

	}

	if (message != NULL) {
		struct xccdf_message *msg = xccdf_message_new();
		assume_ex(xccdf_message_set_content(msg, message), rule_ritem);
		assume_ex(xccdf_message_set_severity(msg, XCCDF_MSG_INFO), rule_ritem);
		assume_ex(xccdf_rule_result_add_message(rule_ritem, msg), rule_ritem);
	}
	return rule_ritem;
}

static int _xccdf_policy_report_rule_result(struct xccdf_policy *policy,
					    struct xccdf_result *result,
					    const struct xccdf_rule *rule,
					    struct xccdf_check *check,
					    int res,
					    const char *message)
{
	struct xccdf_rule_result * rule_result = NULL;
	int ret=0;

	if (res == -1)
		return res;

	if (result != NULL) {
		/* Add result to policy */
		/* TODO: instance */
		rule_result = _xccdf_rule_result_new_from_rule(policy, rule, check, res, message);
		xccdf_result_add_rule_result(result, rule_result);
	} else
		xccdf_check_free(check);

	ret = xccdf_policy_report_cb(policy, XCCDF_POLICY_OUTCB_END, (void *) rule_result);
	return ret;
}

struct cpe_check_cb_usr
{
	struct xccdf_policy_model* model;
	struct cpe_dict_model* dict;
	struct cpe_lang_model* lang_model;
};

static bool _xccdf_policy_cpe_check_cb(const char* sys, const char* href, const char* name, void* usr)
{
	// FIXME: Check that sys is OVAL

	struct cpe_check_cb_usr* cb_usr = (struct cpe_check_cb_usr*)usr;

	struct xccdf_policy_model* model = cb_usr->model;
	struct cpe_dict_model* dict = cb_usr->dict;
	struct cpe_lang_model* lang_model = cb_usr->lang_model;

	char* prefixed_href = NULL;

	if (dict != NULL || lang_model != NULL)
	{
		char* origin_file = NULL;
		const char* origin_file_c = NULL;

		if (dict != NULL)
		{
			// the href path is relative to the CPE dictionary, we need to figure out
			// a "prefixed path" to deal with the case where CPE dict is not in CWD
			origin_file_c = cpe_dict_model_get_origin_file(dict);
		}
		else
		{
			// the href path is relative to the CPE2 dictionary, we need to figure out
			// a "prefixed path" to deal with the case where CPE2 dict is not in CWD
			origin_file_c = cpe_lang_model_get_origin_file(lang_model);
		}

		// we need to strdup because dirname potentially alters the string
		origin_file = oscap_strdup(origin_file_c ? origin_file_c : "");
		const char* prefix_dirname = dirname(origin_file);
		prefixed_href = oscap_sprintf("%s/%s", prefix_dirname, href);
		oscap_free(origin_file);
	}

	struct oval_agent_session* session = (struct oval_agent_session*)oscap_htable_get(model->cpe_oval_sessions, prefixed_href);

	if (session == NULL)
	{
		struct oval_definition_model* oval_model = oval_definition_model_import(prefixed_href);
		if (oval_model == NULL)
		{
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't import OVAL definition model '%s' for CPE applicability checking", prefixed_href);
			oscap_free(prefixed_href);
			return false;
		}

		session = oval_agent_new_session(oval_model, prefixed_href);
		if (session == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Cannot create OVAL session for '%s' for CPE applicability checking", prefixed_href);
			oscap_free(prefixed_href);
			return false;
		}
		oscap_htable_add(model->cpe_oval_sessions, prefixed_href, session);
	}
	oscap_free(prefixed_href);

	oval_agent_eval_definition(session, name);
	oval_result_t result = OVAL_RESULT_NOT_EVALUATED;
	if (oval_agent_get_definition_result(session, name, &result) != 0)
	{
		// error message should already be set in the function
	}

	return result == OVAL_RESULT_TRUE;
}

static bool _xccdf_policy_cpe_dict_cb(struct cpe_name* name, void* usr)
{
	struct cpe_check_cb_usr* cb_usr = (struct cpe_check_cb_usr*)usr;

	// We have to check all CPE1 dicts in the model.
	// cb_usr->dict has nothing to do with this callback! Do NOT touch it here.

	struct xccdf_policy_model* model = cb_usr->model;
	struct xccdf_benchmark* benchmark = xccdf_policy_model_get_benchmark(model);

	bool ret = false;

	struct cpe_dict_model* embedded_dict = xccdf_benchmark_get_cpe_list(benchmark);
	if (embedded_dict != NULL) {
		ret = cpe_name_applicable_dict(name, embedded_dict, (cpe_check_fn) _xccdf_policy_cpe_check_cb, usr);
	}
	if (ret)
		return true;

	struct oscap_iterator* dicts = oscap_iterator_new(model->cpe_dicts);
	while (!ret && oscap_iterator_has_more(dicts)) {
		struct cpe_dict_model *dict = (struct cpe_dict_model*)oscap_iterator_next(dicts);
		ret = cpe_name_applicable_dict(name, dict, (cpe_check_fn) _xccdf_policy_cpe_check_cb, usr);
	}
	oscap_iterator_free(dicts);
	return ret;
}

static bool xccdf_policy_model_platforms_are_applicable_dict(struct xccdf_policy_model *model, struct cpe_dict_model *dict, struct oscap_string_iterator *platforms)
{
	// at this point we know that the item has 1 or more platforms specified
	bool ret = false;

	while (oscap_string_iterator_has_more(platforms))
	{
		const char* platform = oscap_string_iterator_next(platforms);
		// Platform could be a reference to CPE2 platform, skip the ones
		// that aren't valid CPE names.
		if (!cpe_name_check(platform))
			continue;

		struct cpe_name* name = cpe_name_new(platform);

		struct cpe_check_cb_usr* usr = oscap_alloc(sizeof(struct cpe_check_cb_usr));
		usr->model = model;
		usr->dict = dict;
		usr->lang_model = NULL;
		const bool applicable = cpe_name_applicable_dict(name, dict, (cpe_check_fn) _xccdf_policy_cpe_check_cb, usr);
		oscap_free(usr);

		cpe_name_free(name);

		if (applicable)
		{
			ret = true;

			if (oscap_htable_get(model->cpe_applicable_platforms, platform) == NULL) {
				oscap_htable_add(model->cpe_applicable_platforms, platform, 0);
			}
		}
	}
	oscap_string_iterator_reset(platforms);

	return ret;
}

static bool xccdf_policy_model_platforms_are_applicable_lang_model(struct xccdf_policy_model *model, struct cpe_lang_model *lang_model, struct oscap_string_iterator *platforms)
{
	// at this point we know that the item has 1 or more platforms specified
	bool ret = false;

	while (oscap_string_iterator_has_more(platforms))
	{
		const char* platform = oscap_string_iterator_next(platforms);
		// Specification says that platform should begin with "#" if it is
		// a reference to a CPE2 platform. However content exists where this
		// is not strictly followed so we support both with and without "#"
		// references.

		const char* platform_shifted = platform;
		if (strlen(platform_shifted) >= 1 && *platform_shifted == '#')
		{
			// skip the "#" character
			platform_shifted++;
		}

		struct cpe_check_cb_usr* usr = oscap_alloc(sizeof(struct cpe_check_cb_usr));
		usr->model = model;
		usr->dict = NULL;
		usr->lang_model = lang_model;
		const bool applicable = cpe_platform_applicable_lang_model(platform_shifted, lang_model, (cpe_check_fn)_xccdf_policy_cpe_check_cb, (cpe_dict_fn)_xccdf_policy_cpe_dict_cb, usr);
		oscap_free(usr);

		if (applicable)
		{
			ret = true;

			if (oscap_htable_get(model->cpe_applicable_platforms, platform) == NULL) {
				oscap_htable_add(model->cpe_applicable_platforms, platform, 0);
			}
		}
	}
	oscap_string_iterator_reset(platforms);

	return ret;
}

bool xccdf_policy_model_platforms_are_applicable(struct xccdf_policy_model *model, struct oscap_string_iterator *platforms)
{
	// we have to check whether the item has any platforms at all, if it has none
	// it should be applicable to all platforms
	if (!oscap_string_iterator_has_more(platforms))
		return true;

	bool ret = false;
	// We do not check whether the platform entries are valid platform refs
	// or CPE names. We let the policy_model methods do that instead.
	// Therefore we check all 4 (!) places where a platform may match.
	// CPE2 takes precedence over CPE1 in this implementation. This is not
	// dictated by the specification, it's an arbitrary choice.
	struct xccdf_benchmark* benchmark = xccdf_policy_model_get_benchmark(model);
	struct cpe_lang_model *embedded_lang_model = xccdf_benchmark_get_cpe_lang_model(benchmark);
	if (embedded_lang_model != NULL) {
		if (xccdf_policy_model_platforms_are_applicable_lang_model(model, embedded_lang_model, platforms))
			ret = true;
	}

	struct oscap_iterator *lang_models = oscap_iterator_new(model->cpe_lang_models);
	while (oscap_iterator_has_more(lang_models)) {
		struct cpe_lang_model *lang_model = (struct cpe_lang_model *) oscap_iterator_next(lang_models);
		if (xccdf_policy_model_platforms_are_applicable_lang_model(model, lang_model, platforms))
			ret = true;
	}
	oscap_iterator_free(lang_models);

	struct cpe_dict_model *embedded_dict = xccdf_benchmark_get_cpe_list(benchmark);
	if (embedded_dict != NULL) {
		if (xccdf_policy_model_platforms_are_applicable_dict(model, embedded_dict, platforms))
			ret = true;
	}

	struct oscap_iterator *dicts = oscap_iterator_new(model->cpe_dicts);
	while (oscap_iterator_has_more(dicts)) {
		struct cpe_dict_model *dict = (struct cpe_dict_model *) oscap_iterator_next(dicts);
		if (xccdf_policy_model_platforms_are_applicable_dict(model, dict, platforms))
			ret = true;
	}
	oscap_iterator_free(dicts);

	return ret;
}

bool xccdf_policy_model_item_is_applicable(struct xccdf_policy_model *model, struct xccdf_item *item)
{
	struct xccdf_item* parent = xccdf_item_get_parent(item);
	if (!parent || xccdf_policy_model_item_is_applicable(model, parent))
	{
		struct oscap_string_iterator* platforms = xccdf_item_get_platforms(item);
		bool ret = xccdf_policy_model_platforms_are_applicable(model, platforms);
		oscap_string_iterator_free(platforms);

		return ret;
	}
	else
	{
		// parent is not applicable
		return false;
	}
}

/**
 * Evaluate given check which is immediate child of the rule.
 * A possibe child checks will be evaluated by xccdf_policy_check_evaluate.
 * This duplication is needed to handle @multi-check correctly,
 * which is (in general) not predictable in any way.
 */
static inline int
_xccdf_policy_rule_evaluate(struct xccdf_policy * policy, const struct xccdf_rule *rule, struct xccdf_result *result)
{
	const char* rule_id = xccdf_rule_get_id(rule);
	const bool is_selected = xccdf_policy_is_item_selected(policy, rule_id);
	const char *message = NULL;

	int report = xccdf_policy_report_cb(policy, XCCDF_POLICY_OUTCB_START, (void *) rule);
	if (report)
		return report;

	struct xccdf_refine_rule_internal* r_rule = oscap_htable_get(policy->refine_rules_internal, rule_id);

	xccdf_role_t role = xccdf_get_final_role(rule, r_rule);
	if (role  == XCCDF_ROLE_UNCHECKED )
		return _xccdf_policy_report_rule_result(policy, result, rule, NULL, XCCDF_RESULT_NOT_CHECKED, NULL);

	if (!is_selected)
		return _xccdf_policy_report_rule_result(policy, result, rule, NULL, XCCDF_RESULT_NOT_SELECTED, NULL);

	const bool is_applicable = xccdf_policy_model_item_is_applicable(policy->model, (struct xccdf_item*)rule);
	if (!is_applicable)
		return _xccdf_policy_report_rule_result(policy, result, rule, NULL, XCCDF_RESULT_NOT_APPLICABLE, NULL);

	const struct xccdf_check *orig_check = _xccdf_policy_rule_get_applicable_check(policy, (struct xccdf_item *) rule);
	if (orig_check == NULL)
		// No candidate or applicable check found.
		return _xccdf_policy_report_rule_result(policy, result, rule, NULL, XCCDF_RESULT_NOT_CHECKED, "No candidate or applicable check found.");

	// we need to clone the check to avoid changing the original content
	struct xccdf_check *check = xccdf_check_clone(orig_check);
	if (xccdf_check_get_complex(check))
		return _xccdf_policy_report_rule_result(policy, result, rule, check, xccdf_policy_check_evaluate(policy, check), NULL);

	// Now we are evaluating single simple xccdf:check within xccdf:rule.
	// Since the fact that a check will yield multi-check is not predictable in general
	// we will evaluate the check here. (A link between rule and its only check is
	// somewhat tigher that the one between checks in complex-check tree).
	//
	// Important: if touching this code, please revisit also xccdf_policy_check_evaluate.
	const char *system_name = xccdf_check_get_system(check);
	struct oscap_list *bindings = xccdf_policy_check_get_value_bindings(policy, xccdf_check_get_exports(check));
	if (bindings == NULL)
		return _xccdf_policy_report_rule_result(policy, result, rule, check, XCCDF_RESULT_UNKNOWN, "Value bindings not found.");


	struct xccdf_check_content_ref_iterator *content_it = xccdf_check_get_content_refs(check);
	struct xccdf_check_content_ref *content;
	const char *content_name;
	const char *href;
	int ret = XCCDF_RESULT_NOT_CHECKED; // initialized for the case of no check-content-refs present
	while (xccdf_check_content_ref_iterator_has_more(content_it)) {
		message = NULL;
		content = xccdf_check_content_ref_iterator_next(content_it);
		content_name = xccdf_check_content_ref_get_name(content);
		href = xccdf_check_content_ref_get_href(content);

		if (content_name == NULL && xccdf_check_get_multicheck(check)) {
			// parent element is Rule, @multi-check is required
			struct oscap_stringlist *names = _xccdf_policy_get_namesfor_href(policy, system_name, href);
			if (names != NULL) {
				// multi-check is supported by checking-engine
				struct oscap_string_iterator *name_it = oscap_stringlist_get_strings(names);
				if (!oscap_string_iterator_has_more(name_it)) {
					// Super special case when oval file contains no definitions
					// thus multi-check shall yield zero rule-results.
					report = _xccdf_policy_report_rule_result(policy, result, rule, check, XCCDF_RESULT_UNKNOWN, "No definitions found for @multi-check.");
					oscap_string_iterator_free(name_it);
					oscap_stringlist_free(names);
					xccdf_check_content_ref_iterator_free(content_it);
					oscap_list_free(bindings, (oscap_destruct_func) xccdf_value_binding_free);
					return report;
				}
				while (oscap_string_iterator_has_more(name_it)) {
					const char *name = oscap_string_iterator_next(name_it);
					struct xccdf_check *cloned_check = xccdf_check_clone(check);
					xccdf_check_inject_content_ref(cloned_check, content, name);
					int inner_ret = xccdf_policy_check_evaluate(policy, cloned_check);
					if (inner_ret == -1) {
						xccdf_check_free(cloned_check);
						report = inner_ret;
						break;
					}
					if ((report = _xccdf_policy_report_rule_result(policy, result, rule, cloned_check, inner_ret, NULL)) != 0)
						break;
					if (oscap_string_iterator_has_more(name_it))
						if ((report = xccdf_policy_report_cb(policy, XCCDF_POLICY_OUTCB_START, (void *) rule)) != 0)
							break;
				}
				oscap_string_iterator_free(name_it);
				oscap_stringlist_free(names);
				xccdf_check_content_ref_iterator_free(content_it);
				oscap_list_free(bindings, (oscap_destruct_func) xccdf_value_binding_free);
				xccdf_check_free(check);
				return report;
			}
			else
				message = "Checking engine does not support multi-check; falling back to multi-check='false'";
		}

		struct xccdf_check_import_iterator *check_import_it = xccdf_check_get_imports(check);
		ret = xccdf_policy_evaluate_cb(policy, system_name, content_name, href, bindings, check_import_it);
		// the evaluation has filled check imports at this point, we can simply free the iterator
		xccdf_check_import_iterator_free(check_import_it);

		// the content references are basically alternatives according to the specification
		// we should go through them in the order they are defined and we are done as soon
		// as we process one of them successfully
		if ((xccdf_test_result_type_t) ret != XCCDF_RESULT_NOT_CHECKED) {
			xccdf_check_inject_content_ref(check, content, NULL);
			break;
		}
	}
	if ((xccdf_test_result_type_t) ret == XCCDF_RESULT_NOT_CHECKED)
		message = "None of the check-content-ref elements was resolvable.";
	xccdf_check_content_ref_iterator_free(content_it);
	oscap_list_free(bindings, (oscap_destruct_func) xccdf_value_binding_free);
	/* Negate only once */
	ret = _resolve_negate(ret, check);
	return _xccdf_policy_report_rule_result(policy, result, rule, check, ret, message);
}

/** 
 * Evaluate the XCCDF item. If it is group, start recursive cycle, otherwise get XCCDF check
 * and evaluate it.
 * Name collision with xccdf_item -> changed to xccdf_policy_item 
 */
static int xccdf_policy_item_evaluate(struct xccdf_policy * policy, struct xccdf_item * item, struct xccdf_result * result)
{
    struct xccdf_item_iterator      * child_it;
    struct xccdf_item               * child;
    int ret = 0;

    xccdf_type_t itype = xccdf_item_get_type(item);

    switch (itype) {
        case XCCDF_RULE:{
			return _xccdf_policy_rule_evaluate(policy, (struct xccdf_rule *) item, result);
        } break;

        case XCCDF_GROUP:{
			child_it = xccdf_group_get_content((const struct xccdf_group *)item);
			while (xccdf_item_iterator_has_more(child_it)) {
				child = xccdf_item_iterator_next(child_it);
				ret = xccdf_policy_item_evaluate(policy, child, result);
				if (ret != 0)
					break;
			}
			xccdf_item_iterator_free(child_it);
        } break;
        
        default: 
                    /* TODO: set warning bad argument and return ? */
			assert(false);
			return -1;
            break;
    } 

    return ret;
}

static struct xccdf_default_score * xccdf_item_get_default_score(struct xccdf_item * item, struct xccdf_result * test_result)
{

	struct xccdf_default_score  * score;
	struct xccdf_default_score  * ch_score;
	struct xccdf_rule_result    * rule_result;
	struct xccdf_item           * child;

	xccdf_type_t itype = xccdf_item_get_type(item);

	switch (itype) {
        case XCCDF_RULE: {
		/* Rule */
		const char *rule_id = xccdf_rule_get_id((const struct xccdf_rule *) item);
		rule_result = xccdf_result_get_rule_result_by_id(test_result, rule_id);
		if (rule_result == NULL) {
			dE("Rule result ID(%s) not fount", rule_id);
			return NULL;
		}

		/* Ignore these rules */
		if ((xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_SELECTED) ||
		    (xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_APPLICABLE) ||
		    (xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_INFORMATIONAL) ||
		    (xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_CHECKED))
			    return NULL;

		score = oscap_alloc(sizeof(struct xccdf_default_score));

		/* Count with this rule */
		score->count = 1;

		/* If the test result is 'pass', assign the node a score of 100, otherwise assign a score of 0 */
		if ((xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_PASS) ||
		    (xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_FIXED))
			score->score = 100.0;
		else
			score->score = 0.0;

		/* Default weight */
		score->weight_score = score->score * xccdf_item_get_weight(item);
        } break;

        case XCCDF_BENCHMARK:
        case XCCDF_GROUP: {
		/* Init */
		score = oscap_alloc(sizeof(struct xccdf_default_score));
		score->count = 0;
		score->score = 0.0;
		score->accumulator = 0.0;

		/* Recurse */
		struct xccdf_item_iterator * child_it;
		if (itype == XCCDF_GROUP)
			child_it = xccdf_group_get_content((const struct xccdf_group *)item);
		else
			child_it = xccdf_benchmark_get_content((const struct xccdf_benchmark *)item);

		while (xccdf_item_iterator_has_more(child_it)) {
			child = xccdf_item_iterator_next(child_it);
			ch_score = xccdf_item_get_default_score(child, test_result);

			if (ch_score == NULL) /* we got item that can't be processed */
				continue;

			if (ch_score->count == 0) {  /* we got item that has no selected items */
				oscap_free(ch_score);
				continue;
			}

			/* If child's count value is not 0, then add the child's wighted score to this node's score */
			score->score += ch_score->weight_score;
			score->count++;
			score->accumulator += xccdf_item_get_weight(child);

			oscap_free(ch_score);
		}

		/* Normalize */
		if (score->count && score->accumulator)
			score->score = score->score / score->accumulator;
		/* Default weight */
                score->weight_score = score->score * xccdf_item_get_weight(item);

		xccdf_item_iterator_free(child_it);
	} break;

        default: {
		dE("Unsupported item type: %d", itype);
		score=NULL;
	} break;

	} /* switch */

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
		const char *rule_id = xccdf_rule_get_id((const struct xccdf_rule *) item);
		rule_result = xccdf_result_get_rule_result_by_id(test_result, rule_id);
		if (rule_result == NULL) {
			dE("Rule result ID(%s) not fount", rule_id);
			return NULL;
		}

		/* Ignore these rules */
		if ((xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_SELECTED) ||
		    (xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_APPLICABLE) ||
		    (xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_INFORMATIONAL) ||
		    (xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_CHECKED))
			    return NULL;

		score = oscap_alloc(sizeof(struct xccdf_flat_score));

		/* max possible score = sum of weights*/
		if (unweighted)
			score->weight = 1.0;
		else score->weight =
			xccdf_item_get_weight(item);

		/* score = sum of weights of rules that pass */
		if ((xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_PASS) ||
		    (xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_FIXED)) {
			if (unweighted)
				score->score = 1.0;
			else
				score->score = xccdf_item_get_weight(item);
		} else
			score->score = 0.0;	/* fail */
	} break;
        case XCCDF_BENCHMARK:
        case XCCDF_GROUP:{
		/* Init */
		score = oscap_alloc(sizeof(struct xccdf_flat_score));
		score->score = 0;
		score->weight = 0.0;

		/* Recurse */
		struct xccdf_item_iterator * child_it;
		if (itype == XCCDF_GROUP)
			child_it = xccdf_group_get_content((const struct xccdf_group *)item);
		else
			child_it = xccdf_benchmark_get_content((const struct xccdf_benchmark *)item);

		while (xccdf_item_iterator_has_more(child_it)) {
			child = xccdf_item_iterator_next(child_it);
			ch_score = xccdf_item_get_flat_score(child, test_result, unweighted);

			if (ch_score == NULL) /* we got item that can't be processed */
				continue;

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

        default: {
		dE("Unsupported item type: %d", itype);
		score=NULL;
	} break;

	} /* switch */

	return score;
}

struct oscap_file_entry {
	char* system_name;
	char* file;
};

struct oscap_file_entry *oscap_file_entry_new(void)
{
	struct oscap_file_entry *ret = oscap_calloc(1, sizeof(struct oscap_file_entry));
	return ret;
}

struct oscap_file_entry *oscap_file_entry_dup(struct oscap_file_entry * file_entry)
{
	struct oscap_file_entry *source = (struct oscap_file_entry *) file_entry;

	struct oscap_file_entry *ret = oscap_file_entry_new();
	ret->system_name = oscap_strdup(source->system_name);
	ret->file = oscap_strdup(source->file);

	return ret;
}

void oscap_file_entry_free(struct oscap_file_entry * entry)
{
	oscap_free(entry->system_name);
	oscap_free(entry->file);
	oscap_free(entry);
}

const char* oscap_file_entry_get_system(struct oscap_file_entry* entry)
{
	return entry->system_name;
}

const char* oscap_file_entry_get_file(struct oscap_file_entry* entry)
{
	return entry->file;
}

const struct oscap_file_entry *oscap_file_entry_iterator_next(struct oscap_file_entry_iterator *it)
{
	return oscap_iterator_next((struct oscap_iterator *)it);
}

bool oscap_file_entry_iterator_has_more(struct oscap_file_entry_iterator *it)
{
	return oscap_iterator_has_more((struct oscap_iterator *)it);
}

void oscap_file_entry_iterator_free(struct oscap_file_entry_iterator *it)
{
	oscap_iterator_free((struct oscap_iterator *)it);
}

void oscap_file_entry_iterator_reset(struct oscap_file_entry_iterator *it)
{
	oscap_iterator_reset((struct oscap_iterator *)it);
}

struct oscap_file_entry_list* oscap_file_entry_list_new(void)
{
	return (struct oscap_file_entry_list *) oscap_list_new();
}

static void oscap_file_entry_list_item_destructor(void* item)
{
	oscap_file_entry_free(item);
}

void oscap_file_entry_list_free(struct oscap_file_entry_list* list)
{
	oscap_list_free((struct oscap_list *) list, oscap_file_entry_list_item_destructor);
}

struct oscap_file_entry_iterator* oscap_file_entry_list_get_files(struct oscap_file_entry_list* list)
{
	return (struct oscap_file_entry_iterator *) oscap_iterator_new((struct oscap_list *) list);
}

static bool xccdf_file_entry_cmp_func(void *e1, void *e2)
{
	struct oscap_file_entry *entry1 = (struct oscap_file_entry *) e1;
	struct oscap_file_entry *entry2 = (struct oscap_file_entry *) e2;

	if (oscap_strcmp(entry1->system_name, entry2->system_name))
		return false;

	if (oscap_strcmp(entry1->file, entry2->file))
		return false;

	return true;
}

static struct oscap_file_entry_list * xccdf_check_get_systems_and_files(struct xccdf_check * check)
{
    struct xccdf_check_iterator             * child_it;
    struct xccdf_check                      * child;
    struct xccdf_check_content_ref_iterator * content_it;
    struct xccdf_check_content_ref          * content;
    struct oscap_file_entry					* file_entry;
    char									* href;
    char									* system_name;
    struct oscap_file_entry_list            * files;
    struct oscap_file_entry_list            * sub_files;

    system_name = (char *) xccdf_check_get_system(check);

    files = oscap_file_entry_list_new();
    if (xccdf_check_get_complex(check)) {
        child_it = xccdf_check_get_children(check);
        while (xccdf_check_iterator_has_more(child_it)) {
            child = xccdf_check_iterator_next(child_it);
            sub_files = xccdf_check_get_systems_and_files(child);

            struct oscap_file_entry_iterator *file_it = oscap_file_entry_list_get_files(sub_files);
            while (oscap_file_entry_iterator_has_more(file_it)) {
            	file_entry = (struct oscap_file_entry *) oscap_file_entry_iterator_next(file_it);

                if (!oscap_list_contains((struct oscap_list *)files, (void *) file_entry, (oscap_cmp_func) xccdf_file_entry_cmp_func))
                    oscap_list_add((struct oscap_list *)files, oscap_file_entry_dup(file_entry));
            }
            oscap_file_entry_iterator_free(file_it);
            oscap_file_entry_list_free(sub_files);
        }
        xccdf_check_iterator_free(child_it);
    } else {
        content_it = xccdf_check_get_content_refs(check);
        while (xccdf_check_content_ref_iterator_has_more(content_it)) {
            content = xccdf_check_content_ref_iterator_next(content_it);
            href = (char *) xccdf_check_content_ref_get_href(content);

            file_entry = (struct oscap_file_entry *) oscap_file_entry_new();
            file_entry->system_name = oscap_strdup(system_name);
            file_entry->file = oscap_strdup(href);

            if (!oscap_list_contains((struct oscap_list *) files, file_entry, (oscap_cmp_func) xccdf_file_entry_cmp_func))
                oscap_list_add((struct oscap_list *) files, (void *) file_entry);
            else
            	oscap_file_entry_free((void *) file_entry);
        }
        xccdf_check_content_ref_iterator_free(content_it);
    }

    return files;
}

struct oscap_file_entry_list * xccdf_item_get_systems_and_files(struct xccdf_item * item)
{

    struct xccdf_item_iterator  * child_it;
    struct xccdf_item           * child;
    struct xccdf_check_iterator * check_it;
    struct xccdf_check          * check;
    struct oscap_file_entry_list * files;
    struct oscap_file_entry_list * sub_files;
    struct oscap_file_entry     * file_entry;

    xccdf_type_t itype = xccdf_item_get_type(item);
    files = oscap_file_entry_list_new();

    switch (itype) {
        case XCCDF_RULE:
            check_it = xccdf_rule_get_checks((struct xccdf_rule *) item);
            while(xccdf_check_iterator_has_more(check_it)) {
                check = xccdf_check_iterator_next(check_it);
                sub_files = xccdf_check_get_systems_and_files(check);

                struct oscap_file_entry_iterator *file_it = oscap_file_entry_list_get_files(sub_files);
                while (oscap_file_entry_iterator_has_more(file_it)) {
                    file_entry = (struct oscap_file_entry *) oscap_file_entry_iterator_next(file_it);
                    if (!oscap_list_contains((struct oscap_list *) files, file_entry, (oscap_cmp_func) xccdf_file_entry_cmp_func))
                        oscap_list_add((struct oscap_list *) files, oscap_file_entry_dup(file_entry));
                }
                oscap_file_entry_iterator_free(file_it);
                oscap_file_entry_list_free(sub_files);
            }
            xccdf_check_iterator_free(check_it);
            break;

        case XCCDF_BENCHMARK:
        case XCCDF_GROUP:
            if (itype == XCCDF_GROUP) child_it = xccdf_group_get_content((const struct xccdf_group *)item);
            else child_it = xccdf_benchmark_get_content((const struct xccdf_benchmark *)item);
            while (xccdf_item_iterator_has_more(child_it)) {
                    child = xccdf_item_iterator_next(child_it);
                    sub_files = xccdf_item_get_systems_and_files(child);

                    struct oscap_file_entry_iterator *file_it = oscap_file_entry_list_get_files(sub_files);
                    while (oscap_file_entry_iterator_has_more(file_it)) {
                    	file_entry = (struct oscap_file_entry *) oscap_file_entry_iterator_next(file_it);

                        if (!oscap_list_contains((struct oscap_list *) files, file_entry, (oscap_cmp_func) xccdf_file_entry_cmp_func))
                            oscap_list_add((struct oscap_list *) files, oscap_file_entry_dup(file_entry));
                    }
                    oscap_file_entry_iterator_free(file_it);
                    oscap_file_entry_list_free(sub_files);
            }
            xccdf_item_iterator_free(child_it);
        break;
        
        default: 
            oscap_file_entry_list_free(files);
            files = NULL;
            break;
    } 

    return files;
}

static bool xccdf_cmp_func(const char *s1, const char *s2)
{
    return !oscap_strcmp(s1, s2);
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
            oscap_stringlist_free(sub_names);
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

struct oscap_stringlist * xccdf_item_get_files(struct xccdf_item * item)
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
                oscap_stringlist_free(sub_names);
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
                    oscap_stringlist_free(sub_names);
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

bool xccdf_policy_model_set_tailoring(struct xccdf_policy_model *model, struct xccdf_tailoring *tailoring)
{
	// Clear cached policies, because we (might) have to resolve differently
	// with Tailoring element in place.
	oscap_list_free(model->policies, (oscap_destruct_func) xccdf_policy_free);
	model->policies = oscap_list_new();

	xccdf_tailoring_free(model->tailoring);
	model->tailoring = tailoring;

	xccdf_tailoring_resolve(tailoring, xccdf_policy_model_get_benchmark(model));

	return true;
}

struct xccdf_tailoring *xccdf_policy_model_get_tailoring(struct xccdf_policy_model *model)
{
	return model->tailoring;
}

bool xccdf_policy_model_add_cpe_dict(struct xccdf_policy_model *model, const char * cpe_dict)
{
		__attribute__nonnull__(model);
		__attribute__nonnull__(cpe_dict);

		struct cpe_dict_model* dict = cpe_dict_model_import(cpe_dict);
		return oscap_list_add(model->cpe_dicts, dict);
}

bool xccdf_policy_model_add_cpe_lang_model(struct xccdf_policy_model *model, const char * cpe_lang)
{
		__attribute__nonnull__(model);
		__attribute__nonnull__(cpe_lang);

		struct cpe_lang_model* lang_model = cpe_lang_model_import(cpe_lang);
		return oscap_list_add(model->cpe_lang_models, lang_model);
}

bool xccdf_policy_model_add_cpe_autodetect(struct xccdf_policy_model *model, const char* filepath)
{
	oscap_document_type_t doc_type = 0;
	if (oscap_determine_document_type(filepath, &doc_type) != 0) {
		oscap_seterr(OSCAP_EFAMILY_XCCDF, "Encountered issues when detecting document "
		                                  "type of '%s'.", filepath);
		return false;
	}

	if (doc_type == OSCAP_DOCUMENT_CPE_DICTIONARY) {
		return xccdf_policy_model_add_cpe_dict(model, filepath);
	}
	else if (doc_type == OSCAP_DOCUMENT_CPE_LANGUAGE) {
		return xccdf_policy_model_add_cpe_lang_model(model, filepath);
	}

	oscap_seterr(OSCAP_EFAMILY_XCCDF, "File '%s' wasn't detected as either CPE dictionary or "
	                                  "CPE lang model. Can't register it to the XCCDF policy model.", filepath);
	return false;
}

struct oscap_htable_iterator *xccdf_policy_model_get_cpe_oval_sessions(struct xccdf_policy_model *model)
{
	return oscap_htable_iterator_new(model->cpe_oval_sessions);
}

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
OSCAP_DEPRECATED(
bool xccdf_policy_model_register_engine_callback(struct xccdf_policy_model * model, char * sys, void * func, void * usr)
{
	return xccdf_policy_model_register_engine_and_query_callback(model, sys, func, usr, NULL);
}
)

bool
xccdf_policy_model_register_engine_and_query_callback(struct xccdf_policy_model *model, char *sys, xccdf_policy_engine_eval_fn eval_fn, void *usr, xccdf_policy_engine_query_fn query_fn)
{
        __attribute__nonnull__(model);
	struct xccdf_policy_engine *engine = xccdf_policy_engine_new(sys, eval_fn, usr, query_fn);
	return oscap_list_add(model->engines, engine);
}

void xccdf_policy_model_unregister_engines(struct xccdf_policy_model *model, const char *sys)
{
	__attribute__nonnull__(model);
	if (sys == NULL)
		oscap_list_free(model->engines, (oscap_destruct_func) oscap_free);
	else {
		struct oscap_list *rest = oscap_list_new();
		struct oscap_iterator *cb_it = oscap_iterator_new(model->engines);
		while (oscap_iterator_has_more(cb_it)) {
			struct xccdf_policy_engine *engine = oscap_iterator_next(cb_it);
			if (xccdf_policy_engine_filter(engine, sys))
				oscap_free(engine);
			else
				oscap_list_add(rest, engine);
		}
		oscap_iterator_free(cb_it);
		oscap_list_free0(model->engines);
		model->engines = rest;
	}
}

bool xccdf_policy_model_register_start_callback(struct xccdf_policy_model * model, policy_reporter_start func, void * usr)
{

        __attribute__nonnull__(model);
	struct reporter *reporter = reporter_new(XCCDF_POLICY_OUTCB_START, func, usr);
        return oscap_list_add(model->callbacks, reporter);
}

bool xccdf_policy_model_register_output_callback(struct xccdf_policy_model * model, policy_reporter_output func, void * usr)
{

        __attribute__nonnull__(model);
	struct reporter *reporter = reporter_new(XCCDF_POLICY_OUTCB_END, func, usr);
        return oscap_list_add(model->callbacks, reporter);
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

static bool xccdf_policy_model_add_default_cpe(struct xccdf_policy_model* model);

/**
 * New XCCDF Policy model. Create new structure and fill the policies list with 
 * policy entries that are inherited from XCCDF benchmark Profile elements. For each 
 * profile element call xccdf_policy_new function.
 */
struct xccdf_policy_model * xccdf_policy_model_new(struct xccdf_benchmark * benchmark) {

	__attribute__nonnull__(benchmark);

	struct xccdf_policy_model       * model;

	model = oscap_alloc(sizeof(struct xccdf_policy_model));
	if (model == NULL)
		return NULL;
	memset(model, 0, sizeof(struct xccdf_policy_model));

	model->benchmark = benchmark;
	model->tailoring = NULL;
	model->policies  = oscap_list_new();
        model->callbacks = oscap_list_new();
	model->engines = oscap_list_new();

	model->cpe_dicts = oscap_list_new();
	model->cpe_lang_models = oscap_list_new();
	model->cpe_oval_sessions = oscap_htable_new();
	model->cpe_applicable_platforms = oscap_htable_new();

	if (!xccdf_policy_model_add_default_cpe(model))
	{
		oscap_seterr(OSCAP_EFAMILY_XCCDF, "Failed to add default CPE to newly created XCCDF policy model.");
	}

        /* Resolve document */
        xccdf_benchmark_resolve(benchmark);
	return model;
}

static bool xccdf_policy_model_add_default_cpe(struct xccdf_policy_model* model)
{
	char* cpe_dict_path = oscap_sprintf("%s/openscap-cpe-dict.xml", oscap_path_to_cpe());
	const bool ret = xccdf_policy_model_add_cpe_dict(model, cpe_dict_path);
	oscap_free(cpe_dict_path);

	return ret;
}

static inline bool
_xccdf_policy_add_selector_internal(struct xccdf_policy *policy, struct xccdf_benchmark *benchmark, struct xccdf_select *sel, bool resolve)
{
	/* This is the only way, how one can add selector to a policy. */
	bool result = oscap_list_add(policy->selects, sel);

	struct xccdf_item *item = xccdf_benchmark_get_member(benchmark, XCCDF_ITEM, xccdf_select_get_item(sel));
	if (item != NULL) {
		/* If selector points to a single item. Update internal dictionary. */
		oscap_htable_detach(policy->selected_internal, xccdf_select_get_item(sel));
		result &= oscap_htable_add(policy->selected_internal, xccdf_select_get_item(sel), sel);
		if (resolve) {
			const struct xccdf_item *parent = xccdf_item_get_parent(item);
			/* If we are adding a selector to a top-level XCCDF Group (its parent is the Benchmark),
			we have to consider the parent selected even though it is not in
			the final selected hashmap. XCCDF Benchmark can't be unselected. */
			xccdf_policy_resolve_item(policy, item, (parent == NULL || xccdf_item_get_type(parent) == XCCDF_BENCHMARK) ?
				true : xccdf_policy_is_item_selected(policy, xccdf_item_get_id(parent)));
		}
		return result;
	}

	/** If the selector doesn't point to an item it can still point to the cluster. */
	struct oscap_htable_iterator *hit = xccdf_benchmark_get_cluster_items(benchmark, xccdf_select_get_item(sel));
	if (hit == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XCCDF, "Selector ID(%s) does not exist in Benchmark.", xccdf_select_get_item(sel));
		return false;
	}
	if (!oscap_htable_iterator_has_more(hit)) {
		oscap_htable_iterator_free(hit);
		return false;
	}
	while (oscap_htable_iterator_has_more(hit)) {
		const char *item_id = oscap_htable_iterator_next_key(hit);
		if (item_id == NULL) {
			assert(item_id != NULL);
			continue;
		}
		oscap_htable_detach(policy->selected_internal, item_id);
		result &= oscap_htable_add(policy->selected_internal, item_id, sel);
	}
	if (resolve) {
		oscap_htable_iterator_reset(hit);
		while (oscap_htable_iterator_has_more(hit)) {
			item = (struct xccdf_item *) oscap_htable_iterator_next_value(hit);
			if (item == NULL)
				continue;
			const struct xccdf_item *parent = xccdf_item_get_parent(item);
			xccdf_policy_resolve_item(policy, item, (parent == NULL || xccdf_item_get_type(parent) == XCCDF_BENCHMARK) ?
				true : xccdf_policy_is_item_selected(policy, xccdf_item_get_id(parent)));
		}
	}
	oscap_htable_iterator_free(hit);
	return result;
}

bool
xccdf_policy_add_select(struct xccdf_policy *policy, struct xccdf_select *sel)
{
	struct xccdf_benchmark *benchmark = xccdf_policy_model_get_benchmark(xccdf_policy_get_model(policy));
	return _xccdf_policy_add_selector_internal(policy, benchmark, sel, true);
}

static void _xccdf_policy_add_profile_selectors(struct xccdf_policy* policy, struct xccdf_benchmark *benchmark, struct xccdf_profile *profile) {
	if (!profile)
		return;

	const char *parent_profile_id = xccdf_profile_get_extends(profile);
	struct xccdf_profile *parent_profile = NULL;

	if (parent_profile_id != NULL) {
		if (strcmp(parent_profile_id, xccdf_profile_get_id(profile)) == 0) {
			// We are shadowing a profile, we need to get the original profile from
			// benchmark directly to avoid an endless loop.
			parent_profile = xccdf_benchmark_get_profile_by_id(benchmark, parent_profile_id);
		}
		else {
			struct xccdf_policy *parent_profile_policy = xccdf_policy_model_get_policy_by_id(xccdf_policy_get_model(policy), parent_profile_id);
			parent_profile = parent_profile_policy != NULL ? xccdf_policy_get_profile(parent_profile_policy) : NULL;
		}
	}

	// Add selectors from parent profile if any, these selectors should be added
	// before selectors from the "real" policy profile are added so that they
	// can be overridden.
	if (parent_profile)
		_xccdf_policy_add_profile_selectors(policy, benchmark, parent_profile);

	struct xccdf_select_iterator *sel_it = xccdf_profile_get_selects(profile);
	/* Iterate through selects in profile */
	while (xccdf_select_iterator_has_more(sel_it)) {
		struct xccdf_select *sel = xccdf_select_iterator_next(sel_it);
		if (sel == NULL) {
			assert(false);
			continue;
		}

		struct xccdf_select *clone = xccdf_select_clone(sel);
		_xccdf_policy_add_selector_internal(policy, benchmark, clone, false);
	}
	xccdf_select_iterator_free(sel_it);
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

	policy = oscap_alloc(sizeof(struct xccdf_policy));
	if (policy == NULL)
		return NULL;
	memset(policy, 0, sizeof(struct xccdf_policy));

	policy->profile = profile;
	policy->selects = oscap_list_new();
	policy->values  = oscap_list_new();
	policy->results = oscap_list_new();

	policy->selected_internal = oscap_htable_new();
	policy->selected_final = oscap_htable_new();
	policy->refine_rules_internal = oscap_htable_new();
	policy->model = model;

	benchmark = xccdf_policy_model_get_benchmark(model);

	if (profile) {
		_xccdf_policy_add_profile_selectors(policy, benchmark, profile);
		xccdf_policy_add_profile_refine_rules(policy, benchmark, profile);
	}

        /* Iterate through items in benchmark and resolve rules */
        item_it = xccdf_benchmark_get_content(benchmark);
        while (xccdf_item_iterator_has_more(item_it)) {
            item = xccdf_item_iterator_next(item_it);
            xccdf_policy_resolve_item(policy, item, true);
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

/**
 * If policy has the select specified by item_id return the select, NULL otherwise
 */
struct xccdf_select * xccdf_policy_get_select_by_id(struct xccdf_policy * policy, const char *item_id)
{
        __attribute__nonnull__(policy);
	return oscap_htable_get(policy->selected_internal, item_id);
}


struct xccdf_select_iterator * xccdf_policy_get_selected_rules(struct xccdf_policy * policy) {

    return (struct xccdf_select_iterator *) oscap_iterator_new_filter( policy->selects, 
                                                                       (oscap_filter_func) xccdf_policy_filter_selected, 
                                                                       policy);
}

OSCAP_DEPRECATED(
bool xccdf_policy_set_selected(struct xccdf_policy * policy, char * idref) {
	return false;
}
)

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
        if (oscap_streq(xccdf_policy_get_id(policy), id)) {
            xccdf_policy_iterator_free(policy_it);
            return policy;
        }
    }
    xccdf_policy_iterator_free(policy_it);

	struct xccdf_profile *profile = NULL;
	struct xccdf_tailoring *tailoring = xccdf_policy_model_get_tailoring(policy_model);

	// Tailoring profiles take precedence over Benchmark profiles.
	if (tailoring) {
		profile = xccdf_tailoring_get_profile_by_id(tailoring, id);
	}

	if (!profile) {
		if (id == NULL) {
			profile = xccdf_profile_new();
			xccdf_profile_set_id(profile, NULL);
			struct oscap_text * title = oscap_text_new();
			oscap_text_set_text(title, "No profile (default benchmark)");
			oscap_text_set_lang(title, "en");
			xccdf_profile_add_title(profile, title);
		}
		else {
			struct xccdf_benchmark *benchmark = xccdf_policy_model_get_benchmark(policy_model);
			if (benchmark == NULL) {
				assert(benchmark != NULL);
				return NULL;
			}
			profile = xccdf_benchmark_get_profile_by_id(benchmark, id);
			if (profile == NULL)
				return NULL;
		}
	}

	policy = xccdf_policy_new(policy_model, profile);
	if (policy != NULL)
		oscap_list_add(policy_model->policies, policy);
	return policy;
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
    r_rule_it = xccdf_profile_get_refine_rules(profile);
    while (xccdf_refine_rule_iterator_has_more(r_rule_it)) {
        r_rule = xccdf_refine_rule_iterator_next(r_rule_it);
        item = xccdf_benchmark_get_item(benchmark, xccdf_refine_rule_get_item(r_rule));
        if (item != NULL) {
            /* Proccess refine rule appliement */
            /* In r_rule we have refine rule that match  - no more then one !*/
            if (xccdf_item_get_type(item) == XCCDF_GROUP) { 
                /* Perform check of weight attribute  - ignore other attributes */
                if (xccdf_refine_rule_weight_defined(r_rule)) {
                        oscap_seterr(OSCAP_EFAMILY_XCCDF, "'Weight' attribute not specified, only 'weight' attribute applies to groups items");
                        xccdf_refine_rule_iterator_free(r_rule_it);
                        return false;            
                }
                else {
                    /* Apply the rule changes */
                    xccdf_group_set_weight((struct xccdf_group *) item, xccdf_refine_rule_get_weight(r_rule) );
                }
                
            } else if (xccdf_item_get_type(item) == XCCDF_RULE) {
                /* Perform all changes in rule */
                if ((int)xccdf_refine_rule_get_role(r_rule) > 0)
                    xccdf_rule_set_role((struct xccdf_rule *) item, xccdf_refine_rule_get_role(r_rule));
                if ((int)xccdf_refine_rule_get_severity(r_rule) != XCCDF_LEVEL_NOT_DEFINED)
                    xccdf_rule_set_severity((struct xccdf_rule *) item, xccdf_refine_rule_get_severity(r_rule));

            } else {}/* TODO oscap_err ? */;

        } else {
            oscap_seterr(OSCAP_EFAMILY_XCCDF, "Refine rule item points to nonexisting XCCDF item");
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
    struct xccdf_benchmark          * benchmark;
    int                               ret       = -1;
    const char			    * doc_version = NULL;

    __attribute__nonnull__(policy);

    /* Add result to policy */
    struct xccdf_result * result = xccdf_result_new();

	xccdf_result_set_start_time_current(result);

    /** Set ID of TestResult */
    const char * id = NULL;
	if ((xccdf_policy_get_profile(policy) != NULL) && (xccdf_profile_get_id(xccdf_policy_get_profile(policy)) != NULL)) {
		id = oscap_strdup(xccdf_profile_get_id(xccdf_policy_get_profile(policy)));
		xccdf_result_set_profile(result, id);
	}
    else
        id = oscap_strdup("default-profile");

    /* Get all constant information */
    benchmark = xccdf_policy_model_get_benchmark(xccdf_policy_get_model(policy));
    const struct xccdf_version_info* version_info = xccdf_benchmark_get_schema_version(benchmark);
    doc_version = xccdf_version_info_get_version(version_info);

#ifdef __USE_GNU
    if (strverscmp("1.2", doc_version) >= 0)
#else
    if (strcmp("1.2", doc_version) >= 0)
#endif
    {
        // we have to enforce a certain type of ids for XCCDF 1.2+

        char rid[32+strlen(id)];
        sprintf(rid, "xccdf_org.open-scap_testresult_%s", id);
        xccdf_result_set_id(result, rid);
    }
    else {

    	// previous behaviour for backwards compatibility

        char rid[11+strlen(id)];
        sprintf(rid, "OSCAP-Test-%s", id);
        xccdf_result_set_id(result, rid);
    }

    oscap_free(id);

	/** We need to process document top-down order.
	 * See conflicts/requires and Item Processing Algorithm */
	struct xccdf_item_iterator *item_it = xccdf_benchmark_get_content(benchmark);
	while (xccdf_item_iterator_has_more(item_it)) {
		struct xccdf_item *item = xccdf_item_iterator_next(item_it);
		ret = xccdf_policy_item_evaluate(policy, item, result);
		if (ret == -1) {
			xccdf_item_iterator_free(item_it);
			xccdf_result_free(result);
			return NULL;
		}
		if (ret != 0)
			break;
	}
	xccdf_item_iterator_free(item_it);

	struct oscap_htable_iterator *it = oscap_htable_iterator_new(policy->model->cpe_applicable_platforms);
	while (oscap_htable_iterator_has_more(it)) {
		const char *key = oscap_htable_iterator_next_key(it);
		xccdf_result_add_applicable_platform(result, key);
	}
	oscap_htable_iterator_free(it);

    xccdf_policy_add_result(policy, result);

	xccdf_result_set_end_time_current(result);

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


const char *xccdf_policy_get_value_of_item(struct xccdf_policy * policy, struct xccdf_item * item)
{
    struct xccdf_profile * profile = xccdf_policy_get_profile(policy);
	const char *selector = NULL;

	if (profile != NULL) {
		/* Get set_value for this item */
		struct xccdf_setvalue *s_value = NULL;
		struct xccdf_setvalue *last_s_value = NULL;
		struct xccdf_setvalue_iterator *s_value_it = xccdf_profile_get_setvalues(profile);
		while (xccdf_setvalue_iterator_has_more(s_value_it)) {
			s_value = xccdf_setvalue_iterator_next(s_value_it);
			if (strcmp(xccdf_setvalue_get_item(s_value), xccdf_value_get_id((struct xccdf_value *) item)) == 0)
				last_s_value = s_value;
		}
		xccdf_setvalue_iterator_free(s_value_it);
		if (last_s_value != NULL)
			return xccdf_setvalue_get_value(last_s_value);

		/* We don't have set-value in profile, look for refine-value */
		struct xccdf_refine_value_iterator *r_value_it = xccdf_profile_get_refine_values(profile);
		while (xccdf_refine_value_iterator_has_more(r_value_it)) {
			struct xccdf_refine_value *r_value = xccdf_refine_value_iterator_next(r_value_it);
			if (!strcmp(xccdf_refine_value_get_item(r_value), xccdf_value_get_id((struct xccdf_value *) item))) {
				selector = xccdf_refine_value_get_selector(r_value);
				break;
			}
		}
		xccdf_refine_value_iterator_free(r_value_it);
	}

	struct xccdf_value_instance *instance = xccdf_value_get_instance_by_selector((struct xccdf_value *) item, selector);
	if (instance == NULL) {
                oscap_seterr(OSCAP_EFAMILY_XCCDF, "Attempt to get non-existent selector \"%s\" from variable \"%s\"",
				selector, xccdf_value_get_id((struct xccdf_value *) item));
		return NULL;
	}
	return xccdf_value_instance_get_value(instance);
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
            struct xccdf_refine_rule_internal * r_rule = xccdf_policy_get_refine_rule_by_item(policy, item);
            if (r_rule == NULL) return item;

            new_item = (struct xccdf_item *) xccdf_rule_clone((struct xccdf_rule *) item);
            if (xccdf_refine_rule_internal_get_role(r_rule) > 0)
                xccdf_rule_set_role((struct xccdf_rule *) new_item, xccdf_refine_rule_internal_get_role(r_rule));
            if (xccdf_refine_rule_internal_get_severity(r_rule) != XCCDF_LEVEL_NOT_DEFINED)
                xccdf_rule_set_severity((struct xccdf_rule *) new_item, xccdf_refine_rule_internal_get_severity(r_rule));
            if (xccdf_weight_defined(xccdf_refine_rule_internal_get_weight(r_rule)))
                xccdf_rule_set_weight((struct xccdf_rule *) new_item, xccdf_refine_rule_internal_get_weight(r_rule));
                break;
            }
        case XCCDF_GROUP: {
        struct xccdf_refine_rule_internal * r_rule = xccdf_policy_get_refine_rule_by_item(policy, item);
            if (r_rule == NULL) return item;

            new_item = (struct xccdf_item *) xccdf_group_clone((struct xccdf_group *) item);
            if (xccdf_weight_defined(xccdf_refine_rule_internal_get_weight(r_rule)))
                xccdf_group_set_weight((struct xccdf_group *) new_item, xccdf_refine_rule_internal_get_weight(r_rule));
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
                if (!oscap_strcmp(xccdf_value_instance_get_value(instance), value))
                    selector = xccdf_value_instance_get_selector(instance);
            }
            xccdf_value_instance_iterator_free(instance_it);
            instance_it = xccdf_value_get_instances((struct xccdf_value *) new_item);
            while (xccdf_value_instance_iterator_has_more(instance_it)) {
                instance = xccdf_value_instance_iterator_next(instance_it);
                if (oscap_strcmp(xccdf_value_instance_get_selector(instance), selector))
                    xccdf_value_instance_iterator_remove(instance_it);
            }
            xccdf_value_instance_iterator_free(instance_it);
            if (selector == NULL) {
                instance = xccdf_value_get_instance_by_selector((struct xccdf_value *) new_item, NULL);
                xccdf_value_instance_set_defval_string(instance, value);
            }

            int oper = xccdf_policy_get_refine_value_oper(policy, item);
            if (oper == -1) break;
            xccdf_value_set_oper((struct xccdf_value *) item, (xccdf_operator_t) oper);
            break;
        }
        default:
            return NULL;
    }
    return new_item;
}

struct oscap_file_entry_list * xccdf_policy_model_get_systems_and_files(struct xccdf_policy_model * policy_model)
{
    return xccdf_item_get_systems_and_files((struct xccdf_item *) xccdf_policy_model_get_benchmark(policy_model));
}

struct oscap_stringlist * xccdf_policy_model_get_files(struct xccdf_policy_model * policy_model)
{
    return xccdf_item_get_files((struct xccdf_item *) xccdf_policy_model_get_benchmark(policy_model));
}

struct xccdf_benchmark *xccdf_policy_get_benchmark(const struct xccdf_policy *policy)
{
	if (policy == NULL)
		return NULL;
        const struct xccdf_policy_model *model = xccdf_policy_get_model(policy);
        if (model == NULL)
                return NULL;
        return xccdf_policy_model_get_benchmark(model);
}

static void _xccdf_policy_destroy_cpe_oval_session(void* ptr)
{
	struct oval_agent_session* session = (struct oval_agent_session*)ptr;
	struct oval_definition_model* model = oval_agent_get_definition_model(session);
	oval_agent_destroy_session(session);
	oval_definition_model_free(model);
}

void xccdf_policy_model_free(struct xccdf_policy_model * model) {

	oscap_list_free(model->policies, (oscap_destruct_func) xccdf_policy_free);
	xccdf_policy_model_unregister_engines(model, NULL);
	oscap_list_free(model->callbacks, (oscap_destruct_func) oscap_free);
	xccdf_tailoring_free(model->tailoring);
        xccdf_benchmark_free(model->benchmark);

	oscap_list_free(model->cpe_dicts, (oscap_destruct_func) cpe_dict_model_free);
	oscap_list_free(model->cpe_lang_models, (oscap_destruct_func) cpe_lang_model_free);
	oscap_htable_free(model->cpe_oval_sessions, (oscap_destruct_func) _xccdf_policy_destroy_cpe_oval_session);
	oscap_htable_free(model->cpe_applicable_platforms, NULL);
        oscap_free(model);
}



void xccdf_policy_free(struct xccdf_policy * policy) {

        /* If ID of policy's profile is NULL then this
         * profile is created by Policy layer and need
         * to be freed
         */
        if (xccdf_profile_get_id(policy->profile) == NULL)
            xccdf_profile_free((struct xccdf_item *) policy->profile);

	oscap_list_free(policy->selects, (oscap_destruct_func) xccdf_select_free);
	oscap_list_free(policy->values, (oscap_destruct_func) xccdf_value_binding_free);
	oscap_list_free(policy->results, (oscap_destruct_func) xccdf_result_free);
	oscap_htable_free0(policy->selected_internal);
	oscap_htable_free0(policy->selected_final);
	oscap_htable_free(policy->refine_rules_internal, (oscap_destruct_func) xccdf_refine_rule_internal_free);
        oscap_free(policy);
}

void xccdf_value_binding_free(struct xccdf_value_binding * binding) {

        oscap_free(binding->name);
        oscap_free(binding->value);
        oscap_free(binding->setvalue);
        oscap_free(binding);
}


