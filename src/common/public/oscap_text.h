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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

/**
 * @file
 * Multilingual text processing interface.
 * @author Lukas Kuklinek <lkuklinek@redhat.com>
 * @author David Niemoller <David.Niemoller@g2-inc.com>
 *
 * @addtogroup COMMON
 * @{
 * @addtogroup STRINGS
 * @{
 * Functions to access and manipulate textual data.
 */

#pragma once
#ifndef OSCAP_TEXT_H_
#define OSCAP_TEXT_H_

#include <stdbool.h>
#include "oscap_export.h"

/**
 * @name Common language codes
 * @{
 */
/// English
OSCAP_API extern const char *const OSCAP_LANG_ENGLISH;
/// American English
OSCAP_API extern const char *const OSCAP_LANG_ENGLISH_US;
/// Default language (i.e. American English)
OSCAP_API extern const char *const OSCAP_LANG_DEFAULT;
/** @} */

/**
 * @struct oscap_text
 * Representation of internationalizable character strings
 */
struct oscap_text;

/**
 * @struct oscap_stringlist
 * A collection of strings.
 */
struct oscap_stringlist;

/**
 * Create an internationalized text field.
 * @param lang - language identifier (@see oscap_text_lang)
 * @param encoding - language encoding (@see oscap_text_encoding)
 * @memberof oscap_text
 */
OSCAP_API struct oscap_text *oscap_text_new(void);

/**
 * Clone an internationalized text field.
 * @param text oscap_text structure to clone
 * @memberof oscap_text
 */
OSCAP_API struct oscap_text *oscap_text_clone(const struct oscap_text * text);

/**
 * Create an internationalized text field with HTML content.
 * @param lang - language identifier (@see oscap_text_lang)
 * @param encoding - language encoding (@see oscap_text_encoding)
 * @memberof oscap_text
 */
OSCAP_API struct oscap_text *oscap_text_new_html(void);

/**
 * Release an internationalized text field.
 * @memberof oscap_text
 */
OSCAP_API void oscap_text_free(struct oscap_text *);

/************************************************************/
/**
 * @name Getters
 * Return value is pointer to structure's member. Do not free unless you null the pointer in the structure. 
 * Use remove function otherwise.
 * @{
 * */

/// @memberof oscap_stringlist
OSCAP_API struct oscap_string_iterator *oscap_stringlist_get_strings(const struct oscap_stringlist* list);
/// @memberof oscap_stringlist
OSCAP_API struct oscap_stringlist *oscap_stringlist_clone(struct oscap_stringlist *list);

/// @memberof oscap_text
OSCAP_API const char *oscap_text_get_text(const struct oscap_text *text);
/// @memberof oscap_text
OSCAP_API const char *oscap_text_get_lang(const struct oscap_text *text);
/**
 * Get plaintext representation of the text.
 * Caller is responsible for freeing returned string.
 * @memberof oscap_text
 */
OSCAP_API char *oscap_text_get_plaintext(const struct oscap_text *text);

/**
 * Does this text posses a HTML content?
 * @memberof oscap_text
 */
OSCAP_API bool oscap_text_get_is_html(const struct oscap_text *text);
/**
 * Can this text contain substitutions?
 * @memberof oscap_text
 */
OSCAP_API bool oscap_text_get_can_substitute(const struct oscap_text *text);
/**
 * Can this text override parent content?
 * @memberof oscap_text
 */
OSCAP_API bool oscap_text_get_can_override(const struct oscap_text *text);
/**
 * Does this text override parent content?
 * @memberof oscap_text
 */
OSCAP_API bool oscap_text_get_overrides(const struct oscap_text *text);

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
 * Set whether this text overrides parent content.
 * @memberof oscap_text
 */
OSCAP_API bool oscap_text_set_overrides(struct oscap_text *text, bool overrides);

/// @memberof oscap_text
OSCAP_API bool oscap_text_set_text(struct oscap_text *text, const char * string);
/// @memberof oscap_text
OSCAP_API bool oscap_text_set_lang(struct oscap_text *text, const char *string);

