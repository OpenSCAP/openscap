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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      Maros Barabas <mbarabas@redhat.com>
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <math.h>

#include "util.h"
#include "_error.h"
#include "oscap.h"

#ifdef _WIN32
#include <stdlib.h>
#else
#include <libgen.h>
#include <strings.h>
#endif


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
#ifdef _WIN32
	return _fullpath(resolved_path, path, PATH_MAX);
#else
	return realpath(path, resolved_path);
#endif
}

char *oscap_basename(char *path)
{
#ifdef _WIN32
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

char *oscap_dirname(char *path)
{
#ifdef _WIN32
	char dirpath[_MAX_DIR];
	_splitpath_s(path, NULL, 0, dirpath, _MAX_DIR, NULL, 0, NULL, 0);
#else
	char *dirpath = dirname(path);
#endif
	return oscap_strdup(dirpath);
}

int oscap_strcasecmp(const char *s1, const char *s2)
{
#ifdef _WIN32
	return _stricmp(s1, s2);
#else
	return strcasecmp(s1, s2);
#endif
}

int oscap_strncasecmp(const char *s1, const char *s2, size_t n)
{
#ifdef _WIN32
	return _strnicmp(s1, s2, n);
#else
	return strncasecmp(s1, s2, n);
#endif
}
