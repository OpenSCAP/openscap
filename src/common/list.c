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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "list.h"
static inline bool _oscap_iterator_has_more_internal(const struct oscap_iterator *it);

struct oscap_list *oscap_list_new(void)
{
	struct oscap_list *list = calloc(1, sizeof(struct oscap_list));
	return list;
}

void oscap_create_lists(struct oscap_list **first, ...)
{
	va_list ap;
	va_start(ap, first);
	for (struct oscap_list **cur = first; cur != NULL; cur = va_arg(ap, struct oscap_list **))
		*cur = oscap_list_new();
	va_end(ap);
}

bool oscap_list_add(struct oscap_list * list, void *value)
{
	__attribute__nonnull__(list);
	if (value == NULL) return false;

	struct oscap_list_item *item = malloc(sizeof(struct oscap_list_item));
	item->next = NULL;
	item->data = value;
	++list->itemcount;

	if (list->last == NULL)
		list->first = list->last = item;
	else {
		list->last->next = item;
		list->last = item;
	}
	return true;
}

bool oscap_list_push(struct oscap_list *list, void *value)
{
	return oscap_list_add(list,value);
}

bool oscap_list_pop(struct oscap_list *list, oscap_destruct_func destructor)
{
	if (list == NULL || list->first == NULL) return false;
	struct oscap_list_item *cur = list->first, *prev = NULL;

	while (cur != list->last) {
		prev = cur;
		cur = cur->next;
	}

	if (destructor) destructor(cur->data);
	free(cur);

	list->last = prev;
	if (prev) prev->next = NULL;
	else list->first = NULL;

	--list->itemcount;

	return true;
}

bool oscap_list_remove(struct oscap_list *list, void *value, oscap_cmp_func compare, oscap_destruct_func destructor)
{
	if (list == NULL || list->first == NULL) return false;
	struct oscap_list_item *cur = list->first, *prev = NULL;

	while (cur != list->last && !compare(cur->data, value)) {
		prev = cur;
		cur = cur->next;
	}

	if (compare(cur->data, value)) {
		if (prev)
			prev->next = cur->next;
		else
			list->first = cur->next;

		if (cur == list->last)
			list->last = prev;

		if (destructor) destructor(cur->data);
		free(cur);

		--list->itemcount;
		return true;
	}

	return false;
}

struct oscap_list * oscap_list_clone(const struct oscap_list * list, oscap_clone_func cloner)
{
        if (list == NULL) 
            return NULL;

        struct oscap_list       * copy = oscap_list_new();
        struct oscap_list_item  * item;

        item = list->first;
        while (item != NULL) {
                if (cloner)
                    oscap_list_add(copy, cloner(item->data));
                else oscap_list_add(copy, item->data);
                item = item->next;
        }

        return copy;
}

struct oscap_list *oscap_list_destructive_join(struct oscap_list *list1, struct oscap_list *list2)
{
	if (list1->first == NULL)
		list1->first = list2->first;
	else list1->last->next = list2->first;
	if (list2->last != NULL) list1->last = list2->last;
	list1->itemcount += list2->itemcount;
	free(list2);
	return list1;
}

int oscap_list_get_itemcount(struct oscap_list *list)
{
	__attribute__nonnull__(list);
	return list->itemcount;
}

void oscap_list_free(struct oscap_list *list, oscap_destruct_func destructor)
{
	struct oscap_list_item *item, *to_del;
	if (list) {
		item = list->first;
		while (item != NULL) {
			to_del = item;
			item = item->next;
			if (destructor)
				destructor(to_del->data);
			free(to_del);
		}
		free(list);
	}
}

void oscap_list_free0(struct oscap_list *list)
{
	oscap_list_free(list, NULL);
}

bool oscap_ptr_cmp(void *node1, void *node2)
{
	return node1 == node2;
}

void* oscap_list_find(struct oscap_list *list, void *what, oscap_cmp_func compare)
{
	if (list == NULL) return false;
	if (compare == NULL) compare = oscap_ptr_cmp;

	struct oscap_fast_iterator *it = oscap_fast_iterator_new(list);

	while (oscap_fast_iterator_has_more(it)) {
		void *item = oscap_fast_iterator_next(it);
		if (compare(item, what)) {
			oscap_fast_iterator_free(it);
			return item;
		}
	}

	oscap_fast_iterator_free(it);
	return NULL;
}

bool oscap_list_contains(struct oscap_list *list, void *what, oscap_cmp_func compare)
{
	return oscap_list_find(list, what, compare) != NULL;
}

void oscap_list_dump(struct oscap_list *list, oscap_dump_func dumper, int depth)
{
	if (list == NULL) {
		printf(" (NULL list)\n");
		return;
	}
	printf(" (list, %u item%s)\n", (unsigned)list->itemcount, (list->itemcount == 1 ? "" : "s"));
	struct oscap_list_item *item = list->first;
	while (item) {
		dumper(item->data, depth);
		item = item->next;
	}
}

