/**
 * @file oval_results_impl.h
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


#ifndef OVAL_RESULTS_IMPL_H_
#define OVAL_RESULTS_IMPL_H_

#include "api/oval_results.h"
#include "oval_definitions_impl.h"
#include "oval_system_characteristics_impl.h"

struct oval_result_system *oval_result_system_new
	(struct oval_syschar_model *);
void oval_result_system_free(struct oval_result_system *);
int oval_result_system_parse
	(xmlTextReaderPtr , struct oval_parser_context *,
		struct oval_syschar_model *,oscap_consumer_func, void*);
xmlNode *oval_result_system_to_dom
	(struct oval_result_system *,
			struct oval_results_model *,
			struct oval_result_directives *,
			xmlDocPtr, xmlNode *);

struct oval_result_definition *get_oval_result_definition    (struct oval_result_system *, char *);
struct oval_result_definition *get_oval_result_definition_new
	(struct oval_result_system *, struct oval_definition *);
struct oval_result_test *get_oval_result_test    (struct oval_result_system *, char *);
struct oval_result_test *get_oval_result_test_new
	(struct oval_result_system *, struct oval_test *);


//MOVE
struct oval_result_definition *oval_result_definition_new(struct oval_result_system *, char *);
void oval_result_definition_free(struct oval_result_definition *);


int oval_result_definition_parse
	(xmlTextReaderPtr , struct oval_parser_context *,struct oval_result_system *,
		oscap_consumer_func, void*);
struct oval_result_definition *make_result_definition_from_oval_definition(struct oval_result_system *, struct oval_definition *);
xmlNode *oval_result_definition_to_dom
	(struct oval_result_definition *, oval_result_directive_content_t,
			xmlDocPtr, xmlNode *);

struct oval_result_test *oval_result_test_new(struct oval_result_system *, char *);
void oval_result_test_free(struct oval_result_test *);
struct oval_result_test *make_result_test_from_oval_test
	(struct oval_result_system *system, struct oval_test *oval_test);
int oval_result_test_parse_tag
	(xmlTextReaderPtr , struct oval_parser_context *,
			struct oval_result_system *,
			oscap_consumer_func, void *);
xmlNode *oval_result_test_to_dom
	(struct oval_result_test *, xmlDocPtr, xmlNode *);

int oval_result_directives_parse_tag
	(xmlTextReaderPtr, struct oval_parser_context *, struct oval_result_directives *);
int oval_result_directives_to_dom
	(struct oval_result_directives *, xmlDoc *, xmlNode *);

struct oval_result_item *oval_result_item_new
	(struct oval_result_system *, char *);
void oval_result_item_free(struct oval_result_item *);
int oval_result_item_parse_tag
	(xmlTextReaderPtr , struct oval_parser_context *,
		struct oval_result_system *,
		oscap_consumer_func, void*);
xmlNode *oval_result_item_to_dom
	(struct oval_result_item *, xmlDocPtr , xmlNode *);

void oval_result_item_set_result(struct oval_result_item *, oval_result_t);
void oval_result_item_add_message(struct oval_result_item *, struct oval_message *);

struct oval_result_test *oval_result_test_new();
void oval_result_test_free(struct oval_result_test *);

void oval_result_test_set_result(struct oval_result_test *, oval_result_t);
void oval_result_test_set_message(struct oval_result_test *, struct oval_message *);
void oval_result_test_add_item(struct oval_result_test *, struct oval_result_item *);
void oval_result_test_add_binding(struct oval_result_test *, struct oval_variable_binding *);


struct oval_result_criteria_node *oval_result_criteria_node_new
	(oval_criteria_node_type_t, int, ...);
void oval_result_criteria_node_free(struct oval_result_criteria_node *);
struct oval_result_criteria_node *make_result_criteria_node_from_oval_criteria_node
	(struct oval_result_system *, struct oval_criteria_node *);

int oval_result_criteria_node_parse
	(xmlTextReaderPtr , struct oval_parser_context *,
		struct oval_result_system *, oscap_consumer_func, void*);
xmlNode *oval_result_criteria_node_to_dom
	(struct oval_result_criteria_node *, xmlDocPtr, xmlNode *);

void oval_result_criteria_node_set_result(struct oval_result_criteria_node *, oval_result_t);
void oval_result_criteria_node_set_negate(struct oval_result_criteria_node *, bool);
void oval_result_criteria_node_set_operator(struct oval_result_criteria_node *, oval_operator_t);	//type==NODETYPE_CRITERIA
void oval_result_criteria_node_add_subnode(struct oval_result_criteria_node *, struct oval_result_criteria_node *);	//type==NODETYPE_CRITERIA
void oval_result_criteria_node_set_test(struct oval_result_criteria_node *, struct oval_result_test *);	//type==NODETYPE_CRITERION
void oval_result_criteria_node_set_extends(struct oval_result_criteria_node *, struct oval_result_definition *);	//type==NODETYPE_EXTENDDEF

oval_result_t oval_result_parse
	(xmlTextReaderPtr, char *, oval_result_t);

#endif				/* OVAL_RESULTS_IMPL_H_ */
