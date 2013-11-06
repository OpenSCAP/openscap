/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 * Author: Martin Preisler <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/check_engine_plugin.h"
#include "common/alloc.h"
#include "common/util.h"
#include "common/_error.h"

#include <dlfcn.h>

#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

static struct check_engine_plugin_def *check_engine_plugin_def_new(void)
{
	struct check_engine_plugin_def *ret = (struct check_engine_plugin_def*) oscap_calloc(1, sizeof(struct check_engine_plugin_def));
	return ret;
}

static void check_engine_plugin_def_free(struct check_engine_plugin_def *plugin)
{
	if (plugin->module_handle)
		check_engine_plugin_unload(plugin);

	oscap_free(plugin);
}

struct check_engine_plugin_def *check_engine_plugin_load(const char* path)
{
	struct check_engine_plugin_def *ret = check_engine_plugin_def_new();

	// Clear any pre-existing dlerrors
	dlerror();

	ret->module_handle = dlopen(path, RTLD_LAZY);

	if (!ret->module_handle) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Failed to load extra check engine from '%s'. Details: '%s'.",
			path, dlerror());

		check_engine_plugin_def_free(ret);
		return NULL;
	}

	// Clear any pre-existing dlerrors
	dlerror();

	check_engine_plugin_entry_fn entry_fn = NULL;
	*(void **)(&entry_fn) = dlsym(ret->module_handle, STRINGIZE(OPENSCAP_CHECK_ENGINE_PLUGIN_ENTRY));

	char *error = NULL;
	if ((error = dlerror()) != NULL) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Failed to retrieve module entry '%s' from loaded extra check engine '%s'. Details: '%s'.",
			STRINGIZE(OPENSCAP_CHECK_ENGINE_PLUGIN_ENTRY), path, dlerror());

		check_engine_plugin_def_free(ret);
		return NULL;
	}

	if ((*entry_fn)(ret) != 0) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Failed to fill check_engine_plugin_def when loading check engine plugin '%s'.", path);

		check_engine_plugin_def_free(ret);
		return NULL;
	}

	return ret;
}

void check_engine_plugin_unload(struct check_engine_plugin_def *plugin)
{
	if (!plugin->module_handle) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Failed to unload this check engine plugin. It seems the plugin hasn't been loaded!");
	}

	dlclose(plugin->module_handle);
	plugin->module_handle = NULL;
}

int check_engine_plugin_register(struct check_engine_plugin_def *plugin, struct xccdf_policy_model *model, const char* path_hint)
{
	if (!plugin->module_handle) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Failed to register this check engine plugin to given policy_model, the plugin hasn't been loaded!");

		return -1;
	}

	if (!plugin->register_fn) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Plugin seems to have been loaded but its register_fn member hasn't been filled. Bad plugin entry function implementation suspected.");

		return -1;
	}

	return (plugin->register_fn)(model, path_hint, &plugin->user_data);
}

int check_engine_plugin_cleanup(struct check_engine_plugin_def *plugin, struct xccdf_policy_model *model)
{
	if (!plugin->module_handle) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Failed to cleanup this check engine plugin, the plugin hasn't been loaded!");

		return -1;
	}

	if (!plugin->cleanup_fn) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Plugin seems to have been loaded but its cleanup_fn member hasn't been filled. Bad plugin entry function implementation suspected.");

		return -1;
	}

	return (plugin->cleanup_fn)(model, &plugin->user_data);
}

int check_engine_plugin_export_results(struct check_engine_plugin_def *plugin, struct xccdf_policy_model *model, bool validate, const char *path_hint)
{
	if (!plugin->module_handle) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Failed to export results from this check engine plugin, the plugin hasn't been loaded!");

		return -1;
	}

	if (!plugin->export_results_fn) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Plugin seems to have been loaded but its export_results_fn member hasn't been filled. Bad plugin entry function implementation suspected.");

		return -1;
	}

	return (plugin->export_results_fn)(model, validate, path_hint, &plugin->user_data);
}
