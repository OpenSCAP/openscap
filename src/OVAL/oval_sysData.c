/*
 * oval_sysdata.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_system_characteristics_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct oval_sysdata_s{
	oval_family_enum family  ;
	oval_subtype_enum subtype;
} oval_sysdata_t;

int   oval_iterator_sysdata_has_more       (struct oval_iterator_sysdata_s *oc_sysdata){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_sysdata);
}
struct oval_sysdata_s *oval_iterator_sysdata_next(struct oval_iterator_sysdata_s *oc_sysdata){
	return (struct oval_sysdata_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_sysdata);
}

oval_family_enum oval_sysdata_family  (struct oval_sysdata_s *sysdata){
	return ((struct oval_sysdata_s*)sysdata)->family;
}
oval_subtype_enum oval_sysdata_subtype(struct oval_sysdata_s *sysdata){
	return ((struct oval_sysdata_s*)sysdata)->subtype;
}
