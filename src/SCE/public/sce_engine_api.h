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

struct sce_check_result* sce_check_result_new(void);
void sce_check_result_free(struct sce_check_result* v);

void sce_check_result_set_href(struct sce_check_result* v, const char* href);
const char* sce_check_result_get_href(struct sce_check_result* v);
void sce_check_result_set_basename(struct sce_check_result* v, const char* basename);
const char* sce_check_result_get_basename(struct sce_check_result* v);
void sce_check_result_set_stdout(struct sce_check_result* v, const char* details);
const char* sce_check_result_get_stdout(struct sce_check_result* v);
void sce_check_result_set_exit_code(struct sce_check_result* v, int exit_code);
int sce_check_result_get_exit_code(struct sce_check_result* v);
void sce_check_result_reset_environment_variables(struct sce_check_result* v);
void sce_check_result_add_environment_variable(struct sce_check_result* v, const char* var);
void sce_check_result_set_xccdf_result(struct sce_check_result* v, xccdf_test_result_type_t result);
xccdf_test_result_type_t sce_check_result_get_xccdf_result(struct sce_check_result* v);

/**
 * Exports details (in XML form) of given check result to given file
 */
void sce_check_result_export(struct sce_check_result* v, const char* target_file);

struct sce_session* sce_session_new(void);
void sce_session_free(struct sce_session* s);

/**
 * Removes all check results from the session
 */
void sce_session_reset(struct sce_session* s);

/**
 * Adds a check result to the session
 *
 * @param s session to add the check result to
 * @param result result to be added (the session takes ownership of it, don't deallocate it!)
 */
void sce_session_add_check_result(struct sce_session* s, struct sce_check_result* result);
void sce_session_export_to_directory(struct sce_session* s, const char* directory);

struct sce_parameters* sce_parameters_new(void);
void sce_parameters_free(struct sce_parameters* v);
void sce_parameters_set_xccdf_directory(struct sce_parameters* v, const char* value);
const char* sce_parameters_get_xccdf_directory(struct sce_parameters* v);

/**
 * Sets SCE session to use for check results storage
 *
 * @param v
 * @param value SCE session to use (sce_parameters take ownership of it, don't deallocate it!)
 */
void sce_parameters_set_session(struct sce_parameters* v, struct sce_session* value);
struct sce_session* sce_parameters_get_session(struct sce_parameters* v);

/**
 * Just a convenience shortcut of setting a session to a newly allocated session
 *
 * The session gets automatically freed when sce_parameters are freed, don't deallocate it!
 */
void sce_parameters_allocate_session(struct sce_parameters* v);

/**
 * Internal rule evaluation callback, don't use directly
 *
 * @see xccdf_policy_model_register_engine_sce
 */
xccdf_test_result_type_t sce_engine_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id, const char *href,
			       struct xccdf_value_binding_iterator *value_binding_it,
			       struct xccdf_check_import_iterator *check_import_it,
			       void *usr);

/**
 * Registers SCE to given policy model
 *
 * @param model model to register SCE to
 * @param sce_parameters various parameters to be used with SCE (you are responsible to deallocate them!)
 */
bool xccdf_policy_model_register_engine_sce(struct xccdf_policy_model * model, struct sce_parameters *sce_parameters);

#endif
