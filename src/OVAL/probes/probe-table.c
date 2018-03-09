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
#include "oval_types.h"

#ifdef INDEPENDENT_ENVIRONMENTVARIABLE_PROBE
#include "independent/environmentvariable_probe.h"
#endif

#ifdef INDEPENDENT_ENVIRONMENTVARIABLE58_PROBE
#include "independent/environmentvariable58_probe.h"
#endif

#ifdef INDEPENDENT_FAMILY_PROBE
#include "independent/family_probe.h"
#endif

#ifdef INDEPENDENT_FILEHASH_PROBE
#include "independent/filehash_probe.h"
#endif

#ifdef INDEPENDENT_FILEHASH58_PROBE
#include "independent/filehash58_probe.h"
#endif

#ifdef INDEPENDENT_SQL_PROBE
#include "independent/sql_probe.h"
#endif

#ifdef INDEPENDENT_SQL57_PROBE
#include "independent/sql57_probe.h"
#endif

#ifdef INDEPENDENT_SYSTEM_INFO_PROBE
#include "independent/system_info_probe.h"
#endif

#ifdef INDEPENDENT_TEXTFILECONTENT_PROBE
#include "independent/textfilecontent_probe.h"
#endif

#ifdef INDEPENDENT_TEXTFILECONTENT54_PROBE
#include "independent/textfilecontent54_probe.h"
#endif

#ifdef INDEPENDENT_VARIABLE_PROBE
#include "independent/variable_probe.h"
#endif

#ifdef INDEPENDENT_XMLFILECONTENT_PROBE
#include "independent/xmlfilecontent_probe.h"
#endif

#ifdef LINUX_DPKGINFO_PROBE
#include "unix/linux/dpkginfo_probe.h"
#endif

#ifdef LINUX_IFLISTENERS_PROBE
#include "unix/linux/iflisteners_probe.h"
#endif

#ifdef LINUX_INETLISTENINGSERVERS_PROBE
#include "unix/linux/inetlisteningservers_probe.h"
#endif

#ifdef LINUX_PARTITION_PROBE
#include "unix/linux/partition_probe.h"
#endif

#ifdef LINUX_RPMINFO_PROBE
#include "unix/linux/rpminfo_probe.h"
#endif

#ifdef LINUX_RPMVERIFY_PROBE
#include "unix/linux/rpmverify_probe.h"
#endif

#ifdef LINUX_RPMVERIFYFILE_PROBE
#include "unix/linux/rpmverifyfile_probe.h"
#endif

#ifdef LINUX_RPMVERIFYPACKAGE_PROBE
#include "unix/linux/rpmverifypackage_probe.h"
#endif

#ifdef LINUX_SELINUXBOOLEAN_PROBE
#include "unix/linux/selinuxboolean_probe.h"
#endif

#ifdef LINUX_SELINUXSECURITYCONTEXT_PROBE
#include "unix/linux/selinuxsecuritycontext_probe.h"
#endif

#ifdef LINUX_SYSTEMDUNITDEPENDENCY_PROBE
#include "unix/linux/systemdunitdependency_probe.h"
#endif

#ifdef LINUX_SYSTEMDUNITPROPERTY_PROBE
#include "unix/linux/systemdunitproperty_probe.h"
#endif

#ifdef UNIX_DNSCACHE_PROBE
#include "unix/dnscache_probe.h"
#endif

#ifdef UNIX_FILE_PROBE
#include "unix/file_probe.h"
#endif

#ifdef UNIX_FILEEXTENDEDATTRIBUTE_PROBE
#include "unix/fileextendedattribute_probe.h"
#endif

#ifdef UNIX_GCONF_PROBE
#include "unix/gconf_probe.h"
#endif

#ifdef UNIX_INTERFACE_PROBE
#include "unix/interface_probe.h"
#endif

#ifdef UNIX_PASSWORD_PROBE
#include "unix/password_probe.h"
#endif

#ifdef UNIX_PROCESS_PROBE
#include "unix/process_probe.h"
#endif

#ifdef UNIX_PROCESS58_PROBE
#include "unix/process58_probe.h"
#endif

#ifdef UNIX_ROUTINGTABLE_PROBE
#include "unix/routingtable_probe.h"
#endif

#ifdef UNIX_RUNLEVEL_PROBE
#include "unix/runlevel_probe.h"
#endif

#ifdef UNIX_SHADOW_PROBE
#include "unix/shadow_probe.h"
#endif

#ifdef UNIX_SYMLINK_PROBE
#include "unix/symlink_probe.h"
#endif

#ifdef UNIX_SYSCTL_PROBE
#include "unix/sysctl_probe.h"
#endif

#ifdef UNIX_UNAME_PROBE
#include "unix/uname_probe.h"
#endif

#ifdef UNIX_XINETD_PROBE
#include "unix/xinetd_probe.h"
#endif

