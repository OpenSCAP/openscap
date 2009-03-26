/*
 * oval_sysint.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_system_characteristics_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct oval_sysint_s{
	char* name      ;
	char* ipAddress ;
	char* macAddress;
} oval_sysint_t;
int   oval_iterator_sysint_has_more      (struct oval_iterator_sysint_s *oc_sysint){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_sysint);
}
struct oval_sysint_s *oval_iterator_sysint_next         (struct oval_iterator_sysint_s *oc_sysint){
	return (struct oval_sysint_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_sysint);
}

char* oval_sysint_name      (struct oval_sysint_s *sysint){
	return ((struct oval_sysint_s*)sysint)->name;
}
char* oval_sysint_ipAddress (struct oval_sysint_s *sysint){
	return ((struct oval_sysint_s*)sysint)->ipAddress;
}
char* oval_sysint_macAddress(struct oval_sysint_s *sysint){
	return ((struct oval_sysint_s*)sysint)->macAddress;
}
