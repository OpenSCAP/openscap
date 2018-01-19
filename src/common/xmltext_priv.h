/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 *	Šimon Lukašík
 *
 */

#pragma once
#ifndef _OSCAP_XMLTEXT_PRIV_H
#define _OSCAP_XMLTEXT_PRIV_H

#include "util.h"
#include <libxml/xmlreader.h>


/**
 * Function that jump to next XML starting element.
 * @param reader xmlTextReader to move with
 * @returns zero on success
 */
int xmlTextReaderNextElement(xmlTextReaderPtr reader);

/**
 * Function testing reader function
 * @param reader xmlTextReader
 * @returns zero on success
 */
int xmlTextReaderNextNode(xmlTextReaderPtr reader);

/**
 * Function that jump to next XML starting element. This function makes sure
 * we don't go past end tag of given element.
 * @param reader xmlTextReader to move with
 * @param end_tag name of the end element
 * @returns zero on success
 */
int xmlTextReaderNextElementWE(xmlTextReaderPtr reader, xmlChar* end_tag);


#endif
