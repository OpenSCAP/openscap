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
#ifndef PROBE_NCACHE_H
#define PROBE_NCACHE_H

#include <stddef.h>
#include <pthread.h>
#include "_sexp-types.h"

#define PROBE_NCACHE_INIT_SIZE 24
#define PROBE_NCACHE_ADD_SIZE  8

/**
 * Element name cache structure. This structure contains an array
 * of cached string S-exps representing the names of elements.
 */
typedef struct {
        pthread_rwlock_t lock; /**< pthread read-write lock */
        SEXP_t **name; /**< array of S-exp refs */
        size_t   real; /**< number of cached names */
        size_t   size; /**< pre-allocated size */
} probe_ncache_t;

/**
 * Create new element name cache
 * @return an empty element name cache
 */
probe_ncache_t *probe_ncache_new (void);

/**
 * Free memory used by the element name cache.
 * The S-exp objects stored in the cache are
 * also freed. However, if they are referenced
 * somewhere else, the memory won't be freed, just
 * the reference count will be decremented.
 * @param cache the cache to be freed
 */
void probe_ncache_free (probe_ncache_t *cache);

/**
 * Add a name to the cache. This will create a new S-exp
 * object and return a reference to it. Reference count
 * of such object will be 2 because the cache hold it's
 * own reference to the object.
 * @param cache element name cache
 * @param name name string
 * @return S-exp reference to the name string
 */
SEXP_t *probe_ncache_add (probe_ncache_t *cache, const char *name);

/**
 * Get a reference to an already cached S-exp object. If the
 * object is not found in the cache, it won't be created and
 * NULL will be returned to the caller.
 * @param cache element name cache
 * @param name name string
 * @return S-exp reference to the name string or NULL if not found
 */
SEXP_t *probe_ncache_get (probe_ncache_t *cache, const char *name);

/**
 * Get a reference to a cached S-exp object. If the object is
 * not found in the cache, it will be created and the reference
 * this newly created object will be returned to the caller.
 * @param cache element name cache
 * @param name name string
 * @return S-exp reference to the name string
 */
SEXP_t *probe_ncache_ref (probe_ncache_t *cache, const char *name);

#endif /* PROBE_NCACHE_H */
