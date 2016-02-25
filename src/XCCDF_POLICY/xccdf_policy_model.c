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
#include "public/xccdf_policy.h"
#include "xccdf_policy_model_priv.h"

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
