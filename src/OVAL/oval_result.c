/**
 * @file oval_result.c
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
#include "api/oval_agent_api.h"

typedef struct oval_result {
	struct oval_definition           *definition;
	char                             *message;
	struct oval_result_criteria_node *criteria;
	oval_result_enum                  result;
} oval_result_t;

struct oval_result *oval_result_new(struct oval_results_model *model, char *defid)
{
	oval_result_t *result = (oval_result_t *)malloc(sizeof(oval_result_t));
	struct oval_syschar_model *syschar_model = oval_results_model_syschar_model(model);
	struct oval_object_model *object_model   = oval_syschar_model_object_model(syschar_model);
	result->definition = get_oval_definition_new(object_model, defid);
	result->result     = OVAL_RESULT_UNKNOWN;
	result->message    = NULL;
	result->criteria   = NULL;
	return result;
}

void oval_result_free(struct oval_result *result)
{
	if(result->message !=NULL)free(result->message);
	if(result->criteria!=NULL)free(result->criteria);

	result->criteria = NULL;
	result->definition = NULL;
	result->message = NULL;
	free(result);
}

int oval_iterator_result_has_more(struct oval_iterator_result *oc_result)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_result);
}

struct oval_result *oval_iterator_result_next(struct oval_iterator_result
					      *oc_result)
{
	return (struct oval_result *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_result);
}

struct oval_definition *oval_result_definition(struct oval_result *result)
{
	return ((struct oval_result *)result)->definition;
}

oval_result_enum oval_result_result(struct oval_result *result)
{
	return ((struct oval_result *)result)->result;
}

char *oval_result_message(struct oval_result *result)
{
	return ((struct oval_result *)result)->message;
}

struct oval_result_criteria_node *oval_result_criteria(struct oval_result
						       *result)
{
	return ((struct oval_result *)result)->criteria;
}

int export_results(struct oval_results_model *model, struct export_target *target)
{
	//TODO
	return 0;
}

