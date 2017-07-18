
#include <stdio.h>
#include <string.h>
#include <cvss_score.h>
#include <cvrf.h>


int main(int argc, char **argv)
{
	struct cvrf_model *model;

	/* test export */
	if (argc == 4 && !strcmp(argv[1], "--export-all")) {

		model = cvrf_model_import(argv[2]);
		if(!model)
			return 1;
		cvrf_model_export(model, argv[3]);
		cvrf_model_free(model);
		return 0;
	} else if (argc == 4 && !strcmp(argv[1], "--eval")) {
		const char *os_version = "Red Hat Enterprise Linux Server (v. 7)";
		cvrf_export_results(argv[2], argv[3], os_version);
		return 0;
	}

	fprintf(stdout,
		"Usage: \n\n"
		"  %s --help\n"
		"  %s --export-all input.xml output.xml\n"
		"  %s --eval input.xml results.xml\n",
		argv[0], argv[0], argv[0]);

	return 0;
}
