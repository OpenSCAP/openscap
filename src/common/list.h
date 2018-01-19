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
#include "public/oscap.h"
#include "public/oscap_text.h"


// list item dump function type
typedef void (*oscap_dump_func) ();
// generic comparison function type
typedef bool (*oscap_cmp_func) (void *, void *);

/*
 * Linear linked list.
 */

struct oscap_list_item {
	void *data;
	struct oscap_list_item *next;
};

struct oscap_list {
	struct oscap_list_item *first;
	struct oscap_list_item *last;
	size_t itemcount;
};

// FIXME: SCE engine uses these

struct oscap_list *oscap_list_new(void);
void oscap_create_lists(struct oscap_list **first, ...);
bool oscap_list_add(struct oscap_list *list, void *value);
bool oscap_list_push(struct oscap_list *list, void *value);
bool oscap_list_pop(struct oscap_list *list, oscap_destruct_func destructor);
bool oscap_list_remove(struct oscap_list *list, void *value, oscap_cmp_func compare, oscap_destruct_func destructor);
struct oscap_list *oscap_list_clone(const struct oscap_list * list, oscap_clone_func cloner);
void oscap_list_free(struct oscap_list *list, oscap_destruct_func destructor);
void oscap_list_free0(struct oscap_list *list);
void oscap_list_dump(struct oscap_list *list, oscap_dump_func dumper, int depth);
int oscap_list_get_itemcount(struct oscap_list *list);
bool oscap_list_contains(struct oscap_list *list, void *what, oscap_cmp_func compare);
struct oscap_list *oscap_list_destructive_join(struct oscap_list *list1, struct oscap_list *list2);



/* Linked List iterator. */

typedef bool(*oscap_filter_func) (void *, void *);

struct oscap_iterator {
	struct oscap_list_item *cur;
	struct oscap_list *list;
	oscap_filter_func filter;
	void *user_data;
};

// FIXME: SCE engine uses these

void *oscap_iterator_new(struct oscap_list *list);
void *oscap_iterator_new_filter(struct oscap_list *list, oscap_filter_func filter, void *user_data);
void *oscap_iterator_next(struct oscap_iterator *it);
size_t oscap_iterator_get_itemcount(const struct oscap_iterator *it);
bool oscap_iterator_has_more(struct oscap_iterator *it);
void oscap_iterator_reset(struct oscap_iterator *it);
void *oscap_iterator_detach(struct oscap_iterator *it);
void oscap_iterator_free(struct oscap_iterator *it);


void *oscap_list_find(struct oscap_list *list, void *what, oscap_cmp_func compare);

/**
 * Iterate over an array, given an iterator.
 * Execute @a code for each array member stored in @a val.
 * It is NOT safe to use return or goto inside of the @a code,
 * the iterator would not be freed properly.
 */
#define OSCAP_FOREACH_GENERIC(itype, vtype, val, init_val, code) \
    {                                                            \
        struct itype##_iterator *val##_iter = (init_val);        \
        vtype val;                                               \
        while (itype##_iterator_has_more(val##_iter)) {          \
            val = itype##_iterator_next(val##_iter);             \
            code                                                 \
        }                                                        \
        itype##_iterator_free(val##_iter);                       \
    }

/**
 * Iterate over an array, given an iterator.
 * @param type type of array elements (w/o the struct keyword)
 * @param val name of an variable the member will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @param code code to be executed for each element the iterator hits
 * @see OSCAP_FOREACH_GENERIC
 */
#define OSCAP_FOREACH(type, val, init_val, code) \
        OSCAP_FOREACH_GENERIC(type, struct type *, val, init_val, code)

/**
 * Iterate over an array, given an iterator.
 * It is generally not safe to use break, return or goto inside the loop
 * (iterator wouldn't be properly freed otherwise).
 * Two variables, named VAL and VAL_iter (substitute VAL for actual macro argument)
 * will be added to current variable scope. You can free the iterator explicitly
 * after previous unusual escape from the loop (e.g. using break).
 * @param val name of an variable the string will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @param code code to be executed for each string the iterator hits
 */
