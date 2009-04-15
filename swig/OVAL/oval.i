/*
 * Open Vulnerability and Assessment Language
 * (http://oval.mitre.org/)
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
 *      Maros Barabas   <mbarabas@redhat.com>
 *
 */

%module cpe
%{
 #include "../../src/OVAL/oval_agent_api_impl.h"
 #include "../../src/OVAL/oval_collection_impl.h"
 #include "../../src/OVAL/oval_definitions_impl.h"
 #include "../../src/OVAL/oval_parser_impl.h"
 #include "../../src/OVAL/oval_results_impl.h"
 #include "../../src/OVAL/oval_string_map_impl.h"
 #include "../../src/OVAL/oval_system_characteristics_impl.h"

 #include "../../src/OVAL/api/oval_agent_api.h"
 #include "../../src/OVAL/api/oval_definitions.h"
 #include "../../src/OVAL/api/oval_results.h"
 #include "../../src/OVAL/api/oval_system_characteristics.h"
 
%}

/* oval_agent_api_impl.h */
void add_oval_definition(struct oval_object_model *, struct oval_definition *);
void add_oval_test(struct oval_object_model *, struct oval_test *);
void add_oval_object(struct oval_object_model *, struct oval_object *);
void add_oval_state(struct oval_object_model *, struct oval_state *);
void add_oval_variable(struct oval_object_model *, struct oval_variable *);

struct oval_definition *get_oval_definition_new(struct oval_object_model *, char *);
struct oval_test *get_oval_test_new(struct oval_object_model *, char *);
struct oval_object *get_oval_object_new(struct oval_object_model *, char *);
struct oval_state *get_oval_state_new(struct oval_object_model *, char *);
struct oval_variable *get_oval_variable_new(struct oval_object_model *, char *);

/* oval_collection_impl.h */
typedef void (*oval_item_free_func) (void *);

struct oval_collection *oval_collection_new();
void oval_collection_free(struct oval_collection *);
void oval_collection_free_items(struct oval_collection *, oval_item_free_func);
void oval_collection_add(struct oval_collection *, void *);
struct oval_iterator *oval_collection_iterator(struct oval_collection *);
struct oval_iterator *oval_collection_iterator_new();
void oval_collection_iterator_add(struct oval_iterator *, void *);
int oval_collection_iterator_has_more(struct oval_iterator *);
void *oval_collection_iterator_next(struct oval_iterator *);

/* oval_definitions_impl.h */
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

struct oval_test *oval_test_new();
void oval_test_free(struct oval_test *);

void set_oval_test_family(struct oval_test *, oval_family_enum);
void set_oval_test_subtype(struct oval_test *, oval_subtype_enum);
void set_oval_test_comment(struct oval_test *, char *);
void set_oval_test_id(struct oval_test *, char *);
void set_oval_test_deprecated(struct oval_test *, int);
void set_oval_test_version(struct oval_test *, int);
void set_oval_test_existence(struct oval_test *, oval_existence_enum);
void set_oval_test_check(struct oval_test *, oval_check_enum);
void set_oval_test_object(struct oval_test *, struct oval_object *);
void set_oval_test_state(struct oval_test *, struct oval_state *);
void add_oval_test_notes(struct oval_test *, char *);
int oval_test_parse_tag(xmlTextReaderPtr reader,
                        struct oval_parser_context *context);

struct oval_criteria_node *oval_criteria_node_new(oval_criteria_node_type_enum);
void oval_criteria_node_free(struct oval_criteria_node *);

void set_oval_criteria_node_type(struct oval_criteria_node *,
                                 oval_criteria_node_type_enum);
void set_oval_criteria_node_negate(struct oval_criteria_node *, int);
void set_oval_criteria_node_comment(struct oval_criteria_node *, char *);
void set_oval_criteria_node_operator(struct oval_criteria_node *, oval_operator_enum);  //type==NODETYPE_CRITERIA
void add_oval_criteria_node_subnodes(struct oval_criteria_node *, struct oval_criteria_node *); //type==NODETYPE_CRITERIA
void set_oval_criteria_node_test(struct oval_criteria_node *, struct oval_test *);      //type==NODETYPE_CRITERION
void set_oval_criteria_node_definition(struct oval_criteria_node *, struct oval_definition *);  //type==NODETYPE_EXTENDDEF

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

