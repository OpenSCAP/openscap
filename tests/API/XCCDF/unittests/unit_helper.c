/*
 * Copyright 2012--2014 Red Hat Inc., Durham, North Carolina.
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
 *	Simon Lukasik <slukasik@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "unit_helper.h"
#include <xccdf_benchmark.h>
#include <oval_agent_xccdf_api.h>
#include <oscap_source.h>

static xccdf_test_result_type_t _always_pass_eval_rule(struct xccdf_policy *, const char *, const char *, const char *, struct xccdf_value_binding_iterator *, struct xccdf_check_import_iterator *, void *);
static xccdf_test_result_type_t _always_fail_eval_rule(struct xccdf_policy *, const char *, const char *, const char *, struct xccdf_value_binding_iterator *, struct xccdf_check_import_iterator *, void *);



void
uh_register_simple_engines(struct xccdf_policy_model * model)
{
	xccdf_policy_model_register_engine_and_query_callback(model, "http://check-engine.test/pass", _always_pass_eval_rule, (void*)NULL, NULL);
	xccdf_policy_model_register_engine_and_query_callback(model, "http://check-engine.test/fail", _always_fail_eval_rule, (void*)NULL, NULL);
}

struct xccdf_policy_model *
uh_load_xccdf(const char *filename)
{
	struct xccdf_benchmark *bench = NULL;
	struct oscap_source *source = oscap_source_new_from_file(filename);
	if ((bench = xccdf_benchmark_import_source(source))== NULL)
		fprintf(stderr, "Failed to import the XCCDF content from (%s).\n", filename);
	oscap_source_free(source);
	return xccdf_policy_model_new(bench);
}

struct xccdf_policy *
uh_get_default_policy(struct xccdf_policy_model *policy_model)
{
	struct xccdf_policy *policy = xccdf_policy_model_get_policy_by_id(policy_model, NULL);
	if (policy == NULL)
		fprintf(stderr, "No Policy to evaluate.\n");
	return policy;
}



xccdf_test_result_type_t
_always_pass_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id,
		const char *href, struct xccdf_value_binding_iterator *value_binding_it,
		struct xccdf_check_import_iterator *check_import_it, void *usr)
{
	return XCCDF_RESULT_PASS;
}

xccdf_test_result_type_t
_always_fail_eval_rule(	struct xccdf_policy *policy, const char *rule_id, const char *id,
		const char *href, struct xccdf_value_binding_iterator *value_binding_it,
		struct xccdf_check_import_iterator *check_import_it, void *usr)
{
	return XCCDF_RESULT_FAIL;
}

