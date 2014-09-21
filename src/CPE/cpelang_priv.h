/**
 * @file cpelang_priv.h
 * \brief Interface to Common Platform Enumeration (CPE) Language.
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
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

#ifndef CPELANG_PRIV_H_
#define CPELANG_PRIV_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "cpe_lang.h"
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
	cpe_lang_oper_t oper;	///< operator
	union {
		struct oscap_list *expr;	///< array of subexpressions for operators
		struct cpe_name *cpe;	///< CPE for match operation
		struct {
			char* system;
			char* href;
			char* id;
		} check;
	} meta;			///< operation metadata
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
 * Function for parsing XML CPE language file
 * @param file filename
 */
struct cpe_lang_model *cpe_lang_model_parse_xml(const char *file);

/**
 * Parse function for CPE Lang model
 * @param reader xmlTextReaderPtr structure representing XML model
 */
struct cpe_lang_model *cpe_lang_model_parse(xmlTextReaderPtr reader);

/**
 * Parse CPE platform structure
 * @param reader xmlTextReaderPtr structure representing XML model
 * @return cpe_platform structure with CPE platform item
 */
struct cpe_platform *cpe_platform_parse(xmlTextReaderPtr reader);

/**
 * Parse CPE test expression structure
 * @param reader xmlTextReaderPtr structure representing XML model
 * @return cpe_testexpr structure with CPE test expression item
 */
struct cpe_testexpr *cpe_testexpr_parse(xmlTextReaderPtr reader);

/**
 * Function for export CPE language model to XML
 * @param spec CPE language model structure
 * @param file filename
 */
void cpe_lang_model_export_xml(const struct cpe_lang_model *spec, const char *file);

/**
 * Function for export CPE language top element
 * @param writer xmlTextWriterPtr structure representing XML model
 * @param spec CPE language model structure
 */
void cpe_lang_export(const struct cpe_lang_model *spec, xmlTextWriterPtr writer);

/**
 * Function for export CPE platform element
 * @param writer xmlTextWriterPtr structure representing XML model
 * @param platform CPE platform structure
 */
void cpe_platform_export(const struct cpe_platform *platform, xmlTextWriterPtr writer);

/**
 * Function for export CPE test expression element
 * @param writer xmlTextWriterPtr structure representing XML model
 * @param expr CPE test expression structure
 */
void cpe_testexpr_export(const struct cpe_testexpr *expr, xmlTextWriterPtr writer);

char *cpe_lang_model_detect_version_priv(xmlTextReader *reader);

/** 
 * @cond INTERNAL
 */
OSCAP_HIDDEN_END;
 /* @endcond */

#endif
