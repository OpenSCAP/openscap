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
#include <time.h>
#include "public/oscap.h"
#include "util.h"

/// boolean to string (and vice versa) conversion map
extern const struct oscap_string_map OSCAP_BOOL_MAP[];

/// find starting element at given depth (returns false if none found)
bool oscap_to_start_element(xmlTextReaderPtr reader, int depth);
/// get a copy of a string contained by current element
char *oscap_element_string_copy(xmlTextReaderPtr reader);
/// get a string contained by current element
const char *oscap_element_string_get(xmlTextReaderPtr reader);
/// get depth of current element
int oscap_element_depth(xmlTextReaderPtr reader);
/// get xml content of current element as a string
char *oscap_get_xml(xmlTextReaderPtr reader);
/// get date from a string
time_t oscap_get_date(const char *date);
/// get datetime from a string
time_t oscap_get_datetime(const char *date);


struct xml_metadata {
        char *nspace;           ///< XMLNS (namespace) prefix
        char *URI;              ///< XMLNS (namespace) URI
        char *lang;             ///< XML lang
};

struct oscap_title {
        struct xml_metadata xml;
        char *content;          // human-readable name of this item
};

struct oscap_title *oscap_title_parse(xmlTextReaderPtr reader, const char *name);
void oscap_title_export(const struct oscap_title *title, xmlTextWriterPtr writer);
void oscap_title_free(struct oscap_title *title);

const char *oscap_import_source_get_name(const struct oscap_import_source *src);


/// namespace information entry
struct oscap_nsinfo_entry {
	char *nsprefix;        ///< namespace prefix
	char *nsname;          ///< namespace name
	char *schema_location; ///< schema location for the namespace
};

/// namespace information extracted from the root element
struct oscap_nsinfo {
	struct oscap_list *entries;            ///< list of 'struct oscap_nsinfo_entry'
	struct oscap_nsinfo_entry *root_entry; ///< namespace info entry for the root element
};

#endif