static bool oscap_iterator_no_filter(void *foo, void *bar)
{
	return true;
}

static inline void oscap_iterator_find_nearest(struct oscap_iterator *it)
{
	__attribute__nonnull__(it);
	__attribute__nonnull__(it->list);

	do {
		it->cur = (it->cur ? it->cur->next : it->list->first);
	} while (it->cur && !it->filter(it->cur->data, it->user_data) && _oscap_iterator_has_more_internal(it));
}

void *oscap_iterator_new(struct oscap_list *list)
{
	struct oscap_iterator *it = calloc(1, sizeof(struct oscap_iterator));
	it->cur = NULL;
	it->filter = oscap_iterator_no_filter;
	it->list = list;
	return it;
}

void *oscap_iterator_new_filter(struct oscap_list *list, oscap_filter_func filter, void *user_data)
{
	struct oscap_iterator *it = oscap_iterator_new(list);
	it->filter = filter;
	it->user_data = user_data;
	return it;
}

size_t oscap_iterator_get_itemcount(const struct oscap_iterator * it)
{
	__attribute__nonnull__(it);
	return it->list->itemcount;
}

void *oscap_iterator_detach(struct oscap_iterator *it)
{
	__attribute__nonnull__(it);

	if (!it->cur)
		return NULL;

	struct oscap_list_item *item = it->cur;
	void *value = item->data;

	assert(it->list->first != NULL && it->list->last != NULL);

	if (it->list->first == it->list->last) {
		assert(it->list->first == it->list->last);
		assert(it->list->first == item);
		it->list->first = it->list->last = NULL;
		it->cur = NULL;
	} else if (item == it->list->first) {
		assert(it->list->first != it->list->last);
		assert(item->next != NULL);
		it->list->first = item->next;
		it->cur = NULL;
	} else {
		struct oscap_list_item *cur = it->list->first;
		while (cur->next != item) {
			assert(cur->next != NULL);
			cur = cur->next;
		}
		assert(cur->next == item);
		cur->next = item->next;
		if (item == it->list->last)
			it->list->last = cur;
		it->cur = cur;
	}

	free(item);
	--it->list->itemcount;
	return value;
}

void oscap_iterator_free(struct oscap_iterator *it)
{
	free(it);
}

void *oscap_iterator_next(struct oscap_iterator *it)
{
	__attribute__nonnull__(it);
	oscap_iterator_find_nearest(it);
	return it->cur->data;
}

static inline bool _oscap_iterator_has_more_internal(const struct oscap_iterator *it)
{
	return (!it->cur && it->list->first) || (it->cur && it->cur->next);
}

bool oscap_iterator_has_more(struct oscap_iterator *it)
{
	if (!it || !it->list || !it->list->first)
		return false;
	if (it->cur == NULL) {
		if (it->filter(it->list->first->data, it->user_data))
			return true;
		else
			it->cur = it->list->first;
	}
	while (it->cur->next && !it->filter(it->cur->next->data, it->user_data))
		it->cur = it->cur->next;
	return it->cur->next != NULL;
}

void oscap_iterator_reset(struct oscap_iterator * it)
{
    it->cur = NULL;
}

void *oscap_fast_iterator_new(struct oscap_list *list)
{
	struct oscap_fast_iterator *it = oscap_calloc(1, sizeof(struct oscap_fast_iterator));
	it->cur = NULL;
	it->list = list;
	return it;
}

void oscap_fast_iterator_free(struct oscap_fast_iterator *it)
{
	oscap_free(it);
}

void *oscap_fast_iterator_detach(struct oscap_fast_iterator *it)
{
	__attribute__nonnull__(it);

	if (!it->cur)
		return NULL;

	struct oscap_list_item *item = it->cur;
	void *value = item->data;

	assert(it->list->first != NULL && it->list->last != NULL);

	if (it->list->first == it->list->last) {
		assert(it->list->first == it->list->last);
		assert(it->list->first == item);
		it->list->first = it->list->last = NULL;
		it->cur = NULL;
	} else if (item == it->list->first) {
		assert(it->list->first != it->list->last);
		assert(item->next != NULL);
		it->list->first = item->next;
		it->cur = NULL;
	} else {
		struct oscap_list_item *cur = it->list->first;
		while (cur->next != item) {
			assert(cur->next != NULL);
			cur = cur->next;
		}
		assert(cur->next == item);
		cur->next = item->next;
		if (item == it->list->last)
			it->list->last = cur;
		it->cur = cur;
	}

	free(item);
	--it->list->itemcount;
	return value;
}

