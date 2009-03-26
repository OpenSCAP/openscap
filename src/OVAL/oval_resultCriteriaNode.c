/*
 * oval_result_criteria_node.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_results_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct oval_result_criteria_node_s{
	oval_criteria_node_type_enum type   ;
	oval_result_enum result             ;
} oval_result_criteria_node_t;

typedef struct oval_result_criteria_node_CRITERIA_s{
	oval_criteria_node_type_enum type   ;
	oval_result_enum result             ;
	oval_operator_enum operator         ;//type==NODETYPE_CRITERIA
	struct oval_collection_s *subnodes  ;//type==NODETYPE_CRITERIA
} oval_result_criteria_node_CRITERIA_t;

typedef struct oval_result_criteria_node_CRITERION_s{
	oval_criteria_node_type_enum type   ;
	oval_result_enum result             ;
	struct oval_result_test_s *test           ;//type==NODETYPE_CRITERION
} oval_result_criteria_node_CRITERION_t;

typedef struct oval_result_criteria_node_EXTENDDEF_s{
	oval_criteria_node_type_enum type   ;
	oval_result_enum result             ;
	struct oval_result_s *extends             ;//type==NODETYPE_EXTENDDEF
} oval_result_criteria_node_EXTENDDEF_t;

int   oval_iterator_result_criteria_node_has_more      (struct oval_iterator_result_criteria_node_s *oc_result_criteria_node){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_result_criteria_node);
}
struct oval_result_criteria_node_s *oval_iterator_result_criteria_node_next(struct oval_iterator_result_criteria_node_s *oc_result_criteria_node){
	return (struct oval_result_criteria_node_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_result_criteria_node);
}


oval_criteria_node_type_enum oval_result_criteria_node_type    (struct oval_result_criteria_node_s *node){
	return ((struct oval_result_criteria_node_s*)node)->type;
}
oval_result_enum oval_result_criteria_node_result             (struct oval_result_criteria_node_s *node){
	return ((struct oval_result_criteria_node_s*)node)->result;
}
oval_operator_enum oval_result_criteria_node_operator         (struct oval_result_criteria_node_s *node){
	//type==NODETYPE_CRITERIA
	oval_operator_enum operator = OPERATOR_UNKNOWN;
	if(oval_result_criteria_node_type(node)==NODETYPE_CRITERIA){
		operator = ((struct oval_result_criteria_node_CRITERIA_s*)node)->operator;
	}
	return operator;
}
struct oval_iterator_criteria_node_s *oval_result_criteria_node_subnodes(struct oval_result_criteria_node_s *node){
	//type==NODETYPE_CRITERIA
	struct oval_result_criteria_node_CRITERIA_s *criteria = (struct oval_result_criteria_node_CRITERIA_s*)node;
	return (struct oval_iterator_criteria_node_s*)oval_collection_iterator(criteria->subnodes);
}
struct oval_result_test_s *oval_result_criteria_node_test(struct oval_result_criteria_node_s *node){
	//type==NODETYPE_CRITERION
	struct oval_result_test_s *test = NULL;
	if(oval_result_criteria_node_type(node)==NODETYPE_CRITERION){
		test = ((struct oval_result_criteria_node_CRITERION_s*)node)->test;
	}
}
struct oval_result_s *oval_result_criteria_node_extends(struct oval_result_criteria_node_s *node){
	//type==NODETYPE_EXTENDDEF
	struct oval_result_s *result = NULL;
	if(oval_result_criteria_node_type(node)==NODETYPE_EXTENDDEF){
		result = ((struct oval_result_criteria_node_EXTENDDEF_s*)node)->extends;
	}
	return result;
}//type==NODETYPE_EXTENDDEF
