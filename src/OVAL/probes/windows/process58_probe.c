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

#include <probe/entcmp.h>
#include "probe-api.h"
#include "debug_priv.h"
#include "oval_sexp.h"

#include <strsafe.h>
#include <pcre.h>
#include <windows.h>
#include <Winternl.h>
#include <TlHelp32.h>

 /**
  * \brief is oscap executed with admin rights ?
  */
BOOL is_elevated;


/**
 * \brief       Structure containing the process informations
 */
struct process_info
{
    char* command_line;
    int pid;
    int ppid;
    int priority;
    char* image_path;
    char* current_dir;
    ULONGLONG creation_time;
    int dep_enabled;
    char* primary_window_text;
    char* name;
};


/**
 * \brief       Definition of type using APIs CreateToolhelp32Snapshot and NTAPI
 */
typedef HANDLE(__stdcall CreateToolhelp32Snapshot_t)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL(__stdcall Process32Find_t)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef NTSTATUS(NTAPI* _NtQueryInformationProcess)(
    HANDLE ProcessHandle,
    DWORD ProcessInformationClass,
    PVOID ProcessInformation,
    DWORD ProcessInformationLength,
    PDWORD ReturnLength
    );

/**
 * \brief       Get the Peb Address from process handle
 * \param    \a process_handler : Handle of the process (HANDLE)
 * \return    The Peb Base Address (PVOID)
 */
static PVOID get_peb_address(HANDLE process_handler)
{
    HMODULE h_module = GetModuleHandleA("ntdll.dll");
    if (h_module == NULL) {
        dE("Failed to get module handle ");
    }
    _NtQueryInformationProcess NtQueryInformationProcess =
        (_NtQueryInformationProcess)GetProcAddress(
            h_module, "NtQueryInformationProcess");
    PROCESS_BASIC_INFORMATION pbi;

    NtQueryInformationProcess(process_handler, 0, &pbi, sizeof(pbi), NULL);

    return pbi.PebBaseAddress;
}

/**
 * \brief     Check if the current process has admin privs
 * \return    True if current process is elevated, False if not
 */
static BOOL check_admin_privs()
{
    BOOL ret = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &elevation,
            sizeof(elevation), &cbSize)) {
            ret = elevation.TokenIsElevated;
        }
    }
    if (!ret) {
        dI("\n\nSome processes can not be collected due to missing admin \
            privs.\n The following processes could be collected: \n\n");
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return ret;
}

/**
 * \brief       Log collected process informations in debug Info
 * \param    \a ps_info : Process Information Structure (process_info)
 */
static void log_process_info(struct process_info ps_info)
{
    dI("Process informations :");
    dI("  -Pid : %d", ps_info.pid);
    dI("  -Ppid : %d", ps_info.ppid);
    dI("  -Name : %s", ps_info.name);
    dI("  -Cmd : %s", ps_info.command_line);
    dI("  -Image path : %s", ps_info.image_path);
    dI("  -Creation time : %I64u", ps_info.creation_time);
    dI("  -Current dir : %s", ps_info.current_dir);
    dI("  -Primary windows text : %s", ps_info.primary_window_text);
    dI("  -Dep enabled : %d", ps_info.dep_enabled);
    dI("  -Priority : %d", ps_info.priority);
    printf("\n\n");
}

/**
 * \brief       Get the dep bool from the DWORD
 * \param    \a dep : Windows Data Execution Prevention (DWORD)
 * \return    an integer from the DWORD
 */
static int get_dep_bool_from_dword(DWORD dep)
{
    if (dep == 0) {
        return 0;
    }
    if (dep & 0x00000001) {
        return 1;
    }
    return -1;
}


/**
 * \brief    Regex a collected info to cut a part or to suppress non UTF-8 chars
 * \param    \a string : The string being regex (char*)
 * \param    \a regex : The regex to use on the string (char*)
 * \return    The matching string (char *)
 */
