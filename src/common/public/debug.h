/**
 * @file
 * OpenScap debugging helpers.
 *
 * @addtogroup COMMON
 * @{
 * @addtogroup Internal
 * @{
 * @name Debugging
 */

#pragma once
#ifndef OSCAP_DEBUG_H
#define OSCAP_DEBUG_H

#ifndef oscap_dprintf
#if defined(NDEBUG)
# define oscap_dprintf(...) while(0)
#else
# include <stddef.h>
# include <stdarg.h>
void __oscap_dprintf(const char *, const char *, size_t, const char *, ...);
# define oscap_dprintf(...) __oscap_dprintf (__FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#endif				/* NDEBUG */
#endif				/* oscap_dprintf */

#ifndef OSCAP_DEBUG_FILE
# define OSCAP_DEBUG_FILE     "oscap_debug.log"
#endif

#ifndef OSCAP_DEBUG_FILE_ENV
# define OSCAP_DEBUG_FILE_ENV "OSCAP_DEBUG_FILE"
#endif

#ifndef OSCAP_DEBUG_LEVEL_ENV
# define OSCAP_DEBUG_LEVEL_ENV "OSCAP_DEBUG_LEVEL"
#endif

#ifndef NDEBUG
#include <stdlib.h>
extern int __debuglog_level;
# define debug(l) if ((__debuglog_level = (__debuglog_level == -1 ? atoi (getenv (OSCAP_DEBUG_LEVEL_ENV)) : __debuglog_level)) && __debuglog_level >= (l))
#else
# define debug(l) if (0)
#endif

#define oscap_dlprintf(l, ...) do { debug(l) { oscap_dprintf(__VA_ARGS__); }} while(0)

#endif				/* OSCAP_DEBUG_H */
