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


struct xccdf_list* xccdf_list_new(void)
{
	struct xccdf_list* list = malloc(sizeof(struct xccdf_list));
	memset(list, 0, sizeof(struct xccdf_list));
	return list;
}

bool xccdf_list_add(struct xccdf_list* list, void* value)
{
	assert(list != NULL);
	struct xccdf_list_item* item = malloc(sizeof(struct xccdf_list_item));
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

void xccdf_list_delete(struct xccdf_list* list, xccdf_destruct_func destructor)
{
	struct xccdf_list_item *item, *to_del;
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

void xccdf_list_delete0(struct xccdf_list* list)
{
	xccdf_list_delete(list, NULL);
}

void xccdf_list_dump(struct xccdf_list* list, xccdf_dump_func dumper, int depth)
{
    if (list == NULL) {
        printf(" (NULL list)\n");
        return;
    }
    printf(" (list, %u item%s)\n", (unsigned)list->itemcount, (list->itemcount == 1 ? "" : "s"));
    struct xccdf_list_item *item = list->first;
    while (item) {
        dumper(item->data, depth);
        item = item->next;
    }
}

bool xccdf_iterator_no_filter(void* foo, void* bar) { bar = foo; return true; }

inline void xccdf_iterator_find_nearest(struct xccdf_iterator* it)
{
    while (it->cur && !it->filter(it->cur->data, it->user_data))
        it->cur = it->cur->next;
}

void* xccdf_iterator_new(struct xccdf_list* list)
{
    struct xccdf_iterator* it = calloc(1, sizeof(struct xccdf_iterator));
    it->cur = list->first;
    it->filter = xccdf_iterator_no_filter;
    return it;
}

void* xccdf_iterator_new_filter(struct xccdf_list* list, xccdf_filter_func filter, void* user_data)
{
    struct xccdf_iterator* it = xccdf_iterator_new(list);
    it->filter = filter;
    it->user_data = user_data;
    xccdf_iterator_find_nearest(it);
    return it;
}

void xccdf_iterator_delete(struct xccdf_iterator* it)
{
    free(it);
}

void* xccdf_iterator_next(struct xccdf_iterator* it)
{
	if (!it->cur) return NULL;
    void* ret = it->cur->data;
    it->cur = it->cur->next;
    xccdf_iterator_find_nearest(it);
    return ret;
}

bool xccdf_iterator_has_more(struct xccdf_iterator* it)
{
    if (it->cur) return true;
    xccdf_iterator_delete(it);
    return false;
}

XCCDF_ITERATOR_GEN_T(const char*, string)


#define XCCDF_DEFAULT_HSIZE 256

inline unsigned int xccdf_htable_hash(const char *str, size_t htable_size)
{
	unsigned h = 0;
	unsigned char *p;
	for(p=(unsigned char *) str; *p != '\0'; p++)  h = (97 * h) + *p;
	return h % htable_size;
}

struct xccdf_htable* xccdf_htable_new1(xccdf_compare_func cmp, size_t hsize)
{
	struct xccdf_htable* t;
	t = malloc(sizeof(struct xccdf_htable));
	if(t == NULL) return NULL;
	t->hsize = hsize;
	t->itemcount = 0;
	t->table = calloc(hsize, sizeof(struct xccdf_list_item *));
	if(t->table == NULL)
	{
		free(t);
		return NULL;
	}
	t->cmp = cmp;
	return t;
}

struct xccdf_htable* xccdf_htable_new(void)
{
	return xccdf_htable_new1(strcmp, XCCDF_DEFAULT_HSIZE);
}

bool xccdf_htable_add(struct xccdf_htable* htable, const char* key, void* item)
{
    assert(htable != NULL);
    unsigned int hashcode = xccdf_htable_hash(key, htable->hsize);
    struct xccdf_htable_item* htitem = htable->table[hashcode];
    while(htitem != NULL)
    {
        if( (*(htable->cmp))(htitem->key, key) == 0) return false;
        if(htitem->next == NULL) break;
        htitem = htitem->next;
    }
    // htitem points to the last item
    struct xccdf_htable_item* newhtitem;
    newhtitem = malloc(sizeof(struct xccdf_htable_item));
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

	
void* xccdf_htable_get(struct xccdf_htable* htable, const char* key)
{
	unsigned int hashcode = xccdf_htable_hash(key, htable->hsize);
	struct xccdf_htable_item* htitem = htable->table[hashcode];
	while(htitem != NULL)
	{
		if( (*(htable->cmp))(htitem->key, key) == 0) return htitem->value;
		htitem = htitem->next;
	}
	return NULL;
}	

void xccdf_htable_delete(struct xccdf_htable* htable, xccdf_destruct_func destructor)
{
    if (htable) {
        size_t ht;
        struct xccdf_htable_item *cur, *next;

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

