/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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


/**
 * @addtogroup XCCDF_POLICY
 * @{
 * @file xccdf_policy.h
 * Open-scap XCCDF Policy library interface.
 * @author Maros Barabas <mbarabas@redhat.com>
 * @author Dave Niemoller <david.niemoller@g2-inc.com>
 */

#ifndef XCCDF_POLICY_H_
#define XCCDF_POLICY_H_

#include <xccdf_benchmark.h>
#include <stdbool.h>
#include <time.h>
#include <oscap.h>

/**
 * @struct xccdf_policy_model
 * Handle all policies for given XCCDF benchmark
 */
struct xccdf_policy_model;

/**
 * @struct xccdf_policy
 * Policy structure that abstract benchmark's profile
 */
struct xccdf_policy;

/**
 * @struct xccdf_value_binding
 * Value bindings of policy
 */
struct xccdf_value_binding;

struct xccdf_value_binding_iterator;

/**
 * @struct xccdf_policy_iterator
 * Iterate through policies
 * @see xccdf_policy_model_get_policies
 */
struct xccdf_policy_iterator;

/**
 * Type of a query over checking-engine data.
 * This allows xccdf_policy module to query checking engine and acquire comprehensive info.
 */
typedef enum {
	POLICY_ENGINE_QUERY_NAMES_FOR_HREF = 1,		/// Considering xccdf:check-content-ref, what are possible @name attributes for given href?
} xccdf_policy_engine_query_t;

/**
 * Type of function which implements queries defined within xccdf_policy_engine_query_t.
 *
 * Each checking engine may register its own function of the xccdf_policy_engine_query_fn
 * type. The registered function is then used by xccdf_policy module to acquire comprehensive
 * info about the checking-engine itself or the data fed in. First argument of the function
 * is always user data as registered. Second argument defines the query. Third argument is
 * dependent on query and defined as follows:
 *  - (const char *)href -- for POLICY_ENGINE_QUERY_NAMES_FOR_HREF
 *
 * Expected return type depends also on query as follows:
 *  - (struct oscap_stringlists *) -- for POLICY_ENGINE_QUERY_NAMES_FOR_HREF
 *  - NULL shall be returned if the function doesn't understand the query.
 */
typedef void *(*xccdf_policy_engine_query_fn) (void *, xccdf_policy_engine_query_t, void *);

/**
 * Type of function which implements OpenSCAP checking engine.
 *
 * This function defines basic interface between XCCDF module and thee checking engine.
 * For each checking engine required for evaluation there should be at least one such
 * function registerd. The registered function is then used by xccdf_policy module to
 * perform evaluation on the machine.
 */
typedef xccdf_test_result_type_t (*xccdf_policy_engine_eval_fn) (struct xccdf_policy *policy, const char *rule_id, const char *definition_id, const char *href_if, struct xccdf_value_binding_iterator *value_binding_it, struct xccdf_check_import_iterator *check_imports_it, void *user_data);

/************************************************************/

/**
 * Constructor of Policy Model structure
 * @param benchmark Struct xccdf_benchmark with benchmark model
 * @return new xccdf_policy_model
 * @note
 * The policy model will take ownership of given benchmark and free it
 * when it's being destructed!
 * @memberof xccdf_policy_model
 */
struct xccdf_policy_model *xccdf_policy_model_new(struct xccdf_benchmark *benchmark);

/**
 * Constructor of Policy structure
 * @param model Policy model
 * @param profile Profile from XCCDF Benchmark
 * @memberof xccdf_policy
 */
struct xccdf_policy * xccdf_policy_new(struct xccdf_policy_model * model, struct xccdf_profile * profile);

/**
 * Constructor of structure with profile bindings - refine_rules, refine_values and set_values
 * @memberof xccdf_value_binding
 * @return new structure of xccdf_value_binding
 */
struct xccdf_value_binding * xccdf_value_binding_new(void);

/** 
 * Destructor of Policy Model structure
 * @memberof xccdf_policy_model
 */
void xccdf_policy_model_free(struct xccdf_policy_model *);

/** 
 * Destructor of Policy structure
 * @memberof xccdf_policy
 */
void xccdf_policy_free(struct xccdf_policy *);

/** 
 * Destructor of Value binding structure
 * @memberof xccdf_value_binding
 */
void xccdf_value_binding_free(struct xccdf_value_binding *);

