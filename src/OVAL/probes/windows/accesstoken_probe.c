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

#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <Ntsecapi.h>
#include <ntstatus.h>
#include <LM.h>
#include <winnt.h>
#include <stdbool.h>
#include "debug_priv.h"
#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "probe/probe.h"
#include "accesstoken_probe.h"
#include "list.h"

#define LAST_WELL_KNOWN_SID WinAuthenticationFreshKeyAuthSid
#define OVAL_PRIVILEGES_COUNT 44

WCHAR *privileges_texts[OVAL_PRIVILEGES_COUNT] = {
	L"SeAssignPrimaryTokenPrivilege",
	L"SeAuditPrivilege",
	L"SeBackupPrivilege",
	L"SeChangeNotifyPrivilege",
	L"SeCreateGlobalPrivilege",
	L"SeCreatePagefilePrivilege",
	L"SeCreatePermanentPrivilege",
	L"SeCreateSymbolicLinkPrivilege",
	L"SeCreateTokenPrivilege",
	L"SeDebugPrivilege",
	L"SeEnableDelegationPrivilege",
	L"SeImpersonatePrivilege",
	L"SeIncreaseBasePriorityPrivilege",
	L"SeIncreaseQuotaPrivilege",
	L"SeIncreaseWorkingSetPrivilege",
	L"SeLoadDriverPrivilege",
	L"SeLockMemoryPrivilege",
	L"SeMachineAccountPrivilege",
	L"SeManageVolumePrivilege",
	L"SeProfileSingleProcessPrivilege",
	L"SeRelabelPrivilege",
	L"SeRemoteShutdownPrivilege",
	L"SeRestorePrivilege",
	L"SeSecurityPrivilege",
	L"SeShutdownPrivilege",
	L"SeSyncAgentPrivilege",
	L"SeSystemEnvironmentPrivilege",
	L"SeSystemProfilePrivilege",
	L"SeSystemtimePrivilege",
	L"SeTakeOwnershipPrivilege",
	L"SeTcbPrivilege",
	L"SeTimeZonePrivilege",
	L"SeUndockPrivilege",
	L"SeUnsolicitedInputPrivilege",
	L"SeBatchLogonRight",
	L"SeInteractiveLogonRight"
	L"SeNetworkLogonRight",
	L"SeRemoteInteractiveLogonRight",
	L"SeServiceLogonRight",
	L"SeDenyBatchLogonRight",
	L"SeDenyInteractiveLogonRight",
	L"SeDenyNetworkLogonRight",
	L"SeDenyRemoteInteractiveLogonRight",
	L"SeDenyServiceLogonRight",
	L"SeTrustedCredManAccessPrivilege"
};

static int get_all_local_users(struct oscap_list *list)
{
	NET_API_STATUS status;

	USER_INFO_0 *buffer = NULL;
	DWORD preffered_max_len = MAX_PREFERRED_LENGTH;
	DWORD entries_read = 0;
	DWORD total_entries = 0;
	DWORD resume_handle = 0;
	status = NetUserEnum(NULL, 0, 0, (LPBYTE *)&buffer, preffered_max_len, &entries_read, &total_entries, &resume_handle);
	if (status != NERR_Success) {
		dD("NetUserEnum failed: %d", status);
		return 1;
	}
	for (DWORD i = 0; i < entries_read; i++) {
		WCHAR *user_name = buffer[i].usri0_name;
		oscap_list_add(list, wcsdup(user_name));
	}
	NetApiBufferFree(buffer);
	return 0;
}


static int get_user_local_groups(WCHAR *user, struct oscap_list *list)
{
	NET_API_STATUS status;

	LOCALGROUP_USERS_INFO_0 *buffer = NULL;
	DWORD preffered_max_len = MAX_PREFERRED_LENGTH;
	DWORD entries_read = 0;
	DWORD total_entries = 0;
	/*
	 * LG_INCLUDE_INDIRECT means the function also returns the names of
	 * the local groups in which the user is indirectly a member (that is,
	 * the user has membership in a global group that is itself a member
	 * of one or more local groups).
	 */
	status = NetUserGetLocalGroups(NULL, user, 0, LG_INCLUDE_INDIRECT, (LPBYTE *)&buffer, preffered_max_len, &entries_read, &total_entries);
	if (status != NERR_Success) {
		dD("NetUserGetLocalGroups failed: %d", status);
		return 1;
	}
	for (DWORD i = 0; i < entries_read; i++) {
		WCHAR *group_name = buffer[i].lgrui0_name;
		oscap_list_add(list, wcsdup(group_name));
	}
	NetApiBufferFree(buffer);
	return 0;
}

