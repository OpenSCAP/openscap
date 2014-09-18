/**
 * @file oval_parser_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009-2014 Red Hat Inc., Durham, North Carolina.
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

#ifndef OVAL_PARSER_H_
#define OVAL_PARSER_H_

#include <libxml/xmlreader.h>
#include "public/oval_agent_api.h"
#include "common/util.h"

OSCAP_HIDDEN_START;

#define OVAL_ROOT_ELM_DEFINITIONS "oval_definitions"
#define OVAL_ROOT_ELM_DIRECTIVES "oval_directives"
#define OVAL_ROOT_ELM_RESULTS "oval_results"
#define OVAL_ROOT_ELM_SYSCHARS "oval_system_characteristics"
#define OVAL_ROOT_ELM_VARIABLES "oval_variables"

struct oval_parser_context {
	struct oval_definition_model *definition_model;
	struct oval_syschar_model *syschar_model;
	struct oval_results_model *results_model;
	struct oval_variable_model *variable_model;
	struct oval_directives_model *directives_model;
	xmlTextReader *reader;
	void *user_data;
};

int oval_definition_model_parse(xmlTextReaderPtr, struct oval_parser_context *);
int oval_syschar_model_parse(xmlTextReaderPtr, struct oval_parser_context *);
int oval_results_model_parse(xmlTextReaderPtr , struct oval_parser_context *);

int oval_parser_boolean_attribute(xmlTextReaderPtr reader, char *attname, int defval);
int oval_parser_int_attribute(xmlTextReaderPtr reader, char *attname, int defval);

typedef int (*oval_xml_tag_parser) (xmlTextReaderPtr, struct oval_parser_context *, void *);
int oval_parser_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_xml_tag_parser, void *);
int oval_parser_skip_tag(xmlTextReaderPtr reader, struct oval_parser_context *context);

char *oval_determine_document_schema_version_priv(xmlTextReader *reader, oscap_document_type_t doc_type);

OSCAP_HIDDEN_END;

#endif				/* OVAL_PARSER_H_ */
