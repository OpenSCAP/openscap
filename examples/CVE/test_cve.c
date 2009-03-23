
//#include "config.h"

#include <stdio.h>

#include <cve.h>

int main(int argc, char **argv)
{
	int i;
	cveInfo_t *cveList, *cve;
	cveReference_t *ref;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <xmlfile>\n", argv[0]);
		fprintf(stderr, "You can get one at: http://nvd.nist.gov/download.cfm\n");
		return -1;
	}

	fprintf(stderr, "[begin: parse]\n");
	i = cveParse(argv[1], &cveList);
	if (i < 0) {
		fprintf(stderr, "cveParse returned error: %d.\n", i);
		return -1;
	}
	fprintf(stderr, "[end: parse]\n");
	fprintf(stderr, "[done]\n");

	fprintf(stderr, "Total cves processed: %d\n", i);

	cve = cveList;
	while (cve != NULL) {
		fprintf(stderr, "[cveInfo: %s, %s, %s, %s, %s]\n", cve->id, cve->pub, cve->mod, cve->cwe ? cve->cwe : "", cve->summary);
		fprintf(stderr, "[cvss: %s, %s, %s, %s, %s, %s, %s, %s, %s]\n", cve->score, cve->vector, cve->complexity, cve->authentication, cve->confidentiality, cve->integrity, cve->availability, cve->source, cve->generated);

		i = 0;
		ref = cve->refs;
		while (ref != NULL) {
			i++;
			ref = ref->next;
		}
		fprintf(stderr, "References: %d\n", i);

		ref = cve->refs;
		while (ref != NULL) {
			fprintf(stderr, "\t[cveReference: %s, %s, %s, %s]\n", ref->summary, ref->href, ref->refType, ref->source);
			ref = ref->next;
		}
		fprintf(stderr, "\n");

		cve = cve->next;
	}

	cveDelAll(cveList);

	return 0;
}
