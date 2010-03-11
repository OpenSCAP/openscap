
/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

OSCAP_HIDDEN_END;

#endif				/* PROBE_CACHE_H */
