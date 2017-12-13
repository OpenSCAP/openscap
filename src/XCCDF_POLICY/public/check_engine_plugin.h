/**
 * @addtogroup XCCDF_POLICY
 * @{
 * @addtogroup Check Engine Plugin
 * @{
 *
 * @file check_engine_plugin.h
 * @author Martin Preisler <mpreisle@redhat.com>
 */

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
 */

#ifndef OPENSCAP_CHECK_ENGINE_PLUGIN_H_
#define OPENSCAP_CHECK_ENGINE_PLUGIN_H_

#include "xccdf_policy.h"
#include "oscap_export.h"

struct check_engine_plugin_def
{
	int _reserved; // FIXME: version checking?

	void *module_handle;
	void *user_data;

	// NB: path hint is the input file path, can be used for relative path resolution

	// first arg: policy model to register with, second arg: path hint, third arg: user data
int (*register_fn)(struct xccdf_policy_model *, const char*, void**);
	// first arg: policy model, second arg: user data
int (*cleanup_fn)(struct xccdf_policy_model *, void**);
	// first arg: policy model, second arg: validate, third arg: path hint, fourth arg: user data
int (*export_results_fn)(struct xccdf_policy_model *, bool, const char*, void**);
	// first arg: user data
const char *(*get_capabilities_fn)(void**);
};

OSCAP_API struct check_engine_plugin_def *check_engine_plugin_load2(const char* path, bool quiet);
OSCAP_API struct check_engine_plugin_def *check_engine_plugin_load(const char* path);
OSCAP_API void check_engine_plugin_unload(struct check_engine_plugin_def *plugin);

OSCAP_API int check_engine_plugin_register(struct check_engine_plugin_def *plugin, struct xccdf_policy_model *model, const char *path_hint);
OSCAP_API int check_engine_plugin_cleanup(struct check_engine_plugin_def *plugin, struct xccdf_policy_model *model);
OSCAP_API int check_engine_plugin_export_results(struct check_engine_plugin_def *plugin, struct xccdf_policy_model *model, bool validate, const char *path_hint);
OSCAP_API const char *check_engine_plugin_get_capabilities(struct check_engine_plugin_def *plugin);

/**
 * This is the entry point of shared objects implementing extra check engines
 */
OSCAP_API typedef int (*check_engine_plugin_entry_fn) (struct check_engine_plugin_def*);

#define OPENSCAP_CHECK_ENGINE_PLUGIN_ENTRY OPENSCAP_CHECK_ENGINE_PLUGIN_ENTRY

/**
 * @brief Retrieves list of plugin names that are known to OpenSCAP
 *
 * Loading of these will be attempted automatically
 */
OSCAP_API const char * const *check_engine_plugin_get_known_plugins(void);

/// @}
/// @}
#endif