static char* regex_collected_info(char* string, char* regex)
{
    pcre* re_comp;
    pcre_extra* pcre_extra;
    int pcre_exec_ret;
    int sub_string_vect[MAX_PATH];
    const char* pcre_error_str;
    int pcre_error_offset;
    char* sub_string_match = "";
    int j = 0;

    re_comp = pcre_compile(
        regex, PCRE_UTF8,
        &pcre_error_str,
        &pcre_error_offset,
        NULL
    );
    if (re_comp == NULL) {
        dE("Could not compile '%s': %s\n", regex, pcre_error_str);
        return "PCRE_ERROR";
    }
    pcre_extra = pcre_study(re_comp, 0, &pcre_error_str);

    if (pcre_error_str != NULL) {
        dE("Could not study '%s': %s\n", regex, pcre_error_str);
        return "PCRE_ERROR";
    }
    pcre_exec_ret = pcre_exec(re_comp,
        pcre_extra,
        string,
        strlen(string),
        0,
        0,
        sub_string_vect,
        30);

    if (pcre_exec_ret < 0) {
        switch (pcre_exec_ret) {
        case PCRE_ERROR_NOMATCH:
            dE("PCRE_ERROR_NOMATCH %s %s", string, regex);
            break;
        case PCRE_ERROR_NULL:
            dE("PCRE_ERROR_NULL");
            break;
        case PCRE_ERROR_BADOPTION:
            dE("PCRE_ERROR_BADOPTION");
            break;
        case PCRE_ERROR_BADMAGIC:
            dE("PCRE_ERROR_BADMAGIC");
            break;
        case PCRE_ERROR_UNKNOWN_NODE:
            dE("PCRE_ERROR_UNKNOWN_NODE");
            break;
        case PCRE_ERROR_NOMEMORY:
            dE("PCRE_ERROR_NOMEMORY");
            break;
        default:
            dE("PCRE UNKNOWN ERROR");
            break;
        }
    }
    else {
        pcre_get_substring(string, sub_string_vect, pcre_exec_ret, j, &(sub_string_match));
    }
    return sub_string_match;
}



/**
 * \brief       Get the current dir of the process
 * \param    \a ps_info : Process Information Structure (process_info*)
 * \param    \a process_handler : Handle of the process (HANDLE)
 * \param    \a rtl_user_process_params : Process user parameters (PVOID)
 * \return    An integer containing the execution status
 */
static int get_process_current_dir(struct process_info* ps_info, HANDLE process_handler, PVOID rtl_user_process_params)
{
    UNICODE_STRING ustr_current_dir;
    WCHAR* current_dir;
    if (!ReadProcessMemory(process_handler,
        &(((RTL_USER_PROCESS_PARAMETERS*)rtl_user_process_params)->ImagePathName),
        &ustr_current_dir, sizeof(ustr_current_dir), NULL))
    {
        dE("Failed to ReadProcessMemory (%d)", GetLastError());
        return PROBE_ESYSTEM;
    }

    current_dir = (WCHAR*)calloc(sizeof(WCHAR),ustr_current_dir.Length);
    if (current_dir == NULL) {
        dE("Failed to allocate memory (%d)", GetLastError());
        return PROBE_EFAULT;
    }

    if (!ReadProcessMemory(process_handler, ustr_current_dir.Buffer,
        current_dir, ustr_current_dir.Length, NULL))
    {
        dE("Failed to ReadProcessMemory (%d)", GetLastError());
        free(current_dir);
        return PROBE_ESYSTEM;
    }
    // The regex will cut the "current_dir" variable wich contains the path of the executable.
    // In order to only get the current directory path and to follow the OVAL specifications v5.11.
    // The current dir must be as follow :
    // "C:\Program Files\Windows Defender\" and not "C:\Program Files\Windows Defender\MsMpEng.exe" 
    char* res = regex_collected_info(oscap_windows_wstr_to_str(current_dir), ".*[[:ascii:]](\\\\)");
    if (res == "PCRE_ERROR") {
        ps_info->current_dir = (char*)malloc(
            strlen(oscap_windows_wstr_to_str(current_dir)) * sizeof(char*) + 1);
        if (ps_info->current_dir == NULL) {
            ps_info->current_dir = "";
        }
        else {
            StringCchCopy(ps_info->current_dir,
                strlen(oscap_windows_wstr_to_str(current_dir)) * sizeof(char*) + 1,
                oscap_windows_wstr_to_str(current_dir));
        }
    }
    else {
        ps_info->current_dir = (char*)malloc(
            strlen(res) * sizeof(char*) + 1);
        if (ps_info->current_dir == NULL) {
            ps_info->current_dir = "";
        }
        else {
            StringCchCopy(ps_info->current_dir,
                strlen(res) * sizeof(char*) + 1,
                res);
        }
    }
    free(current_dir);
    return NULL;
}

