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
#include <stddef.h>
#include "api/oval_agent_api.h"
#include "oval_parser_impl.h"
#include "oval_definitions_impl.h"
#include "oval_results_impl.h"

#define NAMESPACE_OVALRES "http://oval.mitre.org/XMLSchema/oval-results-5"
#define NAMESPACE_OVALDEF "http://oval.mitre.org/XMLSchema/oval-definitions-5"

#define DEBUG_OVALRES_PARSER 1

int _ovalres_parser_process_node_consume_collected_objects(xmlTextReaderPtr reader,
		  struct oval_parser_context *context, void *null)
{
	return oval_syschar_parse_tag(reader, context);
}

int _ovalres_parser_process_node_consume_system_data(xmlTextReaderPtr reader,
		  struct oval_parser_context *context, void *null)
{
	return oval_sysdata_parse_tag(reader, context);
}

//typedef int (*oval_xml_tag_parser)    (xmlTextReaderPtr, struct oval_parser_context*, void*);
int _oval_results_parser_consume_definitions
	(xmlTextReaderPtr reader, struct oval_parser_context *context, void *null)
{
	if(DEBUG_OVALRES_PARSER)oval_parser_log_debug(context,"Calling oval_parser_parse_node");
	return ovaldef_parse_node(reader, context);
}

void _oval_results_parser_consume_system
	(struct oval_result_system *system, struct oval_results_model *model)
{
	oval_results_model_add_system(model, system);
}

int _ovalres_parser_parse_system
	(xmlTextReaderPtr reader,
			struct oval_parser_context *context, void *null)
{
	struct oval_results_model *model = context->results_model;
	struct oval_object_model  *object_model = context->object_model;
	struct oval_syschar_model *syschar_model = oval_syschar_model_new(object_model, NULL);
	return oval_result_system_parse
		(reader, context, syschar_model,
			(oscap_consumer_func)_oval_results_parser_consume_system, model);
}

int _ovalres_parser_process_node
	(xmlTextReaderPtr reader, struct oval_parser_context *context, struct oval_result_directives **directives)
{
	int return_code = xmlTextReaderRead(reader);
	while (return_code == 1) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			if(DEBUG_OVALRES_PARSER){//DEBUG
				char message[200]; *message = 0;
				sprintf(message,
						"ovalres_parser: at depth %d",
						xmlTextReaderDepth(reader));
				oval_parser_log_debug(context, message);
			}//DEBUG
			if (xmlTextReaderDepth(reader) > 0) {
				char *tagname = (char*) xmlTextReaderLocalName(reader);
				char *namespace =
				    (char*) xmlTextReaderNamespaceUri(reader);
				if(DEBUG_OVALRES_PARSER){//DEBUG
					char message[200]; *message = 0;
					sprintf(message,
							"ovalres_parser: processing <%s:%s>",
							namespace, tagname);
					oval_parser_log_debug(context, message);
				}//DEBUG
				int is_ovalres = strcmp((const char *) NAMESPACE_OVALRES, namespace)==0;
				int is_ovaldef = (is_ovalres)?false:(strcmp((const char *) NAMESPACE_OVALDEF, namespace)==0);
				if (is_ovalres && (strcmp(tagname, "generator") == 0)) {
					//SKIP GENERATOR CODE
					if(DEBUG_OVALRES_PARSER)
						oval_parser_log_debug(context, "ovalres_parser_process_node: SKIPPING <generator>");
					return_code =
					    oval_parser_skip_tag(reader,context);
				}else if (is_ovalres && (strcmp(tagname, "directives") == 0)) {
					*directives = oval_result_directives_new();
					return_code = oval_result_directives_parse_tag(reader, context, *directives);
				}else if (is_ovaldef && (strcmp(tagname, "oval_definitions") == 0)) {
					if(DEBUG_OVALRES_PARSER)oval_parser_log_debug(context,"Calling oval_parser_parse_node");
					return_code = ovaldef_parse_node(reader, context);
				}else if (is_ovalres && (strcmp(tagname, "results") == 0)) {
					return_code = oval_parser_parse_tag
						(reader, context,
							(oval_xml_tag_parser)_ovalres_parser_parse_system, NULL);
				} else {
					char message[200]; *message = 0;
					sprintf(message,
							"ovalres_parser_process_node: UNPROCESSED TAG <%s:%s>",
							namespace, tagname);
					oval_parser_log_warn(context, message);
					return_code = oval_parser_skip_tag(reader, context);
				}
				free(tagname);
				free(namespace);
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

struct oval_result_directives *ovalres_parser_parse
    (struct oval_results_model *model, xmlTextReader *reader, oval_xml_error_handler eh,
     void *user_arg)
{
	struct oval_parser_context context;
	context.error_handler = eh;
	context.reader          = reader;
	context.results_model   = model;
	context.object_model    = oval_results_model_get_object_model(model);
	context.syschar_sysinfo = NULL;
	context.user_data       = user_arg;
	xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, &context);
	char *tagname   = (char*) xmlTextReaderLocalName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int is_ovalres = strcmp(NAMESPACE_OVALRES, namespace)==0;
	struct oval_result_directives *directives[] = {NULL};
	if(is_ovalres && (strcmp(tagname,"oval_results")==0)){
		_ovalres_parser_process_node(reader, &context, directives);
	}else{
		char message[200]; *message = 0;
		sprintf(message,
				"ovalres_parser: UNPROCESSED TAG <%s:%s>",
				namespace, tagname);
		oval_parser_log_warn(&context, message);
		oval_parser_skip_tag(reader,&context);
	}
	return *directives;
}
