/*
 * oval_sysinfo.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
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
