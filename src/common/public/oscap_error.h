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
 * @addtogroup ERRORS
 * @{
 * Error checking mechanism. Purse of this mechanism is to inform user about problems that occured 
 * during executaion of library functions. Mechanism is similar to linux errno variable. When the problem 
 * raise, the information about it is stored in library buffer. This information consists of error family,
 * and textual description. Example of usage:
 *
 * @code
 * syschar = oval_probe_object_eval (pctx, object);
 * if (syschar == NULL && oscap_err()) {
 *     printf("Error: (%d) %s\n", oscap_err_family(), oscap_err_desc());
 * }
 * oscap_clearerr()
 * @endcode
 *
 */
#pragma once
#ifndef OSCAP_ERROR_H
#define OSCAP_ERROR_H

#include <stdint.h>
#include <stdbool.h>

/// Error family type
typedef uint16_t oscap_errfamily_t;

/**
 * @name OpenSCAP error families
 * @{
 */
#define OSCAP_EFAMILY_NONE     0	/**< None */
#define OSCAP_EFAMILY_GLIBC    1	/**< Errno errors */
#define OSCAP_EFAMILY_XML      2	/**< Libxml  errors */
#define OSCAP_EFAMILY_OSCAP    3	/**< OSCAP general errors */
#define OSCAP_EFAMILY_OVAL     4	/**< OVAL errors (OVAL & probes) */
#define OSCAP_EFAMILY_XCCDF    5	/**< XCCDF errors */
#define OSCAP_EFAMILY_SCE      6	/**< SCE errors */
#define OSCAP_EFAMILY_NET	7	/**< Errors from network communication. Presumably from libcurl. */
#define OSCAP_EFAMILY_WINDOWS	8	/**< Windows API Errors */
/** @} */

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
 * Get last error description.
 */
const char *oscap_err_desc(void);

/**
 * Get the full description for all the errors which has occured in this
 * thread since the last call of this function or \ref oscap_clearerr.
 * This function is destructive. Subsequent call shall return NULL.
 * @returns zero terminated string describing these errors, which shall
 * be disposed by caller.
 * @retval NULL if there are no errors.
 */
char *oscap_err_get_full_error(void);

/// @}
/// @}
#endif				/* OSCAP_ERROR_H */
