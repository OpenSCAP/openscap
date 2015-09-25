#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "OVAL/oval_glob_to_regex.h"
#define COUNT 18

int test_glob_to_regex(const char *glob, const char *regex, int noescape);

/* Test vectors */
/* ATTENTION: The backslash must be escaped in C strings */
const char *globs[] = {
	"list.?",
	"project.*",
	"*old",
	"type*.[ch]",
	"*.*",
	"*",
	"?",
	"\\*",
	"\\?",
	"\\[hello\\]",
	"x[[:digit:]]\\*",
	"",
	"~/files/*.txt",
	"\\",
	"[ab",
	".*.conf",
	"docs/?b",
	"xy/??z",
};
const char *regexes[] = {
	"^list\\.[^/]$",
	"^project\\.[^/]*$",
	"^(?=[^.])[^/]*old$",
	"^type[^/]*\\.[ch]$",
	"^(?=[^.])[^/]*\\.[^/]*$",
	"^(?=[^.])[^/]*$",
	"^[^./]$",
	"^\\*$",
	"^\\?$",
	"^\\[hello\\]$",
	"^x[[:digit:]]\\*$",
	"^$",
	"^~/files/(?=[^.])[^/]*\\.txt$",
	"^\\\\$",
	NULL,
	"^\\.[^/]*\\.conf$",
	"^docs/[^./]b$",
	"^xy/[^./][^/]z$"
};
const char *noescape_regexes[] = {
	"^list\\.[^/]$",
	"^project\\.[^/]*$",
	"^(?=[^.])[^/]*old$",
	"^type[^/]*\\.[ch]$",
	"^(?=[^.])[^/]*\\.[^/]*$",
	"^(?=[^.])[^/]*$",
	"^[^./]$",
	"^\\\\[^/]*$",
	"^\\\\[^/]$",
	"^\\\\[hello\\\\]$",
	"^x[[:digit:]]\\\\[^/]*$",
	"^$",
	"^~/files/(?=[^.])[^/]*\\.txt$",
	"^\\\\$",
	NULL,
	"^\\.[^/]*\\.conf$",
	"^docs/[^./]b$",
	"^xy/[^./][^/]z$"
};

int test_glob_to_regex(const char *glob, const char *regex, int noescape)
{
	char *result;
	int retval;
	result = oval_glob_to_regex(glob, noescape);
	if (regex == NULL) {
		if (result == NULL) {
			printf("\tPASS\t%s\tNULL\tNULL\n", glob);
			retval = 1;
		} else {
			printf("\tFAIL\t%s\t%s\tNULL\n", glob, result);
			retval = 0;
		}
	} else {
		if (result == NULL) {
			printf("\tFAIL\t%s\tNULL\t%s\n", glob, regex);
			retval = 0;
		} else if (strcmp(regex, result) == 0) {
			printf("\tPASS\t%s\t%s\t%s\n", glob, result, regex);
			retval = 1;
		} else {
			printf("\tFAIL\t%s\t%s\t%s\n", glob, result, regex);
			retval = 0;
		}
	}
	free(result);
	return retval;
}

int main (int argc, char *argv[])
{
	int i, retval = 0;

	if (argc == 1) {
		printf("glob_noescape=false:\n");
		printf("Result\tInput\tOutput\tExpected\n");
		for (i = 0; i < COUNT; i++) {
			if(!test_glob_to_regex(globs[i], regexes[i], 0)) {
				retval = 1;
			}
		}
		printf("glob_noescape=true:\n");
		printf("Result\tInput\tOutput\tExpected\n");
		for (i = 0; i < COUNT; i++) {
			if(!test_glob_to_regex(globs[i], noescape_regexes[i], 1)) {
				retval = 1;
			}
		}
	}

	return retval;
}
