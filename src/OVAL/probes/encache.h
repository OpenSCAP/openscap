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
#ifndef ENCACHE_H
#define ENCACHE_H

#include <pthread.h>
#include <stdint.h>

#define ENCACHE_INIT_SIZE 24
#define ENCACHE_ADD_SIZE  8

typedef struct {
        pthread_rwlock_t lock;
        SEXP_t **name; /* array of S-exp refs */
        size_t   real; /* number of cached names */
        size_t   size; /* pre-allocated size */
} encache_t;

/**
 * Create new element name cache
 */
encache_t *encache_new (void);

/**
 * Free memory used by the element name cache.
 * The S-exp objects stored in the cache are
 * also freed. However, if they are referenced
 * somewhere else, the memory won't be freed, just
 * the reference count will be decremented.
 * @param cache the cache to be freed
 */
void encache_free (encache_t *cache);

/**
 * Add a name to the cache. This will create a new S-exp
 * object and return a reference to it. Reference count
 * of such object will be 2 because the cache hold it's
 * own reference to the object.
 * @param cache
 * @param name
 */
SEXP_t *encache_add (encache_t *cache, const char *name);

/**
 * Get a reference to an already cached S-exp object. If the
 * object is not found in the cache, it won't be created and
 * NULL will be returned to the caller.
 * @param cache
 * @param name
 */
SEXP_t *encache_get (encache_t *cache, const char *name);

/**
 * Get a reference to a cached S-exp object. If the object is
 * not found in the cache, it will be created and the reference
 * this newly created object will be returned to the caller.
 * @param cache
 * @param name
 */
SEXP_t *encache_ref (encache_t *cache, const char *name);

#endif /* ENCACHE_H */
