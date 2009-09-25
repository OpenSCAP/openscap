#include <cpedict.h>

// dump contents of an CPE dictionary item
void cpe_dictitem_dump(struct cpe_dictitem * item)
{
	// print name and title
	printf("  Name:  ");
	cpe_name_write(cpe_dictitem_get_name(item), stdout);
	printf("\n");
	printf("  Title: %s\n", cpe_dictitem_get_title(item));

	// print notes
	printf("  Notes:\n");
	OSCAP_FOREACH_STR (itn, cpe_dictitem_get_notes(item),
		printf("    %s\n", oscap_string_iterator_next(itn));
	)

	// print deprecation info
	if (cpe_dictitem_get_deprecated(item)) {
		printf("  Depracated by: ");
		cpe_name_write(cpe_dictitem_get_deprecated(item), stdout);
		printf(" on %s\n", cpe_dictitem_get_deprecation_date(item));
	}

	// print references
	printf("  References:\n");
	OSCAP_FOREACH (cpe_dict_reference, ref, cpe_dictitem_get_references(item),
		printf("    %s (%s)\n", cpe_dict_reference_get_content(ref), cpe_dict_reference_get_href(ref));
	)

	// print checks
	printf("  Checks:\n");
	OSCAP_FOREACH (cpe_dict_check, ck, cpe_dictitem_get_checks(item),
		printf("    id: %s, system: %s, href: %s\n",
			cpe_dict_check_get_identifier(ck), cpe_dict_check_get_system(ck), cpe_dict_check_get_href(ck));
	)

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
		     cpe_dict_get_generator_timestamp(dict), cpe_dict_get_generator_product_name(dict),
		     cpe_dict_get_generator_product_version(dict), cpe_dict_get_generator_schema_version(dict));

		// dump each dictionary item
		OSCAP_FOREACH (cpe_dictitem, item, cpe_dict_get_items(dict),
			cpe_dictitem_dump(item);
		)

		// for each CPE specified on command line, try to match it against the dictionary
		for (i = 2; i < argc; ++i)
			printf("%s KNOWN: %s\n", cpe_name_match_dict_str(argv[i], dict) ? "   " : "NOT", argv[i]);

		// free system resources
		cpe_dict_free(dict);
	} else {
		// dictionary failed to load (dict == NULL)
		printf("Error while loading CPE dictionary.\n");
		return 1;
	}

	oscap_cleanup(); // clean caches

	return 0;
}
