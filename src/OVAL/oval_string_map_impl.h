/*
 * oval_string_map_impl.h
 *
 *  Created on: Mar 9, 2009
 *      Author: david.niemoller
 */

#include "oval_collection_impl.h"

#ifndef OVAL_STRING_MAP_IMPL_H_
#define OVAL_STRING_MAP_IMPL_H_

struct oval_string_map_s;

struct oval_string_map *oval_string_map_new();
void oval_string_map_put(struct oval_string_map *, char *, void *);
struct oval_iterator *oval_string_map_keys(struct oval_string_map *);
struct oval_iterator *oval_string_map_values(struct oval_string_map *);
void *oval_string_map_get_value(struct oval_string_map *, char *);
void oval_string_map_free(struct oval_string_map *, oval_item_free_func);

#endif				/* OVAL_STRING_MAP_IMPL_H_ */
