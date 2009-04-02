/*
 * oval_sysint.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "oval_system_characteristics_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_sysint {
	char *name;
	char *ipAddress;
	char *macAddress;
} oval_sysint_t;
int oval_iterator_sysint_has_more(struct oval_iterator_sysint *oc_sysint)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysint);
}

struct oval_sysint *oval_iterator_sysint_next(struct oval_iterator_sysint
					      *oc_sysint)
{
	return (struct oval_sysint *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysint);
}

char *oval_sysint_name(struct oval_sysint *sysint)
{
	return ((struct oval_sysint *)sysint)->name;
}

char *oval_sysint_ipAddress(struct oval_sysint *sysint)
{
	return ((struct oval_sysint *)sysint)->ipAddress;
}

char *oval_sysint_macAddress(struct oval_sysint *sysint)
{
	return ((struct oval_sysint *)sysint)->macAddress;
}
