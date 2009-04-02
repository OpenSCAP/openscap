/*
 * oval_result_test.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "oval_results_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_result_test {
	struct oval_test *test;
	oval_check_enum check;
	oval_result_enum result;
	char *message;
	struct oval_collection *items;
} oval_result_test_t;

int oval_iterator_result_test_has_more(struct oval_iterator_result_test
				       *oc_result_test)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_result_test);
}

struct oval_result_test *oval_iterator_result_test_next(struct
							oval_iterator_result_test
							*oc_result_test)
{
	return (struct oval_result_test *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_result_test);
}

struct oval_test *oval_result_test_test(struct oval_result_test *rtest)
{
	return ((struct oval_result_test *)rtest)->test;
}

oval_check_enum oval_result_test_check(struct oval_result_test *rtest)
{
	return ((struct oval_result_test *)rtest)->check;
}

oval_result_enum oval_result_test_result(struct oval_result_test *rtest)
{
	return ((struct oval_result_test *)rtest)->result;
}

char *oval_result_test_message(struct oval_result_test *rtest)
{
	return ((struct oval_result_test *)rtest)->message;
}

struct oval_iterator_result_item *oval_result_test_items(struct oval_result_test
							 *rtest)
{
	return (struct oval_iterator_result_item *)
	    oval_collection_iterator(rtest->items);
}
