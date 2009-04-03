/**
 * @file oval_collection.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

#include <stdlib.h>
#include <stdio.h>
#include "oval_definitions_impl.h"
#include "oval_collection_impl.h"

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

struct oval_collection *oval_collection_new()
{
	struct oval_collection *collection =
	    (struct oval_collection *)malloc(sizeof(oval_collection_t));
	collection->item_collection_frame = NULL;
	return collection;
}

void oval_collection_free(struct oval_collection *collection)
{
	oval_collection_free_items(collection, NULL);
}

void oval_collection_free_items(struct oval_collection *collection,
				oval_item_free_func free_func)
{
	struct _oval_collection_item_frame *frame =
	    collection->item_collection_frame;
	while (frame != NULL) {
		if (free_func != NULL) {
			void *item = frame->item;
			(*free_func) (item);
		}
		struct _oval_collection_item_frame *temp = frame;
		frame = frame->next;
		free(temp);
	}
	free(collection);
}

void oval_collection_add(struct oval_collection *collection, void *item)
{
	struct _oval_collection_item_frame *next =
	    malloc(sizeof(_oval_collection_item_frame_t));
	next->next = collection->item_collection_frame;
	collection->item_collection_frame = next;
	next->item = item;
}

struct oval_iterator *oval_collection_iterator(struct oval_collection
					       *collection)
{
	struct oval_iterator *iterator =
	    (struct oval_iterator *)malloc(sizeof(oval_iterator_t));
	iterator->item_iterator_frame = NULL;
	struct _oval_collection_item_frame *collection_frame =
	    collection->item_collection_frame;
	while (collection_frame != NULL) {
		struct _oval_collection_item_frame *iterator_frame =
		    (struct _oval_collection_item_frame *)
		    malloc(sizeof(_oval_collection_item_frame_t));
		iterator_frame->next = iterator->item_iterator_frame;
		iterator_frame->item = collection_frame->item;
		iterator->item_iterator_frame = iterator_frame;
		collection_frame = collection_frame->next;
	}
	return iterator;
}

int oval_collection_iterator_has_more(struct oval_iterator *iterator)
{
	int has_more = iterator->item_iterator_frame != NULL;
	if (!has_more)
		free(iterator);
	return has_more;
}

void *oval_collection_iterator_next(struct oval_iterator *iterator)
{
	struct _oval_collection_item_frame *oc_next =
	    iterator->item_iterator_frame;
	void *next;
	if (oc_next == NULL) {
		free(iterator);
		next = NULL;
	} else {
		next = oc_next->item;
		iterator->item_iterator_frame = oc_next->next;
		free(oc_next);
	}
	return next;
}

struct oval_iterator_string *oval_collection_string_iterator(struct
							     oval_collection
							     *os_string)
{
	return (struct oval_iterator_string *)
	    oval_collection_iterator(os_string);
}

struct oval_iterator *oval_collection_iterator_new()
{
	struct oval_iterator *iterator =
	    (struct oval_iterator *)malloc(sizeof(oval_iterator_t));
	iterator->item_iterator_frame = NULL;
	return iterator;
}

void oval_collection_iterator_add(struct oval_iterator *iterator, void *item)
{
	struct _oval_collection_item_frame *newframe =
	    (struct _oval_collection_item_frame *)
	    malloc(sizeof(_oval_collection_item_frame_t));
	newframe->next = iterator->item_iterator_frame;
	newframe->item = item;
	iterator->item_iterator_frame = newframe;
}

int oval_iterator_string_has_more(struct oval_iterator_string *iterator)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 iterator);
}

char *oval_iterator_string_next(struct oval_iterator_string *iterator)
{
	return (char *)oval_collection_iterator_next((struct oval_iterator *)
						     iterator);
}

//TEST FREEFUNC
void oval_collection_main_freefunc(void *item)
{
	printf("FREEFUNC: item = %s\n", item);
}

//TEST MAIN
int oval_collection_main(int argc, char **argv)
{

	char *array[] =
	    { "hello\0", "tom\0", "now is the time\0", "for all good men\0",
	0 };
	struct oval_collection *collection = oval_collection_new();
	char **arrayin;
	for (arrayin = array; *arrayin != NULL; arrayin++) {
		char *string = *arrayin;
		oval_collection_add(collection, (void *)string);
	}

	int i;
	for (i = 0; i < 2; i++) {
		struct oval_iterator *iterator =
		    oval_collection_iterator(collection);
		while (oval_collection_iterator_has_more(iterator))
			printf("[%d] string = %s\n", i,
			       oval_collection_iterator_next(iterator));
	}

	oval_collection_free_items(collection, &oval_collection_main_freefunc);
}
