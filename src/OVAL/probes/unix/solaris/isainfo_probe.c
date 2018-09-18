/**
 * @file isainfo_probe.c
 * @brief isainfo probe
 * @author "Ryan E Haggerty" <rhaggerty@tresys.com
 *
 * 2011/05/06 rhaggerty@tresys.com
 *
 * This probe relates to the instruction set architecture of a system and
 * returns the following:
 *	*isainfo_object
 *		Due to its very nature, a system has exactly one item that can be returned,
 *		therefore there are no attributes to define in the object declaration.
 */

/*
 * Copyright (c) 2011 Tresys Technology LLC, Columbia, Maryland, USA
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
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "probe-api.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <regex.h>
#include <sys/systeminfo.h>
#include "../../../../common/debug_priv.h"
#include "isainfo_probe.h"

/* man sysinfo (2) recommends using 257 for this size */
#define MAX_STR_RESULT 257

struct result_info {
	unsigned int bits;
	char kernel_isa[MAX_STR_RESULT];
	char application_isa[MAX_STR_RESULT];
};


static void report_finding(struct result_info *res, probe_ctx *ctx)
{
	SEXP_t *item;

	item = probe_item_create(OVAL_SOLARIS_ISAINFO, NULL,
		"bits", OVAL_DATATYPE_INTEGER, (int64_t)res->bits,
		"kernel_isa", OVAL_DATATYPE_STRING, res->kernel_isa,
		"application_isa", OVAL_DATATYPE_STRING, res->application_isa,
		NULL);
	probe_item_collect(ctx, item);
}

int read_sysinfo(probe_ctx *ctx) {
	dI("In read_sysinfo for isainfo probe");

	int err = 1;
	int ret = 0;
	char tmp_buf[MAX_STR_RESULT];
	struct result_info result;

	/* sysinfo returns -1 on failure according to man sysinfo (2) */

	ret = sysinfo(SI_ARCHITECTURE_64, tmp_buf, MAX_STR_RESULT);

	if (ret != -1) {
		result.bits = 64;
	}
	else {
		result.bits = 32;
	}

	if (sysinfo(SI_ARCHITECTURE_K, result.kernel_isa, MAX_STR_RESULT) == -1) {
		return err;
	}
#if defined(OS_SOLARIS)
	if ((sysinfo(SI_ARCHITECTURE_32, result.application_isa, MAX_STR_RESULT) == -1) &&
	   (sysinfo(SI_ARCHITECTURE_64, result.application_isa, MAX_STR_RESULT) == -1)) {
#else
	if (sysinfo(SI_ARCHITECTURE_NATIVE, result.application_isa, MAX_STR_RESULT) == -1) {
#endif
		return err;
	}

	/* if we made it this far, there were no errors in the sysinfo calls */
	err = 0;
	report_finding(&result, ctx);
	return err;
}

int isainfo_probe_main(probe_ctx *ctx, void *probe_arg)
{
	if (read_sysinfo(ctx)) {
		return PROBE_EACCESS;
	}

	return 0;
}
