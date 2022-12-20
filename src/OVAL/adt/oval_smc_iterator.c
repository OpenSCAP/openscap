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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
 * through map/list structure. Limits: It supposes that list (secondary_col) is
 * never empty.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "oval_smc_iterator_impl.h"
#include "oval_string_map_impl.h"
#include "oval_smc_impl.h"

struct oval_smc_iterator {
	struct oval_collection *primary_col;		///< list of lists
	struct oval_iterator *primary_it;		///< iterating through primary_col
	struct oval_iterator *secondary_it;			///< iterating through a single item of primary_col
};

struct oval_smc_iterator *oval_smc_iterator_new(struct oval_smc *mapping)
{
	if (mapping == NULL)
		return NULL;

	struct oval_smc_iterator *it = calloc(1, sizeof(struct oval_smc_iterator));

	it->primary_col = oval_string_map_collect_values((struct oval_string_map *) mapping, NULL);
	it->primary_it = oval_collection_iterator(it->primary_col);
	it->secondary_it = NULL;
	return it;
}

void oval_smc_iterator_free(struct oval_smc_iterator *it)
{
	if (it == NULL)
		return;
	oval_collection_free(it->primary_col);
	oval_collection_iterator_free(it->primary_it);
	oval_collection_iterator_free(it->secondary_it);
	free(it);
}

bool oval_smc_iterator_has_more(struct oval_smc_iterator *it)
{
	return it != NULL &&
		(oval_collection_iterator_has_more(it->primary_it) ||
		(it->secondary_it &&
			oval_collection_iterator_has_more(it->secondary_it)));
}

void *oval_smc_iterator_next(struct oval_smc_iterator *it)
{
	if (it == NULL)
		return NULL;
	if (it->secondary_it == NULL || !oval_collection_iterator_has_more(it->secondary_it)) {
		// Rewind primary and find new secondary
		if (it->secondary_it != NULL) {
			oval_collection_iterator_free(it->secondary_it);
			it->secondary_it = NULL;
		}
		if (!oval_collection_iterator_has_more(it->primary_it)) {
			assert(false);
			return NULL;
		}
		struct oval_collection *secondary_col = (struct oval_collection *) oval_collection_iterator_next(it->primary_it);
		it->secondary_it = oval_collection_iterator(secondary_col);
	}
	assert(oval_collection_iterator_has_more(it->secondary_it));
	return oval_collection_iterator_next(it->secondary_it);
}
