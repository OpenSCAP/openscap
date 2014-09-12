
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "oval_agent_api.h"
#include "oval_results.h"
#include "oscap.h"
#include "oscap_error.h"
#include "oscap_source.h"

int main(int argc, char **argv)
{
	struct oval_results_model *results_model = NULL;
	struct oval_definition_model *definition_model = NULL;

	definition_model=oval_definition_model_new();
	results_model = oval_results_model_new(definition_model,NULL);

	struct oscap_source *source = oscap_source_new_from_file(argv[1]);
	oval_results_model_import_source(results_model, source);
	oscap_source_free(source);

	oval_results_model_export(results_model, NULL, argv[2]);

	oval_results_model_free(results_model);
	oval_definition_model_free(definition_model);
	oscap_cleanup();
}

