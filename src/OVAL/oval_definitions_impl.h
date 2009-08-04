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
#include "api/oval_definitions.h"
#include "oval_parser_impl.h"
#include "oval_agent_api_impl.h"

oval_family_enum oval_family_parse(xmlTextReaderPtr);
oval_subtype_enum oval_subtype_parse(xmlTextReaderPtr);
oval_affected_family_enum oval_affected_family_parse(xmlTextReaderPtr);
oval_operator_enum oval_operator_parse(xmlTextReaderPtr, char *,
				       oval_operator_enum);
oval_operation_enum oval_operation_parse(xmlTextReaderPtr, char *,
					 oval_operation_enum);
oval_definition_class_enum oval_definition_class_parse(xmlTextReaderPtr);
oval_existence_enum oval_existence_parse(xmlTextReaderPtr, char *,
					 oval_existence_enum);
oval_check_enum oval_check_parse(xmlTextReaderPtr, char *, oval_check_enum);
oval_entity_type_enum oval_entity_type_parse(xmlTextReaderPtr);
oval_datatype_enum oval_datatype_parse(xmlTextReaderPtr, char *,
				       oval_datatype_enum);
oval_entity_varref_type_enum oval_entity_varref_type_parse(xmlTextReaderPtr);
oval_set_type_enum oval_set_type_parse(xmlTextReaderPtr);
oval_set_operation_enum oval_set_operation_parse(xmlTextReaderPtr, char *,
						 oval_set_operation_enum);
oval_variable_type_enum oval_variable_type_parse(xmlTextReaderPtr);
oval_component_type_enum oval_component_type_parse(xmlTextReaderPtr);
oval_arithmetic_operation_enum oval_arithmetic_operation_parse(xmlTextReaderPtr,
							       char *,
							       oval_arithmetic_operation_enum);
oval_datetime_format_enum oval_datetime_format_parse(xmlTextReaderPtr, char *,
						     oval_arithmetic_operation_enum);
oval_message_level_enum oval_message_level_parse(xmlTextReaderPtr, char *,
						     oval_message_level_enum);

struct oval_affected *oval_affected_new();
void oval_affected_free(struct oval_affected *);

void set_oval_affected_family(struct oval_affected *,
			      oval_affected_family_enum);
void add_oval_affected_platform(struct oval_affected *, char *);
void add_oval_affected_product(struct oval_affected *, char *);

typedef void (*oval_affected_consumer) (struct oval_affected *, void *);
int oval_affected_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context,
			    oval_affected_consumer, void *);
void oval_affected_to_print(struct oval_affected *affected, char *indent, int idx);

struct oval_test *oval_test_new(char *);
void oval_test_free(struct oval_test *);

void set_oval_test_family(struct oval_test *, oval_family_enum);
void set_oval_test_subtype(struct oval_test *, oval_subtype_enum);
void set_oval_test_comment(struct oval_test *, char *);
void set_oval_test_deprecated(struct oval_test *, int);
void set_oval_test_version(struct oval_test *, int);
void set_oval_test_existence(struct oval_test *, oval_existence_enum);
void set_oval_test_check(struct oval_test *, oval_check_enum);
void set_oval_test_object(struct oval_test *, struct oval_object *);
void set_oval_test_state(struct oval_test *, struct oval_state *);
void add_oval_test_notes(struct oval_test *, char *);
int oval_test_parse_tag(xmlTextReaderPtr reader,
			struct oval_parser_context *context);
void oval_test_to_print(struct oval_test *test, char *indent, int index);

struct oval_criteria_node *oval_criteria_node_new(oval_criteria_node_type_enum);
void oval_criteria_node_free(struct oval_criteria_node *);

void set_oval_criteria_node_type(struct oval_criteria_node *,
				 oval_criteria_node_type_enum);
void set_oval_criteria_node_negate(struct oval_criteria_node *, int);
void set_oval_criteria_node_comment(struct oval_criteria_node *, char *);
void set_oval_criteria_node_operator(struct oval_criteria_node *, oval_operator_enum);	//type==NODETYPE_CRITERIA
void add_oval_criteria_node_subnodes(struct oval_criteria_node *, struct oval_criteria_node *);	//type==NODETYPE_CRITERIA
void set_oval_criteria_node_test(struct oval_criteria_node *, struct oval_test *);	//type==NODETYPE_CRITERION
void set_oval_criteria_node_definition(struct oval_criteria_node *, struct oval_definition *);	//type==NODETYPE_EXTENDDEF
void oval_criteria_node_to_print(struct oval_criteria_node *node, char *indent, int index);

typedef void (*oval_criteria_consumer) (struct oval_criteria_node *, void *);
int oval_criteria_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			    oval_criteria_consumer, void *);

struct oval_reference *oval_reference_new();
void oval_reference_free(struct oval_reference *);

