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
