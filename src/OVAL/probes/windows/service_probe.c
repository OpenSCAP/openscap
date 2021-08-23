/*
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
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <windows.h>
#include "service_probe.h"
#include <strsafe.h>
#include <stdio.h>
#include <probe/entcmp.h>
#include <debug_priv.h>

#define MAX_LEN_SC_NAME 255


 /**
  * \brief      Stock all information about a service_item from Oval Specification v5.11
  */
static struct service_info
{
    char* service_name_str;
    char* display_name_str;
    char* description_str;
    char* service_type;
    char* start_type;
    char* current_state;
    char** controls_accepted;
    char* start_name_str;
    char* path_str;
    int pid;
    bool service_flag;
    const char* dependencies;
};

/**
 * \brief       Modify service_type DWORD in service_type STR
 * \param    \a service_type Windows Service type (DWORD)
 * \return    The associated \e str service_type from the (DWORD)
 */
static char* service_types_to_str(DWORD service_type)
{
    switch (service_type) {
    case SERVICE_KERNEL_DRIVER:
        return "SERVICE_KERNEL_DRIVER";
    case SERVICE_FILE_SYSTEM_DRIVER:
        return "SERVICE_FILE_SYSTEM_DRIVER";
    case SERVICE_WIN32_OWN_PROCESS:
        return "SERVICE_WIN32_OWN_PROCESS";
    case SERVICE_WIN32_SHARE_PROCESS:
        return "SERVICE_WIN32_SHARE_PROCESS";
    case SERVICE_INTERACTIVE_PROCESS:
        return "SERVICE_INTERACTIVE_PROCESS";
    default:
        return "unknown";
    }
}

/**
 * \brief       Modify controls_accepted DWORD in controls_accepted STR
 * \param    \a controls_accepted Windows Controls Accepted (DWORD)
 * \return    The associated \e str controls_accepted from the (DWORD)
 */
static char* controls_accepted_to_str(DWORD controls_accepted)
{
    switch (controls_accepted) {
    case SERVICE_ACCEPT_STOP:
        return "SERVICE_ACCEPT_STOP";
    case SERVICE_ACCEPT_PAUSE_CONTINUE:
        return "SERVICE_ACCEPT_PAUSE_CONTINUE";
    case SERVICE_ACCEPT_SHUTDOWN:
        return "SERVICE_ACCEPT_SHUTDOWN";
    case SERVICE_ACCEPT_PARAMCHANGE:
        return "SERVICE_ACCEPT_PARAMCHANGE";
    case SERVICE_ACCEPT_NETBINDCHANGE:
        return "SERVICE_ACCEPT_NETBINDCHANGE";
    case SERVICE_ACCEPT_HARDWAREPROFILECHANGE:
        return "SERVICE_ACCEPT_HARDWAREPROFILECHANGE";
    case SERVICE_ACCEPT_POWEREVENT:
        return "SERVICE_ACCEPT_POWEREVENT";
    case SERVICE_ACCEPT_SESSIONCHANGE:
        return "SERVICE_ACCEPT_SESSIONCHANGE";
    case SERVICE_ACCEPT_PRESHUTDOWN:
        return "SERVICE_ACCEPT_PRESHUTDOWN";
    case SERVICE_ACCEPT_TIMECHANGE:
        return "SERVICE_ACCEPT_TIMECHANGE";
    case SERVICE_ACCEPT_TRIGGEREVENT:
        return "SERVICE_ACCEPT_TRIGGEREVENT";    
    default:
        return "unknown";

    }
}

/**
 * \brief       Modify start_type DWORD in start_type STR
 * \param    \a start_type    Windows Start type (DWORD)
 * \return    The associated \e str start_type from the (DWORD)
 */
static char* start_types_to_str(DWORD start_type)
{
    switch (start_type) {
    case SERVICE_BOOT_START:
        return "SERVICE_BOOT_START";
    case SERVICE_SYSTEM_START:
        return "SERVICE_SYSTEM_START";
    case SERVICE_AUTO_START:
        return "SERVICE_AUTO_START";
    case SERVICE_DEMAND_START:
        return "SERVICE_DEMAND_START";
    case SERVICE_DISABLED:
        return "SERVICE_DISABLED";
    default:
        return "unknown";
    }
}

