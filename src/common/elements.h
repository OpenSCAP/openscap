/**
 * @file elements.h
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 *      Maros Barabas <mbarabas@redhat.com>
 */

#ifndef OSCAP_ELEMENTS_H_
#define OSCAP_ELEMENTS_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <stdbool.h>
#include "public/oscap.h"

/**
 * */
struct oscap_title {
	struct xml_metadata xml;
	char *content;		// human-readable name of this item
};

struct oscap_title *oscap_title_parse(xmlTextReaderPtr reader, const char *name);
void oscap_title_export(const struct oscap_title *title, xmlTextWriterPtr writer);
void oscap_title_free(struct oscap_title *title);

const char *oscap_import_source_get_name(const struct oscap_import_source *src);

#endif
