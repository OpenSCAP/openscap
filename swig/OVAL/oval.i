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

%module oval
%{
 #include "../../src/OVAL/api/oval_agent_api.h"
 #include "../../src/OVAL/api/oval_definitions.h"
 #include "../../src/OVAL/api/oval_system_characteristics.h"
 #include "../../src/OVAL/api/oval_results.h"
%}

/* oval_agent_api.h */
struct import_source *import_source_file(char *filename);
void import_source_free(struct import_source *);

struct oval_object_model;

struct oval_object_model *oval_object_model_new();
typedef int (*oval_xml_error_handler) (struct oval_xml_error *, void *user_arg);

void load_oval_definitions(struct oval_object_model *, struct import_source *,
                           oval_xml_error_handler, void *);

struct oval_definition *get_oval_definition(struct oval_object_model *,
                                            char *def_urn);
struct oval_test *get_oval_test(struct oval_object_model *, char *tst_urn);
struct oval_object *get_oval_object(struct oval_object_model *, char *obj_urn);
struct oval_state *get_oval_state(struct oval_object_model *, char *ste_urn);
struct oval_variable *get_oval_variable(struct oval_object_model *,
                                        char *var_urn);

struct oval_iterator_definition *get_oval_definitions(struct oval_object_model
                                                      *);
struct oval_iterator_test *get_oval_tests(struct oval_object_model *);
struct oval_iterator_object *get_oval_objects(struct oval_object_model *);
struct oval_iterator_state *get_oval_states(struct oval_object_model *);
struct oval_iterator_variable *get_oval_variables(struct oval_object_model *);
/*int export_characteristics(struct oval_iterator_syschar *,
                           struct export_target *);*/

//int export_results(struct oval_result *, struct export_target *);

/*struct oval_iterator_syschar *probe_object(struct oval_object *,
                                           struct oval_iterator_variable_binding
                                           *);*/
/*struct oval_result_test *resolve_test(struct oval_test *,
                                      struct oval_iterator_syschar *,
                                      struct oval_iterator_variable_binding *);*/
/*struct oval_result *resolve_definition(struct oval_definition *,
                                       struct oval_iterator_syschar *,
                                       struct oval_iterator_variable_binding *);*/

/* oval_definitions.h */
int oval_iterator_string_has_more(struct oval_iterator_string *);
char *oval_iterator_string_next(struct oval_iterator_string *);

int oval_iterator_affected_has_more(struct oval_iterator_affected *);
struct oval_affected *oval_iterator_affected_next(struct oval_iterator_affected
                                                  *);

int oval_iterator_test_has_more(struct oval_iterator_test *);
struct oval_test *oval_iterator_test_next(struct oval_iterator_test *);

oval_affected_family_enum oval_affected_family(struct oval_affected *);
struct oval_iterator_string *oval_affected_platform(struct oval_affected *);
struct oval_iterator_string *oval_affected_product(struct oval_affected *);

int oval_iterator_criteria_node_has_more(struct oval_iterator_criteria_node *);
struct oval_criteria_node *oval_iterator_criteria_node_next(struct
                                                            oval_iterator_criteria_node
                                                            *);

oval_criteria_node_type_enum oval_criteria_node_type(struct oval_criteria_node
                                                     *);
int oval_criteria_node_negate(struct oval_criteria_node *);
char *oval_criteria_node_comment(struct oval_criteria_node *);
oval_operator_enum oval_criteria_node_operator(struct oval_criteria_node *);
struct oval_iterator_criteria_node *oval_criteria_node_subnodes(struct oval_criteria_node *);
struct oval_test *oval_criteria_node_test(struct oval_criteria_node *);
struct oval_definition *oval_criteria_node_definition(struct oval_criteria_node *);

int oval_iterator_reference_has_more(struct oval_iterator_reference *);
struct oval_reference *oval_iterator_reference_next(struct
                                                    oval_iterator_reference *);

char *oval_reference_source(struct oval_reference *);
char *oval_reference_id(struct oval_reference *);
char *oval_reference_url(struct oval_reference *);

