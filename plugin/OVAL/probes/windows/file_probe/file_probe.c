#include "probe-plugin-api.h"
#include "probe-common.h"
#include "debug_priv.h"

static int file_probe_offline_mode_supported(void)
{
	dI(__FUNCTION__);
	return 0;
}

static void* file_probe_init(void)
{
	dI(__FUNCTION__);
	return NULL;
}

static int file_probe_main(probe_ctx* ctx, void* arg)
{
	dI(__FUNCTION__);
	return PROBE_ESUCCESS;
}

static void file_probe_fini(void* arg)
{
	dI(__FUNCTION__);
}

oval_subtype_t probe_plugin_get_oval_subtype()
{
	return (oval_subtype_t)OVAL_WINDOWS_FILE;
}

probe_init_function_t probe_plugin_get_init_function()
{
	return file_probe_init;
}

probe_main_function_t probe_plugin_get_main_function()
{
	return file_probe_main;
}

probe_fini_function_t probe_plugin_get_fini_function()
{
	return file_probe_fini;
}

probe_offline_mode_function_t probe_plugin_get_offline_mode_function()
{
	return file_probe_offline_mode_supported;
}