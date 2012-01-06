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
 *      "Martin Preisler" <mpreisle@redhat.com>
 */

#ifndef SCE_ENGINE_API_H_
#define SCE_ENGINE_API_H

#include <xccdf.h>
#include <xccdf_policy.h>

struct sce_parameters* sce_parameters_new(void);
void sce_parameters_free(struct sce_parameters* v);
void sce_parameters_set_xccdf_directory(struct sce_parameters* v, const char* value);
const char* sce_parameters_get_xccdf_directory(struct sce_parameters* v);
void sce_parameters_set_results_target_directory(struct sce_parameters* v, const char* value);
const char* sce_parameters_get_results_target_directory(struct sce_parameters* v);

xccdf_test_result_type_t sce_engine_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id,
			       const char *href, struct xccdf_value_binding_iterator *it, void *usr);

bool xccdf_policy_model_register_engine_sce(struct xccdf_policy_model * model, struct sce_parameters *sce_parameters);

#endif
