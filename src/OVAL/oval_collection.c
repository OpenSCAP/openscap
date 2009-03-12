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

struct OvalCollection_s{
	OvalCollection_target* target_ptr;
};

OvalCollection_ptr newOvalCollection(OvalCollection_target* target_ptr){
	OvalCollection_ptr ocp = (OvalCollection_ptr)malloc(sizeof(OvalCollection_t));
	ocp->target_ptr = target_ptr;

	return ocp;
}
int OvalCollection_hasMore(OvalCollection_ptr ocp){

	int hasMore = (*(ocp->target_ptr))!=NULL;
	if(!hasMore)free(ocp);
	return hasMore;
}
OvalCollection_target OvalCollection_next(OvalCollection_ptr ocp){
	OvalCollection_target* target_ptr = ocp->target_ptr++;
	return (OvalCollection_target)*target_ptr;
}

OvalCollection_string newOvalCollection_string(char** string_array){
	return (OvalCollection_string)newOvalCollection((OvalCollection_target*)string_array);
}
int   OvalCollection_string_hasMore      (OvalCollection_string oc_string){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_string);
}
char* OvalCollection_string_next         (OvalCollection_string oc_string){
	return (char*)OvalCollection_next((OvalCollection_ptr)oc_string);
}

//TEST MAIN
int main(int argc, char **argv){

	char* array[] = {"hello\0", "tom\0", NULL};

	/*
	OvalCollection_ptr ocp = newOvalCollection((OvalCollection_target*) array);

	printf("new collection has content: %d\n",OvalCollection_hasMore(ocp));

	while(OvalCollection_hasMore(ocp))printf("string = %s\n",OvalCollection_next(ocp));
	*/

	OvalCollection_string oc_string = newOvalCollection_string(array);

	printf("new collection has content: %d\n",OvalCollection_string_hasMore(oc_string));

	while(OvalCollection_string_hasMore(oc_string))printf("string = %s\n",OvalCollection_next(oc_string));
}
