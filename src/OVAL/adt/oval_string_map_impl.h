/**
 * @file oval_string_map_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#ifndef OVAL_STRING_MAP_IMPL_H_
#define OVAL_STRING_MAP_IMPL_H_

#include "oval_collection_impl.h"
#include "../common/util.h"


struct oval_string_map;

struct oval_string_map *oval_string_map_new(void);
void oval_string_map_put(struct oval_string_map *, const char *, void *);

void oval_string_map_put_string(struct oval_string_map *, const char *, const char *);
struct oval_iterator *oval_string_map_keys(struct oval_string_map *);
struct oval_iterator *oval_string_map_values(struct oval_string_map *);
void *oval_string_map_get_value(struct oval_string_map *, const char *);
void oval_string_map_free(struct oval_string_map *, oscap_destruct_func);
void oval_string_map_free0(struct oval_string_map *);
void oval_string_map_free_string(struct oval_string_map *);
struct oval_collection *oval_string_map_collect_values(struct oval_string_map *map, struct oval_collection *collection);


#endif				/* OVAL_STRING_MAP_IMPL_H_ */
