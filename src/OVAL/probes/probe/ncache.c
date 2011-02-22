#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sexp.h>

#include "common/public/alloc.h"
#include "common/bfind.h"
#include "common/assume.h"

#include "ncache.h"

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
        probe_ncache_t *cache;
        cache = oscap_talloc (probe_ncache_t);

        if (pthread_rwlock_init (&cache->lock, NULL) != 0) {
                oscap_free (cache);
                return (NULL);
        }

        cache->name = oscap_calloc (PROBE_NCACHE_INIT_SIZE, sizeof (SEXP_t *));
        cache->size = PROBE_NCACHE_INIT_SIZE;
        cache->real = 0;

        return (cache);
}

void probe_ncache_free (probe_ncache_t *cache)
{
        size_t i;

        assume_d (cache != NULL, /* void */);

        for (i = 0; i < cache->real; ++i)
                if (cache->name[i] != NULL)
                        SEXP_free (cache->name[i]);

        oscap_free (cache->name);
        pthread_rwlock_destroy (&cache->lock);
        oscap_free (cache);

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

        assume_d (cache != NULL, NULL);
        assume_d (name  != NULL, NULL);

        ref = SEXP_string_new (name, strlen (name));

        if (ref == NULL)
                return (NULL);

        PROBE_NCACHE_WLOCK(cache, NULL);

        if (cache->size <= cache->real) {
                cache->size += PROBE_NCACHE_ADD_SIZE;
                cache->name  = oscap_realloc (cache->name, sizeof (SEXP_t *) * cache->size);
        }

        assume_d (cache->name != NULL, NULL);
        assume_d (cache->size > cache->real, NULL);

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

        assume_d (cache != NULL, NULL);
        assume_d (name  != NULL, NULL);

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

        assume_d (cache != NULL, NULL);
        assume_d (name  != NULL, NULL);

        ref = probe_ncache_get (cache, name);

        if (ref == NULL)
                ref = probe_ncache_add (cache, name);

        return (ref);
}
