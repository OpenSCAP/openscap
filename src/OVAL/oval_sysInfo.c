/*
 * oval_sysInfo.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_system_characteristics_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_sysInfo_s{
	char* osName                          ;
	char* osVersion                       ;
	char* osArchitecture                  ;
	char* primaryHostName                 ;
	Oval_sysInterface* interfaces         ;
} Oval_sysInfo_t;

typedef Oval_sysInfo_t* Oval_sysInfo_ptr;

OvalCollection_sysInfo newOvalCollection_sysInfo(Oval_sysInfo* sysInfo_array){
	return (OvalCollection_sysInfo)newOvalCollection((OvalCollection_target*)sysInfo_array);
}
int   OvalCollection_sysInfo_hasMore      (OvalCollection_sysInfo oc_sysInfo){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_sysInfo);
}
Oval_sysInfo OvalCollection_sysInfo_next         (OvalCollection_sysInfo oc_sysInfo){
	return (Oval_sysInfo)OvalCollection_next((OvalCollection_ptr)oc_sysInfo);
}

char* Oval_sysInfo_osName                          (Oval_sysInfo sysInfo){
	return ((Oval_sysInfo_ptr)sysInfo)->osName;
}
char* Oval_sysInfo_osVersion                       (Oval_sysInfo sysInfo){
	return ((Oval_sysInfo_ptr)sysInfo)->osVersion;
}
char* Oval_sysInfo_osArchitecture                  (Oval_sysInfo sysInfo){
	return ((Oval_sysInfo_ptr)sysInfo)->osArchitecture;
}
char* Oval_sysInfo_primaryHostName                 (Oval_sysInfo sysInfo){
	return ((Oval_sysInfo_ptr)sysInfo)->primaryHostName;
}
OvalCollection_sysInterface Oval_sysInfo_interfaces(Oval_sysInfo sysInfo){
	Oval_sysInterface* interfaces = ((Oval_sysInfo_ptr)sysInfo)->interfaces;
	return newOvalCollection_sysInterface(interfaces);
}
