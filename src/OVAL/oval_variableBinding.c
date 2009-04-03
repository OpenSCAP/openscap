/**
 * @file oval_variableBinding.c
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