/**
 * Sets the Tailoring element to use in the policy.
 *
 * Profiles from the Benchmark *may* be shadowed by profiles in the given
 * Tailoring element! Calling this function will affect the results of
 * xccdf_policy_model_get_policy_by_id calls.
 *
 * @important This does INVALIDATE preexisting xccdf_policy pointers in
 * this xccdf_policy_model! It is recommended to call this function before
 * policy for any profile is queried.
 *
 * @note
 * The policy model will take ownership of given tailoring and free it
 * when it's being destructed or when new tailoring is being set.
 */
bool xccdf_policy_model_set_tailoring(struct xccdf_policy_model *model, struct xccdf_tailoring *tailoring);

/**
 * Retrieves the Tailoring element used in this policy.
 * @memberof xccdf_policy_model
 */
struct xccdf_tailoring *xccdf_policy_model_get_tailoring(struct xccdf_policy_model *model);

/**
 * Get human readable title of given XCCDF Item. This finds title with best matching language
 * and resolves <xccdf:sub> substitution in accordance with the given XCCDF Policy.
 * @memberof xccdf_policy
 * @param policy XCCDF Policy
 * @param item XCCDF Item to query title from
 * @param preferred_lang Language of your choice, Null value for the default.
 * @returns plaintext C string which must be freed by caller
 */
char *xccdf_policy_get_readable_item_title(struct xccdf_policy *policy, struct xccdf_item *item, const char *preferred_lang);

/**
 * Get human readable description of given XCCDF Item. This function searches for description
 * with the best matching language and resolves any inner <xccdf:sub> substitution (in accordance
 * with the given XCCDF Policy.
 * @memberof xccdf_policy
 * @param policy XCCDF Policy
 * @param item XCCDF Item to query description from
 * @param preferred_lang Language of your choice, Null value for the default.
 * @returns plaintext C string which must be freed by caller
 */
char *xccdf_policy_get_readable_item_description(struct xccdf_policy *policy, struct xccdf_item *item, const char *preferred_lang);

/**
 * Registers an additional CPE dictionary for applicability testing
 * The one embedded in the evaluated XCCDF take precedence!
 */
bool xccdf_policy_model_add_cpe_dict_source(struct xccdf_policy_model * model, struct oscap_source *source);

/**
 * Registers an additional CPE dictionary for applicability testing
 * The one embedded in the evaluated XCCDF take precedence!
 *
 * @deprecated Deprecated in favor of @ref xccdf_policy_model_add_cpe_dict_source
 */
bool xccdf_policy_model_add_cpe_dict(struct xccdf_policy_model * model, const char * cpe_dict);

/**
 * Registers an additional CPE lang model for applicability testing
 * The one embedded in the evaluated XCCDF take precedence!
 */
bool xccdf_policy_model_add_cpe_lang_model_source(struct xccdf_policy_model * model, struct oscap_source *source);

/**
 * Registers an additional CPE lang model for applicability testing
 * The one embedded in the evaluated XCCDF take precedence!
 *
 * @deprecated Deprecated in favor of @ref xccdf_policy_model_add_cpe_lang_model_source
 */
OSCAP_DEPRECATED(bool xccdf_policy_model_add_cpe_lang_model(struct xccdf_policy_model * model, const char *cpe_lang));

/**
 * Registers an additional CPE resource (either dictionary or language)
 * Autodetects given file and acts accordingly.
 * The one embedded in the evaluated XCCDF take precedence!
 */
bool xccdf_policy_model_add_cpe_autodetect_source(struct xccdf_policy_model *model, struct oscap_source *source);

/**
 * Registers an additional CPE resource (either dictionary or language)
 * Autodetects given file and acts accordingly.
 * The one embedded in the evaluated XCCDF take precedence!
 *
 * @deprecated Deprecated in favor of @ref xccdf_policy_model_add_cpe_autodetect_source
 */
OSCAP_DEPRECATED(bool xccdf_policy_model_add_cpe_autodetect(struct xccdf_policy_model *model, const char *filepath));

/**
 * Retrieves an iterator of all OVAL sessions created for CPE applicability evaluation
 * key is the OVAL href, value is the OVAL session itself (type oval_agent_session*)
 */
struct oscap_htable_iterator *xccdf_policy_model_get_cpe_oval_sessions(struct xccdf_policy_model *model);

/**
 * Function to register callback for checking system
 * @param model XCCDF Policy Model
 * @param sys String representing given checking system
 * @param func Callback - pointer to function called by XCCDF Policy system when rule parsed
 * @param usr optional parameter for passing user data to callback
 * @memberof xccdf_policy_model
 * @return true if callback registered succesfully, false otherwise
 *
 * @deprecated This function is deprecated by @ref xccdf_policy_model_register_engine_and_query_callback
 * and might be dropped from future releases.
 */
