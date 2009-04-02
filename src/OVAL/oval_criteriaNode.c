/*
 * oval_criteria_node.c
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
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
			    malloc(sizeof(oval_criteria_node_CRITERIA_t));
			((struct oval_criteria_node_CRITERIA *)node)->
			    operator = OPERATOR_UNKNOWN;
			((struct oval_criteria_node_CRITERIA *)node)->subnodes =
			    oval_collection_new();
		} break;
	case NODETYPE_CRITERION:{
			node =
			    (struct oval_criteria_node *)
			    malloc(sizeof(oval_criteria_node_CRITERION_t));
			((struct oval_criteria_node_CRITERION *)node)->test =
			    NULL;
		} break;
	case NODETYPE_EXTENDDEF:{
			node =
			    (struct oval_criteria_node *)
			    malloc(sizeof(oval_criteria_node_EXTENDDEF_t));
			((struct oval_criteria_node_EXTENDDEF *)node)->
			    definition = NULL;
		}
	}
	node->type = type;
	node->negate = 0;
	node->comment = NULL;
	return node;
}

void oval_criteria_node_free(struct oval_criteria_node *node)
{
	oval_criteria_node_type_enum type = node->type;
	if (node->comment != NULL)
		free(node->comment);
	switch (type) {
	case NODETYPE_CRITERIA:{
			struct oval_collection *subnodes =
			    ((struct oval_criteria_node_CRITERIA *)node)->
			    subnodes;
			void free_node(void *null) {
				oval_criteria_node_free(node);
			}
			oval_collection_free_items(subnodes, &free_node);
		} break;
	case NODETYPE_CRITERION:{
			//NOOP
		}
		break;
	case NODETYPE_EXTENDDEF:{
			//NOOP
		}
	}
	if (node->comment != NULL)
		free(node->comment);
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
				    char *comment)
{
	node->comment = comment;
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

int _oval_criteria_subnode_consumer(xmlTextReaderPtr reader,
				    struct oval_parser_context *context,
				    void *user)
{
	struct oval_criteria_node_CRITERIA *criteria =
	    (struct oval_criteria_node_CRITERIA *)user;
	void consumer(struct oval_criteria_node *subnode, void *null) {
		add_oval_criteria_node_subnodes((struct oval_criteria_node *)
						criteria, subnode);
	}
	int return_code =
	    oval_criteria_parse_tag(reader, context, &consumer, NULL);
	return return_code;
}

//typedef void (*oval_criteria_consumer)(struct oval_criteria_node *node, void*);
int oval_criteria_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context,
			    oval_criteria_consumer consumer, void *user)
{
	xmlChar *tagname = xmlTextReaderName(reader);
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
		char *comment = xmlTextReaderGetAttribute(reader, "comment");
		set_oval_criteria_node_comment(node, comment);
		set_oval_criteria_node_negate(node,
					      oval_parser_boolean_attribute
					      (reader, "negate", 0));
		oval_criteria_node_type_enum type =
		    oval_criteria_node_type(node);
		return_code = 1;
		switch (type) {
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
				    xmlTextReaderGetAttribute(reader,
							      "test_ref");
				struct oval_object_model *model =
				    oval_parser_context_model(context);
				struct oval_test *test =
				    get_oval_test_new(model, test_ref);
				set_oval_criteria_node_test(node, test);
			} break;
		case NODETYPE_EXTENDDEF:{
				char *definition_ref =
				    xmlTextReaderGetAttribute(reader,
							      "definition_ref");
				struct oval_object_model *model =
				    oval_parser_context_model(context);
				struct oval_definition *definition =
				    get_oval_definition_new(model,
							    definition_ref);
				set_oval_criteria_node_definition(node,
								  definition);
			}
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
				 int index)
{
	char *nodetype;
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
	char nxtindent[100];
	*nxtindent = 0;
	strcat(nxtindent, indent);
	if (index == 0) {
		strcat(nxtindent, nodetype);
		strcat(nxtindent, ".");
	} else {
		strcat(nxtindent, nodetype);
		strcat(nxtindent, "[");
		char itoad[10];
		*itoad = 0;
		itoa(index, itoad, 10);
		strcat(nxtindent, itoad);
		strcat(nxtindent, "].");
	}
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
			for (index = 1;
			     oval_collection_iterator_has_more(subnodes);
			     index++) {
				void *subnode =
				    oval_collection_iterator_next(subnodes);
				oval_criteria_node_to_print((struct
							     oval_criteria_node
							     *)subnode,
							    nxtindent, index);
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
	}
}
