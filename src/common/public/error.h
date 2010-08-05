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
 * error code(detailed classification in scope of family) and textual description. Example of usage:
 *
 * @code
 * syschar = oval_probe_object_eval (pctx, object);
 * if (syschar == NULL && oscap_err()) {
 *     printf("Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
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
/// Error code type
typedef uint16_t oscap_errcode_t;

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
/** @} */

/**
 * @name OSCAP family codes
 * @{
 */
#define OSCAP_ENONE         0	/**< None */
#define OSCAP_EXMLELEM      1	/**< Unknown XML element */
#define OSCAP_EINVARG       2	/**< Function called with invalid argument */
#define OSCAP_ENOTIMPL      254 /**< Not implemented*/
/** @} */

/**
 * @name OVAL family codes
 * @{
 */
#define OVAL_EOVALINT        1
#define OVAL_EPROBECONTEXT   255
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
 * @name XCCDF family codes
 * @{
 */
#define XCCDF_EREFIDCONFLICT  1   /**< Conflict in refine rules - same idref */
#define XCCDF_EREFGROUPATTR   2   /**< Bad attribute of group refid */
#define XCCDF_EUNKNOWNTYPE    3   /**< Bad type of xccdf item */  
#define XCCDF_EUNKNOWNCB      4   /**< Unknown callback - missing registration */
#define XCCDF_EBADID          5   /**< Bad id in reference - item with that id does not exist */
#define XCCDF_EVALUE          6   /**< Can't find referenced value instance */
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
 * Get last error code.
 */
oscap_errcode_t oscap_err_code(void);

/**
 * Get last error description.
 */
const char *oscap_err_desc(void);


#endif				/* OSCAP_ERROR_H */
