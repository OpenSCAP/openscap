/**
 * @file   oval_probe_h
 * @brief  OVAL probe interface API public header
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * @addtogroup PROBEINTERFACE
 * @{
 */
/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
#pragma once
#ifndef OVAL_PROBE_H
#define OVAL_PROBE_H

#include <stdarg.h>
#include <stdint.h>
#include "oval_definitions.h"
#include "oval_system_characteristics.h"
#include "oval_probe_session.h"

/*
 * probe session flags
 */
#define OVAL_PDFLAG_NOREPLY  0x0001	/**< don't send probe result to library - just an ack */
#define OVAL_PDFLAG_NORECONN 0x0002	/**< don't try to reconnect on fatal errors */
#define OVAL_PDGLAG_RUNALL   0x0004	/**< execute all probes when executing the first */
#define OVAL_PDFLAG_RUNNOW   0x0008	/**< execute all probes immediately */

#define OVAL_PDFLAG_MASK (0x0001|0x0002|0x0004|0x0008)

/**
 * Evaluate system info probe
 * @param sess probe session
 */
struct oval_sysinfo *oval_probe_query_sysinfo(oval_probe_session_t *sess) __attribute__ ((nonnull(1)));

/**
 * Evaluate an object
 * @param sess probe session
 * @param object the object to evaluate
 */
int oval_probe_query_object(oval_probe_session_t *psess, struct oval_object *object, int flags, struct oval_syschar **out_syschar) __attribute__ ((nonnull(1, 2)));

/**
 * Probe all objects and update system characteristic model in the session
 * @param sess probe session
 * @return 0 on success
 */
int oval_probe_query_objects(oval_probe_session_t *sess) __attribute__ ((nonnull(1)));

/**
 * Probe objects required for the evalatuation of the specified definition and update the system characteristics model associated with the session
 * @param sess probe session
 * @param id definition id
 * @return 0 on success
 */
int oval_probe_query_definition(oval_probe_session_t *sess, const char *id) __attribute__ ((nonnull(1, 2)));

/**
 * Query the specified variable and all its dependencies in order to compute the vector of its values
 * @param sess probe session
 * @param variable the variable to query
 * @return 0 on success
 */
int oval_probe_query_variable(oval_probe_session_t *sess, struct oval_variable *variable);

#endif				/* OVAL_PROBE_H */