static int get_user_global_groups(WCHAR *user, struct oscap_list *list)
{
	NET_API_STATUS status;

	GROUP_USERS_INFO_0 *buffer = NULL;
	DWORD preffered_max_len = MAX_PREFERRED_LENGTH;
	DWORD entries_read = 0;
	DWORD total_entries = 0;
	status = NetUserGetGroups(NULL, user, 0, (LPBYTE *)&buffer, preffered_max_len, &entries_read, &total_entries);
	if (status != NERR_Success) {
		dD("NetUserGetGroups failed: %d", status);
		return 1;
	}
	for (DWORD i = 0; i < entries_read; i++) {
		WCHAR *group_name = buffer[i].grui0_name;
		oscap_list_add(list, wcsdup(group_name));
	}
	NetApiBufferFree(buffer);
	return 0;
}

static int get_all_local_groups(struct oscap_list *list)
{
	NET_API_STATUS status;
	LOCALGROUP_INFO_0 *buffer = NULL;
	DWORD preffered_max_len = MAX_PREFERRED_LENGTH;
	DWORD entries_read = 0;
	DWORD total_entries = 0;
	DWORD resume_handle = 0;
	status = NetLocalGroupEnum(NULL, 0, (LPBYTE *)&buffer, preffered_max_len, &entries_read, &total_entries, &resume_handle);
	if (status != NERR_Success) {
		dD("NetLocalGroupEnum failed: %d", status);
		return 1;
	}
	for (DWORD i = 0; i < entries_read; i++) {
		WCHAR *group_name = buffer[i].lgrpi0_name;
		oscap_list_add(list, wcsdup(group_name));
	}
	NetApiBufferFree(buffer);
	return 0;
}

static int get_all_global_groups(struct oscap_list *list)
{
	NET_API_STATUS status;
	GROUP_INFO_0 *buffer = NULL;
	DWORD preffered_max_len = MAX_PREFERRED_LENGTH;
	DWORD entries_read = 0;
	DWORD total_entries = 0;
	DWORD resume_handle = 0;
	status = NetGroupEnum(NULL, 0, (LPBYTE *)&buffer, preffered_max_len, &entries_read, &total_entries, &resume_handle);
	if (status != NERR_Success) {
		dD("NetGroupEnum failed: %d", status);
		return 1;
	}
	for (DWORD i = 0; i < entries_read; i++) {
		WCHAR *group_name = buffer[i].grpi0_name;
		oscap_list_add(list, wcsdup(group_name));
	}
	NetApiBufferFree(buffer);
	return 0;
}

static int get_local_group_members(WCHAR *group_name, struct oscap_list *list)
{
	NET_API_STATUS status;
	LOCALGROUP_MEMBERS_INFO_1 *buffer = NULL;
	DWORD preffered_max_len = MAX_PREFERRED_LENGTH;
	DWORD entries_read = 0;
	DWORD total_entries = 0;
	DWORD resume_handle = 0;
	status = NetLocalGroupGetMembers(NULL, group_name, 1, (LPBYTE *) &buffer, preffered_max_len, &entries_read, &total_entries, &resume_handle);
	if (status != NERR_Success) {
		dD("NetLocalGroupGetMembers failed: %d", status);
		return 1;
	}
	for (DWORD i = 0; i < entries_read; i++) {
		WCHAR *member_name = buffer[i].lgrmi1_name;
		oscap_list_add(list, wcsdup(member_name));
	}
	NetApiBufferFree(buffer);
	return 0;
}

static int get_global_group_members(WCHAR *group_name, struct oscap_list *list)
{
	NET_API_STATUS status;
	GROUP_USERS_INFO_0 *buffer = NULL;
	DWORD preffered_max_len = MAX_PREFERRED_LENGTH;
	DWORD entries_read = 0;
	DWORD total_entries = 0;
	DWORD resume_handle = 0;
	status = NetGroupGetUsers(NULL, group_name, 0, (LPBYTE *)&buffer, preffered_max_len, &entries_read, &total_entries, &resume_handle);
	if (status != NERR_Success) {
		dD("NetGroupGetUsers failed: %d", status);
		return 1;
	}
	for (DWORD i = 0; i < entries_read; i++) {
		WCHAR *member_name = buffer[i].grui0_name;
		oscap_list_add(list, wcsdup(member_name));
	}
	NetApiBufferFree(buffer);
	return 0;
}