struct oscap_stringlist *oscap_stringlist_clone(struct oscap_stringlist *list)
{
    void *clone = oscap_list_new(); // oscap_stringlist (or oscap_list)
    OSCAP_FOR_STR(str, oscap_stringlist_get_strings(list))
        oscap_list_add(clone, oscap_strdup(str));
    return clone;
}

struct oscap_string_iterator *oscap_stringlist_get_strings(const struct oscap_stringlist* list)
{
	return oscap_iterator_new((struct oscap_list *) list);
}

bool oscap_stringlist_add_string(struct oscap_stringlist* list, const char *str)
{
	return oscap_list_add((struct oscap_list *) list, strdup(str));
}

struct oscap_stringlist * oscap_stringlist_new(void)
{
    return (struct oscap_stringlist *) oscap_list_new();
}

void oscap_stringlist_free(struct oscap_stringlist *list)
{
	oscap_list_free((struct oscap_list *) list, free);
}

OSCAP_ITERATOR_GEN_T(const char *, oscap_string)
OSCAP_ITERATOR_REMOVE_T(const char *, oscap_string, free)
OSCAP_ITERATOR_GEN(oscap_stringlist)
OSCAP_ITERATOR_REMOVE(oscap_stringlist, oscap_stringlist_free)
    /*OSCAP_ITERATOR_RESET(oscap_string)*/


#define OSCAP_DEFAULT_HSIZE 389
static inline unsigned int oscap_htable_hash(const char *str, size_t htable_size)
{
	unsigned h = 0;
	unsigned char *p;
	for (p = (unsigned char *)str; *p != '\0'; p++)
		h = (97 * h) + *p;
	return h % htable_size;
}

struct oscap_htable *oscap_htable_new1(oscap_compare_func cmp, size_t hsize)
{
	struct oscap_htable *t;
    
    assert(hsize > 0);

	t = malloc(sizeof(struct oscap_htable));
	if (t == NULL)
		return NULL;
	t->hsize = hsize;
	t->itemcount = 0;
	t->table = calloc(hsize, sizeof(struct oscap_htable_item *));
	if (t->table == NULL) {
		free(t);
		return NULL;
	}
	t->cmp = cmp;
	return t;
}

struct oscap_htable * oscap_htable_clone(const struct oscap_htable * table, oscap_clone_func cloner)
{
	struct oscap_htable *t = oscap_htable_new();
	if (t == NULL)
		return NULL;

	for (size_t i = 0; i < table->hsize; ++i) {
		struct oscap_htable_item *item = table->table[i];
		while (item != NULL) {
			oscap_htable_add(t, item->key, (void *) cloner(item->value));
			item = item->next;
		}
	}
	
	return t;
}

static int oscap_htable_cmp(const char *s1, const char *s2)
{
	if (s1 == NULL)
		return -1;
	if (s2 == NULL)
		return 1;
	return strcmp(s1, s2);
}

struct oscap_htable *oscap_htable_new(void)
{
	return oscap_htable_new1(oscap_htable_cmp, OSCAP_DEFAULT_HSIZE);
}

static struct oscap_htable_item *oscap_htable_lookup(struct oscap_htable *htable, const char *key)
{
	__attribute__nonnull__(htable);
	if (key == NULL)
		return NULL;
	unsigned int hashcode = oscap_htable_hash(key, htable->hsize);
	struct oscap_htable_item *htitem = htable->table[hashcode];
	while (htitem != NULL) {
		if (htable->cmp(htitem->key, key) == 0)
			return htitem;
		htitem = htitem->next;
	}
	return NULL;
}

bool oscap_htable_add(struct oscap_htable * htable, const char *key, void *item)
{
	__attribute__nonnull__(htable);
	/*
	   unsigned int hashcode = oscap_htable_hash(key, htable->hsize);
	   struct oscap_htable_item* htitem = htable->table[hashcode];
	   while(htitem != NULL)
	   {
	   if( (*(htable->cmp))(htitem->key, key) == 0) return false;
	   if(htitem->next == NULL) break;
	   htitem = htitem->next;
	   }
	   // htitem points to the last item
	 */
	if (oscap_htable_lookup(htable, key) != NULL)
		return false;
	unsigned int hashcode = oscap_htable_hash(key, htable->hsize);
	struct oscap_htable_item *newhtitem;
	newhtitem = malloc(sizeof(struct oscap_htable_item));
	newhtitem->key = strdup(key);
	newhtitem->value = item;
	newhtitem->next = htable->table[hashcode];
	htable->table[hashcode] = newhtitem;
	htable->itemcount++;
	return true;
}

