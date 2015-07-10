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

#pragma once
#ifndef OSCAP_TEXT_PRIV_H_
#define OSCAP_TEXT_PRIV_H_

#include "public/oscap_text.h"
#include "elements.h"
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

OSCAP_HIDDEN_START;

/**
 * oscap_text traits
 * @see oscap_text
 */
struct oscap_text_traits {
	bool override_given : 1; ///< override attribute given
	bool html           : 1; ///< XHTML-formatted text
	bool can_override   : 1; ///< can override parent's value
	bool can_substitute : 1; ///< can proces the sub element and such
	bool overrides      : 1; ///< overrides parent's value
};

struct oscap_text {
	char *lang;
	char *text;
    struct oscap_text_traits traits;
};

struct oscap_list;

/// Basic traits for plain text
extern const struct oscap_text_traits OSCAP_TEXT_TRAITS_PLAIN;
/// Basic traits for HTML text
extern const struct oscap_text_traits OSCAP_TEXT_TRAITS_HTML;

/**
 * full-featured constructor
 * @param traits text features
 * @param string text data
 * @param lang text language
 */
struct oscap_text *oscap_text_new_full(struct oscap_text_traits traits, const char *string, const char *lang);

/**
 * Return a new string by parsing an xml element
 */
struct oscap_text *oscap_text_new_parse(struct oscap_text_traits traits, xmlTextReaderPtr reader);

xmlNode *oscap_text_to_dom(struct oscap_text *text, xmlNode *parent, const char *elname);
bool oscap_text_export(struct oscap_text *text, xmlTextWriter *writer, const char *elname);
bool oscap_textlist_export(struct oscap_text_iterator *texts, xmlTextWriter *writer, const char *elname);

char *_xhtml_to_plaintext(const char *xhtml_in);

OSCAP_HIDDEN_END;

#endif
