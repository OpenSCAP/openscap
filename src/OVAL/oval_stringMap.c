/**
 * @file oval_stringMap.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "oval_string_map_impl.h"
#include "../common/util.h"
#include "../common/public/debug.h"

struct _oval_string_map_entry_s;

typedef struct _oval_string_map_entry {
	struct _oval_string_map_entry *next;
	char *key;
	void *item;
} _oval_string_map_entry_t;

typedef struct oval_string_map {
	struct _oval_string_map_entry *entries;
} oval_string_map_t;

/* failed   - NULL 
 * success  - oval_definition_model 
 * */
struct oval_string_map *oval_string_map_new()
{
	struct oval_string_map *map = (struct oval_string_map *)oscap_alloc(sizeof(oval_string_map_t));
	if (map == NULL)
		return NULL;

	map->entries = NULL;
	return map;
}

static struct _oval_string_map_entry *_oval_string_map_entry_new(struct
								 _oval_string_map_entry
								 *after, struct
								 _oval_string_map_entry
								 *before)
{
	struct _oval_string_map_entry *entry =
	    (struct _oval_string_map_entry *)oscap_alloc(sizeof(_oval_string_map_entry_t));
	if (entry == NULL)
		return NULL;

	entry->next = before;
	if (after != NULL)
		after->next = entry;
	return entry;
}

void oval_string_map_put(struct oval_string_map *map, const char *key, void *item)
{
	__attribute__nonnull__(map);

	char *temp = (char *)oscap_alloc((strlen(key) + 1) * sizeof(char) + 1);
	char *usekey = strcpy(temp, key);

	/* SEARCH FOR INSERTION POINT */
	struct _oval_string_map_entry *insert_before = map->entries, *insert_after = NULL, *insertion;
	if (insert_before == NULL) {
		map->entries = insertion = _oval_string_map_entry_new(NULL, NULL);
	} else {
		int compare;
		while (insert_before != NULL && ((compare = strcmp(usekey, insert_before->key)) < 0)) {
			insert_after = insert_before;
			insert_before = insert_after->next;
		}
		if (insert_before == NULL) {
			insertion = _oval_string_map_entry_new(insert_after, NULL);
		} else if (compare == 0) {
			insertion = insert_before;
		} else {
			insertion = _oval_string_map_entry_new(insert_after, insert_before);
			if (insert_after == NULL)
				map->entries = insertion;
		}
	}
	if (insertion == NULL) {
		free(temp);
		return;
	}

	insertion->key = usekey;
	insertion->item = item;
}

void oval_string_map_put_string(struct oval_string_map *map, const char *key, const char *item)
{
	char *temp = (char *)oscap_alloc((strlen(item) + 1) * sizeof(char) + 1);
	char *useval = strcpy(temp, item);
	oval_string_map_put(map, key, useval);
}

struct oval_iterator *oval_string_map_keys(struct oval_string_map *map)
{
	__attribute__nonnull__(map);

	struct oval_iterator *iterator = oval_collection_iterator_new();
	struct _oval_string_map_entry *entry = map->entries;
	while (entry != NULL) {
		oval_collection_iterator_add(iterator, (void *)entry->key);
		entry = entry->next;
	}
	return iterator;
}

struct oval_iterator *oval_string_map_values(struct oval_string_map *map)
{
	__attribute__nonnull__(map);

	struct oval_iterator *iterator = oval_collection_iterator_new();
	struct _oval_string_map_entry *entry = map->entries;
	int count;
	for (count = 0; entry != NULL; count++) {
		oval_collection_iterator_add(iterator, entry->item);
		entry = entry->next;
	}
	return iterator;
}

void *oval_string_map_get_value(struct oval_string_map *map, char *key)
{
	__attribute__nonnull__(map);

	if (key == NULL)
		return NULL;

	struct _oval_string_map_entry *entry;
	for (entry = map->entries; (entry != NULL) && (strcmp(key, entry->key) != 0); entry = entry->next) {
	}
	return (entry == NULL) ? NULL : entry->item;
}

void oval_string_map_free(struct oval_string_map *map, oscap_destruct_func free_func)
{
	__attribute__nonnull__(map);

	struct _oval_string_map_entry *entry = map->entries;
	struct _oval_string_map_entry *next;
	while (entry != NULL) {
		if (free_func != NULL)
			if (entry->item)
				(*free_func) (entry->item);
		next = entry->next;
		oscap_free(entry->key);
		entry->item = NULL;
		entry->key = NULL;
		entry->next = NULL;
		oscap_free(entry);
		entry = next;
	}
	oscap_free(map);
}

void oval_string_map_free_string(struct oval_string_map *map)
{
	oval_string_map_free(map, oscap_free);
}

//TEST FREEFUNC
/*static void oval_string_map_main_freefunc(void *item)
{
	oscap_dprintf("FREEFUNC: item = %s\n", (const char *) item);
}*/

/**
 * This method is included as a test stub.
 */
/*int oval_string_map_main(int argc, char **argv)
{

	oscap_dprintf("TEST::START\n");
	char *keys[] = { "key1", "key3", "key0", "key2", "key2", 0 };
	char *entries[] =
	    { "hello", "tom", "now is the time", "for all good men", "for me",
      0 };

	struct oval_string_map *map = oval_string_map_new();
	oscap_dprintf("TEST::START::has new map\n");
	int idx;
	for (idx = 0; keys[idx] != NULL; idx++) {
		char *key = keys[idx];
		oscap_dprintf("TEST::adding key %s -> %s\n", key, entries[idx]);
		oval_string_map_put(map, key, (void *)entries[idx]);
	}
	for (idx = 0; keys[idx] != NULL; idx++) {
		char *key = keys[idx];
		oscap_dprintf("TEST::getting key %s -> %s\n", key,
		       (const char *) oval_string_map_get_value(map, key));
	}

	oval_string_map_free(map, &oval_string_map_main_freefunc);
	return 0;
}*/
