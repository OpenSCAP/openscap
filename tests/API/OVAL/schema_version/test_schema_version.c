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
#include "OVAL/public/oval_schema_version.h"

static const char *cmp_to_cstr(int cmp)
{
	if (cmp == 0) {
		return "is same as";
	} else if (cmp < 0) {
		return "is older than";
	}
        return "is newer than";
}

int main(int argc, char **argv)
{
	if (argc != 4) {
		printf("Not enough arguments\n");
		return 1;
	}
	const char *v1_str = argv[1];
	const char *v2_str = argv[2];
	int expected_result = atoi(argv[3]);
	oval_schema_version_t v1 = oval_schema_version_from_cstr(v1_str);
	oval_schema_version_t v2 = oval_schema_version_from_cstr(v2_str);
	int cmp = oval_schema_version_cmp(v1, v2);
	int retval;
	if (cmp == expected_result) {
		retval = 0;
		printf("\tPASS\t%s %s %s\n", v1_str, cmp_to_cstr(cmp), v2_str);
	} else {
		retval = 1;
		printf("\tFAIL\t%s %s %s but expected result is '%s'\n", v1_str,
			cmp_to_cstr(cmp), v2_str, cmp_to_cstr(expected_result));
	}
	return retval;
}