int oval_iterator_definition_has_more(struct oval_iterator_definition *);
struct oval_definition *oval_iterator_definition_next(struct
                                                      oval_iterator_definition
                                                      *);

char *oval_definition_id(struct oval_definition *);
int oval_definition_version(struct oval_definition *);
oval_definition_class_enum oval_definition_class(struct oval_definition *);
int oval_definition_deprecated(struct oval_definition *);
char *oval_definition_title(struct oval_definition *);
char *oval_definition_description(struct oval_definition *);
struct oval_iterator_affected *oval_definition_affected(struct oval_definition
                                                        *);
struct oval_iterator_reference *oval_definition_reference(struct oval_definition
                                                          *);
struct oval_criteria_node *oval_definition_criteria(struct oval_definition *);
int oval_iterator_object_has_more(struct oval_iterator_object *);
struct oval_object *oval_iterator_object_next(struct oval_iterator_object *);

oval_family_enum oval_object_family(struct oval_object *);
oval_subtype_enum oval_object_subtype(struct oval_object *);
//char *oval_object_name(struct oval_object *);
struct oval_iterator_string *oval_object_notes(struct oval_object *);
char *oval_object_comment(struct oval_object *);
char *oval_object_id(struct oval_object *);
int oval_object_deprecated(struct oval_object *);
int oval_object_version(struct oval_object *);
struct oval_iterator_object_content *oval_object_object_content(struct
                                                                oval_object *);
struct oval_iterator_behavior *oval_object_behaviors(struct oval_object *);

int oval_iterator_test_has_more(struct oval_iterator_test *);
struct oval_test *oval_iterator_test_next(struct oval_iterator_test *);

oval_family_enum oval_test_family(struct oval_test *);
oval_subtype_enum oval_test_subtype(struct oval_test *);
struct oval_iterator_string *oval_test_notes(struct oval_test *);
char *oval_test_comment(struct oval_test *);
char *oval_test_id(struct oval_test *);
int oval_test_deprecated(struct oval_test *);
int oval_test_version(struct oval_test *);
oval_existence_enum oval_test_existence(struct oval_test *);
oval_check_enum oval_test_check(struct oval_test *);
struct oval_object *oval_test_object(struct oval_test *);
struct oval_state *oval_test_state(struct oval_test *);

int oval_iterator_variable_binding_has_more(struct
                                            oval_iterator_variable_binding *);
struct oval_variable_binding *oval_iterator_variable_binding_next(struct
                                                                  oval_iterator_variable_binding
                                                                  *);

struct oval_variable *oval_variable_binding_variable(struct
                                                     oval_variable_binding *);
char *oval_variable_binding_value(struct oval_variable_binding *);

int oval_iterator_object_content_has_more(struct oval_iterator_object_content
                                          *);
struct oval_object_content *oval_iterator_object_content_next(struct
                                                              oval_iterator_object_content
                                                              *);

char *oval_object_content_field_name(struct oval_object_content *);
oval_object_content_type_enum oval_object_content_type(struct
                                                       oval_object_content *);
struct oval_entity *oval_object_content_entity(struct oval_object_content *);
oval_check_enum oval_object_content_varCheck(struct oval_object_content *); 
struct oval_set *oval_object_content_set(struct oval_object_content *); 
int oval_iterator_entity_has_more(struct oval_iterator_entity *);
struct oval_entity *oval_iterator_entity_next(struct oval_iterator_entity *);

char *oval_entity_name(struct oval_entity *);
oval_entity_type_enum oval_entity_type(struct oval_entity *);
oval_datatype_enum oval_entity_datatype(struct oval_entity *);
oval_operation_enum oval_entity_operation(struct oval_entity *);
int oval_entity_mask(struct oval_entity *);
oval_entity_varref_type_enum oval_entity_varref_type(struct oval_entity *);
struct oval_variable *oval_entity_variable(struct oval_entity *);
struct oval_value *oval_entity_value(struct oval_entity *);

int oval_iterator_set_has_more(struct oval_iterator_set *);
struct oval_set *oval_iterator_set_next(struct oval_iterator_set *);

