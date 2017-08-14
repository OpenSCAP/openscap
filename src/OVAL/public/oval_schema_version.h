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

#ifndef OVAL_SCHEMA_VERSION_H
#define OVAL_SCHEMA_VERSION_H

enum oval_schema_version_components {
	OVAL_SCHEMA_VERSION_CORE_MAJOR = 0,
	OVAL_SCHEMA_VERSION_CORE_MINOR,
	OVAL_SCHEMA_VERSION_CORE_UPDATE,
	OVAL_SCHEMA_VERSION_PLATFORM_MAJOR,
	OVAL_SCHEMA_VERSION_PLATFORM_MINOR,
	OVAL_SCHEMA_VERSION_PLATFORM_UPDATE,
	OVAL_SCHEMA_VERSION_COMPONENTS_COUNT
};

#define OVAL_SCHEMA_VERSION_INVALID oval_schema_version_from_cstr(NULL)
#define OVAL_SCHEMA_VERSION(v) oval_schema_version_from_cstr(#v)

typedef struct {
	int component[OVAL_SCHEMA_VERSION_COMPONENTS_COUNT];
} oval_schema_version_t;

/**
 * Parses an OVAL schema version
 * @param ver_str OVAL version as string
 * @return internal representation of OVAL schema version
 */
oval_schema_version_t oval_schema_version_from_cstr(const char *ver_str);

/**
 * Converts OVAL schema version from an internal representation to a string.
 * @param version OVAL schema version
 * @return string that needs to be free
 *
 * This function should have been declared to return non-const char*.
 * You need to free the result despite it being declared as const char*.
 */
const char *oval_schema_version_to_cstr(oval_schema_version_t version);

/**
 * Compare two versions in the internal representation
 * Returns
 *   0 ... if the versions are the same
 *  <0 ... if 'v1' is older than 'v2'
 *  >0 ... if 'v1' is newer than 'v2'
 */
int oval_schema_version_cmp(oval_schema_version_t v1, oval_schema_version_t v2);

#endif
