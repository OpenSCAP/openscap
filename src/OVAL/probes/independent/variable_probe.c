/**
 * @file   variable_probe.c
 * @brief  variable probe
 * @author "Tomas Heinrich" <theinric@redhat.com>
 *
 *  This probe is able to process a variable_object as defined in OVAL 5.8.
 *
 */

/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

/*
 * variable probe
 *
 *   reference:
 *     http://oval.mitre.org/language/version5.8/ovaldefinition/documentation/independent-definitions-schema.html#variable_object
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <probe/probe.h>
#include <probe/option.h>
#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "variable_probe.h"

int variable_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_ALL;
}

int variable_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in, *vr_ent, *sid, *sval, *svallst, *item;

	probe_in = probe_ctx_getobject(ctx);
	vr_ent = probe_obj_getent(probe_in, "var_ref", 1);
	sid = probe_ent_getattrval(vr_ent, "var_ref");
	probe_ent_getvals(vr_ent, &svallst);

	item = probe_item_create(OVAL_INDEPENDENT_VARIABLE, NULL,
		"var_ref", OVAL_DATATYPE_SEXP, sid,
		NULL);

	SEXP_list_foreach(sval, svallst)
		probe_item_ent_add(item, "value", NULL, sval);

	probe_item_collect(ctx, item);
	SEXP_free(vr_ent);
	SEXP_free(sid);
	SEXP_free(svallst);

	return 0;
}
