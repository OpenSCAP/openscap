/*
 * oval_result.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_results_impl.h"
#include "includes/oval_collection_impl.h"

typedef struct oval_result_s{
	struct oval_definition_s *definition      ;
	oval_result_enum result         ;
	char* message                   ;
	struct oval_result_directives_s *directives;
	struct oval_result_criteria_node_s *criteria;
} oval_result_t;

int   oval_iterator_result_has_more      (struct oval_iterator_result_s *oc_result){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_result);
}
struct oval_result_s *oval_iterator_result_next(struct oval_iterator_result_s *oc_result){
	return (struct oval_result_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_result);
}

struct oval_definition_s *oval_result_definition      (struct oval_result_s *result){
	return ((struct oval_result_s*)result)->definition;
}
oval_result_enum oval_result_result         (struct oval_result_s *result){
	return ((struct oval_result_s*)result)->result;
}
char* oval_result_message                   (struct oval_result_s *result){
	return ((struct oval_result_s*)result)->message;
}
struct oval_result_directives_s *oval_directives(struct oval_result_s *result){
	return ((struct oval_result_s*)result)->directives;
}
struct oval_result_criteria_node_s *oval_result_criteria(struct oval_result_s *result){
	return ((struct oval_result_s*)result)->criteria;
}
