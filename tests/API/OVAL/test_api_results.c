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
	struct oval_result_directives *directives = NULL;
	struct oval_results_model *results_model = NULL;
	struct oval_definition_model *model = NULL;

	if(argc>1){
		if ( (model=oval_definition_model_new()) == NULL)
                        _test_error();
		printf("LOAD OVAL RESULTS\n");
		results_model = oval_results_model_new(model,NULL);
		if ((directives = oval_results_model_import(results_model, argv[1])) == NULL)
			_test_error();
		printf("OVAL RESULTS LOADED\n");
		if (argc>2) {
			printf("WRITE OVAL RESULTS\n");
			oval_results_model_export(results_model, directives, argv[2]);
			printf("OVAL RESULTS WRITTEN\n");
		}
	} else printf("USAGE: %s <results.xml>\n", argv[0]);


	if(directives)
		oval_result_directives_free(directives);
	if(results_model)
		oval_results_model_free(results_model);
	if(model)
		oval_definition_model_free(model);
	oscap_cleanup();
}

