/**
 * @file cpelang.h
 * \brief Interface to Common Product Enumeration (CPE) Language
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

/// CPE language operators
enum cpe_lang_oper_t {
	CPE_LANG_OPER_HALT = 0x00,	///< end of instruction list
	CPE_LANG_OPER_AND = 0x01,	///< logical AND
	CPE_LANG_OPER_OR = 0x02,	///< logical OR
	CPE_LANG_OPER_MATCH = 0x03,	///< match against specified CPE
	CPE_LANG_OPER_MASK = 0xFF,	///< mask to extract operator
	CPE_LANG_OPER_NOT = 0x100,	///< negate

	CPE_LANG_OPER_NAND = CPE_LANG_OPER_AND | CPE_LANG_OPER_NOT,
	CPE_LANG_OPER_NOR = CPE_LANG_OPER_OR | CPE_LANG_OPER_NOT,
};

/// CPE language boolean expression
typedef struct cpe_lang_expr {
	enum cpe_lang_oper_t oper;	///< operator
	union {
		struct cpe_lang_expr *expr;	///< array of subexpressions for operators
		cpe_t *cpe;	///< CPE for match operation
	} meta;			///< operation metadata
} cpe_lang_expr_t;

/**
 * CPE platform specification
 */
typedef struct cpe_platform_spec {
	struct cpe_platform **platforms;	///< array of pointers to individual platforms
	size_t platforms_n;	///< number of platforms
	size_t alloc_;		///< allocated memory (internal)
} cpe_platform_spec_t;

/**
 * Single platform representation in CPE language
 */
typedef struct cpe_platform {
	char *id;		///< platform ID
	char *title;		///< human-readable platform description
	char *remark;		///< remark
	struct cpe_lang_expr expr;	///< expression for match evaluation
} cpe_platform_t;

/**
 * New platform specification from file
 * @param fname file name to load
 * @return new platform specification list
 * @retval NULL on failure
 */
cpe_platform_spec_t *cpe_platformspec_new(const char *fname);

/**
 * Add new platform entry to @a platformspec
 * @note @a platformspec will take over memory management of @a platform
 * @param platformspec list of platforms being extended
 * @param platform platform to add to the list
 * @return true on success
 */
bool cpe_platformspec_add(cpe_platform_spec_t * platformspec,
			  cpe_platform_t * platform);

/**
 * Free specified platform specification list
 * @param platformspec pointer to list to be deleted
 */
void cpe_platformspec_delete(cpe_platform_spec_t * platformspec);

/**
 * Match list of CPEs against CPE language platform specification
 * @param cpe List of CPEs describing tested platform as a list of pointers
 * @param n number of CPEs
 * @param platform CPE language platform, that is expected (not) to match given list of CPEs
 * @return result of expression evaluation
 */
bool cpe_language_match_cpe(cpe_t ** cpe, size_t n,
			    const cpe_platform_t * platform);

/**
 * Delete single CPE paltform specification
 * @param platform platform to be deleted
 */
void cpe_platform_delete(cpe_platform_t * platform);

/**
 * Delete CPE language boolean expression
 * @param expr expression to be deleted
 */
void cpe_langexpr_delete(cpe_lang_expr_t * expr);

#endif				/* _CPELANG_H_ */
