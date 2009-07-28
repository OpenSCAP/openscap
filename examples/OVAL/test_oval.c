/*
 * test_driver.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */
#include <stdio.h>
#include <stdlib.h>
#include "api/oval_agent_api.h"

int _test_error_handler(struct oval_xml_error *error, void *null)
{
	//ERROR HANDLING IS TODO
	return 1;
}

int main(int argc, char **argv)
{
	struct oval_object_model *model = oval_object_model_new();

	struct import_source *is = import_source_file(argv[1]);
	load_oval_definitions(model, is, &_test_error_handler, NULL);
	import_source_free(is);

	struct oval_iterator_definition *definitions =
	    get_oval_definitions(model);
	if (!oval_iterator_definition_has_more(definitions)) {
		printf("NO DEFINITIONS FOUND\n");
		return 1;
	}
	int index;
	for (index = 1; oval_iterator_definition_has_more(definitions); index++) {
		struct oval_definition *definition =
		    oval_iterator_definition_next(definitions);
		oval_definition_to_print(definition, "", index);
	}

	return 0;
}
