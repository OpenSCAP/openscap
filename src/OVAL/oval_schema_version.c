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
 *      "Jan Černý" <jcerny@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "common/util.h"
#include "common/debug_priv.h"
#include "public/oval_schema_version.h"

#include <pcre.h>

static int _parse_int(const char *substring, size_t substring_length)
{
	/* Pay attention that substring_length != strlen(substring) */
	char buffer[substring_length + 1]; // +1 for a zero byte
	strncpy(buffer, substring, substring_length);
	buffer[substring_length] = '\0';
	return atoi(buffer);
}

oval_schema_version_t oval_schema_version_from_cstr(const char *ver_str)
{
	oval_schema_version_t version;
	memset(&version, 0, sizeof(oval_schema_version_t));
	if (ver_str == NULL) {
		return version;
	}
	const char *pattern = "([0-9]+)\\.([0-9]+)(?:\\.([0-9]+))?(?::([0-9]+)\\.([0-9]+)(?:\\.([0-9]+))?)?";
	const char *error;
	int erroffset;
	pcre *re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
	if (re == NULL) {
		dE("Regular expression compilation failed with %s", pattern);
		return version;
	}
	const int ovector_size = 30; // must be a multiple of 30
	int ovector[ovector_size];
	int rc = pcre_exec(re, NULL, ver_str, strlen(ver_str), 0, 0, ovector, ovector_size);
	pcre_free(re);
	if (rc < 0) {
		dE("Regular expression %s did not match string %s", pattern, ver_str);
		return version;
	}
	for (int i = 1; i < rc; i++) {
		const char *substring = ver_str + ovector[2 * i];
		size_t substring_length = ovector[2 * i + 1] - ovector[2 * i];
		version.component[i - 1] = _parse_int(substring, substring_length);
	}
	return version;
}

const char *oval_schema_version_to_cstr(oval_schema_version_t version)
{
	size_t buf_len = 32;
	char *buf = oscap_alloc(buf_len);
	const char *format;
	if (version.component[OVAL_SCHEMA_VERSION_PLATFORM_MAJOR]) {
		if (version.component[OVAL_SCHEMA_VERSION_CORE_UPDATE] &&
		version.component[OVAL_SCHEMA_VERSION_PLATFORM_UPDATE]) {
			format = "%1$d.%2$d.%3$d:%4$d.%5$d.%6$d";
		} else if (version.component[OVAL_SCHEMA_VERSION_CORE_UPDATE]) {
			format = "%1$d.%2$d.%3$d:%4$d.%5$d";
		} else if (version.component[OVAL_SCHEMA_VERSION_PLATFORM_UPDATE]) {
			format = "%1$d.%2$d:%4$d.%5$d.%6$d";
		} else {
			format = "%1$d.%2$d:%4$d.%5$d";
		}
	} else if (version.component[OVAL_SCHEMA_VERSION_CORE_UPDATE]) {
		format = "%1$d.%2$d.%3$d";
	} else {
		format = "%1$d.%2$d";
	}
	snprintf(buf, buf_len, format,
		version.component[OVAL_SCHEMA_VERSION_CORE_MAJOR],
		version.component[OVAL_SCHEMA_VERSION_CORE_MINOR],
		version.component[OVAL_SCHEMA_VERSION_CORE_UPDATE],
		version.component[OVAL_SCHEMA_VERSION_PLATFORM_MAJOR],
		version.component[OVAL_SCHEMA_VERSION_PLATFORM_MINOR],
		version.component[OVAL_SCHEMA_VERSION_PLATFORM_UPDATE]);
	return buf;
}

int oval_schema_version_cmp(oval_schema_version_t v1, oval_schema_version_t v2)
{
	for (int i = 0; i < OVAL_SCHEMA_VERSION_COMPONENTS_COUNT; i++) {
		if (v1.component[i] < v2.component[i]) {
			return -1;
		} else if (v1.component[i] > v2.component[i]) {
			return 1;
		}
	}
	return 0;
}
