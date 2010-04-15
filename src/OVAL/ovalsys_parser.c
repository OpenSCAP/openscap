/**
 * @file oval_parser.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
#include "oval_system_characteristics_impl.h"
#include "../common/util.h"
#include "../common/public/debug.h"
#include "../common/_error.h"

const char NAMESPACE_OVALSYS[] = "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5";

static int _ovalsys_parser_process_node_consume_collected_objects(xmlTextReaderPtr reader,
								  struct oval_parser_context *context, void *null)
{
	return oval_syschar_parse_tag(reader, context);
}

static int _ovalsys_parser_process_node_consume_system_data(xmlTextReaderPtr reader,
							    struct oval_parser_context *context, void *null)
{
	return oval_sysdata_parse_tag(reader, context);
}

static int _ovalsys_parser_process_node(xmlTextReaderPtr reader, struct oval_parser_context *context)
{
	int return_code = xmlTextReaderRead(reader);
	while (return_code == 1) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			oscap_dprintf("DEBUG: ovalsys_parser: at depth %d", xmlTextReaderDepth(reader));
			if (xmlTextReaderDepth(reader) > 0) {
				char *tagname = (char *)xmlTextReaderName(reader);
				char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
				oscap_dprintf("DEBUG: ovalsys_parser: processing <%s:%s>", namespace, tagname);
				int is_ovalsys = strcmp((const char *)NAMESPACE_OVALSYS, namespace) == 0;
				if (is_ovalsys && (strcmp(tagname, "generator") == 0)) {
					//SKIP GENERATOR CODE
					return_code = oval_parser_skip_tag(reader, context);
				} else if (is_ovalsys && (strcmp(tagname, "system_info") == 0)) {
					return_code = oval_sysinfo_parse_tag(reader, context);
				} else if (is_ovalsys && (strcmp(tagname, "collected_objects") == 0)) {
					return_code =
					    oval_parser_parse_tag(reader, context,
								  &_ovalsys_parser_process_node_consume_collected_objects,
								  NULL);
				} else if (is_ovalsys && (strcmp(tagname, "system_data") == 0)) {
					return_code =
					    oval_parser_parse_tag(reader, context,
								  &_ovalsys_parser_process_node_consume_system_data,
								  NULL);
				} else {
					oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_EXMLELEM, "Unknown element");
					oscap_dprintf("WARNING: ovalsys_parser: UNPROCESSED TAG <%s:%s>",
						      namespace, tagname);
					return_code = oval_parser_skip_tag(reader, context);
				}
				oscap_free(tagname);
				oscap_free(namespace);
			} else
				return_code = xmlTextReaderRead(reader);
			if ((return_code == 1)
			    && (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT)) {
				return_code = xmlTextReaderRead(reader);
			}
		} else if (xmlTextReaderDepth(reader) > 0) {
			return_code = xmlTextReaderRead(reader);
		} else
			break;
	}
	return return_code;
}

/**
 * return -1 on error >=0 otherwise
 */
int ovalsys_parser_parse(struct oval_syschar_model *model, xmlTextReader * reader, void *user_arg) {
	struct oval_parser_context context;
	context.reader = reader;
	context.definition_model = oval_syschar_model_get_definition_model(model);
	context.syschar_model = model;
	//context.syschar_sysinfo = NULL;
	context.user_data = user_arg;
	int return_code = 1;
	xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, &context);
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int is_ovalsys = strcmp((char *)NAMESPACE_OVALSYS, namespace) == 0;
	if (is_ovalsys && (strcmp(tagname, "oval_system_characteristics") == 0)) {
		return_code = _ovalsys_parser_process_node(reader, &context);
	} else {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_EXMLELEM,
			     "Missing expected oval_system_characteristics element");
		oscap_dprintf("WARNING: ovalsys_parser: UNPROCESSED TAG <%s:%s>", namespace, tagname);
		oval_parser_skip_tag(reader, &context);
		return_code = -1;
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}
