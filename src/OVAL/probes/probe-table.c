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

#include "probe-table.h"
#include "independent/system_info_probe.h"
#include "independent/family_probe.h"
#include "oval_types.h"
#include "independent/textfilecontent54_probe.h"
#include "independent/textfilecontent_probe.h"
#include "independent/environmentvariable_probe.h"
#include "independent/variable_probe.h"
#include "independent/environmentvariable58_probe.h"
#include "independent/filehash_probe.h"
#include "independent/filehash58_probe.h"
#include "independent/sql_probe.h"
#include "independent/sql57_probe.h"
#include "independent/xmlfilecontent_probe.h"
#include "unix/dnscache_probe.h"
#include "unix/file_probe.h"
#include "unix/fileextendedattribute_probe.h"
#include "unix/gconf_probe.h"
#include "unix/interface_probe.h"
#include "unix/password_probe.h"
#include "unix/process_probe.h"
#include "unix/process58_probe.h"
#include "unix/routingtable_probe.h"
#include "unix/runlevel_probe.h"
#include "unix/shadow_probe.h"
#include "unix/symlink_probe.h"
#include "unix/sysctl_probe.h"
#include "unix/uname_probe.h"
#include "unix/xinetd_probe.h"

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
	{OVAL_INDEPENDENT_SQL57, NULL, sql57_probe_main, NULL, NULL},
	{OVAL_INDEPENDENT_TEXT_FILE_CONTENT, textfilecontent_probe_init, textfilecontent_probe_main, NULL, NULL},
	{OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, textfilecontent54_probe_init, textfilecontent54_probe_main, NULL, NULL},
	{OVAL_INDEPENDENT_VARIABLE, variable_probe_init, variable_probe_main, NULL, NULL},
	{OVAL_INDEPENDENT_XML_FILE_CONTENT, xmlfilecontent_probe_init, xmlfilecontent_probe_main, xmlfilecontent_probe_fini, NULL},
	{OVAL_UNIX_DNSCACHE, NULL, dnscache_probe_main, NULL, NULL},
	{OVAL_UNIX_FILE, file_probe_init, file_probe_main, file_probe_fini, NULL},
	{OVAL_UNIX_FILEEXTENDEDATTRIBUTE, fileextendedattribute_probe_init, fileextendedattribute_probe_main, fileextendedattribute_probe_fini, NULL},
	{OVAL_UNIX_GCONF, NULL, gconf_probe_main, NULL, NULL},
	{OVAL_UNIX_INTERFACE, NULL, interface_probe_main, NULL, NULL},
	{OVAL_UNIX_PASSWORD, password_probe_init, password_probe_main, NULL, NULL},
	{OVAL_UNIX_PROCESS, NULL, process_probe_main, NULL, NULL},
	{OVAL_UNIX_PROCESS58, NULL, process58_probe_main, NULL, NULL},
	{OVAL_UNIX_ROUTINGTABLE, NULL, routingtable_probe_main, NULL, NULL},
	{OVAL_UNIX_RUNLEVEL, runlevel_probe_init, runlevel_probe_main, NULL, NULL},
	{OVAL_UNIX_SHADOW, shadow_probe_init, shadow_probe_main, NULL, NULL},
	{OVAL_UNIX_SYMLINK, symlink_probe_init, symlink_probe_main, NULL, NULL},
	{OVAL_UNIX_SYSCTL, NULL, sysctl_probe_main, NULL, NULL},
	{OVAL_UNIX_UNAME, NULL, uname_probe_main, NULL, NULL},
	{OVAL_UNIX_XINETD, xinetd_probe_init, xinetd_probe_main, xinetd_probe_fini, NULL},
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

void probe_table_list(FILE *output)
{
	const probe_table_entry_t *entry = probe_table;
	while (entry->type != OVAL_SUBTYPE_UNKNOWN)
	{
		oval_subtype_t type = entry->type;
		fprintf(output, "%-14s", oval_family_get_text(oval_subtype_get_family(type)));
		fprintf(output, "%-29s", oval_subtype_get_text(type));
		fprintf(output, "probe_%s", oval_subtype_get_text(type));
		fprintf(output, "\n");
		entry++;
	}
}

int probe_table_size()
{
	int size = 0;
	const probe_table_entry_t *entry = probe_table;
	while (entry->type != OVAL_SUBTYPE_UNKNOWN) {
		entry++;
		size++;
	}
	return size;
}

bool probe_table_exists(oval_subtype_t type)
{
	probe_table_entry_t *entry = probe_table_get(type);
	return (entry->type != OVAL_SUBTYPE_UNKNOWN);
}
