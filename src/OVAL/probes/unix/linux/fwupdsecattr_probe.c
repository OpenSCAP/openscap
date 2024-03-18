/**
 * @file   fwupdsecattr_probe.c
 * @brief  firmware security attribute probe
 * @author "Kate Hsuan" <hpa@redhat.com>
 *
 * 2022/11/28 hpa@redhat.com
 *  This probe can fetch firmware information from fwupd for testing and
 *  it is defined in 5.11.3.
 *
 */

/*
 * Copyright 2022 Red Hat Inc., Durham, North Carolina.
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
 *      "Kate Hsuan" <hpa@redhat.com>
 */

/*
 * fwupdsecattr probe:
 *
 *  fwupdsecattr_object(string stream-id)
 *
 *  fwupdsecattr_state(string security-attr)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex.h>
#include <dbus/dbus.h>

/* SEAP */
#include "_seap.h"
#include <probe-api.h>
#include <probe/probe.h>
#include <probe/option.h>
#include "probe/entcmp.h"
#include "common/debug_priv.h"

#include "oval_dbus.h"
#include "fwupdsecattr_probe.h"


static struct cachehed hsi_result_cache;

static void hsicache_callback(char *name, const uint32_t value)
{
	struct secattr_cache *entry;

	if (name == NULL)
		return;

	entry = (struct secattr_cache *)malloc(sizeof(struct secattr_cache));
	if (entry == NULL) { 
		dE("Out of memory when creating fwupd cache.");
		return;
	}
	entry->name = oscap_strdup(name);
	entry->hsi_result = value;
	dD("HSI cache add name: %s value: %d\n", entry->name, entry->hsi_result);
	LIST_INSERT_HEAD(&hsi_result_cache, entry, entries);
}

static uint32_t hsicache_get(const char *key)
{
	struct secattr_cache *next;

	LIST_FOREACH(next, &hsi_result_cache, entries) {
		dD("HSI search key: %s (name: %s value: %d)\n", key, next->name, next->hsi_result);
		if (!strncmp(next->name, key, strlen(next->name))) {
			return next->hsi_result;
		}
	}

	dW("HSI key not found: %s\n", key);
	return UINT32_MAX;
}

