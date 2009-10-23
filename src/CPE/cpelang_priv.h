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

#ifndef CPELANG_PRIV_H_
#define CPELANG_PRIV_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "cpelang.h"
#include "../common/util.h"
#include "../common/elements.h"

OSCAP_HIDDEN_START;

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

/**
 * Function for export CPE language model to XML
 * @param target structure with exporting information such filename and encoding
 * @param spec CPE language model structure
 * @relates cpe_lang_model
 */
void cpe_lang_model_export_xml(struct cpe_lang_model * spec, struct oscap_export_target * target);

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

/**
 * Constructor of CPE language model structure
 * @return new structure of cpe_lang_model
 * @relates cpe_lang_model
 */
struct cpe_lang_model * cpe_lang_model_new();

/**
 * Constructor of CPE test expression structure
 * @return new structure of cpe_testexpr
 * @relates cpe_testexpr
 */
struct cpe_testexpr * cpe_testexpr_new();

/**
 * Constructor of CPE platform structure
 * @return new structure of cpe_platform
 * @relates cpe_platform
 */
struct cpe_platform * cpe_platform_new();

OSCAP_HIDDEN_END;

#endif

