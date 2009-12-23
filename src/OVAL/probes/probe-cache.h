#pragma once
#ifndef PROBE_CACHE_H
#define PROBE_CACHE_H

#include <stdbool.h>
#include <pthread.h>
#include <sexp-types.h>
#include <SEAP/generic/redblack.h>
#include <../../common/util.h>

OSCAP_HIDDEN_START;

DEFRBTREE(pcache, SEXP_t * id;
	  SEXP_t * item);

typedef struct {
	TREETYPE(pcache) tree;
	pthread_rwlock_t lock;
} pcache_t;

static inline int pcache_rlock(pcache_t * c)
{
	return (pthread_rwlock_rdlock(&c->lock) == 0 ? 0 : -1);
}

static inline int pcache_wlock(pcache_t * c)
{
	return (pthread_rwlock_wrlock(&c->lock) == 0 ? 0 : -1);
}

static inline int pcache_wunlock(pcache_t * c)
{
	return (pthread_rwlock_unlock(&c->lock) == 0 ? 0 : -1);
}

#define pcache_runlock(c) pcache_wunlock(c)
#define with_pcache_rlocked(c) for (bool __rlk__ = (pcache_rlock (c) == 0 ? true : false); __rlk__; __rlk__ = (pcache_runlock (c) == 0 ? false : abort(), false))
#define with_pcache_wlocked(c) for (bool __wlk__ = (pcache_wlock (c) == 0 ? true : false); __wlk__; __wlk__ = (pcache_wunlock (c) == 0 ? false : abort(), false))

pcache_t *pcache_new(void);
void pcache_free(pcache_t * cache);

int pcache_sexp_add(pcache_t * cache, const SEXP_t * id, SEXP_t * item);
int pcache_cstr_add(pcache_t * cache, const char *id, SEXP_t * item);

int pcache_sexp_del(pcache_t * cache, const SEXP_t * id);
int pcache_cstr_del(pcache_t * cache, const char *id);

SEXP_t *pcache_sexp_get(pcache_t * cache, const SEXP_t * id);
SEXP_t *pcache_cstr_get(pcache_t * cache, const char *id);

typedef struct {
	char *iname;
	SEXP_t *names;
	uint16_t count;
} ncache_item_t;

#define NCACHE_MAX_ITEMS 8

typedef struct {
	ncache_item_t *items[NCACHE_MAX_ITEMS];
	uint8_t count;
	pthread_rwlock_t lock;
} ncache_t;

ncache_t *ncache_new(void);
void ncache_free(ncache_t * cache);

int ncache_item_add(ncache_t * cache, const char *iname, SEXP_t * names, uint16_t count);
int ncache_name_add(ncache_t * cache, const char *iname, SEXP_t * name);

#define with_ncache_rlocked(c) for (bool __rlk__ = (ncache_rlock (c) == 0 ? true : false); __rlk__; __rlk__ = ncache_runlock (c) == 0 ? false : abort())
#define with_ncache_wlocked(c) for (bool __wlk__ = (ncache_wlock (c) == 0 ? true : false); __wlk__; __wlk__ = ncache_wunlock (c) == 0 ? false : abort())

OSCAP_HIDDEN_END;

#endif				/* PROBE_CACHE_H */
