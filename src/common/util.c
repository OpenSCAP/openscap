/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      Maros Barabas <mbarabas@redhat.com>
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <math.h>
#include <sys/stat.h>

#include "util.h"
#include "_error.h"
#include "oscap.h"
#include "oscap_helpers.h"
#include "oscap_pcre.h"
#include "debug_priv.h"

#ifdef OS_WINDOWS
#include <stdlib.h>
#include <windows.h>
#else
#include <libgen.h>
#include <strings.h>
#endif

#define PATH_SEPARATOR '/'

char *oscap_generate_random_string(size_t len, char *charset)
{
	char default_charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *res = NULL;
	charset = (charset != NULL && strlen(charset) > 0) ? charset : default_charset;
	size_t charset_len = strlen(charset);
	if (len > 0) {
		srand(time(NULL));
		res = malloc(len+1);
		res[len] = 0;
		while (len-- > 0) {
			size_t index = (double) rand() / RAND_MAX * (charset_len-1);
			res[len] = charset[index];
		}
	}
	return res;
}

int oscap_string_to_enum(const struct oscap_string_map *map, const char *str)
{
	__attribute__nonnull__(map);

	while (map->string && (str == NULL || strcmp(map->string, str) != 0))
		++map;
	return map->value;
}

const char *oscap_enum_to_string(const struct oscap_string_map *map, int val)
{
	__attribute__nonnull__(map);

	while (map->string && map->value != val)
		++map;
	return map->string;
}

static const size_t CPE_SPLIT_INIT_ALLOC = 8;

char **oscap_split(char *str, const char *delim)
{

	__attribute__nonnull__(str);

	char **stringp = &str;
	int alloc = CPE_SPLIT_INIT_ALLOC;
	char **fields = malloc(alloc * sizeof(char *));
	if (!fields)
		return NULL;

	int i = 0;
	while (*stringp) {
		if (i + 2 > alloc) {
			void *old = fields;
			alloc *= 2;
			fields = realloc(fields, alloc * sizeof(char *));
			if (fields == NULL) {
				free(old);
				return NULL;
			}
		}
		fields[i++] = strsep(stringp, delim);
	}
	fields[i] = NULL;

	return fields;
}

char *oscap_trim(char *str)
{
	int off, i = 0;

        if (str == NULL)
                return (NULL);

        /* left trim */
        while (isspace(str[i])) ++i;

	off = i;

        /* move */
	while (1) {
		str[i - off] = str[i];
                if (str[i] == '\0')
                        break;
                ++i;
	}

        i -= off;

        /* right trim */
        while (i > 0) {
                if (!isspace(str[--i])) {
                        str[i + 1] = '\0';
                        break;
                }
        }

	return str;
}

char *oscap_vsprintf(const char *fmt, va_list ap)
{
    if (fmt == NULL) return NULL;

    char *ret = NULL;
    va_list args;
    va_copy(args, ap);

    char foo[2];
    int length = vsnprintf(foo, 1, fmt, ap);
    if (length < 0) goto cleanup;

    ret = malloc(sizeof(char) * (length + 1));
    vsprintf(ret, fmt, args);
    assert(ret[length] == '\0');

cleanup:
    va_end(args);
    return ret;
}

char *oscap_sprintf(const char *fmt, ...)
{
    char *ret = NULL;
    va_list ap;
    va_start(ap, fmt);
    ret = oscap_vsprintf(fmt, ap);
    va_end(ap);
    return ret;
}

char *oscap_rtrim(char *str, char ch)
{
	if (str == NULL) return NULL;
	for (char *pos = str + strlen(str) - 1; pos >= str; --pos) {
		if (*pos == ch) *pos = '\0';
		else break;
	}
	return str;
}

void oscap_strtoupper(char *str)
{
    if (!str) return;
    for (; *str; ++str) *str = toupper(*str);
}

