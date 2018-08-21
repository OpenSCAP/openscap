/*
 * Copyright 2018 Red Hat Inc., Durham, North Carolina.
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
 *      Jan Černý <jcerny@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <windows.h>
#include "debug_priv.h"
#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "probe/probe.h"
#include "registry_probe.h"
#include "oscap_helpers.h"

#define UNLIMITED_DEPTH -1

struct ent_info {
	SEXP_t *behaviors_ent;
	SEXP_t *hive_ent;
	SEXP_t *key_ent;
	SEXP_t *name_ent;
};

struct registry_key_info {
	const char *hive_str;
	const char *key_str;
	DWORD max_value_len;
	DWORD max_name_len;
	HKEY opened_key_handle;
	FILETIME last_write_time;
	int windows_view;
};

enum registry_windows_view {
	REGISTRY_WINDOWS_VIEW_64_BIT,
	REGISTRY_WINDOWS_VIEW_32_BIT
};

enum registry_recurse_direction {
	REGISTRY_RECURSE_DIRECTION_NONE,
	REGISTRY_RECURSE_DIRECTION_DOWN,
	REGISTRY_RECURSE_DIRECTION_UP
};

static const char *windows_view_to_str(int windows_view)
{
	if (windows_view == REGISTRY_WINDOWS_VIEW_64_BIT) {
		return "64_bit";
	}
	if (windows_view == REGISTRY_WINDOWS_VIEW_32_BIT) {
		return "32_bit";
	}
	return NULL;
}

static const char *registry_value_type_to_str(DWORD type)
{
	const char *type_str = NULL;
	switch (type) {
	case REG_BINARY:
		type_str = "reg_binary";
		break;
	case REG_DWORD:
		type_str = "reg_dword";
		break;
	case REG_DWORD_BIG_ENDIAN:
		type_str = "reg_dword_big_endian";
		break;
	case REG_EXPAND_SZ:
		type_str = "reg_expand_sz";
		break;
	case REG_LINK:
		type_str = "reg_link";
		break;
	case REG_MULTI_SZ:
		type_str = "reg_multi_sz";
		break;
	case REG_NONE:
		type_str = "reg_none";
		break;
	case REG_QWORD:
		type_str = "reg_qword";
		break;
	case REG_SZ:
		type_str = "reg_sz";
		break;
	case REG_RESOURCE_LIST: /* new in 5.11.1 */
		type_str = "reg_resource_list";
		break;
	case REG_FULL_RESOURCE_DESCRIPTOR: /* new in 5.11.1 */
		type_str = "reg_full_resource_descriptor";
		break;
	case REG_RESOURCE_REQUIREMENTS_LIST: /* new in 5.11.1 */
		type_str = "reg_resource_requirements_list";
		break;
	default:
		/*  The empty string is also allowed as a valid value to support empty elements associated with error conditions. */
		type_str = "";
	}
	return type_str;
}

static void report_finding(probe_ctx *ctx, const char *hive, const char *key, const char *name, FILETIME last_write_time, DWORD type, SEXP_t *value, int windows_view)
{
	SEXP_t *item = probe_item_create(OVAL_WINDOWS_REGISTRY, NULL,
		"hive", OVAL_DATATYPE_STRING, hive, NULL);

	if (key != NULL) {
		SEXP_t *key_sexp = SEXP_string_new(key, strlen(key));
		probe_item_ent_add(item, "key", NULL, key_sexp);
		SEXP_free(key_sexp);
	}

	if (name != NULL) {
		SEXP_t *name_sexp = SEXP_string_new(name, strlen(name));
		probe_item_ent_add(item, "name", NULL, name_sexp);
		SEXP_free(name_sexp);
	}

	/* convert FILETIME structure to a 64-bit unsigned integer */
	uint64_t last_write_time_uint64 = (((uint64_t) last_write_time.dwHighDateTime) << 32) + last_write_time.dwLowDateTime;
	SEXP_t *last_write_time_sexp = SEXP_number_newu_64(last_write_time_uint64);
	probe_item_ent_add(item, "last_write_time", NULL, last_write_time_sexp);
	SEXP_free(last_write_time_sexp);

	if (name != NULL) {
		const char *type_str = registry_value_type_to_str(type);
		SEXP_t *type_sexp = SEXP_string_new(type_str, strlen(type_str));
		probe_item_ent_add(item, "type", NULL, type_sexp);
		SEXP_free(type_sexp);

		probe_item_ent_add(item, "value", NULL, value);
	}

	const char *windows_view_str = windows_view_to_str(windows_view);
	SEXP_t *windows_view_sexp = SEXP_string_new(windows_view_str, strlen(windows_view_str));
	probe_item_ent_add(item, "windows_view", NULL, windows_view_sexp);
	SEXP_free(windows_view_sexp);

	probe_item_collect(ctx, item);
}

