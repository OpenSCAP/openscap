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

/***************************************************************************/

/**
 * Top-level structure of the CVRF hierarchy
 *
 */
struct cvrf_model {
	char *doc_title;
	struct product_tree *tree;
	struct oscap_list *vulnerabilities;	/* 1-n */
};
OSCAP_ACCESSOR_STRING(cvrf_model, doc_title)
OSCAP_IGETINS_GEN(cvrf_vulnerability, cvrf_model, vulnerabilities, vulnerability)
OSCAP_ITERATOR_REMOVE_F(cvrf_vulnerability)

/***************************************************************************
 * Product tree offshoot of main CVRF model
 */

struct product_name {
	char *product_id;
	char *cpe;
};
OSCAP_ACCESSOR_STRING(product_name, product_id)
OSCAP_ACCESSOR_STRING(product_name, cpe)

struct cvrf_branch {
	char *branch_type;
	char *branch_name;
	struct product_name *full_name;
	struct oscap_list *subbranches;
};
OSCAP_ACCESSOR_STRING(cvrf_branch, branch_type)
OSCAP_ACCESSOR_STRING(cvrf_branch, branch_name)

struct oscap_list_iterator *cvrf_branch_get_subbranches(struct cvrf_branch *branch) {
	return oscap_iterator_new(branch->subbranches);
}

struct product_tree {
	struct product_name *full_name;
	struct oscap_list *branches;
};

struct oscap_list_iterator *product_tree_get_branches(struct product_tree *tree) {
	return oscap_iterator_new(tree->branches);
}

/***************************************************************************
 * Vulnerability offshoot of main CVRF model
 *
 */
struct cvrf_vulnerability {
	int ordinal;
	char *vulnerability_title;
	char *cve_id;
	char *cwe_id;

	//Product Status
	struct oscap_list *product_statuses;

};
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, vulnerability_title)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, cve_id)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, cwe_id)
OSCAP_IGETINS_GEN(product_status, cvrf_vulnerability, product_statuses, product_status)
OSCAP_ITERATOR_REMOVE_F(product_status)

struct product_status {
	char *status;
	struct oscap_stringlist *product_ids;
};
OSCAP_ACCESSOR_STRING(product_status, status)

struct oscap_string_iterator *product_status_get_ids(struct product_status *stat) {
	return oscap_stringlist_get_strings(stat->product_ids);
}

/* End of variable definitions
 * */
/***************************************************************************/
/* XML string variables definitions
 * */

#define TAG_CVRF_DOC BAD_CAST "cvrfdoc"
#define TAG_CVRF_DOC_TITLE BAD_CAST "DocumentTitle"
#define ATTR_DOC_TITLE_LANG BAD_CAST "xml:lang"
// Vulnerabilities
#define TAG_VULNERABILITY BAD_CAST "Vulnerability"
#define ATTR_VULNERABILITY_ORDINAL BAD_CAST "Ordinal"
#define TAG_VULNERABILITY_TITLE BAD_CAST "Title"
#define TAG_VULNERABILITY_ID BAD_CAST "ID"
#define TAG_VULNERABILITY_CVE BAD_CAST "CVE"
#define TAG_VULNERABILITY_CWE BAD_CAST "CWE"
#define TAG_PRODUCT_STATUSES BAD_CAST "ProductStatuses"
#define TAG_PRODUCT_STATUS BAD_CAST "Status"
#define TAG_PRODUCT_ID BAD_CAST "ProductID"
#define ATTR_STATUS_TYPE BAD_CAST "Type"
// Product Tree
#define TAG_PRODUCT_TREE BAD_CAST "ProductTree"
#define TAG_BRANCH_TYPE BAD_CAST "BranchType"
#define TAG_BRANCH BAD_CAST "Branch"
#define ATTR_BRANCH_TYPE BAD_CAST "Type"
#define ATTR_BRANCH_NAME BAD_CAST "Name"
#define TAG_PRODUCT_NAME BAD_CAST "FullProductName"
#define ATTR_PRODUCT_ID BAD_CAST "ProductID"
// namespaces
#define CVRF_NS BAD_CAST "http://www.icasi.org/CVRF/schema/cvrf/1.1"
#define PROD_NS BAD_CAST "http://www.icasi.org/CVRF/schema/prod/1.1"
#define VULN_NS BAD_CAST "http://www.icasi.org/CVRF/schema/vuln/1.1"

