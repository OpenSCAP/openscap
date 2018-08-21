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
#include <wbemidl.h>
#include <WMIUtils.h>
#include <windows.h>
#include "debug_priv.h"
#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "probe/probe.h"
#include "list.h"
#include "util.h"
#include "oval_definitions.h"
#include "oval_sexp.h"
#include "wmi57_probe.h"
#include "oscap_helpers.h"

static struct oscap_list *get_wql_fields(WCHAR *wql)
{
	HRESULT hr = 0;
	IWbemQuery *query = NULL;

	hr = CoCreateInstance(&CLSID_WbemQuery, 0, CLSCTX_INPROC_SERVER, &IID_IWbemQuery, (LPVOID *)&query);
	if (FAILED(hr)) {
		dE("Failed to create IWbemQuery object.");
		return NULL;
	}

	hr = query->lpVtbl->Parse(query, L"WQL", wql, 0);
	if (FAILED(hr)) {
		dE("Failed to parse IWbemQuery object.");
		return NULL;
	}

	/* Perform an analysis of the WQL statement */
	SWbemRpnEncodedQuery *wql_analysis = NULL;
	hr = query->lpVtbl->GetAnalysis(query, WMIQ_ANALYSIS_RPN_SEQUENCE, 0, (LPVOID *)&wql_analysis);
	if (FAILED(hr)) {
		dE("Failed to analyse IWbemQuery object.");
		return NULL;
	}

	ULONG fields_count = wql_analysis->m_uSelectListSize;
	struct oscap_list *fields_list = oscap_list_new();

	for (ULONG i = 0; i < fields_count; i++) {
		for (ULONG j = 0; j < wql_analysis->m_ppSelectList[i]->m_uNameListSize; j++) {
			WCHAR *field = wql_analysis->m_ppSelectList[i]->m_ppszNameList[j];
			oscap_list_add(fields_list, wcsdup(field));
			if (wcscmp(field, L"*") == 0) {
				dE("The WQL query '%S' contains * in SELECT statement, which is not allowed by OVAL Definitions Schema.", wql);
				/* free the memory that the parser returns in a previous call to GetAnalysis */
				query->lpVtbl->FreeMemory(query, wql_analysis);
				/* free the memory that the parser is holding */
				query->lpVtbl->Empty(query);
				oscap_list_free(fields_list, free);
				return NULL;
			}
		}
	}

	/* free the memory that the parser returns in a previous call to GetAnalysis */
	query->lpVtbl->FreeMemory(query, wql_analysis);
	/* free the memory that the parser is holding */
	query->lpVtbl->Empty(query);

	return fields_list;
}

