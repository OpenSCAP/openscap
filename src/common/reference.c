/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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

/**
 * @file
 * Dublin Core references handling
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "reference_priv.h"
#include "list.h"
#include "debug_priv.h"

#define NS_DUBLINCORE BAD_CAST "http://purl.org/dc/elements/1.1/"

OSCAP_ACCESSOR_SIMPLE(bool,oscap_reference,is_dublincore)
OSCAP_ACCESSOR_STRING(oscap_reference,href)
OSCAP_ACCESSOR_STRING(oscap_reference,title)
OSCAP_ACCESSOR_STRING(oscap_reference,creator)
OSCAP_ACCESSOR_STRING(oscap_reference,subject)
OSCAP_ACCESSOR_STRING(oscap_reference,description)
OSCAP_ACCESSOR_STRING(oscap_reference,publisher)
OSCAP_ACCESSOR_STRING(oscap_reference,contributor)
OSCAP_ACCESSOR_STRING(oscap_reference,date)
OSCAP_ACCESSOR_STRING(oscap_reference,type)
OSCAP_ACCESSOR_STRING(oscap_reference,format)
OSCAP_ACCESSOR_STRING(oscap_reference,identifier)
OSCAP_ACCESSOR_STRING(oscap_reference,source)
OSCAP_ACCESSOR_STRING(oscap_reference,language)
OSCAP_ACCESSOR_STRING(oscap_reference,relation)
OSCAP_ACCESSOR_STRING(oscap_reference,coverage)
OSCAP_ACCESSOR_STRING(oscap_reference,rights)

OSCAP_ITERATOR_GEN(oscap_reference)

struct oscap_reference *oscap_reference_new(void)
{
    return oscap_calloc(1, sizeof(struct oscap_reference));
}

void oscap_reference_free(struct oscap_reference *ref)
{
    if (ref) {
        free(ref->title);
        free(ref->creator);
        free(ref->subject);
        free(ref->description);
        free(ref->publisher);
        free(ref->contributor);
        free(ref->date);
        free(ref->type);
        free(ref->format);
        free(ref->identifier);
        free(ref->source);
        free(ref->language);
        free(ref->relation);
        free(ref->coverage);
        free(ref->rights);
        free(ref->href);
        free(ref);
    }
}

#define DC_ITEM_TO_DOM(ITEM) do { \
    if (ref->ITEM != NULL) \
        xmlNewTextChild(ref_node, ns_dc, BAD_CAST #ITEM, BAD_CAST ref->ITEM); \
    } while(0)

xmlNode *oscap_reference_to_dom(struct oscap_reference *ref, xmlNode *parent, xmlDoc *doc, const char *elname)
{
    if (!ref) return NULL;
    xmlNode *ref_node = xmlNewChild(parent, NULL, BAD_CAST elname, NULL);

    if (ref->href != NULL)
        xmlNewProp(ref_node, BAD_CAST "href", BAD_CAST ref->href);

    if (!ref->is_dublincore) {
        xmlNodeAddContent(ref_node, BAD_CAST ref->title);
        return ref_node;
    }
    
    xmlNs *ns_dc = xmlSearchNsByHref(doc, parent, NS_DUBLINCORE);
    if (ns_dc == NULL) // the namespace hasn't been defined in ancestor elements
        ns_dc = xmlNewNs(ref_node, NS_DUBLINCORE, BAD_CAST "dc");

    DC_ITEM_TO_DOM(title);
    DC_ITEM_TO_DOM(creator);
    DC_ITEM_TO_DOM(subject);
    DC_ITEM_TO_DOM(description);
    DC_ITEM_TO_DOM(publisher);
    DC_ITEM_TO_DOM(contributor);
    DC_ITEM_TO_DOM(date);
    DC_ITEM_TO_DOM(type);
    DC_ITEM_TO_DOM(format);
    DC_ITEM_TO_DOM(identifier);
    DC_ITEM_TO_DOM(source);
    DC_ITEM_TO_DOM(language);
    DC_ITEM_TO_DOM(relation);
    DC_ITEM_TO_DOM(coverage);
    DC_ITEM_TO_DOM(rights);

    return ref_node;
}

#define DC_DOM_SCAN(ITEM) do { \
    if (oscap_streq((const char*)cur->name, #ITEM)) \
        ref->ITEM = (char*) xmlNodeGetContent(cur); \
    } while(0)

struct oscap_reference *oscap_reference_new_parse(xmlTextReaderPtr reader)
{
    assert(reader != NULL);

    struct oscap_reference *ref = oscap_calloc(1, sizeof(struct oscap_reference));

    int depth = oscap_element_depth(reader);

    xmlNode* ref_node = xmlTextReaderExpand(reader);

    ref->href = (char*) xmlGetProp(ref_node, BAD_CAST "href");

    for (xmlNode* cur = ref_node->children; cur != NULL; cur = cur->next)
		if (cur->type == XML_ELEMENT_NODE) { ref->is_dublincore = true; break; }

    if (ref->is_dublincore) {
        for (xmlNode* cur = ref_node->children; cur != NULL; cur = cur->next) {
            if (cur->type != XML_ELEMENT_NODE
				|| cur->ns == NULL
				|| !oscap_streq((const char* ) cur->ns->href, (const char *) NS_DUBLINCORE))
					continue;

            DC_DOM_SCAN(title);
            DC_DOM_SCAN(creator);
            DC_DOM_SCAN(subject);
            DC_DOM_SCAN(description);
            DC_DOM_SCAN(publisher);
            DC_DOM_SCAN(contributor);
            DC_DOM_SCAN(date);
            DC_DOM_SCAN(type);
            DC_DOM_SCAN(format);
            DC_DOM_SCAN(identifier);
            DC_DOM_SCAN(source);
            DC_DOM_SCAN(language);
            DC_DOM_SCAN(relation);
            DC_DOM_SCAN(coverage);
            DC_DOM_SCAN(rights);
        }
    }
    else {
        ref->title = (char*) xmlNodeGetContent(ref_node);
    }

    if (!oscap_to_start_element(reader, depth))
	    dW("oscap_to_start_element returned `false'");

    return ref;
}

#define DC_ITEM_CLONE(ITEM) do { new_ref->ITEM = oscap_strdup(ref->ITEM); } while(0)

struct oscap_reference *oscap_reference_clone(const struct oscap_reference *ref)
{
    if (ref == NULL) return NULL;

    struct oscap_reference *new_ref = oscap_reference_new();

    new_ref->is_dublincore = ref->is_dublincore;
    DC_ITEM_CLONE(title);
    DC_ITEM_CLONE(creator);
    DC_ITEM_CLONE(subject);
    DC_ITEM_CLONE(description);
    DC_ITEM_CLONE(publisher);
    DC_ITEM_CLONE(contributor);
    DC_ITEM_CLONE(date);
    DC_ITEM_CLONE(type);
    DC_ITEM_CLONE(format);
    DC_ITEM_CLONE(identifier);
    DC_ITEM_CLONE(source);
    DC_ITEM_CLONE(language);
    DC_ITEM_CLONE(relation);
    DC_ITEM_CLONE(coverage);
    DC_ITEM_CLONE(rights);
    DC_ITEM_CLONE(href);

    return new_ref;
}