char *oscap_expand_ipv6(const char *input)
{
	// we have to do a double pass because we need to know the number of
	// specified components, :: will fill up to a total of 8 if encountered

	const char* input_it = input;
	unsigned short component_count = 1; // fencepost, we start with 1
	while (*input_it) {
		if (*input_it == ':') {
			// don't count ::1 as 2 components
			if (input_it != input)
				component_count++;

			// the next character is either a part of a component or another ':'
			// in any case we don't want to count it
			input_it++;
		}

		input_it++;
	}

	// IPv6 is at most eight 4-tuples of [0-9a-f] with 7 separators, plus \0
	char* ret = malloc(8 * 4 * sizeof(char) + 7 + 1);
	char* output_it = ret;

	input_it = input;
	while (*input_it) {
		// signifies that we have finished writing a component this iteration
		bool closed_component = false;

		if (*input_it == ':') {
			const char next = *(input_it + 1 * sizeof(char));
			if (next == ':') {
				// skip one extra char to skip over the whole ::
				input_it++;

				for (unsigned short i = component_count; i < 8; ++i) {
					// don't add leading separators
					if (output_it != ret)
						*output_it++ = ':';

					*output_it++ = '0';
					closed_component = true;
				}
			}
			else {
				closed_component = true;
			}
		}
		else {
			*output_it++ = *input_it;
		}

		input_it++;

		// if this was not the last char of the input add a separator
		if (closed_component && *input_it)
			*output_it++ = ':';
	}

	*output_it = '\0';

	return ret;
}

char *oscap_realpath(const char *path, char *resolved_path)
{
#ifdef OS_WINDOWS
	return _fullpath(resolved_path, path, PATH_MAX);
#else
	return realpath(path, resolved_path);
#endif
}

char *oscap_basename(char *path)
{
#ifdef OS_WINDOWS
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(path, NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT);
	size_t base_len = strlen(fname) + strlen(ext) + 1;
	char *base = malloc(base_len);
	strncpy(base, fname, base_len);
	strncat(base, ext, base_len - strlen(base) - 1);
	return base;
#else
	char *base = basename(path);
	return oscap_strdup(base);
#endif
}

#ifdef OS_WINDOWS
char *oscap_dirname(char *path)
{
	if (path == NULL || *path == '\0' || (strchr(path, '/') == NULL && strchr(path, '\\') == NULL)) {
		return strdup(".");
	}
	char dir[_MAX_DIR];
	char drive[_MAX_DRIVE];
	char dirname[_MAX_PATH];
	_splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);
	_makepath_s(dirname, _MAX_PATH, drive, dir, NULL, NULL);
	oscap_rtrim(dirname, '/');
	oscap_rtrim(dirname, '\\');
	return oscap_strdup(dirname);
}
#else
char *oscap_dirname(char *path)
{
	char *dirpath = dirname(path);
	return oscap_strdup(dirpath);
}
#endif

int oscap_strcasecmp(const char *s1, const char *s2)
{
#ifdef OS_WINDOWS
	return _stricmp(s1, s2);
#else
	return strcasecmp(s1, s2);
#endif
}

int oscap_strncasecmp(const char *s1, const char *s2, size_t n)
{
#ifdef OS_WINDOWS
	return _strnicmp(s1, s2, n);
#else
	return strncasecmp(s1, s2, n);
#endif
}

char *oscap_strtok_r(char *str, const char *delim, char **saveptr)
{
#ifdef OS_WINDOWS
	return strtok_s(str, delim, saveptr);
#else
	return strtok_r(str, delim, saveptr);
#endif
}

char *oscap_strerror_r(int errnum, char *buf, size_t buflen)
{
#ifdef OS_WINDOWS
	strerror_s(buf, buflen, errnum);
	return buf;
#elif defined(OS_APPLE) || defined(OS_FREEBSD)
	strerror_r(errnum, buf, buflen);
	return buf;
#else
	return strerror_r(errnum, buf, buflen);
#endif
}

