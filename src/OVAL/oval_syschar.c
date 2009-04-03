/**
 * @file oval_syschar.c 
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
#include "oval_system_characteristics_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_syschar {
	oval_syschar_collection_flag_enum flag;
	struct oval_collection *messages;
	struct oval_sysinfo *sysinfo;
	struct oval_object *object;
	struct oval_collection *variable_bindings;
	struct oval_collection *sysdata;
} oval_syschar_t;

int oval_iterator_syschar_has_more(struct oval_iterator_syschar *oc_syschar)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_syschar);
}

struct oval_syschar *oval_iterator_syschar_next(struct oval_iterator_syschar
						*oc_syschar)
{
	return (struct oval_syschar *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_syschar);
}

oval_syschar_collection_flag_enum oval_syschar_flag(struct oval_syschar
						    *syschar)
{
	return ((struct oval_syschar *)syschar)->flag;
}

struct oval_iterator_string *oval_syschar_messages(struct oval_syschar *syschar)
{
	return (struct oval_iterator_string *)oval_collection_iterator(syschar->
								       messages);
}

struct oval_sysinfo *oval_syschar_sysinfo(struct oval_syschar *syschar)
{
	return ((struct oval_syschar *)syschar)->sysinfo;
}

struct oval_object *oval_syschar_object(struct oval_syschar *syschar)
{
	return ((struct oval_syschar *)syschar)->object;
}

struct oval_iterator_variable_binding *oval_syschar_variable_bindings(struct
								      oval_syschar
								      *syschar)
{
	return (struct oval_iterator_variable_binding *)
	    oval_collection_iterator(syschar->variable_bindings);
}

struct oval_iterator_sysdata *oval_syschar_sysdata(struct oval_syschar *syschar)
{
	return (struct oval_iterator_sysdata *)
	    oval_collection_iterator(syschar->sysdata);
}
