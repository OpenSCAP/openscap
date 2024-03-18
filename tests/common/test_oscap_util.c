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

int test_oscap_path_startswith(void);
int test_oscap_strrm(void);

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

int main (int argc, char *argv[])
{
	int retval = 0;

	if ((retval = test_oscap_path_startswith()) != 0)
		return retval;
	if ((retval = test_oscap_strrm()) != 0)
		return retval;

	return retval;
}