static int get_all_security_attributes(DBusConnection *conn, void(*callback)(char *name, const uint32_t value), void *cbarg)
{
	int ret = 1;
	DBusMessage *msg = NULL;
	DBusPendingCall *pending = NULL;
	char *property_name = NULL;
	char *appstream_name = NULL;
	uint32_t hsi_flags = 0;

	msg = dbus_message_new_method_call(
		"org.freedesktop.fwupd",
		"/",
		"org.freedesktop.fwupd",
		"GetHostSecurityAttrs"
	);
	if (msg == NULL) {
		dD("Failed to create dbus_message via dbus_message_new_method_call!");
		goto cleanup;
	}

	DBusMessageIter args, property_iter;
	_DBusBasicValue value;

	if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
		dD("Failed to send message via D-Bus!");
		goto cleanup;
	}
	if (pending == NULL) {
		dD("Invalid D-Bus pending call!");
		goto cleanup;
	}

	dbus_connection_flush(conn);
	dbus_message_unref(msg); msg = NULL;

	dbus_pending_call_block(pending);
	msg = dbus_pending_call_steal_reply(pending);
	if (msg == NULL) {
		dD("Failed to steal D-Bus pending call reply.");
		goto cleanup;
	}
	dbus_pending_call_unref(pending); pending = NULL;

	if (!dbus_message_iter_init(msg, &args)) {
		dD("Failed to initialize iterator over received D-Bus message.");
		goto cleanup;
	}

	if (dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_ERROR) {
		dbus_message_iter_get_basic(&args, &value);
		dW("Received an error from D-Bus (%s): %s", dbus_message_get_error_name(msg), value.str);
		goto cleanup;
	}

	if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_ARRAY && dbus_message_iter_get_element_type(&args) != DBUS_TYPE_DICT_ENTRY) {
		dD("Expected array of dict_entry argument in reply. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&args)));
		goto cleanup;
	}

	// The variant format is a{sv}
	dbus_message_iter_recurse(&args, &property_iter);
	do {
		DBusMessageIter array_entry, dict_entry, value_variant;

		/* Process array */
		if (dbus_message_iter_get_arg_type(&property_iter) != DBUS_TYPE_ARRAY) {
			dD("Expected property_iter is an array but receiving a %s", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&property_iter)));
			goto cleanup;
		}
		dbus_message_iter_recurse(&property_iter, &array_entry);

		if (dbus_message_iter_get_arg_type(&array_entry) != DBUS_TYPE_DICT_ENTRY) {
			dD("Expected array_entry is a dict but receiving a %s", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&array_entry)));
			goto cleanup;
		}

		do {
			dbus_message_iter_recurse(&array_entry, &dict_entry);

			if (dbus_message_iter_get_arg_type(&dict_entry) != DBUS_TYPE_STRING) {
				dD("Expected dict_entry is a string but receiving a %s", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&dict_entry)));
				goto cleanup;
			}

			dbus_message_iter_get_basic(&dict_entry, &value);
			property_name = oscap_strdup(value.str);
			dD("Element key: %s", property_name);

			if (dbus_message_iter_next(&dict_entry) == false) {
				dW("Expected another field in dict_entry.");
				free(property_name);
				goto cleanup;
			}

			if (dbus_message_iter_get_arg_type(&dict_entry) != DBUS_TYPE_VARIANT) {
				dW("Expected variant as value in dict_entry. Instead received: %s.", dbus_message_type_to_string(dbus_message_iter_get_arg_type(&dict_entry)));
				free(property_name);
				goto cleanup;
			}

			dbus_message_iter_recurse(&dict_entry, &value_variant);

			const int arg_type = dbus_message_iter_get_arg_type(&value_variant);

			switch (arg_type) {
				case DBUS_TYPE_UINT32:
					if(!strncmp(property_name, "HsiResult", strlen("HsiResult"))) {
						_DBusBasicValue hsiresult_value;
						dbus_message_iter_get_basic(&value_variant, &hsiresult_value);
						hsi_flags = hsiresult_value.u32;
					}
					break;
				case DBUS_TYPE_STRING:
					if(!strncmp(property_name, "AppstreamId", strlen("AppstreamId"))) {
						free(appstream_name);
						appstream_name = oval_dbus_value_to_string(&value_variant);
						dD("Element string: %s", appstream_name);
					}
					break;
			}
			free(property_name);
		} while (dbus_message_iter_next(&array_entry));
		callback(appstream_name, hsi_flags);
		free(appstream_name);
		appstream_name = NULL;
	} while (dbus_message_iter_next(&property_iter));

	dbus_message_unref(msg); msg = NULL;
	ret = 0;

cleanup:
	if (pending != NULL)
		dbus_pending_call_unref(pending);

	if (msg != NULL)
		dbus_message_unref(msg);

	free(appstream_name);

	return ret;
}

/**
 * fwupd_security_attr_result_to_string:
 * @result: security attribute result, e.g. %FWUPD_SECURITY_ATTR_RESULT_ENABLED
 *
 * Returns the printable string for the result enum.
 *
 * Returns: string, or %NULL
 **/
