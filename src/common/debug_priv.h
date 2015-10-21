/**
 * @file debug_priv.h
 * @brief oscap debug helpers private header
 */
/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *       Daniel Kopecek <dkopecek@redhat.com>
 *       Peter Vrabec <pvrabec@redhat.com>
 */

#pragma once
#ifndef OSCAP_DEBUG_PRIV_H_
#define OSCAP_DEBUG_PRIV_H_

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include "util.h"

OSCAP_HIDDEN_START;

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

/**
 * Name of the environment variable that can be used to enable
 * striping of __FILE__ paths. It's numeric value specifies how
 * many path tokens from the end should remain after striping.
 * Value 1 is equal to basename(__FILE__)
 * If this environment variable isn't defined or is set to 0
 * then no striping is done.
 */
#ifndef OSCAP_DEBUG_PATHSTRIP_ENV
# define OSCAP_DEBUG_PATHSTRIP_ENV "OSCAP_DEBUG_PSTRIP"
#endif


#define OSCAP_DEBUGOBJ_SEXP 1

#ifndef _A
#define _A(x) assert(x)
#endif


enum oscap_verbosity_levels {
	DBG_E = 1,
	DBG_W,
	DBG_I,
	DBG_D,
	DBG_UNKNOWN = -1
};

# define __dlprintf_wrapper(l, ...) __oscap_dlprintf (l, __FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)

extern int __debuglog_level;

/**
 * Using this macro you can create a "debug block" with a verbosity level `l'.
 * Example:
 *  The following code inside the debug block will be executed only if the debug level
 *  is larger that or equal to 3.
 *
 * debug(3) {
 *   int foo;
 *   foo = do_something_only_in_debug_mode();
 *   ...
 * }
 *
 */
# define debug(l) if ((__debuglog_level = (__debuglog_level == -1 ? atoi (getenv (OSCAP_DEBUG_LEVEL_ENV) == NULL ? "0" : getenv (OSCAP_DEBUG_LEVEL_ENV)) : __debuglog_level)) && __debuglog_level >= (l))

/**
 * Version of the oscap_dprintf function with support for debug level.
 * Uses logic similar to the debug() macro described above.
 * @param level debug level
 * @param file  name of the source file
 * @param fn    name of the function
 * @param line  current line
 * @param fmt   printf-line format string
 * @param ...   __oscap_dlprintf parameters
 */
void __oscap_dlprintf(int level, const char *file, const char *fn, size_t line, const char *fmt, ...);

/**
 * Convenience macro for calling __oscap_dlprintf. Only the fmt & it's arguments
 * need to be specified. The __FILE__, __PRETTY_FUNCTION__ and __LINE__ macros
 * are used for the first three arguments.
 */
# define oscap_dlprintf(l, ...) __dlprintf_wrapper (l, __VA_ARGS__)

void __oscap_debuglog_object (const char *file, const char *fn, size_t line, int objtype, void *obj);

# define dO(type, obj) __oscap_debuglog_object(__FILE__, __PRETTY_FUNCTION__, __LINE__, type, obj)


#define dI(...) oscap_dlprintf(DBG_I, __VA_ARGS__)
#define dW(...) oscap_dlprintf(DBG_W, __VA_ARGS__)
#define dE(...) oscap_dlprintf(DBG_E, __VA_ARGS__)
#define dD(...) oscap_dlprintf(DBG_D, __VA_ARGS__)

OSCAP_HIDDEN_END;

#endif
