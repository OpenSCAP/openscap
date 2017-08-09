
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 */

/*
 * text.c
 *
 *  Created on: Jan 14, 2010
 *      Author: david.niemoller
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdio.h>

#include "text_priv.h"
#include "util.h"
#include "list.h"

const char * const OSCAP_LANG_ENGLISH    = "en";
const char * const OSCAP_LANG_ENGLISH_US = "en-US";
const char * const OSCAP_LANG_DEFAULT    = "en-US";

const struct oscap_text_traits OSCAP_TEXT_TRAITS_PLAIN = { .html = false };
const struct oscap_text_traits OSCAP_TEXT_TRAITS_HTML  = { .html = true };


OSCAP_ACCESSOR_STRING(oscap_text, text)
OSCAP_ACCESSOR_STRING(oscap_text, lang)
OSCAP_GENERIC_GETTER(bool, oscap_text, is_html, traits.html)
OSCAP_GENERIC_GETTER(bool, oscap_text, can_substitute, traits.can_substitute)
OSCAP_GENERIC_GETTER(bool, oscap_text, can_override, traits.can_override)
OSCAP_GENERIC_GETTER(bool, oscap_text, overrides, traits.overrides)
OSCAP_ITERATOR_GEN_T(struct oscap_text *, oscap_text)
    /*OSCAP_ITERATOR_RESET(oscap_text)*/
OSCAP_ITERATOR_REMOVE_T(struct oscap_text *, oscap_text, oscap_text_free)


bool oscap_text_set_overrides(struct oscap_text *text, bool overrides)
{
	text->traits.overrides = overrides;
	return text->traits.can_override;
}

void oscap_text_free(struct oscap_text *text)
{
    if (text != NULL) {
        free(text->lang);
        free(text->text);
        free(text);
    }
}

struct oscap_text *oscap_text_new_full(struct oscap_text_traits traits, const char *string, const char *lang)
{
    struct oscap_text *text = oscap_calloc(1, sizeof(struct oscap_text));
    text->traits = traits;
    text->text   = oscap_strdup(string);
    text->lang   = oscap_strdup(lang);
    return text;
}


struct oscap_text *oscap_text_new(void)
{
    return oscap_text_new_full(OSCAP_TEXT_TRAITS_PLAIN, NULL, NULL);
}

struct oscap_text * oscap_text_clone(const struct oscap_text * text)
{
    return oscap_text_new_full(text->traits, text->text, text->lang);   
}

struct oscap_text *oscap_text_new_html(void)
{
    //assert(false); // TODO implement
    return oscap_text_new_full(OSCAP_TEXT_TRAITS_HTML, NULL, NULL);
}

struct oscap_text *oscap_text_new_parse(struct oscap_text_traits traits, xmlTextReaderPtr reader)
{
    assert(reader != NULL);

    // new text
    struct oscap_text *text = oscap_text_new_full(traits, NULL, NULL);

    // extract 'override' attribute
    if (text->traits.can_override) {
        xmlTextReaderMoveToAttribute(reader, BAD_CAST "override");
        text->traits.overrides = oscap_string_to_enum(OSCAP_BOOL_MAP, (const char *) xmlTextReaderConstValue(reader));
		if (xmlTextReaderConstValue(reader) != NULL) text->traits.override_given = true;
    }

    // extract language
    text->lang = (char *) xmlTextReaderXmlLang(reader);

    xmlTextReaderMoveToElement(reader);

    // extract content
    if (text->traits.html || text->traits.can_substitute)
		text->text = oscap_get_xml(reader);
    else text->text = oscap_element_string_copy(reader);

    return text;
}