/**
 * \brief       Get the command line of the process
 * \param    \a ps_info : Process Information Structure (process_info*)
 * \param    \a process_handler : Handle of the process (HANDLE)
 * \param    \a rtl_user_process_params : Process user parameters (PVOID)
 * \return    An integer containing the execution status
 */
static int get_process_command_line(struct process_info* ps_info, HANDLE process_handler, PVOID rtl_user_process_params)
{
    UNICODE_STRING ustr_command_line;
    WCHAR* command_line;
    if (!ReadProcessMemory(process_handler,
        &(((RTL_USER_PROCESS_PARAMETERS*)rtl_user_process_params)->CommandLine),
        &ustr_command_line, sizeof(ustr_command_line), NULL))
    {
        dE("Failed to ReadProcessMemory (%d)", GetLastError());
        return PROBE_ESYSTEM;
    }

    command_line = (WCHAR*)calloc(ustr_command_line.Length,sizeof(WCHAR));
    if (command_line == NULL) {
        dE("Failed to allocate memory (%d)", GetLastError());
        return PROBE_EFAULT;
    }

    if (!ReadProcessMemory(process_handler, ustr_command_line.Buffer,
        command_line, ustr_command_line.Length, NULL))
    {
        dE("Failed to ReadProcessMemory (%d)", GetLastError());
        free(command_line);
        return PROBE_ESYSTEM;
    }

    ps_info->command_line = (char*)malloc(
        strlen(oscap_windows_wstr_to_str(command_line)) * sizeof(char*) + 1);
    if (ps_info->command_line == NULL) {
        ps_info->command_line = "";
    }
    else {
        StringCchCopy(ps_info->command_line,
            strlen(oscap_windows_wstr_to_str(command_line)) * sizeof(char*) + 1,
            oscap_windows_wstr_to_str(command_line));
    }
    free(command_line);
    return NULL;
}


/**
 * \brief       Get the name of the process
 * \param    \a ps_info : Process Information Structure (process_info*)
 * \return    An integer containing the execution status
 */
static int get_process_name(struct process_info* ps_info)
{
    char* ps_name = (char*)malloc(strlen(ps_info->image_path) + 1);
    if (ps_name == NULL) {
        dE("Failed to allocate memory");
        return PROBE_EFAULT;
    }
    strcpy_s(ps_name, strlen(ps_info->image_path) + 1, ps_info->image_path);
    while ((ps_name = strsep(&ps_name, ".exe")) != NULL) {
        break;
    }
    ps_info->name = (char*)malloc(sizeof(ps_name) + 1);
    if (ps_info->name == NULL || ps_name == NULL) {
        dE("Failed to allocate memory");
        free(ps_name);
        return PROBE_EFAULT;
    }

    strcpy_s(ps_info->name, strlen(ps_name) + 1, ps_name);

    free(ps_name);
    return NULL;
}

/**
 * \brief       Get the primary_window_text of the process
 * \param    \a ps_info : Process Information Structure (process_info*)
 * \return    An integer containing the execution status
 */
static int get_process_primary_window_text(struct process_info* ps_info)
{
    HWND hwnd = NULL;
    int window_text_length = 0;

    do
    {
        hwnd = FindWindowEx(NULL, hwnd, NULL, NULL);
        DWORD dwProcID = 0;
        GetWindowThreadProcessId(hwnd, &dwProcID);
        if (dwProcID == ps_info->pid) {
            break;
        }
    } while (hwnd != NULL);

    if (hwnd == NULL) {
        return PROBE_EFAULT;
    }
    window_text_length = GetWindowTextLength(hwnd) + 1;
    ps_info->primary_window_text = (char*)malloc(window_text_length);
    if (ps_info->primary_window_text == NULL)
    {
        dE("Failed to allocate memory");
        return PROBE_EFAULT;
    }
    if (!GetWindowText(hwnd, ps_info->primary_window_text, window_text_length)) {
        ps_info->primary_window_text = "";
    }
    return NULL;
}

