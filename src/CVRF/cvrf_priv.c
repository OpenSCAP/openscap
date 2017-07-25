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


/***************************************************************************
 ***************************************************************************
 * CVRF structure definitions
 */

struct cvrf_index {
	char *source_url;
	char *index_file;
	struct oscap_list *models;
};
OSCAP_ACCESSOR_STRING(cvrf_index, source_url)
OSCAP_ACCESSOR_STRING(cvrf_index, index_file)
OSCAP_IGETINS_GEN(cvrf_model, cvrf_index, models, model)
OSCAP_ITERATOR_REMOVE_F(cvrf_model)

struct cvrf_index *cvrf_index_new() {

	struct cvrf_index *ret;

	ret = oscap_alloc(sizeof(struct cvrf_index));
	if (ret == NULL)
		return NULL;

	ret->source_url = NULL;
	ret->index_file = NULL;
	ret->models = oscap_list_new();

	return ret;
}

void cvrf_index_free(struct cvrf_index *index) {

	if (index == NULL)
		return;

	oscap_free(index->source_url);
	oscap_free(index->index_file);
	oscap_list_free(index->models, (oscap_destruct_func) cvrf_model_free);
	oscap_free(index);
}



/***************************************************************************
 * CVRF Model
 * Top-level structure of the CVRF hierarchy
 */
struct cvrf_model {
	char *doc_title;
	char *doc_type;
	struct cvrf_document *document;
	struct cvrf_product_tree *tree;
	struct oscap_list *vulnerabilities;	/* 1-n */
};
OSCAP_ACCESSOR_STRING(cvrf_model, doc_title)
OSCAP_ACCESSOR_STRING(cvrf_model, doc_type)
OSCAP_IGETINS_GEN(cvrf_vulnerability, cvrf_model, vulnerabilities, vulnerability)
OSCAP_ITERATOR_REMOVE_F(cvrf_vulnerability)

struct cvrf_product_tree *cvrf_model_get_product_tree(struct cvrf_model *model) {
	return model->tree;
}
struct cvrf_document *cvrf_model_get_document(struct cvrf_model *model) {
	return model->document;
}
const char *cvrf_model_get_identification(struct cvrf_model *model) {
	struct cvrf_doc_tracking *tracking = cvrf_document_get_tracking(model->document);
	return (cvrf_doc_tracking_get_tracking_id(tracking));
}

struct cvrf_model *cvrf_model_new() {

	struct cvrf_model *ret;

	ret = oscap_alloc(sizeof(struct cvrf_model));
	if (ret == NULL)
		return NULL;

	ret->vulnerabilities = oscap_list_new();
	ret->tree = cvrf_product_tree_new();
	ret->doc_title = NULL;
	ret->doc_type = NULL;
	ret->document = cvrf_document_new();

	return ret;
}

void cvrf_model_free(struct cvrf_model *cvrf) {

	if (cvrf == NULL)
		return;

	oscap_free(cvrf->doc_title);
	oscap_free(cvrf->doc_type);
	cvrf_document_free(cvrf->document);
	oscap_list_free(cvrf->vulnerabilities, (oscap_destruct_func) cvrf_vulnerability_free);
	cvrf_product_tree_free(cvrf->tree);
	oscap_free(cvrf);
}


/***************************************************************************
 * CVRF Document
 */
struct cvrf_document {
	struct cvrf_doc_tracking *tracking;
};

struct cvrf_doc_tracking *cvrf_document_get_tracking(struct cvrf_document *doc) {
	return doc->tracking;
}

void cvrf_document_set_tracking(struct cvrf_document *doc, struct cvrf_doc_tracking *track) {
	doc->tracking = track;
}


struct cvrf_document *cvrf_document_new() {

	struct cvrf_document *ret;

	ret = oscap_alloc(sizeof(struct cvrf_document));
	if (ret == NULL)
		return NULL;

	ret->tracking = cvrf_doc_tracking_new();

	return ret;
}

void cvrf_document_free(struct cvrf_document *doc) {

	if (doc == NULL)
		return;

	cvrf_doc_tracking_free(doc->tracking);
	oscap_free(doc);
}


/***************************************************************************
 * CVRF DocumentTracking
 */
struct cvrf_doc_tracking {
	char *tracking_id;
	char *tracking_alias;
	char *tracking_status;
	int tracking_version;
	char *init_release_date;
	char *cur_release_date;

	// Generator
	char *generator_engine;
	char *generator_date;
};
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, tracking_id)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, tracking_alias)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, tracking_status)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, init_release_date)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, cur_release_date)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, generator_engine)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, generator_date)


struct cvrf_doc_tracking *cvrf_doc_tracking_new() {