OSCAP_DEPRECATED(bool xccdf_policy_model_register_engine_callback(struct xccdf_policy_model * model, char * sys, void * func, void * usr));

/**
 * Function to register callback for checking system
 * @param model XCCDF Policy Model
 * @param sys String representing given checking system
 * @param eval_fn Callback - pointer to function called by XCCDF Policy system when rule parsed
 * @param usr optional parameter for passing user data to callback
 * @param query_fn - optional parameter for providing xccdf_policy_engine_query_fn implementation for given system.
 * @memberof xccdf_policy_model
 * @return true if callback registered succesfully, false otherwise
 */
bool xccdf_policy_model_register_engine_and_query_callback(struct xccdf_policy_model *model, char *sys, xccdf_policy_engine_eval_fn eval_fn, void *usr, xccdf_policy_engine_query_fn query_fn);

typedef int (*policy_reporter_output)(struct xccdf_rule_result *, void *);

/**
 * Function to register output callback for checking system that will be called AFTER each rule evaluation.
 * @param model XCCDF Policy Model
 * @param func Callback - pointer to function called by XCCDF Policy system when rule parsed
 * @param usr optional parameter for passing user data to callback
 * @memberof xccdf_policy_model
 * @return true if callback registered succesfully, false otherwise
 */
bool xccdf_policy_model_register_output_callback(struct xccdf_policy_model * model, policy_reporter_output func, void * usr);

typedef int (*policy_reporter_start)(struct xccdf_rule *, void *);

/**
 * Function to register start callback for checking system that will be called BEFORE each rule evaluation.
 * @param model XCCDF Policy Model
 * @param func Callback - pointer to function called by XCCDF Policy system when rule parsed
 * @param usr optional parameter for passing user data to callback
 * @memberof xccdf_policy_model
 * @return true if callback registered succesfully, false otherwise
 */
bool xccdf_policy_model_register_start_callback(struct xccdf_policy_model * model, policy_reporter_start func, void * usr);

/************************************************************/
/**
 * @name Getters
 * Return value is pointer to structure's member. Do not free unless you null the pointer in the structure. 
 * Use remove function otherwise.
 * @{
 * */

/**
 * Get model from Policy (parent structure of Policy to access the benchmark)
 * @param policy XCCDF Policy
 * @return Policy model
 * @memberof xccdf_policy
 */
struct xccdf_policy_model * xccdf_policy_get_model(const struct xccdf_policy * policy);

/**
 * Get Benchmark from Policy Model
 * @param item Policy model structure
 * @return XCCDF Benchmark for given policy model
 * @memberof xccdf_policy_model
 */
struct xccdf_benchmark * xccdf_policy_model_get_benchmark(const struct xccdf_policy_model * item);

/**
 * Get Value Bindings from XCCDF Policy
 * @memberof xccdf_policy
 */
struct xccdf_value_binding_iterator  * xccdf_policy_get_values(const struct xccdf_policy * item);

/**
 * Get policies from Policy Model. Be aware, this function returns only a list of previously
 * initialized policies. A policy can be initialized by xccdf_policy_model_get_policy_by_id.
 * @param model Policy Model
 * @return Iterator for list of policies
 * @memberof xccdf_policy_model
 */
struct xccdf_policy_iterator * xccdf_policy_model_get_policies(const struct xccdf_policy_model *model);

/**
 * Build all policies that can be useful for user. The useful policy is any
 * that contains at least one Rule. For example the default profile oftentimes
 * does not contain any rules.
 * @memberof xccdf_policy_model
 * @param policy_model - XCCDF Policy Model
 * @return 0 on success
 */
int xccdf_policy_model_build_all_useful_policies(struct xccdf_policy_model *policy_model);

/**
 * Get selected rules from policy
 * @memberof xccdf_policy
 * @return Pointer to select iterator.
 * @retval NULL on faliure
 */
struct xccdf_select_iterator * xccdf_policy_get_selected_rules(struct xccdf_policy *);

/**
 * Get XCCDF Profile from Policy
 * @memberof xccdf_policy
 * @return XCCDF Profile
 */
struct xccdf_profile * xccdf_policy_get_profile(const struct xccdf_policy *);

/**
 * Get rules from Policy
 * @memberof xccdf_policy
 * @return xccdf_select_iterator
 */
