
#include <stdlib.h>
#include <stdio.h>
#include "oval_agent_api.h"
#include "oval_results.h"
#include "oscap.h"
#include "error.h"

int main(int argc, char *argv[]) {
	struct oval_directives_model *model;

	model=oval_directives_model_new();

	oval_directives_model_import(model, argv[1]);
	oval_directives_model_export(model, argv[2]);

	oval_directives_model_free(model);
	oscap_cleanup();
	return 0;
}