void *oscap_htable_detach(struct oscap_htable *htable, const char *key)
{
	struct oscap_htable_item *htitem = oscap_htable_lookup(htable, key);
	if (htitem) {
		void *val = htitem->value;
		free(htitem->key);
		htitem->key = NULL;
		htitem->value = NULL;
		htable->itemcount--;
		return val;
	}
	return NULL;
}

void *oscap_htable_get(struct oscap_htable *htable, const char *key)
{
	__attribute__nonnull__(htable);
	struct oscap_htable_item *htitem = oscap_htable_lookup(htable, key);
	return htitem ? htitem->value : NULL;
}

void oscap_print_depth(int);

void oscap_htable_dump(struct oscap_htable *htable, oscap_dump_func dumper, int depth)
{
	if (htable == NULL) {
		printf(" (NULL hash table)\n");
		return;
	}
	printf(" (hash table, %u item%s)\n", (unsigned)htable->itemcount, (htable->itemcount == 1 ? "" : "s"));
	int i;
	for (i = 0; i < (int)htable->hsize; ++i) {
		struct oscap_htable_item *item = htable->table[i];
		while (item) {
			oscap_print_depth(depth);
			printf("'%s':\n", item->key);
			dumper(item->value, depth + 1);
			item = item->next;
		}
	}
}

void oscap_htable_free(struct oscap_htable *htable, oscap_destruct_func destructor)
{
	if (htable) {
		size_t ht;
		struct oscap_htable_item *cur, *next;

		for (ht = 0; ht < htable->hsize; ++ht) {
			cur = htable->table[ht];
			while (cur) {
				next = cur->next;
				free(cur->key);
				if (destructor)
					destructor(cur->value);
				free(cur);
				cur = next;
			}
		}

		free(htable->table);
		free(htable);
	}
}

void oscap_htable_free0(struct oscap_htable *htable)
{
	oscap_htable_free(htable, NULL);
}

struct oscap_htable_iterator {
	struct oscap_htable *htable;	// Table we iterate through
	struct oscap_htable_item *cur;	// The current item
	size_t hpos;			// Line on which we are now
};

struct oscap_htable_iterator *
oscap_htable_iterator_new(struct oscap_htable *htable)
{
	struct oscap_htable_iterator *hit = calloc(1, sizeof(struct oscap_htable_iterator));
	hit->htable = htable;
	hit->cur = NULL;
	hit->hpos = 0;
	return hit;
}

bool
oscap_htable_iterator_has_more(struct oscap_htable_iterator *hit)
{
	__attribute__nonnull__(hit);
	if (hit->htable == NULL)
		return false;
	size_t i = hit->hpos;
	if (hit->cur != NULL) {
		if (hit->cur->next != NULL)
			return true;
		if (i + 1 >= hit->htable->hsize)
			return false;
		i++;
	}
	for (; i < hit->htable->hsize; i++) {
		if (hit->htable->table[i] == NULL)
			continue;
		if (i != hit->hpos)
			hit->hpos = i - 1;
		return true;
	}
	hit->hpos = i;
	return false;
}

const struct oscap_htable_item *
oscap_htable_iterator_next(struct oscap_htable_iterator *hit)
{
	__attribute__nonnull__(hit);
	if (hit->cur != NULL) {
		if (hit->cur->next != NULL) {
			hit->cur = hit->cur->next;
			return hit->cur;
		}
		if (hit->hpos + 1 >= hit->htable->hsize) {
			assert(false);
			return NULL;
		}
		hit->hpos++;
	}
	for (; hit->hpos < hit->htable->hsize; hit->hpos++) {
		if (hit->htable->table[hit->hpos] == NULL)
			continue;
		hit->cur = hit->htable->table[hit->hpos];
		return hit->cur;
	}
	assert(false); // no more item found
	return NULL;
}

const char *
oscap_htable_iterator_next_key(struct oscap_htable_iterator *hit)
{
	const struct oscap_htable_item *item = oscap_htable_iterator_next(hit);
	return (item == NULL) ? NULL : item->key;
}

void *
oscap_htable_iterator_next_value(struct oscap_htable_iterator *hit)
{
	const struct oscap_htable_item *item = oscap_htable_iterator_next(hit);
	return (item == NULL) ? NULL : item->value;
}

void oscap_htable_iterator_next_kv(struct oscap_htable_iterator *hit, const char **key, void **value)
{
	const struct oscap_htable_item *item = oscap_htable_iterator_next(hit);
	if (item == NULL)
		return;

	*key = item->key;
	*value = item->value;
}

void
oscap_htable_iterator_reset(struct oscap_htable_iterator *hit)
{
	__attribute__nonnull__(hit);
	hit->cur = NULL;
	hit->hpos = 0;
}

void
oscap_htable_iterator_free(struct oscap_htable_iterator *hit)
{
	free(hit);
}

void oscap_print_depth(int depth)
{
	while (depth--)
		printf("  ");
}
