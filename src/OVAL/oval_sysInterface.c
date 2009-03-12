/*
 * oval_sysInterface.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_system_characteristics_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_sysInterface_s{
	char* name      ;
	char* ipAddress ;
	char* macAddress;
} Oval_sysInterface_t;

typedef Oval_sysInterface_t* Oval_sysInterface_ptr;

OvalCollection_sysInterface newOvalCollection_sysInterface(Oval_sysInterface* sysInterface_array){
	return (OvalCollection_sysInterface)newOvalCollection((OvalCollection_target*)sysInterface_array);
}
int   OvalCollection_sysInterface_hasMore      (OvalCollection_sysInterface oc_sysInterface){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_sysInterface);
}
Oval_sysInterface OvalCollection_sysInterface_next         (OvalCollection_sysInterface oc_sysInterface){
	return (Oval_sysInterface)OvalCollection_next((OvalCollection_ptr)oc_sysInterface);
}

char* Oval_sysInterface_name      (Oval_sysInterface sysInterface){
	return ((Oval_sysInterface_ptr)sysInterface)->name;
}
char* Oval_sysInterface_ipAddress (Oval_sysInterface sysInterface){
	return ((Oval_sysInterface_ptr)sysInterface)->ipAddress;
}
char* Oval_sysInterface_macAddress(Oval_sysInterface sysInterface){
	return ((Oval_sysInterface_ptr)sysInterface)->macAddress;
}
