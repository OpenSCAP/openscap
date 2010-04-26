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
 */

#pragma once
#ifndef OSCAP_TEXT_H_
#define OSCAP_TEXT_H_


/**
 * @name Common language codes
 * @{
 */
/// English
extern const char * const OSCAP_LANG_ENGLISH;
/// American English
extern const char * const OSCAP_LANG_ENGLISH_US;
/// Default language (i.e. American English)
extern const char * const OSCAP_LANG_DEFAULT;
/** @} */

/**
 * @struct oscap_text
 * Representation of internationalizable character strings
 */
struct oscap_text;

/**
 * Create an internationalized text field.
 * @param lang - language identifier (@see oscap_text_lang)
 * @param encoding - language encoding (@see oscap_text_encoding)
 * @memberof oscap_text
 */
struct oscap_text *oscap_text_new(void);

/**
 * Clone an internationalized text field.
 * @param text oscap_text structure to clone
 * @memberof oscap_text
 */
struct oscap_text *oscap_text_clone(const struct oscap_text * text);

/**
 * Create an internationalized text field with HTML content.
 * @param lang - language identifier (@see oscap_text_lang)
 * @param encoding - language encoding (@see oscap_text_encoding)
 * @memberof oscap_text
 */
struct oscap_text *oscap_text_new_html(void);

/**
 * Release an internationalized text field.
 * @memberof oscap_text
 */
void oscap_text_free(struct oscap_text *);

/// @memberof oscap_text
const char *oscap_text_get_text(const struct oscap_text *text);
/// @memberof oscap_text
bool oscap_text_set_text(struct oscap_text *text, const char * string);

/// @memberof oscap_text
const char *oscap_text_get_lang(const struct oscap_text *text);
/// @memberof oscap_text
bool oscap_text_set_lang(struct oscap_text *text, const char *string);

/**
 * Does this text posses a HTML content?
 * @memberof oscap_text
 */
bool oscap_text_get_is_html(const struct oscap_text *text);
/**
 * Can this text contain substitutions?
 * @memberof oscap_text
 */
bool oscap_text_get_can_substitute(const struct oscap_text *text);
/**
 * Can this text override parent content?
 * @memberof oscap_text
 */
bool oscap_text_get_can_override(const struct oscap_text *text);
/**
 * Does this text override parent content?
 * @memberof oscap_text
 */
bool oscap_text_get_overrides(const struct oscap_text *text);
/**
 * Set whether this text overrides parent content.
 * @memberof oscap_text
 */
//bool oscap_text_set_overrides(struct oscap_text *text, bool overrides);


/** @struct oscap_text_iterator
 * Internationalized string iterator.
 * @see oscap_iterator
 */
struct oscap_text_iterator;
/// @memberof oscap_text_iterator
struct oscap_text *oscap_text_iterator_next(struct oscap_text_iterator *it);
/// @memberof oscap_text_iterator
bool oscap_text_iterator_has_more(struct oscap_text_iterator *it);
/// @memberof oscap_text_iterator
void oscap_text_iterator_free(struct oscap_text_iterator *it);
/// @memberof oscap_text_iterator
void oscap_text_iterator_remove(struct oscap_text_iterator *it);

#endif

