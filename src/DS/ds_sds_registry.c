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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/alloc.h"
#include "common/elements.h"
#include "common/_error.h"
#include "common/public/oscap.h"
#include "common/util.h"
#include "ds_sds_registry.h"
#include "sds_index_priv.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

struct ds_sds_registry {
	struct oscap_source *source;            ///< Source DataStream raw representation
	struct ds_sds_index *index;             ///< Source DataStream index
};

struct ds_sds_registry *ds_sds_registry_new_from_source(struct oscap_source *source)
{
	if (oscap_source_get_scap_type(source) != OSCAP_DOCUMENT_SDS) {
		return NULL;
	}
	struct ds_sds_registry *sds_registry = (struct ds_sds_registry *) oscap_calloc(1, sizeof(struct ds_sds_registry));
	sds_registry->source = source;
	return sds_registry;
}

void ds_sds_registry_free(struct ds_sds_registry *sds_registry)
{
	if (sds_registry != NULL) {
		ds_sds_index_free(sds_registry->index);
		oscap_source_free(sds_registry->source);
		oscap_free(sds_registry);
	}
}

struct ds_sds_index *ds_sds_registry_get_sds_idx(struct ds_sds_registry *registry)
{
	if (registry->index == NULL) {
		xmlTextReader *reader = oscap_source_get_xmlTextReader(registry->source);
		if (reader == NULL) {
			return NULL;
		}
		registry->index = ds_sds_index_parse(reader);
		xmlFreeTextReader(reader);
	}
	return registry->index;
}