void set_oval_reference_source(struct oval_reference *, char *);
void set_oval_reference_id(struct oval_reference *, char *);
void set_oval_reference_url(struct oval_reference *, char *);

typedef void (*oval_reference_consumer) (struct oval_reference *, void *);
int oval_reference_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			     oval_reference_consumer, void *);
void oval_reference_to_print(struct oval_reference *ref, char *indent, int index);

struct oval_definition *oval_definition_new(char *id);
void oval_definition_free(struct oval_definition *);

void set_oval_definition_id(struct oval_definition *, char *);
void set_oval_definition_version(struct oval_definition *, int);
void set_oval_definition_class(struct oval_definition *,
			       oval_definition_class_enum);
void set_oval_definition_deprecated(struct oval_definition *, int);
void set_oval_definition_title(struct oval_definition *, char *);
void set_oval_definition_description(struct oval_definition *, char *);
void set_oval_definition_criteria(struct oval_definition *,
				  struct oval_criteria_node *);
void add_oval_definition_affected(struct oval_definition *,
				  struct oval_affected *);
void add_oval_definition_reference(struct oval_definition *,
				   struct oval_reference *);
int oval_definition_parse_tag(xmlTextReaderPtr reader,
			      struct oval_parser_context *context);
void oval_definition_to_print(struct oval_definition *definition, char *indent, int idx);

struct oval_object *oval_object_new(char *id);
void oval_object_free(struct oval_object *);

void set_oval_object_family(struct oval_object *, oval_family_enum);
void set_oval_object_subtype(struct oval_object *, oval_subtype_enum);
void set_oval_object_name(struct oval_object *, char *);
void add_oval_object_notes(struct oval_object *, char *);
void set_oval_object_comment(struct oval_object *, char *);
void set_oval_object_deprecated(struct oval_object *, int);
void set_oval_object_version(struct oval_object *, int);
void add_oval_object_object_content(struct oval_object *,
				    struct oval_object_content *);
void add_oval_object_behaviors(struct oval_object *, struct oval_behavior *);
int oval_object_parse_tag(xmlTextReaderPtr reader,
			  struct oval_parser_context *context);
void oval_object_to_print(struct oval_object *object, char *indent, int idx);

struct oval_state *oval_state_new(char *);
void oval_state_free(struct oval_state *);

void set_oval_state_family(struct oval_state *, oval_family_enum);
void set_oval_state_subtype(struct oval_state *, oval_subtype_enum);
void set_oval_state_name(struct oval_state *, char *);
void add_oval_state_notes(struct oval_state *, char *);
void set_oval_state_comment(struct oval_state *, char *);
void set_oval_state_deprecated(struct oval_state *, int);
void set_oval_state_version(struct oval_state *, int);
int oval_state_parse_tag(xmlTextReaderPtr reader,
			 struct oval_parser_context *context);
void oval_state_to_print(struct oval_state *, char *indent, int index);

struct oval_variable *oval_variable_new(char *);
void oval_variable_free(struct oval_variable *);

void set_oval_variable_type(struct oval_variable *, oval_variable_type_enum);
void set_oval_variable_datatype(struct oval_variable *, oval_datatype_enum);
void add_oval_variable_values(struct oval_variable *, struct oval_value *);	//type==OVAL_VARIABLE_CONSTANT
void set_oval_variable_component(struct oval_variable *, struct oval_component *);	//type==OVAL_VARIABLE_LOCAL
int oval_variable_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context);
void oval_variable_to_print(struct oval_variable *variable, char *indent, int idx);

void set_oval_variable_binding_variable(struct oval_variable_binding *, struct oval_variable *);
void set_oval_variable_binding_value   (struct oval_variable_binding *, char *);

struct oval_variable_binding *oval_variable_binding_new();
void oval_variable_binding_free(struct oval_variable_binding *);

void set_oval_variable_binding_variable(struct oval_variable_binding *,
					struct oval_variable *);
void set_oval_variable_binding_value(struct oval_variable_binding *, char *);

struct oval_object_content
    *oval_object_content_new(oval_object_content_type_enum type);
void oval_object_content_free(struct oval_object_content *);
typedef void (*oval_variable_binding_consumer) (struct oval_variable_binding *,
					      void *);
int oval_variable_binding_parse_tag(xmlTextReaderPtr,
			       struct oval_parser_context *, oval_variable_binding_consumer, void *);

void set_oval_object_content_type(struct oval_object_content *, oval_object_content_type_enum);

void set_oval_object_content_field_name(struct oval_object_content *, char *);
void set_oval_object_content_entity(struct oval_object_content *, struct oval_entity *);	//type == OVAL_OBJECTCONTENT_ENTITY
void set_oval_object_content_varCheck(struct oval_object_content *, oval_check_enum);	//type == OVAL_OBJECTCONTENT_ENTITY
void set_oval_object_content_set(struct oval_object_content *, struct oval_set *);	//type == OVAL_OBJECTCONTENT_SET
typedef void (*oval_object_content_consumer) (struct oval_object_content *,
					      void *);
