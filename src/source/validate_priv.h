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
 *
 */
#ifndef OSCAP_SOURCE_VALIDATE_H
#define OSCAP_SOURCE_VALIDATE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/public/oscap.h"
#include "common/util.h"
#include "source/public/oscap_source.h"


struct oscap_schema_table_entry {
	oscap_document_type_t doc_type;
	const char *schema_version;
	const char *schema_path;
};

/**
 * validate given XML file
 * @return 0 on pass; -1 error; 1 fail
 */
int oscap_source_validate_priv(struct oscap_source *source, oscap_document_type_t doc_type, const char *version, xml_reporter reporter, void *user);

#endif
