/**
 * @file   probe-cache.c
 * @brief  probe cache API implementation
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 */

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
#include <assume.h>
#include <alloc.h>
#include "SEAP/generic/rbt/rbt.h"
#include "probe-cache.h"

pcache_t *pcache_new(void)
{
	pcache_t *cache;

	cache = oscap_talloc(pcache_t);
	cache->tree = rbt_str_new();

	return (cache);
}

static void pcache_free_node(struct rbt_str_node *n)
{
        oscap_free(n->key);
        SEXP_free(n->data);
}

void pcache_free(pcache_t *cache)
{
        rbt_str_free_cb(cache->tree, &pcache_free_node);
	oscap_free(cache);
	return;
}

int pcache_sexp_add(pcache_t *cache, const SEXP_t *id, SEXP_t *item)
{
        SEXP_t *r;
        char   *k;

	assume_d(cache != NULL, -1);
	assume_d(id    != NULL, -1);
	assume_d(item  != NULL, -1);

        k = SEXP_string_cstr(id);
        r = SEXP_ref(item);

        if (rbt_str_add(cache->tree, k, (void *)r) != 0) {
                SEXP_free(r);
                oscap_free(k);
                return (-1);
        }

	return (0);
}

int pcache_cstr_add(pcache_t *cache, const char *id, SEXP_t * item)
{
	return (-1);
}

int pcache_sexp_del(pcache_t *cache, const SEXP_t * id)
{
	return (-1);
}

int pcache_cstr_del(pcache_t *cache, const char *id)
{
	return (-1);
}

SEXP_t *pcache_sexp_get(pcache_t *cache, const SEXP_t * id)
{
        char    b[128], *k = b;
        SEXP_t *r = NULL;

        if (SEXP_string_cstr_r(id, k, sizeof b) == ((size_t)-1))
                k = SEXP_string_cstr(id);

        if (k == NULL)
                return(NULL);

        rbt_str_get(cache->tree, k, (void *)&r);

        if (k != b)
                oscap_free(k);

        return (r != NULL ? SEXP_ref(r) : NULL);
}

SEXP_t *pcache_cstr_get(pcache_t *cache, const char *k)
{
        SEXP_t *r = NULL;

        rbt_str_get(cache->tree, k, (void *)&r);

        return (r != NULL ? SEXP_ref(r) : NULL);
}
