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

#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include "common/debug_priv.h"
#include "common/list.h"
#include "oscap_helpers.h"
#include "oval_dbus.h"


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
	dD("LoadUnit: %s", unit);

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
		dD("Expected object path argument in reply. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
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

static int get_systemd_units(DBusConnection *conn, const char *method, bool unit_files,
		struct oscap_htable *seen, int(*callback)(const char *, void *), void *cbarg)
{
	DBusMessage *msg = NULL;
	DBusPendingCall *pending = NULL;
	char ret = 1;

	msg = dbus_message_new_method_call(
		"org.freedesktop.systemd1",
		"/org/freedesktop/systemd1",
		"org.freedesktop.systemd1.Manager",
		method
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
	while (dbus_message_iter_get_arg_type(&unit_iter) != DBUS_TYPE_INVALID) {
		if (dbus_message_iter_get_arg_type(&unit_iter) != DBUS_TYPE_STRUCT) {
			dD("Expected unit struct as elements in returned array. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&unit_iter)));
			goto cleanup;
		}

		DBusMessageIter unit_full_path_and_name;
		dbus_message_iter_recurse(&unit_iter, &unit_full_path_and_name);

		if (dbus_message_iter_get_arg_type(&unit_full_path_and_name) != DBUS_TYPE_STRING) {
			dD("Expected string as the first element in the unit struct. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&unit_full_path_and_name)));
			goto cleanup;
		}

		_DBusBasicValue value;
		dbus_message_iter_get_basic(&unit_full_path_and_name, &value);
		/* ListUnitFiles returns paths, whereas ListUnits returns unit names.
		 * Never turn a template into a different, nonexistent unit by removing
		 * its @. Actual instances are returned by ListUnits or have their own
		 * unit file (for example, an enabled instance symlink).
		 */
		char *unit_path = oscap_strdup(value.str);
		const char *unit_name = unit_files ? basename(unit_path) : unit_path;
		const char *instance = strchr(unit_name, '@');
		bool is_template = instance != NULL && instance[1] == '.';
		if (!is_template && oscap_htable_get(seen, unit_name) == NULL) {
			oscap_htable_add(seen, unit_name, (void *) true);
			int cbret = callback(unit_name, cbarg);
			if (cbret != 0) {
				free(unit_path);
				goto cleanup;
			}
		}
		free(unit_path);
		dbus_message_iter_next(&unit_iter);
	}

	dbus_message_unref(msg); msg = NULL;

	ret = 0;

cleanup:
	if (pending != NULL)
		dbus_pending_call_unref(pending);

	if (msg != NULL)
		dbus_message_unref(msg);

	return ret;
}

static int get_all_systemd_units(DBusConnection *conn, int(*callback)(const char *, void *), void *cbarg)
{
	struct oscap_htable *seen = oscap_htable_new();
	/* Keep installed units even when disabled and not currently loaded. Add
	 * loaded units to include template instances and transient units too. */
	int ret = get_systemd_units(conn, "ListUnitFiles", true, seen, callback, cbarg);
	if (ret == 0)
		ret = get_systemd_units(conn, "ListUnits", false, seen, callback, cbarg);
	oscap_htable_free(seen, NULL);
	return ret;
}

#endif