/***************************************************************************/
/* Constructors of CVRF structures cvrf_*<structure>*_new()
 *
 */

struct product_name *product_name_new() {

	struct product_name *ret;

	ret = oscap_alloc(sizeof(struct product_name));
	if (ret == NULL)
		return NULL;

	ret->cpe = NULL;
	ret->product_id = NULL;

	return ret;
}


struct cvrf_branch *cvrf_branch_new() {

	struct cvrf_branch *ret;

	ret = oscap_alloc(sizeof(struct cvrf_branch));
	if (ret == NULL)
		return NULL;

	ret->full_name = product_name_new();
	ret->subbranches = oscap_list_new();
	ret->branch_name = NULL;
	ret->branch_type = NULL;

	return ret;
}

struct product_tree *product_tree_new() {

	struct product_tree *ret;

	ret = oscap_alloc(sizeof(struct product_tree));
	if (ret == NULL)
		return NULL;

	ret->branches = oscap_list_new();
	ret->full_name = product_name_new();

	return ret;
}

struct product_status *product_status_new() {

	struct product_status *ret;

	ret = oscap_alloc(sizeof(struct product_status));
	if (ret == NULL)
		return NULL;

	ret->product_ids = oscap_stringlist_new();
	ret->status = NULL;

	return ret;
}

struct cvrf_vulnerability *cvrf_vulnerability_new() {

	struct cvrf_vulnerability *ret;

	ret = oscap_alloc(sizeof(struct cvrf_vulnerability));
	if (ret == NULL)
		return NULL;

	ret->product_statuses = oscap_list_new();
	ret->cve_id = NULL;
	ret->cwe_id = NULL;
	ret->vulnerability_title = NULL;

	return ret;
}

struct cvrf_model *cvrf_model_new() {

	struct cvrf_model *ret;

	ret = oscap_alloc(sizeof(struct cvrf_model));
	if (ret == NULL)
		return NULL;

	ret->vulnerabilities = oscap_list_new();
	ret->tree = product_tree_new();
	ret->doc_title = NULL;

	return ret;
}

/***************************************************************************
 * Parsing functions
 *
 */

struct cvrf_model *cvrf_model_parse_xml(const char *file) {

	__attribute__nonnull__(file);

	struct cvrf_model *cvrf = NULL;
	int rc;

	struct oscap_source *source = oscap_source_new_from_file(file);
	struct xmlTextReaderPtr *reader = oscap_source_get_xmlTextReader(source);
	if (!reader) {
		oscap_source_free(source);
		return NULL;
	}

	rc = xmlTextReaderNextNode(reader);
	if (rc == -1) {
		xmlFreeTextReader(reader);
		oscap_source_free(source);
		return NULL;
	}

	cvrf = cvrf_model_parse(reader);

	xmlFreeTextReader(reader);
	oscap_source_free(source);

	return cvrf;
}

struct cvrf_model *cvrf_model_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_model *ret = NULL;
	struct cvrf_vulnerability *vulnerability = NULL;
	struct product_tree *tree = NULL;
	struct cvrf_branch *branch = NULL;

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_DOC) &&
	    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

		ret = cvrf_model_new();
		if (ret == NULL)
			return NULL;

		xmlTextReaderNextElement(reader);

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_DOC_TITLE)) {
			ret->doc_title = oscap_element_string_copy(reader);
			xmlTextReaderNextElement(reader);
		}

		tree = product_tree_parse(reader);
		ret->tree = tree;

		printf((char *)xmlTextReaderConstLocalName(reader));

		while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY) == 0) {
			vulnerability = cvrf_vulnerability_parse(reader);
			if (vulnerability)
				cvrf_model_add_vulnerability(ret, vulnerability);
			xmlTextReaderNextElement(reader);
		}
	}

	return ret;
}

