#pragma once
#ifndef PROBE_CACHE_H
#define PROBE_CACHE_H

#include <sexp-types.h>
#include <SEAP/generic/redblack.h>
#include <../../common/util.h>

OSCAP_HIDDEN_START;

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

OSCAP_HIDDEN_END;

#endif /* PROBE_CACHE_H */