char *oscap_path_join(const char *path1, const char *path2)
{
	if (path1 == NULL) {
		return oscap_strdup(path2);
	}
	if (path2 == NULL) {
		return oscap_strdup(path1);
	}
	size_t path1_len = strlen(path1);
	size_t path2_len = strlen(path2);
	size_t path2_shift = 0;
	while (path1_len >= 1 && path1[path1_len - 1] == PATH_SEPARATOR) {
		path1_len--;
	}
	while (path2_shift < path2_len && path2[path2_shift] == PATH_SEPARATOR) {
		path2_shift++;
	}
	path2_len -= path2_shift;
	const size_t joined_path_len = path1_len + 1 + path2_len;
	char *joined_path = malloc(joined_path_len + 1);
	strncpy(joined_path, path1, path1_len);
	joined_path[path1_len++] = PATH_SEPARATOR;
	strncpy(joined_path + path1_len, path2 + path2_shift, path2_len);
	joined_path[joined_path_len] = '\0';
	return joined_path;
}

#ifndef OS_WINDOWS
FILE *oscap_fopen_with_prefix(const char *prefix, const char *path)
{
	FILE *fp;
	if (prefix != NULL) {
		char *path_with_prefix = oscap_sprintf("%s%s", prefix, path);
		fp = fopen(path_with_prefix, "r");
		free(path_with_prefix);
	} else {
		fp = fopen(path, "r");
	}
	return fp;
}
#endif

#ifdef OS_WINDOWS
char *oscap_windows_wstr_to_str(const wchar_t *wstr)
{
	if (wstr == NULL) {
		return NULL;
	}
	const int required_size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char *str = malloc(required_size);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, required_size, NULL, NULL);
	return str;
}

wchar_t *oscap_windows_str_to_wstr(const char *str)
{
	if (str == NULL) {
		return NULL;
	}
	const int required_size = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	wchar_t *wstr = malloc(required_size * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, required_size);
	return wstr;
}

char *oscap_windows_error_message(unsigned long error_code)
{
	wchar_t *buffer = NULL;
	/* According to FormatMessage documentation, if FORMAT_MESSAGE_ALLOCATE_BUFFER
	 * flag is set, the buffer must be freed by LocalFree and must be casted to LPWSTR.
	 */
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR) &buffer, 0, NULL);
	char *error_message = oscap_windows_wstr_to_str(buffer);
	LocalFree(buffer);
	return error_message;
}
#endif

int oscap_open_writable(const char *filename)
{
#ifdef OS_WINDOWS
	int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
#else
	int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC,
			S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
#endif
	if (fd == -1) {
		if (errno == EACCES) {
			/* File already exists and we aren't allowed to create a new one
			with the same name */
#ifdef OS_WINDOWS
			fd = open(filename, O_WRONLY|O_TRUNC, S_IREAD|S_IWRITE);
#else
			fd = open(filename, O_WRONLY|O_TRUNC,
					S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
#endif
		}
		if (fd == -1) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP,
					"Could not open output file '%s': %s",
					filename, strerror(errno));
			return -1;
		}
	}
	return fd;
}

bool oscap_path_startswith(const char *path, const char *prefix)
{
	bool res = true;
	const char *del = "/";
	char *path_dup = oscap_strdup(path);
	char **path_split = oscap_split(path_dup, del);
	char *prefix_dup = oscap_strdup(prefix);
	char **prefix_split = oscap_split(prefix_dup, del);
	int i = 0, j = 0;
	while (prefix_split[i] && path_split[j]) {
		if (!strcmp(prefix_split[i], "")) {
			++i;
			continue;
		}
		if (!strcmp(path_split[j], "")) {
			++j;
			continue;
		}
		if (strcmp(prefix_split[i], path_split[j])) {
			res = false;
			break;
		}
		++i;
		++j;
	}
	free(path_dup);
	free(path_split);
	free(prefix_dup);
	free(prefix_split);
	return res;
}

char *oscap_concat(char *str1, char *str2)
{
	if (str1 == NULL || str2 == NULL)
		return str1;
	size_t str1_len = strlen(str1);
	size_t str2_len = strlen(str2);
	str1 = realloc(str1, str1_len + str2_len + 1);
	strncat(str1, str2, str2_len);
	return str1;
}
