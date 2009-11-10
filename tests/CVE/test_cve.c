
//#include "config.h"

#include <stdio.h>

#include <cve.h>


int main(int argc, char **argv)
{
	struct cve_model* cve;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <xmlfile>\n", argv[0]);
		fprintf(stderr,
			"You can get one at: http://nvd.nist.gov/download.cfm\n");
		return -1;
	}

	fprintf(stdout, "[begin: parse]\n");
	struct oscap_import_source *src = oscap_import_source_new(argv[1], NULL);
	cve = cve_model_import(src);
	if (cve == NULL) {
		fprintf(stderr, "cveParse returned an error.\n");
		return -1;
	}
	fprintf(stdout, "[end: parse]\n");
	fprintf(stdout, "[done]\n");

	/*OSCAP_FOREACH (cve_info, info, cve_get_entries(cve),
		fprintf(stdout, "[cveInfo: %s, %s, %s, %s, %s]\n", cve_info_get_id(info),
			cve_info_get_pub(info), cve_info_get_mod(info),
			cve_info_get_cwe(info) ? cve_info_get_cwe(info) : "",
			cve_info_get_summary(info));
		fprintf(stdout, "[cvss: %s, %s, %s, %s, %s, %s, %s, %s, %s]\n",
			cve_info_get_score(info), cve_info_get_vector(info), cve_info_get_complexity(info),
			cve_info_get_authentication(info), cve_info_get_confidentiality(info),
			cve_info_get_integrity(info), cve_info_get_availability(info), cve_info_get_source(info),
			cve_info_get_generated(info));

		OSCAP_FOREACH (cve_reference, ref, cve_info_get_references(info),
			fprintf(stdout, "\t[cveReference: %s, %s, %s, %s]\n",
				cve_reference_get_summary(ref), cve_reference_get_href(ref),
				cve_reference_get_type(ref), cve_reference_get_source(ref));
		)
		fprintf(stdout, "\n");
	)*/

	struct oscap_export_target *tgt = oscap_export_target_new("test_cve.out", NULL);
	cve_model_export_xml(cve, tgt);
	oscap_cleanup();  // clean caches

	return 0;
}
