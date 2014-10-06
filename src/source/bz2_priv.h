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
#ifndef OSCAP_SOURCE_BZIP2_H
#define OSCAP_SOURCE_BZIP2_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_BZ2

#include "common/public/oscap.h"
#include "common/util.h"
#include <libxml/tree.h>

OSCAP_HIDDEN_START;

/**
 * Parse *.xml.bz2 file to XML DOM
 * @param filepath The path to *.xml.bz2 file
 * @returns DOM representation of the file
 */
xmlDoc *bz2_read_doc(const char *filepath);

/**
 * Recognize whether the file can be parsed by this
 * bz2 parser.
 * @param filepath The path to *.xml.bz2 file
 * @returns true if can be parsed.
 */
bool bz2_is_file_bzip(const char *filepath);

OSCAP_HIDDEN_END;

#endif
#endif