struct product_tree *product_tree_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct product_tree *tree;
	struct cvrf_branch *branch;
	struct product_name *full_name;

	/* allocate platform structure here */
	tree = product_tree_new();
	if (tree == NULL)
		return NULL;

	/* If <empty /> then return, because there is no child element */
	if (xmlTextReaderIsEmptyElement(reader))
		return tree;

	/* skip from <entry> node to next one */
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_TREE) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME)) {
			full_name = product_name_new();
			full_name->product_id = (char *)xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_ID);
			full_name->cpe = (char *)oscap_element_string_copy(reader);
			tree->full_name = full_name;
		}
		else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH)) {
			branch = cvrf_branch_parse(reader);
			if (branch)
				oscap_list_add(tree->branches, branch);
		}
		printf("%s\n",xmlTextReaderConstLocalName(reader));
		xmlTextReaderNextNode(reader);
		printf("%s\n\n",xmlTextReaderConstLocalName(reader));
	}

	xmlTextReaderNextElement(reader);

	return tree;
}

struct cvrf_branch *cvrf_branch_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_branch *branch;
	struct cvrf_branch *subbranch;
	struct product_name *full_name;

	/* allocate platform structure here */
	branch = cvrf_branch_new();
	if (branch == NULL)
		return NULL;

	branch->branch_name = (char *)xmlTextReaderGetAttribute(reader, ATTR_BRANCH_NAME);
	branch->branch_type = (char *)xmlTextReaderGetAttribute(reader, ATTR_BRANCH_TYPE);
	xmlTextReaderNextElement(reader);
	printf("Next Elem: %s\n",xmlTextReaderConstLocalName(reader));

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME)) {
		full_name = product_name_new();
		full_name->product_id = (char *)xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_ID);
		full_name->cpe = (char *)oscap_element_string_copy(reader);
		branch->full_name = full_name;
		xmlTextReaderNextNode(reader);
		xmlTextReaderNextNode(reader);
	}
	else {
		while(xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH) == 0) {
			if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
				xmlTextReaderNextNode(reader);
				continue;
			}
			subbranch = cvrf_branch_parse(reader);
			if (subbranch) {
				oscap_list_add(branch->subbranches, subbranch);
			}
			xmlTextReaderNextNode(reader);
			xmlTextReaderNextNode(reader);
			xmlTextReaderNextNode(reader);
		}
	}

	return branch;
}

struct cvrf_vulnerability *cvrf_vulnerability_parse(xmlTextReaderPtr reader) {

	__attribute__nonnull__(reader);

	struct cvrf_vulnerability *vulnerability = NULL;
	struct product_status *stat = NULL;

	/* allocate platform structure here */
	vulnerability = cvrf_vulnerability_new();
	if (vulnerability == NULL)
		return NULL;

	vulnerability->ordinal = (int)xmlTextReaderGetAttribute(reader, ATTR_VULNERABILITY_ORDINAL);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_TITLE)) {
			vulnerability->vulnerability_title = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_CVE)) {
			vulnerability->cve_id = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_CWE)) {
			vulnerability->cwe_id = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STATUSES)) {
			xmlTextReaderNextElement(reader);
			while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STATUS) == 0) {
				stat = product_status_parse(reader);
				if (stat != NULL)
					cvrf_vulnerability_add_product_status(vulnerability, stat);
				xmlTextReaderNextNode(reader);
			}
			printf((char *)xmlTextReaderConstLocalName(reader));

		}
		else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STATUS)) {
			stat = product_status_parse(reader);
			if (stat != NULL)
				cvrf_vulnerability_add_product_status(vulnerability, stat);
		}

		xmlTextReaderNextNode(reader);
	}

	return vulnerability;
}


struct product_status *product_status_parse(xmlTextReaderPtr reader) {

	__attribute__nonnull__(reader);

	struct product_status *stat;

