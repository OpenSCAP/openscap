/*
 * test_driver.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "oval_agent_api.h"
#include "oscap.h"
#include "oscap_error.h"
#include "oscap_source.h"

//typedef int (*oval_xml_error_handler) (struct oval_xml_error *, void *user_arg);
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
	printf("START\n");
	if (argc > 1) {
		struct oscap_source *source = oscap_source_new_from_file(argv[1]);
		struct oval_definition_model *model;
		printf("LOAD OVAL DEFINITIONS\n");
		model = oval_definition_model_import_source(source);
		oscap_source_free(source);
		if (model == NULL) {
                        _test_error();
		}
		printf("OVAL DEFINITIONS LOADED\n");
		if (argc > 2) {
			printf("LOAD OVAL SYSCHAR\n");
			struct oval_syschar_model *syschar_model = oval_syschar_model_new(model);
			struct oscap_source *source = oscap_source_new_from_file(argv[2]);
			if (oval_syschar_model_import_source(syschar_model, source) < 1)
                                _test_error();
			printf("OVAL SYSCHAR LOADED\n");

			int count;
			struct oval_syschar_iterator *syschars = oval_syschar_model_get_syschars(syschar_model);
			for (count = 0; oval_syschar_iterator_has_more(syschars); count++) {
				oval_syschar_iterator_next(syschars);
			}
			oval_syschar_iterator_free(syschars);

			if (count)
				printf("THERE ARE %d SYSCHARS\n", count);
			else
				printf("NO DEFINITIONS FOUND\n");

			oval_syschar_model_export(syschar_model, "-");
			oval_syschar_model_free(syschar_model);
		}
		oval_definition_model_free(model);
	} else
		printf("USAGE:Test <oval_definitions.xml> [<system_characteristics.xml>]");
	oscap_cleanup();
}
