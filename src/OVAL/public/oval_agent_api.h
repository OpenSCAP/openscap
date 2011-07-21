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
#include "reporter.h"

#ifdef ENABLE_XCCDF
#include "xccdf_policy.h"
#endif

struct oval_agent_session;

/**
 * @var oval_agent_session_t
 * Agent session consists of connection to system checking engine,
 * definition model, system characteristics model and results model.
 */
typedef struct oval_agent_session oval_agent_session_t;

/**
 * Create new session for OVAL agent from OVAL definition model
 * @param model OVAL Definition model
 * @param name Name of file that can be referenced from XCCDF Benchmark
 */
oval_agent_session_t * oval_agent_new_session(struct oval_definition_model * model, const char * name);

/**
 * Probe the system and evaluate specified definition
 * @return 0 on success; -1 error; 1 warning
 */
int oval_agent_eval_definition(oval_agent_session_t *, const char *);

/**
 * Get the OVAL result of a definition from an agent session
 * @return 0 on success; -1 error
 */
int oval_agent_get_definition_result(oval_agent_session_t *, const char *, oval_result_t *);

/**
 * Clean resuls that were generated in this agent session
 */
int oval_agent_reset_session(oval_agent_session_t * ag_sess);

/**
 * Abort a running probe session
 */
int oval_agent_abort_session(oval_agent_session_t *ag_sess);

/**
 * Probe and evaluate all definitions from the content, call the callback functions upon single evaluation
 * @return 0 on success; -1 error; 1 warning
 */
int oval_agent_eval_system(oval_agent_session_t * ag_sess, oscap_reporter cb, void *arg);

/**
 * Get a result model from agent session
 */
struct oval_results_model * oval_agent_get_results_model(oval_agent_session_t * ag_sess);
/**
 * Get a filename under which was created
 */
const char * oval_agent_get_filename(oval_agent_session_t * ag_sess);

/**
 * Finish OVAL agent session
 */
void oval_agent_destroy_session(oval_agent_session_t * ag_sess);


#ifdef ENABLE_XCCDF

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
 *  struct oval_definition_model * def_model = oval_definition_model_import(oval_file);
 *  struct xccdf_benchmark * benchmark = xccdf_benchmark_import(file);
 *  struct xccdf_policy_model * policy_model = xccdf_policy_model_new(benchmark);
 *  struct oval_agent_session * sess = oval_agent_new_session(def_model, "name-of-file");
 *  ...
 *  xccdf_policy_model_register_engine_callback(policy_model, "http://oval.mitre.org/XMLSchema/oval-definitions-5", oval_agent_eval_rule, (void *) sess);
 * \endcode
 * 
 */
xccdf_test_result_type_t oval_agent_eval_rule (struct xccdf_policy * policy, const char * rule_id,
        const char * id, const char * href, struct xccdf_value_binding_iterator * it, void * usr);

/**
 * Resolve variables from XCCDF Value Bindings and set their values to OVAL Variables
 * @param session OVAL Agent Session
 * @param it XCCDF Value Bindng iterator
 * @return 0 if resolving pass
 * \par Example
 * Example in oval_agent.c in function oval_agent_eval_rule
 */
int oval_agent_resolve_variables(struct oval_agent_session * session, struct xccdf_value_binding_iterator *it);


/**
 * Function to register predefined oval callback for XCCDF evaluation proccess
 * @param model XCCDF Policy Model
 * @param sess oval_agent_session_t parameter for passing session data to callback
 * @memberof xccdf_policy_model
 * @return true if callback registered succesfully, false otherwise
 */
bool xccdf_policy_model_register_engine_oval(struct xccdf_policy_model * model, struct oval_agent_session * sess);

/**
 * Transform OVAL Sysinfo into XCCDF Test Result
 * @param session OVAL Agent session
 * @param ritem XCCDF Result
 */
void oval_agent_export_sysinfo_to_xccdf_result(struct oval_agent_session * session, struct xccdf_result * ritem);

/************************************************************/
/** @} End of Evaluators group */

#endif

/**
 * @) END OVALDEF
 * @) END OVALAGENT
 */
#endif				/**OVAL_AGENT_API_H_ */


