/*
 * Copyright 2015 Red Hat Inc., Durham, North Carolina.
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

#ifndef XCCDF_POLICY_RESOLVE_H_
#define XCCDF_POLICY_RESOLVE_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <xccdf_benchmark.h>
#include <xccdf_policy_priv.h>
#include <math.h>


/*
 * Struct contains final values defined in several xccdf_refine_rule structures.
 * There is no "item" member. Hash-table key is used instead of this member.
 * This structure is used only for internal rule processing.
 */
struct xccdf_refine_rule_internal;

char* xccdf_refine_rule_internal_get_selector(const struct xccdf_refine_rule_internal*);
xccdf_role_t xccdf_refine_rule_internal_get_role(const struct xccdf_refine_rule_internal*);
xccdf_level_t xccdf_refine_rule_internal_get_severity(const struct xccdf_refine_rule_internal*);
xccdf_numeric xccdf_refine_rule_internal_get_weight(const struct xccdf_refine_rule_internal*);

/**
 * Return refine-rule belonging to an item (by item ID)
 * @return refine-rule or NULL
 */
struct xccdf_refine_rule_internal*  xccdf_policy_get_refine_rule_by_item(struct xccdf_policy* policy, struct xccdf_item* item);

/**
 * Return true, if value of weight is valid
 */
bool xccdf_weight_defined(xccdf_numeric weight);

/**
 * Return final role of rule (role set by role potentially refined by refine-rule)
 */
xccdf_role_t xccdf_get_final_role(const struct xccdf_rule* rule, const struct xccdf_refine_rule_internal* r_rule);

/**
 * Return final weight of rule (role set by role potentially refined by refine-rule)
 */
float xccdf_get_final_weight(const struct xccdf_rule* rule, const struct xccdf_refine_rule_internal* r_rule);

/**
 * Return final severity of rule (role set by role potentially refined by refine-rule)
 */
xccdf_level_t xccdf_get_final_severity(const struct xccdf_rule* rule, const struct xccdf_refine_rule_internal* r_rule);

/**
 * Process refine-rules from profile and fill hash table with final value of refine-rules for specific item-id
 */
void xccdf_policy_add_profile_refine_rules(struct xccdf_policy* policy, struct xccdf_benchmark* benchmark, struct xccdf_profile* profile);

/**
 * Free function for xccdf_refine_rule_internal
 */
void xccdf_refine_rule_internal_free(struct xccdf_refine_rule_internal* item);

#endif