	struct cvrf_doc_tracking *ret;

	ret = oscap_alloc(sizeof(struct cvrf_doc_tracking));
	if (ret == NULL)
		return NULL;

	ret->tracking_id = NULL;
	ret->tracking_alias = NULL;
	ret->tracking_status = NULL;
	ret->cur_release_date = NULL;
	ret->init_release_date = NULL;
	ret->generator_engine = NULL;
	ret->generator_date = NULL;

	return ret;
}

void cvrf_doc_tracking_free(struct cvrf_doc_tracking *tracking) {

	if (tracking == NULL)
		return;

	oscap_free(tracking->tracking_id);
	oscap_free(tracking->tracking_alias);
	oscap_free(tracking->tracking_status);
	oscap_free(tracking->init_release_date);
	oscap_free(tracking->cur_release_date);
	oscap_free(tracking->generator_engine);
	oscap_free(tracking->generator_date);
	oscap_free(tracking);
}


/***************************************************************************
****************************************************************************
 * Product tree offshoot of main CVRF model
 */
struct cvrf_product_tree {
	struct cvrf_product_name *full_name;
	struct oscap_list *branches;
	struct oscap_list *relationships;
};
OSCAP_IGETINS_GEN(cvrf_relationship, cvrf_product_tree, relationships, relationship)
OSCAP_ITERATOR_REMOVE_F(cvrf_relationship)

struct oscap_iterator *cvrf_product_tree_get_branches(struct cvrf_product_tree *tree) {
	return oscap_iterator_new(tree->branches);
}

struct cvrf_product_tree *cvrf_product_tree_new() {

	struct cvrf_product_tree *ret;

	ret = oscap_alloc(sizeof(struct cvrf_product_tree));
	if (ret == NULL)
		return NULL;

	ret->branches = oscap_list_new();
	ret->full_name = cvrf_product_name_new();
	ret->relationships = oscap_list_new();

	return ret;
}

void cvrf_product_tree_free(struct cvrf_product_tree *tree) {

	if (tree == NULL)
		return;

	cvrf_product_name_free(tree->full_name);
	oscap_list_free(tree->branches, (oscap_destruct_func) cvrf_branch_free);
	oscap_list_free(tree->relationships, (oscap_destruct_func) cvrf_relationship_free);
	oscap_free(tree);
}


/***************************************************************************
 * CVRF Branch
 */
struct cvrf_branch {
	char *branch_type;
	char *branch_name;
	struct cvrf_product_name *full_name;
	struct oscap_list *subbranches;
};
OSCAP_ACCESSOR_STRING(cvrf_branch, branch_type)
OSCAP_ACCESSOR_STRING(cvrf_branch, branch_name)

struct oscap_iterator *cvrf_branch_get_subbranches(struct cvrf_branch *branch) {
	return oscap_iterator_new(branch->subbranches);
}

struct cvrf_product_name *cvrf_branch_get_cvrf_product_name(struct cvrf_branch *branch) {
	return branch->full_name;
}

struct cvrf_branch *cvrf_branch_new() {

	struct cvrf_branch *ret;

	ret = oscap_alloc(sizeof(struct cvrf_branch));
	if (ret == NULL)
		return NULL;

	ret->full_name = cvrf_product_name_new();
	ret->subbranches = oscap_list_new();
	ret->branch_name = NULL;
	ret->branch_type = NULL;

	return ret;
}

void cvrf_branch_free(struct cvrf_branch *branch) {

	if (branch == NULL)
		return;

	oscap_free(branch->branch_type);
	oscap_free(branch->branch_name);
	cvrf_product_name_free(branch->full_name);
	oscap_list_free(branch->subbranches, (oscap_destruct_func) cvrf_branch_free);
	oscap_free(branch);
}


/***************************************************************************
 * CVRF Relationship
 */
struct cvrf_relationship {
	char *product_reference;
	char *relation_type;
	char *relates_to_ref;
	struct cvrf_product_name *full_name;
};
OSCAP_ACCESSOR_STRING(cvrf_relationship, product_reference)
OSCAP_ACCESSOR_STRING(cvrf_relationship, relation_type)
OSCAP_ACCESSOR_STRING(cvrf_relationship, relates_to_ref)

struct cvrf_product_name *cvrf_relationship_get_product_name(struct cvrf_relationship *relation) {
	return relation->full_name;
}
struct cvrf_relationship *cvrf_relationship_new() {

	struct cvrf_relationship *ret;

	ret = oscap_alloc(sizeof(struct cvrf_relationship));
	if (ret == NULL)
		return NULL;