struct xccdf_select_iterator * xccdf_policy_get_selects(const struct xccdf_policy *);

/**
 * Get variable name from value bindings
 * @memberof xccdf_value_binding
 * @return String
 */
char * xccdf_value_binding_get_name(const struct xccdf_value_binding *);

/**
 * Get value from value bindings
 * @memberof xccdf_value_binding
 * @return String
 */
char * xccdf_value_binding_get_value(const struct xccdf_value_binding *);

/**
 * get variable type from value bindings
 * @memberof xccdf_value_binding
 * @return xccdf_value_type_t
 */
xccdf_value_type_t xccdf_value_binding_get_type(const struct xccdf_value_binding *);

/**
 * get Value operator from value bindings
 * @memberof xccdf_value_binding
 * @return xccdf_operator_t
 */
xccdf_operator_t xccdf_value_binding_get_operator(const struct xccdf_value_binding *);

/**
 * get Set Value from value bindings
 * @memberof xccdf_value_binding
 * @return String
 */
char * xccdf_value_binding_get_setvalue(const struct xccdf_value_binding *);

/**
 * Get results of all XCCDF Policy results
 * @memberof xccdf_policy_model
 */
struct xccdf_result_iterator * xccdf_policy_get_results(const struct xccdf_policy * policy);

/**
 * Get XCCDF Result structure by it's idetificator if there is one
 * @memberof xccdf_policy_model
 * @return structure xccdf_result if found, NULL otherwise
 */
struct xccdf_result * xccdf_policy_get_result_by_id(struct xccdf_policy * policy, const char * id);

/**
 * Get ID of XCCDF Profile that is implemented by XCCDF Policy
 * @param policy XCCDF Policy
 * @memberof xccdf_policy
 * @return ID of Policy's Profile
 */
const char * xccdf_policy_get_id(struct xccdf_policy * policy);

/**
 * Get XCCDF Policy from Policy model by speciefied ID of Profile
 * @param policy_model XCCDF Policy model
 * @param id ID of Profile
 * @memberof xccdf_policy_model
 * @return XCCDF Policy
 */
struct xccdf_policy * xccdf_policy_model_get_policy_by_id(struct xccdf_policy_model * policy_model, const char * id);

/************************************************************/
/** @} End of Getters group */

/************************************************************/
/**
 * @name Setters
 * For lists use add functions. Parameters of set functions are duplicated in memory and need to 
 * be freed by caller.
 * @{
 */

/**
 * Add Policy to Policy Model
 * @memberof xccdf_policy_model
 * @return true if policy has been added succesfully
 */
bool xccdf_policy_model_add_policy(struct xccdf_policy_model *, struct xccdf_policy *);

/**
 * Add rule to Policy
 * @memberof xccdf_policy
 * @return true if rule has been added succesfully
 */
bool xccdf_policy_add_select(struct xccdf_policy *, struct xccdf_select *);

/**
 * Set a new selector to the Policy structure
 * @memberof xccdf_policy
 * @return true if rule has been added succesfully
 * @deprecated This function is deprecated by @ref xccdf_policy_add_select
 * and might be dropped from future releases.
 */
OSCAP_DEPRECATED(
bool xccdf_policy_set_selected(struct xccdf_policy * policy, char * idref)
);

/**
 * Add result to XCCDF Policy Model
 * @memberof xccdf_policy_model
 */
bool xccdf_policy_add_result(struct xccdf_policy * policy, struct xccdf_result * item);

/**
 * Add value binding to the Policy structure
 * @memberof xccdf_policy
 * @return true if rule has been added succesfully
 */
bool xccdf_policy_add_value(struct xccdf_policy *, struct xccdf_value_binding *);

/**
 * Get the selection settings of the item.
 * @memberof xccdf_policy
 * @return true if the item is selected
 */
bool xccdf_policy_is_item_selected(struct xccdf_policy *policy, const char *id);

/**
 * Retrieves number of selected items in the policy
 * @note This is meant to be used to estimate scanning progress for example.
 */
int xccdf_policy_get_selected_rules_count(struct xccdf_policy *policy);

/**
 * Get select from policy by specified ID of XCCDF Item
 * @memberof xccdf_policy
 * @return XCCDF Select
 */
struct xccdf_select * xccdf_policy_get_select_by_id(struct xccdf_policy * policy, const char *item_id);

/************************************************************/
/** @} End of Setters group */

/************************************************************/
/**
 * @name Evaluators
 * @{
 * */