oval_set_type_enum oval_set_type(struct oval_set *);
oval_set_operation_enum oval_set_operation(struct oval_set *);
struct oval_iterator_set *oval_set_subsets(struct oval_set *);  
struct oval_iterator_object *oval_set_objects(struct oval_set *);
struct oval_iterator_state *oval_set_filters(struct oval_set *);

int oval_iterator_behavior_has_more(struct oval_iterator_behavior *);
struct oval_behavior *oval_iterator_behavior_next(struct oval_iterator_behavior
                                                  *);

struct oval_iterator_string *oval_behavior_attribute_keys(struct oval_behavior
                                                          *);
char *oval_behavior_value_for_key(struct oval_behavior *, char *attribute_key);

int oval_iterator_value_has_more(struct oval_iterator_value *);
struct oval_value *oval_iterator_value_next(struct oval_iterator_value *);

oval_datatype_enum oval_value_datatype(struct oval_value *);
char *oval_value_text(struct oval_value *);
unsigned char *oval_value_binary(struct oval_value *);
char oval_value_boolean(struct oval_value *);
float oval_value_float(struct oval_value *);
long oval_value_integer(struct oval_value *);

int oval_iterator_state_has_more(struct oval_iterator_state *);
struct oval_state *oval_iterator_state_next(struct oval_iterator_state *);

oval_family_enum oval_state_family(struct oval_state *);
oval_subtype_enum oval_state_subtype(struct oval_state *);
char *oval_state_name(struct oval_state *);
char *oval_state_comment(struct oval_state *);
char *oval_state_id(struct oval_state *);
int oval_state_deprecated(struct oval_state *);
int oval_state_version(struct oval_state *);
struct oval_iterator_string *oval_state_notes(struct oval_state *);
struct oval_iterator_entity *oval_state_entities(struct oval_state *);

//int oval_iterator_variable_has_more(struct oval_iterator_variable *);
/*struct oval_variable *oval_iterator_variable_next(struct oval_iterator_variable
                                                  *);*/
char *oval_variable_id(struct oval_variable *);
char *oval_variable_comment(struct oval_variable *);
int oval_variable_version(struct oval_variable *);
int oval_variable_deprecated(struct oval_variable *);
oval_variable_type_enum oval_variable_type(struct oval_variable *);
oval_datatype_enum oval_variable_datatype(struct oval_variable *);
struct oval_iterator_value *oval_variable_values(struct oval_variable *);
struct oval_component *oval_variable_component(struct oval_variable *);

int oval_iterator_component_has_more(struct oval_iterator_component *);
struct oval_component *oval_iterator_component_next(struct
                                                    oval_iterator_component *);

oval_component_type_enum oval_component_type(struct oval_component *);
struct oval_value *oval_component_literal_value(struct oval_component *);
struct oval_object *oval_component_object(struct oval_component *); 
char *oval_component_object_field(struct oval_component *); 
struct oval_variable *oval_component_variable(struct oval_component *); 
struct oval_iterator_component *oval_component_function_components(struct oval_component *);
oval_arithmetic_operation_enum oval_component_arithmetic_operation(struct oval_component *);
char *oval_component_begin_character(struct oval_component *);
char *oval_component_end_character(struct oval_component *);
char *oval_component_split_delimiter(struct oval_component *);
int oval_component_substring_start(struct oval_component *);
int oval_component_substring_length(struct oval_component *);
oval_datetime_format_enum oval_component_timedif_format_1(struct oval_component *); 
oval_datetime_format_enum oval_component_timedif_format_2(struct oval_component *); 

/* oval_system_characteristics.h */
int oval_iterator_result_item_has_more(struct oval_iterator_result_item *);
struct oval_result_item *oval_iterator_result_item_next(struct
                                                        oval_iterator_result_item
                                                        *);

struct oval_sysdata *oval_result_item_sysdata(struct oval_result_item *);
oval_result_enum oval_result_item_result(struct oval_result_item *);
char *oval_result_item_message(struct oval_result_item *);

int oval_iterator_result_test_has_more(struct oval_iterator_result_test *);
struct oval_result_test *oval_iterator_result_test_next(struct
                                                        oval_iterator_result_test
                                                        *);

