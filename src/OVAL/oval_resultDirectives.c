/**
 * @file oval_resultDirectives.c
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
