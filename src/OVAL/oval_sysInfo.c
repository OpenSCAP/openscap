/*
 * oval_sysinfo.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_system_characteristics_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct oval_sysinfo_s{
	char* osName                          ;
	char* osVersion                       ;
	char* osArchitecture                  ;
	char* primaryHostName                 ;
	struct oval_collection_s *interfaces  ;
} oval_sysinfo_t;

int   oval_iterator_sysinfo_has_more(struct oval_iterator_sysinfo_s *oc_sysinfo){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_sysinfo);
}
struct oval_sysinfo_s *oval_iterator_sysinfo_next(struct oval_iterator_sysinfo_s *oc_sysinfo){
	return (struct oval_sysinfo_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_sysinfo);
}

char* oval_sysinfo_osName                          (struct oval_sysinfo_s *sysinfo){
	return ((struct oval_sysinfo_s*)sysinfo)->osName;
}
char* oval_sysinfo_osVersion                       (struct oval_sysinfo_s *sysinfo){
	return ((struct oval_sysinfo_s*)sysinfo)->osVersion;
}
char* oval_sysinfo_osArchitecture                  (struct oval_sysinfo_s *sysinfo){
	return ((struct oval_sysinfo_s*)sysinfo)->osArchitecture;
}
char* oval_sysinfo_primaryHostName                 (struct oval_sysinfo_s *sysinfo){
	return ((struct oval_sysinfo_s*)sysinfo)->primaryHostName;
}
struct oval_iterator_sysint_s *oval_sysinfo_interfaces(struct oval_sysinfo_s *sysinfo){
	return (struct oval_iterator_sysint_s*)oval_collection_iterator(sysinfo->interfaces);
}