struct oval_test *oval_result_test_test(struct oval_result_test *);
oval_check_enum oval_result_test_check(struct oval_result_test *);
oval_result_enum oval_result_test_result(struct oval_result_test *);
char *oval_result_test_message(struct oval_result_test *);
struct oval_iterator_result_item *oval_result_test_items(struct oval_result_test
                                                         *);

int oval_iterator_result_criteria_node_has_more(struct
                                                oval_iterator_result_criteria_node
                                                *);
struct oval_result_criteria_node *oval_iterator_result_criteria_node_next(struct
                                                                          oval_iterator_result_criteria_node
                                                                          *);

oval_criteria_node_type_enum oval_result_criteria_node_type(struct
                                                            oval_result_criteria_node
                                                            *);
oval_result_enum oval_result_criteria_node_result(struct
                                                  oval_result_criteria_node *);
oval_operator_enum oval_result_criteria_node_operator(struct oval_result_criteria_node *);  
struct oval_iterator_criteria_node *oval_result_criteria_node_subnodes(struct oval_result_criteria_node *);
struct oval_result_test *oval_result_criteria_node_test(struct oval_result_criteria_node *);
struct oval_result *oval_result_criteria_node_extends(struct oval_result_criteria_node *);

int oval_result_directives_definition_true(struct oval_result_directives *);
int oval_result_directives_definition_false(struct oval_result_directives *);
int oval_result_directives_definition_unknown(struct oval_result_directives *);
int oval_result_directives_definition_error(struct oval_result_directives *);
int oval_result_directives_definition_not_evaluated(struct
                                                    oval_result_directives *);
int oval_result_directives_definition_not_applicable(struct
                                                     oval_result_directives *);

int oval_iterator_result_has_more(struct oval_iterator_result *);
struct oval_result *oval_iterator_result_next(struct oval_iterator_result *);

struct oval_definition *oval_result_definition(struct oval_result *);
oval_result_enum oval_result_result(struct oval_result *);
char *oval_result_message(struct oval_result *);
//struct oval_result_directives *oval_result_directives(struct oval_result *);
struct oval_result_criteria_node *oval_result_criteria(struct oval_result *);

/* oval_results.h */
int oval_iterator_sysint_has_more(struct oval_iterator_sysint *);
struct oval_sysint *oval_iterator_sysint_next(struct oval_iterator_sysint *);

char *oval_sysint_name(struct oval_sysint *);
//char *oval_sysint_ip_address(struct oval_sysint *);
//char *oval_sysint_mac_address(struct oval_sysint *);

int oval_iterator_sysinfo_has_more(struct oval_iterator_sysinfo *);
struct oval_sysinfo *oval_iterator_sysinfo_next(struct oval_iterator_sysinfo *);

//char *oval_sysinfo_os_name(struct oval_sysinfo *);
//char *oval_sysinfo_os_version(struct oval_sysinfo *);
//char *oval_sysinfo_os_architecture(struct oval_sysinfo *);
//char *oval_sysinfo_primary_host_name(struct oval_sysinfo *);
struct oval_iterator_sysint *oval_sysinfo_interfaces(struct oval_sysinfo *);

int oval_iterator_sysdata_has_more(struct oval_iterator_sysdata *);
struct oval_sysdata *oval_iterator_sysdata_next(struct oval_iterator_sysdata *);

oval_family_enum oval_sysdata_family(struct oval_sysdata *);
oval_subtype_enum oval_sysdata_subtype(struct oval_sysdata *);

int oval_iterator_syschar_has_more(struct oval_iterator_syschar *);
struct oval_syschar *oval_iterator_syschar_next(struct oval_iterator_syschar *);

oval_syschar_collection_flag_enum oval_syschar_flag(struct oval_syschar *);
struct oval_iterator_string *oval_syschar_messages(struct oval_syschar *);
struct oval_sysinfo *oval_syschar_sysinfo(struct oval_syschar *);
struct oval_object *oval_syschar_object(struct oval_syschar *);
struct oval_iterator_variable_binding *oval_syschar_variable_bindings(struct
                                                                      oval_syschar
                                                                      *);
struct oval_iterator_sysdata *oval_syschar_sysdata(struct oval_syschar *);

