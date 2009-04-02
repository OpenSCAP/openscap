/*
 * oval_parser.h
 *
 *  Created on: Mar 9, 2009
 *      Author: Compaq_Administrator
 */

#ifndef OVAL_PARSER_H_
#define OVAL_PARSER_H_

#include <liboval/oval_agent_api.h>
#include <libxml/xmlreader.h>

struct oval_parser_context_s;

struct oval_object_model_s *oval_parser_context_model(struct oval_parser_context_s *context);

void oval_parser_parse(struct oval_object_model_s*, char*, oval_xml_error_handler, void*);

int oval_parser_boolean(const char*,int);

int oval_parser_boolean_attribute(xmlTextReaderPtr reader,char* attname,int defval);

int oval_parser_skip_tag(xmlTextReaderPtr reader, struct oval_parser_context_s *context);

typedef void (*oval_xml_value_consumer)(char*, void*);

int oval_parser_text_value(xmlTextReaderPtr reader, struct oval_parser_context_s *context, oval_xml_value_consumer, void*);

typedef int (*oval_xml_tag_parser)    (xmlTextReaderPtr, struct oval_parser_context_s*, void*);

int oval_parser_parse_tag(xmlTextReaderPtr, struct oval_parser_context_s*, oval_xml_tag_parser, void*);


#endif /* OVAL_PARSER_H_ */
