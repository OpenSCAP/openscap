/*
 * oval_stringMap.c
 *
 *  Created on: Mar 9, 2009
 *      Author: david.niemoller
 */
#include "includes/oval_string_map_impl.h"
#include <string.h>

	struct _oval_string_map_entry_s;

	typedef struct _oval_string_map_entry_s{
		struct _oval_string_map_entry_s *next;
		char*                      key ;
		struct oval_collection_item_s *  item;
	} _oval_string_map_entry_t;

	typedef struct oval_string_map_s{
		struct _oval_string_map_entry_s *entries;
	} oval_string_map_t;

	struct oval_string_map_s *oval_string_map_new(){
		struct oval_string_map_s *map = (struct oval_string_map_s*)malloc(sizeof(oval_string_map_t));
		map->entries = NULL;
		return map;
	}

	struct _oval_string_map_entry_s *_oval_string_map_entry_new(struct _oval_string_map_entry_s *after, struct _oval_string_map_entry_s *before){
		struct _oval_string_map_entry_s *entry = (struct _oval_string_map_entry_s*)malloc(sizeof(_oval_string_map_entry_t));
		entry->next = before;
		if(after!=NULL)after->next = entry;
		return entry;
	}

	void oval_string_map_put(struct oval_string_map_s *map, char* key,struct oval_collection_item_s *item){
		char* temp = (char*)malloc((strlen(key)+1)*sizeof(char)+1);
		key = strcpy(temp, key);
		//SEARCH FOR INSERTION POINT
		struct _oval_string_map_entry_s *insert_before = map->entries, *insert_after = NULL, *insertion;
		if(insert_before==NULL){
			map->entries = insertion = _oval_string_map_entry_new(NULL, NULL);
		}else{
			int compare;
			while( insert_before!=NULL  && ((compare = strcmp(key,insert_before->key))<0)){
				insert_after = insert_before;
				insert_before = insert_after->next;
			}
			if(insert_before==NULL){
				insertion = _oval_string_map_entry_new(insert_after, NULL);
			}else if(compare==0){
				insertion = insert_before;
			}else {
				insertion = _oval_string_map_entry_new(insert_after, insert_before);
				if(insert_after==NULL)map->entries = insertion;
			}
		}
		insertion->key  = key;
		insertion->item = item;
	}

	struct oval_iterator_s *oval_string_map_keys(struct oval_string_map_s *map){
		struct oval_iterator_s *iterator = oval_collection_iterator_new();
		struct _oval_string_map_entry_s *entry = map->entries;
		while(entry!=NULL){
			oval_collection_iterator_add(iterator, entry->key);
			entry = entry->next;
		}
		return iterator;
	}
	struct oval_iterator_s *oval_string_map_values   (struct oval_string_map_s *map){
		struct oval_iterator_s *iterator = oval_collection_iterator_new();
		struct _oval_string_map_entry_s *entry = map->entries;
		while(entry!=NULL){
			oval_collection_iterator_add(iterator, entry->item);
			entry = entry->next;
		}
		return iterator;
	}
	struct oval_collection_item_s *oval_string_map_get_value(struct oval_string_map_s *map, char* key){
		struct _oval_string_map_entry_s *entry;
		for(entry = map->entries; (entry!=NULL) && (strcmp(key,entry->key)!=0);entry = entry->next){
		}
		return (entry==NULL)?NULL:entry->item;
	}
	void oval_string_map_free(struct oval_string_map_s *map, oval_item_free_func free_func){
		struct _oval_string_map_entry_s *entry = map->entries;
		struct _oval_string_map_entry_s *next;
		while(entry!=NULL){
			if(free_func!=NULL)(*free_func)(entry->item);
			next = entry->next;
			free(entry->key);
			free(entry);
			entry = next;
		}
		free(map);
	}
	//TEST FREEFUNC
	void oval_string_map_main_freefunc(struct oval_collection_item_s *item){
		printf("FREEFUNC: item = %s\n",item);
	}

	//TEST MAIN
	int oval_string_map_main(int argc, char **argv){

		printf("TEST::START\n");
		char* keys   [] = {"key1" , "key3", "key0"           , "key2"            , "key2",0};
		char* entries[] = {"hello", "tom" , "now is the time", "for all good men", "for me", 0};

		struct oval_string_map_s *map = oval_string_map_new();
		printf("TEST::START::has new map\n");
		int index;
		for(index=0;keys[index]!=NULL;index++){
			char* key = keys[index];
			printf("TEST::adding key %s -> %s\n",key,entries[index]);
			oval_string_map_put(map, key, entries[index]);
		}
		for(index=0;keys[index]!=NULL;index++){
			char* key = keys[index];
			printf("TEST::getting key %s -> %s\n",key,oval_string_map_get_value(map,key));
		}

		oval_string_map_free(map, &oval_string_map_main_freefunc);
	}
