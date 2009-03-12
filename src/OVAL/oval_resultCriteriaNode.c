/*
 * oval_resultCriteriaNode.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_results_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_resultCriteriaNode_s{
	Oval_criteriaNode_type_enum type    ;
	Oval_result_enum result             ;
} Oval_resultCriteriaNode_t;

typedef struct Oval_resultCriteriaNode_CRITERIA_s{
	Oval_criteriaNode_type_enum type    ;
	Oval_result_enum result             ;
	Oval_operator_enum operator         ;//type==NODETYPE_CRITERIA
	Oval_criteriaNode* subnodes         ;//type==NODETYPE_CRITERIA
} Oval_resultCriteriaNode_CRITERIA_t;

typedef struct Oval_resultCriteriaNode_CRITERION_s{
	Oval_criteriaNode_type_enum type    ;
	Oval_result_enum result             ;
	Oval_resultTest test                ;//type==NODETYPE_CRITERION
} Oval_resultCriteriaNode_CRITERION_t;

typedef struct Oval_resultCriteriaNode_EXTENDDEF_s{
	Oval_criteriaNode_type_enum type    ;
	Oval_result_enum result             ;
	Oval_result extends                 ;//type==NODETYPE_EXTENDDEF
} Oval_resultCriteriaNode_EXTENDDEF_t;

typedef Oval_resultCriteriaNode_t* Oval_resultCriteriaNode_ptr;
typedef Oval_resultCriteriaNode_CRITERIA_t* Oval_resultCriteriaNode_CRITERIA_ptr;
typedef Oval_resultCriteriaNode_CRITERION_t* Oval_resultCriteriaNode_CRITERION_ptr;
typedef Oval_resultCriteriaNode_EXTENDDEF_t* Oval_resultCriteriaNode_EXTENDDEF_ptr;

OvalCollection_resultCriteriaNode newOvalCollection_resultCriteriaNode(Oval_resultCriteriaNode* resultCriteriaNode_array){
	return (OvalCollection_resultCriteriaNode)newOvalCollection((OvalCollection_target*)resultCriteriaNode_array);
}
int   OvalCollection_resultCriteriaNode_hasMore      (OvalCollection_resultCriteriaNode oc_resultCriteriaNode){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_resultCriteriaNode);
}
Oval_resultCriteriaNode OvalCollection_resultCriteriaNode_next         (OvalCollection_resultCriteriaNode oc_resultCriteriaNode){
	return (Oval_resultCriteriaNode)OvalCollection_next((OvalCollection_ptr)oc_resultCriteriaNode);
}


Oval_criteriaNode_type_enum Oval_resultCriteriaNode_type    (Oval_resultCriteriaNode node){
	return ((Oval_resultCriteriaNode_ptr)node)->type;
}
Oval_result_enum Oval_resultCriteriaNode_result             (Oval_resultCriteriaNode node){
	return ((Oval_resultCriteriaNode_ptr)node)->result;
}
Oval_operator_enum Oval_resultCriteriaNode_operator         (Oval_resultCriteriaNode node){
	//type==NODETYPE_CRITERIA
	Oval_operator_enum operator = OPERATOR_UNKNOWN;
	if(Oval_resultCriteriaNode_type(node)==NODETYPE_CRITERIA){
		operator = ((Oval_resultCriteriaNode_CRITERIA_ptr)node)->operator;
	}
	return operator;
}
OvalCollection_criteriaNode Oval_resultCriteriaNode_subnodes(Oval_resultCriteriaNode node){
	//type==NODETYPE_CRITERIA
	OvalCollection_criteriaNode oc_node = NULL;
	if(Oval_resultCriteriaNode_type(node)==NODETYPE_CRITERIA){
		Oval_criteriaNode* subnodes = ((Oval_resultCriteriaNode_CRITERIA_ptr)node)->subnodes;
		oc_node = newOvalCollection_criteriaNode(subnodes);
	}
	return oc_node;
}
Oval_resultTest Oval_resultCriteriaNode_test                (Oval_resultCriteriaNode node){
	//type==NODETYPE_CRITERION
	Oval_resultTest test = NULL;
	if(Oval_resultCriteriaNode_type(node)==NODETYPE_CRITERION){
		test = ((Oval_resultCriteriaNode_CRITERION_ptr)node)->test;
	}
}
Oval_result Oval_resultCriteriaNode_extends                 (Oval_resultCriteriaNode node){
	//type==NODETYPE_EXTENDDEF
	Oval_result result = NULL;
	if(Oval_resultCriteriaNode_type(node)==NODETYPE_EXTENDDEF){
		result = ((Oval_resultCriteriaNode_EXTENDDEF_ptr)node)->extends;
	}
	return result;
}//type==NODETYPE_EXTENDDEF
