/*
 * oval_collection.c
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct _oval_collection_item_frame_s{
	struct _oval_collection_item_frame_s *next;
	struct oval_collection_item_s *item;
}_oval_collection_item_frame_t;

typedef struct oval_collection_s{
	struct _oval_collection_item_frame_s *item_collection_frame;
} oval_collection_t;

typedef struct oval_iterator_s{
	struct _oval_collection_item_frame_s *item_iterator_frame;
} oval_iterator_t;

struct oval_collection_s *oval_collection_new(){
	struct oval_collection_s *collection = (struct oval_collection_s*)malloc(sizeof(oval_collection_t));
	collection->item_collection_frame = NULL;
	return collection;
}
void oval_collection_free (struct oval_collection_s *collection){
	oval_collection_free_items(collection,NULL);
}
void oval_collection_free_items(struct oval_collection_s *collection, oval_item_free_func free_func){
	struct _oval_collection_item_frame_s *frame = collection->item_collection_frame;
	while(frame!=NULL){
		if(free_func!=NULL){
			struct oval_collection_item_s *item = frame->item;
			(*free_func)(item);
		}
		struct _oval_collection_item_frame_s *temp = frame;
		frame = frame->next;
		free(temp);
	}
	free(collection);
}

void oval_collection_add(struct oval_collection_s *collection, struct oval_collection_item_s *item){
	struct _oval_collection_item_frame_s *next = malloc(sizeof(_oval_collection_item_frame_t));
	next->next = collection->item_collection_frame;
	collection->item_collection_frame = next;
	next->item = item;
}

struct oval_iterator_s *oval_collection_iterator(struct oval_collection_s *collection){
	struct oval_iterator_s *iterator = (struct oval_iterator_s*)malloc(sizeof(oval_iterator_t));
	iterator->item_iterator_frame = NULL;
	struct _oval_collection_item_frame_s *collection_frame = collection->item_collection_frame;
	while(collection_frame!=NULL){
		struct _oval_collection_item_frame_s *iterator_frame = (struct _oval_collection_item_frame_s*)malloc(sizeof(_oval_collection_item_frame_t));
		iterator_frame->next = iterator->item_iterator_frame;
		iterator_frame->item = collection_frame->item;
		iterator->item_iterator_frame = iterator_frame;
		collection_frame = collection_frame->next;
	}
	return iterator;
}

int oval_collection_iterator_has_more(struct oval_iterator_s *iterator){
	int has_more = iterator->item_iterator_frame!=NULL;
	if(!has_more)free(iterator);
	return has_more;
}
struct oval_collection_item_s *oval_collection_iterator_next(struct oval_iterator_s *iterator){
	struct _oval_collection_item_frame_s *oc_next = iterator->item_iterator_frame;
	struct oval_collection_item_s *next;
	if(oc_next==NULL){
		free(iterator);
		next = NULL;
	}else{
		next = oc_next->item;
		iterator->item_iterator_frame = oc_next->next;
		free(oc_next);
	}
	return next;
}
struct oval_iterator_string_s *oval_collection_string_iterator(struct oval_collection_s *oc_string){
	return (struct oval_iterator_string_s*)oval_collection_iterator(oc_string);
}
struct oval_iterator_s *oval_collection_iterator_new(){
	struct oval_iterator_s *iterator = (struct oval_iterator_s*)malloc(sizeof(oval_iterator_t));
	iterator->item_iterator_frame = NULL;
	return iterator;
}
void oval_collection_iterator_add(struct oval_iterator_s *iterator, struct oval_collection_item_s *item){
	struct _oval_collection_item_frame_s *newframe = (struct _oval_collection_item_frame_s*)malloc(sizeof(_oval_collection_item_frame_t));
	newframe->next = iterator->item_iterator_frame;
	newframe->item = item;
	iterator->item_iterator_frame = newframe;
}
int oval_iterator_string_has_more(struct oval_iterator_string_s *iterator){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)iterator);
}
char *oval_iterator_string_next(struct oval_iterator_string_s *iterator){
	return (char*)oval_collection_iterator_next((struct oval_iterator_s*)iterator);
}

//TEST FREEFUNC
void oval_collection_main_freefunc(struct oval_collection_item_s *item){
	printf("FREEFUNC: item = %s\n",item);
}
//TEST MAIN
int oval_collection_main(int argc, char **argv){

	char* array[] = {"hello\0", "tom\0", "now is the time\0", "for all good men\0", 0};
	struct oval_collection_s *collection = oval_collection_new();
	char** arrayin;
	for(arrayin = array;*arrayin!=NULL;arrayin++){
		char* string = *arrayin;
		oval_collection_add(collection, string);
	}

	int i;for(i=0;i<2;i++){
		struct oval_iterator_s *iterator = oval_collection_iterator(collection);
		while(oval_collection_iterator_has_more (iterator))printf("[%d] string = %s\n",i,oval_collection_iterator_next(iterator));
	}

	oval_collection_free_items(collection, &oval_collection_main_freefunc);
}

