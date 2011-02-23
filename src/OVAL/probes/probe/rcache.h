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
#ifndef RCACHE_H
#define RCACHE_H

#include <stddef.h>
#include <sexp.h>
#include "../SEAP/generic/rbt/rbt.h"

/**
 * Probe cache structure.
 */
typedef struct {
        rbt_t *tree; /**< red-black tree used to store the items */
} probe_rcache_t;

/**
 * Create a new probe cache.
 * @return probe cache pointer or NULL on failure
 */
probe_rcache_t *probe_rcache_new(void);

/**
 * Free the probe cache. This function frees the memory used to store
 * the probe cache items and also the items themselves.
 * @param cache the cache to be freed
 */
void probe_rcache_free(probe_rcache_t *cache);

/**
 * Add a new S-exp to the cache identified by an S-exp string.
 * @param cache probe cache
 * @param id S-exp string object containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval 0 on success
 * @retval -1 on failure
 */
int probe_rcache_sexp_add(probe_rcache_t *cache, const SEXP_t *id, SEXP_t *item);

/**
 * Add a new S-exp to the cache identified by a C string.
 * @param cache probe cache
 * @param id C string containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval 0 on success
 * @retval -1 on failure
 */
int probe_rcache_cstr_add(probe_rcache_t *cache, const char *id, SEXP_t *item);

/**
 * Delete an S-exp from the cache identified by an S-exp string.
 * @param cache probe cache
 * @param id S-exp string object containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval 0 on success
 * @retval -1 on failure
 */
int probe_rcache_sexp_del(probe_rcache_t *cache, const SEXP_t *id);


/**
 * Delete an S-exp from the cache identified by a C string.
 * @param cache probe cache
 * @param id C string containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval 0 on success
 * @retval -1 on failure
 */
int probe_rcache_cstr_del(probe_rcache_t *cache, const char *id);

/**
 * Get a reference to an cached S-exp identified by an S-exp string.
 * @param cache probe cache
 * @param id S-exp string object containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval S-exp reference to the requested item
 */
SEXP_t *probe_rcache_sexp_get(probe_rcache_t *cache, const SEXP_t *id);

/**
 * Get a reference to an cached S-exp identified by a C string.
 * @param cache probe cache
 * @param id C string containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval S-exp reference to the requested item
 */
SEXP_t *probe_rcache_cstr_get(probe_rcache_t *cache, const char *id);

#endif /* PROBE_RCACHE_H */