/**
 * \brief       Modify service_state DWORD in service_state STR
 * \param    \a service_state    Windows Service State (DWORD)
 * \return    The associated \e str service_state from the (DWORD)
 */
static char* service_state_to_str(DWORD service_state)
{
    switch (service_state) {
    case SERVICE_STOPPED:
        return "SERVICE_STOPPED";
    case SERVICE_START_PENDING:
        return "SERVICE_START_PENDING";
    case SERVICE_STOP_PENDING:
        return "SERVICE_STOP_PENDING";
    case SERVICE_RUNNING:
        return "SERVICE_RUNNING";
    case SERVICE_CONTINUE_PENDING:
        return "SERVICE_CONTINUE_PENDING";
    case SERVICE_PAUSE_PENDING:
        return "SERVICE_PAUSE_PENDING";
    case SERVICE_PAUSED:
        return "SERVICE_PAUSED";
    default:
        return "unknown";
    }
}

/**
 * \brief       Obtains all controls accepted from the arg DWORD
 * \details     Use bitmask operations to obtain all controls accepted
 * \param    \a controls_accepted    Windows Controls Accepted (DWORD)
 * \return    list of \e str with all controls_accepted
 */
static char** get_controls_accepted(DWORD controls_accepted)
{
    //dD("\n\n ********************* CONTROL ACCEPTEDS 0x%X ****************** \n\n", controls_accepted);
    uint32_t bytes[14] = {
        0x00000400, 0x00000200, 0x00000100, 0x00000080, 0x00000040, 0x00000020,
        0x00000010, 0x00000008, 0x00000004, 0x00000002, 0x00000001
    };
    uint32_t found_bytes[14];
    for (int i = 0; i < 14; i++) {
        found_bytes[i] = controls_accepted & bytes[i];
    }
    static char* controls_accepted_list[14];
    for (int j = 0; j < 14; j++) {
        controls_accepted_list[j] = controls_accepted_to_str((DWORD)found_bytes[j]);
    }
    return controls_accepted_list;
}

/**
 * \brief       Set service description in struct service_info
 * \param    \a sc_info    struct service_info*
 * \param    \a lpsd   typedef LPSERVICE_DESCRIPTION which contains service description
 */
static void set_service_description(struct service_info* sc_info, LPSERVICE_DESCRIPTION  lpsd)
{
    if (lpsd->lpDescription) {
        sc_info->description_str = (char*)malloc(
            strlen(lpsd->lpDescription) * sizeof(char*) + 1);
        if (sc_info->description_str == NULL) {
            sc_info->description_str = "";
        }
        else {
            StringCchCopy(sc_info->description_str,
                strlen(lpsd->lpDescription) * sizeof(char*) + 1,
                lpsd->lpDescription);
        }
    }
    else {
        sc_info->description_str = "";
    }
}

/**
 * \brief       Set service configuration in struct service_info
 * \param    \a sc_info    struct service_info*
 * \param    \a lpsc   typedef LPQUERY_SERVICE_CONFIG which contains service configuration
 */
