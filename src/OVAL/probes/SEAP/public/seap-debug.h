/**
 * @file   seap-debug.h
 * @brief  SEAP debug helpers public header
 * @author Daniel Kopecek <dkopecek@redhat.com>
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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#pragma once
#ifndef SEAP_DEBUG_H
#define SEAP_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sexp-types.h"
#include <assert.h>
#ifndef _A
#define _A(x) assert(x)
#endif

#ifndef _D
#if defined(NDEBUG)
# define _D(...) while(0)
# define _W(...) while(0)
# define _I(...) while(0)
# define _F(...) while(0)
# define _S(sexp) while(0)
# define _LOGCALL_ while(0)
#else
# include <stddef.h>
# include <stdarg.h>
  /**
   * printf-like function for writing debug messages into the output
   * file (see SEAP_DEBUG_FILE and SEAP_DEBUG_FILE_ENV).
   * @param prefix  debug message prefix
   * @param srcfile name of the source file
   * @param srcfn   name of the function
   * @param srcln   line
   * @param fmt     printf-like format string
   */
void __seap_debuglog (const char *prefix, const char *srcfile, const char *srcfn, size_t srcln, const char *fmt, ...);

void __seap_debuglog_sexp (const char *file, const char *fn, size_t line, SEXP_t *sexp);
  /**
   * Convenience macro for calling __seap_debuglog. Only the fmt & it's arguments
   * need to be specified. The __FILE__, __PRETTY_FUNCTION__ and __LINE__ macros
   * are used for the first three arguments.
   */
# define _D(...) __seap_debuglog("DEBUG: ", __FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
# define _W(...) __seap_debuglog("WARN: ",  __FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
# define _I(...) __seap_debuglog("INFO: ",  __FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
# define _F(...) __seap_debuglog("FAIL: ",  __FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
# define _S(sexp) __seap_debuglog_sexp(__FILE__, __PRETTY_FUNCTION__, __LINE__, sexp)

# if defined(SEAP_VERBOSE_DEBUG)
#  define _LOGCALL_ _D("called\n")
# else
#  define _LOGCALL_ while(0)
# endif
#endif /* NDEBUG */
#endif /* _D */
  /**
   * Hardcoded output filename.
   */
#define SEAP_DEBUG_FILE "probe_debug.log"
  /**
   * Name of the environment variable that can be used to change the
   * default output filename.
   */
#define SEAP_DEBUG_FILE_ENV "SEAP_DEBUG_FILE"

#ifdef __cplusplus
}
#endif

#endif /* SEAP_DEBUG_H */
