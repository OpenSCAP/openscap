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
#include "public/oval_agent_api.h"
#include "oval_parser_impl.h"
#include "oval_definitions_impl.h"
#include "../common/util.h"
#include "../common/public/debug.h"
#include "../common/_error.h"

struct oval_definition_model *oval_parser_context_model(struct oval_parser_context
						    *context)
{
        __attribute__nonnull__(context);

	return context->definition_model;
}

static int oval_parser_log (struct oval_parser_context *, oval_xml_severity_t severity, char*);

int oval_parser_log_info(struct oval_parser_context *context, char* message){

	return oval_parser_log(context, OVAL_LOG_INFO, message);
}

int oval_parser_log_debug(struct oval_parser_context *context, char* message){

	return oval_parser_log(context, OVAL_LOG_DEBUG, message);
}

int oval_parser_log_warn(struct oval_parser_context *context, char* message){

	return oval_parser_log(context, OVAL_LOG_WARN, message);
}

static int oval_parser_log
		(struct oval_parser_context *context,
				oval_xml_severity_t severity, char* message){

        /*__attribute__nonnull__(context);

	xmlTextReader *reader = context->reader;
	char msgfield[strlen(message) + 1];
	*msgfield = 0;
	strcat(msgfield, message);
	struct oval_xml_error xml_error;
	xml_error.message     = msgfield;
	xml_error.severity    = severity;
	xml_error.system_id   = (char*) xmlTextReaderBaseUri(reader);
	xml_error.line_number = xmlTextReaderGetParserLineNumber(reader);*/
        oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_EUNKNOWN, "Deprecated error handler called !");
	/*if(xml_error.system_id!=NULL)
                oscap_free(xml_error.system_id);*/
	return 0;
}

void libxml_error_handler(void *user, const char *message,
			   xmlParserSeverities severity,
			   xmlTextReaderLocatorPtr locator)
{
	/*struct oval_parser_context *context = (struct oval_parser_context*)user;
	xVmlTextReader *reader = context->reader;
	char msgfield[strlen(message) + 1];
	*msgfield = 0;
	strcat(msgfield, message);
	struct oval_xml_error xml_error;
	xml_error.message     = msgfield;
	xml_error.severity    = severity;
	xml_error.system_id   = (char*) xmlTextReaderLocatorBaseURI(locator);
	xml_error.line_number = xmlTextReaderGetParserLineNumber(reader);*/
        oscap_setxmlerr(xmlGetLastError());

	/*if(xml_error.system_id!=NULL)
                oscap_free(xml_error.system_id);*/
}

typedef int (*_oval_parser_process_tag_func) (xmlTextReaderPtr reader,
					      struct oval_parser_context *
					      context);

static int _oval_parser_process_tags(xmlTextReaderPtr reader,
			      struct oval_parser_context *context,
			      _oval_parser_process_tag_func tag_func)
{

	const int depth = xmlTextReaderDepth(reader);
	int return_code;
	char *tagname = (char*) xmlTextReaderLocalName(reader);
	while ((return_code = xmlTextReaderRead(reader)) == 1) {
		//char *subname = (char*) xmlTextReaderLocalName(reader);
		switch (xmlTextReaderNodeType(reader)) {
		case XML_READER_TYPE_ELEMENT:{
				return_code = (*tag_func) (reader, context);
			}
			break;
		case XML_READER_TYPE_END_ELEMENT:{
				if (depth == xmlTextReaderDepth(reader)) {
					oscap_free(tagname);
					return return_code;
				}
			}
			break;
		}
		if (return_code != 1) {
			oscap_dprintf("NOTICE:oval_parser_process_tags::%s processing terminated on error\n",
			     tagname);
		}
		if (return_code != 1)
			break;
	}
	oscap_free(tagname);
	return return_code;
}

#define DEBUG_OVAL_PARSER 0
#define  STUB_OVAL_PARSER 0

/**
 * return 1 on success, -1,0 on failure
 */
