/*
 * test_driver.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */
#include <stdio.h>
#include <stdlib.h>
#include "api/oval_agent_api.h"
#include "../../src/common/oscap.h"

int _test_error_handler(struct oval_xml_error *error, void *null)
{
	//ERROR HANDLING IS TODO
	return 1;
}

int main(int argc, char **argv)
{
	struct oval_object_model *model = oval_object_model_new();

	struct oval_import_source *is = oval_import_source_new_file(argv[1]);
	oval_object_model_load(model, is, &_test_error_handler, NULL);
	oval_import_source_free(is);

	struct oval_definition_iterator *definitions =
	    oval_object_model_get_definitions(model);
	if (!oval_definition_iterator_has_more(definitions)) {
		printf("NO DEFINITIONS FOUND\n");
		return 1;
	}
	int index;
	for (index = 1; oval_definition_iterator_has_more(definitions); index++) {
		struct oval_definition *definition =
		    oval_definition_iterator_next(definitions);
		oval_definition_to_print(definition, "", index);
	}
	oval_definition_iterator_free(definitions);

	oval_object_model_free(model);

	oscap_cleanup();

	return 0;
}
