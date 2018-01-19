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
#ifndef OSCAP_SOURCE_PRIV_H
#define OSCAP_SOURCE_PRIV_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/parser.h>
#include <libxml/xmlreader.h>

#include "common/util.h"
#include "oscap.h"
#include "oscap_source.h"


/**
 * Create new oscap_source from raw memory. The memory can contain \0 bytes
 * and they are not considered NULL-terminations! Always pass the correct
 * size. This constructor is meant as a last resort when no other constructor
 * will work for your use case. If at all possible you should use the more
 * specialized constructors.
 * oscap_source will will not allocate memory new memory buffer
 * @param buffer Memory buffer with raw data
 * @param size size of the memory buffer
 * @param filepath Suggested filename for the file or NULL
 * @returns newly created oscap_source_structure
 */
struct oscap_source *oscap_source_new_take_memory(char *buffer, size_t size, const char *filepath);

/**
 * Build new oscap_source from existing xmlDoc. The xmlDoc becomes owned
 * by oscap_source.
 * @memberof oscap_source
 * @param doc XML DOM to build from
 * @param filepath Suggested filename for the file or NULL
 * @returns newly created oscap_source
 */
struct oscap_source *oscap_source_new_from_xmlDoc(xmlDoc *doc, const char *filepath);

/**
 * Get an xmlTextReader assigned with this resource. The reader needs to be
 * disposed by caller.
 * @memberof oscap_source
 * @param source Resource to read the content
 * @returns xmlTextReader structure to read the content
 */
xmlTextReader *oscap_source_get_xmlTextReader(struct oscap_source *source);

/**
 * Get a DOM representation of this resource. The document ins still owned
 * by oscap_source.
 * @memberof oscap_source
 * @param source Resource to build DOM representation from
 * @returns xmlDoc structure to read the content
 */
xmlDoc *oscap_source_get_xmlDoc(struct oscap_source *source);


#endif