int ovaldef_parse_node(xmlTextReaderPtr reader,
			      struct oval_parser_context *context)
{
	const char *oval_namespace =
	    "http://oval.mitre.org/XMLSchema/oval-definitions-5";
	const char *tagname_generator  = "generator";
	const char *tagname_definitions = "definitions";
	const char *tagname_tests = "tests";
	const char *tagname_objects = "objects";
	const char *tagname_states = "states";
	const char *tagname_variables = "variables";
	int depth = xmlTextReaderDepth(reader);/*tree_depth*/
		oscap_dprintf("DEBUG: oval_parser: START PARSE (depth = %d)",depth);
		oscap_dprintf("DEBUG: oval_parser: ENCLOSING TAG <%s:%s>", 
                        xmlTextReaderNamespaceUri(reader), xmlTextReaderLocalName(reader));
	int return_code = xmlTextReaderRead(reader);
	while (return_code == 1) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			if (xmlTextReaderDepth(reader) > depth) {
				char *tagname = (char*) xmlTextReaderLocalName(reader);
				char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
				oscap_dprintf("DEBUG: oval_parser: PROCESSING TAG <%s:%s> depth = %d",
							namespace, tagname, xmlTextReaderDepth(reader));
				int is_oval = strcmp(namespace, oval_namespace)==0;
				if (is_oval && (strcmp(tagname, tagname_definitions) == 0)) {
					return_code = (STUB_OVAL_PARSER)
						?oval_parser_skip_tag(reader, context)
					    :_oval_parser_process_tags(reader,
								      context,
								      &oval_definition_parse_tag);
				} else if (is_oval && strcmp(tagname, tagname_tests) == 0) {
					return_code = (STUB_OVAL_PARSER)
						?oval_parser_skip_tag(reader, context)
						:_oval_parser_process_tags(reader,
								      context,
								      &oval_test_parse_tag);
				} else if (is_oval && strcmp(tagname, tagname_objects) ==
					   0) {
					return_code = (STUB_OVAL_PARSER)
						?oval_parser_skip_tag(reader, context)
					    :_oval_parser_process_tags(reader,
								      context,
								      &oval_object_parse_tag);
				} else if (is_oval && strcmp(tagname, tagname_states) == 0) {
					return_code = (STUB_OVAL_PARSER)
						?oval_parser_skip_tag(reader, context)
					    :_oval_parser_process_tags(reader,
								      context,
								      &oval_state_parse_tag);
				} else if (is_oval && strcmp(tagname, tagname_variables) ==
					   0) {
					return_code = (STUB_OVAL_PARSER)
						?oval_parser_skip_tag(reader, context)
					    :_oval_parser_process_tags(reader,
								      context,
								      &oval_variable_parse_tag);
				} else if (is_oval && strcmp(tagname, tagname_generator) ==
					   0) {
					/*GENERATOR SKIPPED*/
					return_code =
					    oval_parser_skip_tag(reader,
								 context);
				} else {
                                        oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_EXMLELEM, "Unknown xml element");
					oscap_dprintf("WARNING: oval_parser: UNPROCESSED TAG <%s:%s>",namespace, tagname);
					return_code =
					    oval_parser_skip_tag(reader,
								 context);
				}
				oscap_free(tagname);
				oscap_free(namespace);
			} else
				return_code = xmlTextReaderRead(reader);
			if ((return_code == 1)
			    && (xmlTextReaderNodeType(reader) !=
				XML_READER_TYPE_ELEMENT)) {
				return_code = xmlTextReaderRead(reader);
			}
		} else if (xmlTextReaderDepth(reader) > depth) {
			return_code = xmlTextReaderRead(reader);
		} else
			break;
	}
	return return_code;
}

int ovaldef_parser_parse
    (struct oval_definition_model *model, xmlTextReader *reader, 
     void *user_arg) {
	int retcode = 0;
	if(reader){
		struct oval_parser_context context;
		context.reader        = reader;
		context.definition_model  = model;
		context.user_data     = user_arg;
		xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, &context);
		retcode = ovaldef_parse_node(reader, &context);
	}else{
                oscap_setxmlerr(xmlGetLastError());
		oscap_dprintf("ERROR: ovaldef_parser_parse: xmlTextReader is NULL (%s:%d)", __FILE__, __LINE__);
                return -1;
	}
	return retcode;
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
			char *value = (char*) xmlTextReaderValue(reader);
			(*consumer) (value, user);
			oscap_free(value);
		}
	}
	return return_code;
}

/**
 * return 1 on success, -1,0 on failure
 */
/*typedef int (*oval_xml_tag_parser)    (xmlTextReaderPtr, struct oval_parser_context*, void*);*/
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
        if (return_code < 0) 
                oscap_setxmlerr(xmlGetLastError());
	return return_code;
}

int oval_parser_boolean_attribute(xmlTextReaderPtr reader, char *attname,
				  int defval)
{
	char *string = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
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

int oval_parser_int_attribute(xmlTextReaderPtr reader, char *attname,
				  int defval)
{
	char *string = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST attname);
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
	char *platform = *(char**)user;
	if (platform == NULL)
		platform = oscap_strdup(text);
	else {
		int size = strlen(platform) + strlen(text) + 1;
		char *newtext =
			(char *) oscap_alloc(size * sizeof(char *));
		*newtext = 0;
		strcat(newtext, platform);
		strcat(newtext, text);
		oscap_free(platform);
		platform = newtext;
	}
	*(char**)user = platform;
}
