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
#ifndef OS_WINDOWS
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

int test_oscap_path_startswith(void);
int test_oscap_strrm(void);
int test_oscap_indent(void);
int test_oscap_remove_excess_whitespace(void);
#ifndef OS_WINDOWS
int test_oscap_path_in_root(void);
#endif

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

#ifndef OS_WINDOWS
/* The root of test_oscap_path_in_root, and its links. */
static const char *const in_root_dirs[] = {"etc", "etc/authselect", "etc/pam.d"};
static const char *const in_root_file = "etc/authselect/system-auth";
static const char *const in_root_links[][2] = {
	{"etc/pam.d/system-auth", "/etc/authselect/system-auth"},
	{"etc/pam.d/missing-auth", "/missing/../etc/authselect/system-auth"},
	{"etc/dirlink", "/etc/authselect"},
};

static const struct {
	const char *path;
	bool follow;
	const char *want;	/* result under the root, or NULL */
	int err;		/* errno when want is NULL */
} in_root_tests[] = {
	{"/etc/pam.d/system-auth", true, "/etc/authselect/system-auth", 0},
	{"/../etc/./pam.d/../authselect", true, "/etc/authselect", 0},
	{"/etc/pam.d/missing-auth", true, NULL, ENOENT},
	{"/missing/../etc/authselect", true, NULL, ENOENT},
	{"/etc/authselect/system-auth/../system-auth", true, NULL, ENOTDIR},
	{"/etc/authselect/system-auth/.", true, NULL, ENOTDIR},
	{"/etc/dirlink", false, "/etc/dirlink", 0},
	{"/etc/dirlink/", false, "/etc/authselect/", 0},
	{"/etc/pam.d/system-auth/", false, "/etc/authselect/system-auth/", 0},
};

int test_oscap_path_in_root()
{
	const size_t ndirs = sizeof(in_root_dirs) / sizeof(in_root_dirs[0]);
	const size_t nlinks = sizeof(in_root_links) / sizeof(in_root_links[0]);
	const size_t ntests = sizeof(in_root_tests) / sizeof(in_root_tests[0]);
	char root[] = "path_in_root.XXXXXX", path[PATH_MAX], want[PATH_MAX];
	struct stat st;
	char *got;
	size_t i;
	int ret = 0;
	FILE *f;

	if (mkdtemp(root) == NULL)
		return 1;
	for (i = 0; ret == 0 && i < ndirs; i++) {
		snprintf(path, sizeof(path), "%s/%s", root, in_root_dirs[i]);
		if (mkdir(path, 0700) == -1)
			ret = 2;
	}
	snprintf(path, sizeof(path), "%s/%s", root, in_root_file);
	if (ret == 0 && ((f = fopen(path, "w")) == NULL || fclose(f) != 0))
		ret = 3;
	for (i = 0; ret == 0 && i < nlinks; i++) {
		snprintf(path, sizeof(path), "%s/%s", root, in_root_links[i][0]);
		if (symlink(in_root_links[i][1], path) == -1)
			ret = 4;
	}

	for (i = 0; ret == 0 && i < ntests; i++) {
		errno = 0;
		got = oscap_path_in_root(root, in_root_tests[i].path, in_root_tests[i].follow);
		if (in_root_tests[i].want == NULL) {
			if (got != NULL || errno != in_root_tests[i].err) {
				fprintf(stderr, "oscap_path_in_root(%s, %d) = %s, errno %d, want NULL, errno %d\n",
					in_root_tests[i].path, in_root_tests[i].follow, got != NULL ? got : "NULL", errno, in_root_tests[i].err);
				ret = 10 + (int)i;
			}
		} else {
			snprintf(want, sizeof(want), "%s%s", root, in_root_tests[i].want);
			if (got == NULL || strcmp(got, want) != 0) {
				fprintf(stderr, "oscap_path_in_root(%s, %d) = %s, errno %d, want %s\n",
					in_root_tests[i].path, in_root_tests[i].follow, got != NULL ? got : "NULL", errno, want);
				ret = 10 + (int)i;
			}
		}
		free(got);
	}

	/* A stat of a file with a trailing slash fails, as outside a root. */
	got = oscap_path_in_root(root, "/etc/pam.d/system-auth/", false);
	if (ret == 0 && (got == NULL || stat(got, &st) != -1 || errno != ENOTDIR))
		ret = 5;
	free(got);

	for (i = nlinks; i > 0; i--) {
		snprintf(path, sizeof(path), "%s/%s", root, in_root_links[i - 1][0]);
		unlink(path);
	}
	snprintf(path, sizeof(path), "%s/%s", root, in_root_file);
	unlink(path);
	for (i = ndirs; i > 0; i--) {
		snprintf(path, sizeof(path), "%s/%s", root, in_root_dirs[i - 1]);
		rmdir(path);
	}
	rmdir(root);
	return ret;
}
#endif

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
#ifndef OS_WINDOWS
	if ((retval = test_oscap_path_in_root()) != 0)
		return retval;
#endif

	return retval;
}
