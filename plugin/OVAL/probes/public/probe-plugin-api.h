#pragma once

#include "probe-table.h"

#ifdef PROBE_PLUGIN_EXPORTS
#define PROBE_PLUGIN_API __declspec(dllexport)
#else
#define PROBE_PLUGIN_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef oval_subtype_t (*probe_plugin_get_oval_subtype_t)(void);
typedef probe_init_function_t (*probe_plugin_get_init_function_t)(void);
typedef probe_main_function_t (*probe_plugin_get_main_function_t)(void);
typedef probe_fini_function_t (*probe_plugin_get_fini_function_t)(void);
typedef probe_offline_mode_function_t (*probe_plugin_get_offline_mode_function_t)(void);

PROBE_PLUGIN_API oval_subtype_t probe_plugin_get_oval_subtype();
PROBE_PLUGIN_API probe_init_function_t probe_plugin_get_init_function();
PROBE_PLUGIN_API probe_main_function_t probe_plugin_get_main_function();
PROBE_PLUGIN_API probe_fini_function_t probe_plugin_get_fini_function();
PROBE_PLUGIN_API probe_offline_mode_function_t probe_plugin_get_offline_mode_function();

#ifdef __cplusplus
}
#endif