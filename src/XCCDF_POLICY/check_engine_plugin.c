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
#include "common/util.h"
#include "common/_error.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

static struct check_engine_plugin_def *check_engine_plugin_def_new(void)
{
	struct check_engine_plugin_def *ret = (struct check_engine_plugin_def*) calloc(1, sizeof(struct check_engine_plugin_def));
	return ret;
}

static void check_engine_plugin_def_free(struct check_engine_plugin_def *plugin)
{
	//if (plugin->module_handle)
	// FIXME: Warning?

	free(plugin);
}

struct check_engine_plugin_def *check_engine_plugin_load2(const char* path, bool quiet)
{
#ifndef _WIN32
	struct check_engine_plugin_def *ret = check_engine_plugin_def_new();

	const char *path_prefix = getenv("OSCAP_CHECK_ENGINE_PLUGIN_DIR");
	char *full_path = path_prefix ? oscap_sprintf("%s/%s", path_prefix, path) : oscap_strdup(path);
	// NB: valgrind reports a leak on the next line, I have confirmed this to be a false positive
	ret->module_handle = dlopen(full_path, RTLD_LAZY);
	free(full_path);

	char *error = NULL;
	if (!ret->module_handle) {
		error = dlerror();

		if (!quiet)
			oscap_seterr(OSCAP_EFAMILY_GLIBC,
				"Failed to load extra check engine from '%s'. Details: '%s'.",
				path, error);

		check_engine_plugin_def_free(ret);
		return NULL;
	}

	check_engine_plugin_entry_fn entry_fn = NULL;
	*(void **)(&entry_fn) = dlsym(ret->module_handle, STRINGIZE(OPENSCAP_CHECK_ENGINE_PLUGIN_ENTRY));

	if ((error = dlerror()) != NULL) {
		if (!quiet)
			oscap_seterr(OSCAP_EFAMILY_GLIBC,
				"Failed to retrieve module entry '%s' from loaded extra check engine '%s'. Details: '%s'.",
				STRINGIZE(OPENSCAP_CHECK_ENGINE_PLUGIN_ENTRY), path, error);

		dlclose(ret->module_handle);
		check_engine_plugin_def_free(ret);
		return NULL;
	}

	if ((*entry_fn)(ret) != 0) {
		if (!quiet)
			oscap_seterr(OSCAP_EFAMILY_GLIBC,
				"Failed to fill check_engine_plugin_def when loading check engine plugin '%s'.", path);

		dlclose(ret->module_handle);
		check_engine_plugin_def_free(ret);
		return NULL;
	}

	return ret;
#else
	// TODO
	return NULL;
#endif
}

struct check_engine_plugin_def *check_engine_plugin_load(const char* path)
{
	return check_engine_plugin_load2(path, false);
}

void check_engine_plugin_unload(struct check_engine_plugin_def *plugin)
{
#ifndef _WIN32
	if (!plugin->module_handle) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Failed to unload this check engine plugin. It seems the plugin hasn't been loaded!");
	}
	else {
		dlclose(plugin->module_handle);
		plugin->module_handle = NULL;
	}
#else
	// TODO
#endif

	check_engine_plugin_def_free(plugin);
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

const char *check_engine_plugin_get_capabilities(struct check_engine_plugin_def *plugin)
{
	if (!plugin->module_handle) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Failed to get capabilities of this check engine plugin, the plugin hasn't been loaded!");

		return NULL;
	}

	if (!plugin->get_capabilities_fn) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
			"Plugin seems to have been loaded but its get_capabilities_fn member hasn't been filled. Bad plugin entry function implementation suspected.");

		return NULL;
	}

	return (plugin->get_capabilities_fn)(&plugin->user_data);
}

#ifndef LT_CURRENT_MINUS_AGE
#error "LT_CURRENT_MINUS_AGE has not been defined! It is necessary to figure out plugin paths to load."
#endif

const char * const known_plugins[] = {"libopenscap_sce.so." STRINGIZE(LT_CURRENT_MINUS_AGE), NULL};

const char * const *check_engine_plugin_get_known_plugins(void)
{
	return known_plugins;
}
