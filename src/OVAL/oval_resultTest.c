/**
 * @file oval_resultTest.c 
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
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
