/*
 * Copyright 1015 Red Hat Inc., Durham, North Carolina.
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
 */
#include "xccdf_policy_resolve.h"
#include "item.h"
#include "common/_error.h"

struct xccdf_refine_rule_internal {
	char* selector;
	xccdf_role_t role;
	xccdf_level_t severity;
	xccdf_numeric weight;
};

OSCAP_GETTER(char*, xccdf_refine_rule_internal, selector);
OSCAP_GETTER(xccdf_role_t, xccdf_refine_rule_internal, role);
OSCAP_GETTER(xccdf_level_t, xccdf_refine_rule_internal, severity);
OSCAP_GETTER(xccdf_numeric, xccdf_refine_rule_internal, weight);

/**
 * Merge refine rule from profile with internal refine rule
 * Defined src refine-rule will override dst values
 * @param dst Internal refine rule
 * @param src Refine rule from profile
 */
static void _merge_refine_rules(struct xccdf_refine_rule_internal* dst, const struct xccdf_refine_rule* src)
{
	bool new_weight_defined = xccdf_refine_rule_weight_defined(src);
	if (new_weight_defined) {
		xccdf_numeric new_weight = xccdf_refine_rule_get_weight(src);
		dst->weight = new_weight;
	}

	const char* new_selector = xccdf_refine_rule_get_selector(src);
	if (new_selector != NULL) {
		oscap_free(dst->selector);
		dst->selector = oscap_strdup( new_selector );
	}

	xccdf_role_t new_role = xccdf_refine_rule_get_role(src);
	if (new_role != 0) {
		dst->role = new_role;
	}

	xccdf_level_t new_severity = xccdf_refine_rule_get_severity(src);
	if (new_severity != XCCDF_LEVEL_NOT_DEFINED) {
		dst->severity = new_severity;
	}
}

/**
 * Allocate memory for new struct and init it with refine-rule values
 * @param rr Refine rule from profile
 * @return allocated internal refine-rule
 */
static inline struct xccdf_refine_rule_internal* _xccdf_refine_rule_internal_new_from_refine_rule(const struct xccdf_refine_rule* rr)
{
	struct xccdf_refine_rule_internal* new_rr = oscap_calloc(1, sizeof(struct xccdf_refine_rule_internal));
	if (new_rr != NULL) {
		new_rr->selector = oscap_strdup(xccdf_refine_rule_get_selector(rr));
		new_rr->weight = rr->weight;
		new_rr->role = rr->role;
		new_rr->severity = rr->severity;
	}
	return new_rr;
}

struct xccdf_refine_rule_internal* xccdf_policy_get_refine_rule_by_item(struct xccdf_policy* policy, struct xccdf_item* rule)
{
	const char* item_id = xccdf_item_get_id(rule);
	return oscap_htable_get(policy->refine_rules_internal, item_id);
}

xccdf_role_t xccdf_get_final_role(const struct xccdf_rule* rule, const struct xccdf_refine_rule_internal* r_rule)
{
	if (r_rule == NULL) {
		return xccdf_rule_get_role(rule);
	} else {
		return r_rule->role;
	}
}

/**
 * Return true, if value of weight is valid
 */
bool xccdf_weight_defined(xccdf_numeric weight){
	return (!isnan(weight));
}

float xccdf_get_final_weight(const struct xccdf_rule* rule, const struct xccdf_refine_rule_internal* r_rule)
{
	if (r_rule != NULL){
		if (xccdf_weight_defined(r_rule->weight) ) {
			return r_rule->weight;
		}
	}
	return xccdf_rule_get_weight(rule);
}

xccdf_level_t xccdf_get_final_severity(const struct xccdf_rule* rule, const struct xccdf_refine_rule_internal* r_rule)
{
	if (r_rule == NULL){
		return xccdf_rule_get_severity(rule);
	} else {
		return r_rule->severity;
	}
}

/**
 * Put refine-rule into hash table with item_id as key. Refine-rules are with same key are merged.
 * @param refine_rules_internal hash table
 * @param new_rr refine-rule to add
 * @param item_id key to hash table
 */
static void _add_refine_rule(struct oscap_htable* refine_rules_internal, const struct xccdf_refine_rule* new_rr, const char* item_id)
{
	struct xccdf_refine_rule_internal* old = oscap_htable_get(refine_rules_internal, item_id);
	if (old != NULL) { // modify refine-rule in hash-table
		_merge_refine_rules(old, new_rr);
	} else { // add new refine-rule to hash-table
		struct xccdf_refine_rule_internal* new_internal_rr = _xccdf_refine_rule_internal_new_from_refine_rule(new_rr);
		oscap_htable_add(refine_rules_internal, item_id, new_internal_rr);
	}
}

void xccdf_refine_rule_internal_free(struct xccdf_refine_rule_internal* item)
{
	oscap_free(item->selector);
	oscap_free(item);
}

static inline void _xccdf_policy_add_xccdf_refine_rule_internal(struct xccdf_policy* policy, struct xccdf_benchmark* benchmark, const struct xccdf_refine_rule* refine_rule)
{
	const char* rr_item_id = xccdf_refine_rule_get_item(refine_rule);
	struct xccdf_item* item = xccdf_benchmark_get_member(benchmark, XCCDF_ITEM, rr_item_id);
	if (item != NULL) { // get item by id
		_add_refine_rule(policy->refine_rules_internal, refine_rule, rr_item_id);
		return;
	}

	// try to get items by cluster-id
	struct oscap_htable_iterator* hit = xccdf_benchmark_get_cluster_items(benchmark, rr_item_id);
	if (hit == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XCCDF, "Selector ID(%s) does not exist in Benchmark.", rr_item_id);
		return;
	}

	while (oscap_htable_iterator_has_more(hit)) { // iterate through every item in cluster
		const char* item_id = oscap_htable_iterator_next_key(hit);
		if (item_id == NULL) {
			assert(item_id != NULL);
			continue;
		}
		_add_refine_rule(policy->refine_rules_internal,refine_rule, item_id);
	}
	oscap_htable_iterator_free(hit);
}

void xccdf_policy_add_profile_refine_rules(struct xccdf_policy* policy, struct xccdf_benchmark* benchmark, struct xccdf_profile* profile)
{
	struct xccdf_refine_rule_iterator* rr_it = xccdf_profile_get_refine_rules(profile);
	/* Iterate through refine_rules in profile */
	while (xccdf_refine_rule_iterator_has_more(rr_it)) {
		struct xccdf_refine_rule* rr = xccdf_refine_rule_iterator_next(rr_it);
		if (rr == NULL) {
			assert(false);
			continue;
		}
		struct xccdf_refine_rule* clone = xccdf_refine_rule_clone(rr);
		_xccdf_policy_add_xccdf_refine_rule_internal(policy, benchmark, clone);
	}
	xccdf_refine_rule_iterator_free(rr_it);
}
