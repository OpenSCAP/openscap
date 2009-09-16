/**
 * @file oval_criteriaNode.c
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
#include <string.h>
#include "oval_definitions_impl.h"
#include "oval_collection_impl.h"
#include "oval_agent_api_impl.h"

typedef struct oval_criteria_node {
	oval_criteria_node_type_enum type;
	int negate;
	char *comment;
} oval_criteria_node_t;

typedef struct oval_criteria_node_CRITERIA {
	oval_criteria_node_type_enum type;
	int negate;
	char *comment;
	oval_operator_enum operator;	//type==NODETYPE_CRITERIA
	struct oval_collection *subnodes;	//type==NODETYPE_CRITERIA
} oval_criteria_node_CRITERIA_t;

typedef struct oval_criteria_node_CRITERION {
	oval_criteria_node_type_enum type;
	int negate;
	char *comment;
	struct oval_test *test;	//type==NODETYPE_CRITERION
} oval_criteria_node_CRITERION_t;

typedef struct oval_criteria_node_EXTENDDEF {
	oval_criteria_node_type_enum type;
	int negate;
	char *comment;
	struct oval_definition *definition;	//type==NODETYPE_EXTENDDEF
} oval_criteria_node_EXTENDDEF_t;

int oval_iterator_criteria_node_has_more(struct oval_iterator_criteria_node
					 *oc_criteria_node)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_criteria_node);
}

struct oval_criteria_node *oval_iterator_criteria_node_next(struct
							    oval_iterator_criteria_node
							    *oc_criteria_node)
{
	return (struct oval_criteria_node *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_criteria_node);
}

oval_criteria_node_type_enum oval_criteria_node_type(struct oval_criteria_node
						     *node)
{
	return ((struct oval_criteria_node *)node)->type;
}

int oval_criteria_node_negate(struct oval_criteria_node *node)
{
	return ((struct oval_criteria_node *)node)->negate;
}

char *oval_criteria_node_comment(struct oval_criteria_node *node)
{
	return ((struct oval_criteria_node *)node)->comment;
}

oval_operator_enum oval_criteria_node_operator(struct oval_criteria_node *node)
{
	//type==NODETYPE_CRITERIA
	return ((struct oval_criteria_node_CRITERIA *)node)->operator;
}

struct oval_iterator_criteria_node *oval_criteria_node_subnodes(struct
								oval_criteria_node
								*node)
{
	//type==NODETYPE_CRITERIA
	struct oval_criteria_node_CRITERIA *criteria =
	    (struct oval_criteria_node_CRITERIA *)node;
	return (struct oval_iterator_criteria_node *)
	    oval_collection_iterator(criteria->subnodes);
}

struct oval_test *oval_criteria_node_test(struct oval_criteria_node *node)
{
	//type==NODETYPE_CRITERION
	return ((struct oval_criteria_node_CRITERION *)node)->test;
}

struct oval_definition *oval_criteria_node_definition(struct oval_criteria_node
						      *node)
{
	//type==NODETYPE_EXTENDDEF
	return ((struct oval_criteria_node_EXTENDDEF *)node)->definition;
}

struct oval_criteria_node *oval_criteria_node_new(oval_criteria_node_type_enum
						  type)
{
	struct oval_criteria_node *node;
	switch (type) {
	case NODETYPE_CRITERIA:{
			node =
			    (struct oval_criteria_node *)
			    calloc(1, sizeof(oval_criteria_node_CRITERIA_t));
			((struct oval_criteria_node_CRITERIA *)node)->
			    operator = OPERATOR_UNKNOWN;
			((struct oval_criteria_node_CRITERIA *)node)->subnodes =
			    oval_collection_new();
		} break;
	case NODETYPE_CRITERION:{
			node =
			    (struct oval_criteria_node *)
			    calloc(1, sizeof(oval_criteria_node_CRITERION_t));
			((struct oval_criteria_node_CRITERION *)node)->test =
			    NULL;
		} break;
	case NODETYPE_EXTENDDEF:{
			node =
			    (struct oval_criteria_node *)
			    calloc(1, sizeof(oval_criteria_node_EXTENDDEF_t));
			((struct oval_criteria_node_EXTENDDEF *)node)->
			    definition = NULL;
		}break;
	case NODETYPE_UNKNOWN:
	default: return NULL;
	}
	node->type = type;
	node->negate = 0;
	node->comment = NULL;
	return node;
}

void oval_criteria_node_free(struct oval_criteria_node *node)
{
	oval_criteria_node_type_enum type = node->type;
	switch (type) {
	case NODETYPE_CRITERIA:{
			struct oval_criteria_node_CRITERIA *criteria = (struct oval_criteria_node_CRITERIA *)node;
			oval_collection_free_items(criteria->subnodes, (oscap_destruct_func)oval_criteria_node_free);
			criteria->subnodes = NULL;
		} break;
	case NODETYPE_CRITERION:{
			//NOOP
		}
		break;
	case NODETYPE_EXTENDDEF:{
			//NOOP
		}
	case NODETYPE_UNKNOWN:{
			//NOOP
		}
	}
	if (node->comment != NULL){
		free(node->comment);
	}
	node->comment = NULL;
	free(node);
}

void set_oval_criteria_node_type(struct oval_criteria_node *node,
				 oval_criteria_node_type_enum type)
{
	node->type = type;
}

void set_oval_criteria_node_negate(struct oval_criteria_node *node, int negate)
{
	node->negate = negate;
}

void set_oval_criteria_node_comment(struct oval_criteria_node *node,
				    char *comm)
{
	if(node->comment!=NULL)free(node->comment);
	node->comment = comm==NULL?NULL:strdup(comm);
}

void set_oval_criteria_node_operator(struct oval_criteria_node *node,
				     oval_operator_enum operator)
{
	//type==NODETYPE_CRITERIA
	struct oval_criteria_node_CRITERIA *criteria =
	    (struct oval_criteria_node_CRITERIA *)node;
	criteria->operator = operator;
}

void add_oval_criteria_node_subnodes(struct oval_criteria_node *node,
				     struct oval_criteria_node *subnode)
{
	//type==NODETYPE_CRITERIA
	struct oval_criteria_node_CRITERIA *criteria =
	    (struct oval_criteria_node_CRITERIA *)node;
	oval_collection_add(criteria->subnodes, (void *)subnode);
}

void set_oval_criteria_node_test(struct oval_criteria_node *node,
				 struct oval_test *test)
{
	//type==NODETYPE_CRITERION
	struct oval_criteria_node_CRITERION *criterion =
	    (struct oval_criteria_node_CRITERION *)node;
	criterion->test = test;
}

void set_oval_criteria_node_definition(struct oval_criteria_node *node,
				       struct oval_definition *definition)
{
	//type==NODETYPE_EXTENDDEF
	struct oval_criteria_node_EXTENDDEF *extenddef =
	    (struct oval_criteria_node_EXTENDDEF *)node;
	extenddef->definition = definition;
}

void _oval_criteria_subnode_consume(struct oval_criteria_node *subnode, void *criteria) {
	add_oval_criteria_node_subnodes((struct oval_criteria_node *)
					criteria, subnode);
}
int _oval_criteria_subnode_consumer(xmlTextReaderPtr reader,
				    struct oval_parser_context *context,
				    void *user)
{
	struct oval_criteria_node_CRITERIA *criteria =
	    (struct oval_criteria_node_CRITERIA *)user;
	int return_code =
	    oval_criteria_parse_tag(reader, context, &_oval_criteria_subnode_consume, criteria);
	return return_code;
}

//typedef void (*oval_criteria_consumer)(struct oval_criteria_node *node, void*);
int oval_criteria_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context,
			    oval_criteria_consumer consumer, void *user)
{
	char *tagname = (char*) xmlTextReaderName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	oval_criteria_node_type_enum type = NODETYPE_UNKNOWN;
	if (strcmp(tagname, "criteria") == 0)
		type = NODETYPE_CRITERIA;
	else if (strcmp(tagname, "criterion") == 0)
		type = NODETYPE_CRITERION;
	else if (strcmp(tagname, "extend_definition") == 0)
		type = NODETYPE_EXTENDDEF;
	int return_code;
	if (type != NODETYPE_UNKNOWN) {
		struct oval_criteria_node *node = oval_criteria_node_new(type);
		node->type = type;
		char *comm = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
		if(comm!=NULL){
			set_oval_criteria_node_comment(node, comm);
			free(comm);comm=NULL;
		}
		set_oval_criteria_node_negate(node,
					      oval_parser_boolean_attribute
					      (reader, "negate", 0));
		return_code = 1;
		switch (oval_criteria_node_type(node)) {
		case NODETYPE_CRITERIA:{
				struct oval_criteria_node_CRITERIA *criteria =
				    (struct oval_criteria_node_CRITERIA *)node;
				oval_operator_enum operator =
				    oval_operator_parse(reader, "operator",
							OPERATOR_AND);
				set_oval_criteria_node_operator((struct
								 oval_criteria_node
								 *)criteria,
								operator);
				return_code =
				    oval_parser_parse_tag(reader, context,
							  &_oval_criteria_subnode_consumer,
							  criteria);
			} break;
		case NODETYPE_CRITERION:{
				char *test_ref =
				    (char *) xmlTextReaderGetAttribute(reader,
							      BAD_CAST "test_ref");
				struct oval_object_model *model =
				    oval_parser_context_model(context);
				struct oval_test *test = get_oval_test_new(model, test_ref);
				free(test_ref);test_ref=NULL;
				set_oval_criteria_node_test(node, test);
			} break;
		case NODETYPE_EXTENDDEF:{
				char *definition_ref =
				    (char *) xmlTextReaderGetAttribute(reader,
							      BAD_CAST "definition_ref");
				struct oval_object_model *model =
				    oval_parser_context_model(context);
				struct oval_definition *definition =
				    get_oval_definition_new(model, definition_ref);
				set_oval_criteria_node_definition(node,
								  definition);
				free(definition_ref);definition_ref=NULL;
			}
		case NODETYPE_UNKNOWN: break;
		}
		//oval_parser_parse_tag(reader, context,&_oval_criteria_parse_tag,node);
		(*consumer) (node, user);
	} else {
		return_code = 0;
		printf("NOTICE::oval_criteria_parse_tag::node type unknown");
		oval_parser_skip_tag(reader, context);
	}
	free(tagname);
	free(namespace);
	return return_code;
}

void oval_criteria_node_to_print(struct oval_criteria_node *node, char *indent,
				 int idx)
{
	char *nodetype = NULL;
	char nxtindent[100];

	switch (node->type) {
	case NODETYPE_CRITERIA:
		nodetype = "CRITERIA";
		break;
	case NODETYPE_CRITERION:
		nodetype = "CRITERION";
		break;
	case NODETYPE_EXTENDDEF:
		nodetype = "EXTEND_DEFINITION";
		break;
	case NODETYPE_UNKNOWN:
		nodetype = "UNKNOWN_CRITNODE";
		break;
	}

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%s%s.", indent, nodetype);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%s%s[%d].", indent, nodetype, idx);

	printf("%sCOMMENT = %s\n", nxtindent, node->comment);
	printf("%sNEGATE  = %d\n", nxtindent, node->negate);
	switch (node->type) {
	case NODETYPE_CRITERIA:{
			struct oval_criteria_node_CRITERIA *criteria =
			    (struct oval_criteria_node_CRITERIA *)node;
			printf("%sOPERATOR = %d\n", nxtindent,
			       criteria->operator);
			struct oval_iterator *subnodes =
			    oval_collection_iterator(criteria->subnodes);
			for (idx = 1;
			     oval_collection_iterator_has_more(subnodes);
			     idx++) {
				void *subnode =
				    oval_collection_iterator_next(subnodes);
				oval_criteria_node_to_print((struct
							     oval_criteria_node
							     *)subnode,
							    nxtindent, idx);
			}
		} break;
	case NODETYPE_CRITERION:{
			struct oval_criteria_node_CRITERION *criterion =
			    (struct oval_criteria_node_CRITERION *)node;
			if (criterion->test != NULL)
				oval_test_to_print(criterion->test, nxtindent,
						   0);
			else
				printf("%sTEST    = <<NONE>>\n", nxtindent);
		}
		break;
	case NODETYPE_EXTENDDEF:{
			struct oval_criteria_node_EXTENDDEF *extenddef =
			    (struct oval_criteria_node_EXTENDDEF *)node;
			if (extenddef->definition != NULL)
				oval_definition_to_print(extenddef->definition,
							 nxtindent, 0);
			else
				printf("%sDEFINITION = <<NONE>>\n", nxtindent);
		}
		break;
	case NODETYPE_UNKNOWN: break;
	}
}

xmlNode *_oval_CRITERIA_to_dom
	(struct oval_criteria_node *cnode, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *criteria_node = xmlNewChild(parent, ns_definitions, "criteria", NULL);

	oval_operator_enum operator = oval_criteria_node_operator(cnode);
	if(operator!=OPERATOR_AND)
		xmlNewProp(criteria_node, "operator", oval_operator_text(operator));

	struct oval_iterator_criteria_node *subnodes
		= oval_criteria_node_subnodes(cnode);
	while(oval_iterator_criteria_node_has_more(subnodes)){
		struct oval_criteria_node *subnode
			= oval_iterator_criteria_node_next(subnodes);
		oval_criteria_node_to_dom(subnode, doc, criteria_node);
	}

	return criteria_node;
}

xmlNode *_oval_CRITERION_to_dom
	(struct oval_criteria_node *cnode, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *criterion_node = xmlNewChild(parent, ns_definitions, "criterion", NULL);

	struct oval_test *test = oval_criteria_node_test(cnode);
	char *test_ref = oval_test_id(test);
	xmlNewProp(criterion_node, "test_ref", test_ref);

	return criterion_node;
}

xmlNode *_oval_EXTENDDEF_to_dom
	(struct oval_criteria_node *cnode, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *extenddef_node = xmlNewChild(parent, ns_definitions, "extend_definition", NULL);

	struct oval_definition *definition = oval_criteria_node_definition(cnode);
	char *definition_ref = oval_definition_id(definition);
	xmlNewProp(extenddef_node, "definition_ref", definition_ref);

	return extenddef_node;
}


xmlNode *oval_criteria_node_to_dom
	(struct oval_criteria_node *cnode, xmlDoc *doc, xmlNode *parent)
{
	xmlNode *criteria_node;
	switch(oval_criteria_node_type(cnode))
	{
	case NODETYPE_CRITERIA : criteria_node = _oval_CRITERIA_to_dom
		(cnode, doc, parent);break;
	case NODETYPE_CRITERION: criteria_node = _oval_CRITERION_to_dom
		(cnode, doc, parent);break;
	case NODETYPE_EXTENDDEF: criteria_node = _oval_EXTENDDEF_to_dom
		(cnode, doc, parent);break;
	default: criteria_node = NULL; break;
	}

	bool negate = oval_criteria_node_negate(cnode);
	if(negate)xmlNewProp(criteria_node, "negate", "true");

	char *comment  = oval_criteria_node_comment(cnode);
	if(comment)xmlNewProp(criteria_node, "comment", comment);

	return criteria_node;
}

