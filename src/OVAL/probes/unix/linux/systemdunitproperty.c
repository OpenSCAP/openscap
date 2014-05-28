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
#include "systemdshared.h"

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

	return 0;
}

static int get_all_properties_by_unit_path(DBusConnection *conn, const char *unit_path)
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
		dI("Failed to create dbus_message via dbus_message_new_method_call!\n");
		goto cleanup;
	}

	DBusMessageIter args, property_iter;

	const char *interface = "org.freedesktop.systemd1.Unit";

	dbus_message_iter_init_append(msg, &args);
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface)) {
		dI("Failed to append interface '%s' string parameter to dbus message!\n", interface);
		goto cleanup;
	}

	if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
		dI("Failed to send message via dbus!\n");
		goto cleanup;
	}
	if (pending == NULL) {
		dI("Invalid dbus pending call!\n");
		goto cleanup;
	}

	dbus_connection_flush(conn);
	dbus_message_unref(msg); msg = NULL;

	dbus_pending_call_block(pending);
	msg = dbus_pending_call_steal_reply(pending);
	if (msg == NULL) {
		dI("Failed to steal dbus pending call reply.\n");
		goto cleanup;
	}
	dbus_pending_call_unref(pending); pending = NULL;

	if (!dbus_message_iter_init(msg, &args)) {
		dI("Failed to initialize iterator over received dbus message.\n");
		goto cleanup;
	}

	if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_ARRAY && dbus_message_iter_get_element_type(&args) != DBUS_TYPE_DICT_ENTRY) {
		dI("Expected array of dict_entry argument in reply. Instead received: %s.\n", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
		goto cleanup;
	}

	dbus_message_iter_recurse(&args, &property_iter);
	do {
		DBusMessageIter dict_entry, value_variant;
		dbus_message_iter_recurse(&property_iter, &dict_entry);

		if (dbus_message_iter_get_arg_type(&dict_entry) != DBUS_TYPE_STRING) {
			dI("Expected string as key in dict_entry. Instead received: %s.\n", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&dict_entry)));
			goto cleanup;
		}

		DBusBasicValue value;
		dbus_message_iter_get_basic(&dict_entry, &value);
		char *property_name = oscap_strdup(value.str);

		dbus_message_iter_next(&dict_entry);

		if (dbus_message_iter_get_arg_type(&dict_entry) != DBUS_TYPE_VARIANT) {
			dI("Expected variant as value in dict_entry. Instead received: %s.\n", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&dict_entry)));
			goto cleanup;
		}

		dbus_message_iter_recurse(&dict_entry, &value_variant);

		char *property_value = dbus_value_to_string(&value_variant);
		printf("property '%s' = '%s'\n", property_name, property_value);
		oscap_free(property_value);

		oscap_free(property_name);
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

// temporary testing function
int test_systemd(void)
{
	int ret = 1;
	char *path = NULL;

	DBusError err;
	dbus_error_init(&err);

	DBusConnection *conn = connect_dbus();
	if (!conn)
		goto cleanup;

	const char *unit = "multi-user.target";

	path = get_path_by_unit(conn, unit);

	// just a specific property
	{
		char *value = get_property_by_unit_path(conn, path, "ActiveState");
		printf("property: %s\n", value);
		oscap_free(value);
	}

	// all properties
	{
		get_all_properties_by_unit_path(conn, path);
	}

cleanup:
	dbus_error_free(&err);

	if (path != NULL)
		oscap_free(path);

	disconnect_dbus(conn);

	return ret;
}
