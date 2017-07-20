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
#include "CPE/public/cpe_dict.h"
#include "CVSS/cvss_priv.h"
#include "CVSS/public/cvss_score.h"

#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

#include "OVAL/public/oval_system_characteristics.h"
#include "OVAL/public/oval_definitions.h"

/*****************************************************
 *
 */
struct cvrf_model_eval {
	char *os_name;
	char *os_version;
	struct oscap_stringlist *product_ids;
	struct cvrf_model *model;
};
OSCAP_ACCESSOR_STRING(cvrf_model_eval, os_name);
OSCAP_ACCESSOR_STRING(cvrf_model_eval, os_version);

struct oscap_string_iterator *cvrf_model_eval_get_product_ids(struct cvrf_model_eval *eval) {
	return oscap_stringlist_get_strings(eval->product_ids);
}

struct cvrf_model *cvrf_eval_get_model(struct cvrf_model_eval *eval) {
	return eval->model;
}
void cvrf_eval_set_model(struct cvrf_model_eval *eval, struct cvrf_model *model) {
	eval->model = model;
}


struct cvrf_model_eval *cvrf_model_eval_new() {
	struct cvrf_model_eval *ret;

	ret = oscap_alloc(sizeof(struct cvrf_model_eval));
	if (ret == NULL)
		return NULL;

	ret->os_name = NULL;
	ret->os_version = NULL;
	ret->product_ids = oscap_stringlist_new();
	ret->model = cvrf_model_new();

	return ret;
}

void cvrf_model_eval_free(struct cvrf_model_eval *eval) {

	if (eval == NULL)
		return;

	oscap_free(eval->os_name);
	oscap_free(eval->os_version);
	oscap_stringlist_free(eval->product_ids);
	cvrf_model_free(eval->model);
	oscap_free(eval);
}

void cvrf_get_os_info(const char *input_file, struct cvrf_model_eval *eval) {
	struct oscap_source *source = oscap_source_new_from_file("/cpe/openscap-cpe-dict.xml");
	oscap_source_save_as(source, "oscap_source_export.xml");
	//struct cpe_dict_model *dict = cpe_dict_model_import_source(source);
	//cpe_dict_model_export(dict, "cpe_dict_export.xml");
	oscap_source_free(source);
	//cpe_dict_model_free(dict);
}

/*****************************************************
 *
 */

#define TAG_CVRF_DOC BAD_CAST "cvrfdoc"
#define ATTR_PRODUCT_ID "ProductID"
//namespaces
#define CVRF_NS BAD_CAST "http://www.icasi.org/CVRF/schema/cvrf/1.1"
#define VULN_NS BAD_CAST "http://www.icasi.org/CVRF/schema/vuln/1.1"