/**
 * Call the checking engine for each selected rule in given policy structure
 * @param policy given Policy to evaluate
 * @memberof xccdf_policy
 * @return true if evaluation pass or false in case of error
 * \par Example
 * Before each policy evaluation user has to register callback that will be called for each check.
 * Every checking engine must have registered callback or the particular check will be skipped.
 * In the code below is used the predefined function \ref oval_agent_eval_rule for evaluation OVAL checks:
 * \code
 * xccdf_policy_model_register_engine_oval(policy_mode, agent_session)
 * \endcode
 * \par
 * If you use this predefined OVAL callback, user data structure (last parameter of register function) \b MUST be of type \ref\a oval_agent_session_t:
 * \code
 * struct oval_agent_session * sess = oval_agent_new_session((struct oval_definition_model *) model, "name-of-file");
 * \endcode
 * */
struct xccdf_result *  xccdf_policy_evaluate(struct xccdf_policy * policy);

/**
 * Resolve benchmark by applying all refine_rules and refine_values to rules / values
 * of benchmark. All properties in benchmark will be irreversible changed and user has to
 * load benchmark (from XML) again to discard these changes.
 * @param policy XCCDF policy containing rules/values that will be applied to benchmark rules/values.
 * @return true if process ends succesfuly or false in case of error
 * @memberof xccdf_policy
 */
bool xccdf_policy_resolve(struct xccdf_policy * policy);

/**
 * Generate remediation prescription (presumably a remediation script).
 * @memberof xccdf_policy
 * @param policy XCCDF Policy
 * @param result XCCDF TestResult. This may be omitted to generate the prescription
 * based solely on the XCCDF Policy (xccdf:Profile).
 * @param sys Consider only those fixes that have @system attribute equal to sys
 * @param output_fd write prescription to this file descriptor
 * @returns zero on success, non-zero indicate partial (incomplete) output.
 */
int xccdf_policy_generate_fix(struct xccdf_policy *policy, struct xccdf_result *result, const char *sys, int output_fd);

/**
 * Clone the item and tailor it against given policy (profile)
 * @param policy Policy with profile
 * @param item XCCDF item to be tailored
 * @return new item that has to be freed by user
 * @deprecated This function is deprecated and might be dropped from future releases.
 */
OSCAP_DEPRECATED(struct xccdf_item * xccdf_policy_tailor_item(struct xccdf_policy * policy, struct xccdf_item * item));

/**
 * xccdf_policy_model_get_files and xccdf_item_get_files each return oscap_file_entries instead of raw strings
 */
struct oscap_file_entry;

/// @memberof oscap_file_entry
struct oscap_file_entry *oscap_file_entry_new(void);
/// @memberof oscap_file_entry
struct oscap_file_entry *oscap_file_entry_dup(struct oscap_file_entry* file_entry);
/// @memberof oscap_file_entry
void oscap_file_entry_free(struct oscap_file_entry* entry);
/// @memberof oscap_file_entry
const char* oscap_file_entry_get_system(struct oscap_file_entry* entry);
/// @memberof oscap_file_entry
const char* oscap_file_entry_get_file(struct oscap_file_entry* entry);

/** @struct oscap_file_entry_iterator
 * @see oscap_iterator
 */
struct oscap_file_entry_iterator;

/// @memberof oscap_file_entry_iterator
const struct oscap_file_entry *oscap_file_entry_iterator_next(struct oscap_file_entry_iterator *it);
/// @memberof oscap_file_entry_iterator
bool oscap_file_entry_iterator_has_more(struct oscap_file_entry_iterator *it);
/// @memberof oscap_file_entry_iterator
void oscap_file_entry_iterator_free(struct oscap_file_entry_iterator *it);
/// @memberof oscap_file_entry_iterator
void oscap_file_entry_iterator_reset(struct oscap_file_entry_iterator *it);

/** @struct oscap_file_entry_list
 * @see oscap_list
 */
struct oscap_file_entry_list;

/// @memberof oscap_file_entry_list
struct oscap_file_entry_list* oscap_file_entry_list_new(void);
/// @memberof oscap_file_entry_list
void oscap_file_entry_list_free(struct oscap_file_entry_list* list);
/// @memberof oscap_file_entry_list
struct oscap_file_entry_iterator* oscap_file_entry_list_get_files(struct oscap_file_entry_list* list);

