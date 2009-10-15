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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */



#include "util.h"
#include <string.h>

int oscap_string_to_enum(const struct oscap_string_map* map, const char* str)
{
	while (map->string && (str == NULL || strcmp(map->string, str) != 0)) ++map;
	return map->value;
}

const char* oscap_enum_to_string(const struct oscap_string_map* map, int val)
{
	while (map->string && map->value != val) ++map;
	return map->string;
}

char* oscap_strdup(const char *str) {

        if (str == NULL)
            return NULL;

        return strdup(str);
}

