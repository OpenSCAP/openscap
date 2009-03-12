/*
 * oval_resultItem.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_results_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_resultItem_s{
	Oval_sysData sysData   ;
	Oval_result_enum result;
	char* message          ;
} Oval_resultItem_t;

typedef Oval_resultItem_t* Oval_resultItem_ptr;

OvalCollection_resultItem newOvalCollection_resultItem(Oval_resultItem* resultItem_array){
	return (OvalCollection_resultItem)newOvalCollection((OvalCollection_target*)resultItem_array);
}
int   OvalCollection_resultItem_hasMore      (OvalCollection_resultItem oc_resultItem){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_resultItem);
}
Oval_resultItem OvalCollection_resultItem_next         (OvalCollection_resultItem oc_resultItem){
	return (Oval_resultItem)OvalCollection_next((OvalCollection_ptr)oc_resultItem);
}

Oval_sysData Oval_resultItem_sysData   (Oval_resultItem item){
	return ((Oval_resultItem_ptr)item)->sysData;
}
Oval_result_enum Oval_resultItem_result(Oval_resultItem item){
	return ((Oval_resultItem_ptr)item)->result;
}
char* Oval_resultItem_message          (Oval_resultItem item){
	return ((Oval_resultItem_ptr)item)->message;
}
