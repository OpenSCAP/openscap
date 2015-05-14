/*
 * Copyright 2015 Red Hat Inc., Durham, North Carolina.
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
 *      Jan Černý <jcerny@redhat.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/oscap_string.h"

int test_append_char(void);
int test_append_string(void);

int test_append_char()
{
	const unsigned int limit = 1000;
	int retval = 0;
	struct oscap_string *s = oscap_string_new();
	for (unsigned int i = 0; i < limit; i++) {
		oscap_string_append_char(s, 'a');
	}
	if (strlen(oscap_string_get_cstr(s)) != limit) {
		fprintf(stderr, "Length of result does not match the required length.\n");
		retval = 1;
	}
	oscap_string_free(s);
	return retval;
}

int test_append_string()
{
	const int count_of_strings = 7;
	const char *strings[] = {
		"",
		"A",
		"ABCD",
		"ABCDEFGH",
		"012345678901234567890123456789012345678901234567890123456789012",
		"0123456789012345678901234567890123456789012345678901234567890123",
		"01234567890123456789012345678901234567890123456789012345678901234"
	};
	unsigned int length = 0;
	int retval = 0;
	struct oscap_string *s = oscap_string_new();
	for (int i = 0; i < count_of_strings; i++) {
		oscap_string_append_string(s, strings[i]);
		length += strlen(strings[i]);
	}
	if (strlen(oscap_string_get_cstr(s)) != length) {
		fprintf(stderr, "Length of result does not match the required length.\n");
		retval = 1;
	}
	oscap_string_free(s);
	return retval;
}

int main (int argc, char *argv[])
{
	int retval = 0;
	retval = test_append_char();
	retval = test_append_string();
	return retval;
}
