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
#include <oval_version.h>
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


	{
#define TEST_CONV(v)                                                    \
		do { \
			oval_version_t V; \
			V = oval_version_from_cstr(#v); \
			if (V == OVAL_VERSION_INVALID) \
				return 1; \
		} while(0)
#define TEST_VCMP(v1, op, v2)                                           \
		do { \
			oval_version_t a, b; \
			int r; \
			a = oval_version_from_cstr(#v1); \
			b = oval_version_from_cstr(#v2); \
			r = oval_version_cmp(a, b); \
			if (!(r op 0)) { \
				fprintf(stderr, "%s %s %s: not true\n", \
				        #v1, #op, #v2); \
				return 1; \
			} \
		} while(0)

		TEST_CONV(5);
		TEST_CONV(5.10);
		TEST_CONV(5.10.1);
		TEST_CONV(1.2.3);
		TEST_CONV(11.2.3);
		TEST_CONV(11.22.3);
		TEST_CONV(11.22.33);
		TEST_CONV(255.255.255);
		TEST_CONV(1.0.0);

		TEST_VCMP(5.10,   ==, 5.10);
		TEST_VCMP(5.10,   < , 5.10.1);
		TEST_VCMP(5.10.1, > , 5.10);
		TEST_VCMP(5.10.0, ==, 5.10);
		TEST_VCMP(5.4,    < , 5.10);
		TEST_VCMP(1.0.0,  ==, 1);
		TEST_VCMP(1.0.1,  !=, 1.0.0);
	}

	return 0;
}
