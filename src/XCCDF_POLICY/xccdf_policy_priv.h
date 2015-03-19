/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 *   Simon Lukasik <slukasik@redhat.com>
 *
 */

#pragma once
#ifndef _OSCAP_XCCDF_POLICY_PRIV_H
#define _OSCAP_XCCDF_POLICY_PRIV_H

#include "common/util.h"
#include "public/xccdf_policy.h"

OSCAP_HIDDEN_START;

/**
 * Resolve text substitution in given fix element. Use given xccdf_policy settings
 * for resolving.
 * @memberof xccdf_policy
 * @param policy XCCDF policy used for substitution
 * @param fix a fix element to modify
 * @param rule_result the rule-result for substitution instnace in fix
 * @param test_result the TestResult for xccdf:fact resolution
 * @returns 0 on success, 1 on failure, other value indicate warning
 */
int xccdf_policy_resolve_fix_substitution(struct xccdf_policy *policy, struct xccdf_fix *fix, struct xccdf_rule_result *rule_result, struct xccdf_result *test_result);

/**
 * Execute fix element for a given rule-result. Or find suitable (most appropriate) fix
 * in the policy, assign it to the rule-result and execute.
 * @param policy XCCDF Policy
 * @param rr rule-result element
 * @param fix fix element to execute or NULL
 * @param test_result TestResult element for xccdf:fact resolution.
 * @returns 0 on success. Note that there are 2 special cases, when success is indicated
 * even though remediation was issued. (1) When rule-result/@result != FAIL and (2) when
 * no fix element is suplied and no fix element is find.
 */
int xccdf_policy_rule_result_remediate(struct xccdf_policy *policy, struct xccdf_rule_result *rr, struct xccdf_fix *fix, struct xccdf_result *test_result);

/**
 * Evaluate the XCCDF check.
 * @param policy XCCDF Policy
 * @param check the check to evaluate
 * returns result of the checking engine. Positive number indicates xccdf_test_result_type_t.
 * Negative indicates an error.
 */
int xccdf_policy_check_evaluate(struct xccdf_policy * policy, struct xccdf_check * check);

/**
 * Remediate all rule-results in the given result, with settings of given policy.
 * @memberof xccdf_policy
 * @param policy XCCDF Policy
 * @param result TestResult containing rule-results to remediate
 */
int xccdf_policy_remediate(struct xccdf_policy *policy, struct xccdf_result *result);

/**
 * Report given "rule" to all callbacks with given sysname registered with the policy.
 * @memberof xccdf_policy
 * @param policy XCCDF Policy
 * @param sysname identifier of the reporting callback
 * @param rule pointer to xccdf:Rule or xccdf:rule-result
 * @returns zero on success. Non zero value may indicate user interuption or error.
 */
int xccdf_policy_report_cb(struct xccdf_policy *policy, const char *sysname, void *rule);

/**
 * Get XCCDF Benchmark for given policy
 * @memberof xccdf_policy
 * @param policy XCCDF Policy
 * @returns the benchmark element or NULL.
 */
struct xccdf_benchmark *xccdf_policy_get_benchmark(const struct xccdf_policy *policy);

OSCAP_HIDDEN_END;

#endif
