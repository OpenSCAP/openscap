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

#include <libgen.h>
#include <string.h>

int OPENSCAP_EXTRA_CHECK_ENGINE_ENTRY (struct xccdf_policy_model *model, const char *xccdf_path);

int OPENSCAP_EXTRA_CHECK_ENGINE_ENTRY (struct xccdf_policy_model *model, const char *xccdf_path)
{
	//if (session->sce.parameters != NULL)
	//	sce_parameters_free(session->sce.parameters);

	// FIXME: This leaks memory!
	struct sce_parameters *parameters = sce_parameters_new();
	char *xccdf_pathcopy = strdup(xccdf_path);
	sce_parameters_set_xccdf_directory(parameters, dirname(xccdf_pathcopy));
	sce_parameters_allocate_session(parameters);
	free(xccdf_pathcopy);

	return !xccdf_policy_model_register_engine_sce(model, parameters);
}
