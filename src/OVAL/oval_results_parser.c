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
#include "public/oval_agent_api.h"
#include "oval_parser_impl.h"
#include "oval_definitions_impl.h"
#include "oval_results_impl.h"
#include "../common/util.h"
#include "../common/public/debug.h"
#include "../common/_error.h"

#define NAMESPACE_OVALRES "http://oval.mitre.org/XMLSchema/oval-results-5"
#define NAMESPACE_OVALDEF "http://oval.mitre.org/XMLSchema/oval-definitions-5"

static void _oval_results_parser_consume_system
	(struct oval_result_system *sys, struct oval_results_model *model)
{
	oval_results_model_add_system(model, sys);
}

static int _ovalres_parser_parse_system
	(xmlTextReaderPtr reader,
			struct oval_parser_context *context, void *null)
{
        __attribute__nonnull__(context);

	struct oval_results_model *model = context->results_model;
	struct oval_definition_model  *definition_model = context->definition_model;
	struct oval_syschar_model *syschar_model = oval_syschar_model_new(definition_model);
	return oval_result_system_parse
		(reader, context, syschar_model,
			(oscap_consumer_func)_oval_results_parser_consume_system, model);
}

static int _ovalres_parser_process_node
	(xmlTextReaderPtr reader, struct oval_parser_context *context, struct oval_result_directives **directives)
{
	int return_code = xmlTextReaderRead(reader);
	while (return_code == 1) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        oscap_dprintf("DEBUG: ovalres_parser: at depth %d", xmlTextReaderDepth(reader));
			if (xmlTextReaderDepth(reader) > 0) {
				char *tagname = (char*) xmlTextReaderLocalName(reader);
				char *namespace =
				    (char*) xmlTextReaderNamespaceUri(reader);
				    oscap_dprintf("DEBUG: ovalres_parser: processing <%s:%s>", namespace, tagname);
				int is_ovalres = strcmp((const char *) NAMESPACE_OVALRES, namespace)==0;
				int is_ovaldef = (is_ovalres)?false:(strcmp((const char *) NAMESPACE_OVALDEF, namespace)==0);
				if (is_ovalres && (strcmp(tagname, "generator") == 0)) {
					/*SKIP GENERATOR CODE*/
					oscap_dprintf("DEBUG: ovalres_parser_process_node: SKIPPING <generator>");
					return_code =
					    oval_parser_skip_tag(reader,context);
				}else if (is_ovalres && (strcmp(tagname, "directives") == 0)) {
                                        assert(context != NULL);
					*directives = oval_result_directives_new(context->results_model);
					return_code = oval_result_directives_parse_tag(reader, context, *directives);
				}else if (is_ovaldef && (strcmp(tagname, "oval_definitions") == 0)) {
                                        oscap_dprintf("DEBUG: Calling oval_parser_parse_node");
					return_code = ovaldef_parse_node(reader, context);
				}else if (is_ovalres && (strcmp(tagname, "results") == 0)) {
					return_code = oval_parser_parse_tag
						(reader, context,
							(oval_xml_tag_parser)_ovalres_parser_parse_system, NULL);
				} else {
                                        oscap_seterr(ERR_FAMILY_OSCAP, OSCAP_EXMLELEM, "Unknown element");
					oscap_dprintf("WARNING: ovalres_parser_process_node: UNPROCESSED TAG <%s:%s>",
							namespace, tagname);
					return_code = oval_parser_skip_tag(reader, context);
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
		} else if (xmlTextReaderDepth(reader) > 0) {
			return_code = xmlTextReaderRead(reader);
		} else
			break;
	}
	return return_code;
}

struct oval_result_directives *ovalres_parser_parse
    (struct oval_results_model *model, xmlTextReader *reader,
     void *user_arg)
{
	struct oval_parser_context context;
	context.reader          = reader;
	context.results_model   = model;
	context.definition_model    = oval_results_model_get_definition_model(model);
	/*context.syschar_sysinfo = NULL;*/
	context.user_data       = user_arg;
        oscap_setxmlerr(xmlGetLastError());
	xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, &context);
	char *tagname   = (char*) xmlTextReaderLocalName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int is_ovalres = strcmp(NAMESPACE_OVALRES, namespace)==0;
	struct oval_result_directives *directives[] = {NULL};
	if(is_ovalres && (strcmp(tagname,"oval_results")==0)){
		_ovalres_parser_process_node(reader, &context, directives);
	}else{
		oscap_dprintf("WARNING: ovalres_parser: UNPROCESSED TAG <%s:%s>",
				namespace, tagname);
		oval_parser_skip_tag(reader,&context);
	}
	return *directives;
}
