/*
 * oval_syschar.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_system_characteristics_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_syschar_s{
	Oval_syscharCollectionFlag_enum flag  ;
	char** messages                       ;
	Oval_sysInfo sysInfo                  ;
	Oval_object object                    ;
	Oval_variableBinding* variableBindings;
	Oval_sysData* sysData                 ;
} Oval_syschar_t;

typedef Oval_syschar_t* Oval_syschar_ptr;

OvalCollection_syschar newOvalCollection_syschar(Oval_syschar* syschar_array){
	return (OvalCollection_syschar)newOvalCollection((OvalCollection_target*)syschar_array);
}
int   OvalCollection_syschar_hasMore      (OvalCollection_syschar oc_syschar){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_syschar);
}
Oval_syschar OvalCollection_syschar_next         (OvalCollection_syschar oc_syschar){
	return (Oval_syschar)OvalCollection_next((OvalCollection_ptr)oc_syschar);
}

Oval_syscharCollectionFlag_enum Oval_syschar_flag           (Oval_syschar syschar){
	return ((Oval_syschar_ptr)syschar)->flag;
}
OvalCollection_string Oval_syschar_messages                 (Oval_syschar syschar){
	char** messages = ((Oval_syschar_ptr)syschar)->messages;
	return newOvalCollection_string(messages);
}

Oval_sysInfo Oval_syschar_sysInfo                           (Oval_syschar syschar){
	return ((Oval_syschar_ptr)syschar)->sysInfo;
}
Oval_object Oval_syschar_object                             (Oval_syschar syschar){
	return ((Oval_syschar_ptr)syschar)->object;
}
OvalCollection_variableBinding Oval_syschar_variableBindings(Oval_syschar syschar){
	Oval_variableBinding* bindings = ((Oval_syschar_ptr)syschar)->variableBindings;
	return newOvalCollection_variableBinding(bindings);
}
OvalCollection_sysData Oval_syschar_sysData                 (Oval_syschar syschar){
	Oval_sysData* data = ((Oval_syschar_ptr)syschar)->sysData;
	return newOvalCollection_sysData(data);
}
