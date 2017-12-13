/**
 * @file   oval_probe_session.h
 * @brief  OVAL probe handler API public header
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * @addtogroup PROBEHANDLERS
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
#ifndef OVAL_PROBE_HANDLER
#define OVAL_PROBE_HANDLER

#include "oval_definitions.h"
#include "oscap_export.h"

typedef struct oval_phtbl oval_phtbl_t;
typedef struct oval_ph    oval_ph_t;

/**
 * Type of the handler function. This function takes care of handling
 * all the actions defined bellow, that is: initialization, freeing,
 * opening, evaluating, reseting and closing (whatever that means in
 * your particular case).
 */
OSCAP_API typedef int (oval_probe_handler_t)(oval_subtype_t, void *, int, ...);

#define PROBE_HANDLER_ACT_INIT  0
#define PROBE_HANDLER_ACT_FREE  1
#define PROBE_HANDLER_ACT_OPEN  2
#define PROBE_HANDLER_ACT_EVAL  3
#define PROBE_HANDLER_ACT_RESET 4
#define PROBE_HANDLER_ACT_CLOSE 5
#define PROBE_HANDLER_ACT_ABORT 6

#define PROBE_HANDLER_IGNORE NULL

#endif /* OVAL_PROBE_HANDLER */
/// @}