static HKEY get_hive_from_str(const char *hive_str)
{
	/* OVAL specification limits hive to HKEY_CLASSES_ROOT, HKEY_CURRENT_CONFIG,
	   HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, and HKEY_USERS. */
	if (strcmp("HKEY_CLASSES_ROOT", hive_str) == 0) {
		return HKEY_CLASSES_ROOT;
	}
	if (strcmp("HKEY_CURRENT_CONFIG", hive_str) == 0) {
		return HKEY_CURRENT_CONFIG;
	}
	if (strcmp("HKEY_CURRENT_USER", hive_str) == 0) {
		return HKEY_CURRENT_USER;
	}
	if (strcmp("HKEY_LOCAL_MACHINE", hive_str) == 0) {
		return HKEY_LOCAL_MACHINE;
	}
	if (strcmp("HKEY_USERS", hive_str) == 0) {
		return HKEY_USERS;
	}
	return NULL;
}

static char *bytes_to_hex(BYTE *bytes, DWORD length)
{
	/* Each byte will be repesented by 2 characters. */
	const size_t hex_str_len = 2 * length + 1; // +1 for null byte
	char *hex_str = malloc(hex_str_len);
	char *ptr = hex_str;
	for (DWORD i = 0; i < length; i++) {
		ptr += snprintf(ptr, hex_str_len, "%02X", bytes[i]);
	}
	*ptr = '\0';
	return hex_str;
}

static char *build_subkey_path(const char *key_path, const char *subkey_name)
{
	if (key_path == NULL) {
		return oscap_strdup(subkey_name);
	} else {
		return oscap_sprintf("%s\\%s", key_path, subkey_name);
	}
}

static REGSAM get_access_rights(int windows_view)
{
	REGSAM access_rights = KEY_READ;
	/* Set alternate registry view according to windows_view attribute of behaviors element */
	if (windows_view == REGISTRY_WINDOWS_VIEW_64_BIT) {
		access_rights |= KEY_WOW64_64KEY;
	}
	else {
		access_rights |= KEY_WOW64_32KEY;
	}
	return access_rights;
}

static struct ent_info *ent_info_new(SEXP_t *probe_in)
{
	struct ent_info *ei = malloc(sizeof(struct ent_info));
	ei->behaviors_ent = probe_obj_getent(probe_in, "behaviors", 1);
	ei->hive_ent = probe_obj_getent(probe_in, "hive", 1);
	ei->key_ent = probe_obj_getent(probe_in, "key", 1);
	ei->name_ent = probe_obj_getent(probe_in, "name", 1);
	return ei;
}

static void ent_info_free(struct ent_info *ei)
{
	SEXP_free(ei->behaviors_ent);
	SEXP_free(ei->hive_ent);
	SEXP_free(ei->key_ent);
	SEXP_free(ei->name_ent);
	free(ei);
}

static int registry_behaviors_get_max_depth(SEXP_t *behaviors_ent)
{
	int max_depth = UNLIMITED_DEPTH; // default behavior value
	if (behaviors_ent != NULL && probe_ent_attrexists(behaviors_ent, "max_depth")) {
		SEXP_t *max_depth_attrval = probe_ent_getattrval(behaviors_ent, "max_depth");
		char *max_depth_str = SEXP_string_cstr(max_depth_attrval);
		max_depth = strtol(max_depth_str, NULL, 10);
		free(max_depth_str);
		SEXP_free(max_depth_attrval);
	}
	return max_depth;
}

