/**
 * @file oval_parser.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <libxml/xmlreader.h>
#include <stddef.h>

#include "public/oval_agent_api.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_definitions_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/elements.h"
#include "common/public/oscap.h"

/**
 * -1 error; 0 OK; 1 warning
 */
int oval_parser_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, oval_xml_tag_parser tag_parser, void *user)
{
	int ret=0;
	int depth = xmlTextReaderDepth(reader);

	xmlTextReaderRead(reader);
	while ( (ret!=-1) &&  (xmlTextReaderDepth(reader) > depth) ) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			ret = (*tag_parser) (reader, context, user);
		}
                if (xmlTextReaderRead(reader) != 1) {
			ret = -1;
                        break;
		}
	}
	return ret;
}


char *oval_determine_document_schema_version(const char *document, oscap_document_type_t doc_type)
{
	xmlTextReaderPtr reader;
	const char *root_name;
	const char* elm_name;
	int depth;
	xmlChar *version = NULL;

	reader = xmlReaderForFile(document, NULL, 0);
	if (!reader) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Unable to open file: '%s'", document);
		return NULL;
	}

	xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, NULL);

	/* find root element */
	while (xmlTextReaderRead(reader) == 1
	       && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
	/* verify document type */
	switch (doc_type) {
	case OSCAP_DOCUMENT_OVAL_DEFINITIONS:
		root_name = OVAL_ROOT_ELM_DEFINITIONS;
		break;
	case OSCAP_DOCUMENT_OVAL_DIRECTIVES:
		root_name = OVAL_ROOT_ELM_DIRECTIVES;
		break;
	case OSCAP_DOCUMENT_OVAL_RESULTS:
		root_name = OVAL_ROOT_ELM_RESULTS;
		break;
	case OSCAP_DOCUMENT_OVAL_SYSCHAR:
		root_name = OVAL_ROOT_ELM_SYSCHARS;
		break;
	case OSCAP_DOCUMENT_OVAL_VARIABLES:
		root_name = OVAL_ROOT_ELM_VARIABLES;
		break;
	default:
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Unknown document type: %d.", doc_type);
		xmlFreeTextReader(reader);
		return NULL;
	}
	/* verify root element's name */
	elm_name = (const char *) xmlTextReaderConstLocalName(reader);
	if (!elm_name) {
		oscap_setxmlerr(xmlGetLastError());
		xmlFreeTextReader(reader);
		return NULL;
	}
	if (strcmp(root_name, elm_name)) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Document type doesn't match root element's name: '%s'.", elm_name);
		xmlFreeTextReader(reader);
		return NULL;
	}
	/* find generator */
	while (xmlTextReaderRead(reader) == 1
	       && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
	elm_name = (const char *) xmlTextReaderConstLocalName(reader);
	if (!elm_name || strcmp(elm_name, "generator")) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unexpected element: '%s'.", elm_name);
		xmlFreeTextReader(reader);
		return NULL;
	}
	/* find schema_version */
	depth = xmlTextReaderDepth(reader);
	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderDepth(reader) > depth) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT)
			continue;

		elm_name = (const char *) xmlTextReaderConstLocalName(reader);
		if (!strcmp(elm_name, "schema_version")) {
			version = xmlTextReaderReadString(reader);
			break;
		}
	}

	xmlFreeTextReader(reader);
	char* ret = oscap_strdup((const char*)version);
	xmlFree(version);

	return ret;
}

/*
 * -1 error; 0 OK; 1 warning
 */
int oval_definition_model_parse(xmlTextReaderPtr reader, struct oval_parser_context *context)
{
	const char *tagname_generator = "generator";
	const char *tagname_definitions = "definitions";
	const char *tagname_tests = "tests";
	const char *tagname_objects = "objects";
	const char *tagname_states = "states";
	const char *tagname_variables = "variables";

	int depth = xmlTextReaderDepth(reader);
	int ret = 0;

	xmlTextReaderRead(reader);
	while ((xmlTextReaderDepth(reader) > depth) && (ret != -1 )) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			char *tagname = (char *)xmlTextReaderLocalName(reader);
			char *namespace = (char *)xmlTextReaderNamespaceUri(reader);

			int is_oval = strcmp((const char *)OVAL_DEFINITIONS_NAMESPACE, namespace) == 0;
			if (is_oval && (strcmp(tagname, tagname_definitions) == 0)) {
				ret = oval_parser_parse_tag(reader, context, &oval_definition_parse_tag, NULL);
			} else if (is_oval && strcmp(tagname, tagname_tests) == 0) {
				ret = oval_parser_parse_tag(reader, context, &oval_test_parse_tag, NULL);
			} else if (is_oval && strcmp(tagname, tagname_objects) == 0) {
				ret =  oval_parser_parse_tag(reader, context, &oval_object_parse_tag, NULL);
			} else if (is_oval && strcmp(tagname, tagname_states) == 0) {
				ret =  oval_parser_parse_tag(reader, context, &oval_state_parse_tag, NULL);
			} else if (is_oval && strcmp(tagname, tagname_variables) == 0) {
				ret =  oval_parser_parse_tag(reader, context, &oval_variable_parse_tag, NULL);
			} else if (is_oval && strcmp(tagname, tagname_generator) == 0) {
				struct oval_generator *gen;
				gen = oval_definition_model_get_generator(context->definition_model);
				ret = oval_parser_parse_tag(reader, context, &oval_generator_parse_tag, gen);
			} else {
				dW("Unprocessed tag: <%s:%s>.", namespace, tagname);
				oval_parser_skip_tag(reader, context);
			}

			oscap_free(tagname);
			oscap_free(namespace);
		} else {
			if (xmlTextReaderRead(reader) != 1) {
				ret = -1;
				break;
			}
		}
	}

	return ret;
}

/* -1 error; 0 OK */
int oval_parser_skip_tag(xmlTextReaderPtr reader, struct oval_parser_context *context)
{
	int ret = 0;
	int depth = xmlTextReaderDepth(reader);

	xmlTextReaderRead(reader);
	while ( xmlTextReaderDepth(reader) > depth )  {
		if (xmlTextReaderRead(reader) != 1) {
			ret = -1;
                        break;
		}
	}

	return ret;
}

/* -1 error; 0 OK */
int oval_parser_text_value(xmlTextReaderPtr reader, struct oval_parser_context *context, oval_xml_value_consumer consumer, void *user)
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


int oval_parser_boolean_attribute(xmlTextReaderPtr reader, char *attname, int defval)
{
	char *string = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST attname);
	int booval;
	if (string == NULL)
		booval = defval;
	else {
		if (strlen(string) == 1)
			booval = (*string == '1');
		else
			booval = (strcmp(string, "true") == 0);
		oscap_free(string);
	}
	return booval;
}

int oval_parser_int_attribute(xmlTextReaderPtr reader, char *attname, int defval)
{
	char *string = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST attname);
	int value;
	if (string == NULL)
		value = defval;
	else {
		value = atoi(string);
		oscap_free(string);
	}
	return value;
}

void oval_text_consumer(char *text, void *user)
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
