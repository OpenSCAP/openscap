/**
 * @file   oval_probe.h
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

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "oval_definitions.h"
#include "oval_system_characteristics.h"
#include "oval_probe_session.h"
#include "oscap_export.h"

/*
 * probe session flags
 */
#define OVAL_PDFLAG_NOREPLY  0x0001	/**< don't send probe result to library - just an ack */
#define OVAL_PDFLAG_NORECONN 0x0002	/**< don't try to reconnect on fatal errors */
#define OVAL_PDGLAG_RUNALL   0x0004	/**< execute all probes when executing the first */
#define OVAL_PDFLAG_RUNNOW   0x0008	/**< execute all probes immediately */
#define OVAL_PDFLAG_SLAVE    0x0010

#define OVAL_PDFLAG_MASK (0x0001|0x0002|0x0004|0x0008|0x0010)

/**
 * Evaluate system info probe
 * @param sess probe session
 * @param out_sysinfo address of a pointer to hold the result
 */
OSCAP_API int oval_probe_query_sysinfo(oval_probe_session_t *sess, struct oval_sysinfo **out_sysinfo) __attribute__ ((nonnull(1, 2)));

/**
 * Evaluate an object
 * @param sess probe session
 * @param object the object to evaluate
 */
OSCAP_API int oval_probe_query_object(oval_probe_session_t *psess, struct oval_object *object, int flags, struct oval_syschar **out_syschar) __attribute__ ((nonnull(1, 2)));

/**
 * Probe objects required for the evalatuation of the specified definition and update the system characteristics model associated with the session
 * @param sess probe session
 * @param id definition id
 * @return 0 on success; -1 on error; 1 warning
 */
OSCAP_API OSCAP_DEPRECATED(int oval_probe_query_definition(oval_probe_session_t *sess, const char *id)) __attribute__ ((nonnull(1, 2)));

/**
 * Query the specified variable and all its dependencies in order to compute the vector of its values
 * @param sess probe session
 * @param variable the variable to query
 * @return 0 on success
 */
OSCAP_API int oval_probe_query_variable(oval_probe_session_t *sess, struct oval_variable *variable);

#define OVAL_PROBEMETA_LIST_VERBOSE 0x00000001 /**< Be verbose when listing supported probes */
#define OVAL_PROBEMETA_LIST_DYNAMIC 0x00000002 /**< Perform additional checks when listing supported probes (i.e. list only existing external probes) */
#define OVAL_PROBEMETA_LIST_OTYPE   0x00000004 /**< Show the otype / family type of the probe */

OSCAP_API void oval_probe_meta_list(FILE *output, int flags);

OSCAP_API const char *oval_probe_ext_getdir(void);
#endif				/* OVAL_PROBE_H */
/// @}
