
#include "../lib/libcpedict.h"

// dump contents of an CPE dictionary item
void cpe_dictitem_dump(CpeDictItem_t* item)
{
	int i;

	// print name and title
	printf("  Name:  "); cpe_write(item->name, stdout); printf("\n");
	printf("  Title: %s\n", item->title);

	// print notes
	if (item->notes_n) {
		printf("  Notes:\n");
		for (i = 0; i < item->notes_n; ++i)
			printf("    %s\n", item->notes[i]);
	}
	
	// print depracation info
	if (item->depracated) {
		printf("  Depracated by: ");
		cpe_write(item->depracated, stdout);
		printf(" on %s\n", item->depracation_date);
	}
	
	// print references
	if (item->references_n) {
		printf("  References:\n");
		for (i = 0; i < item->references_n; ++i)
			printf("    %s (%s)\n", item->references[i].content, item->references[i].href);
	}

	// print checks
	if (item->check_n) {
		printf("  Checks:\n");
		for (i = 0; i < item->check_n; ++i)
			printf("    id: %s, system: %s, href: %s\n", item->check[i]->identifier, item->check[i]->system, item->check[i]->href);
	}
	
	printf("\n");
}

int main(int argc, char** argv)
{
	CpeDict_t* dict;     // pointer to our CPE dictionary
	CpeDictItem_t* cur;  // pointer to single dictionary item
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
		printf("Generated on %s by %s version %s using schema version %s.\n\n",
			dict->generator.timestamp, dict->generator.product_name,
			dict->generator.product_version, dict->generator.schema_version);

		// dump each dictionary item
		for (cur = dict->first; cur != NULL; cur = cur->next)
			cpe_dictitem_dump(cur);

		// for each CPE specified on command line, try to match it against the dictionary
		for (i = 2; i < argc; ++i)
			printf("%s KNOWN: %s\n", cpe_name_match_dict_str(argv[i], dict) ? "   " : "NOT", argv[i]);

		// free system resources
		cpe_dict_delete(dict);
	}
	else {
		// dictionary failed to load (dict == NULL)
		printf("Error while loading CPE dictionary.\n");
		return 1;
	}

	// free parser data
	xmlCleanupParser();

	return 0;
}