	ret->product_reference = NULL;
	ret->relation_type = NULL;
	ret->relates_to_ref = NULL;
	ret->full_name = cvrf_product_name_new();

	return ret;
}

void cvrf_relationship_free(struct cvrf_relationship *relationship) {
	if (relationship == NULL)
		return;

	oscap_free(relationship->product_reference);
	oscap_free(relationship->relation_type);
	oscap_free(relationship->relates_to_ref);
	cvrf_product_name_free(relationship->full_name);
	oscap_free(relationship);
}


/***************************************************************************
 * CVRF FullProductName
 */
struct cvrf_product_name {
	char *product_id;
	char *cpe;
};
OSCAP_ACCESSOR_STRING(cvrf_product_name, product_id)
OSCAP_ACCESSOR_STRING(cvrf_product_name, cpe)

struct cvrf_product_name *cvrf_product_name_new() {

	struct cvrf_product_name *ret;

	ret = oscap_alloc(sizeof(struct cvrf_product_name));
	if (ret == NULL)
		return NULL;

	ret->cpe = NULL;
	ret->product_id = NULL;

	return ret;
}

void cvrf_product_name_free(struct cvrf_product_name *full_name) {
	if (full_name == NULL)
		return;

	oscap_free(full_name->cpe);
	oscap_free(full_name->product_id);
	oscap_free(full_name);
}


/***************************************************************************
 ***************************************************************************
 * Vulnerability offshoot of main CVRF model
 *
 */
struct cvrf_vulnerability {
	int ordinal;
	char *vulnerability_title;
	char *cve_id;
	char *cwe_id;

	//Product Status
	struct oscap_list *cvrf_product_statuses;
	//Remediations
	struct oscap_list *remediations;

};
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, vulnerability_title)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, cve_id)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, cwe_id)
OSCAP_IGETINS_GEN(cvrf_product_status, cvrf_vulnerability, cvrf_product_statuses, cvrf_product_status)
OSCAP_ITERATOR_REMOVE_F(cvrf_product_status)
OSCAP_IGETINS_GEN(cvrf_remediation, cvrf_vulnerability, remediations, remediation)
OSCAP_ITERATOR_REMOVE_F(cvrf_remediation)

int cvrf_vulnerability_get_remediation_count(struct cvrf_vulnerability *vuln) {
	return oscap_list_get_itemcount(vuln->remediations);
}
int cvrf_vulnerablity_get_ordinal(struct cvrf_vulnerability *vuln) {
	return vuln->ordinal;
}

struct cvrf_vulnerability *cvrf_vulnerability_new() {

	struct cvrf_vulnerability *ret;

	ret = oscap_alloc(sizeof(struct cvrf_vulnerability));
	if (ret == NULL)
		return NULL;

	ret->cvrf_product_statuses = oscap_list_new();
	ret->remediations = oscap_list_new();
	ret->cve_id = NULL;
	ret->cwe_id = NULL;
	ret->vulnerability_title = NULL;

	return ret;
}

void cvrf_vulnerability_free(struct cvrf_vulnerability *vulnerability) {

	if (vulnerability == NULL)
		return;

	oscap_free(vulnerability->vulnerability_title);
	oscap_free(vulnerability->cve_id);
	oscap_free(vulnerability->cwe_id);
	oscap_list_free(vulnerability->cvrf_product_statuses, (oscap_destruct_func) cvrf_product_status_free);
	oscap_list_free(vulnerability->remediations, (oscap_destruct_func) cvrf_remediation_free);
	oscap_free(vulnerability);
}


/***************************************************************************
 * CVRF Remediation
 */
struct cvrf_remediation {
	char *remed_type;
	char *remed_date;
	char *remed_description;
	char *remed_URL;
	char *remed_entitlement;
	char *remed_product_id;
	char *remed_group_id;
};
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_type)
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_date)
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_description)
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_URL)
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_entitlement)
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_product_id)
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_group_id)


struct cvrf_remediation *cvrf_remediation_new() {

	struct cvrf_remediation *ret;
	ret = oscap_alloc(sizeof(struct cvrf_remediation));
	if (ret == NULL)
		return NULL;

	ret->remed_type = NULL;
	ret->remed_date = NULL;
	ret->remed_description = NULL;
	ret->remed_URL = NULL;
	ret->remed_entitlement = NULL;
	ret->remed_product_id = NULL;
	ret->remed_group_id = NULL;

	return ret;
}

void cvrf_remediation_free(struct cvrf_remediation *remed) {

	if (remed == NULL)
		return;

	oscap_free(remed->remed_type);
	oscap_free(remed->remed_date);
	oscap_free(remed->remed_description);
	oscap_free(remed->remed_URL);
	oscap_free(remed->remed_entitlement);
	oscap_free(remed->remed_product_id);
	oscap_free(remed->remed_group_id);
	oscap_free(remed);
}


