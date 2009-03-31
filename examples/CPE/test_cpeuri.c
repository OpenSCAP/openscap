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
		"  %s -p CPE_URI\n"
		"        parse CPE URI and print individual components\n\n"
		"  %s -m CANDIDATE_CPE_URI CPE_URI [...]\n"
		"        match candidate CPE URI against list of CPE URIs\n\n"
		"  %s -h\n"
		"        display this help message\n\n",
		progname, progname, progname);
}

// string representation of entries in enum cpe_part_t
const char *CPE_PART_STR[] =
    { "??? ERROR - run away!", "hardware", "operating system", "application" };

bool cpe_example_parse(const char *cpeuri)
{
	cpe_t *cpe = cpe_new(cpeuri);	// create CPE from URI string
	char *uri;

	if (cpe == NULL) {
		// CPE failed to parse
		fprintf(stderr, "Invalid CPE URI\n");
		return false;
	}
	// get CPE URI as a string
	uri = cpe_get_uri(cpe);

	// print URI components
	printf("URI      : %s\n", uri);
	printf("Part     : %s\n", CPE_PART_STR[cpe->part]);
	printf("Vendor   : %s\n", cpe->vendor);
	printf("Product  : %s\n", cpe->product);
	printf("Version  : %s\n", cpe->version);
	printf("Update   : %s\n", cpe->update);
	printf("Edition  : %s\n", cpe->edition);
	printf("Language : %s\n", cpe->language);

	// delete CPE URI string
	free(uri);
	// delete CPE itself
	cpe_delete(cpe);

	return true;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		// not enough arguments
		print_help(argv[0]);
		return EXIT_FAILURE;
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
