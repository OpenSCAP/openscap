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

#endif				/* OSCAP_DEBUG_H */