static int registry_behaviors_get_recurse_direction(SEXP_t *behaviors_ent)
{
	int recurse_direction = REGISTRY_RECURSE_DIRECTION_NONE; // default behavior value
	if (behaviors_ent != NULL && probe_ent_attrexists(behaviors_ent, "recurse_direction")) {
		SEXP_t *recurse_direction_attrval = probe_ent_getattrval(behaviors_ent, "recurse_direction");
		char *recurse_direction_str = SEXP_string_cstr(recurse_direction_attrval);
		if (strcmp(recurse_direction_str, "none") == 0) {
			recurse_direction = REGISTRY_RECURSE_DIRECTION_NONE;
		} else if (strcmp(recurse_direction_str, "down") == 0) {
			recurse_direction = REGISTRY_RECURSE_DIRECTION_DOWN;
		} else if (strcmp(recurse_direction_str, "up") == 0) {
			recurse_direction = REGISTRY_RECURSE_DIRECTION_UP;
		} else {
			dE("Invalid direction attribute value: %s", recurse_direction_str);
		}
		free(recurse_direction_str);
		SEXP_free(recurse_direction_attrval);
	}
	return recurse_direction;
}

static int registry_behaviors_get_windows_view(SEXP_t *behaviors_ent)
{
	int windows_view = REGISTRY_WINDOWS_VIEW_64_BIT; // default behavior value
	if (behaviors_ent != NULL && probe_ent_attrexists(behaviors_ent, "windows_view")) {
		SEXP_t *windows_view_attrval = probe_ent_getattrval(behaviors_ent, "windows_view");
		char *windows_view_str = SEXP_string_cstr(windows_view_attrval);
		if (strcmp(windows_view_str, "64_bit") == 0) {
			windows_view = REGISTRY_WINDOWS_VIEW_64_BIT;
		} else if (strcmp(windows_view_str, "32_bit") == 0) {
			windows_view = REGISTRY_WINDOWS_VIEW_32_BIT;
		} else {
			dE("Invalid windows_view attribute value: %s", windows_view_str);
		}
		free(windows_view_str);
		SEXP_free(windows_view_attrval);
	}
	return windows_view;
}


static void collect_registry_value(probe_ctx *ctx, struct registry_key_info *ki, DWORD index, SEXP_t *name_ent)
{
	DWORD name_buffer_len = ki->max_name_len + 1; // +1 for terminating null byte
	WCHAR *name_wstr = malloc(name_buffer_len * sizeof(WCHAR));
	name_wstr[0] = '\0';
	DWORD name_len = name_buffer_len;
	DWORD type;
	BYTE *value = malloc(ki->max_value_len);
	DWORD value_len = ki->max_value_len;

	int ret = RegEnumValueW(ki->opened_key_handle, index, name_wstr, &name_len, NULL, &type, value, &value_len);
	if (ret != ERROR_SUCCESS) {
		free(value);
		free(name_wstr);
		return;
	}

	char *name_str = oscap_windows_wstr_to_str(name_wstr);
	if (name_str != NULL && strlen(name_str) == 0) {
		free(name_str);
		name_str = NULL;
	}
	if (name_str == NULL) {
		name_str = strdup("(Default)");
	}

	SEXP_t *tmp = SEXP_string_new(name_str, strlen(name_str));
	if (probe_entobj_cmp(name_ent, tmp) != OVAL_RESULT_TRUE) {
		SEXP_free(tmp);
		free(value);
		free(name_wstr);
		free(name_str);
		return;
	}

	SEXP_t *value_sexp;
	char *str_buf;
	WCHAR *wstr_buf;

	switch (type) {
	case REG_DWORD:
		value_sexp = SEXP_number_newi_32(*(DWORD *)value);
		report_finding(ctx, ki->hive_str, ki->key_str, name_str, ki->last_write_time, type, value_sexp, ki->windows_view);
		SEXP_free(value_sexp);
		break;
	case REG_QWORD:
		value_sexp = SEXP_number_newi_64(*(DWORD64 *)value);
		report_finding(ctx, ki->hive_str, ki->key_str, name_str, ki->last_write_time, type, value_sexp, ki->windows_view);
		SEXP_free(value_sexp);
		break;
	case REG_SZ:
	case REG_EXPAND_SZ:
		str_buf = oscap_windows_wstr_to_str((WCHAR *)value);
		value_sexp = SEXP_string_new(str_buf, strlen(str_buf));
		free(str_buf);
		report_finding(ctx, ki->hive_str, ki->key_str, name_str, ki->last_write_time, type, value_sexp, ki->windows_view);
		SEXP_free(value_sexp);
		break;
	case REG_MULTI_SZ:
		/* A sequence of null-terminated strings, terminated by an empty string (\0). */
		/* Example: String1\0String2\0String3\0LastString\0\0 */
		wstr_buf = (WCHAR *)value;
		while (*wstr_buf != L'\0') {
			str_buf = oscap_windows_wstr_to_str(wstr_buf);
			value_sexp = SEXP_string_new(str_buf, strlen(str_buf));
			free(str_buf);
			report_finding(ctx, ki->hive_str, ki->key_str, name_str, ki->last_write_time, type, value_sexp, ki->windows_view);
			SEXP_free(value_sexp);
			wstr_buf += (wcslen(wstr_buf) + 1);
		}
		break;
	case REG_BINARY:
	default: /* treat unsupported types as binary blobs */
		str_buf = bytes_to_hex(value, value_len);
		value_sexp = SEXP_string_new(str_buf, strlen(str_buf));
		free(str_buf);
		report_finding(ctx, ki->hive_str, ki->key_str, name_str, ki->last_write_time, type, value_sexp, ki->windows_view);
		SEXP_free(value_sexp);
		break;
	}
	SEXP_free(tmp);
	free(name_str);
	free(value);
	free(name_wstr);
}

