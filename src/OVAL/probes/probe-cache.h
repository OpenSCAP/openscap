/**
 * @file   probe-cache.h
 * @brief  probe cache API header file
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

#pragma once
#ifndef PROBE_CACHE_H
#define PROBE_CACHE_H

#include <stdbool.h>
#include <pthread.h>
#include <sexp-types.h>
#include <../../common/util.h>
#include "SEAP/generic/rbt/rbt.h"

/**
 * Probe cache structure.
 */
typedef struct {
        rbt_t *tree; /**< red-black tree used to store the items */
} pcache_t;

/**
 * Create a new probe cache.
 * @return probe cache pointer or NULL on failure
 */
pcache_t *pcache_new(void);

/**
 * Free the probe cache. This function frees the memory used to store
 * the probe cache items and also the items themselves.
 * @param cache the cache to be freed
 */
void pcache_free(pcache_t * cache);

/**
 * Add a new S-exp to the cache identified by an S-exp string.
 * @param cache probe cache
 * @param id S-exp string object containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval 0 on success
 * @retval -1 on failure
 */
int pcache_sexp_add(pcache_t * cache, const SEXP_t * id, SEXP_t * item);

/**
 * Add a new S-exp to the cache identified by a C string.
 * @param cache probe cache
 * @param id C string containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval 0 on success
 * @retval -1 on failure
 */
int pcache_cstr_add(pcache_t * cache, const char *id, SEXP_t * item);


/**
 * Delete an S-exp from the cache identified by an S-exp string.
 * @param cache probe cache
 * @param id S-exp string object containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval 0 on success
 * @retval -1 on failure
 */
int pcache_sexp_del(pcache_t * cache, const SEXP_t * id);


/**
 * Delete an S-exp from the cache identified by a C string.
 * @param cache probe cache
 * @param id C string containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval 0 on success
 * @retval -1 on failure
 */
int pcache_cstr_del(pcache_t * cache, const char *id);

/**
 * Get a reference to an cached S-exp identified by an S-exp string.
 * @param cache probe cache
 * @param id S-exp string object containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval S-exp reference to the requested item
 */
SEXP_t *pcache_sexp_get(pcache_t * cache, const SEXP_t * id);

/**
 * Get a reference to an cached S-exp identified by a C string.
 * @param cache probe cache
 * @param id C string containing the id
 * @param item the S-exp (item) to be stored in the cache
 * @retval S-exp reference to the requested item
 */
SEXP_t *pcache_cstr_get(pcache_t * cache, const char *id);

#endif				/* PROBE_CACHE_H */