int oval_object_content_parse_tag(xmlTextReaderPtr,
				  struct oval_parser_context *,
				  oval_object_content_consumer, void *);
void oval_object_content_to_print(struct oval_object_content *, char *indent, int index);

struct oval_behavior *oval_behavior_new();
void oval_behavior_free(struct oval_behavior *);

void set_oval_behavior_value(struct oval_behavior *, struct oval_value *);
void set_oval_behavior_value_for_key(struct oval_behavior *,
				     struct oval_value *, char *attribute_key);
typedef void (*oval_behavior_consumer) (struct oval_behavior *, void *);
int oval_behavior_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			    oval_family_enum, oval_behavior_consumer, void *);
void oval_behavior_to_print(struct oval_behavior *, char *indent, int index);

struct oval_entity *oval_entity_new();
void oval_entity_free(struct oval_entity *);

void set_oval_entity_type(struct oval_entity *, oval_entity_type_enum);
void set_oval_entity_datatype(struct oval_entity *, oval_datatype_enum);
void set_oval_entity_operator(struct oval_entity *, oval_operator_enum);
void set_oval_entity_mask(struct oval_entity *, int);
void set_oval_entity_varref_type(struct oval_entity *,
				 oval_entity_varref_type_enum);
void set_oval_entity_variable(struct oval_entity *, struct oval_variable *);
void set_oval_entity_value(struct oval_entity *, struct oval_value *);

typedef void (*oval_entity_consumer) (struct oval_entity *, void *);
int oval_entity_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			  oval_entity_consumer, void *);
void oval_entity_to_print(struct oval_entity *, char *indent, int index);

struct oval_set *oval_set_new();
void oval_set_free(struct oval_set *);

void set_oval_set_type(struct oval_set *, oval_set_type_enum);
void set_oval_set_operation(struct oval_set *, oval_set_operation_enum);
void add_oval_set_subsets(struct oval_set *, struct oval_set *);	//type==OVAL_SET_AGGREGATE;
void add_oval_set_objects(struct oval_set *, struct oval_object *);	//type==OVAL_SET_COLLECTIVE;
void add_oval_set_filters(struct oval_set *, struct oval_state *);	//type==OVAL_SET_COLLECTIVE;
typedef void (*oval_set_consumer) (struct oval_set *, void *);
int oval_set_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
		       oval_set_consumer, void *);
void oval_set_to_print(struct oval_set *, char *indent, int index);

struct oval_value *oval_value_new();
void oval_value_free(struct oval_value *);

void set_oval_value_datatype(struct oval_value *, oval_datatype_enum);
void set_oval_value_text(struct oval_value *, char *);
void set_oval_value_binary(struct oval_value *, unsigned char *);	//datatype==OVAL_DATATYPE_BINARY
void set_oval_value_boolean(struct oval_value *, int);	//datatype==OVAL_DATATYPE_BOOLEAN
void set_oval_value_float(struct oval_value *, float);	//datatype==OVAL_DATATYPE_FLOAT
void set_oval_value_integer(struct oval_value *, long);	//datatype==OVAL_DATATYPE_INTEGER

typedef void (*oval_value_consumer) (struct oval_value *, void *);
int oval_value_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			 oval_value_consumer, void *);
void oval_value_to_print(struct oval_value *value, char *indent, int idx);

struct oval_component *oval_component_new(oval_component_type_enum);
void oval_component_free(struct oval_component *);

void set_oval_component_type(struct oval_component *, oval_component_type_enum);
void set_oval_component_literal_value(struct oval_component *, struct oval_value *);	//type==OVAL_COMPONENT_LITERAL
void set_oval_component_object(struct oval_component *, struct oval_object *);	//type==OVAL_COMPONENT_OBJECTREF
void set_oval_component_object_field(struct oval_component *, char *);	//type==OVAL_COMPONENT_OBJECTREF
void set_oval_component_variable(struct oval_component *, struct oval_variable *);	//type==OVAL_COMPONENT_VARREF
void add_oval_component_function_components(struct oval_component *, struct oval_component *);	//type==OVAL_COMPONENT_FUNCTION
typedef void (*oval_component_consumer) (struct oval_component *, void *);
int oval_component_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			     oval_component_consumer, void *);
void oval_component_to_print(struct oval_component *, char *indent, int index);


struct oval_message *oval_message_new();
void oval_message_free(struct oval_message *);

void set_oval_message_text(struct oval_message *, char *);
void set_oval_message_level(struct oval_message *, oval_message_level_enum);
typedef void (*oval_message_consumer)(struct oval_message *, void *);
int oval_message_parse_tag(xmlTextReaderPtr,
			       struct oval_parser_context *, oval_message_consumer, void *);
void oval_message_to_print(struct oval_message *message, char *indent,
			      int index);
#endif