static int collect_registry_key(probe_ctx *ctx, const char *hive_str, const char *key_str, SEXP_t *name_ent, int windows_view)
{
	HKEY hive = get_hive_from_str(hive_str);
	HKEY opened_key_handle;
	REGSAM access_rights = get_access_rights(windows_view);
	WCHAR *key_wstr = oscap_windows_str_to_wstr(key_str);
	LONG err = RegOpenKeyExW(hive, key_wstr, 0, access_rights, &opened_key_handle);
	free(key_wstr);
	if (err != ERROR_SUCCESS) {
		char *error_message = oscap_windows_error_message(err);
		dD("Can't open registry key '%s': RegOpenKeyExW error: %s", key_str, error_message);
		free(error_message);
		return 1;
	}

	DWORD subkeys_count, max_subkey_len, values_count, max_name_len, max_value_len;
	FILETIME last_write_time;
	DWORD ret = RegQueryInfoKeyW(opened_key_handle, NULL, NULL, NULL,
		&subkeys_count, &max_subkey_len, NULL, &values_count, &max_name_len,
		&max_value_len, NULL, &last_write_time);

	SEXP_t *name_sexp_val = probe_ent_getval(name_ent);
	if (key_str == NULL) {
		report_finding(ctx, hive_str, NULL, NULL, last_write_time, REG_NONE, NULL, windows_view);
		RegCloseKey(opened_key_handle);
		return 0;
	}

	if (name_sexp_val == NULL) {
		/* Collect the key itself */
		report_finding(ctx, hive_str, key_str, NULL, last_write_time, REG_NONE, NULL, windows_view);
		RegCloseKey(opened_key_handle);
		return 0;
	}

	struct registry_key_info *ki = malloc(sizeof(struct registry_key_info));
	ki->hive_str = hive_str;
	ki->key_str = key_str;
	ki->max_value_len = max_value_len;
	ki->max_name_len = max_name_len;
	ki->opened_key_handle = opened_key_handle;
	ki->last_write_time = last_write_time;
	ki->windows_view = windows_view;

	/* Enumerate the key values */
	for (DWORD i = 0; i < values_count; i++) {
		collect_registry_value(ctx, ki, i, name_ent);
	}

	free(ki);

	RegCloseKey(opened_key_handle);
	return 0;
}

