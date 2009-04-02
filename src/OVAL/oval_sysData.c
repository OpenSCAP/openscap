/*
 * oval_sysdata.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "oval_system_characteristics_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_sysdata {
	oval_family_enum family;
	oval_subtype_enum subtype;
} oval_sysdata_t;

int oval_iterator_sysdata_has_more(struct oval_iterator_sysdata *oc_sysdata)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysdata);
}

struct oval_sysdata *oval_iterator_sysdata_next(struct oval_iterator_sysdata
						*oc_sysdata)
{
	return (struct oval_sysdata *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysdata);
}

oval_family_enum oval_sysdata_family(struct oval_sysdata *sysdata)
{
	return ((struct oval_sysdata *)sysdata)->family;
}

oval_subtype_enum oval_sysdata_subtype(struct oval_sysdata *sysdata)
{
	return ((struct oval_sysdata *)sysdata)->subtype;
}
