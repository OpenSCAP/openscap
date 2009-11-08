#include <cpedict.h>

// dump contents of an CPE dictionary item
void cpe_dictitem_dump(struct cpe_item * item)
{
	// print name and title
	printf("  Name:  ");
	cpe_name_write(cpe_item_get_name(item), stdout);
	printf("\n");
	OSCAP_FOREACH (oscap_title, title, cpe_item_get_titles(item),
		printf("  Title  %s (%s)\n", oscap_title_get_content(title), "");
	)

	// print notes
	printf("  Notes:\n");
	OSCAP_FOREACH (oscap_title, note, cpe_item_get_notes(item),
		printf("  Note  %s (%s)\n", oscap_title_get_content(note), "");
	)

	// print deprecation info
	if (cpe_item_get_deprecated(item)) {
		printf("  Depracated by: ");
		cpe_name_write(cpe_item_get_deprecated(item), stdout);
		printf(" on %s\n", cpe_item_get_deprecation_date(item));
	}

	// print references
	printf("  References:\n");
	OSCAP_FOREACH (cpe_reference, ref, cpe_item_get_references(item),
		printf("    %s (%s)\n", cpe_reference_get_content(ref), cpe_reference_get_href(ref));
	)

	// print checks
	printf("  Checks:\n");
	OSCAP_FOREACH (cpe_check, ck, cpe_item_get_checks(item),
		printf("    id: %s, system: %s, href: %s\n",
			cpe_check_get_identifier(ck), cpe_check_get_system(ck), cpe_check_get_href(ck));
	)

	printf("\n");
}

// dump contents of an CPE dictionary vendor
void cpe_dict_vendor_dump(struct cpe_vendor * item)
{
	// print name and title
	printf("  Vendor:  ");
	if (cpe_vendor_get_value(item) != NULL) printf("%s", cpe_vendor_get_value(item));
	printf("\n");
	OSCAP_FOREACH (cpe_product, product, cpe_vendor_get_products(item),
		printf("  Product  %s (%d)", cpe_product_get_value(product), cpe_product_get_part(product));
	        OSCAP_FOREACH (cpe_version, version, cpe_product_get_versions(product),
		        printf(" v.%s ", cpe_version_get_value(version));
	        )
                printf("\n");
	)
	printf("\n");
}


int main(int argc, char **argv)
{
	struct cpe_dict_model *dict;	// pointer to our CPE dictionary
	int i;

	if (argc < 2) {
		// not enough parametres
		printf("Usage: %s dictionary.xml [CPEs...]\n", argv[0]);
		return 1;
	}
	// create dictionary from specified filename
	//dict = cpe_dict_new(argv[1]);
        printf("----------------------------------------------------\n");
		struct oscap_import_source *src = oscap_import_source_new(argv[1], NULL);
        dict = cpe_dict_model_import(src);
		oscap_import_source_free(src);
        printf("----------------------------------------------------\n");

        if (dict != NULL) {
		// print dictionary generator info
                struct cpe_generator *generator = cpe_dict_model_get_generator(dict);
		printf
		    ("Generated on %s by %s version %s using schema version %s.\n\n",
		     cpe_generator_get_timestamp(generator), cpe_generator_get_product_name(generator),
		     cpe_generator_get_product_version(generator), cpe_generator_get_schema_version(generator));

		// dump each dictionary item
		OSCAP_FOREACH (cpe_item, item, cpe_dict_model_get_items(dict),
			cpe_dictitem_dump(item);
		)
		OSCAP_FOREACH (cpe_vendor, vendor, cpe_dict_model_get_vendors(dict),
			cpe_dict_vendor_dump(vendor);
		)

		// for each CPE specified on command line, try to match it against the dictionary
		for (i = 2; i < argc; ++i)
			printf("%s KNOWN: %s\n", cpe_name_match_dict_str(argv[i], dict) ? "   " : "NOT", argv[i]);
	
		struct oscap_export_target *tgt = oscap_export_target_new("test_cpedict.out", NULL);
        cpe_dict_model_export(dict, tgt);
		oscap_export_target_free(tgt);
        printf("----------------------------------------------------\n");

	} else {
		// dictionary failed to load (dict == NULL)
		printf("Error while loading CPE dictionary.\n");
		return 1;
	}

	oscap_cleanup(); // clean caches

	return 0;
}
