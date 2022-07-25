// SPDX-License-Identifier: LGPL-2.1-or-later

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
	char *strval = strdup("17 kB\n");
	int ret = read_common_sizet(&size, strval);
	free(strval);
	return (size == 17 && ret == 0);
}

static int test_no_unit()
{
	size_t size;
	char *strval = strdup("42");
	int ret = read_common_sizet(&size, strval);
	free(strval);
	return (ret == -1);
}

static int test_invalid_number()
{
	size_t size;
	char *strval = strdup("www kB\n");
	int ret = read_common_sizet(&size, strval);
	free(strval);
	return (size == 0 && ret == 0);
}

static int test_errno()
{
	size_t size;
	char *strval = strdup("17 kB\n");

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
	if (!test_no_unit()) {
		fprintf(stderr, "test_no_unit has failed\n");
		return 1;
	}
	if (!test_invalid_number()) {
		fprintf(stderr, "test_invalid_number has failed\n");
		return 1;
	}
	if (!test_errno()) {
		fprintf(stderr, "test_errno has failed\n");
		return 1;
	}
	return 0;
}
