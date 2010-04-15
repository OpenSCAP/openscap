/**
 * @addtogroup CVE
 * @{
 *
 * @file cve_priv.h
 * \brief Common Vulnerability and Exposure dictionary
 * 
 *   See details at:
 *     http://cve.mitre.org/
 *     http://nvd.nist.gov/download.cfm
 *  
 */

/**
 * @addtogroup CVEPrivate Private members
 * @{
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

#ifndef CVE_PRIV_H_
#define CVE_PRIV_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "../common/list.h"
#include "../common/elements.h"

/** 
 * @cond INTERNAL
 */
OSCAP_HIDDEN_START;
 /* @endcond */

/** 
 * @struct cve_model
 * Structure holding CVE model
 */
struct cve_model;
/** 
 * @struct cve_entry
 * Structure holding CVE entry data
 */
struct cve_entry;
/** 
 * @struct cve_summary
 * Structure holding CVE summary data
 */
struct cve_summary;
/** 
 * @struct cve_product
 * Structure holding CVE product data
 */
struct cve_product;
/** 
 * @struct cve_configuration
 * Structure CVE vulnerable configuration data
 */
struct cve_configuration;
/** 
 * @struct cwe_entry
 * Structure holding CWE data
 */
struct cwe_entry;
/** 
 * @struct cve_reference
 * Structure holding CVE reference data
 */
struct cve_reference;

/**
 * @name Parse functions
 * Functions for parsing structures from XML. Return value is new structure filled by content of XML file
 * represented by XML reader or import source structure. Structure needs to be freed by the caller.
 * @{
 * */

/**
 * Parse CVE model from XML (private function)
 * @param source OSCAP import source
 * @memberof cve_model
 * @return new CVE model
 */
struct cve_model *cve_model_parse_xml(const struct oscap_import_source *source);

/**
 * Parse CVE model
 * @param reader XML Text Reader representing XML model
 * @memberof cve_model
 * @return parsed CVE mdoel
 */
struct cve_model *cve_model_parse(xmlTextReaderPtr reader);

/**
 * Parse CVE entry
 * @param reader XML Text Reader representing XML model
 * @memberof cve_entry
 * @return parsed CVE entry
 */
struct cve_entry *cve_entry_parse(xmlTextReaderPtr reader);

/*@}*/

/**
 * @name Export functions
 * Functions for export structures to XML by XML writer. Exported structures need to be freed by the caller.
 * @{
 * */

/**
 * Export CVE model to XML file
 * @param cve CVE model
 * @param writer XML Text Writer representing XML model
 * @memberof cve_model
 */
void cve_export(const struct cve_model *cve, xmlTextWriterPtr writer);

/**
 * Export CVE model to XML file
 * @param cve CVE model
 * @param target OSCAP export target
 * @memberof cve_model
 */
void cve_model_export_xml(struct cve_model *cve, const struct oscap_export_target *target);

/**
 * Export CVE reference to XML file
 * @param refer CVE reference
 * @param writer XML Text Writer representing XML model
 * @memberof cve_reference
 */
void cve_reference_export(const struct cve_reference *refer, xmlTextWriterPtr writer);

/**
 * Export CVE summary to XML file
 * @param sum CVE summary
 * @param writer XML Text Writer representing XML model
 * @memberof cve_summary
 */
void cve_summary_export(const struct cve_summary *sum, xmlTextWriterPtr writer);

/**
 * Export CVE entry to XML file
 * @param entry CVE entry
 * @param writer XML Text Writer representing XML model
 * @memberof cve_entry
 */
void cve_entry_export(const struct cve_entry *entry, xmlTextWriterPtr writer);

/** 
 * @cond INTERNAL
 */
OSCAP_HIDDEN_END;
 /* @endcond */

/*@}*/

/*@}*/
/*@}*/

#endif