static void set_service_config(struct service_info* sc_info, LPQUERY_SERVICE_CONFIG lpsc)
{
    sc_info->display_name_str = (char*)malloc(
        strlen(lpsc->lpDisplayName) * sizeof(char*) + 1);
    if (sc_info->display_name_str == NULL) {
        sc_info->display_name_str = "";
    }
    else {
        StringCchCopy(
            sc_info->display_name_str,
            strlen(lpsc->lpDisplayName) * sizeof(char*) + 1,
            lpsc->lpDisplayName);
    }

    sc_info->path_str = (char*)malloc(
        strlen(lpsc->lpBinaryPathName) * sizeof(char*) + 1);
    if (sc_info->path_str == NULL) {
        sc_info->path_str = "";
    }
    else {
        StringCchCopy(sc_info->path_str,
            strlen(lpsc->lpBinaryPathName) * sizeof(char*) + 1,
            lpsc->lpBinaryPathName);
    }

    sc_info->dependencies = (char*)malloc(
        strlen(lpsc->lpDependencies) * sizeof(char*) + 1);
    if (sc_info->dependencies == NULL) {
        sc_info->dependencies = "";
    }
    else {
        StringCchCopy(sc_info->dependencies,
            strlen(lpsc->lpDependencies) * sizeof(char*) + 1,
            lpsc->lpDependencies);
    }

    sc_info->start_name_str = (char*)malloc(
        strlen(lpsc->lpServiceStartName) * sizeof(char*) + 1);
    if (sc_info->start_name_str == NULL) {
        sc_info->start_name_str = "";
    }
    else {
        StringCchCopy(sc_info->start_name_str,
            strlen(lpsc->lpServiceStartName) * sizeof(char*) + 1,
            lpsc->lpServiceStartName);
    }

    sc_info->service_type = service_types_to_str(lpsc->dwServiceType);
    sc_info->start_type = start_types_to_str(lpsc->dwStartType);
}

/**
 * \brief       Set service status process in struct service_info
 * \param    \a sc_info    struct service_info*
 * \param    \a lpss   typedef LPSERVICE_STATUS_PROCESS which contains service status process
 */
static void set_service_status_process(struct service_info* sc_info, LPSERVICE_STATUS_PROCESS lpss)
{
    sc_info->current_state = service_state_to_str(lpss->dwCurrentState);
    sc_info->pid = (int)lpss->dwProcessId;
    sc_info->controls_accepted = get_controls_accepted(lpss->dwControlsAccepted);
    sc_info->service_flag = (bool)lpss->dwServiceFlags;
}


/**
 * \brief       Enum Windows services
 * \param    \a sc_info    LPDWORD dwServicesReturned
 * \param    \a LPENUM_SERVICE_STATUS which points to the list of all Windows services
 * \return   \e int execution control
 */
static int enum_services(LPENUM_SERVICE_STATUS* lpEnumServices, LPDWORD dwServicesReturned)
{
    SC_HANDLE sc = OpenSCManager(
        NULL, NULL, SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS);
    if (sc == NULL)
    {
        dE("ERROR > service_probe.c : Cannot open ServiceManager");
        return PROBE_ESYSTEM;
    }

    DWORD dwBuffSize = 0, dwBytesNeeded = 0, dwResumeHandle = 0;
    BOOL bResult = EnumServicesStatus(
        sc,
        SERVICE_TYPE_ALL,
        SERVICE_STATE_ALL,
        *lpEnumServices,
        dwBuffSize,
        &dwBytesNeeded,
        dwServicesReturned,
        &dwResumeHandle
    );

    if (bResult != FALSE)
    {
        dE("ERROR > service_probe.c : Failed to EnumServicesStatus");
        CloseServiceHandle(sc);
        return PROBE_ESYSTEM;
    }

    *lpEnumServices = (LPENUM_SERVICE_STATUS*)malloc(dwBytesNeeded);
    if (*lpEnumServices == NULL)
    {
        dE("ERROR > service_probe.c : Failed to call malloc");
        CloseServiceHandle(sc);
        return PROBE_EFAULT;
    }

    bResult = EnumServicesStatus(
        sc,
        SERVICE_TYPE_ALL,
        SERVICE_STATE_ALL,
        *lpEnumServices,
        dwBytesNeeded,
        &dwBytesNeeded,
        dwServicesReturned,
        &dwResumeHandle
    );

    CloseServiceHandle(sc);
    if (bResult != FALSE) {
        return 0;
    }
    else {
        return 1;
    }
    
}
/**
 * \brief       Collect all information about a service
 * \param    \a sc_info struct service_info
 * \param    \a service_name   char *
 * \return   \e int execution control
 */
