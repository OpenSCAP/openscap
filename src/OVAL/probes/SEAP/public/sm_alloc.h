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
#ifndef SM_ALLOC_H
#define SM_ALLOC_H

#include "config.h"
#include "src/common/debug_priv.h"
#include "oscap_export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __ATTRIB __attribute__ ((unused)) static

#if !defined(HAVE_POSIX_MEMALIGN)
# if defined (HAVE_MEMALIGN)
extern int posix_memalign (void ** __memptr, size_t __alignment, size_t __size);

# endif /* HAVE_MEMALIGN */
#endif /* HAVE_POSIX_MEMALIGN */

#if defined(NDEBUG)
OSCAP_API void *sm_alloc (size_t s);
OSCAP_API void *sm_calloc (size_t n, size_t s);
OSCAP_API void *sm_realloc (void *p, size_t s);
OSCAP_API void *sm_reallocf (void *p, size_t s);
OSCAP_API void  sm_free (void *p);
#else
OSCAP_API void *  __sm_alloc_dbg (size_t s, const char *f, size_t l);
__ATTRIB void *sm_alloc     (size_t s) { return __sm_alloc_dbg (s, __FUNCTION__, 0); }

OSCAP_API void *  __sm_calloc_dbg (size_t n, size_t s, const char *f, size_t l);
__ATTRIB void *sm_calloc     (size_t n, size_t s) { return __sm_calloc_dbg (n, s, __FUNCTION__, 0); }

OSCAP_API void *  __sm_realloc_dbg (void *p, size_t s, const char *f, size_t l);
__ATTRIB void *sm_realloc     (void *p, size_t s) { return __sm_realloc_dbg (p, s, __FUNCTION__, 0); }

OSCAP_API void *  __sm_reallocf_dbg (void *p, size_t s, const char *f, size_t l);
__ATTRIB void *sm_reallocf     (void *p, size_t s) { return __sm_reallocf_dbg (p, s, __FUNCTION__, 0); }

OSCAP_API void   __sm_free_dbg (void *p, const char *f, size_t l);
__ATTRIB void sm_free     (void *p) { __sm_free_dbg (p, __FUNCTION__, 0); }

# define sm_alloc(s)          __sm_alloc_dbg (s, __PRETTY_FUNCTION__, __LINE__)
# define sm_calloc(n, s)      __sm_calloc_dbg (n, s, __PRETTY_FUNCTION__, __LINE__)
# define sm_realloc(p, s)     __sm_realloc_dbg ((void *)(p), s, __PRETTY_FUNCTION__, __LINE__)
# define sm_reallocf(p, s)    __sm_reallocf_dbg ((void *)(p), s, __PRETTY_FUNCTION__, __LINE__)
# define sm_free(p)           __sm_free_dbg ((void *)(p), __PRETTY_FUNCTION__, __LINE__)
#endif

#define  sm_talloc(T) ((T *) sm_alloc(sizeof(T)))
#define  sm_valloc(v) ((typeof(v) *) sm_alloc(sizeof v))

#include <assert.h>

#ifdef __cplusplus
}
#endif

#endif /* SM_ALLOC_H */
