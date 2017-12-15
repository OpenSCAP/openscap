/**
 * @addtogroup CPE
 * @{
 * @addtogroup cpelang CPE Language
 * @{
 *
 * @image html cpe_language.png "Class diagram"
 * @file cpe_lang.h
 *
 * @brief Interface to Common Platform Enumeration (CPE) Language
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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

#include "cpe_name.h"
#include "oscap.h"
#include "oscap_text.h"
#include "oscap_source.h"
#include "oscap_export.h"

/**
 * CPE language operators
 */
typedef enum {
	CPE_LANG_OPER_INVALID = 0x00,   ///< invalid or unknown operation
	CPE_LANG_OPER_AND = 0x01,	///< logical AND
	CPE_LANG_OPER_OR = 0x02,	///< logical OR
	CPE_LANG_OPER_MATCH = 0x04,	///< fact-ref = match given CPE name against available dictionaries
	CPE_LANG_OPER_CHECK = 0x08, ///< check-ref = evaluate given check 

	CPE_LANG_OPER_MASK = 0xFF,	///< mask to extract the operator w/o possible negation
	CPE_LANG_OPER_NOT = 0x100,	///< negate

	CPE_LANG_OPER_NAND = CPE_LANG_OPER_AND | CPE_LANG_OPER_NOT,
	CPE_LANG_OPER_NOR = CPE_LANG_OPER_OR | CPE_LANG_OPER_NOT,
} cpe_lang_oper_t;

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

/**
 * @struct cpe_testexpr
 * CPE language boolean expression
 */
struct cpe_testexpr;


/************************************************************/
/**
 * @name Iterators
 * @{
 * */

/** 
 * @struct cpe_platform_iterator
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 */
struct cpe_platform_iterator;

/// @memberof cpe_platform_iterator
OSCAP_API struct cpe_platform *cpe_platform_iterator_next(struct cpe_platform_iterator *it);
/// @memberof cpe_platform_iterator
OSCAP_API bool cpe_platform_iterator_has_more(struct cpe_platform_iterator *it);
/// @memberof cpe_platform_iterator
OSCAP_API void cpe_platform_iterator_free(struct cpe_platform_iterator *it);

/**
 * @struct cpe_testexpr_iterator
 * Iterator over CPE language expressions.
 * @see oscap_iterator
 */
struct cpe_testexpr_iterator;
/// @memberof cpe_testexpr_iterator
OSCAP_API struct cpe_testexpr *cpe_testexpr_iterator_next(struct cpe_testexpr_iterator *it);
/// @memberof cpe_testexpr_iterator
OSCAP_API bool cpe_testexpr_iterator_has_more(struct cpe_testexpr_iterator *it);
/// @memberof cpe_testexpr_iterator
OSCAP_API void cpe_testexpr_iterator_free(struct cpe_testexpr_iterator *it);

/************************************************************/
/** @} End of Iterators group */

/************************************************************/
/**
 * @name Getters
 * Return value is pointer to structure's member. Do not free unless you null the pointer in the structure. 
 * Use remove function otherwise.
 * @{
 * */

/**
 * cpe_testexpr functions to get variable members
 * @memberof cpe_testexpr
 */
OSCAP_API cpe_lang_oper_t cpe_testexpr_get_oper(const struct cpe_testexpr *item);

/**
 * Get CPE expression subexpression.
 * Not valid for CPE_LANG_OPER_MATCH operation.
 * @memberof cpe_testexpr
 */
OSCAP_API struct cpe_testexpr_iterator *cpe_testexpr_get_meta_expr(const struct cpe_testexpr *item);

/**
 * Get CPE name to match against.
 * Only valid for CPE_LANG_OPER_MATCH.
 * @memberof cpe_testexpr
 */
OSCAP_API const struct cpe_name *cpe_testexpr_get_meta_cpe(const struct cpe_testexpr *item);

/**
 * Get check system to evaluate
 * Only valid for CPE_LANG_OPER_CHECK.
 * @memberof cpe_testexpr
 */
OSCAP_API const char* cpe_testexpr_get_meta_check_system(const struct cpe_testexpr *item);

/**
 * Get check href to evaluate
 * Only valid for CPE_LANG_OPER_CHECK.
 * @memberof cpe_testexpr
 */
OSCAP_API const char* cpe_testexpr_get_meta_check_href(const struct cpe_testexpr *item);

