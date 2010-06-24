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
 * Clean the system characteristics of objects, probe cashe and results
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
typedef xccdf_test_result_type_t (xccdf_policy_eval_rule_cb_t) (struct xccdf_policy * policy, const char * rule_id,
        const char * id, struct xccdf_value_binding_iterator * it, void * usr);

xccdf_policy_eval_rule_cb_t oval_agent_eval_rule;

struct oval_agent_session * oval_agent_cb_data_get_session(const struct oval_agent_cb_data *);
oval_agent_result_cb_t * oval_agent_cb_data_get_callback(const struct oval_agent_cb_data *);
void * oval_agent_cb_data_get_usr(const struct oval_agent_cb_data *);
bool oval_agent_cb_data_set_session(struct oval_agent_cb_data *, struct oval_agent_session *);
bool oval_agent_cb_data_set_callback(struct oval_agent_cb_data *, oval_agent_result_cb_t *);
bool oval_agent_cb_data_set_usr(struct oval_agent_cb_data *, void *);
struct oval_agent_cb_data * oval_agent_cb_data_new(void);
void oval_agent_cb_data_free(struct oval_agent_cb_data * data);
void oval_agent_resolve_variables(struct oval_agent_session * session, struct xccdf_value_binding_iterator *it);
void oval_agent_export_sysinfo_to_xccdf_result(struct oval_agent_session * sess, struct xccdf_result * ritem);
#endif

/**
 * @) END OVALDEF
 * @) END OVALAGENT
 */
#endif				/**OVAL_AGENT_API_H_ */


