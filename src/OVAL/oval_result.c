/*
 * oval_result.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "oval_results_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_result {
	struct oval_definition *definition;
	oval_result_enum result;
	char *message;
	struct oval_result_directives *directives;
	struct oval_result_criteria_node *criteria;
} oval_result_t;

int oval_iterator_result_has_more(struct oval_iterator_result *oc_result)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_result);
}

struct oval_result *oval_iterator_result_next(struct oval_iterator_result
					      *oc_result)
{
	return (struct oval_result *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_result);
}

struct oval_definition *oval_result_definition(struct oval_result *result)
{
	return ((struct oval_result *)result)->definition;
}

oval_result_enum oval_result_result(struct oval_result *result)
{
	return ((struct oval_result *)result)->result;
}

char *oval_result_message(struct oval_result *result)
{
	return ((struct oval_result *)result)->message;
}

struct oval_result_directives *oval_directives(struct oval_result *result)
{
	return ((struct oval_result *)result)->directives;
}

struct oval_result_criteria_node *oval_result_criteria(struct oval_result
						       *result)
{
	return ((struct oval_result *)result)->criteria;
}
