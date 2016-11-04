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

#include <xccdf_benchmark.h>
#include <xccdf_policy.h>

/**
 * @memberof sce_check_result
 */
struct sce_check_result* sce_check_result_new(void);

/**
 * @memberof sce_check_result
 */
void sce_check_result_free(struct sce_check_result* v);

/**
 * Sets the href used to execute the check that yielded given check result
 *
 * @memberof sce_check_result
 */
void sce_check_result_set_href(struct sce_check_result* v, const char* href);

/**
 * @memberof sce_check_result
 */
const char* sce_check_result_get_href(struct sce_check_result* v);

/**
 * Sets basename of the script that was used for check evaluation
 *
 * @memberof sce_check_result
 */
void sce_check_result_set_basename(struct sce_check_result* v, const char* basename);

/**
 * @memberof sce_check_result
 */
const char* sce_check_result_get_basename(struct sce_check_result* v);

/**
 * Sets stdout that was captured while script was evaluating
 *
 * @param stdout should contain output only from stdout
 * @memberof sce_check_result
 */
void sce_check_result_set_stdout(struct sce_check_result* v, const char* details);

/**
 * @memberof sce_check_result
 */
const char* sce_check_result_get_stdout(struct sce_check_result* v);

/**
 * Sets stderr that was captured while script was evaluating
 *
 * @param stderr should contain output only from stderr
 * @memberof sce_check_result
 */
void sce_check_result_set_stderr(struct sce_check_result* v, const char* details);

/**
 * @memberof sce_check_result
 */
const char* sce_check_result_get_stderr(struct sce_check_result* v);

/**
 * Sets exit code with which the script ended execution after evaluation
 * @memberof sce_check_result
 */
void sce_check_result_set_exit_code(struct sce_check_result* v, int exit_code);

/**
 * @memberof sce_check_result
 */
int sce_check_result_get_exit_code(struct sce_check_result* v);

/**
 * Clears the list of passed environment variables
 *
 * @memberof sce_check_result
 */
void sce_check_result_reset_environment_variables(struct sce_check_result* v);

/**
 * Adds an environment variable entry to list of environment variables that
 * were passed to the script. These include XCCDF result codes,
 * bound XCCDF variables and possibly more.
 *
 * sce_check_result contains these mostly for debugging purposes.
 *
 * @param var entry that will be added, in "VARIABLE_NAME=VARIABLE_VALUE" form
 * @memberof sce_check_result
 */
void sce_check_result_add_environment_variable(struct sce_check_result* v, const char* var);

/**
 * Sets the final xccdf result (after exit code to xccdf mapping takes place)
 *
 * @memberof sce_check_result
 */
void sce_check_result_set_xccdf_result(struct sce_check_result* v, xccdf_test_result_type_t result);

/**
 * @memberof sce_check_result
 */
xccdf_test_result_type_t sce_check_result_get_xccdf_result(struct sce_check_result* v);

/**
 * Exports details (in XML form) of given check result to given file
 *
 * @memberof sce_check_result
 */
void sce_check_result_export(struct sce_check_result* v, const char* target_file);

/**
 * @memberof sce_session
 */
struct sce_session* sce_session_new(void);

/**
 * @memberof sce_session
 */
void sce_session_free(struct sce_session* s);

/**
 * Removes all check results from the session
 *
 * @memberof sce_session
 */
void sce_session_reset(struct sce_session* s);

/**
 * Adds a check result to the session
 *
 * @param s session to add the check result to
 * @param result result to be added (the session takes ownership of it, don't deallocate it!)
 * @memberof sce_session
 */
void sce_session_add_check_result(struct sce_session* s, struct sce_check_result* result);

/**
 * @struct sce_check_result_iterator
 * Iterator over collections of sce_check_results.
 * @see sce_check_result_iterator
 */
struct sce_check_result_iterator;
struct sce_check_result *sce_check_result_iterator_next(struct sce_check_result_iterator *it);
/// @memberof sce_check_result_iterator
bool sce_check_result_iterator_has_more(struct sce_check_result_iterator *it);
/// @memberof sce_check_result_iterator
void sce_check_result_iterator_free(struct sce_check_result_iterator *it);
/// @memberof sce_check_result_iterator
void sce_check_result_iterator_reset(struct sce_check_result_iterator *it);

struct sce_check_result_iterator *sce_session_get_check_results(struct sce_session* s);

/**
 * Exports all check results to given directory
 *
 * @memberof sce_session
 */
void sce_session_export_to_directory(struct sce_session* s, const char* directory);

/**
 * @memberof sce_parameters
 */
struct sce_parameters* sce_parameters_new(void);

/**
 * @memberof sce_parameters
 */
void sce_parameters_free(struct sce_parameters* v);

/**
 * Sets the directory that contains XCCDF that will reference SCE checks
 *
 * @internal This is used to figure out where to look for the scripts
 * @memberof sce_parameters
 */
void sce_parameters_set_xccdf_directory(struct sce_parameters* v, const char* value);

/**
 * @memberof sce_parameters
 */
const char* sce_parameters_get_xccdf_directory(struct sce_parameters* v);

/**
 * Sets SCE session to use for check results storage
 *
 * @param v
 * @param value SCE session to use (sce_parameters take ownership of it, don't deallocate it!)
 * @memberof sce_parameters
 */
void sce_parameters_set_session(struct sce_parameters* v, struct sce_session* value);

/**
 * @memberof sce_parameters
 */
struct sce_session* sce_parameters_get_session(struct sce_parameters* v);

/**
 * Just a convenience shortcut of setting a session to a newly allocated session
 *
 * The session gets automatically freed when sce_parameters are freed, don't deallocate it!
 * @memberof sce_parameters
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
