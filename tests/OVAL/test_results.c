/*
 * test_results.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "oval_agent_api.h"
#include "oval_results.h"
#include "oscap.h"
#include "error.h"

//typedef int (*oval_xml_error_handler) (struct oval_xml_error *, void *user_arg);
static int _test_error(void)
{
        if (oscap_err ()) {
                oscap_errfamily_t f;
                oscap_errcode_t   c;
                const char       *d;

                fprintf (stderr, "GOT error: %d, %d, %s.\n",
                        f = oscap_err_family (),
                        c = oscap_err_code (),
                        d = oscap_err_desc ());
        }
                
	return 0;
}


int main(int argc, char **argv)
{
	printf("START\n");
	if(argc>1){
		struct oval_definition_model *model;

		printf("LOAD OVAL DEFINITIONS\n");
		if ( (model=oval_definition_model_import(argv[1])) == NULL)
                        _test_error();
		printf("OVAL DEFINITIONS LOADED\n");
		if(argc>2){
			printf("LOAD OVAL RESULTS\n");
			struct oval_results_model *results_model = oval_results_model_new(model,NULL);
			if (oval_results_model_import(results_model, argv[2]) == NULL)
                                _test_error();
			printf("OVAL RESULTS LOADED\n");
			if (argc>3) {
				printf("WRITE OVAL RESULTS\n");
				struct oval_result_directives *directives = oval_result_directives_new(results_model);
				oval_results_model_export(results_model, directives, argv[3]);
				oval_result_directives_free(directives);
				printf("OVAL RESULTS WRITTEN\n");
			}
			oval_results_model_free(results_model);
		}
		oval_definition_model_free(model);
	}else printf("USAGE: %s <oval_definitions.xml> [results.xml>]\n", argv[0]);
	oscap_cleanup();
}

