/*
 * Copyright 2016 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Authors:
 *    Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/list.h"
#include "public/oscap_text.h"
#include "public/xccdf_benchmark.h"
#include "public/xccdf_policy.h"
#include "xccdf_policy_model_priv.h"
#include "XCCDF/item.h"

struct xccdf_policy *xccdf_policy_model_get_existing_policy_by_id(struct xccdf_policy_model *policy_model, const char *profile_id)
{
	struct xccdf_policy_iterator *policy_it = xccdf_policy_model_get_policies(policy_model);
	while (xccdf_policy_iterator_has_more(policy_it)) {
		struct xccdf_policy *policy = xccdf_policy_iterator_next(policy_it);
		if (oscap_streq(xccdf_policy_get_id(policy), profile_id)) {
			xccdf_policy_iterator_free(policy_it);
			return policy;
		}
        }
	xccdf_policy_iterator_free(policy_it);
	return NULL;
}

struct xccdf_policy *xccdf_policy_model_create_policy_by_id(struct xccdf_policy_model *policy_model, const char *id)
{
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

	return xccdf_policy_new(policy_model, profile);
}
