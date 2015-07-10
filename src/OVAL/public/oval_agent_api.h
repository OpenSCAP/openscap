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
//#include "oval_probe.h"

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
 * Retrieves OVAL definition model associated with given session
 */
struct oval_definition_model* oval_agent_get_definition_model(oval_agent_session_t* ag_sess);

/**
 * Set a product name for the provided agent session. The
 * product name should be used for all newly created OVAL documents. If
 * there already are some models in the session, they are modified as
 * well.
 */
void oval_agent_set_product_name(oval_agent_session_t *, char *);

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
 * Get the OVAL result definition from an agent session
 * @return NULL if not found
 */
struct oval_result_definition * oval_agent_get_result_definition(oval_agent_session_t *ag_sess, const char *id);

/**
 * Clean resuls that were generated in this agent session
 */
int oval_agent_reset_session(oval_agent_session_t * ag_sess);

/**
 * Abort a running probe session
 */
int oval_agent_abort_session(oval_agent_session_t *ag_sess);

typedef int (*agent_reporter)(const struct oval_result_definition * res_def, void *arg);

/**
 * Probe and evaluate all definitions from the content, call the callback functions upon single evaluation
 * @return 0 on success; -1 error; 1 warning
 */
int oval_agent_eval_system(oval_agent_session_t * ag_sess, agent_reporter cb, void *arg);

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


/**
 * @} END OVALDEF
 * @} END OVALAGENT
 */
#endif				/**OVAL_AGENT_API_H_ */


