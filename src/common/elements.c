/**
 * @file elements.c
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
 *      Maros Barabas <mbarabas@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "public/oscap.h"
#include "util.h"
#include "list.h"
#include "_error.h"
#include "debug_priv.h"
#include "elements.h"


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
		case XML_READER_TYPE_ELEMENT:
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

void oscap_text_consumer(char *text, void *user)
{
	char *platform = *(char **)user;
	if (platform == NULL)
		platform = oscap_strdup(text);
	else {
		int size = strlen(platform) + strlen(text) + 1;
		char *newtext = (char *) oscap_alloc(size * sizeof(char));
		*newtext = 0;
		strcat(newtext, platform);
		strcat(newtext, text);
		oscap_free(platform);
		platform = newtext;
	}
	*(char **)user = platform;
}

/* -1 error; 0 OK */
int oscap_parser_text_value(xmlTextReaderPtr reader, oscap_xml_value_consumer consumer, void *user)
{
	int depth = xmlTextReaderDepth(reader);
	bool has_value = false;
	int ret = 0;

	if (xmlTextReaderIsEmptyElement(reader)) {
		return ret;
	}

	xmlTextReaderRead(reader);
	while (xmlTextReaderDepth(reader) > depth) {
		int nodetype = xmlTextReaderNodeType(reader);
		if (nodetype == XML_READER_TYPE_CDATA || nodetype == XML_READER_TYPE_TEXT) {
			char *value = (char *)xmlTextReaderValue(reader);
			(*consumer) (value, user);
			oscap_free(value);
			has_value = true;
		}
		if (xmlTextReaderRead(reader) != 1) {
			ret = -1;
			break;
		}
	}

	if (!has_value)
		(*consumer) ("", user);

	return ret;
}

char *oscap_element_string_copy(xmlTextReaderPtr reader)
{
	int t;

	if (xmlTextReaderIsEmptyElement(reader))
		return NULL;

	t = xmlTextReaderNodeType(reader);
	if (t == XML_ELEMENT_NODE || t == XML_ATTRIBUTE_NODE)
		xmlTextReaderRead(reader);
	if (xmlTextReaderHasValue(reader))
		return (char *)xmlTextReaderValue(reader);
	else
		return (char *) calloc(1,1);
}

const char *oscap_element_string_get(xmlTextReaderPtr reader)
{
	if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT ||
			xmlTextReaderNodeType(reader) == XML_READER_TYPE_ATTRIBUTE)
		xmlTextReaderRead(reader);
	if (xmlTextReaderHasValue(reader))
		return (const char *)xmlTextReaderConstValue(reader);
	return NULL;
}

int oscap_element_depth(xmlTextReaderPtr reader)
{
	int depth = xmlTextReaderDepth(reader);
	switch (xmlTextReaderNodeType(reader)) {
	case XML_READER_TYPE_ATTRIBUTE:
	case XML_READER_TYPE_TEXT:
	case XML_READER_TYPE_ENTITY_REFERENCE:
		return depth - 1;
	default:
		return depth;
	}
}

char *oscap_get_xml(xmlTextReaderPtr reader)
{
	return (char *)xmlTextReaderReadInnerXml(reader);
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

xmlNode *oscap_xmlstr_to_dom(xmlNode *parent, const char *elname, const char *content)
{
	char *str = oscap_sprintf("<x xmlns:xhtml='http://www.w3.org/1999/xhtml'>%s</x>", content);
	xmlDoc *doc = xmlReadMemory(str, strlen(str), NULL, NULL,
		XML_PARSE_RECOVER | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET | XML_PARSE_NSCLEAN);
	xmlNode *text_node = xmlCopyNode(xmlDocGetRootElement(doc), 1);
	xmlNodeSetName(text_node, BAD_CAST elname);
	xmlAddChild(parent, text_node);
	xmlFreeDoc(doc);
	oscap_free(str);
	return text_node;
}


const char *oscap_strlist_find_value(char ** const kvalues, const char *key)
{
	if (kvalues == NULL || key == NULL) return NULL;

	for (int i = 0; kvalues[i] != NULL && kvalues[i + 1] != NULL; i += 2)
		if (strcmp(kvalues[i], key) == 0)
			return kvalues[i + 1];

	return NULL;
}

int oscap_xml_save_filename(const char *filename, xmlDocPtr doc)
{
	xmlOutputBufferPtr buff;
	int xmlCode;

	if (strcmp(filename, "-") == 0) {
		xmlCode = xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);
	}
	else {
		int fd = open(filename, O_CREAT|O_TRUNC|O_WRONLY,
				S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		if (fd < 0) {
			oscap_seterr(OSCAP_EFAMILY_GLIBC, "%s '%s'", strerror(errno), filename);
			xmlFreeDoc(doc);
			return -1;
		}

		buff = xmlOutputBufferCreateFd(fd, NULL);
		if (buff == NULL) {
			close(fd);
			oscap_setxmlerr(xmlGetLastError());
			oscap_dlprintf(DBG_W, "xmlOutputBufferCreateFile() failed.\n");
			xmlFreeDoc(doc);
			return -1;
		}

		xmlCode = xmlSaveFormatFileTo(buff, doc, "UTF-8", 1);
		close(fd);
	}
	if (xmlCode <= 0) {
		oscap_setxmlerr(xmlGetLastError());
		oscap_dlprintf(DBG_W, "No bytes exported: xmlCode: %d.\n", xmlCode);
	}

	return (xmlCode >= 1) ? 1 : -1;
}

int oscap_xml_save_filename_free(const char *filename, xmlDocPtr doc)
{
	int ret = oscap_xml_save_filename(filename, doc);
	xmlFreeDoc(doc);
	return ret;
}

xmlNs *lookup_xsi_ns(xmlDoc *doc)
{
	// Look-up xsi namespace pointer. We can be pretty sure that this namespace
	// is defined in root element, because it usually carries xsi:schemaLocation
	// attribute.
	xmlNsPtr ns_xsi = xmlSearchNsByHref(doc, xmlDocGetRootElement(doc), OSCAP_XMLNS_XSI);
	if (ns_xsi == NULL) {
		ns_xsi = xmlNewNs(xmlDocGetRootElement(doc), OSCAP_XMLNS_XSI, BAD_CAST "xsi");
	}
	return ns_xsi;
}
