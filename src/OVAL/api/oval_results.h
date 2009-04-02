#ifndef OVAL_RESULTS_H_
#define OVAL_RESULTS_H_
#include <liboval/oval_definitions.h>
#include <liboval/oval_system_characteristics.h>

	typedef enum{
		OVAL_RESULT_TRUE,
		OVAL_RESULT_FALSE,
		OVAL_RESULT_UNKNOWN,
		OVAL_RESULT_ERROR,
		OVAL_RESULT_NOT_EVALUATED,
		OVAL_RESULT_NOT_APPLICABLE
	} oval_result_enum;

	struct oval_result_item_s;
	struct oval_iterator_result_item_s;

	struct oval_result_test_s;
	struct oval_iterator_result_test_s;

	struct oval_result_criteria_node_s;
	struct oval_iterator_result_criteria_node_s;

	struct oval_result_directives_s;

	struct oval_result_s;
	struct oval_iterator_result_s;

	int  oval_iterator_result_item_has_more                  (struct oval_iterator_result_item_s*);
	struct oval_result_item_s *oval_iterator_result_item_next(struct oval_iterator_result_item_s*);

	struct oval_sysdata_s *oval_result_item_sysdata   (struct oval_result_item_s*);
	oval_result_enum oval_result_item_result          (struct oval_result_item_s*);
	char *oval_result_item_message                    (struct oval_result_item_s*);

	int  oval_iterator_result_test_has_more                   (struct oval_iterator_result_test_s*);
	struct oval_result_test_s *oval_iterator_result_test_next (struct oval_iterator_result_test_s*);

	struct oval_test_s *oval_result_test_test                 (struct oval_result_test_s*);
	oval_check_enum oval_result_test_check                    (struct oval_result_test_s*);
	oval_result_enum oval_result_test_result                  (struct oval_result_test_s*);
	char *oval_result_test_message                            (struct oval_result_test_s*);
	struct oval_iterator_result_item_s *oval_result_test_items(struct oval_result_test_s*);

	int  oval_iterator_result_criteria_node_has_more                           (struct oval_iterator_result_criteria_node_s*);
	struct oval_result_criteria_node_s *oval_iterator_result_criteria_node_next(struct oval_iterator_result_criteria_node_s*);

	oval_criteria_node_type_enum oval_result_criteria_node_type             (struct oval_result_criteria_node_s*);
	oval_result_enum oval_result_criteria_node_result                       (struct oval_result_criteria_node_s*);
	oval_operator_enum oval_result_criteria_node_operator                   (struct oval_result_criteria_node_s*);//type==NODETYPE_CRITERIA
	struct oval_iterator_criteria_node_s *oval_result_criteria_node_subnodes(struct oval_result_criteria_node_s*);//type==NODETYPE_CRITERIA
	struct oval_result_test_s *oval_result_criteria_node_test               (struct oval_result_criteria_node_s*);//type==NODETYPE_CRITERION
	struct oval_result_s *oval_result_criteria_node_extends                 (struct oval_result_criteria_node_s*);//type==NODETYPE_EXTENDDEF

	int oval_result_directives_definition_true          (struct oval_result_directives_s*);
	int oval_result_directives_definition_false         (struct oval_result_directives_s*);
	int oval_result_directives_definition_unknown       (struct oval_result_directives_s*);
	int oval_result_directives_definition_error         (struct oval_result_directives_s*);
	int oval_result_directives_definition_not_evaluated (struct oval_result_directives_s*);
	int oval_result_directives_definition_not_applicable(struct oval_result_directives_s*);

	int  oval_iterator_result_has_more             (struct oval_iterator_result_s*);
	struct oval_result_s *oval_iterator_result_next(struct oval_iterator_result_s*);

	struct oval_definition_s *oval_result_definition         (struct oval_result_s*);
	oval_result_enum oval_result_result                      (struct oval_result_s*);
	char *oval_result_message                                (struct oval_result_s*);
	struct oval_result_directives_s *oval_result_directives  (struct oval_result_s*);
	struct oval_result_criteria_node_s *oval_result_criteria (struct oval_result_s*);

#endif /* OVAL_RESULTS_H_ */