	stat = product_status_new();
	if (stat == NULL)
		return NULL;

	stat->status = (char *)xmlTextReaderGetAttribute(reader, ATTR_STATUS_TYPE);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STATUS) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_ID)) {
			const char *product_id = oscap_element_string_copy(reader);

			if (product_id != NULL)
				oscap_stringlist_add_string(stat->product_ids, product_id);

		}
		xmlTextReaderNextNode(reader);
	}

	return stat;
}


/***************************************************************************
 * Export functions
 *
 */

void cvrf_model_export_xml(struct cvrf_model *cvrf, const char *file) {

	__attribute__nonnull__(cvrf);
	__attribute__nonnull__(file);

	struct xmlTextWriterPtr *writer = xmlNewTextWriterFilename(file, 0);
	if (writer == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return;
	}

	xmlTextWriterSetIndent(writer, 1);
	xmlTextWriterSetIndentString(writer, BAD_CAST "  ");
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

	cvrf_export(cvrf, writer);
	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cvrf_export(const struct cvrf_model *cvrf, xmlTextWriterPtr writer) {

	__attribute__nonnull__(cvrf);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_DOC, CVRF_NS);
	xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:cvrf", BAD_CAST "http://www.icasi.org/CVRF/schema/cvrf/1.1");

	xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_DOC_TITLE, NULL);
	xmlTextWriterWriteAttribute(writer, ATTR_DOC_TITLE_LANG, BAD_CAST "en");
	xmlTextWriterWriteString(writer, BAD_CAST cvrf->doc_title);
	xmlTextWriterEndElement(writer);

	product_tree_export(cvrf->tree, writer);
	OSCAP_FOREACH(cvrf_vulnerability, e, cvrf_model_get_vulnerabilities(cvrf), cvrf_vulnerability_export(e, writer);)

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}


void product_tree_export(const struct product_tree *tree, xmlTextWriterPtr writer) {
	__attribute__nonnull__(tree);
	__attribute__nonnull__(writer);

	struct product_name *name = tree->full_name;
	xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_TREE, PROD_NS);

	if (name->cpe != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_NAME, NULL);
		xmlTextWriterWriteAttribute(writer, BAD_CAST "ProductID", BAD_CAST name->product_id);
		xmlTextWriterWriteString(writer, BAD_CAST name->cpe);
		xmlTextWriterEndElement(writer);
	}

	struct oscap_list_iterator *branches = product_tree_get_branches(tree);
	while (oscap_iterator_has_more(branches)) {
		struct cvrf_branch *branch = oscap_iterator_next(branches);
		cvrf_branch_export(branch, writer);
	}
	oscap_iterator_free(branches);

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}


void cvrf_branch_export(const struct cvrf_branch *branch, xmlTextWriterPtr writer) {
	__attribute__nonnull__(branch);
	__attribute__nonnull__(writer);

	struct product_name *name = branch->full_name;
	xmlTextWriterStartElementNS(writer, NULL, TAG_BRANCH, NULL);
	xmlTextWriterWriteAttribute(writer, ATTR_BRANCH_TYPE, BAD_CAST branch->branch_type);
	xmlTextWriterWriteAttribute(writer, ATTR_BRANCH_NAME, BAD_CAST branch->branch_name);

	if (name->cpe != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_NAME, NULL);
		xmlTextWriterWriteAttribute(writer, BAD_CAST "ProductID", BAD_CAST name->product_id);
		xmlTextWriterWriteString(writer, BAD_CAST name->cpe);
		xmlTextWriterEndElement(writer);
	}
	if (oscap_list_get_itemcount(branch->subbranches) > 0) {
		struct oscap_list_iterator *subbranches = cvrf_branch_get_subbranches(branch);
		while (oscap_iterator_has_more(subbranches)) {
			struct cvrf_branch *subbranch = oscap_iterator_next(subbranches);
			cvrf_branch_export(subbranch, writer);
		}
		oscap_iterator_free(subbranches);
	}

	xmlTextWriterEndElement(writer);

	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}


