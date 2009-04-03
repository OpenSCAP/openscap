/**
 * @file oval_sysInfo.c 
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

typedef struct oval_sysinfo {
	char *osName;
	char *osVersion;
	char *osArchitecture;
	char *primaryHostName;
	struct oval_collection *interfaces;
} oval_sysinfo_t;

int oval_iterator_sysinfo_has_more(struct oval_iterator_sysinfo *oc_sysinfo)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysinfo);
}

struct oval_sysinfo *oval_iterator_sysinfo_next(struct oval_iterator_sysinfo
						*oc_sysinfo)
{
	return (struct oval_sysinfo *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysinfo);
}

char *oval_sysinfo_osName(struct oval_sysinfo *sysinfo)
{
	return ((struct oval_sysinfo *)sysinfo)->osName;
}

char *oval_sysinfo_osVersion(struct oval_sysinfo *sysinfo)
{
	return ((struct oval_sysinfo *)sysinfo)->osVersion;
}

char *oval_sysinfo_osArchitecture(struct oval_sysinfo *sysinfo)
{
	return ((struct oval_sysinfo *)sysinfo)->osArchitecture;
}

char *oval_sysinfo_primaryHostName(struct oval_sysinfo *sysinfo)
{
	return ((struct oval_sysinfo *)sysinfo)->primaryHostName;
}

struct oval_iterator_sysint *oval_sysinfo_interfaces(struct oval_sysinfo
						     *sysinfo)
{
	return (struct oval_iterator_sysint *)oval_collection_iterator(sysinfo->
								       interfaces);
}
