/**
 * @file   systemdunitdependency.c
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
#include "common/list.h"
#include <string.h>

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
	SEXP_t *item = probe_item_create(OVAL_LINUX_SYSTEMDUNITDEPENDENCY, ...);
	probe_item_collect(ctx, item);
*/
        return (0);
}

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

static void get_all_dependencies_by_unit(DBusConnection *conn, const char *unit, struct oscap_htable *receiver, bool include_requires, bool include_wants)
{
	if (!unit || strcmp(unit, "(null)") == 0)
		return;

	// systemctl list-dependencies only recurses into target units
	if (!is_unit_name_a_target(unit))
		return;

	char *path = get_path_by_unit(conn, unit);

	if (include_requires) {
		char *requires_s = get_property_by_unit_path(conn, path, "Requires");
		char **requires = oscap_split(requires_s, ", ");
		for (int i = 0; requires[i] != NULL; ++i) {
			if (oscap_strcmp(requires[i], "") == 0)
				continue;

			if (oscap_htable_add(receiver, requires[i], NULL)) {
				get_all_dependencies_by_unit(conn, requires[i], receiver, include_requires, include_wants);
			}
		}
		oscap_free(requires);
		oscap_free(requires_s);
	}

	if (include_wants) {
		char *wants_s = get_property_by_unit_path(conn, path, "Wants");
		char **wants = oscap_split(wants_s, ", ");
		for (int i = 0; wants[i] != NULL; ++i) {
			if (oscap_strcmp(wants[i], "") == 0)
				continue;

			if (oscap_htable_add(receiver, wants[i], NULL)) {
				get_all_dependencies_by_unit(conn, wants[i], receiver, include_requires, include_wants);
			}
		}
		oscap_free(wants);
		oscap_free(wants_s);
	}

	oscap_free(path);
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

	{
		struct oscap_htable *table = oscap_htable_new();
		get_all_dependencies_by_unit(conn, unit, table, true, true);
		oscap_htable_free0(table);
	}

cleanup:
	dbus_error_free(&err);

	if (path != NULL)
		oscap_free(path);

	disconnect_dbus(conn);

	return ret;
}
