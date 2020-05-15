/**
 * @file   systemdunitproperty_probe.c
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <probe-api.h>
#include <string.h>
#include <probe/probe.h>

#include "probe/entcmp.h"
#include "systemdshared.h"
#include "systemdunitproperty_probe.h"

struct unit_callback_vars {
	DBusConnection *dbus_conn;
	probe_ctx *ctx;
	SEXP_t *unit_entity;
	SEXP_t *property_entity;
	SEXP_t *se_unit;
	SEXP_t *se_property;
	SEXP_t *item;
};

static int collect_property(const char *property, const char *value, struct unit_callback_vars *vars);

static int get_all_properties_by_unit_path_using_interface(DBusConnection *conn, const char *unit_path, const char *interface, void *vars)
{
	int ret = 1;
	DBusMessage *msg = NULL;
	DBusPendingCall *pending = NULL;

	msg = dbus_message_new_method_call(
		"org.freedesktop.systemd1",
		unit_path,
		"org.freedesktop.DBus.Properties",
		"GetAll"
	);
	if (msg == NULL) {
		dD("Failed to create dbus_message via dbus_message_new_method_call!");
		goto cleanup;
	}

	DBusMessageIter args, property_iter;


	dbus_message_iter_init_append(msg, &args);
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface)) {
		dD("Failed to append interface '%s' string parameter to dbus message!", interface);
		goto cleanup;
	}

	if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
		dD("Failed to send message via dbus!");
		goto cleanup;
	}
	if (pending == NULL) {
		dD("Invalid dbus pending call!");
		goto cleanup;
	}

	dbus_connection_flush(conn);
	dbus_message_unref(msg); msg = NULL;

	dbus_pending_call_block(pending);
	msg = dbus_pending_call_steal_reply(pending);
	if (msg == NULL) {
		dD("Failed to steal dbus pending call reply.");
		goto cleanup;
	}
	dbus_pending_call_unref(pending); pending = NULL;

	if (!dbus_message_iter_init(msg, &args)) {
		dD("Failed to initialize iterator over received dbus message.");
		goto cleanup;
	}

	if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_ARRAY && dbus_message_iter_get_element_type(&args) != DBUS_TYPE_DICT_ENTRY) {
		dD("Expected array of dict_entry argument in reply. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
		goto cleanup;
	}

	dbus_message_iter_recurse(&args, &property_iter);
	do {
		DBusMessageIter dict_entry, value_variant;
		dbus_message_iter_recurse(&property_iter, &dict_entry);

		if (dbus_message_iter_get_arg_type(&dict_entry) != DBUS_TYPE_STRING) {
			dD("Expected string as key in dict_entry. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&dict_entry)));
			goto cleanup;
		}

		_DBusBasicValue value;
		dbus_message_iter_get_basic(&dict_entry, &value);
		char *property_name = oscap_strdup(value.str);

		if (dbus_message_iter_next(&dict_entry) == false) {
			dW("Expected another field in dict_entry.");
			free(property_name);
			goto cleanup;
		}

		if (dbus_message_iter_get_arg_type(&dict_entry) != DBUS_TYPE_VARIANT) {
			dD("Expected variant as value in dict_entry. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&dict_entry)));
			free(property_name);
			goto cleanup;
		}

		dbus_message_iter_recurse(&dict_entry, &value_variant);

		int cbret = 0;
		const int arg_type = dbus_message_iter_get_arg_type(&value_variant);
		// DBUS_TYPE_ARRAY is a special case, we report each element as one value entry
		if (arg_type == DBUS_TYPE_ARRAY) {
			DBusMessageIter array;
			dbus_message_iter_recurse(&value_variant, &array);

			do {
				char *element = dbus_value_to_string(&array);
				if (element == NULL)
					continue;

				const int elementcbret = collect_property(property_name, element, vars);
				if (elementcbret > cbret)
					cbret = elementcbret;

				free(element);
			}
			while (dbus_message_iter_next(&array));
		}
		else {
			char *property_value = dbus_value_to_string(&value_variant);
			cbret = collect_property(property_name, property_value, vars);
			free(property_value);
		}

		free(property_name);
		if (cbret != 0) {
			goto cleanup;
		}
	}
	while (dbus_message_iter_next(&property_iter));

	dbus_message_unref(msg); msg = NULL;
	ret = 0;

cleanup:
	if (pending != NULL)
		dbus_pending_call_unref(pending);

	if (msg != NULL)
		dbus_message_unref(msg);

	return ret;
}

static int get_all_properties_by_unit_path(DBusConnection *conn, const char *unit_path, void *vars)
{
	const char *interface = "org.freedesktop.systemd1.Unit";
	int ret = get_all_properties_by_unit_path_using_interface(conn, unit_path, interface, vars);
	return ret;
}

static int collect_property(const char *property, const char *value, struct unit_callback_vars *vars)
{
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

	get_all_properties_by_unit_path(vars->dbus_conn, unit_path, vars);

	if (vars->item != NULL) {
		probe_item_collect(vars->ctx, vars->item);
		vars->item = NULL;
		SEXP_free(vars->se_property);
		vars->se_property = NULL;
	}

	SEXP_free(se_unit);
	return 0;
}

int systemdunitproperty_probe_offline_mode_supported(void)
{
	return PROBE_OFFLINE_OWN;
}

int systemdunitproperty_probe_main(probe_ctx *ctx, void *probe_arg)
{
	SEXP_t *unit_entity, *probe_in, *property_entity;
	oval_schema_version_t oval_version;

	probe_in = probe_ctx_getobject(ctx);
	oval_version = probe_obj_get_platform_schema_version(probe_in);

	if (oval_schema_version_cmp(oval_version, OVAL_SCHEMA_VERSION(5.11)) < 0) {
		// OVAL 5.10 and less
		return PROBE_EOPNOTSUPP;
	}

	DBusError dbus_error;
	DBusConnection *dbus_conn;

	dbus_error_init(&dbus_error);
	dbus_conn = connect_dbus();

	if (dbus_conn == NULL) {
		dbus_error_free(&dbus_error);
		SEXP_t *msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_INFO, "DBus connection failed, could not identify systemd units.");
		probe_cobj_set_flag(probe_ctx_getresult(ctx), ctx->offline_mode == PROBE_OFFLINE_NONE ? SYSCHAR_FLAG_ERROR : SYSCHAR_FLAG_NOT_COLLECTED);
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		return 0;
	}

	unit_entity = probe_obj_getent(probe_in, "unit", 1);
	property_entity = probe_obj_getent(probe_in, "property", 1);

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
