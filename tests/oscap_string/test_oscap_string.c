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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/oscap_string.h"
#include "common/oscap_buffer.h"

int test_append_char(void);
int test_append_string(void);
int test_append_binary_data(void);
int test_null_parameters(void);
int test_buffer_clear(void);

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

#define ASSERT_LENGTH(STR, LENGTH) if (oscap_buffer_get_length(STR) != (LENGTH)) { fprintf(stderr, "Length of result does not match the required length.\n"); return 1;}

int test_append_binary_data()
{
	static const char* data[] = {
		"\0",
		"\0A",
		"ABC"
	};

	struct oscap_buffer *s = oscap_buffer_new();
	ASSERT_LENGTH(s, 0);

	oscap_buffer_append_binary_data(s, data[0], 1);
	ASSERT_LENGTH(s, 1);

	oscap_buffer_append_binary_data(s, data[1], 2);
	ASSERT_LENGTH(s, 3);

	oscap_buffer_append_binary_data(s, data[0], 0); // do not increase length
	ASSERT_LENGTH(s, 3);

	oscap_buffer_append_binary_data(s, data[2], 3);
	ASSERT_LENGTH(s, 6);


	// Check output data
	int length = oscap_buffer_get_length(s); // length is valid due to previous ASSERT_LENGTH
	static const char expected_result[] = {'\0', '\0', 'A', 'A', 'B', 'C'};
	const char* result = oscap_buffer_get_raw(s);
	for (int i = 0; i < length; ++i) {
		char expected = expected_result[i];
		char got = result[i];
		if ( expected != got ) {
			fprintf(stderr, "Invalid character at index %d, expected %x and got %x\n", i, expected, got);
			return 1;
		}
	}

	return 0;
}

int test_null_parameters() {
	struct oscap_buffer *s = oscap_buffer_new();
	oscap_buffer_append_binary_data(s, NULL, 4);

	if (oscap_buffer_get_length(s) != 0) {
		fprintf(stderr, "Appended NULL changed the length of string\n");
		return 1;
	}
	oscap_buffer_free(NULL);
	oscap_buffer_free(s);
	return 0;
}

int test_buffer_clear()
{
	struct oscap_buffer *s = oscap_buffer_new();
	ASSERT_LENGTH(s, 0);

	oscap_buffer_clear(s);
	ASSERT_LENGTH(s, 0);

	char data = 'A';
	oscap_buffer_append_binary_data(s, &data, 1);
	ASSERT_LENGTH(s, 1);

	oscap_buffer_clear(s);
	ASSERT_LENGTH(s, 0);

	oscap_buffer_free(s);
	return 0;
}

int main (int argc, char *argv[])
{
	int retval = 0;
	if ((retval = test_append_char()) != 0 ) {
		return retval;
	}

	if ((retval = test_append_string()) != 0 ) {
		return retval;
	}

	if ((retval = test_append_binary_data()) != 0 ) {
		return retval;
	}

	if ((retval = test_null_parameters()) != 0 ) {
		return retval;
	}

	if ((retval = test_buffer_clear()) != 0 ) {
		return retval;
	}

	return retval;
}
