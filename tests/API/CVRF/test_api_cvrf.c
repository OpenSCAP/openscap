
#include <stdio.h>
#include <string.h>
#include <cvss_score.h>
#include <cvrf.h>


int main(int argc, char **argv)
{
	struct cvrf_model *model;
	struct cvrf_product_tree *tree;
	struct cvrf_vulnerability_iterator *vuln;

	/* test export */
	if (argc == 4 && !strcmp(argv[1], "--export-all")) {

		model = cvrf_model_import(argv[2]);
		if(!model)
			return 1;
		cvrf_model_export(model, argv[3]);
		cvrf_model_free(model);
		return 0;
	}

	fprintf(stdout,
		"Usage: \n\n"
		"  %s --help\n"
		"  %s --export-all input.xml output.xml\n",
		argv[0], argv[0]);

	return 0;
}
