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
 * OpenSCAP error reporting & logging mechanism.
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
#define OSCAP_EFAMILY_OSCAP    3	/**< OSCAP errors */
#define OSCAP_EFAMILY_OVAL     4	/**< OVAL errors (OVAL & probes) */
/** @} */

/**
 * @name OSCAP family error codes
 * @{
 * @see OSCAP_EFAMILY_OSCAP
 */
#define OSCAP_ENONE         0	/**< None */
#define OSCAP_EALLOC        1	/**< OSCAP allocation error */
#define OSCAP_EXMLELEM      2	/**< Unknown element in XML */
#define OSCAP_EXMLNODETYPE  3	/**< Bad node type in XML */
#define OSCAP_EXMLNOELEMENT 4	/**< No expected element */
#define OSCAP_ECPEINVOP     5	/**< Invalid CPE Language operation */
#define OSCAP_EOVALINT      6	/**< OVAL internal error */
#define OSCAP_EREGEXP       7	/**< Error in regexp compilation */
#define OSCAP_EREGEXPCOMP   8	/**< Invalid string comparison in regexps */
#define OSCAP_EUNDATATYPE   9	/**< Unsupported data type */
#define OSCAP_EINVARG       10	/**< Invalid argument */
#define OSCAP_EVARTYPE      11	/**< Variable type is not valid */
#define OSCAP_ESEAPINIT     12	/**< Failed seap initialization */
#define OSCAP_EUSER1        201	/**< User defined error */
#define OSCAP_EUSER2        202	/**< User defined error */
#define OSCAP_EUNKNOWN      255	/**< Unknown/Unexpected error */
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