static int collect_service_info(char* service_name, struct service_info* sc_info)
{
    SC_HANDLE sc_manager, sc_handler;
    LPQUERY_SERVICE_CONFIG lpsc = NULL;
    LPSERVICE_DESCRIPTION lpsd = NULL;
    LPSERVICE_STATUS_PROCESS lpss = NULL;
    DWORD dwBytesNeeded = 0, cbBufSize = 0, dwError = 0;
    int err = 0;
    sc_info->service_name_str = service_name;

    // Get Service Manager Handler
    sc_manager = OpenSCManager(NULL, NULL,
        SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS);

    if (sc_manager == NULL)
    {
        dE("ERROR > service_probe.c : Cannot open ServiceManager");
        return PROBE_ESYSTEM;
    }

    // Get Service Handler
    // Need Config and Status Access
    sc_handler = OpenService(sc_manager, (LPCSTR)service_name,
        (SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS));

    if (sc_handler == NULL)
    {
        dE("ERROR > service_probe.c : Cannot open ServiceManager (%d)", GetLastError());
        CloseServiceHandle(sc_manager);
        return PROBE_ESYSTEM;
    }

    // Get configuration service informations.
    if (!QueryServiceConfig(sc_handler, NULL, 0, &dwBytesNeeded))
    {
        dwError = GetLastError();
        if (ERROR_INSUFFICIENT_BUFFER == dwError)
        {
            cbBufSize = dwBytesNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG)malloc(cbBufSize);
            if (lpsc == NULL) {
                dE("ERROR > service_probe.c : Failed to malloc");
                err = PROBE_EFAULT;
                goto cleanup;
            }
        }
        else
        {
            dE("ERROR > service_probe.c : Cannot open QueryServiceConfig (%d)", dwError);
            err = PROBE_ESYSTEM;
            goto cleanup;
        }
    }
    if (!QueryServiceConfig(sc_handler, lpsc, cbBufSize, &dwBytesNeeded))
    {
        dE("ERROR > service_probe.c : Cannot open QueryServiceConfig (%d)", GetLastError());
        err = PROBE_ESYSTEM;
        free(lpsc);
        goto cleanup;
    }

    // Get description service informations
    if (!QueryServiceConfig2(
        sc_handler,
        SERVICE_CONFIG_DESCRIPTION,
        NULL,
        sizeof(LPSERVICE_DESCRIPTIONA),
        &dwBytesNeeded))
    {
        dwError = GetLastError();
        if (ERROR_INSUFFICIENT_BUFFER == dwError)
        {
            cbBufSize = dwBytesNeeded;
            lpsd = (LPSERVICE_DESCRIPTION)malloc(cbBufSize);
            if (lpsd == NULL) {
                dE("ERROR > service_probe.c : Failed to call malloc");
                err = PROBE_EFAULT;
                free(lpsc);
                goto cleanup;
            }
        }
        else
        {
            dE("ERROR > service_probe.c : Cannot open QueryServiceConfig2 (%d)", dwError);
            err = PROBE_ESYSTEM;
            free(lpsc);
            goto cleanup;
        }
    }

    if (!QueryServiceConfig2(
        sc_handler,
        SERVICE_CONFIG_DESCRIPTION,
        (LPBYTE)lpsd,
        cbBufSize,
        &dwBytesNeeded))
    {
        dE("ERROR > service_probe.c : Cannot open QueryServiceConfig2 (%d)", GetLastError());
        err = PROBE_ESYSTEM;
        free(lpsc);
        free(lpsd);
        goto cleanup;
    }

    // Get status service informations 
    if (!QueryServiceStatusEx(
        sc_handler,
        SC_STATUS_PROCESS_INFO,
        NULL,
        0,
        &dwBytesNeeded))
    {
        dwError = GetLastError();
        if (ERROR_INSUFFICIENT_BUFFER == dwError)
        {
            cbBufSize = dwBytesNeeded;
            lpss = (LPSERVICE_STATUS_PROCESS)malloc(cbBufSize);
            if (lpss == NULL) {
                dE("ERROR > service_probe.c : Failed to call malloc");
                err = PROBE_EFAULT;
                free(lpsc);
                free(lpsd);
                goto cleanup;
            }
        }
        else
        {
            dE("ERROR > service_probe.c : Cannot open QueryServiceStatusEx (%d)", dwError);
            err = PROBE_ESYSTEM;
            free(lpsc);
            free(lpsd);
            goto cleanup;
        }
    }
    if (!QueryServiceStatusEx(
        sc_handler,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)lpss,
        cbBufSize,
        &dwBytesNeeded))
    {
        dE("ERROR > service_probe.c : Cannot open QueryServiceStatusEx (%d)", GetLastError());
        err = PROBE_ESYSTEM;
        free(lpsc);
        free(lpsd);
        free(lpss);
        goto cleanup;
    }

    // Set result in service_info struct
    set_service_config(sc_info, lpsc);
    set_service_description(sc_info, lpsd);
    set_service_status_process(sc_info, lpss);

    free(lpsc);
    free(lpss);
    free(lpsd);
    CloseServiceHandle(sc_handler);
    CloseServiceHandle(sc_manager);
    return sc_info;