/**
 * \brief       Get the Data Execution Protection Policy of the process
 * \param    \a ps_info : Process Information Structure (process_info*)
 * \param    \a process_handler : Handle of the process (HANDLE)
 * \return    An integer containing the execution status
 */
static int get_process_dep_enabled(struct process_info* ps_info, HANDLE process_handler)
{
    DWORD dep = 0;
    BOOL perma = 0;
    if (GetProcessDEPPolicy(process_handler, &dep, &perma)) {
        ps_info->dep_enabled = get_dep_bool_from_dword(dep);
        return NULL;
    }
    return 1;
}

/**
 * \brief       Get the creation time of the process int INT64
 * \param    \a ps_info : Process Information Structure (process_info*)
 * \param    \a process_handler : Handle of the process (HANDLE)
 * \return    A big integer (ULONGLONG)
 */
static ULONGLONG get_process_creation_time(HANDLE process_handler)
{
    FILETIME ft_create_time, time2, time3, time4;
    if (!GetProcessTimes(process_handler, &ft_create_time, &time2, &time3, &time4)) {
        ULARGE_INTEGER time;
        time.HighPart = ft_create_time.dwHighDateTime;
        time.LowPart = ft_create_time.dwLowDateTime;
        return time.QuadPart;
    }
    return PROBE_ESYSTEM;
}

/**
 * \brief       Get the Process Priority from his associated Thread
 * \param    \a ps_info : Process Information Structure (process_info*)
 * \return    int
 */
static int get_process_priority(struct process_info* ps_info)
{
    DWORD dwMainThreadID = 0;
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap != INVALID_HANDLE_VALUE) {
        THREADENTRY32 th32;
        th32.dwSize = sizeof(THREADENTRY32);
        BOOL bOK = TRUE;

        if (Thread32First(hThreadSnap, &th32) == NULL) {
            dE("Can't find \"%s\" in library %d", "Thread32First", GetLastError());
            return PROBE_ESYSTEM;
        }

        if (Thread32Next(hThreadSnap, &th32) == NULL) {
            dE("Can't find \"%s\" in library %d", "Thread32Next", GetLastError());
            return PROBE_ESYSTEM;
        }

        for (bOK = Thread32First(hThreadSnap, &th32); bOK;

            bOK = Thread32Next(hThreadSnap, &th32)) {
            if (th32.th32OwnerProcessID == ps_info->pid) {
                HANDLE thread_handler = OpenThread(THREAD_ALL_ACCESS,
                    TRUE, th32.th32ThreadID);
                if (thread_handler != NULL) {
                    ps_info->priority = GetThreadPriority(thread_handler);
                    CloseHandle(thread_handler);
                    return 0;
                } 
                else {
                    ps_info->priority = -1;
                }
            }
        }
    }
    ps_info->priority = -1;
    return PROBE_ESYSTEM;
}
/**
 * \brief       Collect all informations about a process
 * \param    \a ps_info : Process Information Structure (process_info*)
 * \return    An integer containing the execution status
 */