/***************************************************************************
 * CVRF ProductStatus
 */
struct cvrf_product_status {
	char *status;
	struct oscap_stringlist *product_ids;
};
OSCAP_ACCESSOR_STRING(cvrf_product_status, status)

struct oscap_string_iterator *cvrf_product_status_get_ids(struct cvrf_product_status *stat) {
	return oscap_stringlist_get_strings(stat->product_ids);
}

struct cvrf_product_status *cvrf_product_status_new() {

	struct cvrf_product_status *ret;

	ret = oscap_alloc(sizeof(struct cvrf_product_status));
	if (ret == NULL)
		return NULL;

	ret->product_ids = oscap_stringlist_new();
	ret->status = NULL;

	return ret;
}

void cvrf_product_status_free(struct cvrf_product_status *status) {

	if (status == NULL)
		return;

	oscap_free(status->status);
	oscap_stringlist_free(status->product_ids);
	oscap_free(status);
}

/* End of CVRF structure definitions
 ***************************************************************************/


/****************************************************************************
/* XML string variables definitions
 * */

#define TAG_CVRF_DOC BAD_CAST "cvrfdoc"
#define TAG_CVRF_DOC_TITLE BAD_CAST "DocumentTitle"
#define TAG_CVRF_DOC_TYPE BAD_CAST "DocumentType"
#define ATTR_DOC_TITLE_LANG BAD_CAST "xml:lang"
//Document
#define TAG_DOCUMENT_TRACKING BAD_CAST "DocumentTracking"
#define TAG_TRACKING_IDENTIFICATION BAD_CAST "Identification"
#define TAG_TRACKING_ID BAD_CAST "ID"
#define TAG_TRACKING_ALIAS BAD_CAST "Alias"
#define TAG_TRACKING_STATUS BAD_CAST "Status"
#define TAG_TRACKING_VERSION BAD_CAST "Version"
#define TAG_TRACKING_INIT_RELEASE BAD_CAST "InitialReleaseDate"
#define TAG_TRACKING_CUR_RELEASE BAD_CAST "CurrentReleaseDate"
#define TAG_GENERATOR BAD_CAST "Generator"
#define TAG_GENERATOR_ENGINE BAD_CAST "Engine"
#define TAG_GENERATOR_DATE BAD_CAST "Date"
// Product Tree
#define TAG_CVRF_PRODUCT_TREE BAD_CAST "ProductTree"
#define TAG_BRANCH_TYPE BAD_CAST "BranchType"
#define TAG_BRANCH BAD_CAST "Branch"
#define ATTR_BRANCH_TYPE BAD_CAST "Type"
#define ATTR_BRANCH_NAME BAD_CAST "Name"
#define TAG_CVRF_PRODUCT_NAME BAD_CAST "FullProductName"
#define ATTR_PRODUCT_ID BAD_CAST "ProductID"
//Relationship
#define TAG_RELATIONSHIP BAD_CAST "Relationship"
#define ATTR_PRODUCT_REFERENCE BAD_CAST "ProductReference"
#define ATTR_RELATION_TYPE BAD_CAST "RelationType"
#define ATTR_RELATES_TO_REF BAD_CAST "RelatesToProductReference"
#define TAG_GROUP_ID BAD_CAST "GroupID"
// Vulnerabilities
#define TAG_VULNERABILITY BAD_CAST "Vulnerability"
#define ATTR_VULNERABILITY_ORDINAL BAD_CAST "Ordinal"
#define TAG_VULNERABILITY_TITLE BAD_CAST "Title"
#define TAG_VULNERABILITY_ID BAD_CAST "ID"
#define TAG_VULNERABILITY_CVE BAD_CAST "CVE"
#define TAG_VULNERABILITY_CWE BAD_CAST "CWE"
#define TAG_CVRF_PRODUCT_STATUSES BAD_CAST "ProductStatuses"
#define TAG_CVRF_PRODUCT_STATUS BAD_CAST "Status"
#define TAG_PRODUCT_ID BAD_CAST "ProductID"
#define ATTR_STATUS_TYPE BAD_CAST "Type"
// Remediations
#define TAG_REMEDIATIONS BAD_CAST "Remediations"
#define TAG_REMEDIATION BAD_CAST "Remediation"
#define TAG_REMEDIATION_DESC BAD_CAST "Description"
#define TAG_REMEDIATION_URL BAD_CAST "URL"
// namespaces
#define CVRF_NS BAD_CAST "http://www.icasi.org/CVRF/schema/cvrf/1.1"
#define PROD_NS BAD_CAST "http://www.icasi.org/CVRF/schema/prod/1.1"
#define VULN_NS BAD_CAST "http://www.icasi.org/CVRF/schema/vuln/1.1"

