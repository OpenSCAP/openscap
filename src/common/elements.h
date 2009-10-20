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

/* XML Metadata. Here should be every general attribute
 * that can be present in every xml element such as xml:lang
 * or xml namespace.
 * */
struct xml_metadata {
        char *namespace;
        char *lang;
};

/**
 * */
struct oscap_title {
        struct xml_metadata xml;
	char *content;		        // human-readable name of this item
};

struct oscap_title * oscap_title_parse(xmlTextReaderPtr reader, const char * name);
void oscap_title_export(const struct oscap_title * title, xmlTextWriterPtr writer);
void oscap_title_free(struct oscap_title * title);

struct oscap_title_iterator;
struct oscap_title* oscap_title_iterator_next(struct oscap_title_iterator* it);
void oscap_title_iterator_free(struct oscap_title_iterator* it);
bool oscap_title_iterator_has_more(struct oscap_title_iterator* it);
 
void xml_metadata_free(struct xml_metadata xml);

#endif
