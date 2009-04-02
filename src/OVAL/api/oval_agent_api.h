#ifndef OVAL_AGENT_API_H_
#define OVAL_AGENT_API_H_
#include <liboval/oval_definitions.h>
#include <liboval/oval_system_characteristics.h>
#include <liboval/oval_results.h>

	struct export_target_s;
	struct import_source_s;

	struct oval_object_model_s;

	struct oval_object_model_s *oval_object_model_new();


	typedef enum {
	    XML_VALIDITY_WARNING = 1,
	    XML_VALIDITY_ERROR = 2,
	    XML_WARNING = 3,
	    XML_ERROR = 4
	} oval_xml_severity_enum;

	struct oval_xml_error_s{
		void* user_arg;
		char* message;
		oval_xml_severity_enum severity;
		int   line_number;
		char* system_id;
	};

	typedef int (*oval_xml_error_handler)(struct oval_xml_error_s*, void* user_arg);

	void load_oval_definitions(struct oval_object_model_s*, struct import_source_s*, oval_xml_error_handler, void*);

	struct oval_definition_s *get_oval_definition(struct oval_object_model_s*, char *def_urn);
	struct oval_test_s       *get_oval_test      (struct oval_object_model_s*, char *tst_urn);
	struct oval_object_s     *get_oval_object    (struct oval_object_model_s*, char *obj_urn);
	struct oval_state_s      *get_oval_state     (struct oval_object_model_s*, char *ste_urn);
	struct oval_variable_s   *get_oval_variable  (struct oval_object_model_s*, char *var_urn);

	struct oval_iterator_definition_s *get_oval_definitions(struct oval_object_model_s*);
	struct oval_iterator_test_s       *get_oval_tests      (struct oval_object_model_s*);
	struct oval_iterator_object_s     *get_oval_objects    (struct oval_object_model_s*);
	struct oval_iterator_state_s      *get_oval_states     (struct oval_object_model_s*);
	struct oval_iterator_variable_s   *get_oval_variables  (struct oval_object_model_s*);

	/* needs oval_characteristics, export_target_s struct definitions */
	int export_characteristics(struct oval_iterator_syschar_s*,struct export_target_s*);

	/* needs oval_results, export_target_s struct definitions */
	int export_results(struct oval_result_s*,struct export_target_s*);

	struct oval_iterator_syschar_s *probe_object  (struct oval_object_s *   ,                                  struct oval_iterator_variable_binding_s*);
	struct oval_result_test_s *resolve_test       (struct oval_test_s *     , struct oval_iterator_syschar_s*, struct oval_iterator_variable_binding_s*);
	struct oval_result_s *resolve_definition      (struct oval_definition_s*, struct oval_iterator_syschar_s*, struct oval_iterator_variable_binding_s*);

#endif /*OVAL_AGENT_API_H_ */
