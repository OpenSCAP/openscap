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



#include "util.h"
#include <string.h>

struct oscap_import_source {
        char * filename;
        char * encoding;
};
OSCAP_GETTER(const char *, oscap_import_source, filename)
OSCAP_GETTER(const char *, oscap_import_source, encoding)

struct oscap_export_target {
        char * filename;
        char * encoding;
        int indent; 
        char * indent_string;
};
OSCAP_GETTER(const char *, oscap_export_target, filename)
OSCAP_GETTER(const char *, oscap_export_target, encoding)
OSCAP_GETTER(int, oscap_export_target, indent)
OSCAP_GETTER(const char *, oscap_export_target, indent_string)

struct oscap_import_source * oscap_import_source_new(const char * filename, const char * encoding) {

        struct oscap_import_source *target = oscap_calloc(1, sizeof(struct oscap_import_source));

        if (filename == NULL)
                return NULL;
        if (encoding == NULL)
             target->encoding = strdup("UTF-8");
        else target->encoding = strdup(encoding);
                
        target->filename = strdup(filename);

        return target;
}

void oscap_import_source_free(struct oscap_import_source * target) {
        
        if (target == NULL) return;

        if (target->filename != NULL) oscap_free(target->filename);
        if (target->encoding != NULL) oscap_free(target->encoding);
        oscap_free(target);
}

struct oscap_export_target * oscap_export_target_new(const char * filename, const char * encoding) {

        struct oscap_export_target *target = oscap_calloc(1, sizeof(struct oscap_export_target));

        if (filename == NULL)
                return NULL;
        if (encoding == NULL)
             target->encoding = strdup("UTF-8");
        else target->encoding = strdup(encoding);
                
        target->filename = strdup(filename);
        // default values
        target->indent = 0;
        target->indent_string = strdup("    ");

        return target;
}

void oscap_export_target_free(struct oscap_export_target * target) {
        
        if (target == NULL) return;

        if (target->filename != NULL) oscap_free(target->filename);
        if (target->encoding != NULL) oscap_free(target->encoding);
        if (target->indent_string != NULL) oscap_free(target->indent_string);
        oscap_free(target);
}


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