struct oval_definition *oval_definition_new();
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

struct oval_object *oval_object_new();
void oval_object_free(struct oval_object *);

void set_oval_object_family(struct oval_object *, oval_family_enum);
void set_oval_object_subtype(struct oval_object *, oval_subtype_enum);
void set_oval_object_name(struct oval_object *, char *);
void add_oval_object_notes(struct oval_object *, char *);
void set_oval_object_comment(struct oval_object *, char *);
void set_oval_object_id(struct oval_object *, char *);
void set_oval_object_deprecated(struct oval_object *, int);
void set_oval_object_version(struct oval_object *, int);
void add_oval_object_object_content(struct oval_object *,
                                    struct oval_object_content *);
void add_oval_object_behaviors(struct oval_object *, struct oval_behavior *);
int oval_object_parse_tag(xmlTextReaderPtr reader,
                          struct oval_parser_context *context);

struct oval_state *oval_state_new();
void oval_state_free(struct oval_state *);

void set_oval_state_family(struct oval_state *, oval_family_enum);
void set_oval_state_subtype(struct oval_state *, oval_subtype_enum);
void set_oval_state_name(struct oval_state *, char *);
void add_oval_state_notes(struct oval_state *, char *);
void set_oval_state_comment(struct oval_state *, char *);
void set_oval_state_id(struct oval_state *, char *);
void set_oval_state_deprecated(struct oval_state *, int);
void set_oval_state_version(struct oval_state *, int);
int oval_state_parse_tag(xmlTextReaderPtr reader,
                         struct oval_parser_context *context);

struct oval_variable *oval_variable_new();
void oval_variable_free(struct oval_variable *);

void set_oval_variable_id(struct oval_variable *, char *);
void set_oval_variable_type(struct oval_variable *, oval_variable_type_enum);
void set_oval_variable_datatype(struct oval_variable *, oval_datatype_enum);
void add_oval_variable_values(struct oval_variable *, struct oval_value *);     //type==OVAL_VARIABLE_CONSTANT
void set_oval_variable_component(struct oval_variable *, struct oval_component *);      //type==OVAL_VARIABLE_LOCAL
int oval_variable_parse_tag(xmlTextReaderPtr reader,
                            struct oval_parser_context *context);

struct oval_variable_binding *oval_variable_binding_new();
void oval_variable_binding_free(struct oval_variable_binding *);

void set_oval_variable_binding_variable(struct oval_variable_binding *,
                                        struct oval_variable *);
void set_oval_variable_binding_value(struct oval_variable_binding *, char *);

struct oval_object_content
    *oval_object_content_new(oval_object_content_type_enum type);
void oval_object_content_free(struct oval_object_content *);

void set_oval_object_content_field_name(struct oval_object_content *, char *);
void set_oval_object_content_entity(struct oval_object_content *, struct oval_entity *);        //type == OVAL_OBJECTCONTENT_ENTITY
void set_oval_object_content_varCheck(struct oval_object_content *, oval_check_enum);   //type == OVAL_OBJECTCONTENT_ENTITY
void set_oval_object_content_set(struct oval_object_content *, struct oval_set *);      //type == OVAL_OBJECTCONTENT_SET
typedef void (*oval_object_content_consumer) (struct oval_object_content *,
                                              void *);
int oval_object_content_parse_tag(xmlTextReaderPtr,
                                  struct oval_parser_context *,
                                  oval_object_content_consumer, void *);

struct oval_behavior *oval_behavior_new();
void oval_behavior_free(struct oval_behavior *);

void set_oval_behavior_value(struct oval_behavior *, struct oval_value *);
void set_oval_behavior_value_for_key(struct oval_behavior *,
                                     struct oval_value *, char *attribute_key);
