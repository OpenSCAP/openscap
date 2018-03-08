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
#include <probe/probe.h>
#include <string.h>
#include "probe/entcmp.h"
#include "systemdshared.h"

const char *UNIT_FILE_STATE = "UnitFileState";

static int get_all_properties_by_unit_path(DBusConnection *conn, const char *unit_path, int(*callback)(const char *name, const char *value, void *arg), void *cbarg)
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
		dI("Failed to create dbus_message via dbus_message_new_method_call!");
		goto cleanup;
	}

	DBusMessageIter args, property_iter;

	const char *interface = "org.freedesktop.systemd1.Unit";

	dbus_message_iter_init_append(msg, &args);
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface)) {
		dI("Failed to append interface '%s' string parameter to dbus message!", interface);
		goto cleanup;
	}

	if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
		dI("Failed to send message via dbus!");
		goto cleanup;
	}
	if (pending == NULL) {
		dI("Invalid dbus pending call!");
		goto cleanup;
	}

	dbus_connection_flush(conn);
	dbus_message_unref(msg); msg = NULL;

	dbus_pending_call_block(pending);
	msg = dbus_pending_call_steal_reply(pending);
	if (msg == NULL) {
		dI("Failed to steal dbus pending call reply.");
		goto cleanup;
	}
	dbus_pending_call_unref(pending); pending = NULL;

	if (!dbus_message_iter_init(msg, &args)) {
		dI("Failed to initialize iterator over received dbus message.");
		goto cleanup;
	}

	if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_ARRAY && dbus_message_iter_get_element_type(&args) != DBUS_TYPE_DICT_ENTRY) {
		dI("Expected array of dict_entry argument in reply. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
		goto cleanup;
	}

	dbus_message_iter_recurse(&args, &property_iter);
	do {
		DBusMessageIter dict_entry, value_variant;
		dbus_message_iter_recurse(&property_iter, &dict_entry);

		if (dbus_message_iter_get_arg_type(&dict_entry) != DBUS_TYPE_STRING) {
			dI("Expected string as key in dict_entry. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&dict_entry)));
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
			dI("Expected variant as value in dict_entry. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&dict_entry)));
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

				const int elementcbret = callback(property_name, element, cbarg);
				if (elementcbret > cbret)
					cbret = elementcbret;

				free(element);
			}
			while (dbus_message_iter_next(&array));
		}
		else {
			char *property_value = dbus_value_to_string(&value_variant);
			cbret = callback(property_name, property_value, cbarg);
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

struct unit_callback_vars {
	DBusConnection *dbus_conn;
	probe_ctx *ctx;
	SEXP_t *unit_entity;
	SEXP_t *property_entity;
	SEXP_t *se_unit;
	SEXP_t *se_property;
	SEXP_t *item;
	char *offline_unit_state;
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

	if (vars->dbus_conn != NULL) {
		char *unit_path = get_path_by_unit(vars->dbus_conn, unit);

		if (unit_path == NULL) {
			return 1;
		}
	
		get_all_properties_by_unit_path(vars->dbus_conn, unit_path,
						property_callback, vars);
	} else {
		property_callback(UNIT_FILE_STATE, vars->offline_unit_state, vars);
	}

	if (vars->item != NULL) {
		probe_item_collect(vars->ctx, vars->item);
		vars->item = NULL;
		SEXP_free(vars->se_property);
		vars->se_property = NULL;
	}

	SEXP_free(se_unit);
	return 0;
}

void get_unit_file_state_in_offline_mode(const char *unit, struct unit_callback_vars *vars)
{
	char systemctl_call[] = "systemctl list-unit-files | grep -P '\\S+\\.service'";

	FILE *p = popen(systemctl_call, "r");
	size_t len = 0;
	char *ln = NULL;

	while (getline(&ln, &len, p) != -1) {
		char name[256];
		char state[16];
		size_t i = 0;
		
		// Find name
		while (i < len && ln[i] != ' ') ++i;
		strncpy(name, ln, i);
		name[i] = '\0';
		
		// Find state
		size_t begin_state = 0;
		while (i < len && ln[i] == ' ') ++i;
		begin_state = i;
		while (i < len && ln[i] != ' ' && ln[i] != '\n') ++i;
		strncpy(state, &ln[begin_state], i - begin_state);
		state[i - begin_state] = '\0';
	
		vars->offline_unit_state = state;
		unit_callback(name, vars);
	}
	free(ln);
	pclose(p);
}

void probe_offline_mode()
{
	probe_setoption(PROBEOPT_OFFLINE_MODE_SUPPORTED, PROBE_OFFLINE_CHROOT);
}

int probe_main(probe_ctx *ctx, void *probe_arg)
{
	SEXP_t *unit_entity, *probe_in, *property_entity;
	oval_schema_version_t oval_version;

	probe_in = probe_ctx_getobject(ctx);
	oval_version = probe_obj_get_platform_schema_version(probe_in);

	if (oval_schema_version_cmp(oval_version, OVAL_SCHEMA_VERSION(5.11)) < 0) {
		// OVAL 5.10 and less
		return PROBE_EOPNOTSUPP;
	}
	
	unit_entity = probe_obj_getent(probe_in, "unit", 1);
	property_entity = probe_obj_getent(probe_in, "property", 1);
	
	struct unit_callback_vars vars;

	vars.dbus_conn = NULL;
	vars.ctx = ctx;
	vars.unit_entity = unit_entity;
	vars.property_entity = property_entity;
	vars.offline_unit_state = NULL;

	DBusError dbus_error;
	DBusConnection *dbus_conn;

	dbus_error_init(&dbus_error);
	dbus_conn = connect_dbus();

	if (dbus_conn != NULL) {
		vars.dbus_conn = dbus_conn;
		get_all_systemd_units(dbus_conn, unit_callback, &vars);
		dbus_error_free(&dbus_error);
		disconnect_dbus(dbus_conn);
	} else {
		// Allows the probe to retrieve property UnitFileState even if systemd is not accessible
		SEXP_t *temp_unit_entity, *temp_property_entity;
		const char *cstr_unit_entity = SEXP_string_cstr(temp_unit_entity = probe_ent_getval(unit_entity));
		SEXP_free(temp_unit_entity);
		const char *cstr_property_entity = SEXP_string_cstr(temp_property_entity = probe_ent_getval(property_entity));
		SEXP_free(temp_property_entity);
		
		if(strcmp(cstr_property_entity, UNIT_FILE_STATE) != 0) {
			dbus_error_free(&dbus_error);
			SEXP_t *msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_INFO, "DBus connection failed, could not identify systemd units.");
			probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
			probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
			SEXP_free(msg);
			
			return 0;
		}
		
		get_unit_file_state_in_offline_mode(cstr_unit_entity, &vars);
		
		free(cstr_unit_entity);
		free(cstr_property_entity);
	}

	SEXP_free(unit_entity);
	SEXP_free(property_entity);

	return 0;
}
