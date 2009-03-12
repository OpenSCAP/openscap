/*
 * oval_affected.c
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include <includes/oval_definitions.h>
#include "includes/oval_collection_impl.h"

	typedef struct Oval_affected_s{
		Oval_affected_family_enum family;
		char** platforms;
		char** products;
	} Oval_affected_t;

	typedef Oval_affected_t* Oval_affected_ptr;

	OvalCollection_affected newOvalCollection_affected(Oval_affected* affected_array){
		return (OvalCollection_affected)newOvalCollection((OvalCollection_target*)affected_array);
	}
	int   OvalCollection_affected_hasMore      (OvalCollection_affected oc_affected){
		return OvalCollection_hasMore((OvalCollection_ptr)oc_affected);
	}
	Oval_affected OvalCollection_affected_next         (OvalCollection_affected oc_affected){
		return (Oval_affected)OvalCollection_next((OvalCollection_ptr)oc_affected);
	}

	Oval_affected_family_enum Oval_affected_family  (Oval_affected affected){
		return ((Oval_affected_ptr)affected)->family;
	};
	OvalCollection_string     Oval_affected_platform(Oval_affected affected){
		char** platforms = ((Oval_affected_ptr)affected)->platforms;
		return newOvalCollection_string(platforms);
	};
	OvalCollection_string     Oval_affected_product (Oval_affected affected){
		char** products = ((Oval_affected_ptr)affected)->products;
		return newOvalCollection_string(products);
	}
