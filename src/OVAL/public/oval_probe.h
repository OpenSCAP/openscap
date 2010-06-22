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
 * probe context flags
 */
#define OVAL_PDFLAG_NOREPLY  0x0001	/** don't send probe result to library - just an ack */
#define OVAL_PDFLAG_NORECONN 0x0002	/** don't try to reconnect on fatal errors */
#define OVAL_PDGLAG_RUNALL   0x0004	/** execute all probes when executing the first */
#define OVAL_PDFLAG_RUNNOW   0x0008	/** execute all probes immediately */

#define OVAL_PDFLAG_MASK (0x0001|0x0002|0x0004|0x0008)

/*
 * probe context attributes
 */
#define OVAL_PCTX_ATTR_RETRY       0x0001	/* set maximum retry count (send, connect) */
#define OVAL_PCTX_ATTR_RECVTIMEOUT 0x0002	/* set receive timeout - in miliseconds */
#define OVAL_PCTX_ATTR_SENDTIMEOUT 0x0003	/* set send timeout - in miliseconds */
#define OVAL_PCTX_ATTR_SCHEME      0x0004	/* set communication scheme */
#define OVAL_PCTX_ATTR_DIR         0x0005	/* set directory where the probes are located */
#define OVAL_PCTX_ATTR_MODEL       0x0006	/* set definition model - update registered commands is necessary */

/**
 * Evaluate system info probe
 * @param pctx probe context
 */
struct oval_sysinfo *oval_probe_sysinfo_query(oval_probe_session_t *sess) __attribute__ ((nonnull(1)));

/**
 * Evaluate an object
 * @param pctx probe context
 * @param object the object to evaluate
 */
struct oval_syschar *oval_probe_object_query(oval_probe_session_t *sess, struct oval_object *object, int flags) __attribute__ ((nonnull(1, 2)));

/**
 * Probe system info and update system characteristic model in the session
 * @param sess probe session
 * @return 0 on success
 */
int oval_probe_session_query_sysinfo(oval_probe_session_t *sess) __attribute__ ((nonnull(1)));

/**
 * Probe all objects and update system characteristic model in the session
 * @param sess probe session
 * @return 0 on success
 */
int oval_probe_session_query_objects(oval_probe_session_t *sess) __attribute__ ((nonnull(1)));

/**
 * Probe objects required for evalatuation specified definition and update system characteristic model in the session
 * @param sess probe session
 * @param id definition id
 * @return 0 on success
 */
int oval_probe_session_query_definition(oval_probe_session_t *sess, const char *id) __attribute__ ((nonnull(1, 2)));

#endif				/* OVAL_PROBE_H */
