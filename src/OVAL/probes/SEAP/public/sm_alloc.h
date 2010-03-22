/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

#include <seap-debug.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef  __P
#define __P __attribute__ ((unused)) static

#if defined(NDEBUG)
void *sm_alloc (size_t s);
void *sm_calloc (size_t n, size_t s);
void *sm_realloc (void *p, size_t s);
void *sm_reallocf (void *p, size_t s);
int   sm_memalign (void **p, size_t a, size_t s);
void  sm_free (void *p);
#else
void *  __sm_alloc_dbg (size_t s, const char *f, size_t l);
__P void *sm_alloc     (size_t s) { return __sm_alloc_dbg (s, __FUNCTION__, 0); }

void *  __sm_calloc_dbg (size_t n, size_t s, const char *f, size_t l);
__P void *sm_calloc     (size_t n, size_t s) { return __sm_calloc_dbg (n, s, __FUNCTION__, 0); }

void *  __sm_realloc_dbg (void *p, size_t s, const char *f, size_t l);
__P void *sm_realloc     (void *p, size_t s) { return __sm_realloc_dbg (p, s, __FUNCTION__, 0); }

void *  __sm_reallocf_dbg (void *p, size_t s, const char *f, size_t l);
__P void *sm_reallocf     (void *p, size_t s) { return __sm_reallocf_dbg (p, s, __FUNCTION__, 0); }

int     __sm_memalign_dbg (void **p, size_t a, size_t s, const char *f, size_t l);
__P int __sm_memalign     (void **p, size_t a, size_t s) { return __sm_memalign_dbg (p, a, s, __FUNCTION__, 0); }

void   __sm_free_dbg (void *p, const char *f, size_t l);
__P void sm_free     (void *p) { __sm_free_dbg (p, __FUNCTION__, 0); }

# define sm_alloc(s)          __sm_alloc_dbg (s, __PRETTY_FUNCTION__, __LINE__)
# define sm_calloc(n, s)      __sm_calloc_dbg (n, s, __PRETTY_FUNCTION__, __LINE__)
# define sm_realloc(p, s)     __sm_realloc_dbg ((void *)(p), s, __PRETTY_FUNCTION__, __LINE__)
# define sm_reallocf(p, s)    __sm_reallocf_dbg ((void *)(p), s, __PRETTY_FUNCTION__, __LINE__)
# define sm_memalign(p, a, s) __sm_memalign_dbg (p, a, s, __PRETTY_FUNCTION__, __LINE__)
# define sm_free(p)           __sm_free_dbg ((void *)(p), __PRETTY_FUNCTION__, __LINE__)
#endif

#define  sm_talloc(T) ((T *) sm_alloc(sizeof(T)))
#define  sm_valloc(v) ((typeof(v) *) sm_alloc(sizeof v))

#include <assert.h>
#ifndef _A
# define _A(x) assert(x)
#endif /* _A */

#ifdef __cplusplus
}
#endif

#endif /* SM_ALLOC_H */