/* End of XML variable definitions
 *
 ***************************************************************************/

/****************************************************************************
 * Parsing functions
 *
 */

struct cvrf_index *cvrf_index_parse_xml(const char *index_file) {

	__attribute__nonnull__(index_file);

	struct cvrf_index *index = cvrf_index_new();
	cvrf_index_set_index_file(index, index_file);
	struct oscap_stringlist *file_list = oscap_stringlist_new();
	struct cvrf_model *model;

	FILE* file = fopen(index_file, "r");
	char line[256];

	while (fgets(line, sizeof(line), file)) {
		printf("%s", line);
		oscap_trim(line);
		oscap_stringlist_add_string(file_list, line);
	}
	fclose(file);

	struct oscap_string_iterator *iterator = oscap_stringlist_get_strings(file_list);
	while (oscap_string_iterator_has_more(iterator)) {
		model = cvrf_model_parse_xml(oscap_string_iterator_next(iterator));
		if (model)
			oscap_list_add(index->models, model);
	}
	oscap_string_iterator_free(iterator);

	return index;
}

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
	struct cvrf_doc_tracking *tracking = NULL;
	struct cvrf_vulnerability *vulnerability = NULL;
	struct cvrf_product_tree *tree = NULL;
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

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_DOC_TYPE)) {
			ret->doc_type = oscap_element_string_copy(reader);
			xmlTextReaderNextElement(reader);
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DOCUMENT_TRACKING)) {
			tracking = cvrf_doc_tracking_parse(reader);
			cvrf_document_set_tracking(ret->document, tracking);
			xmlTextReaderNextElement(reader);
		}

		tree = cvrf_product_tree_parse(reader);
		ret->tree = tree;

		while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY) == 0) {
			vulnerability = cvrf_vulnerability_parse(reader);
			if (vulnerability)
				cvrf_model_add_vulnerability(ret, vulnerability);
			xmlTextReaderNextElement(reader);
		}
	}

	return ret;
}

struct cvrf_doc_tracking *cvrf_doc_tracking_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_doc_tracking *tracking;

	tracking = cvrf_doc_tracking_new();
	if (tracking == NULL)
		return NULL;

	if (xmlTextReaderIsEmptyElement(reader))
		return tracking;

	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DOCUMENT_TRACKING) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_IDENTIFICATION)) {
			xmlTextReaderNextElement(reader);
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_ID)) {
				tracking->tracking_id = (char *)oscap_element_string_copy(reader);
			}
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_ALIAS)) {
				tracking->tracking_alias = (char *)oscap_element_string_copy(reader);
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_STATUS)) {
			tracking->tracking_status = (char *)oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_VERSION)) {
			tracking->tracking_version = (int)oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_INIT_RELEASE)) {
			tracking->init_release_date = (char *)oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_CUR_RELEASE)) {
			tracking->cur_release_date = (char *)oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR)) {
			xmlTextReaderNextElement(reader);
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR_ENGINE)) {
				tracking->generator_engine = (char *)oscap_element_string_copy(reader);
				xmlTextReaderNextElement(reader);
			}
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR_DATE)) {
				tracking->generator_date = (char *)oscap_element_string_copy(reader);
			}
		}

		xmlTextReaderNextNode(reader);
	}

	return tracking;
}

struct cvrf_product_tree *cvrf_product_tree_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_product_tree *tree;
	struct cvrf_branch *branch;
	struct cvrf_relationship *relation;
	struct cvrf_product_name *full_name;

	/* allocate platform structure here */
	tree = cvrf_product_tree_new();
	if (tree == NULL)
		return NULL;

	/* If <empty /> then return, because there is no child element */
	if (xmlTextReaderIsEmptyElement(reader))
		return tree;

	/* skip from <entry> node to next one */
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_PRODUCT_TREE) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_PRODUCT_NAME)) {
			full_name = cvrf_product_name_new();
			full_name->product_id = (char *)xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_ID);
			full_name->cpe = (char *)oscap_element_string_copy(reader);
			tree->full_name = full_name;
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH)) {
			while(xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH) == 0) {
				branch = cvrf_branch_parse(reader);
				if (branch)
					oscap_list_add(tree->branches, branch);
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_RELATIONSHIP)) {
			relation = cvrf_relationship_parse(reader);
			if (relation)
				oscap_list_add(tree->relationships, relation);
		}
		xmlTextReaderNextNode(reader);
	}

	xmlTextReaderNextElement(reader);

	return tree;
}

