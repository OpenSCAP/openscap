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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
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
#include "reporter.h"


/**
 * @addtogroup ITER
 * @{
 *
 * Iterators concept.
 *
 * Any iterator name takes a form of <tt>struct OBJECT_iterator</tt>, where @c OBJECT
 * is a name of particular datatype the iterator iterates over.
 *
 * Each iterator type defines several manipulation functions, namely:
 * - @c OBJECT_iterator_has_more - returns true if there is anything left to iterate over
 * - @c OBJECT_iterator_next     - returns next item in the collection
 * - @c OBJECT_iterator_free     - destroys the iterator
 *
 * You can also use @ref OSCAP_FOREACH convience macro.
 */

/**
 * Iterate over an array, given an iterator.
 * Execute @a code for each array member stored in @a val.
 * It is NOT safe to use return or goto inside of the @a code,
 * the iterator would not be freed properly.
 */
#define OSCAP_FOREACH_GENERIC(itype, vtype, val, init_val, code) \
    {                                                            \
        struct itype##_iterator *val##_iter = (init_val);        \
        vtype val;                                               \
        while (itype##_iterator_has_more(val##_iter)) {          \
            val = itype##_iterator_next(val##_iter);             \
            code                                                 \
        }                                                        \
        itype##_iterator_free(val##_iter);                       \
    }

/**
 * Iterate over an array, given an iterator.
 * @param type type of array elements (w/o the struct keyword)
 * @param val name of an variable the member will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @param code code to be executed for each element the iterator hits
 * @see OSCAP_FOREACH_GENERIC
 */
#define OSCAP_FOREACH(type, val, init_val, code) \
        OSCAP_FOREACH_GENERIC(type, struct type *, val, init_val, code)

/**
 * Iterate over an array of strings, given an iterator.
 * @param val name of an variable the string will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @param code code to be executed for each string the iterator hits
 * @see OSCAP_FOREACH_GENERIC
 */
#define OSCAP_FOREACH_STR(val, init_val, code) \
        OSCAP_FOREACH_GENERIC(oscap_string, const char *, val, init_val, code)

/**
 * Iterate over an array, given an iterator.
 * It is generally not safe to use break, return or goto inside the loop
 * (iterator wouldn't be properly freed otherwise).
 * Two variables, named VAL and VAL_iter (substitute VAL for actual macro argument)
 * will be added to current variable scope. You can free the iterator explicitly
 * after previous unusual escape from the loop (e.g. using break).
 * @param val name of an variable the string will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @param code code to be executed for each string the iterator hits
 */
#define OSCAP_FOR_GENERIC(itype, vtype, val, init_val)                  \
    vtype val = NULL; struct itype##_iterator *val##_iter = (init_val); \
    while (itype##_iterator_has_more(val##_iter)                        \
            ? (val = itype##_iterator_next(val##_iter), true)           \
            : (itype##_iterator_free(val##_iter), val##_iter = NULL, false))

/**
 * Iterate over an array, given an iterator.
 * @param type type of array elements (w/o the struct keyword)
 * @param val name of an variable the member will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @see OSCAP_FOR_GENERIC
 */
#define OSCAP_FOR(type, val, init_val) OSCAP_FOR_GENERIC(type, struct type *, val, init_val)

/**
 * Iterate over an array of strings, given an iterator.
 * @param val name of an variable the member will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @see OSCAP_FOR_GENERIC
 */
#define OSCAP_FOR_STR(val, init_val) OSCAP_FOR_GENERIC(oscap_string, const char *, val, init_val)

/** @} */


/**
 * Release library internal caches.
 *
 * This function should be called once you finish working with
 * any of the libraries included in OpenScap framework.
 * It frees internally allocated memory, e.g. cache of the XML parser.
 */
void oscap_cleanup(void);



/**
 * @addtogroup VALID
 * @{
 * XML schema based validation of XML representations of SCAP documents.
 */

/// SCAP document type identifiers
typedef enum oscap_document_type {
	OSCAP_DOCUMENT_OVAL_DEFINITIONS = 1,  ///< OVAL Definitions file
	OSCAP_DOCUMENT_OVAL_SYSCHAR,          ///< OVAL system characteristics file
	OSCAP_DOCUMENT_OVAL_RESULTS,          ///< OVAL results file
	OSCAP_DOCUMENT_XCCDF,                 ///< XCCDF benchmark file
	OSCAP_DOCUMENT_CPE_LANGUAGE,          ///< CPE language file
	OSCAP_DOCUMENT_CPE_DICTIONARY,        ///< CPE dictionary file
} oscap_document_type_t;


/**
 * Validate a SCAP document file against a XML schema.
 *
 * Schemas are searched relative to path specified by the OSCAP_SCHEMA_PATH environment variable,
 * which contains a list of colon-separated paths.
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
 * @return Success or failure.
 */
bool oscap_validate_document(const char *xmlfile, oscap_document_type_t doctype, const char *version, oscap_reporter reporter, void *arg);

/**
 * Apply a XSLT stylesheet to a XML file.
 *
 * Stylesheets are searched relative to path specified by the OSCAP_XSLT_PATH environment variable,
 * which contains a list of colon-separated paths.
 * If the variable does not exist a default path is used (usually something like $PREFIX/share/openscap/schemas).
 *
 * @param xmlfile File to be transformed.
 * @param xsltfile XSLT filename
 * @param outfile Result file shall be written here (NULL for stdout).
 * @param params list of key-value pairs to pass to the stylesheet.
 * @return Success or failure.
 */
bool oscap_apply_xslt(const char *xmlfile, const char *xsltfile, const char *outfile, const char **params);

/************************************************************/
/** @} validation group end */

/** @} */

#endif
