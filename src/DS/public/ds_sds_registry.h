/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
 * Author:
 *     Šimon Lukašík
 */
#ifndef DS_SDS_SOURCE_H
#define DS_SDS_SOURCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "oscap.h"
#include "oscap_source.h"

/**
 * The ds_sds_registry is structure tight closely to oscap_source.
 *
 * The ds_sds_registry represents resource for further processing representing
 * Source DataStream (file). The ds_sds_registry can be decomposed into multiple
 * oscap_source structures.
 *
 */
struct ds_sds_registry;

/**
 * Create new ds_sds_registry from existing oscap_source. This assumes that
 * the given oscap_source represents source DataStream. This function takes
 * over the ownership of oscap_source structure.
 * @memberof ds_sds_registry
 * @param source The oscap_source representing a source datastream
 * @returns newly created ds_sds_registry structure
 */
struct ds_sds_registry *ds_sds_registry_new_from_source(struct oscap_source *source);

/**
 * Dispose ds_sds_registry structure.
 * @param sds_registry Registry to dispose
 */
void ds_sds_registry_free(struct ds_sds_registry *sds_registry);

#endif
