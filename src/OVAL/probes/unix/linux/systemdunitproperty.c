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
#include <string.h>
#include "probe/entcmp.h"
#include "systemdshared.h"

struct unit_callback_vars {
	DBusConnection *dbus_conn;
	probe_ctx *ctx;
	SEXP_t *unit_entity;
	SEXP_t *property_entity;
	SEXP_t *se_unit;
	SEXP_t *se_property;
	SEXP_t *item;
};

static int property_callback(const char *property, const char *value, void *cbarg)
{
	struct unit_callback_vars *vars = (struct unit_callback_vars *)cbarg;

	if (vars->se_property != NULL) {
		//
		// Compare the previously matched entity to the current one
		// If they are the same, continue to fill the current item
		// with property values. If not, collect the item and create
		// a new one for the current property.
		//
		if (SEXP_strcmp(vars->se_property, property) == 0) {
			SEXP_t *se_value = SEXP_string_new(value, strlen(value));
			probe_item_ent_add(vars->item, "value", NULL, se_value);
			SEXP_free(se_value);
			return 0;
		} else {
			probe_item_collect(vars->ctx, vars->item);
			vars->item = NULL;
			SEXP_free(vars->se_property);
			vars->se_property = NULL;
		}
	}

	SEXP_t *se_property = SEXP_string_new(property, strlen(property));

	if (probe_entobj_cmp(vars->property_entity, se_property) != OVAL_RESULT_TRUE) {
		SEXP_free(se_property);
		return 0;
	} 

	vars->se_property = se_property;
	vars->item = probe_item_create(OVAL_LINUX_SYSTEMDUNITPROPERTY, NULL,
				       "unit", OVAL_DATATYPE_SEXP, vars->se_unit,
				       "property", OVAL_DATATYPE_SEXP, vars->se_property,
				       "value", OVAL_DATATYPE_STRING, value,
				       NULL);
	return 0;
}

static int unit_callback(const char *unit, void *cbarg)
{
	struct unit_callback_vars *vars = (struct unit_callback_vars *)cbarg;
	SEXP_t *se_unit = SEXP_string_new(unit, strlen(unit));

	if (probe_entobj_cmp(vars->unit_entity, se_unit) != OVAL_RESULT_TRUE) {
		/* Do nothing, continue with the next unit */
		SEXP_free(se_unit);
		return 0;
	}

	vars->se_unit = se_unit;
	vars->se_property = NULL;
	vars->item = NULL;

	char *unit_path = get_path_by_unit(vars->dbus_conn, unit);

	if (unit_path == NULL) {
		return 1;
	}

	get_all_properties_by_unit_path(vars->dbus_conn, unit_path,
					property_callback, vars);

	if (vars->item != NULL) {
		probe_item_collect(vars->ctx, vars->item);
		vars->item = NULL;
		SEXP_free(vars->se_property);
		vars->se_property = NULL;
	}

	SEXP_free(se_unit);
	return 0;
}

int probe_main(probe_ctx *ctx, void *probe_arg)
{
	SEXP_t *unit_entity, *probe_in, *property_entity;
	oval_version_t oval_version;

	probe_in = probe_ctx_getobject(ctx);
	oval_version = probe_obj_get_schema_version(probe_in);

	if (oval_version_cmp(oval_version, OVAL_VERSION(5.11)) < 0) {
		// OVAL 5.10 and less
		return PROBE_EOPNOTSUPP;
	}

	unit_entity = probe_obj_getent(probe_in, "unit", 1);
	property_entity = probe_obj_getent(probe_in, "property", 1);

	DBusError dbus_error;
	DBusConnection *dbus_conn;

	dbus_error_init(&dbus_error);
	dbus_conn = connect_dbus();

	if (dbus_conn == NULL) {
		dbus_error_free(&dbus_error);
		return PROBE_ESYSTEM;
	}

	struct unit_callback_vars vars;

	vars.dbus_conn = dbus_conn;
	vars.ctx = ctx;
	vars.unit_entity = unit_entity;
	vars.property_entity = property_entity;

	get_all_systemd_units(dbus_conn, unit_callback, &vars);

	SEXP_free(unit_entity);
	SEXP_free(property_entity);
	dbus_error_free(&dbus_error);
	disconnect_dbus(dbus_conn);

	return 0;
}
