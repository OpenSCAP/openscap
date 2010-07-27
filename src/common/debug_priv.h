/**
 * @file debug_priv.h
 * @brief oscap debug helpers private header
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 *       Peter Vrabec <pvrabec@redhat.com>
 */ 

#pragma once
#ifndef OSCAP_DEBUG_PRIV_H_
#define OSCAP_DEBUG_PRIV_H_

#include "util.h"
#include "public/debug.h"

#if defined(NDEBUG)
# define oscap_dprintf(...) while(0)
# define oscap_dlprintf(...) while(0)
# define debug(l) if (0)
#else
# include <stdlib.h>
# include <stddef.h>
# include <stdarg.h>

enum {
	DBG_E = 1,
	DBG_W,
	DBG_I
};

# define __dlprintf_wrapper(l, ...) __oscap_dlprintf (l, __FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)

/**
 * printf-like function for writing debug messages into the output
 * file (see SEAP_DEBUG_FILE and SEAP_DEBUG_FILE_ENV).
 * @param file name of the source file
 * @param fn   name of the function
 * @param line current line
 * @param fmt  printf-like format string
 * @param ...  __oscap_dlprintf parameters
 */
void __oscap_dprintf(const char *file, const char *fn, size_t line, const char *fmt, ...);

/**
 * Convenience macro for calling __oscap_dprintf. Only the fmt & it's arguments
 * need to be specified. The __FILE__, __PRETTY_FUNCTION__ and __LINE__ macros
 * are used for the first three arguments.
 */
# define oscap_dprintf(...) __dlprintf_wrapper (0, __VA_ARGS__)

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
#endif                          /* NDEBUG */

#endif
