/**
 * @file oval_results.h
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


#ifndef OVAL_RESULTS_H_
#define OVAL_RESULTS_H_

#include "oval_definitions.h"
#include "oval_system_characteristics.h"
#include <stdbool.h>
/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALRES OVAL Results interface
 * @{
 */
typedef enum {
	OVAL_RESULT_INVALID        = 0,
	OVAL_RESULT_TRUE           = 1,
	OVAL_RESULT_FALSE          = 2,
	OVAL_RESULT_UNKNOWN        = 3,
	OVAL_RESULT_ERROR          = 4,
	OVAL_RESULT_NOT_EVALUATED  = 5,
	OVAL_RESULT_NOT_APPLICABLE = 6
} oval_result_t;

typedef enum {
	OVAL_DIRECTIVE_CONTENT_UNKNOWN = 0,
	OVAL_DIRECTIVE_CONTENT_THIN    = 1,
	OVAL_DIRECTIVE_CONTENT_FULL    = 2
} oval_result_directive_content_t;
/**
* @addtogroup OVALRES_setters Setters
* @{
* @ref OVALRES set methods.
*	These methods will not change the state of a locked instance.
*	@see oval_result_model_get_locked
*	@see oval_result_model_set_locked
* @}
* @addtogroup OVALRES_getters Getters
* @{
* @ref OVALRES get methods.
* @}
* @addtogroup OVALRES_service Service
* @{
* @ref OVALRES import/export methods.
* @}
* @addtogroup OVALRES_eval    Evaluators
* @{
* @ref OVALRES evaluation methods.
* @}
*/
/**
 * @addtogroup Oval_result_definition
 * @{
 * Network interface description.
 * Instances of Oval_result_definition are used to describe existing network interfaces on the system.
 * This information can help identify a specific system on a given network. *
 * @addtogroup oval_result_definition_setters Setters
 * @{
 * @ref Oval_result_definition set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_result_definition_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_result_definition_getters Getters
 * @{
 * @ref Oval_result_definition get methods.
 * @}
 * @addtogroup oval_result_definition_iterators Iterators
 * @{
 * @ref Oval_result_definition iterator methods.
 * @}
 * @addtogroup oval_result_definition_eval Evaluators
 * @{
 * @ref Oval_result_definition evaluator methods
 * @}
 */
/**
 * @struct oval_result_definition
 * Handle: @ref Oval_result_definition
 */
struct oval_result_definition;
/**
 * @struct oval_result_definition_iterator
 * Handle: @ref Oval_result_definition iterator
 */
struct oval_result_definition_iterator;
/**
 * @}
 * @addtogroup Oval_result_item
 * @{
 * Network interface description.
 * Instances of Oval_result_item are used to describe existing network interfaces on the system.
 * This information can help identify a specific system on a given network. *
 * @addtogroup oval_result_item_setters Setters
 * @{
 * @ref Oval_result_item set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_result_item_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_result_item_getters Getters
 * @{
 * @ref Oval_result_item get methods.
 * @}
 * @addtogroup oval_result_item_iterators Iterators
 * @{
 * @ref Oval_result_item iterator methods.
 * @}
 * @addtogroup oval_result_item_eval Evaluators
 * @{
 * @ref Oval_result_item evaluator methods
 * @}
 */
/**
 * @struct oval_result_item
 * Handle: @ref Oval_result_item
 */
struct oval_result_item;
/**
 * @struct oval_result_item_iterator
 * Handle: @ref Oval_result_item iterator
 */
struct oval_result_item_iterator;
/**
 * @}
 * @addtogroup Oval_result_test
 * @{
 * Network interface description.
 * Instances of Oval_result_test are used to describe existing network interfaces on the system.
 * This information can help identify a specific system on a given network. *
 * @addtogroup oval_result_test_setters Setters
 * @{
 * @ref Oval_result_test set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_result_test_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_result_test_getters Getters
 * @{
 * @ref Oval_result_test get methods.
 * @}
 * @addtogroup oval_result_test_iterators Iterators
 * @{
 * @ref Oval_result_test iterator methods.
 * @}
 * @addtogroup oval_result_test_eval Evaluators
 * @{
 * @ref Oval_result_test evaluator methods
 * @}
 */
/**
 * @struct oval_result_test
 * Handle: @ref Oval_result_test
 */
struct oval_result_test;
/**
 * @struct oval_result_test_iterator
 * Handle: @ref Oval_result_test iterator
 */
