/*
 * text.c
 *
 *  Created on: Jan 14, 2010
 *      Author: david.niemoller
 */

#include "util.h"
#include "list.h"
#include <config.h>
#include <string.h>
#include <stdio.h>

#include "text_priv.h"

const char * const OSCAP_LANG_ENGLISH    = "en";
const char * const OSCAP_LANG_ENGLISH_US = "en_US";
const char * const OSCAP_LANG_DEFAULT    = "en_US";

const struct oscap_text_traits OSCAP_TEXT_TRAITS_PLAIN = { .html = false };
const struct oscap_text_traits OSCAP_TEXT_TRAITS_HTML  = { .html = true };


OSCAP_ACCESSOR_STRING(oscap_text, text)
OSCAP_ACCESSOR_STRING(oscap_text, lang)
OSCAP_GENERIC_GETTER(bool, oscap_text, is_html, traits.html)
OSCAP_GENERIC_GETTER(bool, oscap_text, can_substitute, traits.can_substitute)
OSCAP_GENERIC_GETTER(bool, oscap_text, can_override, traits.can_override)
OSCAP_GENERIC_GETTER(bool, oscap_text, overrides, traits.overrides)
OSCAP_ITERATOR_GEN_T(struct oscap_text *, oscap_text)
OSCAP_ITERATOR_REMOVE_T(struct oscap_text *, oscap_text, oscap_text_free)


void oscap_text_free(struct oscap_text *text)
{
    if (text != NULL) {
        oscap_free(text->lang);
        oscap_free(text->text);
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
    assert(false); // TODO implement
    return oscap_text_new_full(OSCAP_TEXT_TRAITS_HTML, NULL, NULL);
}

struct oscap_text *oscap_text_new_parse(struct oscap_text_traits traits, xmlTextReaderPtr reader)
{
    assert(reader != NULL);

    // new text
    struct oscap_text *text = oscap_text_new_full(traits, NULL, NULL);

    // extract 'overrides' attribute
    if (text->traits.can_override) {
        xmlTextReaderMoveToAttribute(reader, BAD_CAST "overrides");
        text->traits.overrides = oscap_string_to_enum(OSCAP_BOOL_MAP, (const char *) xmlTextReaderConstValue(reader));
    }

    // extract language
    text->lang = (char *) xmlTextReaderXmlLang(reader);

    xmlTextReaderMoveToElement(reader);

    // extract content
    if (text->traits.html) text->text = oscap_get_xml(reader);
    else text->text = oscap_element_string_copy(reader);

    // TODO substitution support
    
    return text;
}


