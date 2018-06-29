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
 * @author "Peter Vrabec" <pvrabec@gmail.com>
 */

/*
 * Copyright 2010,2011 Red Hat Inc., Durham, North Carolina.
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


#ifndef OVAL_AGENT_XCCDF_API_H_
#define OVAL_AGENT_XCCDF_API_H_

#include <oscap.h>
#include "oval_agent_api.h"
#include "xccdf_policy.h"
#include "oscap_export.h"

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
 *
 * You can either register this function with xccdf_policy (old fashioned way
 * as described in the example bellow). Alternativelly you can use high level
 * function xccdf_policy_model_register_engine_oval() (recommended) which will
 * register the oval_engine.
 *
 * \par Example
 * Next example shows common use of this function in evaluation proccess of XCCDF file.
 * \par
 * \code
OSCAP_API  *  struct oval_definition_model * def_model = oval_definition_model_import(oval_file);
OSCAP_API  *  struct xccdf_benchmark * benchmark = xccdf_benchmark_import(file);
OSCAP_API  *  struct xccdf_policy_model * policy_model = xccdf_policy_model_new(benchmark);
OSCAP_API  *  struct oval_agent_session * sess = oval_agent_new_session(def_model, "name-of-file");
 *  ...
OSCAP_API  *  xccdf_policy_model_register_engine_and_query_callback(policy_model, "http://oval.mitre.org/XMLSchema/oval-definitions-5", oval_agent_eval_rule, (void *) sess, NULL);
 * \endcode
 * 
 */
xccdf_test_result_type_t oval_agent_eval_rule(struct xccdf_policy * policy, const char * rule_id, const char * id, const char * href,
		struct xccdf_value_binding_iterator * it,
		struct xccdf_check_import_iterator * check_import_it,
		void * usr);

/**
 * Resolve variables from XCCDF Value Bindings and set their values to OVAL Variables
 * @param session OVAL Agent Session
 * @param it XCCDF Value Bindng iterator
 * @return 0 if resolving pass
 * \par Example
 * Example in oval_agent.c in function oval_agent_eval_rule
 */
OSCAP_API int oval_agent_resolve_variables(struct oval_agent_session * session, struct xccdf_value_binding_iterator *it);


/**
 * Function to register predefined oval callback for XCCDF evaluation proccess
 * @param model XCCDF Policy Model
 * @param sess oval_agent_session_t parameter for passing session data to callback
 * @memberof xccdf_policy_model
 * @return true if callback registered succesfully, false otherwise
 */
OSCAP_API bool xccdf_policy_model_register_engine_oval(struct xccdf_policy_model * model, struct oval_agent_session * sess);

/**
 * @} END OVALDEF
 * @} END OVALAGENT
 */
#endif				/**OVAL_AGENT_XCCDF_API_H_ */


