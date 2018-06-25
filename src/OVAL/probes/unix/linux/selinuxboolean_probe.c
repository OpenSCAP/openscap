/**
 * @file   selinuxboolean_probe.c
 * @brief  selinuxboolean probe
 * @author "Petr Lautrbach" <plautrba@redhat.com>
 *
 *  This probe is able to process a selinuxboolean_object as defined in OVAL 5.8.
 *
 */

/*
 * Copyright 2009-2011 Red Hat Inc., Durham, North Carolina.
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
 *   Petr Lautrbach <plautrba@redhat.com>
 */

/*
 * selinuxboolean probe:
 *
 * name
 * current_status
 * pending_status
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"

#include <selinux/selinux.h>
#include "selinuxboolean_probe.h"

static int get_selinuxboolean(SEXP_t *ut_ent, probe_ctx *ctx)
{
	int err = 1, active, pending, len, i;
	SEXP_t *boolean, *item;
	char **booleans;

	if ( ! is_selinux_enabled()) {
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_NOT_APPLICABLE);
		return 0;
	}

	if (security_get_boolean_names(&booleans, &len) == -1) {
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
		return err;
	}

	for (i = 0; i < len; i++) {
		boolean = SEXP_string_new(booleans[i], strlen(booleans[i]));
		if (probe_entobj_cmp(ut_ent, boolean) == OVAL_RESULT_TRUE) {
			active = security_get_boolean_active(booleans[i]);
			pending = security_get_boolean_pending(booleans[i]);
			item = probe_item_create(
				OVAL_LINUX_SELINUXBOOLEAN, NULL,
				"name", OVAL_DATATYPE_SEXP, boolean,
				"current_status",  OVAL_DATATYPE_BOOLEAN, active,
				"pending_status", OVAL_DATATYPE_BOOLEAN, pending,
			      NULL);
			probe_item_collect(ctx, item);
		}
		SEXP_free(boolean);
	}

	for (i = 0; i < len; i++)
		free(booleans[i]);
        free(booleans);

	return 0;
}

int selinuxboolean_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in, *name;
	int err;

	probe_in  = probe_ctx_getobject(ctx);
	name = probe_obj_getent(probe_in, "name", 1);

	if (name == NULL) {
		return PROBE_ENOVAL;
	}

	err = get_selinuxboolean(name, ctx);
	SEXP_free(name);

	return err;
}
