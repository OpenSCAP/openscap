#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <cvss_score.h>
#include <cve_nvd.h>

int main(int argc, char **argv)
{
	struct cve_model *model;
	struct cve_entry *entry;
	const struct cvss_impact *cvss;
	struct cvss_impact *impact;
	struct cve_entry_iterator *entry_it;
	struct cvss_metrics *metrics;
	float base_score;

	/* test export */
	if (argc == 4 && !strcmp(argv[1], "--export-all")) {

		model = cve_model_import(argv[2]);
		if(!model)
			return 1;
		cve_model_export(model, argv[3]);
		cve_model_free(model);
		return 0;
	}

	else if (argc == 3 && !strcmp(argv[1], "--test-cvss")) {

		model = cve_model_import(argv[2]);
		if(!model)
			return 1;

		entry_it = cve_model_get_entries(model);
		while (cve_entry_iterator_has_more(entry_it)) {
			entry = cve_entry_iterator_next(entry_it);

			printf("CVE: %s\n", cve_entry_get_id(entry));
			/* content */
			cvss = cve_entry_get_cvss(entry);
			if (!cvss) {
				printf("No CVSS record here\n");
				continue;
			}
			metrics = cvss_impact_get_base_metrics(cvss);
			base_score = cvss_metrics_get_score(metrics);
			printf("Content: %f\t%s\n", base_score, cvss_impact_to_vector(cvss));
			/* vector */
			impact = cvss_impact_new_from_vector(cvss_impact_to_vector(cvss));
			printf("Vector:  %f\t%s\n", cvss_impact_base_score(impact), cvss_impact_to_vector(impact));

			/* test */
			if(base_score!=cvss_impact_base_score(impact)) {
				printf("Score in content != Score calculated from vector");
				return 1;
			}
		}

		cve_entry_iterator_free(entry_it);
		cve_model_free(model);
		return 0;
	}

	fprintf(stdout,
		"Usage: \n\n"
		"  %s --help\n"
		"  %s --export-all input.xml output.xml\n"
		"  %s --test-cvss input.xml\n",
		argv[0], argv[0], argv[0]);

	return 0;
}

