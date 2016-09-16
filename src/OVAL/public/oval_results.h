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
 * Copyright 2009-2014 Red Hat Inc., Durham, North Carolina.
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
 *      Šimon Lukašík
 */


#ifndef OVAL_RESULTS_H_
#define OVAL_RESULTS_H_

#include "oscap.h"
#include "oscap_source.h"
#include "oval_types.h"
#include "oval_system_characteristics.h"
#include "oval_directives.h"
#include <stdbool.h>


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
 * Create new oval_results_model.
 * The new model is bound to a specified oval_definition_model and variable bindings.
 * @param definition_model the specified oval_definition_model.
 * @param syschar_model the array of specified oval_syschar_model(s) terminated by NULL.
 * @memberof oval_results_model
 */
struct oval_results_model *oval_results_model_new(struct oval_definition_model *definition_model,
						  struct oval_syschar_model **);

/**
 * Import the content from the oscap_source into an oval_result_model.
 * If imported content specifies a model entity that is already registered within the model its content is overwritten.
 * @memberof oval_results_model
 * @param model the oval_results_model
 * @param source The oscap_source to import from
 * @return -1 if an error occurred
 */
int oval_results_model_import_source(struct oval_results_model *model, struct oscap_source *source);

/**
 * Import the content from the file into an oval_result_model.
 * If imported content specifies a model entity that is already registered within the model its content is overwritten.
 * @param model the oval_results_model
 * @param file filename
 * @return -1 if an error occurred
 * @memberof oval_results_model
 * @deprecated This function has been deprecated and it may be dropped from later
 * OpenSCAP releases. Please use oval_results_model_import_source instead.
 */
OSCAP_DEPRECATED(int oval_results_model_import(struct oval_results_model *model, const char *file));
/**
 * Copy an oval_results_model.
 * @return A copy of the specified @ref oval_results_model.
 * @memberof oval_results_model
 */
struct oval_results_model *oval_results_model_clone(struct oval_results_model *);
/**
 * @memberof oval_results_model
 */
void oval_results_model_set_export_system_characteristics(struct oval_results_model *, bool export);

/**
 * @memberof oval_results_model
 */
bool oval_results_model_get_export_system_characteristics(struct oval_results_model *);
/**
 * Free memory allocated to a specified oval results model.
 * @param the specified oval_results model
 * @memberof oval_results_model
 */
void oval_results_model_free(struct oval_results_model *model);
/**
 * Export oval results into file.
 * @param model the oval_results_model
 * @param model the oval_directives_model
 * @param file filename
 * @memberof oval_results_model
 */
int oval_results_model_export(struct oval_results_model *, struct oval_directives_model *, const char *file);

/**
 * Export OVAL results into oscap_source
 * @param results_model The OVAL Results Model to export
 * @param directives_model The Directives Model to amend the export
 * @param filename A suggested name (filename) to assign with the oscap_source
 * This name may later be used when storing the oscap_source to disk drive.
 * @returns Newly created oscap_source or NULL in case of failure
 */
struct oscap_source *oval_results_model_export_source(struct oval_results_model *results_model, struct oval_directives_model *directives_model, const char *name);

/**
 * @name Setters
 * @{
 */
void oval_results_model_set_generator(struct oval_results_model *model, struct oval_generator *generator);
/** @} */

/**
 * @name Getters
 * @{
 */
struct oval_generator *oval_results_model_get_generator(struct oval_results_model *model);
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
struct oval_result_definition *oval_result_system_get_definition(struct oval_result_system *, const char *);
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
 * @return 0 on succeess, or non 0 if an error occurred. Use \ref ERRORS mechanism to examine the error.
 */
int oval_result_system_eval_definition(struct oval_result_system *sys, const char *id);
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
struct oval_definition *oval_result_definition_get_definition(const struct oval_result_definition *);
/**
 * Returns the @idref attribute of a given result definition
 * @memberof oval_result_definition
 */
const char *oval_result_definition_get_id(const struct oval_result_definition *rslt_definition);
/**
 * @memberof oval_result_definition
 */
struct oval_result_system *oval_result_definition_get_system(const struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
int oval_result_definition_get_instance(const struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
oval_result_t oval_result_definition_eval(struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
oval_result_t oval_result_definition_get_result(const struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
struct oval_message_iterator *oval_result_definition_get_messages(const struct oval_result_definition *);
/**
 * @memberof oval_result_definition
 */
struct oval_result_criteria_node *oval_result_definition_get_criteria(const struct oval_result_definition *);
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
void oval_result_test_add_message(struct oval_result_test *, struct oval_message *);
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
struct oval_message_iterator *oval_result_test_get_messages(struct oval_result_test *);
/**
 * @memberof oval_result_test
 */
struct oval_result_item_iterator *oval_result_test_get_items(struct oval_result_test *);
/**
 * @memberof oval_result_test
 */
struct oval_variable_binding_iterator *oval_result_test_get_bindings(struct oval_result_test *);
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
struct oval_sysitem *oval_result_item_get_sysitem(struct oval_result_item *);
/**
 * @memberof oval_result_item
 */
oval_result_t oval_result_item_get_result(struct oval_result_item *);
/**
 * @memberof oval_result_item
 */
struct oval_message_iterator *oval_result_item_get_messages(struct oval_result_item *);
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
/** @} */






/**
 * @memberof oval_result_criteria_node
 */
struct oval_result_criteria_node *oval_result_criteria_node_new(struct oval_result_system *,
								oval_criteria_node_type_t,
								int, int, ...);
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
void oval_result_criteria_node_set_applicability_check(struct oval_result_criteria_node *, bool);
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
bool oval_result_criteria_node_get_applicability_check(struct oval_result_criteria_node *);
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
/** @} */


/** @} */
/**
 * @}END OVALRES
 * @}END OVAL
 */

#endif				/* OVAL_RESULTS_H_ */

