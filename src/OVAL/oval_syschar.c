/*
 * oval_syschar.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_system_characteristics_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct oval_syschar_s{
	oval_syschar_collection_flag_enum flag      ;
	struct oval_collection_s *messages                ;
	struct oval_sysinfo_s *sysinfo                    ;
	struct oval_object_s *object                      ;
	struct oval_collection_s *variable_bindings       ;
	struct oval_collection_s *sysdata                 ;
} oval_syschar_t;

int   oval_iterator_syschar_has_more      (struct oval_iterator_syschar_s *oc_syschar){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_syschar);
}
struct oval_syschar_s *oval_iterator_syschar_next         (struct oval_iterator_syschar_s *oc_syschar){
	return (struct oval_syschar_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_syschar);
}

oval_syschar_collection_flag_enum oval_syschar_flag           (struct oval_syschar_s *syschar){
	return ((struct oval_syschar_s*)syschar)->flag;
}

struct oval_iterator_string_s *oval_syschar_messages(struct oval_syschar_s *syschar){
	return (struct oval_iterator_string_s*)oval_collection_iterator(syschar->messages);
}

struct oval_sysinfo_s *oval_syschar_sysinfo(struct oval_syschar_s *syschar){
	return ((struct oval_syschar_s*)syschar)->sysinfo;
}
struct oval_object_s *oval_syschar_object                             (struct oval_syschar_s *syschar){
	return ((struct oval_syschar_s*)syschar)->object;
}
struct oval_iterator_variable_binding_s *oval_syschar_variable_bindings(struct oval_syschar_s *syschar){
	return (struct oval_iterator_variable_binding_s*)oval_collection_iterator(syschar->variable_bindings);
}
struct oval_iterator_sysdata_s *oval_syschar_sysdata(struct oval_syschar_s *syschar){
	return (struct oval_iterator_sysdata_s*)oval_collection_iterator(syschar->sysdata);
}
