
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "oval_agent_api.h"
#include "oval_results.h"
#include "oscap.h"
#include "oscap_source.h"

int main(int argc, char *argv[]) {
	struct oval_directives_model *model;

	model=oval_directives_model_new();

	struct oscap_source *source = oscap_source_new_from_file(argv[1]);
	oval_directives_model_import_source(model, source);
	oscap_source_free(source);
	oval_directives_model_export(model, argv[2]);

	oval_directives_model_free(model);
	oscap_cleanup();
	return 0;
}


