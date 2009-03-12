/*
 * oval_criteriaNode.c
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include <includes/oval_definitions.h>
#include "includes/oval_collection_impl.h"


typedef struct Oval_criteriaNode_s{
	Oval_criteriaNode_type_enum type      ;
	int                         negate    ;
	char*                       comment   ;
} Oval_criteriaNode_t;
typedef struct Oval_criteriaNode_CRITERIA_s{
	Oval_criteriaNode_type_enum type      ;
	int                         negate    ;
	char*                       comment   ;
	Oval_operator_enum          operator  ;//type==NODETYPE_CRITERIA
	OvalCollection_criteriaNode subnodes  ;//type==NODETYPE_CRITERIA
} Oval_criteriaNode_CRITERIA_t;

typedef struct Oval_criteriaNode_CRITERION_s{
	Oval_criteriaNode_type_enum type      ;
	int                         negate    ;
	char*                       comment   ;
	Oval_test                   test      ;//type==NODETYPE_CRITERION
} Oval_criteriaNode_CRITERION_t;

typedef struct Oval_criteriaNode_EXTENDDEF_s{
	Oval_criteriaNode_type_enum type      ;
	int                         negate    ;
	char*                       comment   ;
	Oval_definition             definition;//type==NODETYPE_EXTENDDEF
} Oval_criteriaNode_EXTENDDEF_t;

typedef Oval_criteriaNode_t* Oval_criteriaNode_ptr;
typedef Oval_criteriaNode_CRITERIA_t*  Oval_criteriaNode_CRITERIA_ptr;
typedef Oval_criteriaNode_CRITERION_t* Oval_criteriaNode_CRITERION_ptr;
typedef Oval_criteriaNode_EXTENDDEF_t* Oval_criteriaNode_EXTENDDEF_ptr;

OvalCollection_criteriaNode newOvalCollection_criteriaNode(Oval_criteriaNode* criteriaNode_array){
	return (OvalCollection_criteriaNode)newOvalCollection((OvalCollection_target*)criteriaNode_array);
}
int   OvalCollection_criteriaNode_hasMore      (OvalCollection_criteriaNode oc_criteriaNode){
	return OvalCollection_hasMore((OvalCollection_ptr)oc_criteriaNode);
}
Oval_criteriaNode OvalCollection_criteriaNode_next         (OvalCollection_criteriaNode oc_criteriaNode){
	return (Oval_criteriaNode)OvalCollection_next((OvalCollection_ptr)oc_criteriaNode);
}

Oval_criteriaNode_type_enum Oval_criteriaNode_type      (Oval_criteriaNode node){
	return ((Oval_criteriaNode_ptr)node)->type;
}
int                         Oval_criteriaNode_negate    (Oval_criteriaNode node){
	return ((Oval_criteriaNode_ptr)node)->negate;
}
char*                       Oval_criteriaNode_comment   (Oval_criteriaNode node){
	return ((Oval_criteriaNode_ptr)node)->comment;
}
Oval_operator_enum          Oval_criteriaNode_operator  (Oval_criteriaNode node){
	//type==NODETYPE_CRITERIA
	return ((Oval_criteriaNode_CRITERIA_ptr)node)->operator;
}
OvalCollection_criteriaNode Oval_criteriaNode_subnodes  (Oval_criteriaNode node){
	//type==NODETYPE_CRITERIA
	return ((Oval_criteriaNode_CRITERIA_ptr)node)->subnodes;
}
Oval_test                   Oval_criteriaNode_test      (Oval_criteriaNode node){
	//type==NODETYPE_CRITERION
	return ((Oval_criteriaNode_CRITERION_ptr)node)->test;
}
Oval_definition             Oval_criteriaNode_definition(Oval_criteriaNode node){
	//type==NODETYPE_EXTENDDEF
	return ((Oval_criteriaNode_EXTENDDEF_ptr)node)->definition;
}