struct cvrf_branch *cvrf_branch_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_branch *branch;
	struct cvrf_branch *subbranch;
	struct cvrf_product_name *full_name;

	/* allocate platform structure here */
	branch = cvrf_branch_new();
	if (branch == NULL)
		return NULL;

	branch->branch_name = (char *)xmlTextReaderGetAttribute(reader, ATTR_BRANCH_NAME);
	branch->branch_type = (char *)xmlTextReaderGetAttribute(reader, ATTR_BRANCH_TYPE);
	xmlTextReaderNextElement(reader);

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_PRODUCT_NAME)) {
		full_name = cvrf_product_name_new();
		full_name->product_id = (char *)xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_ID);
		full_name->cpe = (char *)oscap_element_string_copy(reader);
		branch->full_name = full_name;
		xmlTextReaderNextNode(reader);
		xmlTextReaderNextNode(reader);
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
		}
		xmlTextReaderNextNode(reader);
	}
	return branch;
}

struct cvrf_relationship *cvrf_relationship_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_relationship *relation;
	struct cvrf_product_name *full_name;
	relation = cvrf_relationship_new();

	if (relation == NULL)
		return NULL;

	relation->product_reference = (char *)xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_REFERENCE);
	relation->relation_type = (char *)xmlTextReaderGetAttribute(reader, ATTR_RELATION_TYPE);
	relation->relates_to_ref = (char *)xmlTextReaderGetAttribute(reader, ATTR_RELATES_TO_REF);
	xmlTextReaderNextElement(reader);

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_PRODUCT_NAME)) {
		full_name = cvrf_product_name_new();
		full_name->product_id = (char *)xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_ID);
		full_name->cpe = (char *)oscap_element_string_copy(reader);
		relation->full_name = full_name;
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);

	return relation;
}

struct cvrf_vulnerability *cvrf_vulnerability_parse(xmlTextReaderPtr reader) {

	__attribute__nonnull__(reader);

	struct cvrf_vulnerability *vulnerability = NULL;
	struct cvrf_product_status *stat = NULL;
	struct cvrf_remediation *remed = NULL;

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
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_PRODUCT_STATUSES)) {
			xmlTextReaderNextElement(reader);
			while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_PRODUCT_STATUS) == 0) {
				stat = cvrf_product_status_parse(reader);
				if (stat != NULL)
					cvrf_vulnerability_add_cvrf_product_status(vulnerability, stat);
				xmlTextReaderNextNode(reader);
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_PRODUCT_STATUS)) {
			stat = cvrf_product_status_parse(reader);
			if (stat != NULL)
				cvrf_vulnerability_add_cvrf_product_status(vulnerability, stat);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATIONS)) {
			xmlTextReaderNextElement(reader);
			while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATION) == 0) {
				remed = cvrf_remediation_parse(reader);
				if (remed != NULL)
					cvrf_vulnerability_add_remediation(vulnerability, remed);
				xmlTextReaderNextNode(reader);
			}
		}

		xmlTextReaderNextNode(reader);
	}

	return vulnerability;
}

struct cvrf_remediation *cvrf_remediation_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_remediation *remed = NULL;
	remed = cvrf_remediation_new();
	if (remed == NULL)
		return NULL;

	remed->remed_type = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "Type");
	remed->remed_date = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "Date");
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATION) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATION_DESC)) {
			remed->remed_description = (char *)oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATION_URL)) {
			remed->remed_URL = (char *)oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_ID)) {
			remed->remed_product_id = (char *)oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GROUP_ID)) {
			remed->remed_group_id = (char *)oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "Entitlement")) {
			remed->remed_entitlement = (char *)oscap_element_string_copy(reader);
		}

		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);

	return remed;
}


struct cvrf_product_status *cvrf_product_status_parse(xmlTextReaderPtr reader) {

	__attribute__nonnull__(reader);

	struct cvrf_product_status *stat;

	stat = cvrf_product_status_new();
	if (stat == NULL)
		return NULL;

