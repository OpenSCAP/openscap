
//#include "config.h"

#include <stdio.h>

#include <cve.h>


int main(int argc, char **argv)
{
	struct cve* cve;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <xmlfile>\n", argv[0]);
		fprintf(stderr,
			"You can get one at: http://nvd.nist.gov/download.cfm\n");
		return -1;
	}

	fprintf(stdout, "[begin: parse]\n");
	cve = cve_new(argv[1]);
	if (cve == NULL) {
		fprintf(stderr, "cveParse returned an error.\n");
		return -1;
	}
	fprintf(stdout, "[end: parse]\n");
	fprintf(stdout, "[done]\n");

	struct cve_info_iterator* it = cve_entries(cve);
	while (cve_info_iterator_has_more(it)) {
		struct cve_info* info = cve_info_iterator_next(it);
		fprintf(stdout, "[cveInfo: %s, %s, %s, %s, %s]\n", cve_info_id(info),
			cve_info_pub(info), cve_info_mod(info),
			cve_info_cwe(info) ? cve_info_cwe(info) : "",
			cve_info_summary(info));
		fprintf(stdout, "[cvss: %s, %s, %s, %s, %s, %s, %s, %s, %s]\n",
			cve_info_score(info), cve_info_vector(info), cve_info_complexity(info),
			cve_info_authentication(info), cve_info_confidentiality(info),
			cve_info_integrity(info), cve_info_availability(info), cve_info_source(info),
			cve_info_generated(info));

		struct cve_reference_iterator* refit = cve_info_references(info);
		while (cve_reference_iterator_has_more(refit)) {
			struct cve_reference* ref = cve_reference_iterator_next(refit);
			fprintf(stdout, "\t[cveReference: %s, %s, %s, %s]\n",
				cve_reference_summary(ref), cve_reference_href(ref), cve_reference_type(ref),
				cve_reference_source(ref));
		}
		fprintf(stdout, "\n");
	}

	cve_delete(cve);
	oscap_cleanup();  // clean caches

	return 0;
}
