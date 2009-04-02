/*
 * oval_result_directives.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "oval_results_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_result_directives {
	int definitionTrue;
	int definitionFalse;
	int definitionUnknown;
	int definitionError;
	int definitionNotEvaluated;
	int definitionNotApplicable;
} oval_result_directives_t;

int oval_result_directives_definition_true(struct oval_result_directives
					   *directives)
{
	return ((struct oval_result_directives *)directives)->definitionTrue;
}

int oval_result_directives_definition_false(struct oval_result_directives
					    *directives)
{
	return ((struct oval_result_directives *)directives)->definitionFalse;
}

int oval_result_directives_definition_unknown(struct oval_result_directives
					      *directives)
{
	return ((struct oval_result_directives *)directives)->definitionUnknown;
}

int oval_result_directives_definition_error(struct oval_result_directives
					    *directives)
{
	return ((struct oval_result_directives *)directives)->definitionError;
}

int oval_result_directives_definition_not_evaluated(struct
						    oval_result_directives
						    *directives)
{
	return ((struct oval_result_directives *)directives)->
	    definitionNotEvaluated;
}

int oval_result_directives_definition_not_applicable(struct
						     oval_result_directives
						     *directives)
{
	return ((struct oval_result_directives *)directives)->
	    definitionNotApplicable;
}
