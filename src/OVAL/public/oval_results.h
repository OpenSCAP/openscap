/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALRES
 * Interface for Results model
 * @{
 *
 * Class diagram
 * \image html result_model.png
 *
 * @file
 *
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
 *
 *
 */


/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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


#ifndef OVAL_RESULTS_H_
#define OVAL_RESULTS_H_

#include "oval_types.h"
#include "oval_system_characteristics.h"
#include <stdbool.h>

/**
 * @typedef oval_result_t
 * Result values for the evaluation of an OVAL Definition or an OVAL Test
 */
typedef enum {
	OVAL_RESULT_INVALID = 1, /**< Unspecified state. */
	OVAL_RESULT_TRUE = 2,    /**< Characteristics being evaluated match the information represented in the system characteristic. */
	OVAL_RESULT_FALSE = 4,   /**< Characteristics being evaluated do not match the information represented in the system characteristic. */
	OVAL_RESULT_UNKNOWN = 8, /**< Characteristics being evaluated can not be found in the system characteristic. */
	OVAL_RESULT_ERROR = 16,  /**< Characteristics being evaluated exist in the system characteristic file but there was an error either collecting information or in performing anaylsis. */
	OVAL_RESULT_NOT_EVALUATED = 32, /**< Choice was made not to evaluate the given definition or test. */
	OVAL_RESULT_NOT_APPLICABLE = 64 /**< Definition or test being evaluated is not valid on the given platform. */
} oval_result_t;

/**
 * @typedef oval_result_directive_content_t
 * Values for the directives controlling the expected content of the results file
 */
typedef enum {
	OVAL_DIRECTIVE_CONTENT_UNKNOWN = 0, /**< Undefined value */
	OVAL_DIRECTIVE_CONTENT_THIN = 1,    /**< Only the minimal amount of information will be provided. */
	OVAL_DIRECTIVE_CONTENT_FULL = 2     /**< Very detailed information will be provided allowing in-depth reports to be generated from the results. */
} oval_result_directive_content_t;


const char *oval_result_get_text(oval_result_t);



/**
 * @struct oval_results_model
 * OVAL Results Model holds OVAL results structure instances.
 */
struct oval_results_model;

/**
 * @struct oval_result_system
 */
struct oval_result_system;
/**
 * @struct oval_result_system_iterator
 * @see oval_results_model_get_systems
 */
struct oval_result_system_iterator;

/**
 * @struct oval_result_definition
 */
struct oval_result_definition;
/**
 * @struct oval_result_definition_iterator
 * @see oval_result_system_get_definitions
 */
struct oval_result_definition_iterator;

/**
 * @struct oval_result_test
 */
struct oval_result_test;
/**
 * @struct oval_result_test_iterator
 * @see oval_result_system_get_tests
 */
struct oval_result_test_iterator;

/**
 * @struct oval_result_item
 */
struct oval_result_item;
/**
 * @struct oval_result_item_iterator
 * @see oval_result_test_get_items
 */
struct oval_result_item_iterator;


/**
 * @struct oval_result_criteria_node
 */
struct oval_result_criteria_node;
/**
 * @struct oval_result_criteria_node_iterator
 * @see oval_result_criteria_node_get_subnodes 
 */
struct oval_result_criteria_node_iterator;

/**
 * @struct oval_result_directives
 * This structure holds instance of OVAL Result Directives for particular oval_results_model.
 * OVAL Result Directives describes what information has been included in the results file, therefore  
 * it's necessarry to setup this structure before exporting oval_results_model. Usage example:
 *
 * @code
 * struct oval_result_directives * res_direct = oval_result_directives_new(res_model);
 * oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE | OVAL_RESULT_FALSE , true);
 * oval_result_directives_set_content(res_direct,OVAL_RESULT_FALSE | OVAL_RESULT_TRUE | OVAL_RESULT_ERROR , OVAL_DIRECTIVE_CONTENT_FULL);
 * .....
 * oval_result_directives_free(res_direct);
 * @endcode
 *
 * 
 */
struct oval_result_directives;






/**
 * Import the content from a specified XML stream into a oval_result_model, return -1 if an error occurred.
 * If the input_source specifies a model entity that is already registered within the model its content is overwritten.
 * @param model the oval_results_model
 * @param source the input source (XML)
 * @memberof oval_results_model
 */
struct oval_result_directives *oval_results_model_import(struct oval_results_model *, struct oscap_import_source *);
/**
 * Create new oval_results_model.
 * The new model is bound to a specified oval_definition_model and variable bindings.
 * @param definition_model the specified oval_definition_model.
 * @param syschar_model the array of specified oval_syschar_model(s) terminated by NULL.
 * @memberof oval_results_model
 */
struct oval_results_model *oval_results_model_new(struct oval_definition_model *definition_model,
						  struct oval_syschar_model **);
/**
 * Copy an oval_results_model.
 * @return A copy of the specified @ref oval_results_model.
 * @memberof oval_results_model
 */
struct oval_results_model *oval_results_model_clone(struct oval_results_model *);
/**
 * Free memory allocated to a specified oval results model.
 * @param the specified oval_results model
 * @memberof oval_results_model
 */
void oval_results_model_free(struct oval_results_model *model);
/**
 * Export oval results to XML file.
 * @param model the oval_results_model
 * @param target the export target stream (XML)
 * @memberof oval_results_model
 */
int oval_results_model_export(struct oval_results_model *, struct oval_result_directives *,
			      struct oscap_export_target *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_results_model
 */
void oval_results_model_add_system(struct oval_results_model *, struct oval_result_system *);
/**
 * Lock the result_model instance.
 * The state of a locked instance cannot be changed.
 * This operation has no effect if the model is already locked.
 * @memberof oval_results_model
 */
void oval_results_model_lock(struct oval_results_model *result_model);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Return bound definition model from an oval_results_model.
 * @param model the specified oval_results_model.
 * @memberof oval_results_model
 */
struct oval_definition_model *oval_results_model_get_definition_model(struct oval_results_model *model);

/**
 * Return iterator over reporting systems.
 * @param model the specified results model
 * @memberof oval_results_model
 */
struct oval_result_system_iterator *oval_results_model_get_systems(struct oval_results_model *);
/**
 * @memberof oval_results_model
 */
bool oval_results_model_is_locked(struct oval_results_model *result_model);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Evaluate all result_systems. It's assumed that all necessary system 
 * characteristics for evaluation were altready gathered. 
 * @return 0 on sucess and -1 on fail. Use \ref ERRORS mechanism to examine the error.
 */
int oval_results_model_eval(struct oval_results_model *);
/**
 * Return <b>true</b> if the results_model instance is valid
 * An oval_results_model is valid if all the following are true
 * 	- The associated oval_definition_model is valid
 * 	- The systems attribute is not an empty iterator and
 * 	  each iterated oval_result_system is valid.
 * @memberof oval_results_model
 */
bool oval_results_model_is_valid(struct oval_results_model *results_model);
/** @} */






/**
 * @memberof oval_result_system
 */
struct oval_result_system *oval_result_system_new(struct oval_results_model *, struct oval_syschar_model *);
/**
 * @return A copy of the specified @ref oval_result_system.
 * @memberof oval_result_system
 */
struct oval_result_system *oval_result_system_clone(struct oval_results_model *new_model,
						    struct oval_result_system *old_system);
/**
 * @memberof oval_result_system
 */
void oval_result_system_free(struct oval_result_system *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_result_system
 */
void oval_result_system_add_definition(struct oval_result_system *, struct oval_result_definition *);
/**
 * @memberof oval_result_system
 */
void oval_result_system_add_test(struct oval_result_system *, struct oval_result_test *);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * @memberof oval_result_system
 */
struct oval_results_model *oval_result_system_get_results_model(struct oval_result_system *);
/**
 * @memberof oval_result_system
 */
struct oval_result_definition *oval_result_system_get_definition(struct oval_result_system *, char *);
/**
 * @memberof oval_result_system
 */
struct oval_result_definition_iterator *oval_result_system_get_definitions(struct oval_result_system *);
/**
 * @memberof oval_result_system
 */
struct oval_result_test_iterator *oval_result_system_get_tests(struct oval_result_system *);
/**
 * @memberof oval_result_system
 */
struct oval_syschar_model *oval_result_system_get_syschar_model(struct oval_result_system *);
/**
 * @memberof oval_result_system
 */
struct oval_sysinfo *oval_result_system_get_sysinfo(struct oval_result_system *);
/**
 * @memberof oval_result_system
 */
bool oval_result_system_is_locked(struct oval_result_system *result_system);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_result_system_iterator
 */
bool oval_result_system_iterator_has_more(struct oval_result_system_iterator *);
/**
 * @memberof oval_result_system_iterator
 */
struct oval_result_system *oval_result_system_iterator_next(struct oval_result_system_iterator *);
/**
 * @memberof oval_result_system_iterator
 */
void oval_result_system_iterator_free(struct oval_result_system_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Function evaluates all OVAL definitions of specified result_system. It assumes that all necessary system 
 * characteristics for evaluation were altready gathered.
 * @memberof oval_result_system
 * @param sys is result_system from result_model
 * @return 0 on sucess and -1 on fail. Use \ref ERRORS mechanism to examine the error.
 */
int oval_result_system_eval(struct oval_result_system *sys);
/**
 * Function evaluates specified OVAL definition in result_system. It assumes that all necessary system 
 * characteristics for evaluation were altready gathered.
 * @memberof oval_result_system
 * @param sys is result_system from result_model
 * @param id of the definition from definition_model from result_model
 * @return OVAL_RESULT_INVALID if there was a problem in evaluation. Use \ref ERRORS mechanism to examine the error. Otherwise one of valid
 * values for the evaluation of an OVAL Definitions is returned.
 */
oval_result_t oval_result_system_eval_definition(struct oval_result_system *sys, char *id);
/**
 * Return <b>true</b> if the result_system instance is valid
 * An oval_result_system is valid if all the following are true
 * 	- The associated oval_syschar_model is valid
 * 	- All included result definitions are valid
 * 	- All included result tests are valid
 * @memberof oval_result_system
 */
bool oval_result_system_is_valid(struct oval_result_system *result_system);
/** @} */






/**
 * @memberof oval_result_definition
 */
struct oval_result_definition *oval_result_definition_new(struct oval_result_system *, char *);
/**
 * @return A copy of the specified @ref oval_result_definition.
 * @memberof oval_result_definition
 */
struct oval_result_definition *oval_result_definition_clone
    (struct oval_result_system *new_system, struct oval_result_definition *old_definition);
/**
 * @memberof oval_result_definition
 */
void oval_result_definition_free(struct oval_result_definition *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_result_definition
 */
void oval_result_definition_set_result(struct oval_result_definition *, oval_result_t);
/**
 * @memberof oval_result_definition
 */
void oval_result_definition_set_instance(struct oval_result_definition *, int);
/**
 * @memberof oval_result_definition
 */
void oval_result_definition_set_criteria(struct oval_result_definition *, struct oval_result_criteria_node *);
/**
 * @memberof oval_result_definition
 */
void oval_result_definition_add_message(struct oval_result_definition *, struct oval_message *);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * @memberof oval_result_definition
 */
struct oval_definition *oval_result_definition_get_definition(struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
struct oval_result_system *oval_result_definition_get_system(struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
int oval_result_definition_get_instance(struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
oval_result_t oval_result_definition_eval(struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
oval_result_t oval_result_definition_get_result(struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
struct oval_message_iterator *oval_result_definition_get_messages(struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
struct oval_result_criteria_node *oval_result_definition_get_criteria(struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
bool oval_result_definition_is_locked(struct oval_result_definition *result_definition);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_result_definition_iterator
 */
bool oval_result_definition_iterator_has_more(struct oval_result_definition_iterator *);
/**
 * @memberof oval_result_definition_iterator
 */
struct oval_result_definition *oval_result_definition_iterator_next(struct oval_result_definition_iterator *);
/**
 * @memberof oval_result_definition_iterator
 */
void oval_result_definition_iterator_free(struct oval_result_definition_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the result_definition instance is valid
 * An oval_result_definition is valid if all the following are true
 * 	- The system attribute is non-NULL
 * 	- The associated oval_definition is valid
 * 	- All included result criteria nodes are valid
 * @memberof oval_result_definition
 */
bool oval_result_definition_is_valid(struct oval_result_definition *result_definition);
/** @} */






/**
 * @memberof oval_result_test
 */
struct oval_result_test *oval_result_test_new(struct oval_result_system *, char *);
/**
 * @return A copy of the specified @ref oval_result_test.
 * @memberof oval_result_test
 */
struct oval_result_test *oval_result_test_clone
    (struct oval_result_system *new_system, struct oval_result_test *old_test);
/**
 * @memberof oval_result_test
 */
void oval_result_test_free(struct oval_result_test *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_result_test
 */
void oval_result_test_set_result(struct oval_result_test *, oval_result_t);
/**
 * @memberof oval_result_test
 */
void oval_result_test_set_instance(struct oval_result_test *test, int instance);
/**
 * @memberof oval_result_test
 */
void oval_result_test_set_message(struct oval_result_test *, struct oval_message *);
/**
 * @memberof oval_result_test
 */
void oval_result_test_add_item(struct oval_result_test *, struct oval_result_item *);
/**
 * @memberof oval_result_test
 */
void oval_result_test_add_binding(struct oval_result_test *, struct oval_variable_binding *);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * @memberof oval_result_test
 */
struct oval_test *oval_result_test_get_test(struct oval_result_test *);
/**
 * @memberof oval_result_test
 */
struct oval_result_system *oval_result_test_get_system(struct oval_result_test *);
/**
 * @memberof oval_result_test
 */
oval_result_t oval_result_test_eval(struct oval_result_test *);
/**
 * @memberof oval_result_test
 */
oval_result_t oval_result_test_get_result(struct oval_result_test *);
/**
 * @memberof oval_result_test
 */
int oval_result_test_get_instance(struct oval_result_test *);
/**
 * @memberof oval_result_test
 */
struct oval_message *oval_result_test_get_message(struct oval_result_test *);
/**
 * @memberof oval_result_test
 */
struct oval_result_item_iterator *oval_result_test_get_items(struct oval_result_test *);
/**
 * @memberof oval_result_test
 */
struct oval_variable_binding_iterator *oval_result_test_get_bindings(struct oval_result_test *);
/**
 * Return <b>true</b> if the result_test instance is locked.
 * The state of a locked instance cannot be changed.
 */
bool oval_result_test_is_locked(struct oval_result_test *result_test);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_result_test_iterator
 */
bool oval_result_test_iterator_has_more(struct oval_result_test_iterator *);
/**
 * @memberof oval_result_test_iterator
 */
struct oval_result_test *oval_result_test_iterator_next(struct oval_result_test_iterator *);
/**
 * @memberof oval_result_test_iterator
 */
void oval_result_test_iterator_free(struct oval_result_test_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the result_test instance is valid
 * An oval_result_test is valid if all the following are true
 * 	- The associated oval_test is valid
 * 	- All included result items are valid
 * 	- All included variable bindings are valid
 * @memberof oval_result_test
 */
bool oval_result_test_is_valid(struct oval_result_test *result_test);
/** @} */






/**
 * @memberof oval_result_item
 */
struct oval_result_item *oval_result_item_new(struct oval_result_system *, char *);
/**
 * @return A copy of the specified @ref oval_result_item.
 * @memberof oval_result_item
 */
struct oval_result_item *oval_result_item_clone
    (struct oval_result_system *new_system, struct oval_result_item *old_item);
/**
 * @memberof oval_result_item
 */
void oval_result_item_free(struct oval_result_item *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_result_item
 */
void oval_result_item_set_result(struct oval_result_item *, oval_result_t);
/**
 * @memberof oval_result_item
 */
void oval_result_item_add_message(struct oval_result_item *, struct oval_message *);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * @memberof oval_result_item
 */
struct oval_sysdata *oval_result_item_get_sysdata(struct oval_result_item *);
/**
 * @memberof oval_result_item
 */
oval_result_t oval_result_item_get_result(struct oval_result_item *);
/**
 * @memberof oval_result_item
 */
struct oval_message_iterator *oval_result_item_get_messages(struct oval_result_item *);
/**
 * @memberof oval_result_item
 */
bool oval_result_item_is_locked(struct oval_result_item *result_item);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_result_item_iterator
 */
bool oval_result_item_iterator_has_more(struct oval_result_item_iterator *);
/**
 * @memberof oval_result_item_iterator
 */
struct oval_result_item *oval_result_item_iterator_next(struct oval_result_item_iterator *);
/**
 * @memberof oval_result_item_iterator
 */
void oval_result_item_iterator_free(struct oval_result_item_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the result_item instance is valid
 * An oval_result_item is valid if all the following are true
 * 	- The sysdata attribute is a valid oval_sysdata
 * @memberof oval_result_item
 */
bool oval_result_item_is_valid(struct oval_result_item *result_item);
/** @} */






/**
 * @memberof oval_result_criteria_node
 */
struct oval_result_criteria_node *oval_result_criteria_node_new(struct oval_result_system *, oval_criteria_node_type_t,
								int, ...);
/**
 * @return A copy of the specified @ref oval_result_criteria_node.
 * @memberof oval_result_criteria_node
 */
struct oval_result_criteria_node *oval_result_criteria_node_clone
    (struct oval_result_system *new_system, struct oval_result_criteria_node *old_node);
/**
 * @memberof oval_result_criteria_node
 */
void oval_result_criteria_node_free(struct oval_result_criteria_node *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_result_criteria_node
 */
void oval_result_criteria_node_set_result(struct oval_result_criteria_node *, oval_result_t);
/**
 * @memberof oval_result_criteria_node
 */
void oval_result_criteria_node_set_negate(struct oval_result_criteria_node *, bool);
/**
 * @memberof oval_result_criteria_node
 */
void oval_result_criteria_node_set_operator(struct oval_result_criteria_node *, oval_operator_t);	//type==NODETYPE_CRITERIA
/**
 * @memberof oval_result_criteria_node
 */
void oval_result_criteria_node_add_subnode(struct oval_result_criteria_node *, struct oval_result_criteria_node *);	//type==NODETYPE_CRITERIA
/**
 * @memberof oval_result_criteria_node
 */
void oval_result_criteria_node_set_test(struct oval_result_criteria_node *, struct oval_result_test *);	//type==NODETYPE_CRITERION
/**
 * @memberof oval_result_criteria_node
 */
void oval_result_criteria_node_set_extends(struct oval_result_criteria_node *, struct oval_result_definition *);	//type==NODETYPE_EXTENDDEF
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 */
oval_criteria_node_type_t oval_result_criteria_node_get_type(struct oval_result_criteria_node *);
/**
 * @memberof oval_result_criteria_node
 */
oval_result_t oval_result_criteria_node_eval(struct oval_result_criteria_node *);
/**
 * @memberof oval_result_criteria_node
 */
oval_result_t oval_result_criteria_node_get_result(struct oval_result_criteria_node *);
/**
 * @memberof oval_result_criteria_node
 */
bool oval_result_criteria_node_get_negate(struct oval_result_criteria_node *);
/**
 * @memberof oval_result_criteria_node
 */
oval_operator_t oval_result_criteria_node_get_operator(struct oval_result_criteria_node *);	//type==NODETYPE_CRITERIA
/**
 * @memberof oval_result_criteria_node
 */
struct oval_result_criteria_node_iterator *oval_result_criteria_node_get_subnodes(struct oval_result_criteria_node *);	//type==NODETYPE_CRITERIA
/**
 * @memberof oval_result_criteria_node
 */
struct oval_result_test *oval_result_criteria_node_get_test(struct oval_result_criteria_node *);	//type==NODETYPE_CRITERION
/**
 * @memberof oval_result_criteria_node
 */
struct oval_result_definition *oval_result_criteria_node_get_extends(struct oval_result_criteria_node *);	//type==NODETYPE_EXTENDDEF
/**
 * @memberof oval_result_criteria_node
 */
bool oval_result_criteria_node_is_locked(struct oval_result_criteria_node *result_criteria_node);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_result_criteria_node_iterator
 */
bool oval_result_criteria_node_iterator_has_more(struct oval_result_criteria_node_iterator *);
/**
 * @memberof oval_result_criteria_node_iterator
 */
struct oval_result_criteria_node *oval_result_criteria_node_iterator_next(struct oval_result_criteria_node_iterator *);
/**
 * @memberof oval_result_criteria_node_iterator
 */
void oval_result_criteria_node_iterator_free(struct oval_result_criteria_node_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Returns <b>true</b> if @ref Oval_result_criteria_node is valid.
 * An Oval_result_criteria_node is valid if one of the following is true:
 * 	- The type attribute is @ref OVAL_NODETYPE_CRITERIA -- AND
 * 		- The subnode attribute is not an empty iterator -- AND
 * 		- Each iterated Oval_result_criteria_node is valid.
 * 	- The type attribute is @ref OVAL_NODETYPE_CRITERION -- AND
 * 		- The referenced result test is valid.
 * 	- The type attribute is @ref OVAL_NODETYPE_EXTENDDEF -- AND
 * 		- The referenced result definition is valid.
 * @memberof oval_result_criteria_node
 */
bool oval_result_criteria_node_is_valid(struct oval_result_criteria_node *result_criteria_node);
/** @} */




/**
 * Create new OVAL Results Directives instance. Directives are setup NOT to report any type of result by default.
 * @memberof oval_result_directives
 */
struct oval_result_directives *oval_result_directives_new(struct oval_results_model *);
/**
 * @memberof oval_result_directives
 */
void oval_result_directives_free(struct oval_result_directives *);


/**
 * @name Setters
 * @{
 */
/**
 * Set (or unset) result types that are intended to be reported. Functions does not override previous settings.
 * @memberof oval_result_directives
 */
void oval_result_directives_set_reported(struct oval_result_directives *, int flag, bool val);
/**
 * Configure the depth of infomation.
 * @memberof oval_result_directives
 */
void oval_result_directives_set_content(struct oval_result_directives *, int flag, oval_result_directive_content_t);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * @memberof oval_result_directives
 */
bool oval_result_directives_get_reported(struct oval_result_directives *, oval_result_t);
/**
 * @memberof oval_result_directives
 */
oval_result_directive_content_t oval_result_directives_get_content(struct oval_result_directives *, oval_result_t);
/**
 * @memberof oval_result_directives
 */
bool oval_result_directives_is_locked(struct oval_result_directives *result_directives);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * @memberof oval_result_directives
 */
bool oval_result_directives_is_valid(struct oval_result_directives *result_directives);
/** @} */






/** @} */
/**
 * @}END OVALRES
 * @}END OVAL
 */

#endif				/* OVAL_RESULTS_H_ */
