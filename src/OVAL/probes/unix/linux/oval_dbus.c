/*
 * Copyright 2023 Red Hat Inc., Durham, North Carolina.
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
 *      Evgenii Kolesnikov <ekolesni@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <limits.h>
#include "common/util.h"
#include "oscap_helpers.h"
#include "common/debug_priv.h"
#include "oval_dbus.h"


char *oval_dbus_value_to_string(DBusMessageIter *iter)
{
	const int arg_type = dbus_message_iter_get_arg_type(iter);
	if (dbus_type_is_basic(arg_type)) {
		_DBusBasicValue value;
		dbus_message_iter_get_basic(iter, &value);

		switch (arg_type)
		{
			case DBUS_TYPE_BYTE:
				return oscap_sprintf("%c", value.byt);

			case DBUS_TYPE_BOOLEAN:
				return oscap_strdup(value.bool_val ? "true" : "false");

			case DBUS_TYPE_INT16:
				return oscap_sprintf("%i", value.i16);

			case DBUS_TYPE_UINT16:
				return oscap_sprintf("%u", value.u16);

			case DBUS_TYPE_INT32:
				return oscap_sprintf("%i", value.i32);

			case DBUS_TYPE_UINT32:
				return oscap_sprintf("%u", value.u32);

#ifdef DBUS_HAVE_INT64
			case DBUS_TYPE_INT64:
				return oscap_sprintf("%li", value.i64);

			case DBUS_TYPE_UINT64:
				return oscap_sprintf("%lu", value.u64);
#endif

			case DBUS_TYPE_DOUBLE:
				return oscap_sprintf("%g", value.dbl);

			case DBUS_TYPE_STRING:
			case DBUS_TYPE_OBJECT_PATH:
			case DBUS_TYPE_SIGNATURE:
				return oscap_strdup(value.str);

			// We skip non-basic types for now
			//case DBUS_TYPE_ARRAY:
			//case DBUS_TYPE_STRUCT:
			//case DBUS_TYPE_DICT_ENTRY:
			//case DBUS_TYPE_VARIANT:
			//case DBUS_TYPE_UNIX_FD:
			//	return oscap_sprintf("%i", value.fd);

			default:
				dD("Encountered unknown D-Bus basic type: %d!", arg_type);
				return oscap_strdup("error, unknown basic type!");
		}
	} else if (arg_type == DBUS_TYPE_ARRAY) {
		DBusMessageIter array;
		dbus_message_iter_recurse(iter, &array);

		char *ret = NULL;
		do {
			char *element = oval_dbus_value_to_string(&array);

			if (element == NULL)
				continue;

			char *old_ret = ret;
			if (old_ret == NULL)
				ret = oscap_sprintf("%s", element);
			else
				ret = oscap_sprintf("%s, %s", old_ret, element);

			free(old_ret);
			free(element);
		}
		while (dbus_message_iter_next(&array));

		return ret;
	}

	return NULL;
}

DBusConnection *oval_connect_dbus(void)
{
	DBusConnection *conn = NULL;

	DBusError err;
	dbus_error_init(&err);

	const char *prefix = getenv("OSCAP_PROBE_ROOT");
	if (prefix != NULL) {
		char dbus_address[PATH_MAX] = {0};
		snprintf(dbus_address, PATH_MAX, "unix:path=%s/run/dbus/system_bus_socket", prefix);
		setenv("DBUS_SYSTEM_BUS_ADDRESS", dbus_address, 0);
		/* We won't overwrite DBUS_SYSTEM_BUS_ADDRESS so that
		 * user could have a way to define some non-standard system bus socket location */
	}

	conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (dbus_error_is_set(&err)) {
		dD("Failed to get DBUS_BUS_SYSTEM connection - %s", err.message);
		goto cleanup;
	}
	if (conn == NULL) {
		dD("DBusConnection == NULL!");
		goto cleanup;
	}

	dbus_bus_register(conn, &err);
	if (dbus_error_is_set(&err)) {
		dD("Failed to register on dbus - %s", err.message);
		goto cleanup;
	}

cleanup:
	dbus_error_free(&err);

	return conn;
}

void oval_disconnect_dbus(DBusConnection *conn)
{
	// NOOP

	// Connections retrieved via dbus_bus_get shall not be destroyed,
	// these connections are shared.
}
