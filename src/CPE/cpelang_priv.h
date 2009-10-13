/**
 * @file cpelang_priv.h
 * \brief Interface to Common Platform Enumeration (CPE) Language
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
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


#include <libxml/xmlreader.h>

void parse_file(const char *fname);
struct cpe_platformspec * parse_platformspec(xmlTextReaderPtr reader);
struct cpe_platform * parse_platform(xmlTextReaderPtr reader);
struct cpe_lang_expr * parse_ret_expr(xmlTextReaderPtr reader);
char * parse_text_element(xmlTextReaderPtr reader, char *name);
void print_node(xmlTextReaderPtr reader);