void cvrf_export_results(const char *input_file, const char *export_file, const char *os_name) {
	__attribute__nonnull__(input_file);

	struct xmlTextWriterPtr *writer = xmlNewTextWriterFilename(export_file, 0);
	if (writer == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return;
	}

	struct cvrf_model_eval *eval = cvrf_model_eval_new();
	struct cvrf_model *model = cvrf_model_import(input_file);
	cvrf_eval_set_model(eval, model);
	cvrf_model_eval_set_os_name(eval, os_name);
	cvrf_get_os_info(input_file, eval);

	get_cvrf_product_id_by_OS(eval, model);
	struct cvrf_vulnerability_iterator *it = cvrf_model_get_vulnerabilities(model);
	struct cvrf_vulnerability *vuln;
	const char *product_id = NULL;
	bool vulnerable = false;


	xmlTextWriterSetIndent(writer, 1);
	xmlTextWriterSetIndentString(writer, BAD_CAST "  ");
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

	xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_DOC, CVRF_NS);

	xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "DocumentTitle", NULL);
	xmlTextWriterWriteString(writer, BAD_CAST cvrf_model_get_doc_title(model));
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "OS Name", NULL);
	xmlTextWriterWriteString(writer, BAD_CAST os_name);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "Identification", NULL);
	xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "ID", NULL);
	xmlTextWriterWriteString(writer, BAD_CAST cvrf_model_get_identification(model));
	xmlTextWriterEndElement(writer);
	xmlTextWriterEndElement(writer);

	while (cvrf_vulnerability_iterator_has_more(it)) {
		vuln = cvrf_vulnerability_iterator_next(it);
		struct oscap_string_iterator *product_ids = cvrf_model_eval_get_product_ids(eval);

		xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "Vulnerability", VULN_NS);
		xmlTextWriterWriteAttribute(writer, BAD_CAST "Ordinal", BAD_CAST (char *)cvrf_vulnerablity_get_ordinal(vuln));

		if (cvrf_vulnerability_get_cve_id(vuln) != NULL) {
			xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "CVE", NULL);
			xmlTextWriterWriteString(writer, BAD_CAST cvrf_vulnerability_get_cve_id(vuln));
			xmlTextWriterEndElement(writer);
		}

		while (oscap_string_iterator_has_more(product_ids)) {
			product_id = oscap_string_iterator_next(product_ids);

			xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "Result", NULL);
			xmlTextWriterWriteAttribute(writer, ATTR_PRODUCT_ID, BAD_CAST product_id);
			if (cvrf_product_vulnerability_fixed(vuln, product_id)) {
				xmlTextWriterWriteString(writer, BAD_CAST "FIXED");
			}
			else {
				xmlTextWriterWriteString(writer, BAD_CAST "VULNERABLE");
				vulnerable = true;
			}
			xmlTextWriterEndElement(writer);
		}

		if (vulnerable && cvrf_vulnerability_get_remediation_count(vuln) > 0) {
			xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "Remediations", NULL);
			OSCAP_FOREACH(cvrf_remediation, e, cvrf_vulnerability_get_remediations(vuln), cvrf_remediation_export(e, writer);)
			xmlTextWriterEndElement(writer);
		}

		oscap_string_iterator_free(product_ids);
		xmlTextWriterEndElement(writer);
		vulnerable = false;
	}
	cvrf_vulnerability_iterator_free(it);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndDocument(writer);
	cvrf_model_eval_free(eval);
	xmlFreeTextWriter(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void get_cvrf_product_id_by_OS(struct cvrf_model_eval *eval, struct cvrf_model *model) {

	const char *branch_product_id = NULL;
	const char *relation_product_id = NULL;
	struct cvrf_product_tree *tree = cvrf_model_get_product_tree(model);
	struct cvrf_relationship *relation;
	struct cvrf_product_name *full_name;
	struct cvrf_branch *branch;

	struct oscap_iterator *branches = cvrf_product_tree_get_branches(tree);
	while (oscap_iterator_has_more(branches)) {
		branch = oscap_iterator_next(branches);
		branch_product_id = get_cvrf_product_id_from_branch(eval, branch);

		if (branch_product_id != NULL)
			break;
	}
	oscap_iterator_free(branches);
	printf("Branch Product ID: %s\n", branch_product_id);
	if (branch_product_id == NULL)
		return;


	struct cvrf_relationship_iterator *relationships = cvrf_product_tree_get_relationships(tree);
	while (cvrf_relationship_iterator_has_more(relationships)) {
		relation = cvrf_relationship_iterator_next(relationships);
		if (!strcmp(branch_product_id, cvrf_relationship_get_relates_to_ref(relation))) {
			full_name = cvrf_relationship_get_product_name(relation);
			relation_product_id = cvrf_product_name_get_product_id(full_name);
			printf("Relation Product ID: %s\n", relation_product_id);
			oscap_stringlist_add_string(eval->product_ids, relation_product_id);
		}
	}
	cvrf_relationship_iterator_free(relationships);

}

const char *get_cvrf_product_id_from_branch(struct cvrf_model_eval *eval, struct cvrf_branch *branch) {

	const char *product_id = NULL;
	const char *os_name = cvrf_model_eval_get_os_name(eval);
	struct cvrf_branch *subbranch;
	struct cvrf_product_name *full_name;

	if (!strcmp(cvrf_branch_get_branch_type(branch), "Product Family")) {
		struct oscap_iterator *subbranches = cvrf_branch_get_subbranches(branch);

		while(oscap_iterator_has_more(subbranches)) {
			subbranch = oscap_iterator_next(subbranches);
			product_id = get_cvrf_product_id_from_branch(eval, subbranch);

			if (product_id != NULL)
				break;
		}
		oscap_iterator_free(subbranches);
		return product_id;
	}
	else {
		if (!strcmp(cvrf_branch_get_branch_name(branch), os_name)) {
			full_name = cvrf_branch_get_cvrf_product_name(branch);
			product_id = cvrf_product_name_get_product_id(full_name);
			return product_id;
		}
	}
	return product_id;
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

