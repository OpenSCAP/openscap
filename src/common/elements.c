/**
 * @file elements.c
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
 *      Maros Barabas <mbarabas@redhat.com>
 */

#include "elements.h"

#include <string.h>

#include "util.h"
#include "list.h"

OSCAP_GETTER(const char*, xml_metadata, namespace)
OSCAP_GETTER(const char*, xml_metadata, lang)
OSCAP_ACCESSOR_STRING(oscap_title, content)
OSCAP_ITERATOR_GEN(oscap_title)
OSCAP_ITERATOR_GEN(xml_metadata)
OSCAP_ITERATOR_REMOVE_F(xml_metadata)
OSCAP_ITERATOR_REMOVE_F(oscap_title)

struct oscap_title * oscap_title_parse(xmlTextReaderPtr reader, const char * name) {

	struct oscap_title *ret;

        if (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST name) != 0)
		return NULL;

	if ((ret = oscap_alloc(sizeof(struct oscap_title))) == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct oscap_title));

        ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);

	ret->content = (char *) xmlTextReaderReadString(reader);

	return ret;
}

const char *oscap_title_get_language(const struct oscap_title *title) {
	return title->xml.lang;
}

void oscap_title_export(const struct oscap_title * title, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST title->xml.namespace, BAD_CAST "title", NULL);
        if (title->xml.lang != NULL) 
                xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST title->xml.lang);
        xmlTextWriterWriteString(writer, BAD_CAST title->content);
        xmlTextWriterEndElement(writer);
}

void oscap_title_free(struct oscap_title * title) {

	if (title == NULL) return;

	oscap_free(title->content);
        xml_metadata_free(&title->xml);
	oscap_free(title);
}

void xml_metadata_free(struct xml_metadata * xml) {

        oscap_free(xml->lang);
        oscap_free(xml->namespace);
        oscap_free(xml->URI);
}
