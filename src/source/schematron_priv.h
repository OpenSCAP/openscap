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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
 */
#ifndef OSCAP_SOURCE_SCHEMATRON_H
#define OSCAP_SOURCE_SCHEMATRON_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/public/oscap.h"
#include "common/util.h"
#include "source/public/oscap_source.h"


int oscap_source_validate_schematron_priv(struct oscap_source *source, oscap_document_type_t scap_type, const char *version);

#endif