/**
 * Get check idref to evaluate
 * Only valid for CPE_LANG_OPER_CHECK.
 * @memberof cpe_testexpr
 */
OSCAP_API const char* cpe_testexpr_get_meta_check_id(const struct cpe_testexpr *item);

/**
 * Function to get next expr from array
 * @param expr CPE Test expression structure
 * @memberof cpe_testexpr
 */
OSCAP_API const struct cpe_testexpr *cpe_testexpr_get_next(const struct cpe_testexpr *expr);

/**
 * cpe_lang_model function to get CPE platforms
 * @memberof cpe_lang_model
 */
OSCAP_API struct cpe_platform_iterator *cpe_lang_model_get_platforms(const struct cpe_lang_model *item);

/**
 * cpe_lang_model function to get CPE platforms
 * @memberof cpe_lang_model
 */
OSCAP_API struct cpe_platform *cpe_lang_model_get_item(const struct cpe_lang_model *item, const char *key);

/**
 * Verify whether given CPE platform idref is applicable by evaluating test expression associated with it
 *
 * @memberof cpe_lang_model
 * @param platform idref to the platform to verify (do not pass with "#" prepended)
 * @param lang_model used CPE language model
 * @return true if lang model contains given platform and the platform is applicable
 */
OSCAP_API bool cpe_platform_applicable_lang_model(const char* platform, struct cpe_lang_model *lang_model, cpe_check_fn check_cb, cpe_dict_fn dict_cb, void* usr);

/**
 * cpe_platform functions to get id
 * @memberof cpe_platform
 */
OSCAP_API const char *cpe_platform_get_id(const struct cpe_platform *item);
/**
 * cpe_platform functions to get remark
 * @memberof cpe_platform
 */
OSCAP_API const char *cpe_platform_get_remark(const struct cpe_platform *item);
/**
 * cpe_platform functions to get titles
 * @memberof cpe_platform
 */
OSCAP_API struct oscap_text_iterator *cpe_platform_get_titles(const struct cpe_platform *item);
/**
 * cpe_platform functions to get test expression
 * @memberof cpe_platform
 */
OSCAP_API const struct cpe_testexpr *cpe_platform_get_expr(const struct cpe_platform *item);

/************************************************************/
/** @} End of Getters group */

/************************************************************/
/**
 * @name Setters
 * For lists use add functions. Parameters of set functions are duplicated in memory and need to 
 * be freed by caller.
 * @{
 */

/**
 * Add platform to CPE lang model
 * @memberof cpe_lang_model
 */
OSCAP_API bool cpe_lang_model_add_platform(struct cpe_lang_model *lang, struct cpe_platform *platform);
/**
 * Add title to platform
 * @memberof cpe_platform
 */
OSCAP_API bool cpe_platform_add_title(struct cpe_platform *platform, struct oscap_text *title);

/**
 * Add XML namespace to CPE lang model
 * @memberof cpe_lang_model
 */
/*bool cpe_lang_model_add_xmlns(struct cpe_lang_model * model, struct xml_metadata * xml);*/

/**
 * Add an subexpression to a compound CPE language expression.
 *
 * Expression has to be of type CPE_LANG_OPER_AND or CPE_LANG_OPER_OR, possibly with negation.
 * @memberof cpe_testexpr
 */
OSCAP_API bool cpe_testexpr_add_subexpression(struct cpe_testexpr *expr, struct cpe_testexpr *sub);

/**
 * Set ID of CPE platform
 * @memberof cpe_platform
 */
OSCAP_API bool cpe_platform_set_id(struct cpe_platform *platform, const char *new_id);
/**
 * Set remark of CPE platform
 * @memberof cpe_platform
 */
OSCAP_API bool cpe_platform_set_remark(struct cpe_platform *platform, const char *new_remark);
/**
 * Set evaluation expression for this CPE platform.
 *
 * Expression has to be a logical-test (i.e. its operation shall be AND or OR, possibly with negation)
 * @memberof cpe_platform
 */
OSCAP_API bool cpe_platform_set_expr(struct cpe_platform *platform, struct cpe_testexpr *expr);
/**
 * Set CPE operation.
 *
 * Any subexpressions and CPE names associated with this expression will be removed.
 * @memberof cpe_testexpr
 */
OSCAP_API bool cpe_testexpr_set_oper(struct cpe_testexpr *expr, cpe_lang_oper_t oper);

