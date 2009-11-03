/**
 * @file oval_definitions_impl.h
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


#ifndef OVAL_DEFINITIONS_IMPL
#define OVAL_DEFINITIONS_IMPL

#include <libxml/xmlreader.h>
#include "public/oval_definitions.h"
#include "oval_parser_impl.h"
#include "oval_agent_api_impl.h"
#include "../common/util.h"

OSCAP_HIDDEN_START;

oval_family_t oval_family_parse(xmlTextReaderPtr);
oval_subtype_t oval_subtype_parse(xmlTextReaderPtr);
oval_affected_family_t oval_affected_family_parse(xmlTextReaderPtr);
oval_operator_t oval_operator_parse(xmlTextReaderPtr, char *,
				       oval_operator_t);
oval_operation_t oval_operation_parse(xmlTextReaderPtr, char *,
					 oval_operation_t);
oval_definition_class_t oval_definition_class_parse(xmlTextReaderPtr);
oval_existence_t oval_existence_parse(xmlTextReaderPtr, char *,
					 oval_existence_t);
oval_check_t oval_check_parse(xmlTextReaderPtr, char *, oval_check_t);
oval_entity_type_t oval_entity_type_parse(xmlTextReaderPtr);
oval_datatype_t oval_datatype_parse(xmlTextReaderPtr, char *,
				       oval_datatype_t);
oval_entity_varref_type_t oval_entity_varref_type_parse(xmlTextReaderPtr);
oval_setobject_type_t oval_set_type_parse(xmlTextReaderPtr);
oval_setobject_operation_t oval_set_operation_parse(xmlTextReaderPtr, char *,
						 oval_setobject_operation_t);
oval_variable_type_t oval_variable_type_parse(xmlTextReaderPtr);
oval_component_type_t oval_component_type_parse(xmlTextReaderPtr);
oval_arithmetic_operation_t oval_arithmetic_operation_parse(xmlTextReaderPtr,
							       char *,
							       oval_arithmetic_operation_t);
oval_datetime_format_t oval_datetime_format_parse(xmlTextReaderPtr, char *,
						     oval_arithmetic_operation_t);
oval_message_level_t oval_message_level_parse(xmlTextReaderPtr, char *,
						     oval_message_level_t);
const char * oval_message_level_text(oval_message_level_t);






struct oval_affected *oval_affected_clone(struct oval_affected *);
typedef void (*oval_affected_consumer) (struct oval_affected *, void *);
int oval_affected_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context,
			    oval_affected_consumer, void *);
void oval_affected_to_print(struct oval_affected *affected, char *indent, int idx);

int oval_test_parse_tag(xmlTextReaderPtr reader,
			struct oval_parser_context *context);
void oval_test_to_print(struct oval_test *test, char *indent, int index);
xmlNode *oval_test_to_dom  (struct oval_test *, xmlDoc *, xmlNode *);

struct oval_criteria_node *oval_criteria_node_clone(struct oval_criteria_node *, struct oval_definition_model *);
void oval_criteria_node_to_print(struct oval_criteria_node *node, char *indent, int index);
xmlNode *oval_criteria_node_to_dom (struct oval_criteria_node *, xmlDoc *, xmlNode *);
typedef void (*oval_criteria_consumer) (struct oval_criteria_node *, void *);
int oval_criteria_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			    oval_criteria_consumer, void *);

struct oval_reference *oval_reference_clone(struct oval_reference *);
typedef void (*oval_reference_consumer) (struct oval_reference *, void *);
int oval_reference_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			     oval_reference_consumer, void *);
void oval_reference_to_print(struct oval_reference *ref, char *indent, int index);

int oval_definition_parse_tag(xmlTextReaderPtr reader,
			      struct oval_parser_context *context);
void oval_definition_to_print(struct oval_definition *definition, char *indent, int idx);
xmlNode *oval_definition_to_dom (struct oval_definition *, xmlDoc *, xmlNode *);

int oval_object_parse_tag(xmlTextReaderPtr reader,
			  struct oval_parser_context *context);
void oval_object_to_print(struct oval_object *object, char *indent, int idx);
xmlNode *oval_object_to_dom  (struct oval_object *, xmlDoc *, xmlNode *);

int oval_state_parse_tag(xmlTextReaderPtr reader,
			 struct oval_parser_context *context);
void oval_state_to_print(struct oval_state *, char *indent, int index);
xmlNode *oval_state_to_dom  (struct oval_state *, xmlDoc *, xmlNode *);

int oval_variable_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context);
void oval_variable_to_print(struct oval_variable *variable, char *indent, int idx);
xmlNode *oval_variable_to_dom  (struct oval_variable *, xmlDoc *, xmlNode *);

void oval_variable_binding_to_dom  (struct oval_variable_binding *, xmlDoc *, xmlNode *);
typedef void (*oval_variable_binding_consumer) (struct oval_variable_binding *,
					      void *);
int oval_variable_binding_parse_tag
	(xmlTextReaderPtr, struct oval_parser_context *, oval_variable_binding_consumer, void *);
struct oval_variable_binding *oval_variable_binding_clone(struct oval_variable_binding *, struct oval_definition_model *);

struct oval_object_content *oval_object_content_clone(struct oval_object_content *, struct oval_definition_model *);
typedef void (*oval_object_content_consumer) (struct oval_object_content *,
					      void *);
xmlNode *oval_object_content_to_dom
	(struct oval_object_content *, xmlDoc *, xmlNode *);
int oval_object_content_parse_tag(xmlTextReaderPtr,
				  struct oval_parser_context *,
				  oval_object_content_consumer, void *);
void oval_object_content_to_print(struct oval_object_content *, char *indent, int index);

struct oval_state_content *oval_state_content_clone(struct oval_state_content *, struct oval_definition_model *);
int oval_state_content_parse_tag
	(xmlTextReaderPtr, struct oval_parser_context *,
			oscap_consumer_func, void *);
xmlNode *oval_state_content_to_dom
	(struct oval_state_content *, xmlDoc *, xmlNode *);

struct oval_behavior *oval_behavior_clone(struct oval_behavior *);
typedef void (*oval_behavior_consumer) (struct oval_behavior *, void *);
int oval_behavior_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			    oval_family_t, oval_behavior_consumer, void *);
void oval_behavior_to_print(struct oval_behavior *, char *indent, int index);

struct oval_entity *oval_entity_clone(struct oval_entity *, struct oval_definition_model *);
int oval_entity_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			  oscap_consumer_func, void *);
void oval_entity_to_print(struct oval_entity *, char *indent, int index);
xmlNode *oval_entity_to_dom
	(struct oval_entity *, xmlDoc *, xmlNode *);

struct oval_setobject *oval_setobject_clone(struct oval_setobject *, struct oval_definition_model *);
typedef void (*oval_set_consumer) (struct oval_setobject *, void *);
int oval_set_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
		       oval_set_consumer, void *);
void oval_set_to_print(struct oval_setobject *, char *indent, int index);
xmlNode *oval_set_to_dom
	(struct oval_setobject *, xmlDoc *, xmlNode *);

struct oval_value *oval_value_clone(struct oval_value *);
typedef void (*oval_value_consumer) (struct oval_value *, void *);
int oval_value_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			 oval_value_consumer, void *);
void oval_value_to_print(struct oval_value *value, char *indent, int idx);
xmlNode *oval_value_to_dom(struct oval_value *, xmlDoc *, xmlNode *);

struct oval_component *oval_component_clone(struct oval_component *, struct oval_definition_model *);
typedef void (*oval_component_consumer) (struct oval_component *, void *);
int oval_component_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			     oval_component_consumer, void *);
void oval_component_to_print(struct oval_component *, char *indent, int index);
xmlNode *oval_component_to_dom(struct oval_component *, xmlDoc *, xmlNode *);

typedef void (*oval_message_consumer)(struct oval_message *, void *);
int oval_message_parse_tag(xmlTextReaderPtr,
			       struct oval_parser_context *, oscap_consumer_func, void *);
void oval_message_to_print(struct oval_message *message, char *indent,
			      int index);
void oval_message_to_dom  (struct oval_message *, xmlDoc *, xmlNode *);

OSCAP_HIDDEN_END

#endif
