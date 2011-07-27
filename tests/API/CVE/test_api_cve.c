
//#include "config.h"

#include <stdio.h>
#include <string.h>
#include <cve.h>
#include <cvss.h>

#define ASSERT(c) if ((c) == true) { return 1; }
#define notNULL(c,d) if ((c) == NULL) { fprintf(stderr, "NULL exception: %s\n", d); return 1; }


void print_usage(const char *program_name, FILE *out);

// Print usage.
void print_usage(const char *program_name, FILE *out) 
{
  fprintf(out, 
	  "Usage: \n\n"
	  "  %s --help\n"
	  "  %s --get-all XML_IN ENCODING \n"
	  "  %s --test-export-all XML_IN ENCODING XML_OUT ENCODING\n"
	  "  %s --test-cvss XML_IN ENCODING\n"
	  "  %s --add-entries XML_IN ENCODING entry1 entry2 ...\n"
	  "  %s --\n"
	  "  %s --sanity-check\n",
	  program_name, program_name, program_name, program_name, program_name, program_name, program_name);
}

int main(int argc, char **argv)
{
    struct cve_model * model;
    struct cve_entry * entry;
    const struct cvss_impact * cvss;
    // iterators
    struct cve_entry_iterator * entry_it;

    int ret_val;

    // Wrong argument count or --help
    if (argc == 2 && !strcmp(argv[1], "--help")) {
        print_usage(argv[0], stdout);
        ret_val = 0;
    }
    
    else if (argc == 6 && !strcmp(argv[1], "--test-export-all")) {

        notNULL (model = cve_model_import(argv[2]), "model")
        cve_model_export(model, argv[4]);
        cve_model_free(model);
        return 0;
    }

    else if (argc > 4 && !strcmp(argv[1], "--add-entries")) {
    
        notNULL (model = cve_model_new(), "new model");

        int i;
        for (i=4; i<argc; i++) {
            entry = cve_entry_new();
            cve_entry_set_id(entry, argv[i]);
            cve_model_add_entry(model, entry);
        }

        cve_model_export(model, argv[2]);
        cve_model_free(model);
        return 0;
    }

    else if (argc == 4 && !strcmp(argv[1], "--test-cvss")) {

        notNULL (model = cve_model_import(argv[2]), "model")

        entry_it = cve_model_get_entries(model);
        entry = cve_entry_iterator_next(entry_it);
        cvss = cve_entry_get_cvss(entry);

        struct cvss_metrics *m = cvss_impact_get_base_metrics(cvss);
        if (m == NULL) return 1;

        float base_score = cvss_metrics_get_score(m);
        float bs = cvss_impact_base_score(cvss);
        printf("XML-stored score: %f, calculated score: %f\n", base_score, bs);
    
        return (bs != base_score);
    }
    
    else if (argc == 2 && !strcmp(argv[1], "--sanity-check")) {
        return 0;
    }

    else {
        print_usage(argv[0], stderr);
        ret_val = 1;
    }

    return ret_val;
}
