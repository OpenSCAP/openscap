/*
 * oval_sysData.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_system_characteristics_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_sysData_s{
	Oval_family_enum family  ;
	Oval_subtype_enum subtype;
} Oval_sysData_t;

typedef Oval_sysData_t* Oval_sysData_ptr;

OvalCollection_sysData newOvalCollection_sysData(Oval_sysData* sysData_array){
	return (OvalCollection_sysData)newOvalCollection((OvalCollection_target*)sysData_array);
}
int   OvalCollection_sysData_hasMore      (OvalCollection_sysData oc_sysData){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_sysData);
}
Oval_sysData OvalCollection_sysData_next         (OvalCollection_sysData oc_sysData){
	return (Oval_sysData)OvalCollection_next((OvalCollection_ptr)oc_sysData);
}

Oval_family_enum Oval_sysData_family  (Oval_sysData sysData){
	return ((Oval_sysData_ptr)sysData)->family;
}
Oval_subtype_enum Oval_sysData_subtype(Oval_sysData sysData){
	return ((Oval_sysData_ptr)sysData)->subtype;
}
