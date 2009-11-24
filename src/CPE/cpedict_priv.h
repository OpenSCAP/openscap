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

/**
 * @addtogroup CPEDictPrivate Private members
 * @{
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

/** 
 * @cond INTERNAL
 */
OSCAP_HIDDEN_START;
 /* @endcond */

/**
 * @struct cpe_check
 * Structure representing single CPE check.
 */
struct cpe_check;

/**
 * @struct cpe_reference
 * CPE dictionary item reference.
 */
struct cpe_reference;

/**
 * @struct cpe_item
 * Structure representing single CPE dictionary item.
 */
struct cpe_item;

/**
 * @struct cpe_dict_model
 * Structure representing a CPE dictionary.
 */
struct cpe_dict_model;

/**
 * @struct cpe_item_metadata
 * Structure representing metadata of CPE item
 */
struct cpe_item_metadata;

/**
 * @struct cpe_generator
 * Structure with information about document
 */
struct cpe_generator;

/**
 * @struct cpe_vendor
 * Structure with information about vendor
 */
struct cpe_vendor;
/**
 * @struct cpe_product
 * Product of some vendor
 */
struct cpe_product;
/**
 * @struct cpe_version
 * Version of product
 */
struct cpe_version;
/**
 * @struct cpe_update
 * Update of product version
 */
struct cpe_update;
/**
 * @struct cpe_edition
 * Edition of product update
 */
struct cpe_edition;
/**
 * @struct cpe_language
 * Language of product edition
 */
struct cpe_language;

/**
 * @name Parse functions
 * Functions for parsing structures from XML. Return value is new structure filled by content of XML file
 * represented by XML reader or import source structure. Structure needs to be freed by the caller.
 * @{
 * */

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
 * @relates cpe_item
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
 * Load new CPE dictionary from XML node
 * @relates cpe_dict_model
 * @param node file name of dictionary to import
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict_model * cpe_dict_model_parse(xmlTextReaderPtr reader);

/*@}*/

/**
 * @name Export functions
 * Functions for export structures to XML by XML writer. Exported structures need to be freed by the caller.
 * @{
 * */

/**
 * Export function for CPE dictionary model
 * @param dict CPE dictionary structure
 * @relates cpe_dict_model
 * @param target OSCAP exporting target
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
/*@}*/

/** 
 * @cond INTERNAL
 */
OSCAP_HIDDEN_END;
 /* @endcond */

/** @} */
/** @} */
/** @} */

#endif