typedef struct probe_table_entry {
	oval_subtype_t type;
	probe_init_function_t probe_init_function;
	probe_main_function_t probe_main_function;
	probe_fini_function_t probe_fini_function;
	probe_offline_mode_function_t probe_offline_mode_function;
} probe_table_entry_t;

static const probe_table_entry_t probe_table[] = {
	/* {type, init, main, fini, offline} */
#ifdef INDEPENDENT_ENVIRONMENTVARIABLE_PROBE
	{OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE, NULL, environmentvariable_probe_main, NULL, NULL},
#endif
#ifdef INDEPENDENT_ENVIRONMENTVARIABLE58_PROBE
	{OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE58, NULL, environmentvariable58_probe_main, NULL, NULL},
#endif
#ifdef INDEPENDENT_FAMILY_PROBE
	{OVAL_INDEPENDENT_FAMILY, family_probe_init, family_probe_main, NULL, NULL},
#endif
#ifdef INDEPENDENT_FILEHASH_PROBE
	{OVAL_INDEPENDENT_FILE_HASH, filehash_probe_init, filehash_probe_main, filehash_probe_fini, NULL},
#endif
#ifdef INDEPENDENT_FILEHASH58_PROBE
	{OVAL_INDEPENDENT_FILE_HASH58, filehash58_probe_init, filehash58_probe_main, filehash58_probe_fini, NULL},
#endif
#ifdef INDEPENDENT_SQL_PROBE
	{OVAL_INDEPENDENT_SQL, NULL, sql_probe_main, NULL, NULL},
#endif
#ifdef INDEPENDENT_SQL57_PROBE
	{OVAL_INDEPENDENT_SQL57, NULL, sql57_probe_main, NULL, NULL},
#endif
#ifdef INDEPENDENT_SYSTEM_INFO_PROBE
	{OVAL_INDEPENDENT_SYSCHAR_SUBTYPE, system_info_probe_init, system_info_probe_main, NULL, NULL},
#endif
#ifdef INDEPENDENT_TEXTFILECONTENT_PROBE
	{OVAL_INDEPENDENT_TEXT_FILE_CONTENT, textfilecontent_probe_init, textfilecontent_probe_main, NULL, NULL},
#endif
#ifdef INDEPENDENT_TEXTFILECONTENT54_PROBE
	{OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, textfilecontent54_probe_init, textfilecontent54_probe_main, NULL, NULL},
#endif
#ifdef INDEPENDENT_VARIABLE_PROBE
	{OVAL_INDEPENDENT_VARIABLE, variable_probe_init, variable_probe_main, NULL, NULL},
#endif
#ifdef INDEPENDENT_XMLFILECONTENT_PROBE
	{OVAL_INDEPENDENT_XML_FILE_CONTENT, xmlfilecontent_probe_init, xmlfilecontent_probe_main, xmlfilecontent_probe_fini, NULL},
#endif
#ifdef LINUX_DPKGINFO_PROBE
	{OVAL_LINUX_DPKG_INFO, dpkginfo_probe_init, dpkginfo_probe_main, NULL, NULL},
#endif
#ifdef LINUX_IFLISTENERS_PROBE
	{OVAL_LINUX_IFLISTENERS, NULL, iflisteners_probe_main, NULL, NULL},
#endif
#ifdef LINUX_INETLISTENINGSERVERS_PROBE
	{OVAL_LINUX_INET_LISTENING_SERVERS, NULL, inetlisteningservers_probe_main, NULL, NULL},
#endif
#ifdef LINUX_PARTITION_PROBE
	{OVAL_LINUX_PARTITION, partition_probe_init, partition_probe_main, NULL, NULL},
#endif
#ifdef LINUX_RPMINFO_PROBE
	{OVAL_LINUX_RPM_INFO, rpminfo_probe_init, rpminfo_probe_main, rpminfo_probe_fini, rpminfo_probe_offline_mode},
#endif
#ifdef LINUX_RPMVERIFY_PROBE
	{OVAL_LINUX_RPMVERIFY, rpmverify_probe_init, rpmverify_probe_main, rpmverify_probe_fini, rpmverify_probe_offline_mode},
#endif
#ifdef LINUX_RPMVERIFYFILE_PROBE
	{OVAL_LINUX_RPMVERIFYFILE, rpmverifyfile_probe_init, rpmverifyfile_probe_main, rpmverifyfile_probe_fini, rpmverifyfile_probe_offline_mode},
#endif
#ifdef LINUX_RPMVERIFYPACKAGE_PROBE
	{OVAL_LINUX_RPMVERIFYPACKAGE, rpmverifypackage_probe_init, rpmverifypackage_probe_main, rpmverifypackage_probe_fini, rpmverifypackage_probe_offline_mode},
#endif
#ifdef LINUX_SELINUXBOOLEAN_PROBE
	{OVAL_LINUX_SELINUXBOOLEAN, NULL, selinuxboolean_probe_main, NULL, NULL},
#endif
#ifdef LINUX_SELINUXSECURITYCONTEXT_PROBE
	{OVAL_LINUX_SELINUXSECURITYCONTEXT, NULL, selinuxsecuritycontext_probe_main, NULL, NULL},
#endif
#ifdef LINUX_SYSTEMDUNITDEPENDENCY_PROBE
	{OVAL_LINUX_SYSTEMDUNITDEPENDENCY, NULL, systemdunitdependency_probe_main, NULL, NULL},
#endif
#ifdef LINUX_SYSTEMDUNITPROPERTY_PROBE
	{OVAL_LINUX_SYSTEMDUNITPROPERTY, NULL, systemdunitproperty_probe_main, NULL, NULL},
#endif
#ifdef UNIX_DNSCACHE_PROBE
	{OVAL_UNIX_DNSCACHE, NULL, dnscache_probe_main, NULL, NULL},
#endif
#ifdef UNIX_FILE_PROBE
	{OVAL_UNIX_FILE, file_probe_init, file_probe_main, file_probe_fini, NULL},
#endif
#ifdef UNIX_FILEEXTENDEDATTRIBUTE_PROBE
	{OVAL_UNIX_FILEEXTENDEDATTRIBUTE, fileextendedattribute_probe_init, fileextendedattribute_probe_main, fileextendedattribute_probe_fini, NULL},
#endif
#ifdef UNIX_GCONF_PROBE
	{OVAL_UNIX_GCONF, NULL, gconf_probe_main, NULL, NULL},
#endif
#ifdef UNIX_INTERFACE_PROBE
	{OVAL_UNIX_INTERFACE, NULL, interface_probe_main, NULL, NULL},
#endif
#ifdef UNIX_PASSWORD_PROBE
	{OVAL_UNIX_PASSWORD, password_probe_init, password_probe_main, NULL, NULL},
#endif
#ifdef UNIX_PROCESS_PROBE
	{OVAL_UNIX_PROCESS, NULL, process_probe_main, NULL, NULL},
#endif
#ifdef UNIX_PROCESS58_PROBE
	{OVAL_UNIX_PROCESS58, NULL, process58_probe_main, NULL, NULL},
#endif
#ifdef UNIX_ROUTINGTABLE_PROBE
	{OVAL_UNIX_ROUTINGTABLE, NULL, routingtable_probe_main, NULL, NULL},
#endif
#ifdef UNIX_RUNLEVEL_PROBE
	{OVAL_UNIX_RUNLEVEL, runlevel_probe_init, runlevel_probe_main, NULL, NULL},
#endif
#ifdef UNIX_SHADOW_PROBE
	{OVAL_UNIX_SHADOW, shadow_probe_init, shadow_probe_main, NULL, NULL},
#endif
#ifdef UNIX_SYMLINK_PROBE
	{OVAL_UNIX_SYMLINK, symlink_probe_init, symlink_probe_main, NULL, NULL},
#endif
#ifdef UNIX_SYSCTL_PROBE
	{OVAL_UNIX_SYSCTL, NULL, sysctl_probe_main, NULL, NULL},
#endif
#ifdef UNIX_UNAME_PROBE
	{OVAL_UNIX_UNAME, NULL, uname_probe_main, NULL, NULL},
#endif
#ifdef UNIX_XINETD_PROBE
	{OVAL_UNIX_XINETD, xinetd_probe_init, xinetd_probe_main, xinetd_probe_fini, NULL},
#endif
	{OVAL_SUBTYPE_UNKNOWN, NULL, NULL, NULL, NULL}
};

