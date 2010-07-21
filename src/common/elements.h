/**
 * @file elements.h
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
/// convert a piece of XML string to DOM
xmlNode *oscap_xmlstr_to_dom(xmlNode *parent, const char *elname, const char *content);


#endif
