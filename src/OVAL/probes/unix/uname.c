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
#if defined HAVE_SYS_SYSTEMINFO_H && defined (__SVR4) && defined (__sun)
#include <sys/systeminfo.h>
#endif

#include "seap.h"
#include "probe-api.h"
#include "alloc.h"
#include "uname.h"

int uname_probe_main(probe_ctx *ctx, void *arg)
{
	struct utsname buf;
        SEXP_t *item;
#if defined (__SVR4) && defined (__sun)
	char procbuf[SYS_NMLN];
#endif

	// Collect the info
	uname(&buf);

#if defined (__SVR4) && defined (__sun)
	if (sysinfo(SI_ARCHITECTURE, procbuf, sizeof(procbuf)) == -1)
		return(PROBE_EFATAL);
#endif
        item = probe_item_create(OVAL_UNIX_UNAME, NULL,
                                 "machine_class",  OVAL_DATATYPE_STRING, buf.machine,
                                 "node_name",      OVAL_DATATYPE_STRING, buf.nodename,
                                 "os_name",        OVAL_DATATYPE_STRING, buf.sysname,
                                 "os_release",     OVAL_DATATYPE_STRING, buf.release,
                                 "os_version",     OVAL_DATATYPE_STRING, buf.version,
#if defined (__SVR4) && defined (__sun)
                                 "processor_type", OVAL_DATATYPE_STRING, procbuf,
#else
                                 "processor_type", OVAL_DATATYPE_STRING, buf.machine,
#endif
                                 NULL);

        probe_item_collect(ctx, item);

	return 0;
}
