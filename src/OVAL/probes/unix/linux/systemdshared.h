/**
 * @file   systemdshared.c
 * @brief  functionality shared between systemdunitproperty and systemdunitdependency tests
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

#include <dbus/dbus.h>
#include "common/debug_priv.h"

// Old versions of libdbus API don't have DBusBasicValue and DBus8ByteStruct
// as a public typedefs.
// These two typedefs were copied from libdbus 1.8 branch, see
// http://cgit.freedesktop.org/dbus/dbus/tree/dbus/dbus-types.h?h=dbus-1.8#n137
typedef struct
{
	dbus_uint32_t first32;
	dbus_uint32_t second32;
} _DBus8ByteStruct;

typedef union
{
	unsigned char bytes[8]; /**< as 8 individual bytes */
	dbus_int16_t  i16;   /**< as int16 */
	dbus_uint16_t u16;   /**< as int16 */
	dbus_int32_t  i32;   /**< as int32 */
	dbus_uint32_t u32;   /**< as int32 */
	dbus_bool_t   bool_val; /**< as boolean */
#ifdef DBUS_HAVE_INT64
	dbus_int64_t  i64;   /**< as int64 */
	dbus_uint64_t u64;   /**< as int64 */
#endif
	_DBus8ByteStruct eight; /**< as 8-byte struct */
	double dbl;          /**< as double */
	unsigned char byt;   /**< as byte */
	char *str;           /**< as char* (string, object path or signature) */
	int fd;              /**< as Unix file descriptor */
} _DBusBasicValue;

static char *get_path_by_unit(DBusConnection *conn, const char *unit)
{
	DBusMessage *msg = NULL;
	DBusPendingCall *pending = NULL;
	_DBusBasicValue path;
	char *ret = NULL;

	msg = dbus_message_new_method_call(
		"org.freedesktop.systemd1",
		"/org/freedesktop/systemd1",
		"org.freedesktop.systemd1.Manager",
		// LoadUnit is similar to GetUnit except it will load the unit file
		// if it hasn't been loaded yet.
		"LoadUnit"
	);
	if (msg == NULL) {
		dI("Failed to create dbus_message via dbus_message_new_method_call!");
		goto cleanup;
	}

	DBusMessageIter args;

	dbus_message_iter_init_append(msg, &args);
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &unit)) {
		dI("Failed to append unit '%s' string parameter to dbus message!", unit);
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

	if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_OBJECT_PATH) {
		dI("Expected string argument in reply. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
		goto cleanup;
	}

	dbus_message_iter_get_basic(&args, &path);
	ret = oscap_strdup(path.str);
	dbus_message_unref(msg); msg = NULL;

cleanup:
	if (pending != NULL)
		dbus_pending_call_unref(pending);

	if (msg != NULL)
		dbus_message_unref(msg);

	return ret;
}

static int get_all_systemd_units(DBusConnection* conn, int(*callback)(const char *, void *), void *cbarg)
{
	DBusMessage *msg = NULL;
	DBusPendingCall *pending = NULL;
	char ret = 1;

	msg = dbus_message_new_method_call(
		"org.freedesktop.systemd1",
		"/org/freedesktop/systemd1",
		"org.freedesktop.systemd1.Manager",
		"ListUnits"
	);
	if (msg == NULL) {
		dI("Failed to create dbus_message via dbus_message_new_method_call!");
		goto cleanup;
	}

	DBusMessageIter args, unit_iter;

	// the args should be empty for this call
	dbus_message_iter_init_append(msg, &args);

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

	if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_ARRAY) {
		dI("Expected array of structs in reply. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
		goto cleanup;
	}

	dbus_message_iter_recurse(&args, &unit_iter);
	do {
		if (dbus_message_iter_get_arg_type(&unit_iter) != DBUS_TYPE_STRUCT) {
			dI("Expected unit struct as elements in returned array. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&unit_iter)));
			goto cleanup;
		}

		DBusMessageIter unit_name;
		dbus_message_iter_recurse(&unit_iter, &unit_name);

		if (dbus_message_iter_get_arg_type(&unit_name) != DBUS_TYPE_STRING) {
			dI("Expected string as the first element in the unit struct. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&unit_name)));
			goto cleanup;
		}

		_DBusBasicValue value;
		dbus_message_iter_get_basic(&unit_name, &value);
		char *unit_name_s = oscap_strdup(value.str);
		int cbret = callback(unit_name_s, cbarg);
		oscap_free(unit_name_s);
		if (cbret != 0) {
			goto cleanup;
		}
	}
	while (dbus_message_iter_next(&unit_iter));

	dbus_message_unref(msg); msg = NULL;

	ret = 0;

cleanup:
	if (pending != NULL)
		dbus_pending_call_unref(pending);

	if (msg != NULL)
		dbus_message_unref(msg);

	return ret;
}

static char *dbus_value_to_string(DBusMessageIter *iter)
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
				return oscap_sprintf("%lli", value.i32);

			case DBUS_TYPE_UINT64:
				return oscap_sprintf("%llu", value.u32);
#endif

			case DBUS_TYPE_DOUBLE:
				return oscap_sprintf("%g", value.dbl);

			case DBUS_TYPE_STRING:
			case DBUS_TYPE_OBJECT_PATH:
			case DBUS_TYPE_SIGNATURE:
				return oscap_strdup(value.str);

			// non-basic types
			//case DBUS_TYPE_ARRAY:
			//case DBUS_TYPE_STRUCT:
			//case DBUS_TYPE_DICT_ENTRY:
			//case DBUS_TYPE_VARIANT:

			//case DBUS_TYPE_UNIX_FD:
			//	return oscap_sprintf("%i", value.fd);

			default:
				dI("Encountered unknown dbus basic type!");
				return oscap_strdup("error, unknown basic type!");
		}
	}
	else if (arg_type == DBUS_TYPE_ARRAY) {
		DBusMessageIter array;
		dbus_message_iter_recurse(iter, &array);

		char *ret = NULL;
		do {
			char *element = dbus_value_to_string(&array);

			if (element == NULL)
				continue;

			char *old_ret = ret;
			if (old_ret == NULL)
				ret = oscap_sprintf("%s", element);
			else
				ret = oscap_sprintf("%s, %s", old_ret, element);

			oscap_free(old_ret);
			oscap_free(element);
		}
		while (dbus_message_iter_next(&array));

		return ret;
	}/*
	else if (arg_type == DBUS_TYPE_VARIANT) {
		DBusMessageIter inner;
		dbus_message_iter_recurse(iter, &inner);
		return dbus_value_to_string(&inner);
	}*/

	return NULL;
}

static DBusConnection *connect_dbus()
{
	DBusConnection *conn = NULL;

	DBusError err;
	dbus_error_init(&err);

	conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (dbus_error_is_set(&err)) {
		dI("Failed to get DBUS_BUS_SYSTEM connection - %s", err.message);
		goto cleanup;
	}
	if (conn == NULL) {
		dI("DBusConnection == NULL!");
		goto cleanup;
	}

	dbus_bus_register(conn, &err);
	if (dbus_error_is_set(&err)) {
		dI("Failed to register on dbus - %s", err.message);
		goto cleanup;
	}

cleanup:
	dbus_error_free(&err);

	return conn;
}

static void disconnect_dbus(DBusConnection *conn)
{
	// NOOP

	// Connections retrieved via dbus_bus_get shall not be destroyed,
	// these connections are shared.
}