typedef void (*oval_behavior_consumer) (struct oval_behavior *, void *);
int oval_behavior_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
                            oval_family_enum, oval_behavior_consumer, void *);

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

struct oval_set *oval_set_new();
void oval_set_free(struct oval_set *);

void set_oval_set_type(struct oval_set *, oval_set_type_enum);
void set_oval_set_operation(struct oval_set *, oval_set_operation_enum);
void add_oval_set_subsets(struct oval_set *, struct oval_set *);        //type==OVAL_SET_AGGREGATE;
void add_oval_set_objects(struct oval_set *, struct oval_object *);     //type==OVAL_SET_COLLECTIVE;
void add_oval_set_filters(struct oval_set *, struct oval_state *);      //type==OVAL_SET_COLLECTIVE;
typedef void (*oval_set_consumer) (struct oval_set *, void *);
int oval_set_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
                       oval_set_consumer, void *);

struct oval_value *oval_value_new();
void oval_value_free(struct oval_value *);
void set_oval_value_datatype(struct oval_value *, oval_datatype_enum);
void set_oval_value_text(struct oval_value *, char *);
void set_oval_value_binary(struct oval_value *, unsigned char *);       //datatype==OVAL_DATATYPE_BINARY
void set_oval_value_boolean(struct oval_value *, int);  //datatype==OVAL_DATATYPE_BOOLEAN
void set_oval_value_float(struct oval_value *, float);  //datatype==OVAL_DATATYPE_FLOAT
void set_oval_value_integer(struct oval_value *, long); //datatype==OVAL_DATATYPE_INTEGER

typedef void (*oval_value_consumer) (struct oval_value *, void *);
int oval_value_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
                         oval_value_consumer, void *);

struct oval_component *oval_component_new(oval_component_type_enum);
void oval_component_free(struct oval_component *);

void set_oval_component_type(struct oval_component *, oval_component_type_enum);
void set_oval_component_literal_value(struct oval_component *, struct oval_value *);    //type==OVAL_COMPONENT_LITERAL
void set_oval_component_object(struct oval_component *, struct oval_object *);  //type==OVAL_COMPONENT_OBJECTREF
void set_oval_component_object_field(struct oval_component *, char *);  //type==OVAL_COMPONENT_OBJECTREF
void set_oval_component_variable(struct oval_component *, struct oval_variable *);      //type==OVAL_COMPONENT_VARREF
void add_oval_component_function_components(struct oval_component *, struct oval_component *);  //type==OVAL_COMPONENT_FUNCTION
typedef void (*oval_component_consumer) (struct oval_component *, void *);
int oval_component_parse_tag(xmlTextReaderPtr, struct oval_parser_context *,
                             oval_component_consumer, void *);

/* oval_parser_impl.h */
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

/* oval_results_impl.h */
void set_oval_result_directives_definition_true(struct oval_result_directives *,
                                                int);
void set_oval_result_directives_definition_false(struct oval_result_directives
                                                 *, int);
void set_oval_result_directives_definition_unknown(struct oval_result_directives
                                                   *, int);
void set_oval_result_directives_definition_error(struct oval_result_directives
                                                 *, int);
void set_oval_result_directives_definition_not_evaluated(struct
                                                         oval_result_directives
                                                         *, int);
void set_oval_result_directives_definition_not_applicable(struct
                                                          oval_result_directives
                                                          *, int);

struct oval_result_item *oval_result_item_new();
void oval_result_item_free(struct oval_result_item *);

void set_oval_result_item_sysdata(struct oval_result_item *,
                                  struct oval_sysdata *);
void set_oval_result_item_result(struct oval_result_item *, oval_result_enum);
void set_oval_result_item_message(struct oval_result_item *, char *);

struct oval_result_test *oval_result_test_new();
void oval_result_test_free(struct oval_result_test *);