xmlNode *oscap_text_to_dom(struct oscap_text *text, xmlNode *parent, const char *elname)
{
	if (!text) return NULL;

	xmlNode *text_node = NULL;

	if (text->traits.html || text->traits.can_substitute) {
		text_node = oscap_xmlstr_to_dom(parent, elname, text->text);
		// make sure we use parent's namespace
		xmlSetNs(text_node, parent->ns);
	}
	else {
		// NULL as ns here means that namespace is inherited from parent
		text_node = xmlNewTextChild(parent, NULL, BAD_CAST elname, BAD_CAST text->text);
	}

	if (text_node == NULL) return NULL;

	if (text->lang)
		xmlNodeSetLang(text_node, BAD_CAST text->lang);
	if (text->traits.can_override && text->traits.overrides)
		xmlNewProp(text_node, BAD_CAST "override", BAD_CAST "true");


	return text_node;
}

bool oscap_text_export(struct oscap_text *text, xmlTextWriter *writer, const char *elname)
{
	if (text == NULL || writer == NULL) return false;

	if (elname) xmlTextWriterStartElement(writer, BAD_CAST elname);

	if (text->lang)
		xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST text->lang);
	if (text->traits.can_override && text->traits.overrides)
		xmlTextWriterWriteAttribute(writer, BAD_CAST "override", BAD_CAST "true");

	if (text->traits.html || text->traits.can_substitute)
		xmlTextWriterWriteRaw(writer, BAD_CAST text->text);
	else xmlTextWriterWriteString(writer, BAD_CAST text->text);

	if (elname) xmlTextWriterEndElement(writer);

	return true;
}

char *_xhtml_to_plaintext(const char *xhtml_in)
{
	char *out = NULL;
	char *str = oscap_sprintf("<x xmlns='http://www.w3.org/1999/xhtml'>%s</x>", xhtml_in);
	xmlDoc *doc = xmlParseMemory(str, strlen(str));
	if (doc == NULL) {
		free(str);
		return NULL;
	}
	xmlNode *root = xmlDocGetRootElement(doc);
	if (root == NULL) {
		free(str);
		return out;
	}

	// TODO: better HTML -> plaintext conversion
	// (perhaps use xml_iterate)
	out = (char*) xmlNodeGetContent(root);
	xmlFreeDoc(doc);
	free(str);
	return out;
}

char *oscap_text_get_plaintext(const struct oscap_text *text)
{
    if (text == NULL) return NULL;

    if (!text->traits.html) return oscap_strdup(text->text);

	return _xhtml_to_plaintext(text->text);
}

bool oscap_textlist_export(struct oscap_text_iterator *texts, xmlTextWriter *writer, const char *elname)
{
	if (texts == NULL || writer == NULL || elname == NULL) return false;

	OSCAP_FOR(oscap_text, text, texts)
		oscap_text_export(text, writer, elname);
	
	return true;
}

struct oscap_text *oscap_textlist_get_preferred_text(struct oscap_text_iterator *texts, const char *preferred_lang)
{
	if (preferred_lang == NULL)
		preferred_lang = OSCAP_LANG_DEFAULT;

	oscap_text_iterator_reset(texts);

	while (oscap_text_iterator_has_more(texts)) {
		struct oscap_text *text = oscap_text_iterator_next(texts);
		const char *lang = oscap_text_get_lang(text);
		if (lang && strcmp(lang, preferred_lang) == 0)
			return text;
	}

	// not found as exact match in preferred language, try to find implicit match
	oscap_text_iterator_reset(texts);
	while (oscap_text_iterator_has_more(texts)) {
		struct oscap_text *text = oscap_text_iterator_next(texts);
		const char *lang = oscap_text_get_lang(text);
		if (lang == NULL)
			return text;
	}

	// fallback to any language
	oscap_text_iterator_reset(texts);
	while (oscap_text_iterator_has_more(texts)) {
		struct oscap_text *text = oscap_text_iterator_next(texts);
		return text;
	}

	// nothing found
	return NULL;
}

char *oscap_textlist_get_preferred_plaintext(struct oscap_text_iterator *texts, const char *preferred_lang)
{
	struct oscap_text *text = oscap_textlist_get_preferred_text(texts, preferred_lang);
	return (text != NULL) ? oscap_text_get_plaintext(text) : NULL;
}
