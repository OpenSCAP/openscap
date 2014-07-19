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
#include "common/public/oscap.h"
#include "common/util.h"
#include "oscap_source_priv.h"

typedef enum oscap_source_type {
	OSCAP_SRC_FROM_USER_XML_FILE = 1,               ///< The source originated from XML supplied by user
	// TODO: node originated from DataStream
	// TODO: originated from bzip2ed file
	// TODO: originated from internal temp file (and this shall be later removed)
	// TODO: downloaded from an http address (XCCDF can refer to remote sources)
} oscap_source_type_t;

struct oscap_source {
	oscap_document_type_t scap_type;                ///< Type of SCAP document (XCCDF, OVAL, ...)
        struct {
		oscap_source_type_t type;               ///< Internal type of the oscap_source
		const char *filepath;                   ///< Filepath (if originated from file)
	} origin;                                       ///
};

struct oscap_source *oscap_source_new_from_file(const char *filepath)
{
	/* TODO: At the end of the day, this shall be the only place in
	 * the library  where a path to filename is set from the outside.
	 */
	struct oscap_source *source = (struct oscap_source *) oscap_calloc(1, sizeof(struct oscap_source));
	source->origin.filepath = oscap_strdup(filepath);
	source->origin.type = OSCAP_SRC_FROM_USER_XML_FILE;
	return source;
}

void oscap_source_free(struct oscap_source *source)
{
	if (source != NULL) {
		oscap_free(source->origin.filepath);
		oscap_free(source);
	}
}