static int collect_process_info(struct process_info* ps_info)
{
    HANDLE process_handler;
    PVOID peb_address;
    PVOID rtl_user_process_params;
    char* unknown = "";

    if (strstr(ps_info->image_path, ".exe")) {
        if (get_process_name(ps_info) != 0) {
            ps_info->name = unknown;
        }
    }
    else {
        ps_info->name = ps_info->image_path;
    }

    if (get_process_primary_window_text(ps_info) != 0) {
        ps_info->primary_window_text = unknown;
    }

    process_handler = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_ALL_ACCESS,
        FALSE, ps_info->pid);

    if (process_handler != NULL)
    {
        ps_info->creation_time =
            ps_info->creation_time = get_process_creation_time(process_handler);
        if (ps_info->creation_time == PROBE_ESYSTEM) {
            ps_info->creation_time = -1;
        }

        get_process_priority(ps_info);

        if (get_process_dep_enabled(ps_info, process_handler) != 0) {
            ps_info->dep_enabled = FALSE;
        }

        peb_address = get_peb_address(process_handler);
        if (ReadProcessMemory(process_handler,
            &(((PEB*)peb_address)->ProcessParameters),
            &rtl_user_process_params,
            sizeof(PVOID), NULL))
        {
            if (get_process_command_line(ps_info,
                process_handler,
                rtl_user_process_params) != 0) {
                ps_info->command_line = unknown;
            }

            if (get_process_current_dir(ps_info,
                process_handler,
                rtl_user_process_params) != 0) {
                ps_info->current_dir = unknown;
            }
        }
        else {
            dE("Failed to ReadProcessMemory (%d) pid:%d", GetLastError(), ps_info->pid);
            ps_info->command_line = unknown;
            ps_info->current_dir = unknown;
            log_process_info(*ps_info);
            return 1;
        }
    }
    else {
        dE("Failed to OpenProcess (%d) pid:%d", GetLastError(), ps_info->pid);
        ps_info->creation_time = 0;
        ps_info->command_line = unknown;
        ps_info->dep_enabled = FALSE;
        ps_info->priority = -1;
        ps_info->current_dir = unknown;
        log_process_info(*ps_info);
        return 1;
    }
    return NULL;
}

/**
 * \brief       The probe collect the process_item
 * \param    \a command_line_ent : SEXP_t *S_Expression containing the command line of the process
 * \param    \a pid_ent : SEXP_t *S_Expression containing the pid of the process
 * \param    \a ps_info :    struct process_info
 * \param    \a ctx :  probe_ctx containing the probe context
 */
static void collect_item(SEXP_t* command_line_ent, SEXP_t* pid_ent, struct process_info ps_info, probe_ctx* ctx)
{
    SEXP_t* commnad_line_sexp = SEXP_string_new(
        ps_info.command_line, strlen(ps_info.command_line));
    SEXP_t* pid_sexp = SEXP_number_newi_32(ps_info.pid);
    if ((commnad_line_sexp == NULL || probe_entobj_cmp(command_line_ent, commnad_line_sexp) == OVAL_RESULT_TRUE) &&
        (pid_sexp == NULL || probe_entobj_cmp(pid_ent, pid_sexp) == OVAL_RESULT_TRUE)) {

        SEXP_t* item = probe_item_create(OVAL_WINDOWS_PROCESS_58, NULL,
            "command_line", OVAL_DATATYPE_STRING, ps_info.command_line, NULL);

        SEXP_free(commnad_line_sexp);

        if (ps_info.pid >= 0) {
            probe_item_ent_add(item, "pid", NULL, pid_sexp);
            SEXP_free(pid_sexp);
        }

        if (ps_info.ppid >= 0) {
            SEXP_t* ppid_sexp = SEXP_number_newi_32(ps_info.ppid);
            probe_item_ent_add(item, "ppid", NULL, ppid_sexp);
            SEXP_free(ppid_sexp);
        }

        if (ps_info.priority != -1) {
            SEXP_t* priority_sexp = SEXP_number_newi_32(ps_info.priority);
            probe_item_ent_add(item, "priority", NULL, priority_sexp);
            SEXP_free(priority_sexp);
        }

        if (ps_info.image_path != NULL) {
            SEXP_t* image_path_sexp = SEXP_string_new(
                ps_info.image_path, strlen(ps_info.image_path));
            probe_item_ent_add(item, "image_path", NULL, image_path_sexp);
            SEXP_free(image_path_sexp);
        }

        if (ps_info.current_dir != NULL) {
            SEXP_t* current_dir_sexp = SEXP_string_new(
                ps_info.current_dir, strlen(ps_info.current_dir));
            probe_item_ent_add(item, "current_dir", NULL, current_dir_sexp);
            SEXP_free(current_dir_sexp);
        }

        if (ps_info.creation_time >= 0) {
            SEXP_t* creation_time_sexp = SEXP_number_newu_64(
                ps_info.creation_time);
            probe_item_ent_add(item, "creation_time", NULL, creation_time_sexp);
            SEXP_free(creation_time_sexp);
        }

        if (ps_info.dep_enabled != -1) {
            SEXP_t* dep_enabled_sexp = SEXP_number_newb(ps_info.dep_enabled);
            probe_item_ent_add(item, "dep_enabled", NULL, dep_enabled_sexp);
            SEXP_free(dep_enabled_sexp);
        }

        if (ps_info.primary_window_text != NULL) {
            SEXP_t* primary_window_text_sexp = SEXP_string_new(
                ps_info.primary_window_text, strlen(ps_info.primary_window_text));
            probe_item_ent_add(item, "primary_window_text", NULL,
                primary_window_text_sexp);
            SEXP_free(primary_window_text_sexp);
        }

        if (ps_info.name != NULL) {
            SEXP_t* name_sexp = SEXP_string_new(
                ps_info.name, strlen(ps_info.name));
            probe_item_ent_add(item, "name", NULL, name_sexp);
            SEXP_free(name_sexp);
        }

        probe_item_collect(ctx, item);
    }
}
/**
 * \brief       Collect all processes
 * \param    \a command_line_ent : SEXP_t *S_Expression containing the command line of the process
 * \param    \a pid_ent : SEXP_t *S_Expression containing the pid of the process
 * \param    \a ctx :  probe_ctx containing the probe context
 * \return    An integer containing the execution status
 */
