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

#include "list.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

struct oscap_list *oscap_list_new(void)
{
	struct oscap_list *list = oscap_alloc(sizeof(struct oscap_list));
	memset(list, 0, sizeof(struct oscap_list));
	return list;
}

bool oscap_list_add(struct oscap_list * list, void *value)
{
	__attribute__nonnull__(list);
	struct oscap_list_item *item = oscap_alloc(sizeof(struct oscap_list_item));
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
	} while (!it->filter(it->cur->data, it->user_data) && oscap_iterator_has_more(it));
	/*
	   while (it->cur && !it->filter(it->cur->data, it->user_data))
	   it->cur = it->cur->next;
	 */
}

void *oscap_iterator_new(struct oscap_list *list)
{
	struct oscap_iterator *it = oscap_calloc(1, sizeof(struct oscap_iterator));
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
	oscap_iterator_find_nearest(it);
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
	//if (!it->cur) return NULL;
	//void* ret = it->cur->data;
	//it->cur = it->cur->next;
	oscap_iterator_find_nearest(it);
	return it->cur->data;
}

bool oscap_iterator_has_more(struct oscap_iterator * it)
{
	__attribute__nonnull__(it);
	return (!it->cur && it->list->first) || (it->cur && it->cur->next);
	/*
	   if (it->cur) return true;
	   // oscap_iterator_free(it);
	   return false;
	 */
}

OSCAP_ITERATOR_GEN_T(const char *, oscap_string)
    OSCAP_ITERATOR_REMOVE_T(const char *, oscap_string, free)

#define OSCAP_DEFAULT_HSIZE 256
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
	t = oscap_alloc(sizeof(struct oscap_htable));
	if (t == NULL)
		return NULL;
	t->hsize = hsize;
	t->itemcount = 0;
	t->table = oscap_calloc(hsize, sizeof(struct oscap_list_item *));
	if (t->table == NULL) {
		free(t);
		return NULL;
	}
	t->cmp = cmp;
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
	newhtitem = oscap_alloc(sizeof(struct oscap_htable_item));
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

void oscap_print_depth(int depth)
{
	while (depth--)
		printf("  ");
}
