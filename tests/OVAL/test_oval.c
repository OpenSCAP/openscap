/*
 * test_driver.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */
#include <stdio.h>
#include <stdlib.h>
#include <oval_agent_api.h>
#include <oscap.h>
#include "error.h"

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
	struct oval_definition_model *model = oval_definition_model_new();

	struct oscap_import_source *is = oscap_import_source_new_file(argv[1], NULL);
	if (oval_definition_model_import(model, is, NULL) < 1)
            _test_error();

	struct oval_definition_iterator *definitions =
	    oval_definition_model_get_definitions(model);
	if (!oval_definition_iterator_has_more(definitions)) {
		printf("NO DEFINITIONS FOUND\n");
		return 1;
	}
	int index;
	for (index = 1; oval_definition_iterator_has_more(definitions); index++) {
		struct oval_definition *definition =
		    oval_definition_iterator_next(definitions);
		    /* output is not needed for pusrpuse of this test, is it? */
		    /* oval_definition_to_print(definition, "", index); */
	}
	oval_definition_iterator_free(definitions);

	oval_definition_model_free(model);

	oscap_cleanup();

	return 0;
}
