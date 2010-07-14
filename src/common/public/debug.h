/**
 * @file
 * OpenScap debugging helpers.
 *
 * @addtogroup COMMON
 * @{
 * @addtogroup Debug
 * @{
 * Debuging mechanism.
 *
 * If you compile library with enabled debugging support, you will get 
 * log messages in file named according to OSCAP_DEBUG_FILE macro. You can
 * change log file name by setting OSCAP_DEBUG_FILE environment variable
 * to your needs.
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 */

#pragma once
#ifndef OSCAP_DEBUG_H
#define OSCAP_DEBUG_H

/**
 * Hardcodede output filename. This is used as the default
 * filename if the variable specified by OSCAP_DEBUG_FILE_ENV
 * isn't defined.
 */
#ifndef OSCAP_DEBUG_FILE
# define OSCAP_DEBUG_FILE     "oscap_debug.log"
#endif

/**
 * Name of the environment variable that can be used to change
 * the default output filename.
 */
#ifndef OSCAP_DEBUG_FILE_ENV
# define OSCAP_DEBUG_FILE_ENV "OSCAP_DEBUG_FILE"
#endif

/**
 * Name of the environment variable that can be used to change
 * the debug level, i.e. the number and verbosity of the debug
 * messages.
 * XXX: not implemented yet
 */
#ifndef OSCAP_DEBUG_LEVEL_ENV
# define OSCAP_DEBUG_LEVEL_ENV "OSCAP_DEBUG_LEVEL"
#endif

#endif				/* OSCAP_DEBUG_H */
