/**
 * @file
 * OpenScap allocation helpers.
 *
 * @addtogroup COMMON
 * @{
 * @addtogroup Internal
 * @{
 * Functions to be used from within the OpenSCAP library.
 * @name Memory allocation
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
#ifndef OSCAP_ALLOC_H
#define OSCAP_ALLOC_H

#include <stdlib.h>

#undef  __P
#define __P __attribute__ ((unused)) static

#if defined(NDEBUG)
void *__oscap_alloc(size_t s);
__P void *oscap_alloc(size_t s)
{
	return __oscap_alloc(s);
}

void *__oscap_calloc(size_t n, size_t s);
__P void *oscap_calloc(size_t n, size_t s)
{
	return __oscap_calloc(n, s);
}

void *__oscap_realloc(void *p, size_t s);
__P void *oscap_realloc(void *p, size_t s)
{
	return __oscap_realloc(p, s);
}

void *__oscap_reallocf(void *p, size_t s);
__P void *oscap_reallocf(void *p, size_t s)
{
	return __oscap_reallocf(p, s);
}

void __oscap_free(void *p);
__P void oscap_free(void *p)
{
	__oscap_free(p);
}

# define oscap_alloc(s)       __oscap_alloc (s)
# define oscap_calloc(n, s)   __oscap_calloc (n, s);
# define oscap_realloc(p, s)  __oscap_realloc ((void *)(p), s)
# define oscap_reallocf(p, s) __oscap_reallocf((void *)(p), s)
# define oscap_free(p)        __oscap_free ((void *)(p))
#else
void *__oscap_alloc_dbg(size_t s, const char *f, size_t l);
__P void *oscap_alloc(size_t s)
{
	return __oscap_alloc_dbg(s, __FUNCTION__, 0);
}

void *__oscap_calloc_dbg(size_t n, size_t s, const char *f, size_t l);
__P void *oscap_calloc(size_t n, size_t s)
{
	return __oscap_calloc_dbg(n, s, __FUNCTION__, 0);
}

void *__oscap_realloc_dbg(void *p, size_t s, const char *f, size_t l);
__P void *oscap_realloc(void *p, size_t s)
{
	return __oscap_realloc_dbg(p, s, __FUNCTION__, 0);
}

void *__oscap_reallocf_dbg(void *p, size_t s, const char *f, size_t l);
__P void *oscap_reallocf(void *p, size_t s)
{
	return __oscap_reallocf_dbg(p, s, __FUNCTION__, 0);
}

void __oscap_free_dbg(void **p, const char *f, size_t l);
__P void oscap_free(void *p)
{
	__oscap_free_dbg(&p, __FUNCTION__, 0);
}

/**
 * malloc wrapper
 */
# define oscap_alloc(s)       __oscap_alloc_dbg (s, __PRETTY_FUNCTION__, __LINE__)

/**
 * calloc wrapper
 */
# define oscap_calloc(n, s)   __oscap_calloc_dbg (n, s, __PRETTY_FUNCTION__, __LINE__)

/**
 * realloc wrapper
 */
# define oscap_realloc(p, s)  __oscap_realloc_dbg ((void *)(p), s, __PRETTY_FUNCTION__, __LINE__)

/**
 * realloc wrapper freeing old memory on failure
 */
# define oscap_reallocf(p, s) __oscap_reallocf_dbg ((void *)(p), s, __PRETTY_FUNCTION__, __LINE__)

/**
 * free wrapper
 */
# define oscap_free(p)        __oscap_free_dbg ((void **)((void *)&(p)), __PRETTY_FUNCTION__, __LINE__)
#endif

#define  oscap_talloc(T) ((T *) oscap_alloc(sizeof(T)))
#define  oscap_valloc(v) ((typeof(v) *) oscap_alloc(sizeof v))
#define  OSCAP_SALLOC(TYPE, NAME) struct TYPE* NAME = oscap_calloc(1, sizeof(struct TYPE))

#include <assert.h>
#ifndef _A
# define _A(x) assert(x)
#endif				/* _A */
#endif				/* OSCAP_ALLOC_H */
