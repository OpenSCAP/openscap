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
#include "oval_results_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_result_criteria_node {
	oval_criteria_node_type_enum type;
	oval_result_enum result;
} oval_result_criteria_node_t;

typedef struct oval_result_criteria_node_CRITERIA {
	oval_criteria_node_type_enum type;
	oval_result_enum result;
	oval_operator_enum operator         ;	//type==NODETYPE_CRITERIA
	struct oval_collection *subnodes;	//type==NODETYPE_CRITERIA
} oval_result_criteria_node_CRITERIA_t;

typedef struct oval_result_criteria_node_CRITERION {
	oval_criteria_node_type_enum type;
	oval_result_enum result;
	struct oval_result_test *test;	//type==NODETYPE_CRITERION
} oval_result_criteria_node_CRITERION_t;

typedef struct oval_result_criteria_node_EXTENDDEF {
	oval_criteria_node_type_enum type;
	oval_result_enum result;
	struct oval_result *extends;	//type==NODETYPE_EXTENDDEF
} oval_result_criteria_node_EXTENDDEF_t;

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
	struct oval_result_criteria_node_CRITERIA *criteria =
	    (struct oval_result_criteria_node_CRITERIA *)node;
	return (struct oval_iterator_criteria_node *)
	    oval_collection_iterator(criteria->subnodes);
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
}

struct oval_result *oval_result_criteria_node_extends(struct
						      oval_result_criteria_node
						      *node)
{
	//type==NODETYPE_EXTENDDEF
	struct oval_result *result = NULL;
	if (oval_result_criteria_node_type(node) == NODETYPE_EXTENDDEF) {
		result =
		    ((struct oval_result_criteria_node_EXTENDDEF *)node)->
		    extends;
	}
	return result;
}				//type==NODETYPE_EXTENDDEF