void cvrf_vulnerability_export(const struct cvrf_vulnerability *vuln, xmlTextWriterPtr writer) {
	__attribute__nonnull__(vuln);
	__attribute__nonnull__(writer);

	struct product_status_iterator *iterator = cvrf_vulnerability_get_product_statuses(vuln);
	struct product_status *stat;

	xmlTextWriterStartElementNS(writer, NULL, TAG_VULNERABILITY, VULN_NS);
	xmlTextWriterWriteAttribute(writer, ATTR_VULNERABILITY_ORDINAL, BAD_CAST vuln->ordinal);

	if ((vuln->vulnerability_title) != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_VULNERABILITY_TITLE, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST vuln->vulnerability_title);
		xmlTextWriterEndElement(writer);
	}
	if ((vuln->cve_id) != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_VULNERABILITY_CVE, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST vuln->cve_id);
		xmlTextWriterEndElement(writer);
	}
	if ((vuln->cwe_id) != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_VULNERABILITY_CWE, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST vuln->cwe_id);
		xmlTextWriterEndElement(writer);
	}

	if (oscap_list_get_itemcount(vuln->product_statuses) > 0) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_STATUSES, NULL);
		OSCAP_FOREACH(product_status, e, cvrf_vulnerability_get_product_statuses(vuln), product_status_export(e, writer);)
		xmlTextWriterEndElement(writer);
	}

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}


void product_status_export(const struct product_status *stat, xmlTextWriterPtr writer) {
	__attribute__nonnull__(stat);
	__attribute__nonnull__(writer);

	struct oscap_string_iterator *product_ids = product_status_get_ids(stat);
	const char *product_id;

	xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_STATUS, NULL);
	xmlTextWriterWriteAttribute(writer, ATTR_STATUS_TYPE, BAD_CAST stat->status);

	while (oscap_string_iterator_has_more(product_ids)) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_ID, NULL);
		product_id = oscap_string_iterator_next(product_ids);
		xmlTextWriterWriteString(writer, BAD_CAST product_id);
		xmlTextWriterEndElement(writer);
	}

	oscap_string_iterator_free(product_ids);

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}


/***************************************************************************
 * Free functions
 */

void product_name_free(struct product_name *full_name) {
	if (full_name == NULL)
		return;

	oscap_free(full_name->cpe);
	oscap_free(full_name->product_id);
	oscap_free(full_name);
}

void cvrf_branch_free(struct cvrf_branch *branch) {

	if (branch == NULL)
		return;

	oscap_free(branch->branch_type);
	oscap_free(branch->branch_name);
	product_name_free(branch->full_name);
	oscap_list_free(branch->subbranches, (oscap_destruct_func) cvrf_branch_free);
	oscap_free(branch);
}

void product_tree_free(struct product_tree *tree) {

	if (tree == NULL)
		return;

	oscap_list_free(tree->branches, (oscap_destruct_func) cvrf_branch_free);
	product_name_free(tree->full_name);
	oscap_free(tree);
}

void product_status_free(struct product_status *status) {

	if (status == NULL)
		return;

	oscap_free(status->status);
	oscap_stringlist_free(status->product_ids);
	oscap_free(status);
}

void cvrf_vulnerability_free(struct cvrf_vulnerability *vulnerability) {

	if (vulnerability == NULL)
		return;

	oscap_free(vulnerability->vulnerability_title);
	oscap_free(vulnerability->cve_id);
	oscap_free(vulnerability->cwe_id);
	oscap_list_free(vulnerability->product_statuses, (oscap_destruct_func) product_status_free);
	oscap_free(vulnerability);
}

void cvrf_model_free(struct cvrf_model *cvrf) {

	if (cvrf == NULL)
		return;

	oscap_free(cvrf->doc_title);
	oscap_list_free(cvrf->vulnerabilities, (oscap_destruct_func) cvrf_vulnerability_free);
	product_tree_free(cvrf->tree);
	oscap_free(cvrf);
}

/* End of free functions
 * */
/***************************************************************************/