	stat->status = (char *)xmlTextReaderGetAttribute(reader, ATTR_STATUS_TYPE);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_PRODUCT_STATUS) != 0) {

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
void cvrf_index_export_xml(struct cvrf_index *index, const char *file) {

	__attribute__nonnull__(index);
	__attribute__nonnull__(file);

	struct xmlTextWriterPtr *writer = xmlNewTextWriterFilename(file, 0);
	if (writer == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return;
	}

	xmlTextWriterSetIndent(writer, 1);
	xmlTextWriterSetIndentString(writer, BAD_CAST "  ");
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

	struct cvrf_model_iterator *models = cvrf_index_get_models(index);
	while (cvrf_model_iterator_has_more(models)) {
		struct cvrf_model *model = cvrf_model_iterator_next(models);
		cvrf_export(model, writer);
	}
	cvrf_model_iterator_free(models);

	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

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

	xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_DOC_TYPE, NULL);
	xmlTextWriterWriteString(writer, BAD_CAST cvrf->doc_type);
	xmlTextWriterEndElement(writer);

	cvrf_doc_tracking_export(cvrf_document_get_tracking(cvrf->document), writer);
	cvrf_product_tree_export(cvrf->tree, writer);
	OSCAP_FOREACH(cvrf_vulnerability, e, cvrf_model_get_vulnerabilities(cvrf), cvrf_vulnerability_export(e, writer);)

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cvrf_doc_tracking_export(const struct cvrf_doc_tracking *tracking, xmlTextWriterPtr writer) {
	__attribute__nonnull__(tracking);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_DOCUMENT_TRACKING, NULL);

	if (tracking->tracking_id != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_TRACKING_IDENTIFICATION, NULL);
		xmlTextWriterStartElementNS(writer, NULL, TAG_TRACKING_ID, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST tracking->tracking_id);
		xmlTextWriterEndElement(writer);
		if (tracking->tracking_alias != NULL) {
			xmlTextWriterStartElementNS(writer, NULL, TAG_TRACKING_ALIAS, NULL);
			xmlTextWriterWriteString(writer, BAD_CAST tracking->tracking_alias);
			xmlTextWriterEndElement(writer);
		}
		xmlTextWriterEndElement(writer);
	}
	if (tracking->tracking_status != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_TRACKING_STATUS, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST tracking->tracking_status);
		xmlTextWriterEndElement(writer);
	}
	if (tracking->tracking_version != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_TRACKING_VERSION, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST tracking->tracking_version);
		xmlTextWriterEndElement(writer);
	}
	if (tracking->init_release_date != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_TRACKING_INIT_RELEASE, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST tracking->init_release_date);
		xmlTextWriterEndElement(writer);
	}
	if (tracking->cur_release_date != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_TRACKING_CUR_RELEASE, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST tracking->cur_release_date);
		xmlTextWriterEndElement(writer);
	}
	if (tracking->generator_engine != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_GENERATOR, NULL);
		xmlTextWriterStartElementNS(writer, NULL, TAG_GENERATOR_ENGINE, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST tracking->generator_engine);
		xmlTextWriterEndElement(writer);
		if (tracking->generator_date != NULL) {
			xmlTextWriterStartElementNS(writer, NULL, TAG_GENERATOR_DATE, NULL);
			xmlTextWriterWriteString(writer, BAD_CAST tracking->generator_date);
			xmlTextWriterEndElement(writer);
		}
		xmlTextWriterEndElement(writer);
	}

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cvrf_product_tree_export(const struct cvrf_product_tree *tree, xmlTextWriterPtr writer) {
	__attribute__nonnull__(tree);
	__attribute__nonnull__(writer);

	struct cvrf_product_name *name = tree->full_name;
	xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_PRODUCT_TREE, PROD_NS);

	if (name->cpe != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_PRODUCT_NAME, NULL);
		xmlTextWriterWriteAttribute(writer, BAD_CAST "ProductID", BAD_CAST name->product_id);
		xmlTextWriterWriteString(writer, BAD_CAST name->cpe);
		xmlTextWriterEndElement(writer);
	}

	struct oscap_iterator *branches = cvrf_product_tree_get_branches(tree);
	while (oscap_iterator_has_more(branches)) {
		struct cvrf_branch *branch = oscap_iterator_next(branches);
		cvrf_branch_export(branch, writer);
	}
	oscap_iterator_free(branches);

	struct cvrf_relationship_iterator *relationships = cvrf_product_tree_get_relationships(tree);
	while (cvrf_relationship_iterator_has_more(relationships)) {
		struct cvrf_relationship *relation = cvrf_relationship_iterator_next(relationships);
		cvrf_relationship_export(relation, writer);
	}
	cvrf_relationship_iterator_free(relationships);

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}


