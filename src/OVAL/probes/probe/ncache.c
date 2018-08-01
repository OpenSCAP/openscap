/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *      Daniel Kopecek <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sexp.h>

#include "common/bfind.h"

#include "ncache.h"
#include "_sexp-manip.h"

/**
 * Lock cache for reading.
 * @param c element name cache
 * @param r return value on failure
 */
#define PROBE_NCACHE_RLOCK(c,r)                                      \
        do {                                                    \
                if (pthread_rwlock_rdlock (&(c)->lock) != 0)    \
                        return (r);                             \
        } while (0)

/**
 * Unlock cache which was previously locked for reading.
 * @param c element name cache
 * @return this function calls abort(3) if it is unable to
 *         unlock the cache so as to prevent a deadlock.
 */
#define PROBE_NCACHE_RUNLOCK(c)                                      \
        do {                                                    \
                if (pthread_rwlock_unlock (&(c)->lock) != 0)    \
                        abort ();                               \
        } while (0)

/**
 * Lock cache for writing.
 * @param c element name cache
 * @param r return value on failure
 */
#define PROBE_NCACHE_WLOCK(c,r)                                      \
        do {                                                    \
                if (pthread_rwlock_wrlock (&(c)->lock) != 0)    \
                        return (r);                             \
        } while (0)

/**
 * Unlock cache which was previously locked for writing.
 * @param c element name cache
 * @return this function calls abort(3) if it is unable to
 *         unlock the cache so as to prevent a deadlock.
 */
#define PROBE_NCACHE_WUNLOCK(c) PROBE_NCACHE_RUNLOCK(c)

probe_ncache_t *probe_ncache_new (void)
{
        probe_ncache_t *cache = malloc(sizeof(probe_ncache_t));

        if (pthread_rwlock_init (&cache->lock, NULL) != 0) {
                free (cache);
                return (NULL);
        }

        cache->name = calloc (PROBE_NCACHE_INIT_SIZE, sizeof (SEXP_t *));
        cache->size = PROBE_NCACHE_INIT_SIZE;
        cache->real = 0;

        return (cache);
}

void probe_ncache_free (probe_ncache_t *cache)
{
        size_t i;

	if (cache == NULL) {
		return;
	}

        for (i = 0; i < cache->real; ++i)
                if (cache->name[i] != NULL)
                        SEXP_free (cache->name[i]);

        free (cache->name);
        pthread_rwlock_destroy (&cache->lock);
        free (cache);

        return;
}

static int probe_ncache_cmp1 (const char *name, const SEXP_t **sexp)
{
        return ((-1) * SEXP_strcmp (*sexp, name));
}

static int probe_ncache_cmp2 (const SEXP_t **a, const SEXP_t **b)
{
        return SEXP_string_cmp (*a, *b);
}

SEXP_t *probe_ncache_add (probe_ncache_t *cache, const char *name)
{
        SEXP_t *ref;

	if (cache == NULL || name == NULL) {
		return NULL;
	}

        ref = SEXP_string_new (name, strlen (name));

        if (ref == NULL)
                return (NULL);

        PROBE_NCACHE_WLOCK(cache, NULL);

        if (cache->size <= cache->real) {
                cache->size += PROBE_NCACHE_ADD_SIZE;
                cache->name  = realloc (cache->name, sizeof (SEXP_t *) * cache->size);
        }

	/* TODO: check if this is really needed */
	if (cache->name == NULL || cache->size <= cache->real) {
		return NULL;
	}

        cache->name[cache->real] = ref;
        ++cache->real;

        ref = SEXP_ref (ref);
        qsort (cache->name, cache->real, sizeof (SEXP_t *), (int (*)(const void *, const void *)) &probe_ncache_cmp2);

        PROBE_NCACHE_WUNLOCK(cache);

        return (ref);
}

SEXP_t *probe_ncache_get (probe_ncache_t *cache, const char *name)
{
        SEXP_t **ref = NULL;

	if (cache == NULL || name == NULL) {
		return NULL;
	}

        PROBE_NCACHE_RLOCK(cache, NULL);

        if (cache->real > 0) {
                ref = (SEXP_t **)oscap_bfind (cache->name, cache->real, sizeof (SEXP_t *),
                                              (void *)name, (int (*)(void *, void *))&probe_ncache_cmp1);

                if (ref != NULL)
                        ref = (SEXP_t **)SEXP_ref (*ref);
        }

        PROBE_NCACHE_RUNLOCK(cache);

        return ((SEXP_t *)(ref));
}

SEXP_t *probe_ncache_ref (probe_ncache_t *cache, const char *name)
{
        SEXP_t *ref;

	if (name == NULL) {
		return NULL;
	}

        if (cache == NULL)
                return SEXP_string_new (name, strlen (name));

        ref = probe_ncache_get (cache, name);

        if (ref == NULL)
                ref = probe_ncache_add (cache, name);

        return (ref);
}