/// @memberof oscap_stringlist
OSCAP_API bool oscap_stringlist_add_string(struct oscap_stringlist* list, const char *str);
/// @memberof oscap_stringlist
OSCAP_API struct oscap_stringlist * oscap_stringlist_new(void);
/// @memberof oscap_stringlist
OSCAP_API void oscap_stringlist_free(struct oscap_stringlist *list);


/************************************************************/
/** @} End of Setters group */

/************************************************************/
/**
 * @name Iterators
 * @{
 * */

/** @struct oscap_text_iterator
 * Internationalized string iterator.
 * @see oscap_iterator
 */
struct oscap_text_iterator;
/// @memberof oscap_text_iterator
OSCAP_API struct oscap_text *oscap_text_iterator_next(struct oscap_text_iterator *it);
/// @memberof oscap_text_iterator
OSCAP_API bool oscap_text_iterator_has_more(struct oscap_text_iterator *it);
/// @memberof oscap_text_iterator
OSCAP_API void oscap_text_iterator_free(struct oscap_text_iterator *it);
/// @memberof oscap_text_iterator
OSCAP_API void oscap_text_iterator_remove(struct oscap_text_iterator *it);
/// @memberof oscap_text_iterator
OSCAP_API void oscap_text_iterator_reset(struct oscap_text_iterator *it);

/**
 * @struct oscap_string_iterator
 * String iterator.
 *
 * An iterator iterating over a collection of char*-s.
 * @see oscap_iterator
 */
struct oscap_string_iterator;
/// @memberof oscap_string_iterator
OSCAP_API const char *oscap_string_iterator_next(struct oscap_string_iterator *it);
/// @memberof oscap_string_iterator
OSCAP_API bool oscap_string_iterator_has_more(struct oscap_string_iterator *it);
/// @memberof oscap_string_iterator
OSCAP_API void oscap_string_iterator_free(struct oscap_string_iterator *it);
/// @memberof oscap_string_iterator
OSCAP_API void oscap_string_iterator_remove(struct oscap_string_iterator *it);
/// @memberof oscap_string_iterator
OSCAP_API void oscap_string_iterator_reset(struct oscap_string_iterator *it);

/**
 * @struct oscap_stringlist_iterator
 * Iterator over collections of strings.
 * @see oscap_iterator
 */
struct oscap_stringlist_iterator;
/// @memberof oscap_stringlist_iterator
OSCAP_API struct oscap_stringlist *oscap_stringlist_iterator_next(struct oscap_stringlist_iterator *it);
/// @memberof oscap_stringlist_iterator
OSCAP_API bool oscap_stringlist_iterator_has_more(struct oscap_stringlist_iterator *it);
/// @memberof oscap_stringlist_iterator
OSCAP_API void oscap_stringlist_iterator_free(struct oscap_stringlist_iterator *it);
/// @memberof oscap_stringlist_iterator
OSCAP_API void oscap_stringlist_iterator_remove(struct oscap_stringlist_iterator *it);
/// @memberof oscap_stringlist_iterator
OSCAP_API void oscap_stringlist_iterator_reset(struct oscap_stringlist_iterator *it);

/************************************************************/
/** @} End of Iterators group */

/** @} */

/**
 * @brief gets a plaintext string representing given textlist
 *
 * Iterates through given texts and looks at each to see if the language matches
 * given preferred_lang. If preferred_lang is NULL, OSCAP_DEFAULT_LANG is matched.
 *
 * If a match is found it, a copy of plaintext representation of that text is
 * returned. If nothing is found, plaintext representation of the first text in
 * the list is returned. If the textlist is empty, NULL is returned.
 */
OSCAP_API char* oscap_textlist_get_preferred_plaintext(struct oscap_text_iterator* texts, const char* preferred_lang);

/**
 * @brief gets oscap_text representing given textlist
 *
 * Similar to @ref oscap_textlist_get_preferred_plaintext but returns oscap_text
 * instead of just the plaintext.
 */
OSCAP_API struct oscap_text *oscap_textlist_get_preferred_text(struct oscap_text_iterator *texts, const char *preferred_lang);


/** @} */

#endif