struct oval_result_test_iterator;
/**
 * @}
 * @addtogroup Oval_result_criteria_node
 * @{
 * Network interface description.
 * Instances of Oval_result_criteria_node are used to describe existing network interfaces on the system.
 * This information can help identify a specific system on a given network. *
 * @addtogroup oval_result_criteria_node_setters Setters
 * @{
 * @ref Oval_result_criteria_node set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_result_criteria_node_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_result_criteria_node_getters Getters
 * @{
 * @ref Oval_result_criteria_node get methods.
 * @}
 * @addtogroup oval_result_criteria_node_iterators Iterators
 * @{
 * @ref Oval_result_criteria_node iterator methods.
 * @}
 * @addtogroup oval_result_criteria_node_eval Evaluators
 * @{
 * @ref Oval_result_criteria_node evaluator methods
 * @}
 */
/**
 * @struct oval_result_criteria_node
 * Handle: @ref Oval_result_criteria_node
 */
struct oval_result_criteria_node;
/**
 * @struct oval_result_criteria_node_iterator
 * Handle: @ref Oval_result_criteria_node iterator
 */
struct oval_result_criteria_node_iterator;
/**
 * @}
 * @addtogroup Oval_result_directive
 * @{
 * Network interface description.
 * Instances of Oval_result_directive are used to describe existing network interfaces on the system.
 * This information can help identify a specific system on a given network. *
 * @addtogroup oval_result_directive_setters Setters
 * @{
 * @ref Oval_result_directive set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_result_directive_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_result_directive_getters Getters
 * @{
 * @ref Oval_result_directive get methods.
 * @}
 * @addtogroup oval_result_directive_iterators Iterators
 * @{
 * @ref Oval_result_directive iterator methods.
 * @}
 * @addtogroup oval_result_directive_eval Evaluators
 * @{
 * @ref Oval_result_directive evaluator methods
 * @}
 */
/**
 * @struct oval_result_directives
 * Handle: @ref Oval_result_directives
 */
struct oval_result_directives;
/**
 * @}
 * @addtogroup Oval_result_system
 * @{
 * Network interface description.
 * Instances of Oval_result_system are used to describe existing network interfaces on the system.
 * This information can help identify a specific system on a given network. *
 * @addtogroup oval_result_system_setters Setters
 * @{
 * @ref Oval_result_system set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_result_system_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_result_system_getters Getters
 * @{
 * @ref Oval_result_system get methods.
 * @}
 * @addtogroup oval_result_system_iterators Iterators
 * @{
 * @ref Oval_result_system iterator methods.
 * @}
 * @addtogroup oval_result_system_eval Evaluators
 * @{
 * @ref Oval_result_system evaluator methods
 * @}
 */
/**
 * @struct oval_result_system
 * Handle: @ref Oval_result_system
 */
struct oval_result_system;
/**
 * @struct oval_result_system_iterator
 * Handle: @ref Oval_result_system iterator
 */
struct oval_result_system_iterator;
/**
 * @}
 */
/**
 * Create new oval_results_model.
 * The new model is bound to a specified oval_definition_model and variable bindings.
 * @param definition_model the specified oval_definition_model.
 * @param syschar_model the array of specified oval_syschar_model(s) terminated by NULL.
 * @ingroup OVALRES
 */
struct oval_results_model *oval_results_model_new(
		struct oval_definition_model *definition_model, struct oval_syschar_model **);

/**
 * Copy an oval_results_model.
 * @ingroup OVALRES
 */
struct oval_results_model *oval_results_model_clone(struct oval_results_model *);

/**
 * free memory allocated to a specified oval results model.
 * @param the specified oval_results model
 * @ingroup OVALRES
 */
void oval_results_model_free(struct oval_results_model *model);


/**
 * oval_results_model_definition_model Return bound object model from an oval_results_model.
 * @param model the specified oval_results_model.
 * @ingroup OVALRES_getters
 */
struct oval_definition_model *oval_results_model_get_definition_model
	(struct oval_results_model *model);

/**
 * Return iterator over reporting systems.
 * @param model the specified results model
 * @ingroup OVALRES_getters
 */
struct oval_result_system_iterator *oval_results_model_get_systems
		(struct oval_results_model *);
/**
 * @ingroup OVALRES_setters
 */
void oval_results_model_add_system(struct oval_results_model *, struct oval_result_system *);

/**
 * load oval results from XML file.
 * @param model the oval_results_model
 * @param source the input source (XML)
 * @param error_handler the error handler
 * @param client_data client data;
 * @ingroup OVALRES_service
 */