/**
 * Set CPE name.
 *
 * If the expression does not have type CPE_LANG_OPER_MATCH,
 * this function has no effect and returns false.
 * @memberof cpe_testexpr
 */
OSCAP_API bool cpe_testexpr_set_name(struct cpe_testexpr *expr, struct cpe_name *name);

/************************************************************/
/** @} End of Setters group */

/// @memberof cpe_platform_iterator
OSCAP_API void cpe_platform_iterator_remove(struct cpe_platform_iterator *it, struct cpe_lang_model *parent);
/// @memberof cpe_testexpr_iterator
OSCAP_API void cpe_platform_iterator_reset(struct cpe_platform_iterator *it);

/**
 * Constructor of CPE Language model
 * @memberof cpe_lang_model
 */
OSCAP_API struct cpe_lang_model *cpe_lang_model_new(void);

/**
 * Constructor of CPE test expression
 * @memberof cpe_testexpr
 */
OSCAP_API struct cpe_testexpr *cpe_testexpr_new(void);

/**
 * Constructor of CPE Platform
 * @memberof cpe_platform
 */
OSCAP_API struct cpe_platform *cpe_platform_new(void);

/**
 * Clone CPE test expression
 * @param old_expr CPE test expression
 * @memberof cpe_testexpr
 */
OSCAP_API struct cpe_testexpr * cpe_testexpr_clone(struct cpe_testexpr * old_expr);

/**
 * Free function of CPE test expression
 * @memberof cpe_testexpr
 */
OSCAP_API void cpe_testexpr_free(struct cpe_testexpr *expr);

/**
 * Reset function of CPE test expression
 * @memberof cpe_testexpr
 */
OSCAP_API void cpe_testexpr_iterator_reset(struct cpe_testexpr_iterator *it);

/**
 * Free function of CPE test expression
 * @memberof cpe_lang_model
 */
OSCAP_API void cpe_lang_model_free(struct cpe_lang_model *platformspec);

/**
 * Free function of CPE Platform
 * @memberof cpe_platform
 */
OSCAP_API void cpe_platform_free(struct cpe_platform *platform);

/************************************************************/
/**
 * @name Evaluators
 * @{
 * */

/**
 * Get supported version of CPE language XML
 * @return version of XML file format
 * @memberof cpe_lang_model
 */
OSCAP_API const char * cpe_lang_model_supported(void);

/**
 * Detect version of given CPE language XML
 * @memberof cpe_lang_model
 * @deprecated This function has been deprecated by @ref oscap_source_get_schema_version.
 * This function may be dropped from later versions of the library.
 */
OSCAP_API OSCAP_DEPRECATED(char * cpe_lang_model_detect_version(const char* file));

/**
 * Function to match cpe in platform
 * @param cpe to be matched with
 * @param n size
 * @param platform CPE platform
 * @memberof cpe_platform
 */
OSCAP_API bool cpe_platform_match_cpe(struct cpe_name **cpe, size_t n, const struct cpe_platform *platform);

/************************************************************/
/** @} End of Evaluators group */

/**
 * Load CPE language model from a XML document.
 * @memberof cpe_lang_model
 * @deprecated This function has been deprecated by @ref cpe_lang_model_import_source
 * This function may be dropped from later versions of the library.
 */
OSCAP_API OSCAP_DEPRECATED(struct cpe_lang_model *cpe_lang_model_import(const char *file));

/**
 * Load CPE language model from an oscap_source.
 * @memberof cpe_lang_model
 */
OSCAP_API struct cpe_lang_model *cpe_lang_model_import_source(struct oscap_source *source);

/**
 * Sets the origin file hint
 * @note This is intended for internal use only!
 * @see cpe_lang_model_get_origin_file
 */
OSCAP_API bool cpe_lang_model_set_origin_file(struct cpe_lang_model* lang_model, const char* origin_file);

/**
 * Gets the file the CPE dict model was loaded from
 * @internal
 * This is necessary to figure out the full OVAL file path for applicability
 * testing. We can't do applicability here in the CPE module because that
 * would create awful interdependencies.
 */
OSCAP_API const char* cpe_lang_model_get_origin_file(const struct cpe_lang_model* lang_model);

/**
 * Write the lang_model to a file.
 * @memberof cpe_lang_model
 * @param spec CPE lang model
 * @param file filename
 */
OSCAP_API void cpe_lang_model_export(const struct cpe_lang_model *spec, const char *file);

/**@}*/

/**@}*/

#endif				/* _CPELANG_H_ */
