/*
 * oval_variable_binding.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "oval_definitions_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_variable_binding {
	struct oval_variable *variable;
	char *value;
} oval_variable_binding_t;

int oval_iterator_variable_binding_has_more(struct
					    oval_iterator_variable_binding
					    *oc_variable_binding)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_variable_binding);
}

struct oval_variable_binding *oval_iterator_variable_binding_next(struct
								  oval_iterator_variable_binding
								  *oc_variable_binding)
{
	return (struct oval_variable_binding *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_variable_binding);
}

struct oval_variable *oval_variable_binding_variable(struct
						     oval_variable_binding
						     *binding)
{
	return ((struct oval_variable_binding *)binding)->variable;
}

char *oval_variable_binding_value(struct oval_variable_binding *binding)
{
	return ((struct oval_variable_binding *)binding)->value;
}
