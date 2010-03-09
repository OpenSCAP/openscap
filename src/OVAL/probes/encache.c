#include <stdlib.h>
#include <string.h>
#include <sexp.h>
#include "encache.h"
#include "alloc.h"
#include "common/bfind.h"
#include "common/assume.h"

#define ENCACHE_RLOCK(c,r)                                      \
        do {                                                    \
                if (pthread_rwlock_rdlock (&(c)->lock) != 0)    \
                        return (r);                             \
        } while (0)

#define ENCACHE_RUNLOCK(c)                                      \
        do {                                                    \
                if (pthread_rwlock_unlock (&(c)->lock) != 0)    \
                        abort ();                               \
        } while (0)

#define ENCACHE_WLOCK(c,r)                                      \
        do {                                                    \
                if (pthread_rwlock_wrlock (&(c)->lock) != 0)    \
                        return (r);                             \
        } while (0)

#define ENCACHE_WUNLOCK(c) ENCACHE_RUNLOCK(c)

encache_t *encache_new (void)
{
        encache_t *cache;
        cache = oscap_talloc (encache_t);

        if (pthread_rwlock_init (&cache->lock, NULL) != 0) {
                oscap_free (cache);
                return (NULL);
        }
        
        cache->name = oscap_calloc (ENCACHE_INIT_SIZE, sizeof (SEXP_t *));
        cache->size = ENCACHE_INIT_SIZE;
        cache->real = 0;
        
        return (cache);
}

void encache_free (encache_t *cache)
{
        size_t i;

        assume_d (cache != NULL);
        
        for (i = 0; i < cache->real; ++i)
                if (cache->name[i] != NULL)
                        SEXP_free (cache->name[i]);
        
        oscap_free (cache->name);
        oscap_free (cache);

        return;
}

static int encache_cmp1 (const char *name, const SEXP_t **sexp)
{
        return ((-1) * SEXP_strcmp (*sexp, name));
}

static int encache_cmp2 (const SEXP_t **a, const SEXP_t **b)
{
        return SEXP_string_cmp (*a, *b);
}

SEXP_t *encache_add (encache_t *cache, const char *name)
{
        SEXP_t *ref;
        
        assume_d (cache != NULL);
        assume_d (name  != NULL);
        
        ref = SEXP_string_new (name, strlen (name));
        
        if (ref == NULL)
                return (NULL);
        
        ENCACHE_WLOCK(cache, NULL);
        
        if (cache->size <= cache->real) {
                cache->size += ENCACHE_ADD_SIZE;
                cache->name  = oscap_realloc (cache->name, sizeof (SEXP_t *) * cache->size);
        }

        assume_d (cache->name != NULL);
        assume_d (cache->size > cache->real);
        
        cache->name[cache->real] = ref;
        ++cache->real;
        
        ref = SEXP_ref (ref);
        qsort (cache->name, cache->real, sizeof (SEXP_t *), (int (*)(const void *, const void *)) &encache_cmp2);
        
        ENCACHE_WUNLOCK(cache);
        
        return (ref);
}

SEXP_t *encache_get (encache_t *cache, const char *name)
{
        SEXP_t **ref = NULL;
        
        assume_d (cache != NULL);
        assume_d (name  != NULL);
        
        ENCACHE_RLOCK(cache, NULL);
        
        if (cache->real > 0) {
                ref = (SEXP_t **)oscap_bfind (cache->name, cache->real, sizeof (SEXP_t *),
                                              (void *)name, (int (*)(void *, void *))&encache_cmp1);
                
                if (ref != NULL)
                        ref = (SEXP_t **)SEXP_ref (*ref);
        }
        
        ENCACHE_RUNLOCK(cache);

        return ((SEXP_t *)(ref));
}

SEXP_t *encache_ref (encache_t *cache, const char *name)
{
        SEXP_t *ref;
        
        assume_d (cache != NULL);
        assume_d (name  != NULL);

        ref = encache_get (cache, name);
        
        if (ref == NULL)
                ref = encache_add (cache, name);
        
        return (ref);
}