struct oval_result_directives *oval_results_model_import
	(struct oval_results_model *, struct oval_import_source *,
			oval_xml_error_handler, void*);

/**
 * Create new OVAL results directives.
 * @ingroup Oval_result_directives
 */
struct oval_result_directives *oval_result_directives_new(void);

/**
 * Destroy OVAL results directives.
 * @ingroup Oval_result_directives
 */
void oval_result_directives_free(struct oval_result_directives *);

/**
 * export oval results to XML file.
 * @param model the oval_results_model
 * @param target the export target stream (XML)
 * @ingroup OVALRES_service
 */
int oval_results_model_export(struct oval_results_model *, struct oval_result_directives *, struct oval_export_target *);
/**
 * @ingroup oval_result_system_iterators
 */
bool                        oval_result_system_iterator_has_more(struct oval_result_system_iterator *);
/**
 * @ingroup oval_result_system_iterators
 */
struct oval_result_system *oval_result_system_iterator_next    (struct oval_result_system_iterator *);
/**
 * @ingroup oval_result_system_iterators
 */
void                       oval_result_system_iterator_free    (struct oval_result_system_iterator *);
/**
 * @ingroup oval_result_system_getters
 */
struct oval_result_definition_iterator *oval_result_system_get_definitions  (struct oval_result_system *);
/**
 * @ingroup oval_result_system_getters
 */
struct oval_result_test_iterator       *oval_result_system_get_tests        (struct oval_result_system *);
/**
 * @ingroup oval_result_system_getters
 */
struct oval_syschar_model              *oval_result_system_get_syschar_model(struct oval_result_system *);
/**
 * @ingroup oval_result_system_getters
 */
struct oval_sysinfo                    *oval_result_system_get_sysinfo      (struct oval_result_system *);
/**
 * @ingroup oval_result_system_getters
 */
void oval_result_system_add_definition(struct oval_result_system *, struct oval_result_definition *);
/**
 * @ingroup oval_result_system_getters
 */
void oval_result_system_add_test       (struct oval_result_system *, struct oval_result_test *);
/**
 * @ingroup oval_result_definition_iterators
 */
bool                            oval_result_definition_iterator_has_more(struct oval_result_definition_iterator *);
/**
 * @ingroup oval_result_definition_iterators
 */
struct oval_result_definition *oval_result_definition_iterator_next    (struct oval_result_definition_iterator *);
/**
 * @ingroup oval_result_definition_iterators
 */
void                           oval_result_definition_iterator_free    (struct oval_result_definition_iterator *);
/**
 * @ingroup oval_result_definition_getters
 */
struct oval_definition           *oval_result_definition_get_definition(struct oval_result_definition *);
/**
 * @ingroup oval_result_definition_getters
 */
struct oval_result_system        *oval_result_definition_get_system    (struct oval_result_definition *);
/**
 * @ingroup oval_result_definition_getters
 */
int                               oval_result_definition_get_instance  (struct oval_result_definition *);
/**
 * @ingroup oval_result_definition_getters
 */
oval_result_t                     oval_result_definition_get_result    (struct oval_result_definition *);
/**
 * @ingroup oval_result_definition_getters
 */
struct oval_message_iterator     *oval_result_definition_get_messages      (struct oval_result_definition *);
/**
 * @ingroup oval_result_definition_getters
 */
struct oval_result_criteria_node *oval_result_definition_get_criteria      (struct oval_result_definition *);
/**
 * @ingroup oval_result_definition_setters
 */
void oval_result_definition_set_result  (struct oval_result_definition *, oval_result_t);
/**
 * @ingroup oval_result_definition_setters
 */
void oval_result_definition_set_instance(struct oval_result_definition *, int);
/**
 * @ingroup oval_result_definition_setters
 */
void oval_result_definition_set_criteria(struct oval_result_definition *, struct oval_result_criteria_node *);
/**
 * @ingroup oval_result_definition_setters
 */
void oval_result_definition_add_message (struct oval_result_definition *, struct oval_message *);
/**
 * @ingroup oval_result_item_iterators
 */
bool                      oval_result_item_iterator_has_more(struct oval_result_item_iterator *);
/**
 * @ingroup oval_result_item_iterators
 */
struct oval_result_item *oval_result_item_iterator_next    (struct oval_result_item_iterator *);
/**
 * @ingroup oval_result_item_iterators
 */
