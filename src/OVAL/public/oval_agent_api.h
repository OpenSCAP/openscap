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


/**
 * @var oval_agent_session_t
 * Agent session consists of connection to system checking engine,
 * definition model, system characteristics model and results model.
 */
typedef struct oval_agent_session oval_agent_session_t;

/**
 * @var oval_agent_cb_t
 * This callback is called after evaluation of each definition.
 * @param id definition id that was evaluated
 * @param result definition result
 */
typedef int (oval_agent_cb_t) (const char *id, oval_result_t result, void *arg);

/**
 * Create new session for OVAL agent from OVAL definition model
 */
oval_agent_session_t * oval_agent_new_session(struct oval_definition_model * model);

/**
 * Probe the system and evaluate specified definition
 */
oval_result_t oval_agent_eval_definition(oval_agent_session_t * asess, const char *id);

/**
 * Probe and evaluate all definitions from the content, call the callback functions upon single evaluation
 */
int oval_agent_eval_system(oval_agent_session_t * asess, oval_agent_cb_t * cb, void *arg);

/**
 * Get a result model from agent session
 */
struct oval_results_model * oval_agent_get_results_model(oval_agent_session_t * asess);

/**
 * Finish OVAL agent session
 */
void oval_agent_destroy_session(oval_agent_session_t * asess);

/**
 * @) END OVALDEF
 * @) END OVALAGENT
 */
#endif				/**OVAL_AGENT_API_H_ */


