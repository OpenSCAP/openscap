/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 *      Šimon Lukašík
 *
 *
 *
 * This module implements a simple iterator iterating through SMC structure.
 * That is StringMap that points to the oval_collection objects.
 *
 * This module implements only sub set of iterator functionality for iterating
 * through map/list structure. Limits: It supposes that list (slave_col) is
 * never empty.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/alloc.h"
#include "oval_smc_iterator_impl.h"
#include "oval_string_map_impl.h"
#include "oval_smc_impl.h"

struct oval_smc_iterator {
	struct oval_collection *master_col;		///< list of lists
	struct oval_iterator *master_it;		///< iterating through master_col
	struct oval_iterator *slave_it;			///< iterating through a single item of master_col
};

struct oval_smc_iterator *oval_smc_iterator_new(struct oval_smc *mapping)
{
	if (mapping == NULL)
		return NULL;

	struct oval_smc_iterator *it = calloc(1, sizeof(struct oval_smc_iterator));

	it->master_col = oval_string_map_collect_values((struct oval_string_map *) mapping, NULL);
	it->master_it = oval_collection_iterator(it->master_col);
	it->slave_it = NULL;
	return it;
}

void oval_smc_iterator_free(struct oval_smc_iterator *it)
{
	if (it == NULL)
		return;
	oval_collection_free(it->master_col);
	oval_collection_iterator_free(it->master_it);
	oval_collection_iterator_free(it->slave_it);
	free(it);
}

bool oval_smc_iterator_has_more(struct oval_smc_iterator *it)
{
	return it != NULL &&
		(oval_collection_iterator_has_more(it->master_it) ||
		(it->slave_it &&
			oval_collection_iterator_has_more(it->slave_it)));
}

void *oval_smc_iterator_next(struct oval_smc_iterator *it)
{
	if (it == NULL)
		return NULL;
	if (it->slave_it == NULL || !oval_collection_iterator_has_more(it->slave_it)) {
		// Rewind master and find new slave
		if (it->slave_it != NULL) {
			oval_collection_iterator_free(it->slave_it);
			it->slave_it = NULL;
		}
		if (!oval_collection_iterator_has_more(it->master_it)) {
			assert(false);
			return NULL;
		}
		struct oval_collection *slave_col = (struct oval_collection *) oval_collection_iterator_next(it->master_it);
		it->slave_it = oval_collection_iterator(slave_col);
	}
	assert(oval_collection_iterator_has_more(it->slave_it));
	return oval_collection_iterator_next(it->slave_it);
}