#define OSCAP_FOR_GENERIC(itype, vtype, val, init_val)                  \
    vtype val = NULL; struct itype##_iterator *val##_iter = (init_val); \
    while (itype##_iterator_has_more(val##_iter)                        \
            ? (val = itype##_iterator_next(val##_iter), true)           \
            : (itype##_iterator_free(val##_iter), val##_iter = NULL, false))

/**
 * Iterate over an array, given an iterator.
 * @param type type of array elements (w/o the struct keyword)
 * @param val name of an variable the member will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @see OSCAP_FOR_GENERIC
 */
#define OSCAP_FOR(type, val, init_val) OSCAP_FOR_GENERIC(type, struct type *, val, init_val)

/**
 * Iterate over an array of strings, given an iterator.
 * @param val name of an variable the member will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @see OSCAP_FOR_GENERIC
 */
#define OSCAP_FOR_STR(val, init_val) OSCAP_FOR_GENERIC(oscap_string, const char *, val, init_val)

/*
 * Hash table
 */

// Comparison function.
typedef int (*oscap_compare_func) (const char *, const char *);
// Hash table item.
struct oscap_htable_item {
	struct oscap_htable_item *next;	// Next item.
	char *key;		// Item key.
	void *value;		// Item value.
};

// Hash table.
struct oscap_htable {
	size_t hsize;		// Size of the hash table.
	size_t itemcount;	// Number of elements in the hash table.
	struct oscap_htable_item **table;	// The table itself.
	oscap_compare_func cmp;	// Funcion used to compare keys (e.g. strcmp).
};

/*
 * Create a new hash table.
 * @param cmp Pointer to a function used as the key comparator.
 * @hsize Size of the hash table.
 * @internal
 * @return new hash table
 */
struct oscap_htable *oscap_htable_new1(oscap_compare_func cmp, size_t hsize);

/*
 * Create a new hash table.
 *
 * The table will use strcmp() as the comparison function and will have default table size.
 * @see oscap_htable_new1()
 * @return new hash table
 */
struct oscap_htable *oscap_htable_new(void);

/*
 * Do a Deep Copy of a hashtable and all of its items
 *
 * @return deep copy of hash table
 */
struct oscap_htable * oscap_htable_clone(const struct oscap_htable * table, oscap_clone_func cloner);

/*
 * Add an item to the hash table.
 * @return True on success, false if the key already exists.
 */
bool oscap_htable_add(struct oscap_htable *htable, const char *key, void *item);

/*
 * Get a hash table item.
 * @return An item, NULL if item with specified key is not present in the hash table.
 */
void *oscap_htable_get(struct oscap_htable *htable, const char *key);

void *oscap_htable_detach(struct oscap_htable *htable, const char *key);

void oscap_htable_dump(struct oscap_htable *htable, oscap_dump_func dumper, int depth);

/*
 * Delete the hash table.
 * @param htable Hash table to be deleted.
 * @param destructor Function used to delete individual items.
 */
void oscap_htable_free(struct oscap_htable *htable, oscap_destruct_func destructor);
/*
 * Dispose the hash table -- do not dispose individial items.
 * @param htable Hash table to be deleted.
 */
void oscap_htable_free0(struct oscap_htable *htable);


/**
 * Iterator through htable structure.
 */
struct oscap_htable_iterator;

/**
 * Create new iterator through hash table. No ordering is defined for items.
 * @param htable Hash table to iterate through.
 * @return the iterator
 */
struct oscap_htable_iterator *oscap_htable_iterator_new(struct oscap_htable *htable);

/**
 * Query if the given iterator has more hash table items.
 * @param iterator iterator
 * @return true is there is at least one more item.
 */
bool oscap_htable_iterator_has_more(struct oscap_htable_iterator *hit);

/**
 * Get the next item from iterator. The behavior is undefined if the prior call
 * of oscap_htable_iterator_has_more() returns false
 * @param hit iterator
 */
const struct oscap_htable_item *oscap_htable_iterator_next(struct oscap_htable_iterator *hit);

/**
 * Get the key of next item from the iterator. The behavior is undefined
 * if the prior call of oscap_htable_iterator_has_more returns false.
 * @param hit iterator
 */
const char *oscap_htable_iterator_next_key(struct oscap_htable_iterator *hit);

/**
 * Get the value of next item from the iterator. The behavior is undefined
 * if the prior call of oscap_htable_iterator_has_more returns false.
 * @param hit iterator
 */
void *oscap_htable_iterator_next_value(struct oscap_htable_iterator *hit);

/**
 * Get the key and value of next item from the iterator. The behavior is undefined
 * if the prior call of oscap_htable_iterator_has_more returns false.
 * @param hit iterator
 * @param key pointer to pointer that will point towards the key pointer
 * @param value pointer to pointer that will point towards the value pointer
 */
void oscap_htable_iterator_next_kv(struct oscap_htable_iterator *hit, const char **key, void **value);

/**
 * Reset the hashtable iterator to the begin
 * @param hit iterator
 */
void oscap_htable_iterator_reset(struct oscap_htable_iterator *hit);

/**
 * Dispose the iterator
 * @param hit the iterator to dispose
 */
void oscap_htable_iterator_free(struct oscap_htable_iterator *hit);

void oscap_print_depth(int depth);


#endif
