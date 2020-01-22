/**
 * @file   environmentvariable_probe.c
 * @brief  environmentvariable probe
 * @author "Petr Lautrbach" <plautrba@redhat.com>
 *
 *  This probe is able to process a environmentvariable_object as defined in OVAL 5.8.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *   Petr Lautrbach <plautrba@redhat.com>
 */

/*
 * environmentvariable probe:
 *
 * name
 * value
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "_seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "environmentvariable_probe.h"

extern char **environ;

static int read_environment(SEXP_t *un_ent, probe_ctx *ctx)
{
	int err = 1;
	char **env;
	size_t env_name_size;
	SEXP_t *env_name, *env_value, *item;

	for (env = environ; *env != 0; env++) {
		env_name_size = strchr(*env, '=') - *env; 
		env_name = SEXP_string_new(*env, env_name_size);
		env_value = SEXP_string_newf("%s", *env + env_name_size + 1);
		if (probe_entobj_cmp(un_ent, env_name) == OVAL_RESULT_TRUE) {
			item = probe_item_create(
				OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE, NULL,
				"name",  OVAL_DATATYPE_SEXP, env_name,
				"value", OVAL_DATATYPE_SEXP, env_value,
			      NULL);
			probe_item_collect(ctx, item);
			err = 0;
		}
		SEXP_free(env_name);
		SEXP_free(env_value);
	}
	return err;
}

int environmentvariable_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in, *ent;

	probe_in  = probe_ctx_getobject(ctx);
	ent = probe_obj_getent(probe_in, "name", 1);

	if (ent == NULL) {
		return PROBE_ENOVAL;
	}

	read_environment(ent, ctx);
	SEXP_free(ent);

	return 0;
}