cleanup:
    CloseServiceHandle(sc_handler);
    CloseServiceHandle(sc_manager);
    return err;
}

/**
 * \brief       The probe collect the service_item
 * \param    \a service_name_ent : SEXP_t* S_Expression containing services to collect
 * \param    \a sc_info :    struct service_info
 * \param    \a ctx :  probe_ctx containing the probe context
 */
static void collect_item(SEXP_t* service_name_ent, struct service_info sc_info, probe_ctx* ctx)
{
    SEXP_t* service_name_sexp = SEXP_string_new(sc_info.service_name_str,
        strlen(sc_info.service_name_str));
    if (probe_entobj_cmp(service_name_ent, service_name_sexp) == OVAL_RESULT_TRUE) {
        SEXP_t* item = probe_item_create(OVAL_WINDOWS_SERVICE, NULL,
            "service_name", OVAL_DATATYPE_STRING, sc_info.service_name_str, NULL);

        if (sc_info.display_name_str != NULL) {
            SEXP_t* display_name_sexp = SEXP_string_new(sc_info.display_name_str,
                strlen(sc_info.display_name_str));
            probe_item_ent_add(item, "display_name", NULL, display_name_sexp);
            SEXP_free(display_name_sexp);
        }


        if (sc_info.description_str != NULL) {
            SEXP_t* description_sexp = SEXP_string_new(sc_info.description_str,
                strlen(sc_info.description_str));
            probe_item_ent_add(item, "description", NULL, description_sexp);
            SEXP_free(description_sexp);
        }

        if (sc_info.service_type != NULL) {
            SEXP_t* service_type_sexp = SEXP_string_new(sc_info.service_type,
                strlen(sc_info.service_type));
            probe_item_ent_add(item, "service_type", NULL, service_type_sexp);
            SEXP_free(service_type_sexp);
        }

        if (sc_info.start_type != NULL) {
            SEXP_t* start_type_sexp = SEXP_string_new(sc_info.start_type,
                strlen(sc_info.start_type));
            probe_item_ent_add(item, "start_type", NULL, start_type_sexp);
            SEXP_free(start_type_sexp);
        }

        if (sc_info.current_state != NULL) {
            SEXP_t* current_type_sexp = SEXP_string_new(sc_info.current_state,
                strlen(sc_info.current_state));
            probe_item_ent_add(item, "current_state", NULL, current_type_sexp);
            SEXP_free(current_type_sexp);
        }

        if (sc_info.controls_accepted != NULL) {
            for (int i = 0; i < 14; i++) {
                if (0 != strcmp(sc_info.controls_accepted[i], "unknown")) {
                    SEXP_t* controls_accepted_sexp = SEXP_string_new(
                        sc_info.controls_accepted[i],
                        strlen(sc_info.controls_accepted[i]));
                    SEXP_t* controls_accepted_ent = probe_ent_creat1(
                        "controls_accepted", NULL, controls_accepted_sexp);
                    char* status_str = "exists";
                    SEXP_t* status_sexp = SEXP_string_new(
                        status_str, strlen(status_str));
                    probe_ent_attr_add(controls_accepted_ent, "test", status_sexp);
                    SEXP_list_add(item, controls_accepted_ent);
                    SEXP_free(controls_accepted_ent);
                }
            }
        }

        if (sc_info.start_name_str != NULL) {
            SEXP_t* start_name_sexp = SEXP_string_new(
                sc_info.start_name_str, strlen(sc_info.start_name_str));
            probe_item_ent_add(item, "start_name", NULL, start_name_sexp);
            SEXP_free(start_name_sexp);
        }

        if (sc_info.path_str != NULL) {
            SEXP_t* path_sexp = SEXP_string_new(
                sc_info.path_str, strlen(sc_info.path_str));
            probe_item_ent_add(item, "path", NULL, path_sexp);
            SEXP_free(path_sexp);
        }

        SEXP_t* pid_sexp = SEXP_number_newi_32(sc_info.pid);
        probe_item_ent_add(item, "pid", NULL, pid_sexp);
        SEXP_free(pid_sexp);

        SEXP_t* service_flag_sexp = SEXP_number_newb(sc_info.service_flag);
        probe_item_ent_add(item, "service_flag", NULL, service_flag_sexp);
        SEXP_free(service_flag_sexp);


        if (sc_info.dependencies != NULL) {
            SEXP_t* dependencies_sexp = SEXP_string_new(
                sc_info.dependencies, strlen(sc_info.dependencies));
            probe_item_ent_add(item, "dependencies", NULL, dependencies_sexp);
            SEXP_free(dependencies_sexp);
        }
        probe_item_collect(ctx, item);
    }
    else {
        SEXP_free(service_name_sexp);
    }
}