/**
 * Return names of files that are used in checks of particular rules. Every check needs this file to be
 * evaluated properly. If this file will not be imported and bind to the XCCDF Policy system the result
 * of rule after evaluation will be "Not checked"
 *
 *
 * The resulting list should be freed with oscap_filelist_free.
 */
struct oscap_file_entry_list * xccdf_policy_model_get_systems_and_files(struct xccdf_policy_model * policy_model);

/**
 * Return names of files that are used in checks of particular rules. Every check needs this file to be
 * evaluated properly. If this file will not be imported and bind to the XCCDF Policy system the result
 * of rule after evaluation will be "Not checked"
 *
 * The resulting list should be freed with oscap_filelist_free.
 */
struct oscap_file_entry_list * xccdf_item_get_systems_and_files(struct xccdf_item * item);

/**
 * Return names of files that are used in checks of particular rules. Every check needs this file to be
 * evaluated properly. If this file will not be imported and bind to the XCCDF Policy system the result
 * of rule after evaluation will be "Not checked"
 */
struct oscap_stringlist * xccdf_policy_model_get_files(struct xccdf_policy_model * policy_model);

/**
 * Return names of files that are used in checks of particular rules. Every check needs this file to be
 * evaluated properly. If this file will not be imported and bind to the XCCDF Policy system the result
 * of rule after evaluation will be "Not checked"
 */
struct oscap_stringlist * xccdf_item_get_files(struct xccdf_item * item);

/**
 * Return result of the AND operation for two given attributes.
 * For more details about the attributes A and B please consult 'Table 26: Possible Results for a Single Test' from NISTIR-7275r4.
 * For more details about the AND operation please consult 'Table 12: Truth Table for AND' in the very same document.
 */
xccdf_test_result_type_t xccdf_test_result_resolve_and_operation(xccdf_test_result_type_t A, xccdf_test_result_type_t B);

/************************************************************/
/** @} End of Evaluators group */

/************************************************************/
/**
 * @name Iterators
 * @{
 * */

/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_policy_iterator
 */
bool xccdf_policy_iterator_has_more(struct xccdf_policy_iterator *it);

/**
 * Return the next xccdf_policy structure from the list and increment the iterator
 * @memberof xccdf_policy_iterator
 */
struct xccdf_policy * xccdf_policy_iterator_next(struct xccdf_policy_iterator *it);

/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_policy_iterator
 */
void xccdf_policy_iterator_free(struct xccdf_policy_iterator *it);

/**
 * Reset the iterator structure (it will point to the first item in the list)
 * @memberof xccdf_policy_iterator
 */
void xccdf_policy_iterator_reset(struct xccdf_policy_iterator *it);

/**
 * Return true if the list is not empty, false otherwise
 * @memberof xccdf_value_binding_iterator
 */
bool xccdf_value_binding_iterator_has_more(struct xccdf_value_binding_iterator *it);

/**
 * Return the next xccdf_value_binding structure from the list and increment the iterator
 * @memberof xccdf_value_binding_iterator
 */
struct xccdf_value_binding * xccdf_value_binding_iterator_next(struct xccdf_value_binding_iterator *it);

/**
 * Free the iterator structure (it makes no changes to the list structure)
 * @memberof xccdf_value_binding_iterator
 */
void xccdf_value_binding_iterator_free(struct xccdf_value_binding_iterator *it);

/**
 * Reset the iterator structure (it will point to the first item in the list)
 * @memberof xccdf_value_binding_iterator
 */
void xccdf_value_binding_iterator_reset(struct xccdf_value_binding_iterator *it);

/**
 * Get score of the XCCDF Benchmark
 * @param policy XCCDF Policy
 * @param test_result Test Result model
 * @param system Score system
 * @return XCCDF Score
 */
struct xccdf_score * xccdf_policy_get_score(struct xccdf_policy * policy, struct xccdf_result * test_result, const char * system);

/**
 * Get value of given value item in context of given policy
 * @memberof xccdf_policy
 * @param policy XCCDF policy
 * @param item the xccdf:Value to resolve
 * @returns string representation of resolved value_instance.
 * @retval NULL indicates failure
 */
const char *xccdf_policy_get_value_of_item(struct xccdf_policy * policy, struct xccdf_item * item);

/**
 * Perform textual substitution of cdf:sub elements with respect to given XCCDF policy.
 * @param text text to be substituted
 * @param policy policy to be used
 */
char* xccdf_policy_substitute(const char *text, struct xccdf_policy *policy);

/************************************************************/
/** @} End of Iterators group */

/**
 * @}
 */
#endif


