/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */


#include "item.h"
#include "helpers.h"
#include <math.h>


// constants
static const xccdf_numeric XCCDF_SCORE_MAX_DAFAULT = 100.0f;
static const char *XCCDF_INSTANCE_DEFAULT_CONTEXT = "undefined";

// prototypes
void xccdf_rule_result_free(struct xccdf_rule_result *rr);
void xccdf_identity_free(struct xccdf_identity *identity);
void xccdf_score_free(struct xccdf_score *score);
void xccdf_target_fact_free(struct xccdf_target_fact *fact);
void xccdf_message_free(struct xccdf_message *msg);
void xccdf_instance_free(struct xccdf_instance *inst);
void xccdf_override_free(struct xccdf_override *oride);

struct xccdf_result *xccdf_result_new(void)
{
	struct xccdf_item *result = xccdf_item_new(XCCDF_PROFILE, NULL);
	oscap_create_lists(&result->sub.result.identities, &result->sub.result.targets,
		&result->sub.result.remarks, &result->sub.result.target_addresses,
		&result->sub.result.target_facts, &result->sub.result.setvalues, &result->sub.result.organizations,
		&result->sub.result.rule_results, &result->sub.result.scores, NULL);
	return XRESULT(result);
}

static inline void xccdf_result_free_impl(struct xccdf_item *result)
{
	if (result != NULL) {
		xccdf_item_release(result);

		oscap_free(result->sub.result.test_system);
		oscap_free(result->sub.result.benchmark_uri);
		oscap_free(result->sub.result.profile);

		oscap_list_free(result->sub.result.identities, (oscap_destruct_func) xccdf_identity_free);
		oscap_list_free(result->sub.result.target_facts, (oscap_destruct_func) xccdf_target_fact_free);
		oscap_list_free(result->sub.result.scores, (oscap_destruct_func) xccdf_score_free);
		oscap_list_free(result->sub.result.targets, oscap_free);
		oscap_list_free(result->sub.result.remarks, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(result->sub.result.target_addresses, oscap_free);
		oscap_list_free(result->sub.result.setvalues, (oscap_destruct_func) xccdf_setvalue_free);
		oscap_list_free(result->sub.result.rule_results, (oscap_destruct_func) xccdf_rule_result_free);
		oscap_list_free(result->sub.result.organizations, oscap_free);

		oscap_free(result);
	}
}
XCCDF_FREE_GEN(result)

XCCDF_ACCESSOR_STRING(result, test_system)
XCCDF_ACCESSOR_STRING(result, benchmark_uri)
XCCDF_ACCESSOR_STRING(result, profile)
XCCDF_LISTMANIP(result, identity, identities)
XCCDF_LISTMANIP_STRING(result, target, targets)
XCCDF_LISTMANIP_STRING(result, target_address, target_addresses)
XCCDF_LISTMANIP_STRING(result, organization, organizations)
XCCDF_LISTMANIP_TEXT(result, remark, remarks)
XCCDF_LISTMANIP(result, target_fact, target_facts)
XCCDF_LISTMANIP(result, setvalue, setvalues)
XCCDF_LISTMANIP(result, rule_result, rule_results)
XCCDF_LISTMANIP(result, score, scores)


struct xccdf_rule_result *xccdf_rule_result_new(void)
{
	struct xccdf_rule_result *rr = oscap_calloc(1, sizeof(struct xccdf_rule_result));
	oscap_create_lists(&rr->overrides, &rr->idents, &rr->messages,
		&rr->instances, &rr->fixes, &rr->checks, NULL);
	return rr;
}

void xccdf_rule_result_free(struct xccdf_rule_result *rr)
{
	if (rr != NULL) {
		oscap_free(rr->idref);
		oscap_free(rr->version);

		oscap_list_free(rr->overrides, (oscap_destruct_func) xccdf_override_free);
		oscap_list_free(rr->idents, (oscap_destruct_func) xccdf_ident_free);
		oscap_list_free(rr->messages, (oscap_destruct_func) xccdf_message_free);
		oscap_list_free(rr->instances, (oscap_destruct_func) xccdf_instance_free);
		oscap_list_free(rr->fixes, (oscap_destruct_func) xccdf_fix_free);
		oscap_list_free(rr->checks, (oscap_destruct_func) xccdf_check_free);

		oscap_free(rr);
	}
}

OSCAP_ACCESSOR_SIMPLE(time_t, xccdf_rule_result, time)
OSCAP_ACCESSOR_SIMPLE(xccdf_role_t, xccdf_rule_result, role)
OSCAP_ACCESSOR_SIMPLE(float, xccdf_rule_result, weight)
OSCAP_ACCESSOR_SIMPLE(xccdf_level_t, xccdf_rule_result, severity)
OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_rule_result, result)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, version)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, idref)
OSCAP_IGETINS(xccdf_ident, xccdf_rule_result, idents, ident)
OSCAP_IGETINS(xccdf_fix, xccdf_rule_result, fixes, fix)
OSCAP_IGETINS(xccdf_check, xccdf_rule_result, checks, check)
OSCAP_IGETINS_GEN(xccdf_override, xccdf_rule_result, overrides, override)
OSCAP_IGETINS_GEN(xccdf_message, xccdf_rule_result, messages, message)
OSCAP_IGETINS_GEN(xccdf_instance, xccdf_rule_result, instances, instance)


struct xccdf_identity *xccdf_identity_new(void)
{
	return oscap_calloc(1, sizeof(struct xccdf_identity));
}

void xccdf_identity_free(struct xccdf_identity *identity)
{
	if (identity != NULL) {
		oscap_free(identity->name);
		oscap_free(identity);
	}
}

struct xccdf_score *xccdf_score_new(void)
{
	struct xccdf_score *score = oscap_calloc(1, sizeof(struct xccdf_score));
	score->score = NAN;
	score->maximum = XCCDF_SCORE_MAX_DAFAULT;
	return score;
}

void xccdf_score_free(struct xccdf_score *score)
{
	if (score != NULL) {
		oscap_free(score->system);
		oscap_free(score);
	}
}

struct xccdf_override *xccdf_override_new(void)
{
	return oscap_calloc(1, sizeof(struct xccdf_override));
}

void xccdf_override_free(struct xccdf_override *oride)
{
	if (oride != NULL) {
		oscap_free(oride->authority);
		oscap_text_free(oride->remark);
		oscap_free(oride);
	}
}

struct xccdf_message *xccdf_message_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_message));
}

void xccdf_message_free(struct xccdf_message *msg)
{
    if (msg != NULL) {
        oscap_free(msg->content);
        oscap_free(msg);
    }
}

struct xccdf_target_fact* xccdf_target_fact_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_target_fact));
}

void xccdf_target_fact_free(struct xccdf_target_fact *fact)
{
    if (fact != NULL) {
        oscap_free(fact->name);
        oscap_free(fact->value);
        oscap_free(fact);
    }
}

struct xccdf_instance *xccdf_instance_new(void)
{
    struct xccdf_instance *inst = oscap_calloc(1, sizeof(struct xccdf_instance));
    inst->context = oscap_strdup(XCCDF_INSTANCE_DEFAULT_CONTEXT);
    return inst;
}

void xccdf_instance_free(struct xccdf_instance *inst)
{
    if (inst != NULL) {
        oscap_free(inst->context);
        oscap_free(inst->parent_context);
        oscap_free(inst->content);
        oscap_free(inst);
    }
}