static int collect_value(probe_ctx *ctx, WCHAR *wmi_namespace, WCHAR *wql_query, WCHAR *name, VARIANT *variant, CIMTYPE type)
{
	SEXP_t *value = NULL;
	WCHAR *wstrval;
	char *strval;
	SYSTEMTIME systime;
	oval_datatype_t oval_data_type = OVAL_DATATYPE_UNKNOWN;
	switch (type) {
	case CIM_EMPTY:
		value = SEXP_string_new("", 0);
		oval_data_type = OVAL_DATATYPE_STRING;
		break;
	case CIM_SINT8:
		value = SEXP_number_newi_8(V_I1(variant));
		oval_data_type = OVAL_DATATYPE_INTEGER;
		break;
	case CIM_UINT8:
		value = SEXP_number_newu_8(V_UI1(variant));
		oval_data_type = OVAL_DATATYPE_INTEGER;
		break;
	case CIM_SINT16:
		value = SEXP_number_newi_16(V_I2(variant));
		oval_data_type = OVAL_DATATYPE_INTEGER;
		break;
	case CIM_UINT16:
		value = SEXP_number_newu_16(V_UI2(variant));
		oval_data_type = OVAL_DATATYPE_INTEGER;
		break;
	case CIM_SINT32:
		value = SEXP_number_newi_32(V_I4(variant));
		oval_data_type = OVAL_DATATYPE_INTEGER;
		break;
	case CIM_UINT32:
		value = SEXP_number_newu_32(V_UI4(variant));
		oval_data_type = OVAL_DATATYPE_INTEGER;
		break;
	case CIM_SINT64:
		value = SEXP_number_newi_64(V_I8(variant));
		oval_data_type = OVAL_DATATYPE_INTEGER;
		break;
	case CIM_UINT64:
		value = SEXP_number_newu_64(V_UI8(variant));
		oval_data_type = OVAL_DATATYPE_INTEGER;
		break;
	case CIM_REAL32:
		value = SEXP_number_newf(V_R4(variant));
		oval_data_type = OVAL_DATATYPE_FLOAT;
		break;
	case CIM_REAL64:
		value = SEXP_number_newf(V_R8(variant));
		oval_data_type = OVAL_DATATYPE_FLOAT;
		break;
	case CIM_BOOLEAN:
		value = SEXP_number_newb(V_BOOL(variant));
		oval_data_type = OVAL_DATATYPE_BOOLEAN;
		break;
	case CIM_STRING:
		wstrval = variant->bstrVal;
		strval = oscap_windows_wstr_to_str(wstrval);
		value = SEXP_string_new(strval, strlen(strval));
		oval_data_type = OVAL_DATATYPE_STRING;
		break;
	case CIM_DATETIME:
		VariantTimeToSystemTime(V_DATE(variant), &systime);
		strval = oscap_sprintf("%02u/%02u/%04u %02d.%02d.%02d", systime.wDay, systime.wMonth, systime.wYear, systime.wHour, systime.wMinute, systime.wSecond);
		value = SEXP_string_new(strval, strlen(strval));
		oval_data_type = OVAL_DATATYPE_STRING;
		break;
	case CIM_REFERENCE:
	case CIM_CHAR16:
		/* TODO */
	case CIM_OBJECT:
	case CIM_FLAG_ARRAY:
	case CIM_ILLEGAL:
	default:
		break;
	}

	/* prepare the result element - record entity */
	SEXP_t *result_sexp = probe_ent_creat1("result", NULL, NULL);
	probe_ent_setdatatype(result_sexp, OVAL_DATATYPE_RECORD);

	/* prepare the field element */
	SEXP_t *field_ent = probe_ent_creat1("field", NULL, value);
	probe_ent_setdatatype(field_ent, oval_data_type);

	char *name_str = oscap_windows_wstr_to_str(name);
	/*
	 * Convert the value of the 'name' attribute to lowercase letters, because
	 * it is not allowed to have uppercase letters according to OVAL specification.
	 * See EntityItemFieldType in
	 * https://oval.mitre.org/language/version5.11/ovalsc/documentation/oval-system-characteristics-schema.html#EntityItemFieldType
	 */
	for (char *p = name_str; *p; p++) {
		*p = tolower(*p);
	}
	SEXP_t *name_sexp = SEXP_string_new(name_str, strlen(name_str));
	probe_ent_attr_add(field_ent, "name", name_sexp);
	SEXP_free(name_sexp);

	/* put the field in the record */
	SEXP_list_add(result_sexp, field_ent);
	SEXP_free(field_ent);

	/* Create item and put the record in item */
	SEXP_t *item = probe_item_create(OVAL_WINDOWS_WMI_57, NULL,
		"namespace", OVAL_DATATYPE_STRING, oscap_windows_wstr_to_str(wmi_namespace),
		"wql", OVAL_DATATYPE_STRING, oscap_windows_wstr_to_str(wql_query),
		NULL);
	SEXP_list_add(item, result_sexp);
	SEXP_free(result_sexp);

	probe_item_collect(ctx, item);

	return 0;
}

