/**
 * @addtogroup CPE
 * @{
 * @addtogroup cpelang CPE Language
 * @{
 *
 * @file cpelang.h
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifndef _CPELANG_H_
#define _CPELANG_H_

#include <stdlib.h>

#include "cpeuri.h"
#include "../common/oscap.h"

/// CPE language operators
enum cpe_lang_oper_t {
	CPE_LANG_OPER_HALT  =  0x00,  ///< end of instruction list
	CPE_LANG_OPER_AND   =  0x01,  ///< logical AND
	CPE_LANG_OPER_OR    =  0x02,  ///< logical OR
	CPE_LANG_OPER_MATCH =  0x03,  ///< match against specified CPE

	CPE_LANG_OPER_MASK  =  0xFF,  ///< mask to extract the operator w/o possible negation
	CPE_LANG_OPER_NOT   = 0x100,  ///< negate

	CPE_LANG_OPER_NAND = CPE_LANG_OPER_AND | CPE_LANG_OPER_NOT,
	CPE_LANG_OPER_NOR = CPE_LANG_OPER_OR | CPE_LANG_OPER_NOT,
};

/*
 * @struct cpe_lang_expr
 * CPE language boolean expression
 */
struct cpe_lang_expr;

/**
 * @struct cpe_platformspec
 * CPE platform specification
 */
struct cpe_platformspec;

/**
 * @struct cpe_platform
 * Single platform representation in CPE language
 */
struct cpe_platform;

/** @struct cpe_platform_iterator
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 */
struct cpe_platform_iterator;
/// @relates cpe_platform_iterator
struct cpe_platform* cpe_platform_iterator_next(struct cpe_platform_iterator* it);
/// @relates cpe_platform_iterator
bool cpe_platform_iterator_has_more(struct cpe_platform_iterator* it);

/**
 * New platform specification from file
 * @relates cpe_platformspec
 * @param fname file name to load
 * @return new platform specification list
 * @retval NULL on failure
 */
struct cpe_platformspec *cpe_platformspec_new(const char *fname);

/**
 * Free specified platform specification list
 * @relates cpe_platformspec
 * @param platformspec pointer to list to be deleted
 */
void cpe_platformspec_free(struct cpe_platformspec * platformspec);

/**
 * Get an iterator to platforms contained in this specification.
 * @relates cpe_platformspec
 */
struct cpe_platform_iterator* cpe_platformspec_get_items(const struct cpe_platformspec * platformspec);

/**
 * Get platform with given ID.
 * @relates cpe_platformspec
 * @param platformspec Used platform specfication.
 * @param id Desired platform ID.
 * @raturn Platform with given ID.
 * @retval NULL on failure (e.g. no such platform exists)
 */
struct cpe_platform* cpe_platformspec_get_item(const struct cpe_platformspec * platformspec, const char* id);

/**
 * Match list of CPEs against CPE language platform specification
 * @relates cpe_platform
 * @param cpe List of CPEs describing tested platform as a list of pointers
 * @param n number of CPEs
 * @param platform CPE language platform, that is expected (not) to match given list of CPEs
 * @return result of expression evaluation
 */
bool cpe_platform_match_cpe(struct cpe_name ** cpe, size_t n, const struct cpe_platform * platform);

/**
 * Get CPE paltform ID.
 * @relates cpe_platform
 */
const char* cpe_platform_get_id(const struct cpe_platform* platform);

/**
 * Get CPE paltform title.
 * @relates cpe_platform
 */
const char* cpe_platform_get_title(const struct cpe_platform* platform);

/**
 * Get CPE paltform remark.
 * @relates cpe_platform
 */
const char* cpe_platform_get_remark(const struct cpe_platform* platform);

#endif				/* _CPELANG_H_ */


