#pragma once
#ifndef PROBE_CACHE_H
#define PROBE_CACHE_H

#include <sexp-types.h>
#include <redblack.h>

DEFRBTREE(pcache, SEXP_t *id; SEXP_t *item);

typedef struct {
        TREETYPE(pcache) tree;
} pcache_t;

pcache_t *pcache_new (void);
void      pcache_free (pcache_t *cache);

int     pcache_sexp_add (pcache_t *cache, const SEXP_t *id, SEXP_t *item);
int     pcache_cstr_add (pcache_t *cache, const char *id, SEXP_t *item);

int     pcache_sexp_del (pcache_t *cache, const SEXP_t *id);
int     pcache_cstr_del (pcache_t *cache, const char *id);

SEXP_t *pcache_sexp_get (pcache_t *cache, const SEXP_t *id);
SEXP_t *pcache_cstr_get (pcache_t *cache, const char *id);

#endif /* PROBE_CACHE_H */
