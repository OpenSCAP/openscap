/**
 * @file
 * OpenScap allocation helpers.
 *
 * @addtogroup COMMON
 * @{
 * @addtogroup Memory
 * @{
 * Memory allocation wrapper functions used in library.
 *
 * Don't see these wrappers, just use normal C memory allocation functions.
 * These are here just for compatibility.
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
#include "public/oscap.h"

#if defined(NDEBUG)
/// @cond
// Do not use, we keep this just for ABI compatibility, use malloc instead
OSCAP_DEPRECATED(void *__oscap_alloc(size_t s));

// Do not use, we keep this just for ABI compatibility, use calloc instead
OSCAP_DEPRECATED(void *__oscap_calloc(size_t n, size_t s));

// Do not use, we keep this just for ABI compatibility, use realloc instead
OSCAP_DEPRECATED(void *__oscap_realloc(void *p, size_t s));

// Do not use, we keep this just for ABI compatibility, use realloc instead
OSCAP_DEPRECATED(void *__oscap_reallocf(void *p, size_t s));

// Do not use, we keep this just for ABI compatibility, use free instead
OSCAP_DEPRECATED(void __oscap_free(void *p));
/// @endcond
#endif

/// @cond
#define  oscap_talloc(T) ((T *) malloc(sizeof(T)))
/// @endcond

#endif				/* OSCAP_ALLOC_H */
/// @}
/// @}
