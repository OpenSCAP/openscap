/*
 * test_probes.c
 *
 *  Created on: Aug 4, 2009
 *      Author: Peter Vrabec
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

	struct oval_iterator_object *objects = get_oval_objects(model);
	if (!oval_iterator_object_has_more(objects)) {
		printf("NO DEFINITIONS FOUND\n");
		return 1;
	}

	int index;
	for (index = 1; oval_iterator_object_has_more(objects); index++) {
		struct oval_object *object = oval_iterator_object_next(objects);
		oval_object_to_print(object, "    ", index);
		printf("Callin probe on object\n");
		probe_object(object, model);
	}

	return 0;
}