static const char *
fwupd_security_attr_result_to_string(FwupdSecurityAttrResult result)
{
	if (result == FWUPD_SECURITY_ATTR_RESULT_VALID)
		return "valid";
	if (result == FWUPD_SECURITY_ATTR_RESULT_NOT_VALID)
		return "not-valid";
	if (result == FWUPD_SECURITY_ATTR_RESULT_ENABLED)
		return "enabled";
	if (result == FWUPD_SECURITY_ATTR_RESULT_NOT_ENABLED)
		return "not-enabled";
	if (result == FWUPD_SECURITY_ATTR_RESULT_LOCKED)
		return "locked";
	if (result == FWUPD_SECURITY_ATTR_RESULT_NOT_LOCKED)
		return "not-locked";
	if (result == FWUPD_SECURITY_ATTR_RESULT_ENCRYPTED)
		return "encrypted";
	if (result == FWUPD_SECURITY_ATTR_RESULT_NOT_ENCRYPTED)
		return "not-encrypted";
	if (result == FWUPD_SECURITY_ATTR_RESULT_TAINTED)
		return "tainted";
	if (result == FWUPD_SECURITY_ATTR_RESULT_NOT_TAINTED)
		return "not-tainted";
	if (result == FWUPD_SECURITY_ATTR_RESULT_FOUND)
		return "found";
	if (result == FWUPD_SECURITY_ATTR_RESULT_NOT_FOUND)
		return "not-found";
	if (result == FWUPD_SECURITY_ATTR_RESULT_SUPPORTED)
		return "supported";
	if (result == FWUPD_SECURITY_ATTR_RESULT_NOT_SUPPORTED)
		return "not-supported";
	if (result == FWUPD_SECURITY_ATTR_RESULT_UNKNOWN) {
		dD("Got FWUPD_SECURITY_ATTR_RESULT_UNKNOWN\n");
		return "unknown";
	}
	dW("Unknown/invalid FwupdSecurityAttrResult value: %d\n", result);
	return "invalid-hsi-result";
}

int fwupdsecattr_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *val, *item, *ent, *probe_in;
	char *stream_id = NULL;
	const char *hsi_result_str;
	uint64_t hsi_result = UINT64_MAX;

	probe_in = probe_ctx_getobject(ctx);
	if (probe_in == NULL)
		return PROBE_ENOOBJ;

	ent = probe_obj_getent(probe_in, "stream_id", 1);
	if (ent == NULL)
		return PROBE_ENOENT;

	val = probe_ent_getval(ent);
	if (val == NULL) {
		SEXP_free(ent);
		return PROBE_ENOVAL;
	}

	stream_id = SEXP_string_cstr(val);
	SEXP_free(val);
	SEXP_free(ent);

	if (LIST_EMPTY(&hsi_result_cache)) {
		DBusError dbus_error;
		DBusConnection *dbus_conn;

		dbus_error_init(&dbus_error);
		dbus_conn = oval_connect_dbus();

		if (dbus_conn == NULL) {
			dbus_error_free(&dbus_error);
			SEXP_t *msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_INFO, "D-Bus connection failed, could not identify fwupd.");
			probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
			probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
			SEXP_free(msg);
			goto exit;
		}

		int res = get_all_security_attributes(dbus_conn, hsicache_callback, NULL);
		oval_disconnect_dbus(dbus_conn);

		if (res) {
			dbus_error_free(&dbus_error);
			SEXP_t *msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_INFO, "The fwupd service is not properly installed or configured.");
			probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
			probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
			SEXP_free(msg);
			goto exit;
		}
	}

	hsi_result = hsicache_get(stream_id);

	if (hsi_result == UINT32_MAX) {
		item = probe_item_create(OVAL_LINUX_FWUPDSECATTR, NULL,
					 "security_attr", OVAL_DATATYPE_STRING, "not-found",
					 NULL);
		probe_item_setstatus(item, SYSCHAR_STATUS_NOT_COLLECTED);
		probe_item_collect(ctx, item);
		goto exit;
	}

	hsi_result_str = fwupd_security_attr_result_to_string(hsi_result);
	item = probe_item_create(OVAL_LINUX_FWUPDSECATTR, NULL,
				 "security_attr", OVAL_DATATYPE_STRING, hsi_result_str,
				 NULL);
	probe_item_collect(ctx, item);

exit:
	free(stream_id);
	return 0;
}