void                     oval_result_item_iterator_free    (struct oval_result_item_iterator *);
/**
 * @ingroup oval_result_item_getters
 */
struct oval_sysdata          *oval_result_item_get_sysdata (struct oval_result_item *);
/**
 * @ingroup oval_result_item_getters
 */
oval_result_t                 oval_result_item_get_result  (struct oval_result_item *);
/**
 * @ingroup oval_result_item_getters
 */
struct oval_message_iterator *oval_result_item_get_messages(struct oval_result_item *);
/**
 * @ingroup oval_result_test_iterators
 */
bool                      oval_result_test_iterator_has_more(struct oval_result_test_iterator *);
/**
 * @ingroup oval_result_test_iterators
 */
struct oval_result_test *oval_result_test_iterator_next    (struct oval_result_test_iterator *);
/**
 * @ingroup oval_result_test_iterators
 */
void                     oval_result_test_iterator_free    (struct oval_result_test_iterator *);
/**
 * @ingroup oval_result_test_getters
 */
struct oval_test                      *oval_result_test_get_test    (struct oval_result_test *);
/**
 * @ingroup oval_result_test_getters
 */
struct oval_result_system             *oval_result_test_get_system  (struct oval_result_test *);
/**
 * @ingroup oval_result_test_getters
 */
oval_result_t                          oval_result_test_get_result  (struct oval_result_test *);
/**
 * @ingroup oval_result_test_getters
 */
int                                    oval_result_test_get_instance(struct oval_result_test *);
/**
 * @ingroup oval_result_test_getters
 */
struct oval_message                   *oval_result_test_get_message (struct oval_result_test *);
/**
 * @ingroup oval_result_test_getters
 */
struct oval_result_item_iterator      *oval_result_test_get_items   (struct oval_result_test *);
/**
 * @ingroup oval_result_test_getters
 */
struct oval_variable_binding_iterator *oval_result_test_get_bindings(struct oval_result_test *);
/**
 * @ingroup oval_result_criteria_node_iterators
 */
bool                               oval_result_criteria_node_iterator_has_more(struct oval_result_criteria_node_iterator *);
/**
 * @ingroup oval_result_criteria_node_iterators
 */
struct oval_result_criteria_node *oval_result_criteria_node_iterator_next    (struct oval_result_criteria_node_iterator *);
/**
 * @ingroup oval_result_criteria_node_iterators
 */
void                              oval_result_criteria_node_iterator_free    (struct oval_result_criteria_node_iterator *);
/**
 * @ingroup oval_result_criteria_node_getters
 */
oval_criteria_node_type_t                  oval_result_criteria_node_get_type    (struct oval_result_criteria_node *);
/**
 * @ingroup oval_result_criteria_node_getters
 */
oval_result_t                              oval_result_criteria_node_get_result  (struct oval_result_criteria_node *);
/**
 * @ingroup oval_result_criteria_node_getters
 */
bool                                       oval_result_criteria_node_get_negate  (struct oval_result_criteria_node *);
/**
 * @ingroup oval_result_criteria_node_getters
 */
oval_operator_t                            oval_result_criteria_node_get_operator(struct oval_result_criteria_node *);//type==NODETYPE_CRITERIA
/**
 * @ingroup oval_result_criteria_node_getters
 */
struct oval_result_criteria_node_iterator *oval_result_criteria_node_get_subnodes(struct oval_result_criteria_node *);//type==NODETYPE_CRITERIA
/**
 * @ingroup oval_result_criteria_node_getters
 */
struct oval_result_test                   *oval_result_criteria_node_get_test    (struct oval_result_criteria_node *);//type==NODETYPE_CRITERION
/**
 * @ingroup oval_result_criteria_node_getters
 */
struct oval_result_definition             *oval_result_criteria_node_get_extends (struct oval_result_criteria_node *);//type==NODETYPE_EXTENDDEF
/**
 * @ingroup oval_result_directives_getters
 */
bool                               oval_result_directives_get_reported(struct oval_result_directives *, oval_result_t);
/**
 * @ingroup oval_result_directives_getters
 */
oval_result_directive_content_t    oval_result_directives_get_content (struct oval_result_directives *, oval_result_t);
/**
 * @ingroup oval_result_directives_setters
 */
void oval_result_directives_set_reported(struct oval_result_directives *, oval_result_t, bool);
/**
 * @ingroup oval_result_directives_setters
 */
void oval_result_directives_set_content (struct oval_result_directives *, oval_result_t, oval_result_directive_content_t);
/**
 * @ingroup Oval_result_system
 */
