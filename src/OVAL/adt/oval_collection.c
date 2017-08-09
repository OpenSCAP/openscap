/**
 * @file oval_collection.c
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "oval_adt.h"
#include "oval_collection_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"

/***************************************************************************/
/* Variable definitions
 * */

typedef struct _oval_collection_item_frame {
	struct _oval_collection_item_frame *next;
	void *item;
} _oval_collection_item_frame_t;

typedef struct oval_collection {
	struct _oval_collection_item_frame *item_collection_frame;
} oval_collection_t;

typedef struct oval_iterator {
	struct _oval_collection_item_frame *item_iterator_frame;
} oval_iterator_t;

static bool debug = true;
static struct oval_iterator *_debugStack[0];
static int iterator_count;

/* End of variable definitions
 * */
/***************************************************************************/

struct oval_collection *oval_collection_new()
{
	struct oval_collection *collection = (struct oval_collection *)malloc(sizeof(oval_collection_t));
	if (collection == NULL)
		return NULL;

	collection->item_collection_frame = NULL;
	return collection;
}

void oval_collection_free(struct oval_collection *collection)
{
	oval_collection_free_items(collection, NULL);
}

void oval_collection_free_items(struct oval_collection *collection, oscap_destruct_func free_func)
{
	if (collection) {
		struct _oval_collection_item_frame *frame = collection->item_collection_frame;
		while (frame != NULL) {
			if (free_func != NULL) {
				void *item = frame->item;
				if (item)
					(*free_func) (item);
				frame->item = NULL;
			}
			struct _oval_collection_item_frame *temp = frame;
			frame = frame->next;
			temp->next = NULL;
			free(temp);
		}
		free(collection);
	}
}

int oval_collection_is_empty(struct oval_collection *collection)
{
	__attribute__nonnull__(collection);
	return collection->item_collection_frame == NULL;
}

void oval_collection_add(struct oval_collection *collection, void *item)
{
	__attribute__nonnull__(collection);

	struct _oval_collection_item_frame *next = malloc(sizeof(_oval_collection_item_frame_t));
	if (next == NULL)
		return;

	next->next = collection->item_collection_frame;
	collection->item_collection_frame = next;
	next->item = item;
}

struct oval_iterator *oval_collection_iterator(struct oval_collection *collection)
{
	__attribute__nonnull__(collection);

	struct oval_iterator *iterator = (struct oval_iterator *)malloc(sizeof(oval_iterator_t));
	if (iterator == NULL)
		return NULL;

	if ((iterator_count++) < 0) {
		_debugStack[iterator_count - 1] = iterator;
		dW("iterator_count: %d.", iterator_count);
	}

	iterator->item_iterator_frame = NULL;
	struct _oval_collection_item_frame *collection_frame = collection->item_collection_frame;

	while (collection_frame != NULL) {
		struct _oval_collection_item_frame *iterator_frame =
		    (struct _oval_collection_item_frame *)malloc(sizeof(_oval_collection_item_frame_t));
		if (iterator_frame == NULL)
			return NULL;

		iterator_frame->next = iterator->item_iterator_frame;
		iterator_frame->item = collection_frame->item;
		iterator->item_iterator_frame = iterator_frame;
		collection_frame = collection_frame->next;
	}
	return iterator;
}

bool oval_collection_iterator_has_more(struct oval_iterator * iterator)
{
	__attribute__nonnull__(iterator);

	return iterator->item_iterator_frame != NULL;
}

int oval_collection_iterator_remaining(struct oval_iterator *iterator)
{

	__attribute__nonnull__(iterator);

	int remaining;
	struct _oval_collection_item_frame *next = iterator->item_iterator_frame;

	for (remaining = 0; next; remaining++)
		next = next->next;

	return remaining;
}

void *oval_collection_iterator_next(struct oval_iterator *iterator)
{
	__attribute__nonnull__(iterator);

	struct _oval_collection_item_frame *oc_next = iterator->item_iterator_frame;
	void *next;

	if (oc_next == NULL) {
		next = NULL;
	} else {
		next = oc_next->item;
		iterator->item_iterator_frame = oc_next->next;
		oc_next->item = NULL;
		oc_next->next = NULL;
		free(oc_next);
	}
	return next;
}

void oval_collection_iterator_free(struct oval_iterator *iterator)
{
	if (iterator) {		//NOOP if iterator is NULL
		if ((--iterator_count) < 0) {
			dW("iterator_count: %d.", iterator_count);
			if (iterator != _debugStack[iterator_count]) {
				debug = false;
			}
		}

		while (iterator->item_iterator_frame) {
			struct _oval_collection_item_frame *oc_this;
			oc_this = iterator->item_iterator_frame;
			iterator->item_iterator_frame = oc_this->next;
			oc_this->item = NULL;
			oc_this->next = NULL;
			free(oc_this);
		}
		iterator->item_iterator_frame = NULL;
		free(iterator);
	}
}

struct oval_iterator *oval_collection_iterator_new()
{
	struct oval_iterator *iterator = (struct oval_iterator *)malloc(sizeof(oval_iterator_t));
	if (iterator == NULL)
		return NULL;

	if ((iterator_count++) < 0) {
		_debugStack[iterator_count - 1] = iterator;
		dW("iterator_count: %d.", iterator_count);
	}
	iterator->item_iterator_frame = NULL;
	return iterator;
}

void oval_collection_iterator_add(struct oval_iterator *iterator, void *item)
{
	__attribute__nonnull__(iterator);

	struct _oval_collection_item_frame *newframe =
	    (struct _oval_collection_item_frame *)malloc(sizeof(_oval_collection_item_frame_t));
	if (newframe == NULL)	/* We don't have any information that error occured ! */
		return;

	newframe->next = iterator->item_iterator_frame;
	newframe->item = item;
	iterator->item_iterator_frame = newframe;
}

bool oval_string_iterator_has_more(struct oval_string_iterator * iterator)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)iterator);
}

int oval_string_iterator_remaining(struct oval_string_iterator *iterator)
{
	return oval_collection_iterator_remaining((struct oval_iterator *)iterator);
}

char *oval_string_iterator_next(struct oval_string_iterator *iterator)
{
	return (char *)oval_collection_iterator_next((struct oval_iterator *)iterator);
}

void oval_string_iterator_free(struct oval_string_iterator *iterator)
{
	oval_collection_iterator_free((struct oval_iterator *)iterator);
}
