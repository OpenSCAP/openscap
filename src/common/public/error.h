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
 */

/**
 * @file
 *
 * @addtogroup COMMON
 * @{
 * @defgroup ERRORS Error reporting
 * @{
 * OpenSCAP error reporting mechanism.
 */
#pragma once
#ifndef OSCAP_ERROR_H
#define OSCAP_ERROR_H

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t oscap_errfamily_t;
typedef uint16_t oscap_errcode_t;

/**
 * @name Error levels
 * @{
 * Error level (as defined in XML error handler library).
 * Maybe for future use.
 */
#define OSCAP_ELEVEL_NONE      0	/**< None */
#define OSCAP_ELEVEL_WARNING   1	/**< A simple warning */
#define OSCAP_ELEVEL_ERROR     2	/**< A recoverable error */
#define OSCAP_ELEVEL_FATAL     3	/**< A fatal error */
/** @} */

/**
 * @name Error families
 * @{
 */
#define OSCAP_EFAMILY_NONE     0	/**< None */
#define OSCAP_EFAMILY_GLIBC    1	/**< Errno errors */
#define OSCAP_EFAMILY_XML      2	/**< Libxml - xmlerror errors */
#define OSCAP_EFAMILY_OSCAP    3	/**< OSCAP general errors */
#define OSCAP_EFAMILY_OVAL     4	/**< OVAL errors (OVAL & probes) */
#define OSCAP_EFAMILY_CPE      5	/**< CPE errors */
/** @} */

/**
 * @name OSCAP family error codes
 * @{
 * @see OSCAP_EFAMILY_OSCAP
 */
#define OSCAP_ENONE         0	/**< None */
#define OSCAP_EALLOC        1	/**< Memory allocation error */
#define OSCAP_EXMLELEM      2	/**< Unknown XML element */
#define OSCAP_EREGEXP       3	/**< Error in regexp compilation */
#define OSCAP_EINVARG       4	/**< Function called with invalid argument */
#define OSCAP_ENOTIMPL      254 /**< Not implemented*/
#define OSCAP_EUSER1        255 /**< User defined */
/** @} */


/**
 * @name OVAL family error codes
 * @{
 * @see OSCAP_EFAMILY_OVAL
 */
#define OVAL_EOVALINT        1	  /**< OVAL internal error */

#define OVAL_EPROBECONTEXT   255  /**< Invalid probe context */
#define OVAL_EPROBEINIT      256
#define OVAL_EPROBECONN      257
#define OVAL_EPROBENOTSUPP   258
#define OVAL_EPROBEOBJINVAL  259
#define OVAL_EPROBEITEMINVAL 260
#define OVAL_EPROBENODATA    261
#define OVAL_EPROBECLOSE     262
#define OVAL_EPROBESEND      263
#define OVAL_EPROBERECV      264
#define OVAL_EPROBEUNKNOWN   511
/** @} */

/**
 * @name CPE family error codes
 * @{
 * @see OSCAP_EFAMILY_CPE
 */
#define CPE_EOPERATION        4   /**< Invalid CPE Language operation */
/** @} */


#define oscap_seterr(family, code, desc) __oscap_seterr (__FILE__, __LINE__, __PRETTY_FUNCTION__, family, code, desc)


/**
 * @name Error manipulation functions
 * @{
 */

void __oscap_seterr(const char *file, uint32_t line, const char *func,
		    oscap_errfamily_t family, oscap_errcode_t code, const char *desc);

/**
 * Clear an error.
 */
void oscap_clearerr(void);

/**
 * Check for an error.
 */
bool oscap_err(void);

/**
 * Get last error family.
 */
oscap_errfamily_t oscap_err_family(void);

/**
 * Get last error code.
 */
oscap_errcode_t oscap_err_code(void);

/**
 * Get last error description.
 */
const char *oscap_err_desc(void);

/** @} */

#endif				/* OSCAP_ERROR_H */
