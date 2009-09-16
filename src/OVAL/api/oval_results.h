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
#include <libxml/xmlreader.h>

typedef enum {
	OVAL_RESULT_INVALID        = 0,
	OVAL_RESULT_TRUE           = 1,
	OVAL_RESULT_FALSE          = 2,
	OVAL_RESULT_UNKNOWN        = 3,
	OVAL_RESULT_ERROR          = 4,
	OVAL_RESULT_NOT_EVALUATED  = 5,
	OVAL_RESULT_NOT_APPLICABLE = 6
} oval_result_enum;

typedef enum {
	OVAL_DIRECTIVE_CONTENT_UNKNOWN = 0,
	OVAL_DIRECTIVE_CONTENT_THIN    = 1,
	OVAL_DIRECTIVE_CONTENT_FULL    = 2
} oval_result_directive_content_enum;


struct oval_result_definition;
struct oval_iterator_result_definition;

struct oval_result_item;
struct oval_iterator_result_item;

struct oval_result_test;
struct oval_iterator_result_test;

struct oval_result_criteria_node;
struct oval_iterator_result_criteria_node;

struct oval_result_directives;

struct oval_result_system;
struct oval_iterator_result_system;

int oval_iterator_result_system_has_more
	(struct oval_iterator_result_system *);
struct oval_result_system *oval_iterator_result_system_next
	(struct oval_iterator_result_system *);

struct oval_iterator_result_definition *oval_result_system_definitions
	(struct oval_result_system *);
struct oval_iterator_result_test *oval_result_system_tests
	(struct oval_result_system *);
struct oval_syschar_model *oval_result_system_syschar_model
	(struct oval_result_system *);
struct oval_sysinfo *oval_result_system_sysinfo
	(struct oval_result_system *);

void add_oval_result_system_definition
	(struct oval_result_system *, struct oval_result_definition *);
void add_oval_result_system_test
	(struct oval_result_system *, struct oval_result_test *);

int oval_iterator_result_definition_has_more
	(struct oval_iterator_result_definition *);
struct oval_result_definition *oval_iterator_result_definition_next
	(struct oval_iterator_result_definition *);

struct oval_definition *oval_result_definition_definition
	(struct oval_result_definition *);
struct oval_result_system *oval_result_definition_system
	(struct oval_result_definition *);
int oval_result_definition_instance
	(struct oval_result_definition *);

oval_result_enum oval_result_definition_result
	(struct oval_result_definition *);

struct oval_iterator_message *oval_result_definition_messages
	(struct oval_result_definition *);

struct oval_result_criteria_node *oval_result_definition_criteria
	(struct oval_result_definition *);

void set_oval_result_definition_result
	(struct oval_result_definition *,
			oval_result_enum);

void set_oval_result_definition_instance
	(struct oval_result_definition *, int);

void set_oval_result_definition_criteria
	(struct oval_result_definition *,
		struct oval_result_criteria_node *);

void add_oval_result_definition_message
	(struct oval_result_definition *,
		struct oval_message *);


int oval_iterator_result_test_has_more
	(struct oval_iterator_result_test *);
struct oval_result_test *oval_iterator_result_test_next
	(struct oval_iterator_result_test *);

int oval_iterator_result_item_has_more(struct oval_iterator_result_item *);
struct oval_result_item *oval_iterator_result_item_next(struct
							oval_iterator_result_item
							*);

struct oval_sysdata *oval_result_item_sysdata(struct oval_result_item *);
oval_result_enum oval_result_item_result(struct oval_result_item *);
struct oval_iterator_message *oval_result_item_messages(struct oval_result_item *);

int oval_iterator_result_test_has_more(struct oval_iterator_result_test *);
struct oval_result_test *oval_iterator_result_test_next(struct
							oval_iterator_result_test
							*);

struct oval_test *oval_result_test_test(struct oval_result_test *);
struct oval_result_system *oval_result_test_system(struct oval_result_test *);
oval_result_enum oval_result_test_result(struct oval_result_test *);
int oval_result_test_instance(struct oval_result_test *);
struct oval_message *oval_result_test_message(struct oval_result_test *);
struct oval_iterator_result_item *oval_result_test_items(struct oval_result_test *);
struct oval_iterator_variable_binding *oval_result_test_bindings(struct oval_result_test *);

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
bool oval_result_criteria_node_negate
	(struct oval_result_criteria_node *);

oval_operator_enum oval_result_criteria_node_operator(struct oval_result_criteria_node *);	//type==NODETYPE_CRITERIA
struct oval_iterator_result_criteria_node *oval_result_criteria_node_subnodes
	(struct oval_result_criteria_node *);	//type==NODETYPE_CRITERIA
struct oval_result_test *oval_result_criteria_node_test(struct oval_result_criteria_node *);	//type==NODETYPE_CRITERION
struct oval_result_definition *oval_result_criteria_node_extends(struct oval_result_criteria_node *);	//type==NODETYPE_EXTENDDEF

bool oval_result_directive_reported
	(struct oval_result_directives *, oval_result_enum);
oval_result_directive_content_enum oval_result_directive_content
	(struct oval_result_directives *, oval_result_enum);

void set_oval_result_directive_reported
	(struct oval_result_directives *, oval_result_enum, bool);
void set_oval_result_directive_content
	(struct oval_result_directives *, oval_result_enum, oval_result_directive_content_enum);


#endif				/* OVAL_RESULTS_H_ */
