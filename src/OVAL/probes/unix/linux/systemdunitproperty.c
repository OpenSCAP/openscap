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
#include <dbus/dbus.h>
#include "common/debug_priv.h"

static char *get_path_by_unit(DBusConnection *conn, const char *unit)
{
	DBusMessage *msg = NULL;
	DBusPendingCall *pending = NULL;
	DBusBasicValue path;
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
		dI("Failed to create dbus_message via dbus_message_new_method_call!\n");
		goto cleanup;
	}

	DBusMessageIter args;

	dbus_message_iter_init_append(msg, &args);
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &unit)) {
		dI("Failed to append unit '%s' string parameter to dbus message!\n", unit);
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

	if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_OBJECT_PATH) {
		dI("Expected string argument in reply. Instead received: %s.\n", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
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
