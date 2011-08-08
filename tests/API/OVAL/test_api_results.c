
#include <stdlib.h>
#include <stdio.h>

#include "oval_agent_api.h"
#include "oval_results.h"
#include "oscap.h"
#include "error.h"

int main(int argc, char **argv)
{
	struct oval_results_model *results_model = NULL;
	struct oval_definition_model *definition_model = NULL;

	definition_model=oval_definition_model_new();
	results_model = oval_results_model_new(definition_model,NULL);
	oval_results_model_import(results_model, argv[1]);
	oval_results_model_export(results_model, NULL, argv[2]);

	oval_results_model_free(results_model);
	oval_definition_model_free(definition_model);
	oscap_cleanup();
}

