#include <cpedict.h>

// dump contents of an CPE dictionary item
void cpe_dictitem_dump(struct cpe_dictitem * item)
{
	// print name and title
	printf("  Name:  ");
	cpe_name_write(cpe_dictitem_get_name(item), stdout);
	printf("\n");
	OSCAP_FOREACH (cpe_dictitem_title, title, cpe_dictitem_get_titles(item),
		printf("  Title  %s (%s)\n", cpe_dictitem_title_get_content(title), "");
	)

	// print notes
	printf("  Notes:\n");
	OSCAP_FOREACH (cpe_dictitem_title, note, cpe_dictitem_get_notes(item),
		printf("  Note  %s (%s)\n", cpe_dictitem_title_get_content(note), "");
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

// dump contents of an CPE dictionary vendor
void cpe_dict_vendor_dump(struct cpe_dict_vendor * item)
{
	// print name and title
	printf("  Vendor:  ");
	if (cpe_dict_vendor_get_value(item) != NULL) printf("%s", cpe_dict_vendor_get_value(item));
	printf("\n");
	OSCAP_FOREACH (cpe_dict_product, product, cpe_dict_vendor_get_products(item),
		printf("  Product  %s (%d)", cpe_dict_product_get_value(product), cpe_dict_product_get_part(product));
	        OSCAP_FOREACH (cpe_dict_version, version, cpe_dict_product_get_versions(product),
		        printf(" v.%s ", cpe_dict_version_get_value(version));
	        )
                printf("\n");
	)
	printf("\n");
}

void test_dict_export(struct cpe_dict * dict){

    fprintf(stdout, "Result saved in test_cpedict.out file\n");
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
	//dict = cpe_dict_new(argv[1]);
        printf("----------------------------------------------------\n");
        dict = cpe_dict_parse(argv[1]);
        printf("----------------------------------------------------\n");

        if (dict != NULL) {
		// print dictionary generator info
                struct cpe_generator *generator = cpe_dict_get_generator(dict);
		printf
		    ("Generated on %s by %s version %s using schema version %s.\n\n",
		     cpe_generator_get_timestamp(generator), cpe_generator_get_product_name(generator),
		     cpe_generator_get_product_version(generator), cpe_generator_get_schema_version(generator));

		// dump each dictionary item
		OSCAP_FOREACH (cpe_dictitem, item, cpe_dict_get_items(dict),
			cpe_dictitem_dump(item);
		)
		OSCAP_FOREACH (cpe_dict_vendor, vendor, cpe_dict_get_vendors(dict),
			cpe_dict_vendor_dump(vendor);
		)

		// for each CPE specified on command line, try to match it against the dictionary
		for (i = 2; i < argc; ++i)
			printf("%s KNOWN: %s\n", cpe_name_match_dict_str(argv[i], dict) ? "   " : "NOT", argv[i]);

	} else {
		// dictionary failed to load (dict == NULL)
		printf("Error while loading CPE dictionary.\n");
		return 1;
	}
        dict_export(dict, "test_cpedict.out");
        printf("----------------------------------------------------\n");

	oscap_cleanup(); // clean caches

	return 0;
}
