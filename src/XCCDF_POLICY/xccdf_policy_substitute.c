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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "util.h"
#include "public/xccdf_policy.h"

static char* xccdf_subst_callback(xccdf_subst_type_t type, const char *id, void *arg)
{
	if (id == NULL || arg == NULL)
		return NULL;

	struct xccdf_policy *policy = arg;
	struct xccdf_policy_model *model = xccdf_policy_get_model(policy);
	if (model == NULL)
		return NULL;
	struct xccdf_benchmark *bench = xccdf_policy_model_get_benchmark(model);
	if (bench == NULL)
		return NULL;

	switch (type) {
	case XCCDF_SUBST_SUB: {
		// try substitute a value form <cdf:plain_text> element
		const char *subst = xccdf_benchmark_get_plain_text(bench, id);
		if (subst)
			return oscap_strdup(subst);

		// try substitute a Value value
		struct xccdf_item *value = xccdf_benchmark_get_item(bench, id);
		if (value && xccdf_item_get_type(value) == XCCDF_VALUE) {
			char *ret = oscap_strdup("");
			struct xccdf_value *value_tailored = xccdf_item_to_value(xccdf_policy_tailor_item(policy, value));
			struct xccdf_value_instance_iterator *it = xccdf_value_get_instances(value_tailored);
			if (xccdf_value_instance_iterator_has_more(it)) {
				struct xccdf_value_instance *inst = xccdf_value_instance_iterator_next(it);
				oscap_free(ret);
				ret = oscap_strdup(xccdf_value_instance_get_value(inst));
			}
			xccdf_value_instance_iterator_free(it);
			xccdf_item_free(xccdf_value_to_item(value_tailored));
			return ret;
		}
		break;
	}
	default:
		return NULL; // TODO implement other substitution types
	}
}

char* xccdf_policy_substitute(const char *text, struct xccdf_policy *policy) {
	return oscap_text_xccdf_substitute(text, xccdf_subst_callback, policy);
}
