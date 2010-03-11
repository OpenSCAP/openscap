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

struct xccdf_result *xccdf_result_new(void)
{
	struct xccdf_item *result = xccdf_item_new(XCCDF_PROFILE, NULL);
	oscap_create_lists(&result->sub.result.identities, &result->sub.result.targets,
		&result->sub.result.remarks, &result->sub.result.target_addresses,
		&result->sub.result.target_facts, &result->sub.result.setvalues,
		&result->sub.result.rule_results, &result->sub.result.scores, NULL);
	return XRESULT(result);
}

static inline void xccdf_result_free_impl(struct xccdf_item *result)
{
	if (result != NULL) {
		xccdf_item_release(result);

		oscap_free(result->sub.result.test_system);
		oscap_free(result->sub.result.organization);
		oscap_free(result->sub.result.benchmark_uri);
		oscap_free(result->sub.result.profile);

		// TODO: call proper destructors
		oscap_list_free(result->sub.result.identities, NULL);
		oscap_list_free(result->sub.result.targets, NULL);
		oscap_list_free(result->sub.result.remarks, NULL);
		oscap_list_free(result->sub.result.target_addresses, NULL);
		oscap_list_free(result->sub.result.target_facts, NULL);
		oscap_list_free(result->sub.result.setvalues, NULL);
		oscap_list_free(result->sub.result.rule_results, NULL);
		oscap_list_free(result->sub.result.scores, NULL);

		oscap_free(result);
	}
}
XCCDF_FREE_GEN(result)

XCCDF_ACCESSOR_STRING(result, test_system)
XCCDF_ACCESSOR_STRING(result, organization)
XCCDF_ACCESSOR_STRING(result, benchmark_uri)
XCCDF_ACCESSOR_STRING(result, profile)
XCCDF_LISTMANIP(result, identity, identities)
XCCDF_LISTMANIP(result, target, targets)
XCCDF_LISTMANIP(result, remark, remarks)
XCCDF_LISTMANIP(result, target_address, target_addresses)
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
		oscap_free(rr->version_update);

		// TODO: call proper destructors
		oscap_list_free(rr->overrides, NULL);
		oscap_list_free(rr->idents, NULL);
		oscap_list_free(rr->messages, NULL);
		oscap_list_free(rr->instances, NULL);
		oscap_list_free(rr->fixes, NULL);
		oscap_list_free(rr->checks, NULL);

		oscap_free(rr);
	}
}

OSCAP_ACCESSOR_SIMPLE(time_t, xccdf_rule_result, time)
OSCAP_ACCESSOR_SIMPLE(time_t, xccdf_rule_result, version_time)
OSCAP_ACCESSOR_SIMPLE(xccdf_role_t, xccdf_rule_result, role)
OSCAP_ACCESSOR_SIMPLE(float, xccdf_rule_result, weight)
OSCAP_ACCESSOR_SIMPLE(xccdf_level_t, xccdf_rule_result, severity)
OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_rule_result, result)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, version)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, version_update)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, idref)
OSCAP_IGETINS(xccdf_ident, xccdf_rule_result, idents, ident)
OSCAP_IGETINS(xccdf_fix, xccdf_rule_result, fixes, fix)
OSCAP_IGETINS(xccdf_check, xccdf_rule_result, checks, check)
OSCAP_IGETINS_GEN(xccdf_override, xccdf_rule_result, overrides, override)
OSCAP_IGETINS_GEN(xccdf_message, xccdf_rule_result, messages, message)
OSCAP_IGETINS_GEN(xccdf_instance, xccdf_rule_result, instances, instance)
