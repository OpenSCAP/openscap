#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpeuri.h>

void print_help(const char *progname)
{
	// just print help message
	fprintf(stderr,
		"Usage:\n\n"
		"  %s -w\n"
		"        test CPE uri set functions\n\n"
		"  %s -p CPE_URI\n"
		"        parse CPE URI and print individual components\n\n"
		"  %s -m CANDIDATE_CPE_URI CPE_URI [...]\n"
		"        match candidate CPE URI against list of CPE URIs\n\n"
		"  %s -h\n"
		"        display this help message\n\n",
		progname, progname, progname, progname);
}

// string representation of entries in cpe_part_t
const char *CPE_PART_STR[] =
    { "??? ERROR - run away!", "hardware", "operating system", "application" };

bool cpe_example_parse(const char *cpeuri)
{
	struct cpe_name *cpe = cpe_name_new(cpeuri);	// create CPE from URI string
	char *uri;

	if (cpe == NULL) {
		// CPE failed to parse
		fprintf(stderr, "Invalid CPE URI\n");
		return false;
	}
	// get CPE URI as a string
	uri = cpe_name_get_uri(cpe);

	// print URI components
	printf("URI      : %s\n", uri);
	printf("Part     : %s\n", CPE_PART_STR[cpe_name_get_part(cpe)]);
	printf("Vendor   : %s\n", cpe_name_get_vendor(cpe));
	printf("Product  : %s\n", cpe_name_get_product(cpe));
	printf("Version  : %s\n", cpe_name_get_version(cpe));
	printf("Update   : %s\n", cpe_name_get_update(cpe));
	printf("Edition  : %s\n", cpe_name_get_edition(cpe));
	printf("Language : %s\n", cpe_name_get_language(cpe));

	// delete CPE URI string
	free(uri);
	// delete CPE itself
	cpe_name_free(cpe);

	return true;
}

int main(int argc, char **argv)
{
	if (argc < 2){
		print_help(argv[0]);
		return EXIT_FAILURE;
	} else if(strcmp(argv[1], "-w") == 0) {
		// construct an empty CPE
		struct cpe_name *cpe = cpe_name_new(NULL);

		// set its fields
		cpe_name_set_part(cpe, CPE_PART_APP);
		cpe_name_set_product(cpe, "awesomeproduct");
		cpe_name_set_vendor (cpe, "somevendor");
		cpe_name_set_version(cpe, "123");
		cpe_name_set_edition(cpe, "special");

		// write the CPE as an URI
		char *uri = cpe_name_get_uri(cpe);
		printf("%s\n", uri);

		// release resources
		free(uri);
		cpe_name_free(cpe);
	} else if (strcmp(argv[1], "-h") == 0) {
		print_help(argv[0]);
	} else if (strcmp(argv[1], "-p") == 0) {
		// we are to parse specified URI

		if (argc != 3) {
			// not enough arguments
			fprintf(stderr,
				"ERROR: URI to parse was not specified!\n");
			return EXIT_FAILURE;
		}
		if (!cpe_example_parse(argv[2]))	// parse CPE URI and write out its components
			return EXIT_FAILURE;
	} else if (strcmp(argv[1], "-m") == 0) {
		// we are to match CPE names

		int result;

		if (argc < 4) {
			// not enough arguments
			fprintf(stderr,
				"ERROR: At least two CPE URIs are needed to perform CPE names matching!\n");
			return EXIT_FAILURE;
		}
		// perform CPE name matching
		result = cpe_name_match_strs(argv[2], argc - 3, argv + 3);

		// interpret result
		if (result == -2)
			fprintf(stderr,
				"One of given strings is not a valid CPE URI.\n");
		else if (result == -1)
			printf("%s did not match anything\n", argv[2]);
		else if (result >= 0)
			printf("%s matched %s\n", argv[2], argv[result + 3]);
	} else {
		// unknown option on command line
		fprintf(stderr, "ERROR: Unrecognized option: %s\n\n", argv[1]);
		print_help(argv[0]);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
