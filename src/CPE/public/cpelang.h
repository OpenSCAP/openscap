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
 *      Maros Barabas <mbarabas@redhat.com>
 */

#ifndef CPELANG_H_
#define CPELANG_H_

#include <stdlib.h>

#include "cpeuri.h"
#include "oscap.h"

/**
 * CPE language operators
 */
typedef enum {
	CPE_LANG_OPER_HALT  =  0x00,  ///< end of instruction list
	CPE_LANG_OPER_AND   =  0x01,  ///< logical AND
	CPE_LANG_OPER_OR    =  0x02,  ///< logical OR
	CPE_LANG_OPER_MATCH =  0x04,  ///< match against specified CPE

	CPE_LANG_OPER_MASK  =  0xFF,  ///< mask to extract the operator w/o possible negation
	CPE_LANG_OPER_NOT   = 0x100,  ///< negate

	CPE_LANG_OPER_NAND = CPE_LANG_OPER_AND | CPE_LANG_OPER_NOT,
	CPE_LANG_OPER_NOR = CPE_LANG_OPER_OR | CPE_LANG_OPER_NOT,
} cpe_lang_oper_t;


/**
 * @struct cpe_testexpr
 * CPE language boolean expression
 */
struct cpe_testexpr;

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

/** @struct cpe_platform_iterator
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 */
struct cpe_platform_iterator;
/// @relates cpe_platform_iterator
struct cpe_platform* cpe_platform_iterator_next(struct cpe_platform_iterator* it);
/// @relates cpe_platform_iterator
bool cpe_platform_iterator_has_more(struct cpe_platform_iterator* it);
/// @relates cpe_platform_iterator
void cpe_platform_iterator_free(struct cpe_platform_iterator* it);


/**
 * Load CPE language model from a XML document.
 * @relates cpe_lang_model
 */
struct cpe_lang_model * cpe_lang_model_import(const struct oscap_import_source * source);

/**
 * Write the lang_model to a file.
 * @relates cpe_lang_model
 * @param spec CPE lang model
 * @param target target structure with filename, endcoding and indent information
 */
void cpe_lang_model_export(struct cpe_lang_model * spec, struct oscap_export_target * target);

/**
 * Write the lang_model to a file.
 * @relates cpe_lang_model
 */
void cpe_lang_model_export_xml(struct cpe_lang_model * spec, struct oscap_export_target * target);

/**
 * Load CPE language model from a XML.
 * @relates cpe_lang_model
 */
struct cpe_lang_model * cpe_lang_model_parse_xml(const struct oscap_import_source * source);

/**
 * cpe_testexpr functions to get variable members
 * @relates cpe_testexpr
 */
cpe_lang_oper_t cpe_testexpr_get_oper(const struct cpe_testexpr *item);

/**
 * Get CPE expression subexpression.
 * Not valid for CPE_LANG_OPER_MATCH operation.
 * @relates cpe_testexpr
 */
struct cpe_testexpr * cpe_testexpr_get_meta_expr(const struct cpe_testexpr *item);

/**
 * Get CPE name to match against.
 * Only valid for CPE_LANG_OPER_MATCH.
 * @relates cpe_testexpr
 */
struct cpe_name * cpe_testexpr_get_meta_cpe(const struct cpe_testexpr *item);

/**
 * cpe_platform functions to get variable members
 */
/// @relates cpe_platform
const char * cpe_platform_get_id(const struct cpe_platform *item);
/// @relates cpe_platform
const char * cpe_platform_get_remark(const struct cpe_platform *item);
/// @relates cpe_platform
struct oscap_title_iterator * cpe_platform_get_titles(const struct cpe_platform *item);
/// @relates cpe_platform
const struct cpe_testexpr * cpe_platform_get_expr(const struct cpe_platform *item);

/**
 * cpe_lang_model function to get namespace href
 * @relates cpe_lang_model
 */
const char * cpe_lang_model_get_ns_href(const struct cpe_lang_model *item);

/**
 * cpe_lang_model function to get namespace prefix
 * @relates cpe_lang_model
 */
const char * cpe_lang_model_get_ns_prefix(const struct cpe_lang_model *item);

/**
 * cpe_lang_model function to get CPE platforms
 * @relates cpe_lang_model
 */
struct cpe_platform_iterator * cpe_lang_model_get_platforms(const struct cpe_lang_model *item);

/**
 * cpe_lang_model function to get CPE items
 * @relates cpe_lang_model
 */
struct cpe_platform * cpe_lang_model_get_item(const struct cpe_lang_model *item, const char *key);

// add functions

/**
 * Add platform to CPE lang model
 * @relates cpe_lang_model
 */
bool cpe_lang_model_add_item(struct cpe_lang_model *lang, struct cpe_platform *platform);

/**
 * Constructor of CPE Language model
 * @relates cpe_lang_model
 */
struct cpe_lang_model * cpe_lang_model_new();

/**
 * Constructor of CPE test expression
 * @relates cpe_testexpr
 */
struct cpe_testexpr * cpe_testexpr_new();

/**
 * Constructor of CPE Platform
 * @relates cpe_platform
 */
struct cpe_platform * cpe_platform_new();

/**
 * Free function of CPE test expression
 * @relates cpe_testexpr
 */
void cpe_testexpr_free(struct cpe_testexpr * expr);

/**
 * Free function of CPE test expression
 * @relates cpe_lang_model
 */
void cpe_lang_model_free(struct cpe_lang_model * platformspec);

/**
 * Free function of CPE Platform
 * @relates cpe_platform
 */
void cpe_platform_free(struct cpe_platform * platform);

/**
 * Set / add functions
 * */
/// @relates cpe_lang_model
bool cpe_lang_model_set_ns_href(struct cpe_lang_model *model, const char *new_href);
/// @relates cpe_lang_model
bool cpe_lang_model_set_ns_prefix(struct cpe_lang_model *model, const char *new_prefix);
/// @relates cpe_lang_model
bool cpe_lang_model_add_item(struct cpe_lang_model *lang, struct cpe_platform *platform);
/// @relates cpe_platform
bool cpe_platform_set_id(struct cpe_platform *platform, const char *new_id);
// @relates cpe_platform
//bool cpe_platform_set_remark(cpe_platform, const char *new_remark);
/// @relates cpe_platform
bool cpe_platform_add_title(struct cpe_platform *platform, struct oscap_title *title);

#endif				/* _CPELANG_H_ */


