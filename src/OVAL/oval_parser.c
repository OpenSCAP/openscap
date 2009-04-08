/**
 * @file oval_parser.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#include <string.h>
#include <libxml/xmlreader.h>
#include<stddef.h>
#include "api/oval_agent_api.h"
#include "oval_parser_impl.h"
#include "oval_definitions_impl.h"

struct oval_parser_context {
	int depth;
	struct oval_object_model *model;
};

struct oval_object_model *oval_parser_context_model(struct oval_parser_context
						    *context)
{
	return context->model;
}

struct _libxml_user {
	oval_xml_error_handler error_handler;
	void *user_arg;
};

void _libxml_error_handler(void *user, const char *message,
			   xmlParserSeverities severity,
			   xmlTextReaderLocatorPtr locator)
{
	char msgfield[strlen(message) + 1];
	strcat(msgfield, message);
	struct oval_xml_error xml_error;
	xml_error.message = msgfield;
	xml_error.severity = severity;
	xml_error.system_id = xmlTextReaderLocatorBaseURI(locator);
	xml_error.line_number = xmlTextReaderLocatorLineNumber(locator);

	(*((struct _libxml_user *)user)->error_handler) (&xml_error,
							 ((struct _libxml_user
							   *)user)->user_arg);
	//(*user->error_handler)(NULL,NULL);
}

typedef int (*_oval_parser_process_tag_func) (xmlTextReaderPtr reader,
					      struct oval_parser_context *
					      context);

int _oval_parser_process_tags(xmlTextReaderPtr reader,
			      struct oval_parser_context *context,
			      _oval_parser_process_tag_func tag_func)
{
	const int depth = xmlTextReaderDepth(reader);
	int return_code;
	char *tagname = xmlTextReaderName(reader);
	while ((return_code = xmlTextReaderRead(reader)) == 1) {
		char *subname = xmlTextReaderName(reader);
		switch (xmlTextReaderNodeType(reader)) {
		case XML_READER_TYPE_ELEMENT:{
				return_code = (*tag_func) (reader, context);
			}
			break;
		case XML_READER_TYPE_END_ELEMENT:{
				if (depth == xmlTextReaderDepth(reader))
					return return_code;
			}
			break;
		}
		if (return_code != 1) {
			printf
			    ("NOTICE:oval_parser_process_tags::%s processing terminated on error\n",
			     tagname);
		}
		if (return_code != 1)
			break;
	}
	free(tagname);
	return return_code;
}

int _oval_parser_process_node(xmlTextReaderPtr reader,
			      struct oval_parser_context *context)
{
	const char *oval_namespace =
	    "http://oval.mitre.org/XMLSchema/oval-definitions-5";
	const char *tagname_definitions = "definitions";
	const char *tagname_tests = "tests";
	const char *tagname_objects = "objects";
	const char *tagname_states = "states";
	const char *tagname_variables = "variables";
	int return_code = xmlTextReaderRead(reader);
	while (return_code == 1) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			if (xmlTextReaderDepth(reader) > 0) {
				xmlChar *tagname = xmlTextReaderName(reader);
				//printf("DEBUG::_oval_parser_process_node::<%s>\n",tagname);
				xmlChar *namespace =
				    xmlTextReaderNamespaceUri(reader);
				if ((strcmp(tagname, tagname_definitions) == 0)) {
					return_code =
					    _oval_parser_process_tags(reader,
								      context,
								      &oval_definition_parse_tag);
				} else if (strcmp(tagname, tagname_tests) == 0) {
					return_code =
					    _oval_parser_process_tags(reader,
								      context,
								      &oval_test_parse_tag);
				} else if (strcmp(tagname, tagname_objects) ==
					   0) {
					return_code =
					    _oval_parser_process_tags(reader,
								      context,
								      &oval_object_parse_tag);
				} else if (strcmp(tagname, tagname_states) == 0) {
					return_code =
					    _oval_parser_process_tags(reader,
								      context,
								      &oval_state_parse_tag);
				} else if (strcmp(tagname, tagname_variables) ==
					   0) {
					return_code =
					    _oval_parser_process_tags(reader,
								      context,
								      &oval_variable_parse_tag);
				} else {
					//printf("DEBUG::_oval_parser_process_node::SKIPPING <%s>\n",tagname);
					return_code =
					    oval_parser_skip_tag(reader,
								 context);
				}
			} else
				return_code = xmlTextReaderRead(reader);
			if ((return_code == 1)
			    && (xmlTextReaderNodeType(reader) !=
				XML_READER_TYPE_ELEMENT)) {
				return_code = xmlTextReaderRead(reader);
			}
		} else if (xmlTextReaderDepth(reader) > 0) {
			return_code = xmlTextReaderRead(reader);
		} else
			break;
	}
	return return_code;
}

void oval_parser_parse
    (struct oval_object_model *model, char *docname, oval_xml_error_handler eh,
     void *user_arg) {
	xmlTextReaderPtr reader;
	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlNode *node;
	int ret;
	doc = xmlParseFile(docname);
	reader = xmlNewTextReaderFilename(docname);
	if (reader != NULL) {
		struct _libxml_user user;
		user.error_handler = eh;
		user.user_arg = user_arg;
		xmlTextReaderSetErrorHandler(reader, &_libxml_error_handler,
					     &user);
		struct oval_parser_context context;
		context.depth = 0;
		context.model = model;
		_oval_parser_process_node(reader, &context);
	}
}

int oval_parser_skip_tag(xmlTextReaderPtr reader,
			 struct oval_parser_context *context)
{
	int depth = xmlTextReaderDepth(reader);
	int return_code;
	while (((return_code = xmlTextReaderRead(reader)) == 1)
	       && xmlTextReaderDepth(reader) > depth) ;
	return return_code;
}

int oval_parser_text_value(xmlTextReaderPtr reader,
			   struct oval_parser_context *context,
			   oval_xml_value_consumer consumer, void *user)
{
	int depth = xmlTextReaderDepth(reader);
	int return_code;
	while (((return_code = xmlTextReaderRead(reader)) == 1)
	       && xmlTextReaderDepth(reader) > depth) {
		int nodetype = xmlTextReaderNodeType(reader);
		if (nodetype == XML_READER_TYPE_CDATA
		    || nodetype == XML_READER_TYPE_TEXT) {
			char *value = xmlTextReaderValue(reader);
			(*consumer) (value, user);
		}
	}
	return return_code;
}

//typedef int (*oval_xml_tag_parser)    (xmlTextReaderPtr, struct oval_parser_context*, void*);
int oval_parser_parse_tag(xmlTextReaderPtr reader,
			  struct oval_parser_context *context,
			  oval_xml_tag_parser tag_parser, void *user)
{
	int depth = xmlTextReaderDepth(reader);

	int return_code = xmlTextReaderRead(reader);
	while ((return_code == 1) && (xmlTextReaderDepth(reader) > depth)) {
		int linno = xmlTextReaderGetParserLineNumber(reader);
		int colno = xmlTextReaderGetParserColumnNumber(reader);
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			return_code = (*tag_parser) (reader, context, user);
			if (return_code == 1) {
				if (xmlTextReaderNodeType(reader) !=
				    XML_READER_TYPE_ELEMENT) {
					return_code = xmlTextReaderRead(reader);
				} else {
					int newlinno =
					    xmlTextReaderGetParserLineNumber
					    (reader);
					int newcolno =
					    xmlTextReaderGetParserColumnNumber
					    (reader);
					if (newlinno == linno
					    && newcolno == colno)
						return_code =
						    xmlTextReaderRead(reader);
				}
			}
		} else if (xmlTextReaderDepth(reader) > depth) {
			return_code = xmlTextReaderRead(reader);
		} else
			break;
	}
	return return_code;
}

int oval_parser_boolean_attribute(xmlTextReaderPtr reader, char *attname,
				  int defval)
{
	char *string = xmlTextReaderGetAttribute(reader, attname);
	int booval;
	if (string == NULL)
		booval = defval;
	else {
		if (strlen(string) == 1)
			booval = (*string == '1');
		else
			booval = (strcmp(string, "true") == 0);
		free(string);
	}
	return booval;
}
