/*
 * oval_result_item.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "oval_results_impl.h"
#include "oval_collection_impl.h"
#include "oval_system_characteristics_impl.h"

typedef struct oval_result_item {
	struct oval_sysdata *sysdata;
	oval_result_enum result;
	char *message;
} oval_result_item_t;

int oval_iterator_result_item_has_more(struct oval_iterator_result_item
				       *oc_result_item)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_result_item);
}

struct oval_result_item *oval_iterator_result_item_next(struct
							oval_iterator_result_item
							*oc_result_item)
{
	return (struct oval_result_item *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_result_item);
}

struct oval_sysdata *oval_result_item_sysdata(struct oval_result_item *item)
{
	return ((struct oval_result_item *)item)->sysdata;
}

oval_result_enum oval_result_item_result(struct oval_result_item *item)
{
	return ((struct oval_result_item *)item)->result;
}

char *oval_result_item_message(struct oval_result_item *item)
{
	return ((struct oval_result_item *)item)->message;
}
