/*
 * Copyright 2016 Red Hat Inc., Durham, North Carolina.
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
 */

#include "rpm-helper.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef RPM46_FOUND
int rpmErrorCb (rpmlogRec rec, rpmlogCallbackData data)
{
	dE("RPM: %s", rpmlogRecMessage(rec));
	return RPMLOG_DEFAULT;
}
#endif

#ifndef HAVE_RPMVERIFYFILE
int rpmVerifyFile(const rpmts ts, const rpmfi fi,
		rpmVerifyAttrs * res, rpmVerifyAttrs omitMask)
{
    rpmVerifyAttrs vfy = rpmfiVerify(fi, omitMask);
    if (res)
	*res = vfy;

    return (vfy & RPMVERIFY_LSTATFAIL) ? 1 : 0;
}
#endif

void rpmLibsPreload()
{
	// Don't load rpmrc files. The are useless for us,
	// because we only need to preload libraries
	const char* rcfiles = "";
	rpmReadConfigFiles(rcfiles, NULL);
}

void set_rpm_db_path()
{
	/*
	 * Fedora >=36 changed the default dbpath in librpm from /var/lib/rpm to /usr/lib/sysimage/rpm.
	 * See: https://fedoraproject.org/wiki/Changes/RelocateRPMToUsr
	 *
	 * Therefore, when running openscap on a Fedora >=36 system scanning another systems (such as RHEL, SLES, Fedora<36)
	 * openscap's librpm will try to read the rpm db from /usr/lib/sysimage/rpm which doesn't exist and therefore won't work.
	 * On many systems, /var/lib/rpm is still a symlink to /usr/lib/sysimage/rpm, so using /var/lib/rpm can work there.
	 * However, on some systems, eg. bootc images, /var/lib/rpm isn't a symlink and doesn't contain the RPM database.
	 *
	 * We will first try if the "new" location /usr/lib/sysimage/rpm exists, and use it only if it exists.
	 * If it doesn't exist, we will fall back to the "old" location /var/lib/rpm.
	 */

	struct stat sb;
	const char *dbpath;
	const char *prefix = getenv("OSCAP_PROBE_ROOT");
	char *path_with_prefix = oscap_path_join(prefix, "/usr/lib/sysimage/rpm");
	if (stat(path_with_prefix, &sb) == 0) {
		dbpath = "/usr/lib/sysimage/rpm";
	} else {
		dbpath = "/var/lib/rpm";
	}
	free(path_with_prefix);
	dI("Using %s as rpm database.", dbpath);
	rpmPushMacro(NULL, "_dbpath", NULL, dbpath, RMIL_CMDLINE);
}
