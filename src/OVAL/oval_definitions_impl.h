/**
 * @file oval_definitions_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
#include "public/oval_system_characteristics.h"
#if defined(OVAL_PROBES_ENABLED)
# include "public/oval_probe_session.h"
#endif
#include "public/oval_system_characteristics.h"
#include "oval_parser_impl.h"
#include "adt/oval_string_map_impl.h"
#include "../common/util.h"


oval_family_t oval_family_parse(xmlTextReaderPtr);
xmlNs *oval_family_to_namespace(oval_family_t family, const char *schema_ns, xmlDoc *doc, xmlNode *parent);
oval_subtype_t oval_subtype_parse(xmlTextReaderPtr);
oval_affected_family_t oval_affected_family_parse(xmlTextReaderPtr);
oval_operator_t oval_operator_parse(xmlTextReaderPtr, char *, oval_operator_t);
oval_operation_t oval_operation_parse(xmlTextReaderPtr, char *, oval_operation_t);
oval_definition_class_t oval_definition_class_parse(xmlTextReaderPtr);
oval_existence_t oval_existence_parse(xmlTextReaderPtr, char *, oval_existence_t);
oval_check_t oval_check_parse(xmlTextReaderPtr, char *, oval_check_t);
const char *oval_check_get_description(oval_check_t);
oval_entity_type_t oval_entity_type_parse(xmlTextReaderPtr);
oval_datatype_t oval_datatype_parse(xmlTextReaderPtr, char *, oval_datatype_t);
oval_entity_varref_type_t oval_entity_varref_type_parse(xmlTextReaderPtr);
oval_setobject_type_t oval_set_type_parse(xmlTextReaderPtr);
oval_setobject_operation_t oval_set_operation_parse(xmlTextReaderPtr, char *, oval_setobject_operation_t);
oval_filter_action_t oval_filter_action_parse(xmlTextReaderPtr, char *, oval_filter_action_t);
oval_variable_type_t oval_variable_type_parse(xmlTextReaderPtr);
oval_component_type_t oval_component_type_parse(xmlTextReaderPtr);
oval_arithmetic_operation_t oval_arithmetic_operation_parse(xmlTextReaderPtr, char *, oval_arithmetic_operation_t);
oval_datetime_format_t oval_datetime_format_parse(xmlTextReaderPtr, char *, oval_arithmetic_operation_t);
oval_message_level_t oval_message_level_parse(xmlTextReaderPtr, char *, oval_message_level_t);
void oval_variable_set_type(struct oval_variable *variable, oval_variable_type_t type);


oval_definition_class_t oval_definition_class_enum(char *);
const char *oval_definition_class_text(oval_definition_class_t);


typedef void (*oval_affected_consumer) (struct oval_affected *, void *);
int oval_affected_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, oval_affected_consumer, void *);

char *oval_test_get_state_names(struct oval_test *test);
int oval_test_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *);
xmlNode *oval_test_to_dom(struct oval_test *, xmlDoc *, xmlNode *);

typedef void (*oval_criteria_consumer) (struct oval_criteria_node *, void *);
xmlNode *oval_criteria_node_to_dom(struct oval_criteria_node *, xmlDoc *, xmlNode *);
int oval_criteria_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_criteria_consumer, void *);

typedef void (*oval_reference_consumer) (struct oval_reference *, void *);
int oval_reference_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_reference_consumer, void *);

int oval_definition_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *);
xmlNode *oval_definition_to_dom(struct oval_definition *, xmlDoc *, xmlNode *);

int oval_object_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *);
xmlNode *oval_object_to_dom(struct oval_object *, xmlDoc *, xmlNode *);
struct oval_object *oval_object_clone2(struct oval_definition_model *, struct oval_object *, char *);
struct oval_object *oval_object_create_internal(struct oval_object *, char *);
struct oval_object *oval_object_get_base_obj(struct oval_object *);

OSCAP_DEPRECATED(oval_version_t oval_state_get_schema_version(const struct oval_state *state));
oval_schema_version_t oval_state_get_platform_schema_version(const struct oval_state *state);
int oval_state_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *);
xmlNode *oval_state_to_dom(struct oval_state *, xmlDoc *, xmlNode *);

int oval_variable_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *);
xmlNode *oval_variable_to_dom(struct oval_variable *, xmlDoc *, xmlNode *);

typedef void (*oval_variable_binding_consumer) (struct oval_variable_binding *, void *);
void oval_variable_binding_to_dom(struct oval_variable_binding *, xmlDoc *, xmlNode *);
int oval_variable_binding_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_variable_binding_consumer, void *);

const char *oval_variable_type_get_text(oval_variable_type_t);

typedef void (*oval_filter_consumer) (struct oval_filter *, void *);
int oval_filter_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_filter_consumer, void *);
xmlNode *oval_filter_to_dom(struct oval_filter *, xmlDoc *, xmlNode *);

typedef void (*oval_object_content_consumer) (struct oval_object_content *, void *);
xmlNode *oval_object_content_to_dom(struct oval_object_content *, xmlDoc *, xmlNode *);
int oval_object_content_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_object_content_consumer, void *);

int oval_state_content_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oscap_consumer_func, void *);
xmlNode *oval_state_content_to_dom(struct oval_state_content *, xmlDoc *, xmlNode *);

typedef void (*oval_behavior_consumer) (struct oval_behavior *, void *);
int oval_behavior_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
			    oval_family_t, oval_behavior_consumer, void *);

int oval_entity_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oscap_consumer_func, void *);
xmlNode *oval_entity_to_dom(struct oval_entity *, xmlDoc *, xmlNode *);

int oval_record_field_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
				oscap_consumer_func, void *, oval_record_field_type_t);
xmlNode *oval_record_field_to_dom(struct oval_record_field *, bool, xmlDoc *, xmlNode *, xmlNs *);

typedef void (*oval_set_consumer) (struct oval_setobject *, void *);
int oval_set_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_set_consumer, void *);
xmlNode *oval_set_to_dom(struct oval_setobject *, xmlDoc *, xmlNode *);
void oval_set_propagate_filters(struct oval_definition_model *, struct oval_setobject *, char *);

typedef void (*oval_value_consumer) (struct oval_value *, void *);
int oval_value_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_value_consumer, void *);
xmlNode *oval_value_to_dom(struct oval_value *, xmlDoc *, xmlNode *);
int oval_value_cast(struct oval_value *value, oval_datatype_t new_dt);

oval_syschar_collection_flag_t oval_component_compute(struct oval_syschar_model *sysmod, struct oval_component *component,
						      struct oval_collection *value_collection);
#if defined(OVAL_PROBES_ENABLED)
oval_syschar_collection_flag_t oval_component_query(oval_probe_session_t *sess, struct oval_component *component,
						    struct oval_collection *value_collection);

int oval_probe_session_query_object(oval_probe_session_t *sess, struct oval_object *object);
#endif /* OVAL_PROBES_ENABLED */

