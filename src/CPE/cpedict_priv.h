/**
 * @addtogroup CPE
 * @{
 * @addtogroup cpedict CPE Dictionary
 * @{
 * 
 * 
 * @file cpedict_priv.h
 * \brief Interface to Common Platform Enumeration (CPE) Dictionary.
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

#ifndef CPEDICT_PRIV_H_
#define CPEDICT_PRIV_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <stdlib.h>

#include "cpeuri.h"
#include "../common/public/oscap.h"
#include "../common/util.h"
#include "../common/elements.h"

OSCAP_HIDDEN_START;

/**
 * Function to parse XML to CPE dictionary model
 * @relates cpe_dict_model
 * @param source Structure of name and encoding of importing XML file
 */
struct cpe_dict_model * cpe_dict_model_parse_xml(const struct oscap_import_source *source);

/**
 * Parse generator part of CPE dictionary XML file
 * @param reader xmlTextReaderPtr representing XML model
 * @relates cpe_generator
 * @return new structure cpe_generator
 */
struct cpe_generator * cpe_generator_parse(xmlTextReaderPtr reader);

/**
 * New dictionary item from XML
 * @param node cpe-item node
 * @return new dictionary item
 * @retval NULL on failure
 */
struct cpe_item * cpe_item_parse(xmlTextReaderPtr reader);

/**
 * Parsing function to parse vendors of CPE dictionary
 * @param reader xmlTextReaderPtr representing XML model
 * @relates cpe_vendor
 * @return new structure cpe_vendor
 */
struct cpe_vendor * cpe_vendor_parse(xmlTextReaderPtr reader);

/**
 * Constructor of vendor structure
 * Creates new vendor item
 * @return new structure cpe_venor
 * @relates cpe_vendor
 */
struct cpe_vendor * cpe_vendor_new();

/**
 * Constructor of CPE item structure
 * Creates new CPE item
 * @returns new structure cpe_item
 * @relates cpe_item
 */
struct cpe_item * cpe_item_new();

/**
 * Create new empty CPE dictionary
 * @relates cpe_dict_model
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict_model *cpe_dict_model_new();

/**
 * Load new CPE dictionary from XML node
 * @param node file name of dictionary to import
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict_model * cpe_dict_model_parse(xmlTextReaderPtr reader);

/**
 * Export function for CPE dictionary model
 * @param dict CPE dictionary structure
 * @relates cpe_dict_model
 * @param target OSCAP exporting target
 * @relates oscap_export_target
 */
void cpe_dict_model_export_xml(struct cpe_dict_model * dict, const struct oscap_export_target * target);

/**
 * Internal export function for CPE dictionary model
 * @param dict CPE dictionary
 * @param writer xmlTextWriterPtr representing XML model
 * @relates cpe_dict_model
 */
void cpe_dict_export(const struct cpe_dict_model * dict, xmlTextWriterPtr writer);

/**
 * Exporting function for CPE generator - information of XML document
 * @param generator CPE generator structure
 * @relates cpe_generator
 * @param writer xmlTextWriterPtr representing XML model
 */
void cpe_generator_export(const struct cpe_generator * generator, xmlTextWriterPtr writer);

/**
 * Exporting function for CPE item
 * @param item CPE item structure
 * @relates cpe_item
 * @param writer xmlTextWriterPtr representing XML model
 */
void cpe_item_export(const struct cpe_item * item, xmlTextWriterPtr writer);

/**
 * Exporting function for CPE vendor
 * @param vendor CPE vendor structure
 * @relates cpe_vendor
 * @param writer xmlTextWriterPtr representing XML model
 */
void cpe_vendor_export(const struct cpe_vendor * vendor, xmlTextWriterPtr writer);

OSCAP_HIDDEN_END;

#endif