static int query_wmi(probe_ctx *ctx, WCHAR *wmi_namespace, WCHAR *wql_query)
{
	HRESULT ret = 0;
	IWbemLocator *locator = NULL;
	IWbemServices *services = NULL;
	IEnumWbemClassObject *results = NULL;

	BSTR resource = SysAllocString(wmi_namespace);
	BSTR language = SysAllocString(L"WQL");

	BSTR query = SysAllocString(wql_query);

	ret = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(ret)) {
		dE("Cannot initialize the COM library.");
		goto cleanup;
	}
	ret = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(ret)) {
		dE("Cannot initialize security for the COM library.");
		goto cleanup;
	}

	ret = CoCreateInstance(&CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, &IID_IWbemLocator, (LPVOID *)&locator);
	if (FAILED(ret)) {
		dE("Cannot create any instance of IWbemLocator object.");
		goto cleanup;
	}
	ret = locator->lpVtbl->ConnectServer(locator, resource, NULL, NULL, NULL, 0, NULL, NULL, &services);
	if (FAILED(ret)) {
		locator->lpVtbl->Release(locator);
		dE("Cannot connect to WMI namespace '%S'.", wmi_namespace);
		goto cleanup;
	}

	ret = services->lpVtbl->ExecQuery(services, language, query, WBEM_FLAG_BIDIRECTIONAL, NULL, &results);
	if (FAILED(ret)) {
		locator->lpVtbl->Release(locator);
		services->lpVtbl->Release(services);
		dE("Cannot execute WQL query '%S'.", wql_query);
		goto cleanup;
	}

	struct oscap_list *wql_fields = get_wql_fields(wql_query);
	if (wql_fields == NULL) {
		locator->lpVtbl->Release(locator);
		services->lpVtbl->Release(services);
		results->lpVtbl->Release(results);
		goto cleanup;
	}

	if (results != NULL) {
		IWbemClassObject *result = NULL;
		ULONG returnedCount = 0;

		while ((ret = results->lpVtbl->Next(results, WBEM_INFINITE, 1, &result, &returnedCount)) == S_OK) {
			VARIANT variant;
			CIMTYPE type;

			struct oscap_iterator *it = oscap_iterator_new(wql_fields);
			while (oscap_iterator_has_more(it)) {
				WCHAR *field_name = oscap_iterator_next(it);

				ret = result->lpVtbl->Get(result, field_name, 0, &variant, &type, 0);
				collect_value(ctx, wmi_namespace, wql_query, field_name, &variant, type);
			}

			oscap_iterator_free(it);
			result->lpVtbl->Release(result);
		}
	}

	oscap_list_free(wql_fields, free);

	results->lpVtbl->Release(results);
	services->lpVtbl->Release(services);
	locator->lpVtbl->Release(locator);

cleanup:
	CoUninitialize(); // must be called even if CoInitializeEx fails
	SysFreeString(query);
	SysFreeString(language);
	SysFreeString(resource);

	return 0;
}

int wmi57_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in = probe_ctx_getobject(ctx);
	SEXP_t *namespace_ent = probe_obj_getent(probe_in, "namespace", 1);
	SEXP_t *wql_ent = probe_obj_getent(probe_in, "wql", 1);
	SEXP_t *namespace_val = probe_ent_getval(namespace_ent);
	SEXP_t *wql_val = probe_ent_getval(wql_ent);
	char *namespace_str = SEXP_string_cstr(namespace_val);
	char *wql_str = SEXP_string_cstr(wql_val);
	WCHAR *namespace_wstr = oscap_windows_str_to_wstr(namespace_str);
	WCHAR *wql_wstr = oscap_windows_str_to_wstr(wql_str);

	query_wmi(ctx, namespace_wstr, wql_wstr);

	free(wql_wstr);
	free(namespace_wstr);
	free(wql_str);
	free(namespace_str);
	SEXP_free(wql_val);
	SEXP_free(namespace_val);
	SEXP_free(wql_ent);
	SEXP_free(namespace_ent);
	return 0;
}
