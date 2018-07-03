/*
 * test_driver.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oval_agent_api.h>
#include <oscap.h>
#include "oscap_source.h"
#include "oscap_error.h"

static int _test_error(void)
{
        if (oscap_err ()) {
                oscap_errfamily_t f;
                const char       *d;

                fprintf (stderr, "GOT error: %d, %s.\n",
                        f = oscap_err_family (),
                        d = oscap_err_desc ());
        }
                
	return 0;
}

int main(int argc, char **argv)
{
	struct oscap_source *source = oscap_source_new_from_file(argv[1]);
	struct oval_definition_model *model = oval_definition_model_import_source(source);
	oscap_source_free(source);
	if (model == NULL) 
		_test_error();

	struct oval_definition_iterator *definitions = oval_definition_model_get_definitions(model);
	if (!oval_definition_iterator_has_more(definitions)) {
		printf("NO DEFINITIONS FOUND\n");
		return 1;
	}
	while(oval_definition_iterator_has_more(definitions)) {
		oval_definition_iterator_next(definitions);
	       /* output is not needed for pusrpuse of this test, is it? */
	}
	oval_definition_iterator_free(definitions);

	oval_definition_model_free(model);
	oscap_cleanup();

	return 0;
}
