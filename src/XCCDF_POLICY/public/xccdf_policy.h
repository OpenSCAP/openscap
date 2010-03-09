/*
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
 */


#ifndef XCCDF_POLICY_H_
#define XCCDF_POLICY_H_

#include <stdbool.h>
#include <time.h>
#include <oscap.h>
#include <xccdf.h>

/**
 * @defgroup XCCDFPOLICY XCCDF-POLICY
 * XCCDF Evaluation/Scoring/Reporting Framework WHOOPS.
 * The XCCDF Policy Model represents a loaded XCCDF benchmark document.
 * This model accesses the generating benchmark and the policies supported by the benchmark
 * @author Dave Niemoller <david.niemoller@g2-inc.com>
 * @{
 * @defgroup Xccdf_policy
 * @{
 * @}
 * @defgroup Xccdf_value_binding
 * @{
 * @}
 */
/**
 * @}
 */
/**
 * @struct xccdf_policy_model
 * Handle: @ref XCCDFPOLICY.
 * @ingroup XCCDFPOLICY
 */
struct xccdf_policy_model;

/**
 * @struct xccdf_policy
 * Handle: @ref Xccdf_policy.
 * @ingroup Xccdf_policy
 */
struct xccdf_policy;
struct xccdf_policy_iterator;

/**
 * @struct xccdf_value_binding
 * Handle: @ref Xccdf_value_binding.
 * @ingroup Xccdf_value_binding
 */
struct xccdf_value_binding;
struct xccdf_value_binding_iterator;


struct xccdf_policy_model *xccdf_policy_model_new(struct xccdf_benchmark *);
struct xccdf_policy * xccdf_policy_new(struct xccdf_profile * profile);

struct xccdf_benchmark * xccdf_policy_model_get_benchmark(const struct xccdf_policy_model * item);

struct xccdf_policy_iterator *xccdf_policy_model_get_policies(const struct xccdf_policy_model *);

/**
 * Get selected rules from policy
 * @memberof xccdf_policy
 * @return Pointer to select iterator.
 * @retval NULL on faliure
 */
struct xccdf_select_iterator   *xccdf_policy_get_selected_rules(struct xccdf_policy *);

/**
 * Evaluate policy
 * @memberof xccdf_policy
 * @return true if evaluation pass or false in case of error
 */
bool xccdf_policy_evaluate(struct xccdf_policy *);

struct xccdf_value_binding * xccdf_value_binding_new(const struct xccdf_profile * profile);

struct xccdf_value_binding_iterator *xccdf_policy_get_bound_values(struct xccdf_policy *);

/**
 * Import/Export functions
 */
void xccdf_policy_export_variables(struct xccdf_policy *, char *export_namespace, struct oscap_export_target *);
void xccdf_policy_export_controls (struct xccdf_policy *, char *export_namespace, struct oscap_export_target *);
void xccdf_policy_import_results(struct xccdf_policy *, char *import_namespace, struct oscap_import_source *);
void xccdf_policy_export_results(struct xccdf_policy *, char *scoring_model_namespace, struct oscap_export_target *);

/**
 * Free functions 
 */
void xccdf_policy_model_free(struct xccdf_policy_model *);
void xccdf_policy_free(struct xccdf_policy *);
void xccdf_value_binding_free(struct xccdf_value_binding *);

/// @memberof xccdf_policy_model
bool xccdf_policy_model_add_policy(struct xccdf_policy_model *, struct xccdf_policy *);
/// @memberof xccdf_policy
struct xccdf_profile * xccdf_policy_get_profile(const struct xccdf_policy *);
/// @memberof xccdf_policy
struct xccdf_select_iterator * xccdf_policy_get_rules(const struct xccdf_policy *);
/// @memberof xccdf_policy
struct xccdf_value_binding_iterator * xccdf_policy_get_values(const struct xccdf_policy *);
/// @memberof xccdf_policy
bool xccdf_policy_add_rule(struct xccdf_policy *, struct xccdf_select *);
/// @memberof xccdf_policy
bool xccdf_policy_add_value(struct xccdf_policy *, struct xccdf_value_binding *);

/// @memberof xccdf_value_binding
struct xccdf_refine_rule_iterator * xccdf_value_binding_get_refine_rules(const struct xccdf_value_binding *);
/// @memberof xccdf_value_binding
struct xccdf_refine_value_iterator * xccdf_value_binding_get_refine_values(const struct xccdf_value_binding *);
/// @memberof xccdf_value_binding
struct xccdf_setvalue_iterator * xccdf_value_binding_get_setvalues(const struct xccdf_value_binding *);
/// @memberof xccdf_value_binding
bool xccdf_value_binding_add_refine_rule(struct xccdf_value_binding *, struct xccdf_refine_rule *);
/// @memberof xccdf_value_binding
bool xccdf_value_binding_add_refine_value(struct xccdf_value_binding *, struct xccdf_refine_value *);
/// @memberof xccdf_value_binding
bool xccdf_value_binding_add_setvalue(struct xccdf_value_binding *, struct xccdf_setvalue *);

/// @memberof xccdf_policy
bool xccdf_policy_set_selected(struct xccdf_policy * policy, char * idref);

#endif


