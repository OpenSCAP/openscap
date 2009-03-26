#ifndef OVAL_DEFINITIONS_IMPL
#define OVAL_DEFINITIONS_IMPL
#include <liboval/oval_definitions.h>
#include <libxml/xmlreader.h>
#include "oval_parser_impl.h"

	oval_family_enum             oval_family_parse            (xmlTextReaderPtr);
	oval_subtype_enum            oval_subtype_parse           (xmlTextReaderPtr);
	oval_affected_family_enum    oval_affected_family_parse   (xmlTextReaderPtr);
	oval_operator_enum           oval_operator_parse          (xmlTextReaderPtr);
	oval_definition_class_enum   oval_definition_class_parse  (xmlTextReaderPtr);
	oval_existence_enum          oval_existence_parse         (xmlTextReaderPtr);
	oval_check_enum              oval_check_parse             (xmlTextReaderPtr,char*);
	oval_entity_type_enum        oval_entity_type_parse       (xmlTextReaderPtr);
	oval_datatype_enum           oval_datatype_parse          (xmlTextReaderPtr);
	oval_entity_varref_type_enum oval_entity_varref_type_parse(xmlTextReaderPtr);
	oval_set_type_enum           oval_set_type_parse          (xmlTextReaderPtr);
	oval_set_operation_enum      oval_set_operation_parse     (xmlTextReaderPtr);
	oval_variable_type_enum      oval_variable_type_parse     (xmlTextReaderPtr);
	oval_component_type_enum     oval_component_type_parse    (xmlTextReaderPtr);
	oval_function_type_enum      oval_function_type_parse     (xmlTextReaderPtr);


	struct oval_affected_s              *oval_affected_new();
	void                                 oval_affected_free(struct oval_affected_s*);

	void set_oval_affected_family  (struct oval_affected_s*, oval_affected_family_enum);
	void add_oval_affected_platform(struct oval_affected_s*, char*);
	void add_oval_affected_product (struct oval_affected_s*, char*);

	typedef void (*oval_affected_consumer)(struct oval_affected_s*, void*);
	int oval_affected_parse_tag (xmlTextReaderPtr reader, struct oval_parser_context_s *context, oval_affected_consumer, void*);

	struct oval_test_s                  *oval_test_new();
	void                                 oval_test_free(struct oval_test_s*);

	void set_oval_test_family    (struct oval_test_s*, oval_family_enum);
	void set_oval_test_subtype   (struct oval_test_s*, oval_subtype_enum);
	void set_oval_test_comment   (struct oval_test_s*, char*);
	void set_oval_test_id        (struct oval_test_s*, char*);
	void set_oval_test_deprecated(struct oval_test_s*, int);
	void set_oval_test_version   (struct oval_test_s*, int);
	void set_oval_test_existence (struct oval_test_s*, oval_existence_enum);
	void set_oval_test_check     (struct oval_test_s*, oval_check_enum);
	void set_oval_test_object    (struct oval_test_s*, struct oval_object_s*);
	void set_oval_test_state     (struct oval_test_s*, struct oval_state_s*);
	void add_oval_test_notes     (struct oval_test_s*, char*);
	int oval_test_parse_tag      (xmlTextReaderPtr reader, struct oval_parser_context_s *context);

	struct oval_criteria_node_s         *oval_criteria_node_new(oval_criteria_node_type_enum);
	void                                 oval_criteria_node_free(struct oval_criteria_node_s*);

	void set_oval_criteria_node_type      (struct oval_criteria_node_s*, oval_criteria_node_type_enum);
	void set_oval_criteria_node_negate    (struct oval_criteria_node_s*, int);
	void set_oval_criteria_node_comment   (struct oval_criteria_node_s*, char*);
	void set_oval_criteria_node_operator  (struct oval_criteria_node_s*, oval_operator_enum);    //type==NODETYPE_CRITERIA
	void add_oval_criteria_node_subnodes  (struct oval_criteria_node_s*, struct oval_criteria_node_s*);//type==NODETYPE_CRITERIA
	void set_oval_criteria_node_test      (struct oval_criteria_node_s*, struct oval_test_s*);         //type==NODETYPE_CRITERION
	void set_oval_criteria_node_definition(struct oval_criteria_node_s*, struct oval_definition_s*);   //type==NODETYPE_EXTENDDEF

	typedef void (*oval_criteria_consumer)(struct oval_criteria_node_s*, void*);
	int oval_criteria_parse_tag (xmlTextReaderPtr, struct oval_parser_context_s*, oval_criteria_consumer, void*);

	struct oval_reference_s             *oval_reference_new();
	void                                 oval_reference_free(struct oval_reference_s*);

	void set_oval_reference_source(struct oval_reference_s*, char*);
	void set_oval_reference_id    (struct oval_reference_s*, char*);
	void set_oval_reference_url   (struct oval_reference_s*, char*);

	struct oval_definition_s            *oval_definition_new();
	void                                 oval_definition_free(struct oval_definition_s*);

	void set_oval_definition_id         (struct oval_definition_s*, char*);
	void set_oval_definition_version    (struct oval_definition_s*, int);
	void set_oval_definition_class      (struct oval_definition_s*, oval_definition_class_enum);
	void set_oval_definition_deprecated (struct oval_definition_s*, int);
	void set_oval_definition_title      (struct oval_definition_s*, char*);
	void set_oval_definition_description(struct oval_definition_s*, char*);
	void set_oval_definition_criteria   (struct oval_definition_s*, struct oval_criteria_node_s*);
	void add_oval_definition_affected   (struct oval_definition_s*, struct oval_affected_s*);
	void add_oval_definition_reference  (struct oval_definition_s*, struct oval_reference_s*);
	int oval_definition_parse_tag       (xmlTextReaderPtr reader, struct oval_parser_context_s *context);

	struct oval_object_s                *oval_object_new();
	void                                 oval_object_free(struct oval_object_s*);

	void set_oval_object_family        (struct oval_object_s*, oval_family_enum);
	void set_oval_object_subtype       (struct oval_object_s*, oval_subtype_enum);
	void set_oval_object_name          (struct oval_object_s*, char*);
	void add_oval_object_notes         (struct oval_object_s*, char*);
	void set_oval_object_comment       (struct oval_object_s*, char*);
	void set_oval_object_id            (struct oval_object_s*, char*);
	void set_oval_object_deprecated    (struct oval_object_s*, int);
	void set_oval_object_version       (struct oval_object_s*, int);
	void add_oval_object_object_content(struct oval_object_s*, struct oval_object_content_s*);
	void add_oval_object_behaviors     (struct oval_object_s*, struct oval_behavior_s*);
	int oval_object_parse_tag          (xmlTextReaderPtr reader, struct oval_parser_context_s *context);

	struct oval_state_s                 *oval_state_new();
	void                                 oval_state_free(struct oval_state_s*);

	void set_oval_state_family    (struct oval_state_s*, oval_family_enum);
	void set_oval_state_subtype   (struct oval_state_s*, oval_subtype_enum);
	void set_oval_state_name      (struct oval_state_s*, char*);
	void add_oval_state_notes     (struct oval_state_s*, char*);
	void set_oval_state_comment   (struct oval_state_s*, char*);
	void set_oval_state_id        (struct oval_state_s*, char*);
	void set_oval_state_deprecated(struct oval_state_s*, int);
	void set_oval_state_version   (struct oval_state_s*, int);

	struct oval_variable_s              *oval_variable_new();
	void                                 oval_variable_free(struct oval_variable_s*);

	void set_oval_variable_id       (struct oval_variable_s*, char*);
	void set_oval_variable_type     (struct oval_variable_s*, oval_variable_type_enum);
	void set_oval_variable_datatype (struct oval_variable_s*, oval_datatype_enum);
	void add_oval_variable_values   (struct oval_variable_s*, struct oval_value_s*);//type==OVAL_VARIABLE_CONSTANT
	void set_oval_variable_component(struct oval_variable_s*, struct oval_component_s*);//type==OVAL_VARIABLE_LOCAL

	struct oval_variable_binding_s      *oval_variable_binding_new();
	void                                 oval_variable_binding_free(struct oval_variable_binding_s*);

	void set_oval_variable_binding_variable(struct oval_variable_binding_s*, struct oval_variable_s*);
	void set_oval_variable_binding_value   (struct oval_variable_binding_s*, char*);

	struct oval_object_content_s        *oval_object_content_new(oval_object_content_type_enum type);
	void                                 oval_object_content_free(struct oval_object_content_s*);

	void set_oval_object_content_field_name(struct oval_object_content_s*, char*);
	void set_oval_object_content_entity    (struct oval_object_content_s*, struct oval_entity_s*);//type == OVAL_OBJECTCONTENT_ENTITY
	void set_oval_object_content_varCheck  (struct oval_object_content_s*, oval_check_enum);//type == OVAL_OBJECTCONTENT_ENTITY
	void set_oval_object_content_set       (struct oval_object_content_s*, struct oval_set_s*);   //type == OVAL_OBJECTCONTENT_SET
	typedef void (*oval_object_content_consumer)(struct oval_object_content_s*,void*);
	int oval_object_content_parse_tag(xmlTextReaderPtr, struct oval_parser_context_s*, oval_object_content_consumer, void*);

	struct oval_behavior_s              *oval_behavior_new();
	void                                 oval_behavior_free(struct oval_behavior_s*);

	void set_oval_behavior_value         (struct oval_behavior_s*, struct oval_value_s*);
	void set_oval_behavior_value_for_key (struct oval_behavior_s*, struct oval_value_s*, char *attribute_key);

	struct oval_entity_s                *oval_entity_new();
	void                                 oval_entity_free(struct oval_entity_s*);

	void set_oval_entity_type       (struct oval_entity_s*, oval_entity_type_enum);
	void set_oval_entity_datatype   (struct oval_entity_s*, oval_datatype_enum);
	void set_oval_entity_operator   (struct oval_entity_s*, oval_operator_enum);
	void set_oval_entity_mask       (struct oval_entity_s*, int);
	void set_oval_entity_varref_type(struct oval_entity_s*, oval_entity_varref_type_enum);
	void set_oval_entity_variable   (struct oval_entity_s*, struct oval_variable_s*);
	void set_oval_entity_value      (struct oval_entity_s*, struct oval_value_s*);

	typedef void (*oval_entity_consumer)(struct oval_entity_node_s*, void*);
	int oval_entity_parse_tag (xmlTextReaderPtr, struct oval_parser_context_s*, oval_entity_consumer, void*);


	struct oval_set_s                   *oval_set_new(oval_set_type_enum);
	void                                 oval_set_free(struct oval_set_s*);

	void set_oval_set_type     (struct oval_set_s*, oval_set_type_enum);
	void set_oval_set_operation(struct oval_set_s*, oval_set_operation_enum);
	void add_oval_set_subsets  (struct oval_set_s*, struct oval_set_s*);   //type==OVAL_SET_AGGREGATE;
	void add_oval_set_objects  (struct oval_set_s*, struct oval_object_s*);//type==OVAL_SET_COLLECTIVE;
	void add_oval_set_filters  (struct oval_set_s*, struct oval_state_s*); //type==OVAL_SET_COLLECTIVE;

	struct oval_value_s                 *oval_value_new();
	void                                 oval_value_free(struct oval_value_s*);

	void set_oval_value_datatype(struct oval_value_s*, oval_datatype_enum);
	void set_oval_value_text    (struct oval_value_s*, char*);
	void set_oval_value_binary  (struct oval_value_s*, unsigned char*);//datatype==OVAL_DATATYPE_BINARY
	void set_oval_value_boolean (struct oval_value_s*, int);           //datatype==OVAL_DATATYPE_BOOLEAN
	void set_oval_value_float   (struct oval_value_s*, float);         //datatype==OVAL_DATATYPE_FLOAT
	void set_oval_value_integer (struct oval_value_s*, long);          //datatype==OVAL_DATATYPE_INTEGER

	struct oval_component_s             *oval_component_new(oval_component_type_enum);
	void                                 oval_component_free(struct oval_component_s*);

	void set_oval_component_type               (struct oval_component_s*, oval_component_type_enum);
	void set_oval_component_literal_value      (struct oval_component_s*, struct oval_value_s*);   //type==OVAL_COMPONENT_LITERAL
	void set_oval_component_object             (struct oval_component_s*, struct oval_object_s*);  //type==OVAL_COMPONENT_OBJECTREF
	void set_oval_component_object_field       (struct oval_component_s*, char*);                  //type==OVAL_COMPONENT_OBJECTREF
	void set_oval_component_variable           (struct oval_component_s*, struct oval_variable_s*);//type==OVAL_COMPONENT_VARREF
	void set_oval_component_function_type      (struct oval_component_s*, oval_function_type_enum);//type==OVAL_COMPONENT_FUNCTION
	void add_oval_component_function_components(struct oval_component_s*, struct oval_component_s*);//type==OVAL_COMPONENT_FUNCTION
	void set_oval_component_function_keys      (struct oval_component_s*, char*);                   //type==OVAL_COMPONENT_FUNCTION
#endif
