/**
 * @file oval_resultCriteriaNode.c
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "oval_results_impl.h"
#include "oval_collection_impl.h"

#define DEBUG_RESULT_CRITERIA_NODE 0

typedef struct oval_result_criteria_node {
	oval_criteria_node_type_enum type;
	oval_result_enum result;
	int negate;
} oval_result_criteria_node_t;

typedef struct oval_result_criteria_node_CRITERIA {
	oval_criteria_node_type_enum type;
	oval_result_enum result;
	int negate;
	oval_operator_enum operator;
	struct oval_collection *subnodes;
} oval_result_criteria_node_CRITERIA_t;

typedef struct oval_result_criteria_node_CRITERION {
	oval_criteria_node_type_enum type;
	oval_result_enum result;
	int negate;
	int variable_instance;
	struct oval_result_test *test;
} oval_result_criteria_node_CRITERION_t;

typedef struct oval_result_criteria_node_EXTENDDEF {
	oval_criteria_node_type_enum type;
	oval_result_enum result;
	int negate;
	int variable_instance;
	struct oval_result_definition *extends;
} oval_result_criteria_node_EXTENDDEF_t;


struct oval_result_criteria_node *_oval_result_criteria_node_new()
{
	oval_result_criteria_node_t *node = (oval_result_criteria_node_t *)
		malloc(sizeof(oval_result_criteria_node_t));
	node->type = NODETYPE_UNKNOWN;
	node->negate = 0;
	return node;
}


struct oval_result_criteria_node *oval_result_criteria_node_new
	(oval_criteria_node_type_enum type, int negate, ...)
{
	oval_result_criteria_node_t *node = NULL;
	va_list ap;
	va_start(ap, negate);
	switch(type){
		case NODETYPE_CRITERIA:{
			//(NODETYPE_CRITERIA, negate, operator);
			node = (oval_result_criteria_node_t *)
				malloc(sizeof(oval_result_criteria_node_CRITERIA_t));
			oval_result_criteria_node_CRITERIA_t *criteria =
				(oval_result_criteria_node_CRITERIA_t *)node;
			criteria->operator = (oval_operator_enum)va_arg(ap, int);
			criteria->subnodes = oval_collection_new();
		};break;
		case NODETYPE_CRITERION:{
			//(NODETYPE_CRITERION, negate, test, variable_instance);
			node = (oval_result_criteria_node_t *)
				malloc(sizeof(oval_result_criteria_node_CRITERION_t));
			oval_result_criteria_node_CRITERION_t *criterion =
				(oval_result_criteria_node_CRITERION_t *)node;
			criterion->test = (struct oval_result_test *)va_arg(ap, void *);
			criterion->variable_instance = va_arg(ap, int);
		};break;
		case NODETYPE_EXTENDDEF:{
			//(NODETYPE_EXTENDDEF, negate, definition, variable_instance);
			node = (oval_result_criteria_node_t *)
				malloc(sizeof(oval_result_criteria_node_EXTENDDEF_t));
			oval_result_criteria_node_EXTENDDEF_t *extenddef =
				(oval_result_criteria_node_EXTENDDEF_t *)node;
			extenddef->extends = (struct oval_result_definition *)va_arg(ap, void *);
			extenddef->variable_instance = va_arg(ap, int);
		}break;
		default: break;
	}
	node->negate = negate;
	node->result = OVAL_RESULT_UNKNOWN;
	node->type   = type;
	va_end(ap);
	return node;
}

void oval_result_criteria_node_free(struct oval_result_criteria_node *node)
{
	switch(node->type){
		case NODETYPE_CRITERIA:{
			oval_result_criteria_node_CRITERIA_t *criteria =
				(oval_result_criteria_node_CRITERIA_t *)node;
			criteria->operator = OPERATOR_UNKNOWN;
			oval_collection_free_items
				(criteria->subnodes, (oscap_destruct_func)oval_criteria_node_free);
		};break;
		case NODETYPE_CRITERION:{
			oval_result_criteria_node_CRITERION_t *criterion =
				(oval_result_criteria_node_CRITERION_t *)node;
			criterion->test = NULL;
		};break;
		case NODETYPE_EXTENDDEF:{
			oval_result_criteria_node_EXTENDDEF_t *extenddef =
				(oval_result_criteria_node_EXTENDDEF_t *)node;
			extenddef->extends = NULL;
		}break;
		default: break;
	}
	node->result = OVAL_RESULT_UNKNOWN;
	node->type   = NODETYPE_UNKNOWN;
	free(node);
}

int oval_iterator_result_criteria_node_has_more(struct
						oval_iterator_result_criteria_node
						*oc_result_criteria_node)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_result_criteria_node);
}

struct oval_result_criteria_node *oval_iterator_result_criteria_node_next(struct
									  oval_iterator_result_criteria_node
									  *oc_result_criteria_node)
{
	return (struct oval_result_criteria_node *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_result_criteria_node);
}

oval_criteria_node_type_enum oval_result_criteria_node_type(struct
							    oval_result_criteria_node
							    *node)
{
	return ((struct oval_result_criteria_node *)node)->type;
}

oval_result_enum oval_result_criteria_node_result(struct
						  oval_result_criteria_node
						  *node)
{
	return ((struct oval_result_criteria_node *)node)->result;
}

oval_operator_enum oval_result_criteria_node_operator(struct
						      oval_result_criteria_node
						      *node)
{
	//type==NODETYPE_CRITERIA
	oval_operator_enum operator = OPERATOR_UNKNOWN;
	if (oval_result_criteria_node_type(node) == NODETYPE_CRITERIA) {
		operator =((struct oval_result_criteria_node_CRITERIA *)node)->
		    operator;
	}
	return operator;
}

struct oval_iterator_criteria_node *oval_result_criteria_node_subnodes(struct
								       oval_result_criteria_node
								       *node)
{
	//type==NODETYPE_CRITERIA
	struct oval_iterator_criteria_node *subnodes = NULL;
	if (oval_result_criteria_node_type(node) == NODETYPE_CRITERIA) {
		struct oval_result_criteria_node_CRITERIA *criteria =
		    (struct oval_result_criteria_node_CRITERIA *)node;
		subnodes = (struct oval_iterator_criteria_node *)
		    oval_collection_iterator(criteria->subnodes);
	}
	return subnodes;
}

struct oval_result_test *oval_result_criteria_node_test(struct
							oval_result_criteria_node
							*node)
{
	//type==NODETYPE_CRITERION
	struct oval_result_test *test = NULL;
	if (oval_result_criteria_node_type(node) == NODETYPE_CRITERION) {
		test =
		    ((struct oval_result_criteria_node_CRITERION *)node)->test;
	}
	return test;
}

struct oval_result_definition *oval_result_criteria_node_extends
	(struct oval_result_criteria_node *node)
{
	//type==NODETYPE_EXTENDDEF
	struct oval_result_definition *extends = NULL;
	if (oval_result_criteria_node_type(node) == NODETYPE_EXTENDDEF) {
		extends =
		    ((struct oval_result_criteria_node_EXTENDDEF *)node)->extends;
	}
	return extends;
}

void set_oval_result_criteria_node_result
	(struct oval_result_criteria_node *node, oval_result_enum result)
{
	node->result = result;
}

void set_oval_result_criteria_node_operator
	(struct oval_result_criteria_node *node, oval_operator_enum operator)
{
	//type==NODETYPE_CRITERIA
	if (oval_result_criteria_node_type(node) == NODETYPE_CRITERIA) {
		((struct oval_result_criteria_node_CRITERIA *)node)->
		    operator = operator;
	}
}

 void add_oval_result_criteria_node_subnode
	(struct oval_result_criteria_node *node, struct oval_result_criteria_node *subnode )
 {
	//type==NODETYPE_CRITERIA
	if (oval_result_criteria_node_type(node) == NODETYPE_CRITERIA) {
		struct oval_result_criteria_node_CRITERIA *criteria
			= ((struct oval_result_criteria_node_CRITERIA *)node);
		oval_collection_add(criteria->subnodes, subnode);
	}
 }

void set_oval_result_criteria_node_test
	(struct oval_result_criteria_node *node, struct oval_result_test *test)
{
	//type==NODETYPE_CRITERION
	if (oval_result_criteria_node_type(node) == NODETYPE_CRITERION) {
		((struct oval_result_criteria_node_CRITERION *)node)->
		test = test;
	}
}

void set_oval_result_criteria_node_extends
(struct oval_result_criteria_node *node, struct oval_result_definition *extends)
{
	//type==NODETYPE_EXTENDDEF
	if (oval_result_criteria_node_type(node) == NODETYPE_EXTENDDEF) {
		extends =
		    ((struct oval_result_criteria_node_EXTENDDEF *)node)->
		    extends = extends;
	}
}

void _oval_result_criteria_consume_subnode
	(struct oval_result_criteria_node *subnode, struct oval_result_criteria_node *node)
{
	add_oval_result_criteria_node_subnode(node, subnode);
}

int _oval_result_criteria_parse
	(xmlTextReaderPtr reader , struct oval_parser_context *context,
		void **args)
{
	struct oval_result_system *system = (struct oval_result_system *)args[0];
	struct oval_result_criteria_node *node = (struct oval_result_criteria_node *)args[1];
	return oval_result_criteria_node_parse
		(reader, context, system,
			(oscap_consumer_func)_oval_result_criteria_consume_subnode, node);
}

int oval_result_criteria_node_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
		struct oval_result_system *system,
		oscap_consumer_func consumer, void *client)
{
	int return_code = 1;
	xmlChar *localName = xmlTextReaderLocalName(reader);

	if(DEBUG_RESULT_CRITERIA_NODE){
		char message[200];*message = '\0';
		sprintf(message,
			"oval_result_criteria_node_parse: parsing <%s>",localName);
		oval_parser_log_debug(context, message);
	}

	struct oval_result_criteria_node *node = NULL;
	if(strcmp(localName, "criteria")==0){
		oval_operator_enum operator = oval_operator_parse
			(reader, "operator",OPERATOR_UNKNOWN);
		int negate = oval_parser_boolean_attribute(reader, "negate", false);
		node = oval_result_criteria_node_new(NODETYPE_CRITERIA, negate, operator);
		void *args[] = {system, node};
		return_code = oval_parser_parse_tag
			(reader, context,
				(oval_xml_tag_parser)_oval_result_criteria_parse, args);
	}else if(strcmp(localName, "criterion")==0){
		xmlChar *test_ref = xmlTextReaderGetAttribute(reader, "test_ref");
		int version  = oval_parser_int_attribute(reader, "version",0);
		int variable_instance = oval_parser_int_attribute(reader, "variable_instance",1);
		int negate = oval_parser_boolean_attribute(reader, "negate", false);
		struct oval_result_test *test = get_oval_result_test_new
			(system, test_ref);
		struct oval_test *ovaltst = oval_result_test_test(test);
		int test_vsn = oval_test_version(ovaltst);
		if(test_vsn!=version){
			char message[200]; *message = '\0';
			sprintf
			(
					message,
					"oval_result_criteria_node_parse: "
					"unmatched test versions\n"
					"    test version = %d: criteria version = %d",
					test_vsn, version
			);
			oval_parser_log_warn(context, message);
		}
		node = oval_result_criteria_node_new
			(NODETYPE_CRITERION, negate, test, variable_instance);
		return_code = 1;
		free(test_ref);
	}else if(strcmp(localName, "extend_definition")==0){
		xmlChar *definition_ref = xmlTextReaderGetAttribute(reader, "definition_ref");
		int variable_instance = oval_parser_int_attribute(reader, "variable_instance",1);
		int negate = oval_parser_boolean_attribute(reader, "negate", false);
		struct oval_result_definition *definition = get_oval_result_definition_new
			(system, definition_ref);
		node = oval_result_criteria_node_new
			(NODETYPE_EXTENDDEF, negate, definition, variable_instance);
		return_code = 1;
		free(definition_ref);
	}else{
		char message[200];*message = '\0';
		sprintf("oval_result_criteria_node_parse: TODO handle criteria node <%s>", localName);
		oval_parser_skip_tag(reader, context);
		return_code = 0;
	}
	if(return_code){
		oval_result_enum result = oval_result_parse(reader, "result", 0);
		set_oval_result_criteria_node_result(node, result);
	}
	(*consumer)(node, client);
	free(localName);
	return return_code;
}

