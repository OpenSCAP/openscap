#include <stdio.h>
#include <cce.h>

int main(int argc, char *argv[])
{
	printf("CCE XML Parsing Program:\n");

	if (argc != 3) {
		printf
		    ("Invalid Syntax Usage:\n Syntax: ./cce_parse filen.xml CCE-xxxx-x\n");
		return 1;
	}

	char *docname = argv[1];	/* The CCE XML file to be parsed. (searched) */
	char *id = argv[2];	/* 2nd Argument is the CCE ID to be searched for. */

	printf("Searching for %s\n\n", id);

	if (!cce_validate(docname)) {
		printf("File '%s' does not seem to be a valid CCE\n", docname);
		return 1;
	}

	struct cce* cce_list = cce_new(docname);

	if (cce_list == NULL) {
		printf("CCE document '%s' could not be loaded properly.\n", docname);
		return 1;
	}

	struct cce_entry* cce = cce_get_entry(cce_list, id);

	if (cce == NULL) {
		printf("CCE entry '%s' was not found\n", id);
		cce_free(cce_list);
		return 1;
	}

	/* Example of struct data returned. */
	printf("\nID: %s\n", cce_entry_get_id(cce));
	printf("Description: %s\n", cce_entry_get_description(cce));

	/*
	struct cce_reference_iterator* it = cce_entry_get_references(cce);
	while (cce_reference_iterator_has_more(it)) {
		struct cce_reference* ref = cce_reference_iterator_next(it);
		printf("Ref Source: %s\n", cce_reference_get_source(ref));
		printf("Ref Value: %s\n", cce_reference_get_value(ref));
	}
	*/
	OSCAP_FOREACH (cce_reference, ref, cce_entry_get_references(cce),
		printf("Ref Source: %s\n", cce_reference_get_source(ref));
		printf("Ref Value: %s\n", cce_reference_get_value(ref));
	)

	{
		//struct oscap_string_iterator *it = cce_entry_get_tech_mechs(cce);
		//while (oscap_string_iterator_has_more(it))
		OSCAP_FOREACH_STR (str, cce_entry_get_tech_mechs(cce),
			printf("Technical Mechanism: %s\n", str);
		)

		//it = cce_entry_get_params(cce);
		//while (oscap_string_iterator_has_more(it))
		OSCAP_FOREACH_STR (str, cce_entry_get_params(cce),
			printf("Available Parameter Choices: %s\n", str);
		)
	}

	cce_free(cce_list);

	oscap_cleanup(); // clean caches

	return 0;
}