struct oval_result_system *oval_result_system_new(struct oval_syschar_model *);
/**
 * @ingroup Oval_result_system
 */
struct oval_result_system *oval_result_system_clone(struct oval_result_system *old_system, struct oval_results_model *new_model);
/**
 * @ingroup Oval_result_system
 */
void oval_result_system_free(struct oval_result_system *);
/**
 * @ingroup Oval_result_definition
 */
struct oval_result_definition *oval_result_definition_new(struct oval_result_system *, char *);
/**
 * @ingroup Oval_result_definition
 */
struct oval_result_definition *oval_result_definition_clone(struct oval_result_definition *old_definition, struct oval_result_system *new_system);
/**
 * @ingroup Oval_result_definition
 */
void oval_result_definition_free(struct oval_result_definition *);
/**
 * @ingroup Oval_result_test
 */
struct oval_result_test *oval_result_test_new(struct oval_result_system *, char *);
/**
 * @ingroup Oval_result_test
 */
struct oval_result_test *oval_result_test_clone(struct oval_result_test *old_test, struct oval_result_system *system);
/**
 * @ingroup Oval_result_test
 */
void oval_result_test_free(struct oval_result_test *);
/**
 * @ingroup Oval_result_item
 */
struct oval_result_item *oval_result_item_new(struct oval_result_system *, char *);
/**
 * @ingroup Oval_result_item
 */
struct oval_result_item *oval_result_item_clone(struct oval_result_item *old_item, struct oval_result_system *new_system);
/**
 * @ingroup Oval_result_item
 */
void oval_result_item_free(struct oval_result_item *);
/**
 * @ingroup oval_result_item_setters
 */
void oval_result_item_set_result(struct oval_result_item *, oval_result_t);
/**
 * @ingroup oval_result_item_setters
 */
void oval_result_item_add_message(struct oval_result_item *, struct oval_message *);
/**
 * @ingroup oval_result_item_setters
 */
void oval_result_test_set_result(struct oval_result_test *, oval_result_t);
/**
 * @ingroup oval_result_item_setters
 */
void oval_result_test_set_instance(struct oval_result_test *test, int instance);
/**
 * @ingroup oval_result_item_setters
 */
void oval_result_test_set_message(struct oval_result_test *, struct oval_message *);
/**
 * @ingroup oval_result_item_setters
 */
void oval_result_test_add_item(struct oval_result_test *, struct oval_result_item *);
/**
 * @ingroup oval_result_item_setters
 */
void oval_result_test_add_binding(struct oval_result_test *, struct oval_variable_binding *);
/**
 * @ingroup Oval_result_criteria_node
 */
struct oval_result_criteria_node *oval_result_criteria_node_new(oval_criteria_node_type_t, int, ...);
/**
 * @ingroup Oval_result_criteria_node
 */
struct oval_result_criteria_node *oval_result_criteria_node_clone(struct oval_result_criteria_node *old_node, struct oval_result_system *new_system);
/**
 * @ingroup Oval_result_criteria_node
 */
void oval_result_criteria_node_free(struct oval_result_criteria_node *);
/**
 * @ingroup oval_result_criteria_node_setters
 */
void oval_result_criteria_node_set_result(struct oval_result_criteria_node *, oval_result_t);
/**
 * @ingroup oval_result_criteria_node_setters
 */
void oval_result_criteria_node_set_negate(struct oval_result_criteria_node *, bool);
/**
 * @ingroup oval_result_criteria_node_setters
 */
void oval_result_criteria_node_set_operator(struct oval_result_criteria_node *, oval_operator_t);	//type==NODETYPE_CRITERIA
/**
 * @ingroup oval_result_criteria_node_setters
 */
void oval_result_criteria_node_add_subnode(struct oval_result_criteria_node *, struct oval_result_criteria_node *);	//type==NODETYPE_CRITERIA
/**
 * @ingroup oval_result_criteria_node_setters
 */
void oval_result_criteria_node_set_test(struct oval_result_criteria_node *, struct oval_result_test *);	//type==NODETYPE_CRITERION
/**
 * @ingroup oval_result_criteria_node_setters
 */
void oval_result_criteria_node_set_extends(struct oval_result_criteria_node *, struct oval_result_definition *);	//type==NODETYPE_EXTENDDEF

const char * oval_result_get_text(oval_result_t);

/**
 * @}END OVALRES
 * @}END OVAL
 */

#endif				/* OVAL_RESULTS_H_ */
