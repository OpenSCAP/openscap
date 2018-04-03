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
#include <errno.h>

#include "public/sm_alloc.h"

#if defined(NDEBUG)
/*
 * Normal
 */

void *sm_alloc (size_t s)
{
        void *m;
#if defined(SEAP_MALLOC_STRICT)
        _A(s > 0);
#endif
        m = malloc (s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL)
                exit (ENOMEM);
#endif
        return (m);
}

void *sm_calloc (size_t n, size_t s)
{
        void *m;
#if defined(SEAP_MALLOC_STRICT)
        _A(n > 0);
        _A(s > 0);
#endif
        m = calloc (n, s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL)
                exit (ENOMEM);
#endif
        return (m);
}

void *sm_realloc (void *p, size_t s)
{
        void *m;

        m = realloc (p, s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL && s > 0)
                exit (ENOMEM);
#endif
        return (m);
}

void *sm_reallocf (void *p, size_t s)
{
        void *m;

        m = realloc (p, s);
        if (m == NULL && s > 0) {
                sm_free (p);
#if defined(SEAP_MALLOC_EXIT)
                exit (ENOMEM);
#endif
        }
        return (m);
}

void sm_free (void *p)
{
#if defined(SEAP_MALLOC_STRICT)
        _A(p != NULL);
#endif
        if (p != NULL)
                free (p);
        return;
}

#else
/*
 * Debug
 */

void *__sm_alloc_dbg (size_t s, const char *func, size_t line)
{
        void *m;
#if defined(SEAP_MALLOC_STRICT)
        _A(s > 0);
#endif
        m = malloc (s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL) {
                dI("FAIL: size=%zu", s);
                exit (ENOMEM);
        }
#endif
#if defined(SEAP_VERBOSE_DEBUG)
        dI("%s:%u, ptr=%p, size=%zu", func, line, m, s);
#endif
        return (m);
}

void *__sm_calloc_dbg (size_t n, size_t s, const char *f, size_t l)
{
        void *m;
#if defined(SEAP_MALLOC_STRICT)
        _A(n > 0);
        _A(s > 0);
#endif
        m = calloc (n, s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL) {
                dI("FAIL: nmemb=%zu, size=%zu, total=%zu",
                   n, s, n * s);
                exit (ENOMEM);
        }
#endif
#if defined(SEAP_VERBOSE_DEBUG)
        dI("ptr=%p, nmemb=%zu, size=%zu, total=%zu",
           m, n, s, n * s);
#endif
        return (m);
}

void *__sm_realloc_dbg (void *p, size_t s, const char *f, size_t l)
{
        void *m;
        m = realloc (p, s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL && s > 0) {
                dI("FAIL: old=%p, size=%zu", p, s);
                exit (ENOMEM);
        }
#endif
#if defined(SEAP_VERBOSE_DEBUG)
        dI("%s:%u, old=%p, new=%p, size=%zu", f, l, p, m, s);
#endif
        return (m);
}

void *__sm_reallocf_dbg (void *p, size_t s, const char *f, size_t l)
{
        void *m;
        m = realloc (p, s);
        if (m == NULL && s > 0) {
                dI("FAIL: old=%p, size=%zu", p, s);
                sm_free (p);
#if defined(SEAP_MALLOC_EXIT)
                exit (ENOMEM);
#endif
        } else {
#if defined(SEAP_VERBOSE_DEBUG)
                dI("old=%p, new=%p, size=%zu", p, m, s);
#endif
        }
        return (m);
}

void __sm_free_dbg (void *p, const char *f, size_t l)
{
#if defined(SEAP_MALLOC_STRICT)
        _A(p != NULL);
#endif
#if defined(SEAP_VERBOSE_DEBUG)
        dI("ptr=%p", p);
#endif
        if (p != NULL)
                free (p);
        return;
}
#endif
