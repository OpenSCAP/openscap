/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *      Daniel Kopecek <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stddef.h>
#include <sexp.h>

#include "common/alloc.h"
#include "../SEAP/generic/rbt/rbt.h"

#include "rcache.h"

probe_rcache_t *probe_rcache_new(void)
{
	probe_rcache_t *cache;

	cache = oscap_talloc(probe_rcache_t);
	cache->tree = rbt_str_new();

	return (cache);
}

static void probe_rcache_free_node(struct rbt_str_node *n)
{
        free(n->key);
        SEXP_free(n->data);
}

void probe_rcache_free(probe_rcache_t *cache)
{
        rbt_str_free_cb(cache->tree, &probe_rcache_free_node);
	free(cache);
	return;
}

int probe_rcache_sexp_add(probe_rcache_t *cache, const SEXP_t *id, SEXP_t *item)
{
        SEXP_t *r;
        char   *k;

	if (cache == NULL || id == NULL || item == NULL) {
		return -1;
	}

        k = SEXP_string_cstr(id);
        r = SEXP_ref(item);

        if (rbt_str_add(cache->tree, k, (void *)r) != 0) {
                SEXP_free(r);
                free(k);
                return (-1);
        }

	return (0);
}

int probe_rcache_cstr_add(probe_rcache_t *cache, const char *id, SEXP_t * item)
{
	return (-1);
}

int probe_rcache_sexp_del(probe_rcache_t *cache, const SEXP_t * id)
{
	return (-1);
}

int probe_rcache_cstr_del(probe_rcache_t *cache, const char *id)
{
	return (-1);
}

SEXP_t *probe_rcache_sexp_get(probe_rcache_t *cache, const SEXP_t * id)
{
        char    b[128], *k = b;
        SEXP_t *r = NULL;

        if (SEXP_string_cstr_r(id, k, sizeof b) == ((size_t)-1))
                k = SEXP_string_cstr(id);

        if (k == NULL)
                return(NULL);

        rbt_str_get(cache->tree, k, (void *)&r);

        if (k != b)
                free(k);

        return (r != NULL ? SEXP_ref(r) : NULL);
}

SEXP_t *probe_rcache_cstr_get(probe_rcache_t *cache, const char *k)
{
        SEXP_t *r = NULL;

        rbt_str_get(cache->tree, k, (void *)&r);

        return (r != NULL ? SEXP_ref(r) : NULL);
}
