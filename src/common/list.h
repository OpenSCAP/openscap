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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */


/*
 * @file
 * @internal
 * @{
 */
#ifndef OSCAP_LIST_
#define OSCAP_LIST_

#include <stdlib.h>
#include <stdbool.h>
#include "util.h"

typedef void(*oscap_dump_func)(void*,int);

/*
 * Linear linked list.
 */

struct oscap_list_item {
	void* data;
	struct oscap_list_item* next;
};

struct oscap_list {
	struct oscap_list_item* first;
	struct oscap_list_item* last;
	size_t itemcount;
};

struct oscap_list* oscap_list_new(void);
bool oscap_list_add(struct oscap_list* list, void* value);
void oscap_list_free(struct oscap_list* list, oscap_destruct_func destructor);
void oscap_list_free0(struct oscap_list* list);
void oscap_list_dump(struct oscap_list* list, oscap_dump_func dumper, int depth);

/* Linked List iterator. */

typedef bool(*oscap_filter_func)(void*,void*);

struct oscap_iterator {
    struct oscap_list_item* cur;
    oscap_filter_func filter;
    void* user_data;
};

void* oscap_iterator_new(struct oscap_list* list);
void* oscap_iterator_new_filter(struct oscap_list* list, oscap_filter_func filter, void* user_data);
void* oscap_iterator_next(struct oscap_iterator* it);
bool oscap_iterator_has_more(struct oscap_iterator* it);

/** @struct oscap_string_iterator
 * Iterator over C-style strings.
 * @see oscap_iterator
 */
struct oscap_string_iterator;
/// @relates oscap_string_iterator
const char* oscap_string_iterator_next(struct oscap_string_iterator*);
/// @relates oscap_string_iterator
bool oscap_string_iterator_has_more(struct oscap_string_iterator*);

/*
 * Hash table
 */

// Comparison function.
typedef int (*oscap_compare_func)(const char*, const char*);
// Hash table item.
struct oscap_htable_item {
	struct oscap_htable_item* next; // Next item.
	char* key;                      // Item key.
	void* value;                    // Item value.
};

// Hash table.
struct oscap_htable {
	size_t hsize;                      // Size of the hash table.
	size_t itemcount;                  // Number of elements in the hash table.
	struct oscap_htable_item** table;  // The table itself.
	oscap_compare_func cmp;            // Funcion used to compare keys (e.g. strcmp).
};

/*
 * Create a new hash table.
 * @param cmp Pointer to a function used as the key comparator.
 * @hsize Size of the hash table.
 * @internal
 * @return new hash table
 */
struct oscap_htable* oscap_htable_new1(oscap_compare_func cmp, size_t hsize);

/*
 * Create a new hash table.
 *
 * The table will use strcmp() as the comparison function and will have default table size.
 * @see oscap_htable_new1()
 * @return new hash table
 */
struct oscap_htable* oscap_htable_new(void);

/*
 * Add an item to the hash table.
 * @return True on success, false if the key already exists.
 */
bool oscap_htable_add(struct oscap_htable* htable, const char* key, void* item);

/*
 * Get a hash table item.
 * @return An item, NULL if item with specified key is not present in the hash table.
 */
void* oscap_htable_get(struct oscap_htable* htable, const char* key);

void oscap_htable_dump(struct oscap_htable* htable, oscap_dump_func dumper, int depth);

/*
 * Delete the hash table.
 * @param htable Hash table to be deleted.
 * @param destructor Function used to delete individual items.
 */
void oscap_htable_free(struct oscap_htable* htable, oscap_destruct_func destructor);

void oscap_print_depth(int depth);

#endif


