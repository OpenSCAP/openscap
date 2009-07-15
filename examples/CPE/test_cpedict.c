#include <cpedict.h>

// dump contents of an CPE dictionary item
void cpe_dictitem_dump(struct cpe_dictitem * item)
{
	// print name and title
	printf("  Name:  ");
	cpe_name_write(cpe_dictitem_name(item), stdout);
	printf("\n");
	printf("  Title: %s\n", cpe_dictitem_title(item));

	// print notes
	printf("  Notes:\n");
	struct oscap_string_iterator* itn = cpe_dictitem_notes(item);
	while (oscap_string_iterator_has_more(itn))
		printf("    %s\n", oscap_string_iterator_next(itn));

	// print depracation info
	if (cpe_dictitem_depracated(item)) {
		printf("  Depracated by: ");
		cpe_name_write(cpe_dictitem_depracated(item), stdout);
		printf(" on %s\n", cpe_dictitem_depracation_date(item));
	}

	// print references
	printf("  References:\n");
	struct cpe_dict_reference_iterator* itr = cpe_dictitem_references(item);
	while (cpe_dict_reference_iterator_has_more(itr)) {
		struct cpe_dict_reference* ref = cpe_dict_reference_iterator_next(itr);
		printf("    %s (%s)\n", cpe_dict_reference_content(ref), cpe_dict_reference_href(ref));
	}

	// print checks
	printf("  Checks:\n");
	struct cpe_dict_check_iterator* itc = cpe_dictitem_checks(item);
	while (cpe_dict_check_iterator_has_more(itc)) {
		struct cpe_dict_check* ck = cpe_dict_check_iterator_next(itc);
		printf("    id: %s, system: %s, href: %s\n",
			   cpe_dict_check_identifier(ck), cpe_dict_check_system(ck), cpe_dict_check_href(ck));
	}

	printf("\n");
}

int main(int argc, char **argv)
{
	struct cpe_dict *dict;	// pointer to our CPE dictionary
	int i;

	if (argc < 2) {
		// not enough parametres
		printf("Usage: %s dictionary.xml [CPEs...]\n", argv[0]);
		return 1;
	}
	// create dictionary from specified filename
	dict = cpe_dict_new(argv[1]);

	if (dict != NULL) {
		// print dictionary generator info
		printf
		    ("Generated on %s by %s version %s using schema version %s.\n\n",
		     cpe_dict_generator_timestamp(dict), cpe_dict_generator_product_name(dict),
		     cpe_dict_generator_product_version(dict), cpe_dict_generator_schema_version(dict));

		// dump each dictionary item
		struct cpe_dictitem_iterator* it = cpe_dict_items(dict);
		while (cpe_dictitem_iterator_has_more(it))
			cpe_dictitem_dump(cpe_dictitem_iterator_next(it));

		// for each CPE specified on command line, try to match it against the dictionary
		for (i = 2; i < argc; ++i)
			printf("%s KNOWN: %s\n", cpe_name_match_dict_str(argv[i], dict) ? "   " : "NOT", argv[i]);

		// free system resources
		cpe_dict_delete(dict);
	} else {
		// dictionary failed to load (dict == NULL)
		printf("Error while loading CPE dictionary.\n");
		return 1;
	}

	oscap_cleanup(); // clean caches

	return 0;
}
