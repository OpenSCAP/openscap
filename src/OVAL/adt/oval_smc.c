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
 * Structure providing 1--N mapping, by integrating two structures:
 * oval_string_map and oval_collection.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "oval_smc_impl.h"
#include "oval_string_map_impl.h"
#include "common/util.h"

struct oval_smc *oval_smc_new(void)
{
	return (struct oval_smc *) oval_string_map_new();
}

static inline struct oval_collection *_oval_smc_get_all(struct oval_smc *map, const char *key)
{
	return (struct oval_collection *) oval_string_map_get_value((struct oval_string_map *)map, key);
}

void oval_smc_put_last(struct oval_smc *map, const char *key, void *item)
{
	__attribute__nonnull__(map);

	if (item != NULL) {
		struct oval_collection *list_col = _oval_smc_get_all(map, key);
		if (list_col == NULL) {
			list_col = oval_collection_new();
			oval_string_map_put((struct oval_string_map *) map, key, list_col);
		}
		oval_collection_add(list_col, item);
	}
}

struct oval_iterator *oval_smc_get_all_it(struct oval_smc *map, const char *key)
{
	struct oval_collection *col = _oval_smc_get_all(map, key);
	return (col == NULL) ? NULL : oval_collection_iterator(col);
}

void *oval_smc_get_last(struct oval_smc *map, const char *key)
{
	struct oval_iterator *list_it = oval_smc_get_all_it(map, key);
	if (list_it == NULL)
		return NULL;
	void *found = NULL;
	while (oval_collection_iterator_has_more(list_it)) {
		found = oval_collection_iterator_next(list_it);
	}
	oval_collection_iterator_free(list_it);
	return found;
}

void oval_smc_free0(struct oval_smc *map)
{
	oval_string_map_free((struct oval_string_map *) map, (oscap_destruct_func) oval_collection_free);
}

void oval_smc_free(struct oval_smc *map, oscap_destruct_func destructor)
{
	struct oval_iterator *map_it = oval_string_map_values((struct oval_string_map *) map);
	while (oval_collection_iterator_has_more(map_it)) {
		struct oval_collection *list_col = (struct oval_collection *) oval_collection_iterator_next(map_it);
		oval_collection_free_items(list_col, destructor);
	}
	oval_collection_iterator_free(map_it);
	oval_string_map_free((struct oval_string_map *) map, NULL);
}
