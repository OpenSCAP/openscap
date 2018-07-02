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

static int sce_engine_register(struct xccdf_policy_model *model, const char *path_hint, void **user_data)
{
	struct sce_parameters *parameters = (struct sce_parameters*) *user_data;
	if (parameters != NULL)
		sce_parameters_free(parameters);

	parameters = sce_parameters_new();
	sce_parameters_set_xccdf_directory(parameters, path_hint);
	sce_parameters_allocate_session(parameters);

	*user_data = (void*)parameters; // This way the data will get freed later

	return !xccdf_policy_model_register_engine_sce(model, parameters);
}

static int sce_engine_cleanup(struct xccdf_policy_model *model, void **user_data)
{
	struct sce_parameters *parameters = (struct sce_parameters*) *user_data;
	if (parameters != NULL)
		sce_parameters_free(parameters);

	*user_data = NULL;

	// FIXME: Unregister SCE?
	return 0;
}

static int sce_engine_export_results(struct xccdf_policy_model *model, bool validate, const char *path_hint, void **user_data)
{
	struct sce_parameters *parameters = (struct sce_parameters*) *user_data;
	if (parameters == NULL) {
		return 1;
	}

	struct sce_session *sce_session = sce_parameters_get_session(parameters);
	if (sce_session == NULL)
		return 1;
	struct sce_check_result_iterator *it = sce_session_get_check_results(sce_session);

	while(sce_check_result_iterator_has_more(it))
	{
		struct sce_check_result *check_result = sce_check_result_iterator_next(it);
		char target[2 + strlen(sce_check_result_get_basename(check_result)) + 11 + 1];
		snprintf(target, sizeof(target), "./%s.result.xml", sce_check_result_get_basename(check_result));
		sce_check_result_export(check_result, target);
	}

	sce_check_result_iterator_free(it);

	return 0;
}

static const char *sce_engine_get_capabilities(void **user_data)
{
	return "SCE Version: 1.0";
}

int OPENSCAP_CHECK_ENGINE_PLUGIN_ENTRY (struct check_engine_plugin_def *plugin);

int OPENSCAP_CHECK_ENGINE_PLUGIN_ENTRY (struct check_engine_plugin_def *plugin)
{
	plugin->register_fn = sce_engine_register;
	plugin->cleanup_fn = sce_engine_cleanup;
	plugin->export_results_fn = sce_engine_export_results;
	plugin->get_capabilities_fn = sce_engine_get_capabilities;

	return 0;
}
