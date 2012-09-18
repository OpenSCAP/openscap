/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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
 *	Simon Lukasik <slukasik@redhat.com>
 */

#include <stdio.h>
#include <string.h>
#include "common/list.h"
#include "common/util.h"
#include "../../../assume.h"

static bool _simple_string_filter(void *first, void *second)
{
	return !oscap_strcmp((char *)first, (char *)second);
}

static void _test_first_item_is_not_skipped(void)
{
	// Test that first item is not skipped when using filter.
	struct oscap_stringlist *names = oscap_stringlist_new();
	assume(oscap_stringlist_add_string(names, "Peter"));
	assume(oscap_stringlist_add_string(names, "Tomas"));
	assume(oscap_stringlist_add_string(names, "Peter"));
	struct oscap_string_iterator *it =
		(struct oscap_string_iterator*) oscap_iterator_new_filter(
			(struct oscap_list *) names,
			(oscap_filter_func) _simple_string_filter,
			"Peter");
	assume(oscap_string_iterator_has_more(it));
	assume(strcmp(oscap_string_iterator_next(it), "Peter") == 0);
	assume(oscap_string_iterator_has_more(it));
	assume(strcmp(oscap_string_iterator_next(it), "Peter") == 0);
	assume(oscap_string_iterator_has_more(it) == false);

	oscap_string_iterator_reset(it);
	assume(oscap_string_iterator_has_more(it));
	assume(strcmp(oscap_string_iterator_next(it), "Peter") == 0);
	assume(oscap_string_iterator_has_more(it));
	assume(strcmp(oscap_string_iterator_next(it), "Peter") == 0);
	assume(oscap_string_iterator_has_more(it) == false);
	oscap_string_iterator_free(it);
	oscap_stringlist_free(names);
}

static void _test_not_matching_last_item_is_not_returned(void)
{
	// Test that oscap_iterator_has_more works with filter.
	struct oscap_stringlist *names = oscap_stringlist_new();
	assume(oscap_stringlist_add_string(names, "Peter"));
	struct oscap_string_iterator *it =
		(struct oscap_string_iterator*) oscap_iterator_new_filter(
			(struct oscap_list *) names,
			(oscap_filter_func) _simple_string_filter,
			"Tomas");
	assume(oscap_string_iterator_has_more(it) == false);
	oscap_string_iterator_reset(it);
	assume(oscap_string_iterator_has_more(it) == false);
	oscap_string_iterator_free(it);
	oscap_stringlist_free(names);
}

static void _test_empty_list_has_more(void)
{
	struct oscap_stringlist *names = oscap_stringlist_new();
	struct oscap_string_iterator *it = oscap_stringlist_get_strings(names);
	assume(oscap_string_iterator_has_more(it) == false);
	oscap_string_iterator_reset(it);
	assume(oscap_string_iterator_has_more(it) == false);
	oscap_string_iterator_free(it);
	oscap_stringlist_free(names);
}

static void _test_empty_list_filter_has_more(void)
{
	struct oscap_stringlist *names = oscap_stringlist_new();
	struct oscap_string_iterator *it =
		(struct oscap_string_iterator*) oscap_iterator_new_filter(
			(struct oscap_list *) names,
			(oscap_filter_func) _simple_string_filter,
			NULL);
	assume(oscap_string_iterator_has_more(it) == false);
	oscap_string_iterator_reset(it);
	assume(oscap_string_iterator_has_more(it) == false);
	oscap_string_iterator_free(it);
	oscap_stringlist_free(names);
}

int main(int argc, char *argv[])
{
	_test_first_item_is_not_skipped();
	_test_not_matching_last_item_is_not_returned();
	_test_empty_list_has_more();
	_test_empty_list_filter_has_more();
	return 0;
}