static int collect_process(SEXP_t* command_line_ent, SEXP_t* pid_ent, probe_ctx* ctx)
{
    HANDLE process_handler;
    struct process_info* ps_info = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    int err = 1;

    HINSTANCE const hLib = GetModuleHandleA("kernel32.dll");
    if (hLib == NULL) {
        dE("Can't load kernel32 library %d", GetLastError());
        return PROBE_ESYSTEM;
    }

    CreateToolhelp32Snapshot_t* lp_ps_handler = (CreateToolhelp32Snapshot_t*)
        GetProcAddress(hLib, "CreateToolhelp32Snapshot");
    if (lp_ps_handler == NULL) {
        dE("Can't find \"%s\" in library %d", "CreateToolhelp32Snapshot", GetLastError());
        return PROBE_ESYSTEM;
    }

    Process32Find_t* pProcess32First = (Process32Find_t*)
        GetProcAddress(hLib, "Process32First");
    if (pProcess32First == NULL) {
        dE("Can't find \"%s\" in library %d", "Process32First", GetLastError());
        return PROBE_ESYSTEM;
    }

    Process32Find_t* pProcess32Next = (Process32Find_t*)
        GetProcAddress(hLib, "Process32Next");
    if (pProcess32Next == NULL) {
        dE("Can't find \"%s\" in library %d", "Process32Next", GetLastError());
        return PROBE_ESYSTEM;
    }

    if ((process_handler = lp_ps_handler(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)
        return PROBE_ESYSTEM;

    ps_info = (struct process_info*)malloc(sizeof(struct process_info));
    if (ps_info == NULL) {
        dE("Failed to allocate memory");
        return PROBE_EFAULT;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (pProcess32First(process_handler, &pe32))
    {
        do {
            ps_info->pid = pe32.th32ProcessID;
            ps_info->ppid = pe32.th32ParentProcessID;
            ps_info->image_path = pe32.szExeFile;

            err = collect_process_info(ps_info);

            if (err == 0) {
                collect_item(command_line_ent, pid_ent, *ps_info, ctx);
            }
        } while (pProcess32Next(process_handler, &pe32));
    }

    free(ps_info);
    return NULL;
}


/**
 * \brief       Main collecting all processes information requested.
 * \param    \a ctx :  probe_ctx containing the probe context
 * \return   \e An integer containing the execution status
 */

int process58_probe_main(probe_ctx* ctx, void* arg)
{
    int err = 1;
    is_elevated = check_admin_privs();

    SEXP_t* probe_in = probe_ctx_getobject(ctx);
    SEXP_t* command_line_ent = probe_obj_getent(probe_in, "command_line", 1);
    SEXP_t* pid_ent = probe_obj_getent(probe_in, "pid", 1);
    if (command_line_ent == NULL && pid_ent == NULL) {
        return PROBE_ENOENT;
    }

    err = collect_process(command_line_ent, pid_ent, ctx);

    SEXP_free(pid_ent);
    SEXP_free(command_line_ent);
    return err;
}
