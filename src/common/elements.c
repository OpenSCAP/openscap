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

struct oscap_nsinfo_entry *oscap_nsinfo_entry_new(void)
{
	return oscap_calloc(1, sizeof(struct oscap_nsinfo_entry));
}

struct oscap_nsinfo_entry *oscap_nsinfo_entry_new_fill(const char *nsprefix, const char *nsname)
{
	struct oscap_nsinfo_entry *entry = oscap_nsinfo_entry_new();
	entry->nsprefix = oscap_strdup(nsprefix);
	entry->nsname = oscap_strdup(nsname);
	return entry;
}

void oscap_nsinfo_entry_free(struct oscap_nsinfo_entry *entry)
{
	if (entry != NULL) {
		oscap_free(entry->nsname);
		oscap_free(entry->nsprefix);
		oscap_free(entry->schema_location);
		oscap_free(entry);
	}
}

OSCAP_ACCESSOR_STRING(oscap_nsinfo_entry, nsname)
OSCAP_ACCESSOR_STRING(oscap_nsinfo_entry, nsprefix)
OSCAP_ACCESSOR_STRING(oscap_nsinfo_entry, schema_location)

struct oscap_nsinfo *oscap_nsinfo_new(void)
{
	struct oscap_nsinfo *info = calloc(1, sizeof(struct oscap_nsinfo));
	info->entries = oscap_list_new();
	return info;
}

void oscap_nsinfo_free(struct oscap_nsinfo *info)
{
	if (info != NULL) {
		oscap_list_free(info->entries, (oscap_destruct_func) oscap_nsinfo_entry_free);
		oscap_free(info);
	}
}

OSCAP_IGETINS_GEN(oscap_nsinfo_entry, oscap_nsinfo, entries, entry)
OSCAP_ACCESSOR_SIMPLE(struct oscap_nsinfo_entry *, oscap_nsinfo, root_entry)

static const char *oscap_strlist_find_value(char ** const kvalues, const char *key)
{
	if (kvalues == NULL || key == NULL) return NULL;

	for (int i = 0; kvalues[i] != NULL && kvalues[i + 1] != NULL; i += 2)
		if (strcmp(kvalues[i], key) == 0)
			return kvalues[i + 1];

	return NULL;
}

struct oscap_nsinfo_entry *oscap_nsinfo_get_entry_by_ns(struct oscap_nsinfo *info, const char *ns)
{
    assert(info != NULL);
    assert(ns != NULL);

    struct oscap_nsinfo_entry *result = NULL;

    OSCAP_FOREACH(oscap_nsinfo_entry, entry, oscap_nsinfo_get_entries(info),
        if (oscap_streq(entry->nsname, ns)) result = entry;
    )
    return result;
}

static const char *OSCAP_XSI_NAMESPACE = "http://www.w3.org/2001/XMLSchema-instance";

bool oscap_nsinfo_add_parse(struct oscap_nsinfo *info, xmlTextReaderPtr reader, bool set_root)
{
	if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT &&
		xmlTextReaderNodeType(reader) != XML_READER_TYPE_ATTRIBUTE)
		return false;

	char *schemalocattr = (char *) xmlTextReaderGetAttributeNs(reader, BAD_CAST "schemaLocation", BAD_CAST OSCAP_XSI_NAMESPACE);
	char **schemaloc = (schemalocattr ? oscap_split(schemalocattr, " ") : NULL);
	struct oscap_nsinfo_entry *entry = NULL;
    const char *ns;
    bool present;

	for (int i = 0; xmlTextReaderMoveToAttributeNo(reader, i) == 1; ++i) {
        entry = NULL, present = false;

		if (oscap_streq((const char *) xmlTextReaderConstName(reader), "xmlns")) {
            ns = (const char *) xmlTextReaderConstValue(reader);
            entry = oscap_nsinfo_get_entry_by_ns(info, ns);
            if (entry == NULL) entry = oscap_nsinfo_entry_new_fill(NULL, ns);
            else present = true;
			if (set_root) info->root_entry = entry;
		}
		else if (oscap_streq((const char *) xmlTextReaderConstPrefix(reader), "xmlns")) {
            ns = (const char *) xmlTextReaderConstValue(reader);
            const char *prefix = (const char *) xmlTextReaderConstLocalName(reader);
            entry = oscap_nsinfo_get_entry_by_ns(info, ns);

            if (entry != NULL) oscap_nsinfo_entry_set_nsprefix(entry, prefix), present = true;
			else entry = oscap_nsinfo_entry_new_fill(prefix, ns);
        }
        else continue;

		oscap_nsinfo_entry_set_schema_location(entry, oscap_strlist_find_value(schemaloc, entry->nsname));
        if (!present) oscap_list_add(info->entries, entry);
	}

	oscap_free(schemaloc);
	oscap_free(schemalocattr);

	return true;
}

struct oscap_nsinfo *oscap_nsinfo_new_parse(xmlTextReaderPtr reader)
{
	assert(reader != NULL);
	struct oscap_nsinfo *info = oscap_nsinfo_new();
	if (!oscap_nsinfo_add_parse(info, reader, true)) {
		oscap_nsinfo_free(info);
		return NULL;
	}
	return info;
}

struct oscap_nsinfo *oscap_nsinfo_new_file(const char *fname)
{
	if (fname == NULL) return NULL;
	xmlTextReaderPtr reader = xmlReaderForFile(fname, NULL, 0);
	if (reader == NULL) return NULL;
	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != 1);
	struct oscap_nsinfo *info = oscap_nsinfo_new_parse(reader);
	xmlFreeTextReader(reader);
	return info;
}

