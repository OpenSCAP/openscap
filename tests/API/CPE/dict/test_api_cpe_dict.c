/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * OpenScap CPE Dict Module Test Suite Helper
 *
 * Authors:
 *      Ondrej Moris <omoris@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cpe_dict.h>
#include <cpe_name.h>
#include <string.h>

#define OSCAP_FOREACH_GENERIC(itype, vtype, val, init_val, code) \
    {                                                            \
        struct itype##_iterator *val##_iter = (init_val);        \
        vtype val;                                               \
        while (itype##_iterator_has_more(val##_iter)) {          \
            val = itype##_iterator_next(val##_iter);             \
            code                                                 \
        }                                                        \
        itype##_iterator_free(val##_iter);                       \
    }

#define OSCAP_FOREACH(type, val, init_val, code) \
        OSCAP_FOREACH_GENERIC(type, struct type *, val, init_val, code)

void print_usage(const char *, FILE *);

int main(int argc, char **argv)
{
	struct cpe_dict_model *dict_model;
	struct cpe_generator *generator;
	struct cpe_vendor *vendor;
	struct cpe_product *product;
	struct cpe_version *version;
	struct cpe_update *update;
	struct cpe_edition *edition;
	struct cpe_language *language;
	struct cpe_check *check;
	struct cpe_item *item;
	struct cpe_reference *reference;
	struct cpe_vendor_iterator *vendors_it;
	struct cpe_product_iterator *products_it;
	//struct cpe_check_iterator *check_it;
	//struct cpe_reference_iterator *reference_it;
	struct cpe_version_iterator *versions_it;
	struct cpe_update_iterator *updates_it;
	struct cpe_edition_iterator *editions_it;
	struct cpe_language_iterator *languages_it;
	struct oscap_text *title;
	struct oscap_text_iterator *titles_it;

	struct cpe_name *name = NULL;
	int ret_val = 0;
	bool ret_val_1 = false, ret_val_2 = false;

	if (argc == 2 && !strcmp(argv[1], "--help"))
		print_usage(argv[0], stdout);

	else if (argc == 4 && !strcmp(argv[1], "--list-cpe-names")) {

		struct oscap_source *source = oscap_source_new_from_file(argv[2]);
		if ((dict_model = cpe_dict_model_import_source(source)) == NULL) {
			oscap_source_free(source);
			return 2;
		}

		OSCAP_FOREACH(cpe_item, local_item,
			      cpe_dict_model_get_items(dict_model),
			      cpe_name_write(cpe_item_get_name(local_item),
					     stdout);
			      putchar('\n');)

		    cpe_dict_model_free(dict_model);
		oscap_source_free(source);
	}
	// List all dictionary.
	else if (argc == 4 && !strcmp(argv[1], "--list")) {

		struct oscap_source *source = oscap_source_new_from_file(argv[2]);
		if ((dict_model = cpe_dict_model_import_source(source)) == NULL) {
			oscap_source_free(source);
			return 2;
		}

		// Generator.
		generator = cpe_dict_model_get_generator(dict_model);
		printf("%s:%s:%s:%s\n",
		       cpe_generator_get_timestamp(generator),
		       cpe_generator_get_product_name(generator),
		       cpe_generator_get_product_version(generator),
		       cpe_generator_get_schema_version(generator));

		// Vendors.
		vendors_it = cpe_dict_model_get_vendors(dict_model);
		while (cpe_vendor_iterator_has_more(vendors_it)) {
			vendor = cpe_vendor_iterator_next(vendors_it);
			printf("%s:(", cpe_vendor_get_value(vendor));

			// Titles.
			titles_it = cpe_vendor_get_titles(vendor);
			while (oscap_text_iterator_has_more(titles_it)) {
				title = oscap_text_iterator_next(titles_it);
				printf("%s,", oscap_text_get_text(title));
			}
			oscap_text_iterator_free(titles_it);
			putchar(')');
			putchar(':');

			// Products.
			products_it = cpe_vendor_get_products(vendor);
			while (cpe_product_iterator_has_more(products_it)) {
				product =
				    cpe_product_iterator_next(products_it);
				printf("(%s,%d,(",
				       cpe_product_get_value(product),
				       cpe_product_get_part(product));

				// Versions.
				versions_it = cpe_product_get_versions(product);
				while (cpe_version_iterator_has_more
				       (versions_it)) {
					version =
					    cpe_version_iterator_next
					    (versions_it);
					printf("%s,(",
					       cpe_version_get_value(version));

					// Updates.
					updates_it =
					    cpe_version_get_updates(version);
					while (cpe_update_iterator_has_more
					       (updates_it)) {
						update =
						    cpe_update_iterator_next
						    (updates_it);
						printf("%s,(",
						       cpe_update_get_value
						       (update));

						// Editions.
						editions_it =
						    cpe_update_get_editions
						    (update);
						while
						    (cpe_edition_iterator_has_more
						     (editions_it)) {
							edition =
							    cpe_edition_iterator_next
							    (editions_it);
							printf("%s,(",
							       cpe_edition_get_value
							       (edition));

							// Languages.
							languages_it =
							    cpe_edition_get_languages
							    (edition);
							while
							    (cpe_language_iterator_has_more
							     (languages_it)) {
								language =
								    cpe_language_iterator_next
								    (languages_it);
								printf("%s,(",
								       cpe_language_get_value
								       (language));
							}
							putchar(')');
						}
						putchar(')');
					}
					putchar(')');
				}
				putchar(')');
			}
			putchar(')');
			putchar('\n');
		}
		cpe_dict_model_free(dict_model);
		oscap_source_free(source);
	}

	else if (argc == 5 && !strcmp(argv[1], "--match")) {

		struct oscap_source *source = oscap_source_new_from_file(argv[2]);
		if ((dict_model = cpe_dict_model_import_source(source)) == NULL) {
			oscap_source_free(source);
			return 2;
		}

		name = cpe_name_new(argv[4]);

		ret_val_1 = cpe_name_match_dict(name, dict_model);
		ret_val_2 = cpe_name_match_dict_str(argv[4], dict_model);

		if (ret_val_1 != ret_val_2) {
			fprintf(stderr, "%s was not matched correctly!\n",
				argv[4]);
			ret_val = 2;
		} else
			ret_val = (ret_val_1 == false) ? 1 : 0;

		cpe_name_free(name);
		cpe_dict_model_free(dict_model);
		oscap_source_free(source);
	}

	else if (argc == 5 && !strcmp(argv[1], "--remove")) {

		struct oscap_source *source = oscap_source_new_from_file(argv[2]);
		if ((dict_model = cpe_dict_model_import_source(source)) == NULL) {
			oscap_source_free(source);
			return 2;
		}

		name = cpe_name_new(argv[4]);

		OSCAP_FOREACH(cpe_item, local_item,
			      cpe_dict_model_get_items(dict_model),
			      // filter out items with CPEs given by third argument
			      if (cpe_name_match_one
				  (name, cpe_item_get_name(local_item)))
			      cpe_item_iterator_remove(local_item_iter);)

			OSCAP_FOREACH(cpe_item, local_item,
				      cpe_dict_model_get_items(dict_model),
				      // print remaining CPEs
				      cpe_name_write(cpe_item_get_name
						     (local_item), stdout);
				      putchar('\n');)

		    cpe_name_free(name);
		cpe_dict_model_free(dict_model);
		oscap_source_free(source);
	}

	else if (argc == 6 && !strcmp(argv[1], "--export")) {
		struct oscap_source *source = oscap_source_new_from_file(argv[2]);
		if ((dict_model = cpe_dict_model_import_source(source)) == NULL) {
			oscap_source_free(source);
			return 2;
		}

		cpe_dict_model_export(dict_model, argv[4]);

		cpe_dict_model_free(dict_model);
		oscap_source_free(source);
	}

	else if (argc == 2 && !strcmp(argv[1], "--smoke-test")) {
		if ((dict_model = cpe_dict_model_new()) != NULL)
			cpe_dict_model_free(dict_model);

		if ((generator = cpe_generator_new()) != NULL)
			cpe_generator_free(generator);

		if ((check = cpe_check_new()) != NULL)
			cpe_check_free(check);

		if ((reference = cpe_reference_new()) != NULL)
			cpe_reference_free(reference);

		if ((item = cpe_item_new()) != NULL)
			cpe_item_free(item);

		if ((vendor = cpe_vendor_new()) != NULL)
			cpe_vendor_free(vendor);

		if ((product = cpe_product_new()) != NULL)
			cpe_product_free(product);

		if ((version = cpe_version_new()) != NULL)
			cpe_version_free(version);

		if ((update = cpe_update_new()) != NULL)
			cpe_update_free(update);

		if ((edition = cpe_edition_new()) != NULL)
			cpe_edition_free(edition);

		if ((language = cpe_language_new()) != NULL)
			cpe_language_free(language);
	}

	else {
		print_usage(argv[0], stderr);
		ret_val = 3;
	}

	return ret_val;
}

// Print usage.
void print_usage(const char *program_name, FILE * out)
{
	fprintf(out,
		"Usage: \n\n"
		"  %s --help\n"
		"  %s --list-cpe-names CPE_DICT_XML ENCODING\n"
		"  %s --list           CPE_DICT_XML ENCODING\n"
		"  %s --match          CPE_DICT_XML ENCODING CPE_URI\n"
		"  %s --remove         CPE_DICT_XML ENCODING CPE_URI\n"
		"  %s --export         CPE_DICT_XML ENCODING CPE_DICT_XML ENCODING\n"
		"  %s --smoke-test\n",
		program_name, program_name, program_name, program_name,
		program_name, program_name, program_name);
}