static WCHAR *sid_to_trustee_name(SID *sid)
{
	DWORD name_len = 0;
	DWORD domain_name_len = 0;
	SID_NAME_USE sid_type;
	LookupAccountSidW(NULL, sid, NULL, &name_len, NULL, &domain_name_len, &sid_type);
	WCHAR *name = malloc(name_len * sizeof(WCHAR));
	WCHAR *domain_name = malloc(domain_name_len * sizeof(WCHAR));
	if (!LookupAccountSidW(NULL, sid, name, &name_len, domain_name, &domain_name_len, &sid_type)) {
		free(name);
		free(domain_name);
		DWORD err = GetLastError();
		char *error_message = oscap_windows_error_message(err);
		dD("LookupAccountSidW failed: %s",  error_message);
		free(error_message);
		return NULL;
	}

	if (*domain_name == L'\0') {
		free(domain_name);
		return name;
	} else {
		DWORD result_len = name_len + domain_name_len + 2;
		WCHAR *result = malloc(result_len * sizeof(WCHAR));
		swprintf(result, result_len, L"%s\\%s", domain_name, name);
		free(domain_name);
		free(name);
		return result;
	}
	return NULL;
}

static int get_all_well_known_sids(struct oscap_list *trustees_list)
{
	for (int well_known_sid = 0; well_known_sid < LAST_WELL_KNOWN_SID; well_known_sid++) {
		DWORD sid_len = SECURITY_MAX_SID_SIZE;
		SID *sid = malloc(sid_len);
		if (!CreateWellKnownSid(well_known_sid, NULL, sid, &sid_len)) {
			free(sid);
			continue;
		}
		WCHAR *well_known_trustee_name = sid_to_trustee_name(sid);
		if (well_known_trustee_name != NULL) {
			oscap_list_add(trustees_list, well_known_trustee_name);
		}
		free(sid);
	}
	return 0;
}

static bool _members_cmp_func(WCHAR *s1, WCHAR *s2)
{
	return wcscmp(s1, s2) == 0;
}

static void expand_group(WCHAR *group, struct oscap_list *output_list, int (*get_group_members)(WCHAR *group_name, struct oscap_list *list))
{
	struct oscap_list *members_list = oscap_list_new();
	get_group_members(group, members_list);
	struct oscap_iterator *members_it = oscap_iterator_new(members_list);
	while (oscap_iterator_has_more(members_it)) {
		WCHAR *member = oscap_iterator_next(members_it);
		/* TODO: implement this using a HashSet instead of linked list */
		if (!oscap_list_contains(output_list, member, _members_cmp_func)) {
			oscap_list_add(output_list, wcsdup(member));
		}
	}
	oscap_iterator_free(members_it);
	oscap_list_free(members_list, free);
}

static int get_all_trustee_names(struct oscap_list *trustees_list)
{
	get_all_well_known_sids(trustees_list);
	get_all_local_users(trustees_list);

	struct oscap_list *local_groups_list = oscap_list_new();
	get_all_local_groups(local_groups_list);

	/* Find members of each local group */
	struct oscap_iterator *local_groups_it = oscap_iterator_new(local_groups_list);
	while (oscap_iterator_has_more(local_groups_it)) {
		WCHAR *group = oscap_iterator_next(local_groups_it);
		oscap_list_add(trustees_list, wcsdup(group));
		expand_group(group, trustees_list, get_local_group_members);
	}
	oscap_iterator_free(local_groups_it);
	oscap_list_free(local_groups_list, free);

	struct oscap_list *global_groups_list = oscap_list_new();
	get_all_global_groups(global_groups_list);

	/* Find members of each global group */
	struct oscap_iterator *global_groups_it = oscap_iterator_new(global_groups_list);
	while (oscap_iterator_has_more(global_groups_it)) {
		WCHAR *group = oscap_iterator_next(global_groups_it);
		oscap_list_add(trustees_list, wcsdup(group));
		expand_group(group, trustees_list, get_global_group_members);
	}
	oscap_iterator_free(global_groups_it);
	oscap_list_free(global_groups_list, free);

	return 0;
}

static bool accesstoken_behaviors_get_include_group(SEXP_t *behaviors_ent)
{
	bool include_group = true; // Default value of include_group behaviors
	if (behaviors_ent != NULL && probe_ent_attrexists(behaviors_ent, "include_group")) {
		SEXP_t *include_group_attrval = probe_ent_getattrval(behaviors_ent, "include_group");
		include_group = SEXP_string_getb(include_group_attrval);
		SEXP_free(include_group_attrval);
	}
	return include_group;
}

