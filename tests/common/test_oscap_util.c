/*
 * Copyright 2023 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      Evgenii Kolesnikov <ekolesni@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/util.h"
#include "common/public/oscap_helpers.h"

int test_oscap_path_startswith(void);
int test_oscap_strrm(void);
int test_oscap_indent(void);
int test_oscap_remove_excess_whitespace(void);

int test_oscap_path_startswith()
{
	if (!oscap_path_startswith("/", "/"))
		return 1;
	if (!oscap_path_startswith("/aaa", "/"))
		return 2;
	if (!oscap_path_startswith("/aaa", "/aaa"))
		return 3;
	if (!oscap_path_startswith("/aaa/bbb", "/aaa"))
		return 4;
	if (!oscap_path_startswith("/aaa/bbb/ccc", "/aaa"))
		return 5;
	if (!oscap_path_startswith("/aaa/bbb/ccc/", "/aaa/"))
		return 6;
	if (!oscap_path_startswith("/aaa/bbb/ccc", "/aaa/bbb/ccc"))
		return 7;
	if (!oscap_path_startswith("/aaa/bbb/ccc", ""))
		return 8;
	if (!oscap_path_startswith("", ""))
		return 9;
	if (!oscap_path_startswith("", "/"))
		return 10;

	if (oscap_path_startswith("/bbb", "/aaa"))
		return 101;
	if (oscap_path_startswith("/bbb/aaa", "/aaa"))
		return 102;
	if (oscap_path_startswith("/bbb/aaa/", "/aaa"))
		return 103;
	if (oscap_path_startswith("/bbb/aaa/", "/aaa/"))
		return 104;

	return 0;
}

int test_oscap_strrm()
{
	char str[] = "abcdef12345678def90";
	size_t len = strlen(str);

	oscap_strrm(str, "0");
	if (strncmp(str, "abcdef12345678def9", len) != 0)
		return 1;

	oscap_strrm(str, "abc");
	if (strcmp(str, "def12345678def9") != 0)
		return 2;

	oscap_strrm(str, "def");
	if (strcmp(str, "123456789") != 0)
		return 3;

	oscap_strrm(str, "5");
	if (strcmp(str, "12346789") != 0)
		return 4;

	return 0;
}

int test_oscap_indent()
{
	char *indented;

	char *str1 = "hello";
	indented = oscap_indent(str1, 2);
	if (strcmp(indented, "  hello") != 0)
		return 1;
	free(indented);

	char *str2 = "hello";
	indented = oscap_indent(str2, 0);
	if (strcmp(indented, "hello") != 0)
		return 2;
	free(indented);

	char *str3 = "hello\nworld\nsee you tomorrow";
	indented = oscap_indent(str3, 4);
	if (strcmp(indented, "    hello\n    world\n    see you tomorrow") != 0)
		return 3;
	free(indented);

	char *str4 = "";
	indented = oscap_indent(str4, 2);
	if (strcmp(indented, "  ") != 0)
		return 4;
	free(indented);

	char *str5 = "hello\n\n\n\nbye\n";
	indented = oscap_indent(str5, 2);
	if (strcmp(indented, "  hello\n\n\n\n  bye\n") != 0)
		return 5;
	free(indented);

	char *str6 = "\n\n\n\n\n\n\n\n";
	indented = oscap_indent(str6, 2);
	if (strcmp(indented, "\n\n\n\n\n\n\n\n") != 0)
		return 6;
	free(indented);

	return 0;
}

int test_oscap_remove_excess_whitespace()
{
	char *result;

	char *str1 = "  hello  \n  world  \n";
	result = oscap_remove_excess_whitespace(str1);
	if (strcmp(result, "hello\nworld\n") != 0)
		return 1;
	free(result);

	char *str2 = "  hello    world  ";
	result = oscap_remove_excess_whitespace(str2);
	if (strcmp(result, "hello    world") != 0)
		return 2;
	free(result);

	char *str3 = "  \n  \n  \n";
	result = oscap_remove_excess_whitespace(str3);
	if (strcmp(result, "") != 0)
		return 3;
	free(result);

	char *str4 = "";
	result = oscap_remove_excess_whitespace(str4);
	if (strcmp(result, "") != 0)
		return 4;
	free(result);

	return 0;
}

int main (int argc, char *argv[])
{
	int retval = 0;

	if ((retval = test_oscap_path_startswith()) != 0)
		return retval;
	if ((retval = test_oscap_strrm()) != 0)
		return retval;
	if ((retval = test_oscap_indent()) != 0)
		return retval;
	if ((retval = test_oscap_remove_excess_whitespace()) != 0)
		return retval;

	return retval;
}
