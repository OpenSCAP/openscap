/**
 * @addtogroup CPE
 * @{
 * @addtogroup cpelang CPE Language
 * @{
 * 
 * 
 * @file cpelang_priv.h
 * \brief Interface to Common Platform Enumeration (CPE) Language.
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
 */

/**
 * @addtogroup CPELangPrivate Private members
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

#ifndef CPELANG_PRIV_H_
#define CPELANG_PRIV_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "cpelang.h"
#include "../common/util.h"
#include "../common/list.h"
#include "../common/elements.h"
#include "../common/public/oscap.h"

/** 
 * @cond INTERNAL
 */
OSCAP_HIDDEN_START;
 /* @endcond */

/**
 * @struct cpe_testexpr
 * CPE language boolean expression
 */
struct cpe_testexpr {
        struct xml_metadata xml;
	cpe_lang_oper_t oper;	///< operator
	union {
		struct cpe_testexpr *expr;	///< array of subexpressions for operators
		struct cpe_name *cpe;	        ///< CPE for match operation
	} meta;			                ///< operation metadata
};

/**
 * @struct cpe_lang_model
 * CPE platform specification
 */
struct cpe_lang_model;

/**
 * @struct cpe_platform
 * Single platform representation in CPE language
 */
struct cpe_platform;

/**
 * @name Parse functions
 * Functions for parsing structures from XML. Return value is new structure filled by content of XML file
 * represented by XML reader or import source structure. Structure needs to be freed by the caller.
 * @{
 * */

/**
 * Function for parsing XML CPE language file
 * @param source structure of import source with filename and encoding
 * @relates cpe_lang_model
 */
struct cpe_lang_model * cpe_lang_model_parse_xml(const struct oscap_import_source * source);

/**
 * Parse function for CPE Lang model
 * @param reader xmlTextReaderPtr structure representing XML model
 * @relates cpe_lang_model
 */
struct cpe_lang_model * cpe_lang_model_parse(xmlTextReaderPtr reader);

/**
 * Parse CPE platform structure
 * @param reader xmlTextReaderPtr structure representing XML model
 * @return cpe_platform structure with CPE platform item
 * @relates cpe_platform
 */
struct cpe_platform * cpe_platform_parse(xmlTextReaderPtr reader);

/**
 * Parse CPE test expression structure
 * @param reader xmlTextReaderPtr structure representing XML model
 * @return cpe_testexpr structure with CPE test expression item
 * @relates cpe_testexpr
 */
struct cpe_testexpr * cpe_testexpr_parse(xmlTextReaderPtr reader);

/*@}*/

/**
 * @name Export functions
 * Functions for export structures to XML by XML writer. Exported structures need to be freed by the caller.
 * @{
 * */

/**
 * Function for export CPE language model to XML
 * @param target structure with exporting information such filename and encoding
 * @param spec CPE language model structure
 * @relates cpe_lang_model
 */
void cpe_lang_model_export_xml(const struct cpe_lang_model * spec, struct oscap_export_target * target);

/**
 * Function for export CPE language top element
 * @param writer xmlTextWriterPtr structure representing XML model
 * @param spec CPE language model structure
 * @relates cpe_lang_model
 */
void cpe_lang_export(const struct cpe_lang_model * spec, xmlTextWriterPtr writer);

/**
 * Function for export CPE platform element
 * @param writer xmlTextWriterPtr structure representing XML model
 * @param platform CPE platform structure
 * @relates cpe_platform
 */
void cpe_platform_export(const struct cpe_platform * platform, xmlTextWriterPtr writer);

/**
 * Function for export CPE test expression element
 * @param writer xmlTextWriterPtr structure representing XML model
 * @param expr CPE test expression structure
 * @relates cpe_testexpr
 */
void cpe_testexpr_export(const struct cpe_testexpr expr, xmlTextWriterPtr writer);

/*@}*/

/** 
 * @cond INTERNAL
 */
OSCAP_HIDDEN_END;
 /* @endcond */

/*@}*/

/*@}*/
/*@}*/

#endif

