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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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

#include "oscap_text.h"
#include "oscap_reference.h"
#include "oscap_export.h"

/**
 * This macro will warn, when a deprecated function is used.
 */
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
# define OSCAP_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
# define OSCAP_DEPRECATED(func) __declspec(deprecated) func
#else
# pragma message("WARNING: You need to implement OSCAP_DEPRECATED for this compiler---in order to get deprecation warnings.")
# define OSCAP_DEPRECATED(func) func
#endif

/**
 * Initialize OpenSCAP library.
 *
 * This is currently needed only in multithreaded applications
 * (needs to be called before any child threads are spawned)
 * or applications using the XSLT facility w/ EXSLT extensions.
 * However, it is a good practice to call this function
 * always at the beginning of the program execution.
 */
OSCAP_API void oscap_init(void);

/**
 * Release library internal caches.
 *
 * This function should be called once you finish working with
 * any of the libraries included in OpenScap framework.
 * It frees internally allocated memory, e.g. cache of the XML parser.
 */
OSCAP_API void oscap_cleanup(void);

/// Get version of the OpenSCAP library
OSCAP_API const char *oscap_get_version(void);


/**
 * @addtogroup VALID
 * @{
 * XML schema based validation of XML representations of SCAP documents.
 */

/// SCAP document type identifiers
typedef enum oscap_document_type {
	OSCAP_DOCUMENT_UNKNOWN = 0,           ///< The type is unknown
	OSCAP_DOCUMENT_OVAL_DEFINITIONS,      ///< OVAL Definitions file
	OSCAP_DOCUMENT_OVAL_VARIABLES,        ///< OVAL Variables
	OSCAP_DOCUMENT_OVAL_SYSCHAR,          ///< OVAL system characteristics file
	OSCAP_DOCUMENT_OVAL_RESULTS,          ///< OVAL results file
	OSCAP_DOCUMENT_OVAL_DIRECTIVES,       ///< OVAL directives file
	OSCAP_DOCUMENT_XCCDF,                 ///< XCCDF benchmark file
	OSCAP_DOCUMENT_CPE_LANGUAGE,          ///< CPE language file
	OSCAP_DOCUMENT_CPE_DICTIONARY,        ///< CPE dictionary file
	OSCAP_DOCUMENT_CVE_FEED,              ///< CVE NVD feed
	OSCAP_DOCUMENT_SCE_RESULT,            ///< SCE result file
	OSCAP_DOCUMENT_SDS,                   ///< Source Data Stream file
	OSCAP_DOCUMENT_ARF,                   ///< Result Data Stream file
	OSCAP_DOCUMENT_XCCDF_TAILORING,       ///< XCCDF tailoring file
	OSCAP_DOCUMENT_OCIL,                   ///< OCIL Definitions file
	OSCAP_DOCUMENT_CVRF_FEED             ///< CVRF feed
	// If you are adding a new enum here, make sure you add support for it
	// to utils/oscap-info.c!
} oscap_document_type_t;

/// SCAP versions
typedef enum oscap_document_version {
	OSCAP_DOCUMENT_VERSION_UNKNOWN = 0,
	OSCAP_DOCUMENT_VERSION_1_0 = 10,
	OSCAP_DOCUMENT_VERSION_1_1 = 11,
	OSCAP_DOCUMENT_VERSION_1_2 = 12,
	OSCAP_DOCUMENT_VERSION_1_3 = 13,
	OSCAP_DOCUMENT_VERSION_2_0 = 20,
} oscap_document_version_t;

/**
 * Convert @ref oscap_document_version_t constant to a string
 * @param version OpenSCAP Document Version
 * @returns String with the document version.
 * @retval Returned value might be pointer to static memory and must not be modified.
 * @retval NULL in case of unrecognized document version
 */
OSCAP_API const char *oscap_document_version_to_string(oscap_document_version_t version);

/**
 * Convert @ref oscap_document_type_t constant to human readable (english)
 * representation.
 * @param type OpenSCAP Document Type
 * @returns English string describing document type.
 * @retval Returned value might be pointer to static memory and must not be modified.
 * @retval NULL in case of unrecognized document type
 */
OSCAP_API const char *oscap_document_type_to_string(oscap_document_type_t type);

typedef int (*xml_reporter)(const char *file, int line, const char *msg, void *arg);

/**
 * Apply a XSLT stylesheet to a XML file.
 *
 * If xsltfile is not an absolute path, the file will be searched relatively to a path specified by the OSCAP_XSLT_PATH environment variable.
 * If the variable does not exist a default path is used (usually something like $PREFIX/share/openscap/schemas).
 *
 * @param xmlfile File to be transformed.
 * @param xsltfile XSLT file
 * @param outfile Result file shall be written here (NULL for stdout).
 * @param params list of key-value pairs to pass to the stylesheet.
 * @return the number of bytes written or -1 in case of failure
 */
OSCAP_API int oscap_apply_xslt(const char *xmlfile, const char *xsltfile, const char *outfile, const char **params);

/**
 * Function returns path used to locate OpenSCAP XML schemas
 */
OSCAP_API const char * oscap_path_to_schemas(void);

/**
 * Function returns path used to locate OpenSCAP Default CPE files
 */
OSCAP_API const char * oscap_path_to_cpe(void);

/************************************************************/
/** @} validation group end */

/** @} */

#endif
