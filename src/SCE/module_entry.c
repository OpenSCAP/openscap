/**
 * @file module_entry.c
 * \brief Extra Check Engines Module Entry for Script Check Engine
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
 * Authors:
 *      "Martin Preisler" <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sce_engine_api.h"
#include "XCCDF_POLICY/public/check_engine_plugin.h"

#include <libgen.h>
#include <string.h>

static int sce_engine_register(struct xccdf_policy_model *model, const char *path_hint, void** user_data)
{
	struct sce_parameters *parameters = (struct sce_parameters*) *user_data;
	if (parameters != NULL)
		sce_parameters_free(parameters);

	parameters = sce_parameters_new();
	char *xccdf_pathcopy = strdup(path_hint);
	sce_parameters_set_xccdf_directory(parameters, dirname(xccdf_pathcopy));
	sce_parameters_allocate_session(parameters);
	free(xccdf_pathcopy);

	*user_data = (void*)parameters; // This way the data will get freed later

	return !xccdf_policy_model_register_engine_sce(model, parameters);
}

static int sce_engine_cleanup(struct xccdf_policy_model *model, void** user_data)
{
	struct sce_parameters *parameters = (struct sce_parameters*) *user_data;
	if (parameters != NULL)
		sce_parameters_free(parameters);

	// FIXME: Unregister SCE?
	return 0;
}

int OPENSCAP_CHECK_ENGINE_PLUGIN_ENTRY (struct check_engine_plugin_def *plugin);

int OPENSCAP_CHECK_ENGINE_PLUGIN_ENTRY (struct check_engine_plugin_def *plugin)
{
	plugin->register_fn = sce_engine_register;
	plugin->cleanup_fn = sce_engine_cleanup;

	return 0;
}
