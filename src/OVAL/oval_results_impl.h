#ifndef OVAL_RESULTS_IMPL_H_
#define OVAL_RESULTS_IMPL_H_
#include "api/oval_results.h"
#include "oval_definitions_impl.h"
#include "oval_system_characteristics_impl.h"

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
void set_oval_result_criteria_node_operator(struct oval_result_criteria_node *, oval_operator_enum);	//type==NODETYPE_CRITERIA
void add_oval_result_criteria_node_subnode(struct oval_result_criteria_node *, struct oval_criteria_node *);	//type==NODETYPE_CRITERIA
void set_oval_result_criteria_node_test(struct oval_result_criteria_node *, struct oval_result_test *);	//type==NODETYPE_CRITERION
void set_oval_result_criteria_node_extends(struct oval_result_criteria_node *, struct oval_result *);	//type==NODETYPE_EXTENDDEF

struct oval_result *oval_result_new();
void oval_result_free(struct oval_result *);

void set_oval_result_definition(struct oval_result *, struct oval_definition *);
void set_oval_result_result(struct oval_result *, oval_result_enum);
void set_oval_result_message(struct oval_result *, char *);
void set_oval_result_directives(struct oval_result *,
				struct oval_result_directives *);
void set_oval_result_criteria(struct oval_result *,
			      struct oval_result_criteria_node *);

#endif				/* OVAL_RESULTS_IMPL_H_ */
