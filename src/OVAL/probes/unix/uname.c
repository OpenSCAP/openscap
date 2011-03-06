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

int probe_main(SEXP_t *object, SEXP_t *probe_out, void *arg, SEXP_t *filters)
{
	const char *processor;
	struct utsname buf;
	SEXP_t *r0, *r1, *r2, *r3, *r4, *r5, *item_sexp;

        (void)filters;

	if (object == NULL || probe_out == NULL) {
		return (PROBE_EINVAL);
	}

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
	item_sexp = probe_obj_creat("uname_item", NULL,
		/* entities */
		"machine_class", NULL, r0 = SEXP_string_newf("%s", buf.machine),
		"node_name", NULL, r1 = SEXP_string_newf("%s", buf.nodename),
		"os_name", NULL, r2 = SEXP_string_newf("%s", buf.sysname),
		"os_release", NULL, r3 = SEXP_string_newf("%s", buf.release),
		"os_version", NULL, r4 = SEXP_string_newf("%s", buf.version),
		"processor_type", NULL, r5 = SEXP_string_newf("%s", processor),
		NULL);
	SEXP_vfree(r0, r1, r2, r3, r4, r5, NULL);
	probe_cobj_add_item(probe_out, item_sexp);
	SEXP_free(item_sexp);

	return 0;
}
