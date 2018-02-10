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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "common/assume.h"
#include "_sexp-atomic.h"

#if defined(HAVE_ATOMIC_BUILTINS)
uint16_t SEXP_atomic_dec_u16 (volatile uint16_t *ptr)
{
        return (__sync_sub_and_fetch (ptr, 1));
}

uint16_t SEXP_atomic_inc_u16 (volatile uint16_t *ptr)
{
        return (__sync_fetch_and_add (ptr, 1));
}

bool SEXP_atomic_cas_u16 (volatile uint16_t *ptr, uint16_t old, uint16_t new)
{
        return ((bool) __sync_bool_compare_and_swap (ptr, old, new));
}

uint32_t SEXP_atomic_dec_u32 (volatile uint32_t *ptr)
{
        return (__sync_sub_and_fetch (ptr, 1));
}

uint32_t SEXP_atomic_inc_u32 (volatile uint32_t *ptr)
{
        return (__sync_fetch_and_add (ptr, 1));
}

bool SEXP_atomic_cas_u32 (volatile uint32_t *ptr, uint32_t old, uint32_t new)
{
        return ((bool) __sync_bool_compare_and_swap (ptr, old, new));
}

#ifdef SEXP_ATOMIC_64BITS
uint64_t SEXP_atomic_dec_u64 (volatile uint64_t *ptr)
{
        return (__sync_sub_and_fetch (ptr, 1));
}

uint64_t SEXP_atomic_inc_u64 (volatile uint64_t *ptr)
{
        return (__sync_fetch_and_add (ptr, 1));
}

bool SEXP_atomic_cas_u64 (volatile uint64_t *ptr, uint64_t old, uint64_t new)
{
        return ((bool) __sync_bool_compare_and_swap (ptr, old, new));
}
#endif /* SEXP_ATOMIC_64BITS */

#else
/* Using mutex-based emulation of atomic operations! */
# include <pthread.h>
# define SEXP_ATOMIC_MTX_CNT 17

static pthread_once_t  __SEXP_atomic_once = PTHREAD_ONCE_INIT;
static pthread_mutex_t __SEXP_atomic_mtx[SEXP_ATOMIC_MTX_CNT];

static void SEXP_atomic_deinit (void)
{
        register int i;

        for (i = SEXP_ATOMIC_MTX_CNT; i > 0; --i)
                pthread_mutex_destroy (&__SEXP_atomic_mtx[i - 1]);

        return;
}

static void SEXP_atomic_init (void)
{
        register int i;

        for (i = SEXP_ATOMIC_MTX_CNT; i > 0; --i)
                pthread_mutex_init (&__SEXP_atomic_mtx[i - 1], NULL);

        atexit (SEXP_atomic_deinit);

        return;
}

static inline void SEXP_atomic_once (void)
{
        if (pthread_once (&__SEXP_atomic_once, SEXP_atomic_init) != 0)
                abort ();
}

static inline void SEXP_atomic_lock (uintptr_t ptr)
{
        if (pthread_mutex_lock (&__SEXP_atomic_mtx[ptr % SEXP_ATOMIC_MTX_CNT]) != 0)
                abort ();
}

static inline void SEXP_atomic_unlock (uintptr_t ptr)
{
        if (pthread_mutex_unlock (&__SEXP_atomic_mtx[ptr % SEXP_ATOMIC_MTX_CNT]) != 0)
                abort ();
}

uint16_t SEXP_atomic_dec_u16 (volatile uint16_t *ptr)
{
        uint16_t r;

        SEXP_atomic_once();
        SEXP_atomic_lock((uintptr_t)ptr);
        r = *ptr - 1;
        *ptr = r;
        SEXP_atomic_unlock((uintptr_t)ptr);

        return (r);
}

uint16_t SEXP_atomic_inc_u16 (volatile uint16_t *ptr)
{
        uint16_t r;

        SEXP_atomic_once();
        SEXP_atomic_lock((uintptr_t)ptr);
        r = *ptr;
        *ptr = r + 1;
        SEXP_atomic_unlock((uintptr_t)ptr);

        return (r);
}

bool SEXP_atomic_cas_u16 (volatile uint16_t *ptr, uint16_t old, uint16_t new)
{
        bool r;

        SEXP_atomic_once();
        SEXP_atomic_lock((uintptr_t)ptr);
        if (*ptr == old) {
                *ptr = new;
                r = true;
        } else
                r = false;
        SEXP_atomic_unlock((uintptr_t)ptr);

        return (r);
}

uint32_t SEXP_atomic_dec_u32 (volatile uint32_t *ptr)
{
        uint32_t r;

        SEXP_atomic_once();
        SEXP_atomic_lock((uintptr_t)ptr);
        r = *ptr - 1;
        *ptr = r;
        SEXP_atomic_unlock((uintptr_t)ptr);

        return (r);
}

uint32_t SEXP_atomic_inc_u32 (volatile uint32_t *ptr)
{
        uint32_t r;

        SEXP_atomic_once();
        SEXP_atomic_lock((uintptr_t)ptr);
        r = *ptr;
        *ptr = r + 1;
        SEXP_atomic_unlock((uintptr_t)ptr);

        return (r);
}

bool SEXP_atomic_cas_u32 (volatile uint32_t *ptr, uint32_t old, uint32_t new)
{
        bool r;

        SEXP_atomic_once();
        SEXP_atomic_lock((uintptr_t)ptr);
        if (*ptr == old) {
                *ptr = new;
                r = true;
        } else
                r = false;
        SEXP_atomic_unlock((uintptr_t)ptr);

        return (r);
}

#ifdef SEXP_ATOMIC_64BITS
uint64_t SEXP_atomic_dec_u64 (volatile uint64_t *ptr)
{
        uint64_t r;

        SEXP_atomic_once();
        SEXP_atomic_lock((uintptr_t)ptr);
        r = *ptr - 1;
        *ptr = r;
        SEXP_atomic_unlock((uintptr_t)ptr);

        return (r);
}

uint64_t SEXP_atomic_inc_u64 (volatile uint64_t *ptr)
{
        uint64_t r;

        SEXP_atomic_once();
        SEXP_atomic_lock((uintptr_t)ptr);
        r = *ptr;
        *ptr = r + 1;
        SEXP_atomic_unlock((uintptr_t)ptr);

        return (r);
}

bool SEXP_atomic_cas_u64 (volatile uint64_t *ptr, uint64_t old, uint64_t new)
{
        bool r;

        SEXP_atomic_once();
        SEXP_atomic_lock((uintptr_t)ptr);
        if (*ptr == old) {
                *ptr = new;
                r = true;
        } else
                r = false;
        SEXP_atomic_unlock((uintptr_t)ptr);

        return (r);
}
#endif /* SEXP_ATOMIC_64BITS */

#endif /* !HAVE_ATOMIC_BUILTINS */
