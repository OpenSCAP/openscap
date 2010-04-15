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

#include <seap.h>
#include <string.h>
#include <assert.h>
#include <alloc.h>
#include "probe-cache.h"

#ifndef _A
#define _A(x) assert (x)
#endif

#define PCACHE_RLOCK(c,r)                                       \
        do {                                                    \
                if (pthread_rwlock_rdlock (&(c)->lock) != 0)    \
                        return (r);                             \
        } while (0)

#define PCACHE_RUNLOCK(c)                                       \
        do {                                                    \
                if (pthread_rwlock_unlock (&(c)->lock) != 0)    \
                        abort ();                               \
        } while (0)

#define PCACHE_WLOCK(c,r)                                       \
        do {                                                    \
                if (pthread_rwlock_wrlock (&(c)->lock) != 0)    \
                        return (r);                             \
        } while (0)

#define PCACHE_WUNLOCK(c) PCACHE_RUNLOCK(c)

RBNODECMP(pcache)
{
	return (SEXP_string_cmp(a->id, b->id));
}

RBNODEJOIN(pcache)
{
	(void)b;
	return (a);
}

RBTREECODE(pcache);

pcache_t *pcache_new(void)
{
	pcache_t *cache;

	cache = oscap_talloc(pcache_t);
	cache->tree.root = NULL;
	cache->tree.size = 0;

        if (pthread_rwlock_init (&cache->lock, NULL) != 0) {
                oscap_free (cache);
                return (NULL);
        }
        
	return (cache);
}

void pcache_free(pcache_t * cache)
{
	_A(cache != NULL);
	/* FIXME: free tree */
        pthread_rwlock_destroy (&cache->lock);
	oscap_free(cache);
	return;
}

int pcache_sexp_add(pcache_t *cache, const SEXP_t *id, SEXP_t *item)
{
        int ret;
        NODETYPE(pcache) *new;

	_A(cache != NULL);
	_A(id != NULL);
	_A(item != NULL);

        PCACHE_WLOCK(cache, -1);
        
	new = RB_NEWNODE(pcache) ();
	new->id = SEXP_ref(id);
	new->item = SEXP_ref(item);
        
	if (RB_INSERT(pcache) (&(cache->tree), new) == E_OK) {
		ret = 0;
	} else {
		_D("Can't add item to cache: item=%p, id=%p.\n", item, id);
		oscap_free(new);
		ret = -1;
	}
        
        PCACHE_WUNLOCK(cache);
	return (ret);
}

int pcache_cstr_add(pcache_t * cache, const char *id, SEXP_t * item)
{
	_A(cache != NULL);
	_A(id != NULL);
	_A(item != NULL);

	return (-1);
}

int pcache_sexp_del(pcache_t * cache, const SEXP_t * id)
{
	_A(cache != NULL);
	_A(id != NULL);

	return (-1);
}

int pcache_cstr_del(pcache_t * cache, const char *id)
{
	_A(cache != NULL);
	_A(id != NULL);

	return (-1);
}

SEXP_t *pcache_sexp_get(pcache_t * cache, const SEXP_t * id)
{
        SEXP_t *ret;
	NODETYPE(pcache) key, *node;

	_A(cache != NULL);
	_A(id != NULL);

	key.id = (SEXP_t *) id;

        PCACHE_RLOCK(cache, NULL);
	node = RB_SEARCH(pcache) (&(cache->tree), &key);
        ret  = node == NULL ? NULL : SEXP_ref (node->item);
        PCACHE_RUNLOCK(cache);
        
        return (ret);
}

SEXP_t *pcache_cstr_get(pcache_t * cache, const char *id)
{
        SEXP_t *ret;
	NODETYPE(pcache) key, *node;

	_A(cache != NULL);
	_A(id != NULL);

        PCACHE_RLOCK(cache, NULL);
        key.id = SEXP_string_new(id, strlen(id));
	node = RB_SEARCH(pcache) (&(cache->tree), &key);
        ret  = node == NULL ? NULL : SEXP_ref (node->item);
        PCACHE_RUNLOCK(cache);
        SEXP_free(key.id);
        
        return (ret);
}
