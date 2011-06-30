/**
 * @file   uname.c
 * @brief  uname probe
 * @author "Steve Grubb" <sgrubb@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process an uname_object as defined in OVAL 5.4 and 5.5.
 *
 */

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
 *   Steve Grubb <sgrubb@redhat.com>
 */

/*
 * uname probe:
 *
 * machine_class
 * node_name
 * os_name
 * os_release
 * os_version
 * processor_type
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDIO_EXT_H
# include <stdio_ext.h>
#endif
#include <string.h>
#include <sys/utsname.h>

#include "seap.h"
#include "probe-api.h"
#include "alloc.h"

int probe_main(probe_ctx *ctx, void *arg)
{
	const char *processor;
	struct utsname buf;
        SEXP_t *item;

	// Collect the info
	uname(&buf);
	processor = buf.machine;
#ifdef __linux__
	if (strcmp(processor, "i686") == 0) {
		// See if we have an athlon processor
		FILE *f = fopen("/proc/cpu", "rt");
		if (f) {
			char tmp[128];
#ifdef HAVE_STDIO_EXT_H
                        __fsetlocking(f, FSETLOCKING_BYCALLER);
#endif
			while (fgets(tmp, sizeof(tmp), f)) {
				if (strncmp(tmp, "vendor_id", 9) == 0) {
					if (strstr(tmp, "AuthenticAMD")) {
						processor = "athlon";
						break;
					}
				}
			}
                        fclose(f);
		}
	}
#endif

        item = probe_item_create(OVAL_UNIX_UNAME, NULL,
                                 "machine_class",  OVAL_DATATYPE_STRING, buf.machine,
                                 "node_name",      OVAL_DATATYPE_STRING, buf.nodename,
                                 "os_name",        OVAL_DATATYPE_STRING, buf.sysname,
                                 "os_release",     OVAL_DATATYPE_STRING, buf.release,
                                 "os_version",     OVAL_DATATYPE_STRING, buf.version,
                                 "processor_type", OVAL_DATATYPE_STRING, processor,
                                 NULL);

        probe_item_collect(ctx, item);

	return 0;
}
