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
#include "public/oscap.h"


OSCAP_ACCESSOR_STRING(xml_metadata, nspace)
OSCAP_ACCESSOR_STRING(xml_metadata, URI)
OSCAP_ACCESSOR_STRING(xml_metadata, lang)
OSCAP_ACCESSOR_STRING(oscap_title, content)
OSCAP_ITERATOR_GEN(oscap_title)
OSCAP_ITERATOR_GEN(xml_metadata)
OSCAP_ITERATOR_REMOVE_F(xml_metadata)
OSCAP_ITERATOR_REMOVE_F(oscap_title)

const struct oscap_string_map OSCAP_BOOL_MAP[] = {
	{true, "true"}, {true, "True"}, {true, "TRUE"},
	{true, "yes"}, {true, "Yes"}, {true, "YES"},
	{true, "1"}, {false, NULL}
};


bool oscap_to_start_element(xmlTextReaderPtr reader, int depth)
{
	//int olddepth = xmlTextReaderDepth(reader);
	while (xmlTextReaderDepth(reader) >= depth) {
		switch (xmlTextReaderNodeType(reader)) {
			//TODO: change int values to macros XML_ELEMENT_TYPE_*
		case 1:
			if (xmlTextReaderDepth(reader) == depth)
				return true;
		default:
			break;
		}
		if (xmlTextReaderRead(reader) != 1)
			break;
	}
	return false;
}

char *oscap_element_string_copy(xmlTextReaderPtr reader)
{
	if (xmlTextReaderNodeType(reader) == 1 || xmlTextReaderNodeType(reader) == 2)
		xmlTextReaderRead(reader);
	if (xmlTextReaderHasValue(reader))
		return (char *)xmlTextReaderValue(reader);
	return NULL;
}

const char *oscap_element_string_get(xmlTextReaderPtr reader)
{
	if (xmlTextReaderNodeType(reader) == 1 || xmlTextReaderNodeType(reader) == 2)
		xmlTextReaderRead(reader);
	if (xmlTextReaderHasValue(reader))
		return (const char *)xmlTextReaderConstValue(reader);
	return NULL;
}

int oscap_element_depth(xmlTextReaderPtr reader)
{
	int depth = xmlTextReaderDepth(reader);
	switch (xmlTextReaderNodeType(reader)) {
	case 2:
	case 5:
	case 3:
		return depth - 1;
	default:
		return depth;
	}
}

char *oscap_get_xml(xmlTextReaderPtr reader)
{
	return (char *)xmlTextReaderReadOuterXml(reader);
}

time_t oscap_get_date(const char *date)
{
	if (date) {
		struct tm tm;
		memset(&tm, 0, sizeof(tm));
		if (sscanf(date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) == 3) {
			tm.tm_mon -= 1;
			tm.tm_year -= 1900;
			return mktime(&tm);
		}
	}
	return 0;
}

time_t oscap_get_datetime(const char *date)
{
	if (date) {
		struct tm tm;
		memset(&tm, 0, sizeof(tm));
		if (sscanf
		    (date, "%d-%d-%dT%d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min,
		     &tm.tm_sec) == 6) {
			tm.tm_mon -= 1;
			tm.tm_year -= 1900;
			return mktime(&tm);
		}
	}
	return 0;
}


struct xml_metadata *xml_metadata_new()
{

	struct xml_metadata *item;
	item = oscap_alloc(sizeof(struct xml_metadata));
	if (item == NULL)
		return NULL;

	item->nspace = NULL;
	item->URI = NULL;
	item->lang = NULL;

	return item;
}

struct oscap_title *oscap_title_parse(xmlTextReaderPtr reader, const char *name)
{

	struct oscap_title *ret;

	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST name) != 0)
		return NULL;

	if ((ret = oscap_alloc(sizeof(struct oscap_title))) == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct oscap_title));

	ret->xml.lang = oscap_strdup((char *)xmlTextReaderConstXmlLang(reader));
	ret->xml.nspace = (char *)xmlTextReaderPrefix(reader);

	ret->content = (char *)xmlTextReaderReadString(reader);

	return ret;
}

const char *oscap_title_get_language(const struct oscap_title *title)
{
	return title->xml.lang;
}

void oscap_title_export(const struct oscap_title *title, xmlTextWriterPtr writer)
{

	xmlTextWriterStartElementNS(writer, BAD_CAST title->xml.nspace, BAD_CAST "title", NULL);
	if (title->xml.lang != NULL)
		xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST title->xml.lang);
	xmlTextWriterWriteString(writer, BAD_CAST title->content);
	xmlTextWriterEndElement(writer);
}

void oscap_title_free(struct oscap_title *title)
{

	if (title == NULL)
		return;

	oscap_free(title->content);
	xml_metadata_free(&title->xml);
	oscap_free(title);
}

void xml_metadata_free(struct xml_metadata *xml)
{

	oscap_free(xml->lang);
	oscap_free(xml->nspace);
	oscap_free(xml->URI);
}