/**
 * \brief       Collect all service_item
 * \param    \a service_name_ent : SEXP_t* S_Expression containing services to collect
 * \param    \a ctx :  probe_ctx containing the probe context
 * \return   \e int execution control
 */
static int collect_services(SEXP_t* service_name_ent, probe_ctx* ctx)
{
    LPENUM_SERVICE_STATUS* lpEnumServices = NULL;
    DWORD dwServicesReturned = 0;
    struct service_info* sc_info = NULL;
    char* service_name = NULL;
    int err = 1;

    service_name = malloc(MAX_LEN_SC_NAME + 1);
    if (service_name == NULL) {
        return PROBE_EFAULT;
    }

    sc_info = (struct service_info*)malloc(sizeof(struct service_info));
    if (sc_info == NULL) {
        free(service_name);
        return PROBE_EFAULT;
    }

    lpEnumServices = (LPENUM_SERVICE_STATUS*)malloc(sizeof(LPENUM_SERVICE_STATUS));
    if (lpEnumServices == NULL) {
        free(service_name);
        free(sc_info);
        return PROBE_EFAULT;
    }
    err = enum_services(lpEnumServices, &dwServicesReturned);
    for (DWORD i = 0; i < dwServicesReturned; i++)
    {
        StringCchCopy(
            service_name,
            MAX_LEN_SC_NAME,
            (*lpEnumServices)->lpServiceName);
        err = collect_service_info(service_name, sc_info);
        if (sc_info != NULL) {
            collect_item(service_name_ent, *sc_info, ctx);
            err = 0;
        }
        ++(*lpEnumServices);
    }
    free(service_name);
    free(sc_info);
    free(lpEnumServices);
    return err;
}

/**
 * \brief       Main collecting all services information requested.
 * \param    \a ctx :  probe_ctx containing the probe context
 * \return   \e int execution control
 */
int service_probe_main(probe_ctx* ctx, void* arg)
{
    SEXP_t* probe_in, * service_name_ent;
    int err;

    probe_in = probe_ctx_getobject(ctx);
    service_name_ent = probe_obj_getent(probe_in, "service_name", 1);

    if (service_name_ent == NULL) {
        return PROBE_ENOENT;
    }

    err = collect_services(service_name_ent, ctx);

    SEXP_free(service_name_ent);
    return err;
}