void set_oval_result_test_test(struct oval_result_test *, struct oval_test *);
void set_oval_result_test_check(struct oval_result_test *, oval_check_enum);
void set_oval_result_test_result(struct oval_result_test *, oval_result_enum);
void set_oval_result_test_message(struct oval_result_test *, char *);
void add_oval_result_test_items(struct oval_result_test *,
                                struct oval_result_item *);

struct oval_result_criteria_node *oval_result_criteria_node_new();
void oval_result_criteria_node_free(struct oval_result_criteria_node *);

void set_oval_result_criteria_node_type(struct oval_result_criteria_node *,
                                        oval_criteria_node_type_enum);
void set_oval_result_criteria_node_result(struct oval_result_criteria_node *,
                                          oval_result_enum);
void set_oval_result_criteria_node_operator(struct oval_result_criteria_node *, oval_operator_enum);    //type==NODETYPE_CRITERIA
void add_oval_result_criteria_node_subnode(struct oval_result_criteria_node *, struct oval_criteria_node *);    //type==NODETYPE_CRITERIA
void set_oval_result_criteria_node_test(struct oval_result_criteria_node *, struct oval_result_test *); //type==NODETYPE_CRITERION
void set_oval_result_criteria_node_extends(struct oval_result_criteria_node *, struct oval_result *);   //type==NODETYPE_EXTENDDEF

struct oval_result *oval_result_new();
void oval_result_free(struct oval_result *);

void set_oval_result_definition(struct oval_result *, struct oval_definition *);
void set_oval_result_result(struct oval_result *, oval_result_enum);
void set_oval_result_message(struct oval_result *, char *);
void set_oval_result_directives(struct oval_result *,
                                struct oval_result_directives *);
void set_oval_result_criteria(struct oval_result *,
                              struct oval_result_criteria_node *);

/* oval_string_map_impl.h */

struct oval_string_map *oval_string_map_new();
void oval_string_map_put(struct oval_string_map *, char *, void *);
struct oval_iterator *oval_string_map_keys(struct oval_string_map *);
struct oval_iterator *oval_string_map_values(struct oval_string_map *);
void *oval_string_map_get_value(struct oval_string_map *, char *);
void oval_string_map_free(struct oval_string_map *, oval_item_free_func);

/* oval_system_characteristic_impl.h */
struct oval_sysint *oval_sysint_new();
void oval_sysint_free(struct oval_sysint *);

void set_oval_sysint_name(struct oval_sysint *, char *);
void set_oval_sysint_ip_address(struct oval_sysint *, char *);
void set_oval_sysint_mac_address(struct oval_sysint *, char *);

struct oval_sysinfo *oval_sysinfo_new();
void oval_sysinfo_free(struct oval_sysinfo *);

void set_oval_sysinfo_os_name(struct oval_sysinfo *, char *);
void set_oval_sysinfo_os_version(struct oval_sysinfo *, char *);
void set_oval_sysinfo_os_architecture(struct oval_sysinfo *, char *);
void set_oval_sysinfo_primary_host_name(struct oval_sysinfo *, char *);
void add_oval_sysinfo_interface(struct oval_sysinfo *, struct oval_sysint *);

struct oval_sysdata *oval_sysdata_new();
void oval_sysdata_free(struct oval_sysdata *);

void set_oval_sysdata_family(struct oval_sysdata *, oval_family_enum);
void set_oval_sysdata_subtype(struct oval_sysdata *, oval_subtype_enum);

struct oval_syschar *oval_syschar_new();
void oval_syschar_free(struct oval_syschar *);

void set_oval_syschar_flag(struct oval_syschar *,
                           oval_syschar_collection_flag_enum);
void add_oval_syschar_messages(struct oval_syschar *, char *);
void set_oval_syschar_sysinfo(struct oval_syschar *, struct oval_sysinfo *);
void set_oval_syschar_object(struct oval_syschar *, struct oval_object *);
void add_oval_syschar_variable_binding(struct oval_syschar *,
                                       struct oval_variable_binding *);
void add_oval_syschar_sysdata(struct oval_syschar *, struct oval_sysdata *);

