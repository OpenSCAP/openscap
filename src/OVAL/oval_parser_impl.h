/**
 * @file oval_parser_impl.h
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

#ifndef OVAL_PARSER_H_
#define OVAL_PARSER_H_

#include <libxml/xmlreader.h>
#include "api/oval_agent_api.h"

struct oval_parser_context;

struct oval_object_model *oval_parser_context_model(struct oval_parser_context
						    *context);

void oval_parser_parse(struct oval_object_model *, char *,
		       oval_xml_error_handler, void *);

int oval_parser_boolean(const char *, int);

int oval_parser_boolean_attribute(xmlTextReaderPtr reader, char *attname,
				  int defval);

int oval_parser_skip_tag(xmlTextReaderPtr reader,
			 struct oval_parser_context *context);

typedef void (*oval_xml_value_consumer) (char *, void *);

int oval_parser_text_value(xmlTextReaderPtr reader,
			   struct oval_parser_context *context,
			   oval_xml_value_consumer, void *);

typedef int (*oval_xml_tag_parser) (xmlTextReaderPtr,
				    struct oval_parser_context *, void *);

int oval_parser_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			  oval_xml_tag_parser, void *);

#endif				/* OVAL_PARSER_H_ */
