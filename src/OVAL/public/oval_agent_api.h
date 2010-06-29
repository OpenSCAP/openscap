/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALAGENT
 * OVAL Agent interface
 *
 * This is a high level API for system probing and OVAL Definition content evaluation.
 * @{
 *
 * @file
 *
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
 * @author "Peter Vrabec" <pvrabec@gmail.com>
 */

/*
 * Copyright 2009,2010 Red Hat Inc., Durham, North Carolina.
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


#ifndef OVAL_AGENT_API_H_
#define OVAL_AGENT_API_H_

#include "oval_definitions.h"
#include "oval_system_characteristics.h"
#include "oval_results.h"
#include "oval_variables.h"
#include "oval_probe.h"

#ifdef ENABLE_XCCDF
#include "xccdf_policy.h"

/**
 * @struct oval_agent_cb_data
 * Handle all callback data that are needed by xccdf_policy_eval_rule_cb_t
 */
struct oval_agent_cb_data;

#endif

struct oval_agent_session;

/**
 * @var oval_agent_session_t
 * Agent session consists of connection to system checking engine,
 * definition model, system characteristics model and results model.
 */
typedef struct oval_agent_session oval_agent_session_t;

/**
 * @var oval_agent_result_cb_t
 * This callback is called after evaluation of each definition.
 * @param id definition id that was evaluated
 * @param result definition result
 */
typedef int (oval_agent_result_cb_t) (const char *id, int result, void *arg);

/**
 * Create new session for OVAL agent from OVAL definition model
 */
oval_agent_session_t * oval_agent_new_session(struct oval_definition_model * model);

/**
 * Probe the system and evaluate specified definition
 */
oval_result_t oval_agent_eval_definition(oval_agent_session_t * ag_sess, const char *id);

/**
 * Clean resuls that were generated in this agent session
 */
int oval_agent_reset_session(oval_agent_session_t * ag_sess);

/**
 * Probe and evaluate all definitions from the content, call the callback functions upon single evaluation
 */
int oval_agent_eval_system(oval_agent_session_t * ag_sess, oval_agent_result_cb_t * cb, void *arg);

/**
 * Get a result model from agent session
 */
struct oval_results_model * oval_agent_get_results_model(oval_agent_session_t * ag_sess);

/**
 * Finish OVAL agent session
 */
void oval_agent_destroy_session(oval_agent_session_t * ag_sess);


#ifdef ENABLE_XCCDF

/************************************************************/
/**
 * @name Getters
 * Return value is pointer to structure's member. Do not free unless you null the pointer in the structure. 
 * Use remove function otherwise.
 * @{
 * */

/**
 * Get session of OVAL Agent callback data
 * @param data OVAL Agent callback data
 * @memberof oval_agent_cb_data
 * @return oval_agent_session structure
 */
struct oval_agent_session * oval_agent_cb_data_get_session(const struct oval_agent_cb_data * data);

/**
 * Get callback of OVAL Agent callback data
 * @param data OVAL Agent callback data
 * @memberof oval_agent_cb_data
 * @return oval_agent_result_cb_t callback
 */
oval_agent_result_cb_t * oval_agent_cb_data_get_callback(const struct oval_agent_cb_data * data);

/**
 * Get usr data of OVAL Agent callback data
 * @param data OVAL Agent callback data
 * @memberof oval_agent_cb_data
 * @return void pointer to user data structure
 */
void * oval_agent_cb_data_get_usr(const struct oval_agent_cb_data * data);

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
 * Set the OVAL Agent session to OVAL Agent callback data
 * @param data Oval Agent callback data
 * @param session OVAL Agent Session
 * @memberof oval_agent_cb_data
 * @return Boolean
 */
bool oval_agent_cb_data_set_session(struct oval_agent_cb_data * data, struct oval_agent_session * session);

/**
 * Set the OVAL Agent Result callback to OVAL Agent callback data
 * @param data Oval Agent callback data
 * @param callback OVAL Agent Result callback
 * @memberof oval_agent_cb_data
 * @return Boolean
 */
bool oval_agent_cb_data_set_callback(struct oval_agent_cb_data * data, oval_agent_result_cb_t * callback, void * usr);

/**
 * Set the User data to OVAL Agent callback data
 * @param data Oval Agent callback data
 * @param usr User data
 * @memberof oval_agent_cb_data
 * @return Boolean
 */
bool oval_agent_cb_data_set_usr(struct oval_agent_cb_data * data, void * usr);

/************************************************************/
/** @} End of Setters group */

/************************************************************/
/**
 * @name Evaluators
 * @{
 * */

/**
 * @param policy XCCDF Policy that is being evaluated
 * @param rule_id ID of XCCDF Rule
 * @param id ID of OVAL definition
 * @param it XCCDF Value Binding iterator with value bindings
 * @param usr Void pointer to the user data structure
 * @return XCCDF test result type of evaluated rule
 *
 */
typedef xccdf_test_result_type_t (xccdf_policy_eval_rule_cb_t) (struct xccdf_policy * policy, const char * rule_id,
        const char * id, struct xccdf_value_binding_iterator * it, void * usr);

/**
 * Internal OVAL Agent Callback that can be used to evaluate XCCDF content.
 * \par Example
 * Next example shows common use of this function in evaluation proccess of XCCDF file.
 * \par
 * \code
 *  struct xccdf_benchmark * benchmark = xccdf_benchmark_import(file);
 *  struct xccdf_policy_model * policy_model = xccdf_policy_model_new(benchmark);
 *  struct oval_agent_cb_data * usr = oval_agent_cb_data_new();
 *  ...
 *  xccdf_policy_model_register_callback(policy_model, "http://oval.mitre.org/XMLSchema/oval-definitions-5", oval_agent_eval_rule, (void *) usr);
 * \endcode
 * 
 */
xccdf_test_result_type_t oval_agent_eval_rule (struct xccdf_policy * policy, const char * rule_id,
        const char * id, struct xccdf_value_binding_iterator * it, void * usr);

/**
 * Resolve variables from XCCDF Value Bindings and set their values to OVAL Variables
 * @param session OVAL Agent Session
 * @param it XCCDF Value Bindng iterator
 * \par Example
 * Example in oval_agent.c in function oval_agent_eval_rule
 */
void oval_agent_resolve_variables(struct oval_agent_session * session, struct xccdf_value_binding_iterator *it);

/**
 * Transform OVAL Sysinfo into XCCDF Test Result
 * @param session OVAL Agent session
 * @param ritem XCCDF Result
 */
void oval_agent_export_sysinfo_to_xccdf_result(struct oval_agent_session * session, struct xccdf_result * ritem);

/************************************************************/
/** @} End of Evaluators group */

/**
 * Create new OVAL Agent callback data
 * \par Structure:
 * \arg oval_agent_session
 * \arg oval_agent_result_cb_t
 * \arg usr
 * \par Example
 * \code
 *  struct oval_agent_cb_data *usr = oval_agent_cb_data_new();
 *  oval_agent_cb_data_set_session(usr, session);
 *  oval_agent_cb_data_set_callback(usr, callback);
 *  oval_agent_cb_data_set_usr(usr, (void *) usr_structure);
 * \endcode
 * @memberof oval_agent_cb_data
 */
struct oval_agent_cb_data * oval_agent_cb_data_new(void);

/**
 * Free function of OVAL Agent callback data
 * @param data OVAL Agent callback data
 * @memberof oval_agent_cb_data
 */
void oval_agent_cb_data_free(struct oval_agent_cb_data * data);
#endif

/**
 * @) END OVALDEF
 * @) END OVALAGENT
 */
#endif				/**OVAL_AGENT_API_H_ */


