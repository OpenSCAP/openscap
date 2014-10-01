/**
 * @file elements.h
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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

#define OSCAP_XMLNS_XSI BAD_CAST "http://www.w3.org/2001/XMLSchema-instance"

/// boolean to string (and vice versa) conversion map
extern const struct oscap_string_map OSCAP_BOOL_MAP[];

typedef void (*oscap_xml_value_consumer) (char *, void *);
int oscap_parser_text_value(xmlTextReaderPtr reader, oscap_xml_value_consumer consumer, void *user);
void oscap_text_consumer(char *text, void *user);

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
/// convert a piece of XML string to DOM
xmlNode *oscap_xmlstr_to_dom(xmlNode *parent, const char *elname, const char *content);

/**
 * Save XML Document to the file of the given filename.
 * @param filename path to the file
 * @param doc the XML document content
 * @return 1 on success, -1 on failure (oscap_seterr is set appropriatly).
 */
int oscap_xml_save_filename(const char *filename, xmlDocPtr doc);

/**
 * Save XML Document to the file of the given filename and dispose the document afterwards.
 * @param filename path to the file
 * @param doc the XML document content
 * @return 1 on success, -1 on failure (oscap_seterr is set appropriatly).
 */
int oscap_xml_save_filename_free(const char *filename, xmlDocPtr doc);

xmlNs *lookup_xsi_ns(xmlDoc *doc);

#endif
