/*
 * oval_reference.c
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct oval_reference_s{
	char* source;
	char* id    ;
	char* url   ;
} oval_reference_t;

int   oval_iterator_reference_has_more(struct oval_iterator_reference_s *oc_reference){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_reference);
}
struct oval_reference_s *oval_iterator_reference_next         (struct oval_iterator_reference_s *oc_reference){
	return (struct oval_reference_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_reference);
}

char* oval_reference_source(struct oval_reference_s *reference){
	return ((struct oval_reference_s*)reference)->source;
}
char* oval_reference_id    (struct oval_reference_s *reference){
	return ((struct oval_reference_s*)reference)->id;
}
char* oval_reference_url   (struct oval_reference_s *reference){
	return ((struct oval_reference_s*)reference)->url;
}

struct oval_reference_s *oval_reference_new(){
	struct oval_reference_s *reference = (struct oval_reference_s*)malloc(sizeof(oval_reference_t));
	reference->id     = NULL;
	reference->source = NULL;
	reference->url    = NULL;
	return reference;
}

void oval_reference_free(struct oval_reference_s *reference){
	if(reference->id     != NULL)free(reference->id);
	if(reference->source != NULL)free(reference->source);
	if(reference->url    != NULL)free(reference->url);
	free(reference);
}

void set_oval_reference_source(struct oval_reference_s*, char*);
void set_oval_reference_id    (struct oval_reference_s*, char*);
void set_oval_reference_url   (struct oval_reference_s*, char*);

void oval_reference_to_print(struct oval_reference_s *reference, char* indent, int index){
	char nxtindent[100];*nxtindent = 0;
	strcat(nxtindent,indent);
	if(index==0)strcat(nxtindent,"REFERENCE.");
	else{
		strcat(nxtindent,"REFERENCE[");
		char itoad[10];itoa(index,itoad,10);
		strcat(nxtindent,itoad);
		strcat(nxtindent,"].");
	}
	printf("%sSOURCE = %s\n",nxtindent,reference->source);
	printf("%sID     = %s\n",nxtindent,reference->id);
	printf("%sURL    = %s\n",nxtindent,reference->url);
}