typedef void (*oval_component_consumer) (struct oval_component *, void *);
int oval_component_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_component_consumer, void *);
xmlNode *oval_component_to_dom(struct oval_component *, xmlDoc *, xmlNode *);

/* message */
typedef void (*oval_message_consumer) (struct oval_message *, void *);
int oval_message_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oscap_consumer_func, void *);
void oval_message_to_dom(struct oval_message *, xmlDoc *, xmlNode *);

/* generator */
int oval_generator_parse_tag(xmlTextReader *, struct oval_parser_context *, void *user);
xmlNode *oval_generator_to_dom(struct oval_generator *, xmlDocPtr, xmlNode *);

/* definition_model */
xmlNode *oval_definition_model_to_dom(struct oval_definition_model *definition_model, xmlDocPtr doc, xmlNode * parent);
void oval_definition_model_optimize_by_filter_propagation(struct oval_definition_model *);

struct oval_definition *oval_definition_model_get_new_definition(struct oval_definition_model *, const char *);
struct oval_test       *oval_definition_model_get_new_test(struct oval_definition_model *, const char *);
struct oval_object     *oval_definition_model_get_new_object(struct oval_definition_model *, const char *);
struct oval_state      *oval_definition_model_get_new_state(struct oval_definition_model *, const char *);
struct oval_variable   *oval_definition_model_get_new_variable(struct oval_definition_model *, const char *, oval_variable_type_t type);
void oval_definition_model_add_definition(struct oval_definition_model *, struct oval_definition *);
void oval_definition_model_add_test(struct oval_definition_model *, struct oval_test *);
void oval_definition_model_add_object(struct oval_definition_model *, struct oval_object *);
void oval_definition_model_add_state(struct oval_definition_model *, struct oval_state *);
void oval_definition_model_add_variable(struct oval_definition_model *, struct oval_variable *);

const char * oval_definition_model_get_schema(struct oval_definition_model * model);
void oval_definition_model_set_schema(struct oval_definition_model *model, const char *version);
OSCAP_DEPRECATED(oval_version_t oval_definition_model_get_schema_version(struct oval_definition_model *model));
oval_schema_version_t oval_definition_model_get_core_schema_version(struct oval_definition_model *model);
oval_schema_version_t oval_definition_model_get_platform_schema_version(struct oval_definition_model *model, const char *platform);

struct oval_string_map *oval_definition_model_build_vardef_mapping(struct oval_definition_model *model);
struct oval_string_iterator *oval_definition_model_get_definitions_dependent_on_variable(struct oval_definition_model *model, struct oval_variable *variable);

/* variable model */
struct oval_collection *oval_variable_model_get_values_ref(struct oval_variable_model *, char *);
int oval_variable_bind_ext_var(struct oval_variable *, struct oval_variable_model *, char *);
bool oval_variable_contains_value(struct oval_variable *variable, const char* o_value_text);

#endif