void cvrf_branch_export(const struct cvrf_branch *branch, xmlTextWriterPtr writer) {
	__attribute__nonnull__(branch);
	__attribute__nonnull__(writer);

	struct cvrf_product_name *name = branch->full_name;
	xmlTextWriterStartElementNS(writer, NULL, TAG_BRANCH, NULL);
	xmlTextWriterWriteAttribute(writer, ATTR_BRANCH_TYPE, BAD_CAST branch->branch_type);
	xmlTextWriterWriteAttribute(writer, ATTR_BRANCH_NAME, BAD_CAST branch->branch_name);

	if (name->cpe != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_PRODUCT_NAME, NULL);
		xmlTextWriterWriteAttribute(writer, BAD_CAST "ProductID", BAD_CAST name->product_id);
		xmlTextWriterWriteString(writer, BAD_CAST name->cpe);
		xmlTextWriterEndElement(writer);
	}
	if (oscap_list_get_itemcount(branch->subbranches) > 0) {
		struct oscap_iterator *subbranches = cvrf_branch_get_subbranches(branch);
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

void cvrf_relationship_export(const struct cvrf_relationship *relation, xmlTextWriterPtr writer) {
	__attribute__nonnull__(relation);
	__attribute__nonnull__(writer);

	struct cvrf_product_name *name = relation->full_name;

	xmlTextWriterStartElementNS(writer, NULL, TAG_RELATIONSHIP, NULL);
	xmlTextWriterWriteAttribute(writer, ATTR_PRODUCT_REFERENCE, BAD_CAST relation->product_reference);
	xmlTextWriterWriteAttribute(writer, ATTR_RELATION_TYPE, BAD_CAST relation->relation_type);
	xmlTextWriterWriteAttribute(writer, ATTR_RELATES_TO_REF, BAD_CAST relation->relates_to_ref);

	if (name->cpe != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_PRODUCT_NAME, NULL);
		xmlTextWriterWriteAttribute(writer, BAD_CAST "ProductID", BAD_CAST name->product_id);
		xmlTextWriterWriteString(writer, BAD_CAST name->cpe);
		xmlTextWriterEndElement(writer);
	}

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());

}

void cvrf_vulnerability_export(const struct cvrf_vulnerability *vuln, xmlTextWriterPtr writer) {
	__attribute__nonnull__(vuln);
	__attribute__nonnull__(writer);

	struct cvrf_product_status_iterator *iterator = cvrf_vulnerability_get_cvrf_product_statuses(vuln);
	struct cvrf_product_status *stat;

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
	if (oscap_list_get_itemcount(vuln->cvrf_product_statuses) > 0) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_PRODUCT_STATUSES, NULL);
		OSCAP_FOREACH(cvrf_product_status, e, cvrf_vulnerability_get_cvrf_product_statuses(vuln), cvrf_product_status_export(e, writer);)
		xmlTextWriterEndElement(writer);
	}
	if (oscap_list_get_itemcount(vuln->remediations) > 0) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_REMEDIATIONS, NULL);
		OSCAP_FOREACH(cvrf_remediation, e, cvrf_vulnerability_get_remediations(vuln), cvrf_remediation_export(e, writer);)
		xmlTextWriterEndElement(writer);
	}

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cvrf_remediation_export(const struct cvrf_remediation *remed, xmlTextWriterPtr writer) {
	__attribute__nonnull__(remed);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_REMEDIATION, NULL);
	xmlTextWriterWriteAttribute(writer, BAD_CAST "Type", BAD_CAST remed->remed_type);
	if (remed->remed_date != NULL)
		xmlTextWriterWriteAttribute(writer, BAD_CAST "Date", BAD_CAST remed->remed_date);

	xmlTextWriterStartElementNS(writer, NULL, TAG_REMEDIATION_DESC, NULL);
	xmlTextWriterWriteAttribute(writer, ATTR_DOC_TITLE_LANG, BAD_CAST "en");
	xmlTextWriterWriteString(writer, BAD_CAST remed->remed_description);
	xmlTextWriterEndElement(writer);

	if (remed->remed_URL != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_REMEDIATION_URL, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST remed->remed_URL);
		xmlTextWriterEndElement(writer);
	}
	if (remed->remed_entitlement != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "Entitlement", NULL);
		xmlTextWriterWriteString(writer, BAD_CAST remed->remed_entitlement);
		xmlTextWriterEndElement(writer);
	}
	if (remed->remed_product_id != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_ID, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST remed->remed_product_id);
		xmlTextWriterEndElement(writer);
	}
	if (remed->remed_group_id != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_GROUP_ID, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST remed->remed_group_id);
		xmlTextWriterEndElement(writer);
	}

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cvrf_product_status_export(const struct cvrf_product_status *stat, xmlTextWriterPtr writer) {
	__attribute__nonnull__(stat);
	__attribute__nonnull__(writer);

	struct oscap_string_iterator *product_ids = cvrf_product_status_get_ids(stat);
	const char *product_id;

	xmlTextWriterStartElementNS(writer, NULL, TAG_CVRF_PRODUCT_STATUS, NULL);
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

/* End of export functions
 * */
/***************************************************************************/

