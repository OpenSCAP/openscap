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
	oscap_create_lists(&result->sub.result.identities, &result->sub.result.targets, &result->sub.result.remarks, &result->sub.result.target_addresses,
		&result->sub.result.target_facts, &result->sub.result.setvalues, &result->sub.result.rule_results, &result->sub.result.scores, NULL);
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