static const probe_table_entry_t *probe_table_get(oval_subtype_t type)
{
	const probe_table_entry_t *entry = probe_table;
	while (entry->probe_main_function != NULL && entry->type != type)
	{
		entry++;
	}
	return entry;
}

probe_init_function_t probe_table_get_init_function(oval_subtype_t type)
{
	const probe_table_entry_t *entry = probe_table_get(type);
	return entry->probe_init_function;
}

probe_main_function_t probe_table_get_main_function(oval_subtype_t type)
{
	const probe_table_entry_t *entry = probe_table_get(type);
	return entry->probe_main_function;
}

probe_fini_function_t probe_table_get_fini_function(oval_subtype_t type)
{
	const probe_table_entry_t *entry = probe_table_get(type);
	return entry->probe_fini_function;
}

probe_offline_mode_function_t probe_table_get_offline_mode_function(oval_subtype_t type)
{
	const probe_table_entry_t *entry = probe_table_get(type);
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
	const probe_table_entry_t *entry = probe_table_get(type);
	return (entry->type != OVAL_SUBTYPE_UNKNOWN);
}

oval_subtype_t probe_table_at_index(int idx)
{
	if (idx < 0 || idx >= probe_table_size()) {
		return OVAL_SUBTYPE_UNKNOWN;
	}
	const probe_table_entry_t entry = probe_table[idx];
	return entry.type;
}