/*
 * Copyright 2022 Red Hat Inc., Durham, North Carolina.
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
 *      "Jan Černý" <jcerny@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "memusage.h"
#include "memusage.c"
#define OS_LINUX

static int test_basic()
{
	size_t size;
	char *strval = strdup("17 MB");
	read_common_sizet(&size, strval);
	free(strval);
	return (size == 17);
}

static int test_errno()
{
	size_t size;
	char *strval = strdup("17 MB");

	/* Test that setting errno outside of the read_common_sizet function
	 * doesn't influence the function and doesn't make the function fail.
	 */
	errno = EINVAL;

	int ret = read_common_sizet(&size, strval);
	free(strval);
	return (ret != -1);
}

int main(int argc, char *argv[])
{
	if (!test_basic()) {
		fprintf(stderr, "test_basic has failed\n");
		return 1;
	}
	if (!test_errno()) {
		fprintf(stderr, "test_errno has failed\n");
		return 1;
	}
	return 0;
}
