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


struct oscap_list* oscap_list_new(void)
{
	struct oscap_list* list = malloc(sizeof(struct oscap_list));
	memset(list, 0, sizeof(struct oscap_list));
	return list;
}

bool oscap_list_add(struct oscap_list* list, void* value)
{
	assert(list != NULL);
	struct oscap_list_item* item = malloc(sizeof(struct oscap_list_item));
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

void oscap_list_free(struct oscap_list* list, oscap_destruct_func destructor)
{
	struct oscap_list_item *item, *to_del;
	if (list) {
		item = list->first;
		while (item != NULL) {
			to_del = item;
			item = item->next;
			if (destructor) destructor(to_del->data);
			free(to_del);
		}
		free(list);
	}
}

void oscap_list_free0(struct oscap_list* list)
{
	oscap_list_free(list, NULL);
}

void oscap_list_dump(struct oscap_list* list, oscap_dump_func dumper, int depth)
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

bool oscap_iterator_no_filter(void* foo, void* bar) { bar = foo; return true; }

static inline void oscap_iterator_find_nearest(struct oscap_iterator* it)
{
    while (it->cur && !it->filter(it->cur->data, it->user_data))
        it->cur = it->cur->next;
}

void* oscap_iterator_new(struct oscap_list* list)
{
    struct oscap_iterator* it = calloc(1, sizeof(struct oscap_iterator));
    it->cur = list->first;
    it->filter = oscap_iterator_no_filter;
    return it;
}

void* oscap_iterator_new_filter(struct oscap_list* list, oscap_filter_func filter, void* user_data)
{
    struct oscap_iterator* it = oscap_iterator_new(list);
    it->filter = filter;
    it->user_data = user_data;
    oscap_iterator_find_nearest(it);
    return it;
}

void oscap_iterator_free(struct oscap_iterator* it)
{
    free(it);
}

void* oscap_iterator_next(struct oscap_iterator* it)
{
	if (!it->cur) return NULL;
    void* ret = it->cur->data;
    it->cur = it->cur->next;
    oscap_iterator_find_nearest(it);
    return ret;
}

bool oscap_iterator_has_more(struct oscap_iterator* it)
{
    if (it->cur) return true;
    // oscap_iterator_free(it);
    return false;
}

OSCAP_ITERATOR_GEN_T(const char*, oscap_string)


#define OSCAP_DEFAULT_HSIZE 256

static inline unsigned int oscap_htable_hash(const char *str, size_t htable_size)
{
	unsigned h = 0;
	unsigned char *p;
	for(p=(unsigned char *) str; *p != '\0'; p++)  h = (97 * h) + *p;
	return h % htable_size;
}

struct oscap_htable* oscap_htable_new1(oscap_compare_func cmp, size_t hsize)
{
	struct oscap_htable* t;
	t = malloc(sizeof(struct oscap_htable));
	if(t == NULL) return NULL;
	t->hsize = hsize;
	t->itemcount = 0;
	t->table = calloc(hsize, sizeof(struct oscap_list_item *));
	if(t->table == NULL)
	{
		free(t);
		return NULL;
	}
	t->cmp = cmp;
	return t;
}

struct oscap_htable* oscap_htable_new(void)
{
	return oscap_htable_new1(strcmp, OSCAP_DEFAULT_HSIZE);
}

bool oscap_htable_add(struct oscap_htable* htable, const char* key, void* item)
{
    assert(htable != NULL);
    unsigned int hashcode = oscap_htable_hash(key, htable->hsize);
    struct oscap_htable_item* htitem = htable->table[hashcode];
    while(htitem != NULL)
    {
        if( (*(htable->cmp))(htitem->key, key) == 0) return false;
        if(htitem->next == NULL) break;
        htitem = htitem->next;
    }
    // htitem points to the last item
    struct oscap_htable_item* newhtitem;
    newhtitem = malloc(sizeof(struct oscap_htable_item));
    if(newhtitem == NULL) return false;
    newhtitem->key = strdup(key);
    newhtitem->next = NULL;
    newhtitem->value = item;
    if(htitem == NULL)
        htable->table[hashcode] = newhtitem;
    else htitem->next = newhtitem;
    htable->itemcount++;
    return true;
}

	
void* oscap_htable_get(struct oscap_htable* htable, const char* key)
{
	unsigned int hashcode = oscap_htable_hash(key, htable->hsize);
	struct oscap_htable_item* htitem = htable->table[hashcode];
	while(htitem != NULL)
	{
		if( (*(htable->cmp))(htitem->key, key) == 0) return htitem->value;
		htitem = htitem->next;
	}
	return NULL;
}	

void oscap_print_depth(int);

void oscap_htable_dump(struct oscap_htable* htable, oscap_dump_func dumper, int depth)
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

void oscap_htable_free(struct oscap_htable* htable, oscap_destruct_func destructor)
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
	while (depth--) printf("  ");
}

