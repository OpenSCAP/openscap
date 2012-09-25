/*
 * Copyright 2009,2010,2011 Red Hat Inc., Durham, North Carolina.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 *      Peter Vrabec <pvrabec@redhat.com>
 */

/**
 * @file
 * General OpenScap functions and types.
 * @author Lukas Kuklinek <lkuklinek@redhat.com>
 *
 * @addtogroup COMMON
 * @{
 */

#ifndef OSCAP_H_
#define OSCAP_H_
#include <stdbool.h>
#include <wchar.h>

#include "text.h"
#include "reference.h"
#include "reporter.h"

/**
 * Initialize OpenSCAP library.
 *
 * This is currently needed only in multithreaded applications
 * (needs to be called before any child threads are spawned)
 * or applications using the XSLT facility w/ EXSLT extensions.
 * However, it is a good practice to call this function
 * always at the beginning of the program execution.
 */
void oscap_init(void);

/**
 * Release library internal caches.
 *
 * This function should be called once you finish working with
 * any of the libraries included in OpenScap framework.
 * It frees internally allocated memory, e.g. cache of the XML parser.
 */
void oscap_cleanup(void);

/// Get version of the OpenSCAP library
const char *oscap_get_version(void);


/**
 * @addtogroup VALID
 * @{
 * XML schema based validation of XML representations of SCAP documents.
 */

/// SCAP document type identifiers
typedef enum oscap_document_type {
	OSCAP_DOCUMENT_OVAL_DEFINITIONS = 1,  ///< OVAL Definitions file
	OSCAP_DOCUMENT_OVAL_VARIABLES,        ///< OVAL Variables
	OSCAP_DOCUMENT_OVAL_SYSCHAR,          ///< OVAL system characteristics file
	OSCAP_DOCUMENT_OVAL_RESULTS,          ///< OVAL results file
	OSCAP_DOCUMENT_OVAL_DIRECTIVES,       ///< OVAL directives file
	OSCAP_DOCUMENT_XCCDF,                 ///< XCCDF benchmark file
	OSCAP_DOCUMENT_CPE_LANGUAGE,          ///< CPE language file
	OSCAP_DOCUMENT_CPE_DICTIONARY,        ///< CPE dictionary file
	OSCAP_DOCUMENT_SCE_RESULT,            ///< SCE result file
	OSCAP_DOCUMENT_SDS,                   ///< Source Data Stream file
	OSCAP_DOCUMENT_ARF                    ///< Result Data Stream file
} oscap_document_type_t;


/**
 * Validate a SCAP document file against a XML schema.
 *
 * Schemas are searched relative to path specified by the OSCAP_SCHEMA_PATH environment variable.
 * If the variable does not exist a default path is used (usually something like $PREFIX/share/openscap/schemas).
 *
 * Directory structure must adhere $SCHEMA_PATH/$STANDARD/$VERSION/$SCHEMAFILE.xsd structure, where $STANDARD
 * is oval, xccdf, etc., and $VERSION is a version of the standard.
 *
 * @param xmlfile File to be validated.
 * @param doctype Document type represented by the file.
 * @param version Version of the document, use NULL for library's default.
 * @param reporetr A reporter to by notified of encountered issues. Can be NULL, if a binary document validates / does not validate answer is satisfactonary.
 * @param arg Argument for the reporter.
 * @return 0 on pass; -1 error; 1 fail
 */
int oscap_validate_document(const char *xmlfile, oscap_document_type_t doctype, const char *version, oscap_reporter reporter, void *arg);

/**
 * Validate a SCAP document file against schematron rules.
 *
 * The rules are searched relative to path specified by the OSCAP_SCHEMA_PATH environment variable.
 * If the variable does not exist a default path is used (usually something like $PREFIX/share/openscap/schemas).
 *
 * @param xmlfile File to be validated.
 * @param doctype Document type represented by the file.
 * @param version Version of the document, use NULL for library's default.
 * @param outfile Report from schematron validation is written into the outfile. If NULL, stdou will be used.
 * @return 0 on pass; <0 error; >0 fail
 */
int oscap_schematron_validate_document(const char *xmlfile, oscap_document_type_t doctype, const char *version, const char *outfile);

/**
 * Apply a XSLT stylesheet to a XML file.
 *
 * Stylesheets are searched relative to path specified by the OSCAP_XSLT_PATH environment variable.
 * If the variable does not exist a default path is used (usually something like $PREFIX/share/openscap/schemas).
 *
 * @param xmlfile File to be transformed.
 * @param xsltfile XSLT filename
 * @param outfile Result file shall be written here (NULL for stdout).
 * @param params list of key-value pairs to pass to the stylesheet.
 * @return the number of bytes written or -1 in case of failure
 */
int oscap_apply_xslt(const char *xmlfile, const char *xsltfile, const char *outfile, const char **params);

/**
 * Function returns path used to locate OpenSCAP XML schemas
 */
const char * oscap_path_to_schemas(void);

/**
 * Function returns path used to locate OpenSCAP Schematron files
 */
const char * oscap_path_to_schematron(void);

/************************************************************/
/** @} validation group end */

/** @} */

#endif
