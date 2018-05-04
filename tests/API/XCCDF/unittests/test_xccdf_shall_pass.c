/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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

#include <stdio.h>

#include <xccdf_benchmark.h>
#include <xccdf_policy.h>

#include "unit_helper.h"
#include "../../assume.h"

int main(int argc, char *argv[])
{
	assume(argc == 2);
	struct xccdf_policy_model* policy_model = uh_load_xccdf(argv[1]);
	struct xccdf_policy *policy = uh_get_default_policy(policy_model);
	uh_register_simple_engines(policy_model);

	struct xccdf_result *ritem = xccdf_policy_evaluate(policy);
	assume(ritem != NULL);

	struct xccdf_model_iterator *model_it = xccdf_benchmark_get_models(
			xccdf_policy_model_get_benchmark(policy_model));
	while (xccdf_model_iterator_has_more(model_it)) {
		struct xccdf_model *model = xccdf_model_iterator_next(model_it);
		struct xccdf_score *score = xccdf_policy_get_score(policy, ritem, xccdf_model_get_system(model));
		printf("Score was %f\n", (float) xccdf_score_get_score(score));
		assume((int)xccdf_score_get_score(score) == 100);
	}
	xccdf_policy_model_free(policy_model);
	return 0;
}