static int registry_recurse_down(probe_ctx *ctx, struct ent_info *ei, char *hive_str, char *key_str, int depth, int max_depth, oval_operation_t operation, int windows_view)
{
	HKEY opened_key_handle;
	HKEY hive = get_hive_from_str(hive_str);
	REGSAM access_rights = get_access_rights(windows_view);
	WCHAR *key_wstr = oscap_windows_str_to_wstr(key_str);
	LONG err = RegOpenKeyExW(hive, key_wstr, 0, access_rights, &opened_key_handle);
	free(key_wstr);
	if (err != ERROR_SUCCESS) {
		char *error_message = oscap_windows_error_message(err);
		dD("Can't open registry key '%s': RegOpenKeyExW error: %s", key_str, error_message);
		free(error_message);
		return 0;
	}

	DWORD subkeys_count, max_subkey_len;
	DWORD ret = RegQueryInfoKeyW(opened_key_handle, NULL, NULL, NULL,
		&subkeys_count, &max_subkey_len, NULL, NULL, NULL,
		NULL, NULL, NULL);

	if ((max_depth == UNLIMITED_DEPTH || depth < max_depth) && subkeys_count > 0) {
		DWORD subkey_buffer_len = max_subkey_len + 1; // +1 for terminating null character
		WCHAR *subkey = malloc(subkey_buffer_len * sizeof(WCHAR));
		for (DWORD i = 0; i < subkeys_count; i++) {
			DWORD subkey_len = subkey_buffer_len;
			ret = RegEnumKeyExW(opened_key_handle, i, subkey, &subkey_len, NULL, NULL, NULL, NULL);
			if (ret == ERROR_SUCCESS) {
				char *subkey_str = oscap_windows_wstr_to_str(subkey);
				char *subkey_path = build_subkey_path(key_str, subkey_str);
				registry_recurse_down(ctx, ei, hive_str, subkey_path, depth + 1, max_depth, operation, windows_view);
				free(subkey_path);
				free(subkey_str);
			}
		}
		free(subkey);
	}

	if (key_str != NULL) {
		SEXP_t *tmp = SEXP_string_new(key_str, strlen(key_str));
		if (operation == OVAL_OPERATION_EQUALS || operation == OVAL_OPERATION_CASE_INSENSITIVE_EQUALS || probe_entobj_cmp(ei->key_ent, tmp) == OVAL_RESULT_TRUE) {
			collect_registry_key(ctx, hive_str, key_str, ei->name_ent, windows_view);
		}
	}

	RegCloseKey(opened_key_handle);
	return 0;

}

static int registry_recurse_up(probe_ctx *ctx, struct ent_info *ei, char *hive_str, char *key_str, int max_depth, int windows_view)
{
	int depth = 0;
	do {
		int ret = collect_registry_key(ctx, hive_str, key_str, ei->name_ent, windows_view);
		if (ret != 0) {
			break;
		}
		char *delimiter = strrchr(key_str, '\\');
		if (delimiter == NULL) {
			collect_registry_key(ctx, hive_str, NULL, ei->name_ent, windows_view);
			break;
		}
		*delimiter = '\0';
		depth++;
	} while (max_depth == UNLIMITED_DEPTH || depth < max_depth);
	return 0;
}

int registry_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in = probe_ctx_getobject(ctx);
	struct ent_info *ei = ent_info_new(probe_in);

	SEXP_t *hive_val = probe_ent_getval(ei->hive_ent);
	char *hive_str = SEXP_string_cstr(hive_val);
	SEXP_free(hive_val);

	/* Get behaviors or their default values */
	int max_depth = registry_behaviors_get_max_depth(ei->behaviors_ent);
	int recurse_direction = registry_behaviors_get_recurse_direction(ei->behaviors_ent);
	int windows_view = registry_behaviors_get_windows_view(ei->behaviors_ent);

	oval_operation_t key_operation = probe_ent_getoperation(ei->key_ent, OVAL_OPERATION_EQUALS);
	if (key_operation == OVAL_OPERATION_EQUALS || key_operation == OVAL_OPERATION_CASE_INSENSITIVE_EQUALS) {
		SEXP_t *key_val = probe_ent_getval(ei->key_ent);
		char *key_str = SEXP_string_cstr(key_val);
		SEXP_free(key_val);
		if (recurse_direction == REGISTRY_RECURSE_DIRECTION_NONE || max_depth == 0) {
			/* max_depth == 0 means no recursion no matter what recurse_direction was set in behaviors */
			collect_registry_key(ctx, hive_str, key_str, ei->name_ent, windows_view);
		} else if (recurse_direction == REGISTRY_RECURSE_DIRECTION_DOWN) {
			HKEY hive = get_hive_from_str(hive_str);
			registry_recurse_down(ctx, ei, hive_str, key_str, 0, max_depth, key_operation, windows_view);
		} else if (recurse_direction == REGISTRY_RECURSE_DIRECTION_UP) {
			registry_recurse_up(ctx, ei, hive_str, key_str, max_depth, windows_view);
		}
		free(key_str);
	} else { /* pattern match, not equal, ... */
		HKEY hive = get_hive_from_str(hive_str);
		registry_recurse_down(ctx, ei, hive_str, NULL, 0, UNLIMITED_DEPTH, key_operation, windows_view);
	}

	free(hive_str);
	ent_info_free(ei);

	return 0;
}