static bool accesstoken_behaviors_get_resolve_group(SEXP_t *behaviors_ent)
{
	bool resolve_group = false; // Default value of resolve_group behaviors
	if (behaviors_ent != NULL && probe_ent_attrexists(behaviors_ent, "resolve_group")) {
		SEXP_t *resolve_group_attrval = probe_ent_getattrval(behaviors_ent, "resolve_group");
		resolve_group = SEXP_string_getb(resolve_group_attrval);
		SEXP_free(resolve_group_attrval);
	}
	return resolve_group;
}

static int collect_access_rights(probe_ctx *ctx, WCHAR *security_principle, bool include_group, bool resolve_group)
{
	char *security_principle_str = oscap_windows_wstr_to_str(security_principle);

	LSA_OBJECT_ATTRIBUTES object_attributes;
	ZeroMemory(&object_attributes, sizeof(LSA_OBJECT_ATTRIBUTES));
	LSA_HANDLE lsa_policy_handle;

	NTSTATUS status = LsaOpenPolicy(NULL, &object_attributes, POLICY_LOOKUP_NAMES, &lsa_policy_handle);
	if (status != STATUS_SUCCESS) {
		DWORD err = LsaNtStatusToWinError(status);
		char *error_message = oscap_windows_error_message(err);
		dD("LsaOpenPolicy failed for principle '%s': %s", security_principle_str, error_message);
		free(error_message);
		free(security_principle_str);
		return 1;
	}

	/* Convert the value of the security_principle element to a SID. */
	DWORD sid_len = 0;
	DWORD domain_name_len = 0;
	SID_NAME_USE sid_type;
	LookupAccountNameW(NULL, security_principle, NULL, &sid_len, NULL, &domain_name_len, &sid_type);
	SID *sid = malloc(sid_len);
	WCHAR *domain_name = malloc(domain_name_len * sizeof(WCHAR));
	if (!LookupAccountNameW(NULL, security_principle, sid, &sid_len, domain_name, &domain_name_len, &sid_type)) {
		DWORD err = GetLastError();
		char *error_message = oscap_windows_error_message(err);
		dD("LookupAccountNameW failed for '%s': %s", security_principle_str, error_message);
		free(error_message);
		free(security_principle_str);
		free(sid);
		free(domain_name);
		return 1;
	}

	/* Is it a group? */
	if (sid_type == SidTypeGroup || sid_type == SidTypeWellKnownGroup || sid_type == SidTypeAlias) {
		if (resolve_group) {
			struct oscap_list *group_members_list = oscap_list_new();
			get_local_group_members(security_principle, group_members_list);
			get_global_group_members(security_principle, group_members_list);
			struct oscap_iterator *group_members_it = oscap_iterator_new(group_members_list);
			while (oscap_iterator_has_more(group_members_it)) {
				WCHAR *group_member = oscap_iterator_next(group_members_it);
				collect_access_rights(ctx, group_member, include_group, resolve_group);
			}
			oscap_iterator_free(group_members_it);
			oscap_list_free(group_members_list, free);
		}
		if (!include_group) {
			free(sid);
			free(domain_name);
			free(security_principle_str);
			return 0;
		}
	}

	/* Users and groups can inherit their privileges from their parents */
	struct oscap_list *every_rights_sources = oscap_list_new();
	oscap_list_add(every_rights_sources, wcsdup(security_principle));
	get_user_local_groups(security_principle, every_rights_sources);
	get_user_global_groups(security_principle, every_rights_sources);

	/* Iterate over the items */
	bool privileges_enabled[OVAL_PRIVILEGES_COUNT] = { false };
	struct oscap_iterator *it = oscap_iterator_new(every_rights_sources);
	while (oscap_iterator_has_more(it)) {
		WCHAR *account_name = oscap_iterator_next(it);

		DWORD account_sid_len = 0;
		DWORD account_domain_name_len = 0;
		SID_NAME_USE account_sid_type;
		LookupAccountNameW(NULL, account_name, NULL, &account_sid_len, NULL, &account_domain_name_len, &account_sid_type);
		SID *account_sid = malloc(account_sid_len);
		WCHAR *account_domain_name = malloc(account_domain_name_len * sizeof(WCHAR));
		if (!LookupAccountNameW(NULL, account_name, account_sid, &account_sid_len, account_domain_name, &account_domain_name_len, &account_sid_type)) {
			free(account_sid);
			free(account_domain_name);
			DWORD err = GetLastError();
			char *error_message = oscap_windows_error_message(err);
			dD("LookupAccountNameW failed for '%s': %s", security_principle_str, error_message);
			free(error_message);
			free(security_principle_str);
			return 1;
		}

		LSA_UNICODE_STRING *granted_rights = NULL;
		ULONG granted_rights_count = 0;
		status = LsaEnumerateAccountRights(lsa_policy_handle, account_sid, &granted_rights, &granted_rights_count);
		if (status != STATUS_SUCCESS) {
			free(account_sid);
			free(account_domain_name);
			DWORD err = LsaNtStatusToWinError(status);
			char *error_message = oscap_windows_error_message(err);
			dD("LsaEnumerateAccountRights failed for '%s': %s", security_principle_str, error_message);
			free(error_message);
			/* We should not exit here, because when LsaEnumerateAccountRights
			* failed it can mean that the entity simply doesn't have any specific
			* privileges, it only inhertis privileges form its parent group(s).
			*/
			continue;
		}

		for (int i = 0; i < OVAL_PRIVILEGES_COUNT; i++) {
			if (!privileges_enabled[i]) {
				for (ULONG j = 0; j < granted_rights_count; j++) {
					if (wcscmp(granted_rights[j].Buffer, privileges_texts[i]) == 0) {
						privileges_enabled[i] = true;
						break;
					}
				}
			}
		}
		LsaFreeMemory(granted_rights);
		free(account_sid);
		free(account_domain_name);

	}
	oscap_iterator_free(it);
	oscap_list_free(every_rights_sources, free);

	/* Collect the OVAL item */
	SEXP_t *item = probe_item_create(OVAL_WINDOWS_ACCESS_TOKEN, NULL,
		"security_principle", OVAL_DATATYPE_STRING, strdup(security_principle_str), NULL);
	for (int i = 0; i < OVAL_PRIVILEGES_COUNT; i++) {
		char *privilege_name = oscap_windows_wstr_to_str(privileges_texts[i]);
		/* Convert the element name to lowercase */
		for (char *p = privilege_name; *p; p++) {
			*p = tolower(*p);
		}
		SEXP_t *privilege_value = SEXP_number_newb(privileges_enabled[i]);
		probe_item_ent_add(item, privilege_name, NULL, privilege_value);
		free(privilege_name);
		SEXP_free(privilege_value);
	}
	probe_item_collect(ctx, item);
	free(security_principle_str);

	return 0;
}

