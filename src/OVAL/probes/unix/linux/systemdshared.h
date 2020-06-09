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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *
 */

#pragma once

#ifndef OPENSCAP_OVAL_PROBES_SYSTEMDSHARED_H_
#define OPENSCAP_OVAL_PROBES_SYSTEMDSHARED_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <limits.h>
#include <stdio.h>
#include <dbus/dbus.h>
#include "common/debug_priv.h"
#include "oscap_helpers.h"

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
		dD("Failed to create dbus_message via dbus_message_new_method_call!");
		goto cleanup;
	}

	DBusMessageIter args;

	dbus_message_iter_init_append(msg, &args);
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &unit)) {
		dD("Failed to append unit '%s' string parameter to dbus message!", unit);
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

	if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_OBJECT_PATH) {
		dD("Expected string argument in reply. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
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
		dD("Failed to create dbus_message via dbus_message_new_method_call!");
		goto cleanup;
	}

	DBusMessageIter args, unit_iter;

	// the args should be empty for this call
	dbus_message_iter_init_append(msg, &args);

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

	if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_ARRAY) {
		dD("Expected array of structs in reply. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
		goto cleanup;
	}

	dbus_message_iter_recurse(&args, &unit_iter);
	do {
		if (dbus_message_iter_get_arg_type(&unit_iter) != DBUS_TYPE_STRUCT) {
			dD("Expected unit struct as elements in returned array. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&unit_iter)));
			goto cleanup;
		}

		DBusMessageIter unit_name;
		dbus_message_iter_recurse(&unit_iter, &unit_name);

		if (dbus_message_iter_get_arg_type(&unit_name) != DBUS_TYPE_STRING) {
			dD("Expected string as the first element in the unit struct. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&unit_name)));
			goto cleanup;
		}

		_DBusBasicValue value;
		dbus_message_iter_get_basic(&unit_name, &value);
		char *unit_name_s = oscap_strdup(value.str);
		int cbret = callback(unit_name_s, cbarg);
		free(unit_name_s);
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
	/* Unfortunately we don't know anything about semantics of data that we
	 * recieved from DBus. We can only use conversion that fits in most generic
	 * cases. This is a difference from behavior of systemctl show command.
	 * Systemd knows semantics of DBus data and in some situation it uses a
	 * different conversion, for example converts integer values to date and
	 * time.
	 */
	const int arg_type = dbus_message_iter_get_arg_type(iter);
	if (dbus_type_is_basic(arg_type)) {
		_DBusBasicValue value;
		dbus_message_iter_get_basic(iter, &value);

		switch (arg_type)
		{
			case DBUS_TYPE_BYTE:
				return oscap_sprintf("0x%x", value.byt);

			case DBUS_TYPE_BOOLEAN:
				return oscap_strdup(value.bool_val ? "yes" : "no");

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
				return oscap_sprintf("%lli", value.i64);

			case DBUS_TYPE_UINT64:
				return oscap_sprintf("%llu", value.u64);
#endif

			case DBUS_TYPE_DOUBLE:
				return oscap_sprintf("%g", value.dbl);

			case DBUS_TYPE_STRING:
			case DBUS_TYPE_OBJECT_PATH:
			case DBUS_TYPE_SIGNATURE:
				return oscap_strdup(value.str);

			//case DBUS_TYPE_UNIX_FD:
			//	return oscap_sprintf("%i", value.fd);

			default:
				dD("Encountered unknown dbus basic type!");
				return oscap_strdup("error, unknown basic type!");
		}
	}
	else if (arg_type == DBUS_TYPE_ARRAY || arg_type == DBUS_TYPE_STRUCT ||
			arg_type == DBUS_TYPE_VARIANT || arg_type == DBUS_TYPE_DICT_ENTRY) {
		/* OVAL specification for systemdunitproperty_item says: "The value
		 * of the property associated with a systemd unit. Exactly one value
		 * shall be used for all property types except dbus arrays - each
		 * array element shall be represented by one value."
		 * The properties that are dbus arrays are handled by the caller
		 * (get_all_properties_by_unit_path_using_interface). Therefore this
		 * code handles only dbus arrays that are part of a different top
		 * level dbus type. Also it handles other dbus container types which
		 * are not mentioned in the OVAL specification. We assume that these
		 * types should be serialized into a single value.
		 */
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
				ret = oscap_sprintf("%s ; %s", old_ret, element);

			free(old_ret);
			free(element);
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

static void disconnect_dbus(DBusConnection *conn)
{
	// NOOP

	// Connections retrieved via dbus_bus_get shall not be destroyed,
	// these connections are shared.
}

#endif
