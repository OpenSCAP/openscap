#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "public/cvrf.h"
#include "cvrf_priv.h"

#include "common/list.h"
#include "common/_error.h"
#include "common/xmltext_priv.h"
#include "common/elements.h"
#include "common/oscap_string.h"

#include "CPE/cpelang_priv.h"
#include "CVSS/cvss_priv.h"
#include "CVSS/public/cvss_score.h"

#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

/*****************************************************
 *
 */
struct cvrf_os_sysinfo {
	char *os_name;
	char *os_version;
	char *os_version_id;
	char *os_pretty_name;
};
OSCAP_ACCESSOR_STRING(cvrf_os_sysinfo, os_name);
OSCAP_ACCESSOR_STRING(cvrf_os_sysinfo, os_version);
OSCAP_ACCESSOR_STRING(cvrf_os_sysinfo, os_version_id);
OSCAP_ACCESSOR_STRING(cvrf_os_sysinfo, os_pretty_name);

struct cvrf_os_sysinfo *cvrf_os_sysinfo_new() {
	struct cvrf_os_sysinfo *ret;

	ret = oscap_alloc(sizeof(struct cvrf_os_sysinfo));
	if (ret == NULL)
		return NULL;

	ret->os_name = NULL;
	ret->os_version = NULL;
	ret->os_version_id = NULL;
	ret->os_pretty_name = NULL;

	return ret;
}

void cvrf_os_sysinfo_free(struct cvrf_os_sysinfo *sysinfo) {

	if (sysinfo == NULL)
		return;

	oscap_free(sysinfo->os_name);
	oscap_free(sysinfo->os_version);
	oscap_free(sysinfo->os_version_id);
	oscap_free(sysinfo->os_pretty_name);
	oscap_free(sysinfo);
}

/*****************************************************
 *
 */

#define TAG_CVRF_DOC BAD_CAST "cvrfdoc"
//namespaces
#define CVRF_NS BAD_CAST "http://www.icasi.org/CVRF/schema/cvrf/1.1"
#define VULN_NS BAD_CAST "http://www.icasi.org/CVRF/schema/vuln/1.1"

void cvrf_export_results(const char *input_file, const char *export_file, const char *os_version) {
	__attribute__nonnull__(input_file);

	struct xmlTextWriterPtr *writer = xmlNewTextWriterFilename(export_file, 0);
	if (writer == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return;
	}

	struct cvrf_model *model = cvrf_model_import(input_file);
	char *product_id = get_cvrf_product_id_by_OS(model, os_version);
	struct cvrf_vulnerability_iterator *it = cvrf_model_get_vulnerabilities(model);
	struct cvrf_vulnerability *vuln;

	xmlTextWriterSetIndent(writer, 1);
	xmlTextWriterSetIndentString(writer, BAD_CAST "  ");
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

	xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_DOC, CVRF_NS);

	xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "DocumentTitle", NULL);
	xmlTextWriterWriteString(writer, BAD_CAST cvrf_model_get_doc_title(model));
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "OS Name", NULL);
	xmlTextWriterWriteString(writer, BAD_CAST os_version);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "Product ID", NULL);
	xmlTextWriterWriteString(writer, BAD_CAST product_id);
	xmlTextWriterEndElement(writer);

	while (cvrf_vulnerability_iterator_has_more(it)) {
		vuln = cvrf_vulnerability_iterator_next(it);

		xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "Vulnerability", VULN_NS);

		if (cvrf_vulnerability_get_cve_id(vuln) != NULL) {
			xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "CVE", NULL);
			xmlTextWriterWriteString(writer, BAD_CAST cvrf_vulnerability_get_cve_id(vuln));
			xmlTextWriterEndElement(writer);
		}

		xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "Result", NULL);
		if (cvrf_product_vulnerability_fixed(vuln, product_id)) {
			xmlTextWriterWriteString(writer, BAD_CAST "FIXED");
		}
		else {
			xmlTextWriterWriteString(writer, BAD_CAST "VULNERABLE");
		}
		xmlTextWriterEndElement(writer);
		xmlTextWriterEndElement(writer);
	}
	cvrf_vulnerability_iterator_free(it);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndDocument(writer);
	cvrf_model_free(model);
	xmlFreeTextWriter(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

char *get_cvrf_product_id_by_OS(struct cvrf_model *model, char *os_version) {

	char *branch_product_id = NULL;
	char *full_product_id = NULL;
	struct cvrf_product_tree *tree = cvrf_model_get_product_tree(model);
	struct cvrf_relationship *relation;
	struct cvrf_product_name *full_name;
	struct cvrf_branch *branch;

	struct oscap_iterator *branches = cvrf_product_tree_get_branches(tree);
	while (oscap_iterator_has_more(branches)) {
		branch = oscap_iterator_next(branches);
		branch_product_id = get_cvrf_product_id_from_branch(branch, os_version);

		if (branch_product_id != NULL) {
			break;
		}
	}
	oscap_iterator_free(branches);
	if (branch_product_id == NULL)
		return NULL;


	struct cvrf_relationship_iterator *relationships = cvrf_product_tree_get_relationships(tree);
	while (cvrf_relationship_iterator_has_more(relationships)) {
		relation = cvrf_relationship_iterator_next(relationships);

		if (strcmp(branch_product_id, cvrf_relationship_get_relates_to_ref(relation))) {
			full_name = cvrf_relationship_get_product_name(relation);
			full_product_id = cvrf_product_name_get_product_id(full_name);
			break;
		}
	}
	cvrf_relationship_iterator_free(relationships);

	return full_product_id;
}

char *get_cvrf_product_id_from_branch(struct cvrf_branch *branch, char *os_version) {

	const char *branch_name;
	struct cvrf_branch *subbranch;
	struct cvrf_product_name *full_name;
	char *product_id;

	if (strcmp(cvrf_branch_get_branch_type(branch), "Product Family")) {
		struct oscap_iterator *subbranches = cvrf_branch_get_subbranches(branch);

		while(oscap_iterator_has_more(subbranches)) {
			subbranch = oscap_iterator_next(subbranches);
			product_id = get_cvrf_product_id_from_branch(subbranch, os_version);

			if (product_id != NULL) {
				oscap_iterator_free(subbranches);
				return product_id;
			}
		}
		oscap_iterator_free(subbranches);
	}
	else if (strcmp(cvrf_branch_get_branch_type(branch), "Product Name")) {
		if (strcmp(cvrf_branch_get_branch_name(branch), os_version)) {
			full_name = cvrf_branch_get_cvrf_product_name(branch);
			return cvrf_product_name_get_product_id(full_name);
		} else {
			return NULL;
		}
	}
}

bool cvrf_product_vulnerability_fixed(struct cvrf_vulnerability *vuln, char *product) {

	struct cvrf_product_status_iterator *it = cvrf_vulnerability_get_cvrf_product_statuses(vuln);
	struct cvrf_product_status *stat;
	struct oscap_string_iterator *product_ids;
	const char *product_id;

	while (cvrf_product_status_iterator_has_more(it)) {
		stat = cvrf_product_status_iterator_next(it);
		product_ids = cvrf_product_status_get_ids(stat);

		while (oscap_string_iterator_has_more(product_ids)) {
			product_id = oscap_string_iterator_next(product_ids);

			if (strcmp(product_id, product)) {
				oscap_string_iterator_free(product_ids);
				cvrf_product_status_iterator_free(it);
				return true;
			}
		}
		oscap_string_iterator_free(product_ids);
	}
	cvrf_product_status_iterator_free(it);

	return false;
}