int accesstoken_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in = probe_ctx_getobject(ctx);
	SEXP_t *behaviors_ent = probe_obj_getent(probe_in, "behaviors", 1);
	SEXP_t *security_principle_ent = probe_obj_getent(probe_in, "security_principle", 1);
	SEXP_t *security_principle_val = probe_ent_getval(security_principle_ent);

	bool include_group = accesstoken_behaviors_get_include_group(behaviors_ent);
	bool resolve_group = accesstoken_behaviors_get_resolve_group(behaviors_ent);

	oval_operation_t operation = probe_ent_getoperation(security_principle_ent, OVAL_OPERATION_EQUALS);
	if (operation == OVAL_OPERATION_EQUALS) {
		char *security_principle_str = SEXP_string_cstr(security_principle_val);
		WCHAR *security_principle_wstr = oscap_windows_str_to_wstr(security_principle_str);
		collect_access_rights(ctx, security_principle_wstr, include_group, resolve_group);
		free(security_principle_str);
		free(security_principle_wstr);
	} else {
		struct oscap_list *trustees_list = oscap_list_new();
		get_all_trustee_names(trustees_list);

		struct oscap_iterator *it = oscap_iterator_new(trustees_list);
		while (oscap_iterator_has_more(it)) {
			WCHAR *trustee_wstr = oscap_iterator_next(it);
			char *trustee_str = oscap_windows_wstr_to_str(trustee_wstr);
			SEXP_t *tmp = SEXP_string_new(trustee_str, strlen(trustee_str));
			if (probe_entobj_cmp(security_principle_ent, tmp) == OVAL_RESULT_TRUE) {
				collect_access_rights(ctx, trustee_wstr, include_group, resolve_group);
			}
			free(trustee_str);
			SEXP_free(tmp);
		}
		oscap_iterator_free(it);
		oscap_list_free(trustees_list, free);
	}

	SEXP_free(behaviors_ent);
	SEXP_free(security_principle_ent);
	SEXP_free(security_principle_val);
	return 0;
}
