/**
 * @file   systemdunitdependency_probe.c
 * @brief  implementation of the systemdunitdependency_object
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
#include <probe/probe.h>

#include "probe/entcmp.h"
#include "systemdshared.h"
#include "common/list.h"
#include <string.h>
#include "systemdunitdependency_probe.h"

static void get_all_dependencies_by_unit(DBusConnection *conn, const char *unit, SEXP_t *item, struct oscap_htable *visited_units);

static char *get_property_by_unit_path(DBusConnection *conn, const char *unit_path, const char *property)
{
	DBusMessage *msg = NULL;
	DBusPendingCall *pending = NULL;
	char *ret = NULL;

	msg = dbus_message_new_method_call(
		"org.freedesktop.systemd1",
		unit_path,
		"org.freedesktop.DBus.Properties",
		"Get"
	);
	if (msg == NULL) {
		dD("Failed to create dbus_message via dbus_message_new_method_call!");
		goto cleanup;
	}

	DBusMessageIter args, value_iter;

	const char *interface = "org.freedesktop.systemd1.Unit";

	dbus_message_iter_init_append(msg, &args);
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface)) {
		dD("Failed to append interface '%s' string parameter to dbus message!", interface);
		goto cleanup;
	}
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &property)) {
		dD("Failed to append property '%s' string parameter to dbus message!", property);
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

	if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_VARIANT)
	{
		dD("Expected variant argument in reply. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
		goto cleanup;
	}

	dbus_message_iter_recurse(&args, &value_iter);
	ret = oval_dbus_value_to_string(&value_iter);

	dbus_message_unref(msg); msg = NULL;

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
};

static bool is_unit_name_a_target(const char *unit)
{
	const char *suffix = ".target";
	const size_t suffix_len = strlen(suffix);

	if (!unit)
		return false;

	const size_t len = strlen(unit);
	if (suffix_len >  len)
		return false;

	return strncmp(unit + len - suffix_len, suffix, suffix_len) == 0;
}

static int add_unit_dependency(const char *dependency, SEXP_t *item, struct oscap_htable *visited_units)
{
	if (oscap_htable_get(visited_units, dependency) != NULL) {
		return 1;
	}
	oscap_htable_add(visited_units, dependency, (void *) true);
	SEXP_t *se_dependency = SEXP_string_new(dependency, strlen(dependency));
	probe_item_ent_add(item, "dependency", NULL, se_dependency);
	SEXP_free(se_dependency);
	return 0;
}

static void process_unit_property(const char *property, DBusConnection *conn, const char *path, SEXP_t *item, struct oscap_htable *visited_units)
{
	char *values_s = get_property_by_unit_path(conn, path, property);
	if (values_s) {
		char **values = oscap_split(values_s, ", ");
		for (int i = 0; values[i] != NULL; ++i) {
			if (oscap_strcmp(values[i], "") == 0) {
				continue;
			}

			if (add_unit_dependency(values[i], item, visited_units) == 0) {
				get_all_dependencies_by_unit(conn, values[i], item, visited_units);
			}
		}
		free(values);
	}
	free(values_s);
}

static void get_all_dependencies_by_unit(DBusConnection *conn, const char *unit, SEXP_t *item, struct oscap_htable *visited_units)
{
	if (!unit || strcmp(unit, "(null)") == 0)
		return;

	// systemctl list-dependencies only recurses into target units
	if (!is_unit_name_a_target(unit))
		return;

	char *path = get_path_by_unit(conn, unit);

	process_unit_property("Requires", conn, path, item, visited_units);
	process_unit_property("Wants", conn, path, item, visited_units);

	free(path);
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

	SEXP_t *item = probe_item_create(OVAL_LINUX_SYSTEMDUNITDEPENDENCY, NULL,
					 "unit", OVAL_DATATYPE_SEXP, se_unit,
					 NULL);

	struct oscap_htable *visited_units = oscap_htable_new();
	get_all_dependencies_by_unit(vars->dbus_conn, unit, item, visited_units);
	oscap_htable_free(visited_units, NULL);

	probe_item_collect(vars->ctx, item);
	SEXP_free(se_unit);

	return 0;
}

int systemdunitdependency_probe_offline_mode_supported(void)
{
	return PROBE_OFFLINE_OWN;
}

int systemdunitdependency_probe_main(probe_ctx *ctx, void *probe_arg)
{
	SEXP_t *unit_entity, *probe_in;
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
	dbus_conn = oval_connect_dbus();

	if (dbus_conn == NULL) {
		dbus_error_free(&dbus_error);
		SEXP_t *msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_INFO, "DBus connection failed, could not identify systemd units.");
		probe_cobj_set_flag(probe_ctx_getresult(ctx), ctx->offline_mode == PROBE_OFFLINE_NONE ? SYSCHAR_FLAG_ERROR : SYSCHAR_FLAG_NOT_COLLECTED);
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		return 0;
	}

	unit_entity = probe_obj_getent(probe_in, "unit", 1);

	struct unit_callback_vars vars;

	vars.dbus_conn = dbus_conn;
	vars.ctx = ctx;
	vars.unit_entity = unit_entity;

	get_all_systemd_units(dbus_conn, unit_callback, &vars);

	SEXP_free(unit_entity);
	dbus_error_free(&dbus_error);
	oval_disconnect_dbus(dbus_conn);

	return 0;
}
