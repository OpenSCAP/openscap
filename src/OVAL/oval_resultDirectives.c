/*
 * oval_resultDirectives.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_results_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct Oval_resultDirectives_s{
	int definitionTrue         ;
	int definitionFalse        ;
	int definitionUnknown      ;
	int definitionError        ;
	int definitionNotEvaluated ;
	int definitionNotApplicable;
} Oval_resultDirectives_t;

typedef Oval_resultDirectives_t* Oval_resultDirectives_ptr;

int Oval_resultDirectives_definitionTrue         (Oval_resultDirectives directives){
	return ((Oval_resultDirectives_ptr)directives)->definitionTrue;
}
int Oval_resultDirectives_definitionFalse        (Oval_resultDirectives directives){
	return ((Oval_resultDirectives_ptr)directives)->definitionFalse;
}
int Oval_resultDirectives_definitionUnknown      (Oval_resultDirectives directives){
	return ((Oval_resultDirectives_ptr)directives)->definitionUnknown;
}
int Oval_resultDirectives_definitionError        (Oval_resultDirectives directives){
	return ((Oval_resultDirectives_ptr)directives)->definitionError;
}
int Oval_resultDirectives_definitionNotEvaluated (Oval_resultDirectives directives){
	return ((Oval_resultDirectives_ptr)directives)->definitionNotEvaluated;
}
int Oval_resultDirectives_definitionNotApplicable(Oval_resultDirectives directives){
	return ((Oval_resultDirectives_ptr)directives)->definitionNotApplicable;
}
