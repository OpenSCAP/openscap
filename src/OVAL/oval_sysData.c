/**
 * @file oval_sysData.c 
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

typedef struct oval_sysdata {
	oval_family_enum family;
	oval_subtype_enum subtype;
} oval_sysdata_t;

int oval_iterator_sysdata_has_more(struct oval_iterator_sysdata *oc_sysdata)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysdata);
}

struct oval_sysdata *oval_iterator_sysdata_next(struct oval_iterator_sysdata
						*oc_sysdata)
{
	return (struct oval_sysdata *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysdata);
}

oval_family_enum oval_sysdata_family(struct oval_sysdata *sysdata)
{
	return ((struct oval_sysdata *)sysdata)->family;
}

oval_subtype_enum oval_sysdata_subtype(struct oval_sysdata *sysdata)
{
	return ((struct oval_sysdata *)sysdata)->subtype;
}
