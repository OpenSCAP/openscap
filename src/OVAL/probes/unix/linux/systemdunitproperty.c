/**
 * @file   systemdunitproperty.c
 * @brief  implementation of the systemdunitproperty_object
 * @author
 */

/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <probe-api.h>

int probe_main(probe_ctx *ctx, void *probe_arg)
{
	SEXP_t *unit_entity, *probe_in;
	oval_version_t oval_version;

	probe_in = probe_ctx_getobject(ctx);
	oval_version = probe_obj_get_schema_version(probe_in);

	if (oval_version_cmp(oval_version, OVAL_VERSION(5.11)) < 0) {
		// OVAL 5.10 and less
		return PROBE_EOPNOTSUPP;
	}

	unit_entity = probe_obj_getent(probe_in, "unit", 1);
	SEXP_free(unit_entity);
/*
	SEXP_t *item = probe_item_create(OVAL_LINUX_SYSTEMDUNITPROPERTY, ...);
	probe_item_collect(ctx, item);
*/
        return (0);
}
