/**
 * @file oval_probe_session.h
 * @brief OVAL probe session API public header
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * @addtogroup PROBESESSION
 * @{
 */
/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */
#ifndef OVAL_PROBE_SESSION
#define OVAL_PROBE_SESSION

typedef struct oval_probe_session oval_probe_session_t;

#include "oval_probe_handler.h"
#include "oval_system_characteristics.h"
#include "oval_results.h"

/**
 * Create and initialize a new probe session
 * @param model system characteristics model
 */
oval_probe_session_t *oval_probe_session_new(struct oval_syschar_model *model);

/**
 * Create and initialize a new probe session
 * @param model system characteristics model
 * @param result_system OVAL results system
 */
oval_probe_session_t *oval_probe_session_with_result_system_new(struct oval_syschar_model *model, struct oval_result_system *result_system);

/**
 * Destroy probe session. All state information created during the lifetime
 * of the session is freed, resources used by probes are freed using the probe
 * handler API.
 * @param sess pointer to the probe session structure
 */
void oval_probe_session_destroy(oval_probe_session_t *sess);

/**
 * Send a close request to all probes. In case of external probes, the running
 * processes are shutdown - all cached results are lost.
 * @param sess pointer to the probe session structure
 */
int oval_probe_session_close(oval_probe_session_t *sess);

/**
 * Reset the session. All state information created during the lifetime of the
 * session is freed and reset to its initial state. All cached results are lost.
 * @param sess pointer to the probe session structure
 * @param sysch pointer to a new syschar model or NULL
 */
int oval_probe_session_reset(oval_probe_session_t *sess, struct oval_syschar_model *sysch);

/**
 * Abort the session.
 */
int oval_probe_session_abort(oval_probe_session_t *sess);

/**
 * Set a new handler for an object of the specified type.
 * @param sess pointer to the probe session structure
 * @param type object type
 * @param handler
 * @param ptr user pointer that will be passed to the handler on each invocation of the handler
 */
int oval_probe_session_sethandler(oval_probe_session_t *sess, oval_subtype_t type, oval_probe_handler_t handler, void *ptr);

/**
 * Get system characteristics model from probe session.
 * @param sess pointer to the probe session structure
 */
struct oval_syschar_model *oval_probe_session_getmodel(oval_probe_session_t *sess);

#endif /* OVAL_PROBE_SESSION */
/// @}
