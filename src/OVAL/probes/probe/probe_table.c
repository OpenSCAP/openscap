/*
 * Copyright 2018 Red Hat Inc., Durham, North Carolina.
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

#include "probe_table.h"
#include "independent/system_info.h"
#include "independent/family.h"
#include "oval_types.h"
#include "independent/textfilecontent54.h"
#include "independent/environmentvariable.h"
#include "independent/variable.h"
#include "independent/environmentvariable58.h"
#include "independent/filehash.h"
#include "independent/filehash58.h"
#include "independent/sql.h"

typedef struct probe_table_entry {
	oval_subtype_t type;
	probe_init_function_t probe_init_function;
	probe_main_function_t probe_main_function;
	probe_fini_function_t probe_fini_function;
	probe_offline_mode_function_t probe_offline_mode_function;
} probe_table_entry_t;

static const probe_table_entry_t probe_table[] = {
	/* {type, init, main, fini, offline} */
	{OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE, NULL, environmentvariable_probe_main, NULL, NULL},
	{OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE58, NULL, environmentvariable58_probe_main, NULL, NULL},
	{OVAL_INDEPENDENT_FAMILY, family_probe_init, family_probe_main, NULL, NULL},
	{OVAL_INDEPENDENT_FILE_HASH, filehash_probe_init, filehash_probe_main, filehash_probe_fini, NULL},
	{OVAL_INDEPENDENT_FILE_HASH58, filehash58_probe_init, filehash58_probe_main, filehash58_probe_fini, NULL},
	{OVAL_INDEPENDENT_SQL, NULL, sql_probe_main, NULL, NULL},
	{OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, textfilecontent54_probe_init, textfilecontent54_probe_main, NULL, NULL},
	{OVAL_INDEPENDENT_VARIABLE, variable_probe_init, variable_probe_main, NULL, NULL},
	{OVAL_SUBTYPE_SYSINFO, system_info_probe_init, system_info_probe_main, NULL, NULL},
	{OVAL_SUBTYPE_UNKNOWN, NULL, NULL, NULL, NULL}
};

static probe_table_entry_t *probe_table_get(oval_subtype_t type)
{
	probe_table_entry_t *entry = probe_table;
	while (entry->probe_main_function != NULL && entry->type != type)
	{
		entry++;
	}
	return entry;
}

probe_init_function_t probe_table_get_init_function(oval_subtype_t type)
{
	probe_table_entry_t *entry = probe_table_get(type);
	return entry->probe_init_function;
}

probe_main_function_t probe_table_get_main_function(oval_subtype_t type)
{
	probe_table_entry_t *entry = probe_table_get(type);
	return entry->probe_main_function;
}

probe_fini_function_t probe_table_get_fini_function(oval_subtype_t type)
{
	probe_table_entry_t *entry = probe_table_get(type);
	return entry->probe_fini_function;
}

probe_offline_mode_function_t probe_table_get_offline_mode_function(oval_subtype_t type)
{
	probe_table_entry_t *entry = probe_table_get(type);
	return entry->probe_offline_mode_function;
}