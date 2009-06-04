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
#ifndef XCCDF_LIST_
#define XCCDF_LIST_

#include <stdlib.h>
#include <stdbool.h>

typedef void(*xccdf_destruct_func)(void*);
typedef void(*xccdf_dump_func)(void*,int);

/*
 * Linear linked list.
 */

struct xccdf_list_item {
	void* data;
	struct xccdf_list_item* next;
};

struct xccdf_list {
	struct xccdf_list_item* first;
	struct xccdf_list_item* last;
	size_t itemcount;
};

struct xccdf_list* xccdf_list_new(void);
bool xccdf_list_add(struct xccdf_list* list, void* value);
void xccdf_list_delete(struct xccdf_list* list, xccdf_destruct_func destructor);
void xccdf_list_dump(struct xccdf_list* list, xccdf_dump_func dumper, int depth);

/* Linked List iterator. */

typedef bool(*xccdf_filter_func)(void*,void*);

struct xccdf_iterator {
    struct xccdf_list_item* cur;
    xccdf_filter_func filter;
    void* user_data;
};

void* xccdf_iterator_new(struct xccdf_list* list);
void* xccdf_iterator_new_filter(struct xccdf_list* list, xccdf_filter_func filter, void* user_data);
void* xccdf_iterator_next(struct xccdf_iterator* it);
bool xccdf_iterator_has_more(struct xccdf_iterator* it);

/* Iterator generation macros */

#define XITERATOR(x) ((struct xccdf_iterator*)(x))
#define XCCDF_ITERATOR(n) struct xccdf_##n##_iterator*
#define XCCDF_ITERATOR_FWD(n) struct xccdf_##n##_iterator;
#define XCCDF_ITERATOR_HAS_MORE(n) bool xccdf_##n##_iterator_has_more(XCCDF_ITERATOR(n) it) { return xccdf_iterator_has_more(XITERATOR(it)); }
#define XCCDF_ITERATOR_NEXT(t,n) t xccdf_##n##_iterator_next(XCCDF_ITERATOR(n) it) { return xccdf_iterator_next(XITERATOR(it)); }
#define XCCDF_ITERATOR_GEN_T(t,n) XCCDF_ITERATOR_FWD(n) XCCDF_ITERATOR_HAS_MORE(n) XCCDF_ITERATOR_NEXT(t,n)
#define XCCDF_ITERATOR_GEN_S(n) XCCDF_ITERATOR_GEN_T(struct xccdf_##n*,n)

/*
 * Hash table
 */

// Comparison function.
typedef int (*xccdf_compare_func)(const char*, const char*);
// Hash table item.
struct xccdf_htable_item {
	struct xccdf_htable_item* next; // Next item.
	char* key;                      // Item key.
	void* value;                    // Item value.
};

// Hash table.
struct xccdf_htable {
	size_t hsize;                      // Size of the hash table.
	size_t itemcount;                  // Number of elements in the hash table.
	struct xccdf_htable_item** table;  // The table itself.
	xccdf_compare_func cmp;            // Funcion used to compare keys (e.g. strcmp).
};

/*
 * Create a new hash table.
 * @param cmp Pointer to a function used as the key comparator.
 * @hsize Size of the hash table.
 * @internal
 * @return new hash table
 */
struct xccdf_htable* xccdf_htable_new1(xccdf_compare_func cmp, size_t hsize);

/*
 * Create a new hash table.
 *
 * The table will use strcmp() as the comparison function and will have default table size.
 * @see xccdf_htable_new1()
 * @return new hash table
 */
struct xccdf_htable* xccdf_htable_new(void);

/*
 * Add an item to the hash table.
 * @return True on success, false if the key already exists.
 */
bool xccdf_htable_add(struct xccdf_htable* htable, const char* key, void* item);

/*
 * Get a hash table item.
 * @return An item, NULL if item with specified key is not present in the hash table.
 */
void* xccdf_htable_get(struct xccdf_htable* htable, const char* key);

void xccdf_htable_dump(struct xccdf_htable* htable, xccdf_dump_func dumper, int depth);

/*
 * Delete the hash table.
 * @param htable Hash table to be deleted.
 * @param destructor Function used to delete individual items.
 */
void xccdf_htable_delete(struct xccdf_htable* htable, xccdf_destruct_func destructor);

#endif


