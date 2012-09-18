/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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
 */

#pragma once
#ifndef OSCAP_HTABLE_H
#define OSCAP_HTABLE_H

/**
 * @file
 * Hash Table Interface.
 *
 * @addtogroup COMMON
 * @{
 * @addtogroup ADT
 * @{
 * Functions to access and manipulate Hash Table items.
 */

#include <stdbool.h>
#include "adt.h"

/**
 * @struct oscap_htable
 * Representation of hash table.
 */
struct oscap_htable;

/*
 * Create a new hash table.
 *
 * The table will use strcmp() as the comparison function and will have default table size.
 * @see oscap_htable_new1()
 * @return new hash table
 */
struct oscap_htable *oscap_htable_new(void);

/*
 * Get a hash table item.
 * @return An item, NULL if item with specified key is not present in the hash table.
 */
void *oscap_htable_get(struct oscap_htable *htable, const char *key);

/*
 * Add an item to the hash table.
 * @return True on success, false if the key already exists.
 */
bool oscap_htable_add(struct oscap_htable *htable, const char *key, void *item);

/*
 * Delete the hash table.
 * @param htable Hash table to be deleted.
 * @param destructor Function used to delete individual items.
 */
void oscap_htable_free(struct oscap_htable *htable, oscap_destruct_func destructor);

/// @}
/// @}
#endif
