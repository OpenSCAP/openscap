#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <libxml/xmlreader.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include <libxml/xpathInternals.h>

#include "public/cvrf.h"
#include "cvrf_priv.h"

#include "common/list.h"
#include "common/_error.h"
#include "common/xmltext_priv.h"
#include "common/elements.h"
#include "common/oscap_string.h"
#include "common/util.h"

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

	ret->doc_title = NULL;
	ret->doc_type = NULL;
	ret->document = cvrf_document_new();
	ret->tree = cvrf_product_tree_new();
	ret->vulnerabilities = oscap_list_new();

	return ret;
}

void cvrf_model_free(struct cvrf_model *cvrf) {

	if (cvrf == NULL)
		return;

	oscap_free(cvrf->doc_title);
	oscap_free(cvrf->doc_type);
	cvrf_document_free(cvrf->document);
	cvrf_product_tree_free(cvrf->tree);
	oscap_list_free(cvrf->vulnerabilities, (oscap_destruct_func) cvrf_vulnerability_free);
	oscap_free(cvrf);
}


/***************************************************************************
 * CVRF Document
 */
struct cvrf_document {
	struct cvrf_doc_tracking *tracking;
	struct cvrf_doc_publisher *publisher;
};

struct cvrf_doc_tracking *cvrf_document_get_tracking(struct cvrf_document *doc) {
	return doc->tracking;
}
void cvrf_document_set_tracking(struct cvrf_document *doc, struct cvrf_doc_tracking *track) {
	doc->tracking = track;
}

struct cvrf_doc_publisher *cvrf_document_get_publisher(struct cvrf_document *doc) {
	return doc->publisher;
}
void cvrf_document_set_publisher(struct cvrf_document *doc, struct cvrf_doc_publisher *publisher) {
	doc->publisher = publisher;
}


struct cvrf_document *cvrf_document_new() {

	struct cvrf_document *ret;

	ret = oscap_alloc(sizeof(struct cvrf_document));
	if (ret == NULL)
		return NULL;

	ret->tracking = cvrf_doc_tracking_new();
	ret->publisher = cvrf_doc_publisher_new();

	return ret;
}

void cvrf_document_free(struct cvrf_document *doc) {

	if (doc == NULL)
		return;

	cvrf_doc_tracking_free(doc->tracking);
	cvrf_doc_publisher_free(doc->publisher);
	oscap_free(doc);
}


/***************************************************************************
 * CVRF DocumentPublisher
 */

struct cvrf_doc_publisher {
	cvrf_doc_publisher_type_t publisher_type;
	char *contact_details;
	char *issuing_authority;
};
OSCAP_ACCESSOR_STRING(cvrf_doc_publisher, contact_details)
OSCAP_ACCESSOR_STRING(cvrf_doc_publisher, issuing_authority)

cvrf_doc_publisher_type_t cvrf_doc_publisher_get_type(struct cvrf_doc_publisher *publisher) {
	return publisher->publisher_type;
}


struct cvrf_doc_publisher *cvrf_doc_publisher_new() {

	struct cvrf_doc_publisher *ret;

	ret = oscap_alloc(sizeof(struct cvrf_doc_publisher));
	if (ret == NULL)
		return NULL;

	ret->contact_details = NULL;
	ret->issuing_authority = NULL;

	return ret;
}

void cvrf_doc_publisher_free(struct cvrf_doc_publisher *publisher) {

	if (publisher == NULL)
		return;

	oscap_free(publisher->contact_details);
	oscap_free(publisher->issuing_authority);
	oscap_free(publisher);
}


/***************************************************************************
 * CVRF DocumentTracking
 */
struct cvrf_doc_tracking {
	char *tracking_id;
	char *tracking_alias;
	cvrf_doc_status_type_t tracking_status;
	int tracking_version;
	char *init_release_date;
	char *cur_release_date;

	// Generator
	char *generator_engine;
	char *generator_date;
};
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, tracking_id)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, tracking_alias)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, init_release_date)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, cur_release_date)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, generator_engine)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, generator_date)

cvrf_doc_status_type_t cvrf_doc_tracking_get_tracking_status(struct cvrf_doc_tracking *tracking) {
	return tracking->tracking_status;
}

struct cvrf_doc_tracking *cvrf_doc_tracking_new() {

	struct cvrf_doc_tracking *ret;

	ret = oscap_alloc(sizeof(struct cvrf_doc_tracking));
	if (ret == NULL)
		return NULL;

	ret->tracking_id = NULL;
	ret->tracking_alias = NULL;
	ret->init_release_date = NULL;
	ret->cur_release_date = NULL;
	ret->generator_engine = NULL;
	ret->generator_date = NULL;

	return ret;
}

void cvrf_doc_tracking_free(struct cvrf_doc_tracking *tracking) {

	if (tracking == NULL)
		return;

	oscap_free(tracking->tracking_id);
	oscap_free(tracking->tracking_alias);
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

	ret->full_name = cvrf_product_name_new();
	ret->branches = oscap_list_new();
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
	cvrf_branch_type_t branch_type;
	char *branch_name;
	struct cvrf_product_name *full_name;
	struct oscap_list *subbranches;
};
OSCAP_ACCESSOR_STRING(cvrf_branch, branch_name)

struct oscap_iterator *cvrf_branch_get_subbranches(struct cvrf_branch *branch) {
	return oscap_iterator_new(branch->subbranches);
}
struct cvrf_product_name *cvrf_branch_get_cvrf_product_name(struct cvrf_branch *branch) {
	return branch->full_name;
}
cvrf_branch_type_t cvrf_branch_get_branch_type(struct cvrf_branch *branch) {
	return branch->branch_type;
}

struct cvrf_branch *cvrf_branch_new() {

	struct cvrf_branch *ret;

	ret = oscap_alloc(sizeof(struct cvrf_branch));
	if (ret == NULL)
		return NULL;

	ret->branch_name = NULL;
	ret->full_name = cvrf_product_name_new();
	ret->subbranches = oscap_list_new();

	return ret;
}

void cvrf_branch_free(struct cvrf_branch *branch) {

	if (branch == NULL)
		return;

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
	cvrf_relationship_type_t relation_type;
	char *relates_to_ref;
	struct cvrf_product_name *full_name;
};
OSCAP_ACCESSOR_STRING(cvrf_relationship, product_reference)
OSCAP_ACCESSOR_STRING(cvrf_relationship, relates_to_ref)

struct cvrf_product_name *cvrf_relationship_get_product_name(struct cvrf_relationship *relation) {
	return relation->full_name;
}
cvrf_relationship_type_t cvrf_relationship_get_relation_type(struct cvrf_relationship *relation) {
	return relation->relation_type;
}

struct cvrf_relationship *cvrf_relationship_new() {

	struct cvrf_relationship *ret;

	ret = oscap_alloc(sizeof(struct cvrf_relationship));
	if (ret == NULL)
		return NULL;

	ret->product_reference = NULL;
	ret->relates_to_ref = NULL;
	ret->full_name = cvrf_product_name_new();

	return ret;
}

void cvrf_relationship_free(struct cvrf_relationship *relationship) {
	if (relationship == NULL)
		return;

	oscap_free(relationship->product_reference);
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

	ret->product_id = NULL;
	ret->cpe = NULL;

	return ret;
}

void cvrf_product_name_free(struct cvrf_product_name *full_name) {
	if (full_name == NULL)
		return;

	oscap_free(full_name->product_id);
	oscap_free(full_name->cpe);
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
	char *vulnerability_id;
	char *discovery_date;
	char *release_date;
	char *cve_id;
	char *cwe_id;

	// Product Status
	struct oscap_list *cvrf_product_statuses;
	// Remediations
	struct oscap_list *remediations;
	// threats
	struct oscap_list *threats;

};
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, vulnerability_title)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, vulnerability_id)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, discovery_date)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, release_date)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, cve_id)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, cwe_id)
OSCAP_IGETINS_GEN(cvrf_product_status, cvrf_vulnerability, cvrf_product_statuses, cvrf_product_status)
OSCAP_ITERATOR_REMOVE_F(cvrf_product_status)
OSCAP_IGETINS_GEN(cvrf_remediation, cvrf_vulnerability, remediations, remediation)
OSCAP_ITERATOR_REMOVE_F(cvrf_remediation)
OSCAP_IGETINS_GEN(cvrf_threat, cvrf_vulnerability, threats, threat)
OSCAP_ITERATOR_REMOVE_F(cvrf_threat)

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

	ret->vulnerability_title = NULL;
	ret->vulnerability_id = NULL;
	ret->discovery_date = NULL;
	ret->release_date = NULL;
	ret->cve_id = NULL;
	ret->cwe_id = NULL;
	ret->cvrf_product_statuses = oscap_list_new();
	ret->remediations = oscap_list_new();
	ret->threats = oscap_list_new();

	return ret;
}

void cvrf_vulnerability_free(struct cvrf_vulnerability *vulnerability) {

	if (vulnerability == NULL)
		return;

	oscap_free(vulnerability->vulnerability_title);
	oscap_free(vulnerability->vulnerability_id);
	oscap_free(vulnerability->discovery_date);
	oscap_free(vulnerability->release_date);
	oscap_free(vulnerability->cve_id);
	oscap_free(vulnerability->cwe_id);
	oscap_list_free(vulnerability->cvrf_product_statuses, (oscap_destruct_func) cvrf_product_status_free);
	oscap_list_free(vulnerability->remediations, (oscap_destruct_func) cvrf_remediation_free);
	oscap_list_free(vulnerability->threats, (oscap_destruct_func) cvrf_threat_free);
	oscap_free(vulnerability);
}


/***************************************************************************
 * CVRF threat
 */
struct cvrf_threat {
	cvrf_threat_type_t threat_type;
	char *threat_date;
	char *threat_description;
	struct oscap_stringlist *threat_product_ids;
	struct oscap_stringlist *threat_group_ids;
};
OSCAP_ACCESSOR_STRING(cvrf_threat, threat_date)
OSCAP_ACCESSOR_STRING(cvrf_threat, threat_description)

cvrf_threat_type_t cvrf_threat_get_threat_type(struct cvrf_threat *threat) {
	return threat->threat_type;
}
struct oscap_string_iterator *cvrf_threat_get_product_ids(struct cvrf_threat *threat) {
	return oscap_stringlist_get_strings(threat->threat_product_ids);
}
struct oscap_string_iterator *cvrf_threat_get_group_ids(struct cvrf_threat *threat) {
	return oscap_stringlist_get_strings(threat->threat_group_ids);
}

struct cvrf_threat *cvrf_threat_new() {

	struct cvrf_threat *ret;

	ret = oscap_alloc(sizeof(struct cvrf_threat));
	if (ret == NULL)
		return NULL;

	ret->threat_date = NULL;
	ret->threat_description = NULL;
	ret->threat_product_ids = oscap_stringlist_new();
	ret->threat_group_ids = oscap_stringlist_new();

	return ret;
}

void cvrf_threat_free(struct cvrf_threat *threat) {

	if (threat == NULL)
		return;

	oscap_free(threat->threat_date);
	oscap_free(threat->threat_description);
	oscap_stringlist_free(threat->threat_product_ids);
	oscap_stringlist_free(threat->threat_group_ids);
	oscap_free(threat);
}


/***************************************************************************
 * CVRF Remediation
 */
struct cvrf_remediation {
	cvrf_remediation_type_t remed_type;
	char *remed_date;
	char *remed_description;
	char *remed_URL;
	char *remed_entitlement;
	struct oscap_stringlist *remed_product_ids;
	struct oscap_stringlist *remed_group_ids;
};
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_date)
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_description)
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_URL)
OSCAP_ACCESSOR_STRING(cvrf_remediation, remed_entitlement)

cvrf_remediation_type_t cvrf_remediation_get_type(struct cvrf_remediation *remed) {
	return remed->remed_type;
}

struct oscap_string_iterator *cvrf_remediation_get_product_ids(struct cvrf_remediation *remed) {
	return oscap_stringlist_get_strings(remed->remed_product_ids);
}
struct oscap_string_iterator *cvrf_remediation_get_group_ids(struct cvrf_remediation *remed) {
	return oscap_stringlist_get_strings(remed->remed_group_ids);
}

struct cvrf_remediation *cvrf_remediation_new() {

	struct cvrf_remediation *ret;
	ret = oscap_alloc(sizeof(struct cvrf_remediation));
	if (ret == NULL)
		return NULL;

	ret->remed_date = NULL;
	ret->remed_description = NULL;
	ret->remed_URL = NULL;
	ret->remed_entitlement = NULL;
	ret->remed_product_ids = oscap_stringlist_new();
	ret->remed_group_ids = oscap_stringlist_new();

	return ret;
}

void cvrf_remediation_free(struct cvrf_remediation *remed) {

	if (remed == NULL)
		return;

	oscap_free(remed->remed_date);
	oscap_free(remed->remed_description);
	oscap_free(remed->remed_URL);
	oscap_free(remed->remed_entitlement);
	oscap_stringlist_free(remed->remed_product_ids);
	oscap_stringlist_free(remed->remed_group_ids);
	oscap_free(remed);
}


/***************************************************************************
 * CVRF ProductStatus
 */
struct cvrf_product_status {
	cvrf_product_status_type_t status_type;
	struct oscap_stringlist *product_ids;
};

struct oscap_string_iterator *cvrf_product_status_get_ids(struct cvrf_product_status *stat) {
	return oscap_stringlist_get_strings(stat->product_ids);
}
cvrf_product_status_type_t cvrf_product_status_get_type(struct cvrf_product_status *stat) {
	return stat->status_type;
}

struct cvrf_product_status *cvrf_product_status_new() {

	struct cvrf_product_status *ret;

	ret = oscap_alloc(sizeof(struct cvrf_product_status));
	if (ret == NULL)
		return NULL;

	ret->product_ids = oscap_stringlist_new();

	return ret;
}

void cvrf_product_status_free(struct cvrf_product_status *status) {

	if (status == NULL)
		return;

	oscap_stringlist_free(status->product_ids);
	oscap_free(status);
}

/* End of CVRF structure definitions
 ***************************************************************************/


/****************************************************************************
/* XML string variables definitions
 * */

#define TAG_CVRF_DOC BAD_CAST "cvrfdoc"
#define TAG_DOC_TITLE BAD_CAST "DocumentTitle"
#define TAG_DOC_TYPE BAD_CAST "DocumentType"
#define ATTR_DOC_TITLE_LANG BAD_CAST "xml:lang"
// DocumentPublisher
#define TAG_PUBLISHER BAD_CAST "DocumentPublisher"
#define TAG_CONTACT_DETAILS BAD_CAST "ContactDetails"
#define TAG_ISSUING_AUTHORITY BAD_CAST "IssuingAuthority"
//Document
#define TAG_DOCUMENT_TRACKING BAD_CAST "DocumentTracking"
#define TAG_IDENTIFICATION BAD_CAST "Identification"
#define TAG_TRACKING_ID BAD_CAST "ID"
#define TAG_TRACKING_ALIAS BAD_CAST "Alias"
#define TAG_TRACKING_STATUS BAD_CAST "Status"
#define TAG_TRACKING_VERSION BAD_CAST "Version"
#define TAG_GENERATOR BAD_CAST "Generator"
#define TAG_GENERATOR_ENGINE BAD_CAST "Engine"
#define TAG_GENERATOR_DATE BAD_CAST "Date"
// Product Tree
#define TAG_PRODUCT_TREE BAD_CAST "ProductTree"
#define TAG_BRANCH BAD_CAST "Branch"
#define ATTR_BRANCH_NAME BAD_CAST "Name"
#define TAG_PRODUCT_NAME BAD_CAST "FullProductName"
#define ATTR_PRODUCT_ID BAD_CAST "ProductID"
//Relationship
#define TAG_RELATIONSHIP BAD_CAST "Relationship"
#define ATTR_PRODUCT_REFERENCE BAD_CAST "ProductReference"
#define ATTR_RELATES_TO_REF BAD_CAST "RelatesToProductReference"
#define TAG_GROUP_ID BAD_CAST "GroupID"
// Vulnerabilities
#define TAG_VULNERABILITY BAD_CAST "Vulnerability"
#define ATTR_VULNERABILITY_ORDINAL BAD_CAST "Ordinal"
#define TAG_VULNERABILITY_TITLE BAD_CAST "Title"
#define TAG_VULNERABILITY_ID BAD_CAST "ID"
#define TAG_VULNERABILITY_DISCOVERY BAD_CAST "DiscoveryDate"
#define TAG_VULNERABILITY_RELEASE BAD_CAST "ReleaseDate"
#define TAG_VULNERABILITY_CVE BAD_CAST "CVE"
#define TAG_VULNERABILITY_CWE BAD_CAST "CWE"
#define TAG_PRODUCT_STATUSES BAD_CAST "ProductStatuses"
#define TAG_PRODUCT_STATUS BAD_CAST "Status"
#define TAG_PRODUCT_ID BAD_CAST "ProductID"
#define ATTR_STATUS_TYPE BAD_CAST "Type"
// Remediations
#define TAG_REMEDIATIONS BAD_CAST "Remediations"
#define TAG_REMEDIATION BAD_CAST "Remediation"
#define TAG_REMEDIATION_DESC BAD_CAST "Description"
#define TAG_REMEDIATION_URL BAD_CAST "URL"
// Threats
#define TAG_THREATS BAD_CAST "Threats"
#define TAG_THREAT BAD_CAST "Threat"
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

struct cvrf_index *cvrf_index_parse_xml(struct oscap_source *index_source) {

	__attribute__nonnull__(index_source);

	struct cvrf_index *index = cvrf_index_new();
	cvrf_index_set_index_file(index, oscap_source_readable_origin(index_source));
	struct oscap_stringlist *file_list = oscap_stringlist_new();
	struct cvrf_model *model;

	FILE* file = fopen(oscap_source_get_filepath(index_source), "r");
	char line[256];

	while (fgets(line, sizeof(line), file)) {
		printf("%s", line);
		oscap_trim(line);
		oscap_stringlist_add_string(file_list, line);
	}
	fclose(file);

	struct oscap_string_iterator *iterator = oscap_stringlist_get_strings(file_list);
	while (oscap_string_iterator_has_more(iterator)) {
		const char *filename = oscap_string_iterator_next(iterator);
		model = cvrf_model_import(oscap_source_new_from_file(filename));
		if (model)
			oscap_list_add(index->models, model);
	}
	oscap_string_iterator_free(iterator);
	oscap_source_free(index_source);

	return index;
}

struct cvrf_model *cvrf_model_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_DOC) ||
			!(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT))
		return NULL;

	struct cvrf_model *ret = cvrf_model_new();
	xmlTextReaderNextElement(reader);

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DOC_TITLE)) {
		ret->doc_title = oscap_element_string_copy(reader);
		xmlTextReaderNextElement(reader);
	}
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DOC_TYPE)) {
		ret->doc_type = oscap_element_string_copy(reader);
		xmlTextReaderNextElement(reader);
	}
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PUBLISHER)) {
		struct cvrf_doc_publisher *publisher = cvrf_doc_publisher_parse(reader);
		cvrf_document_set_publisher(ret->document, publisher);
		xmlTextReaderNextElement(reader);
	}
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DOCUMENT_TRACKING)) {
		struct cvrf_doc_tracking *tracking = cvrf_doc_tracking_parse(reader);
		cvrf_document_set_tracking(ret->document, tracking);
		xmlTextReaderNextElement(reader);
	}

	struct cvrf_product_tree *tree = cvrf_product_tree_parse(reader);
	ret->tree = tree;

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY) == 0) {
		struct cvrf_vulnerability *vulnerability = cvrf_vulnerability_parse(reader);
		if (vulnerability) {
			cvrf_model_add_vulnerability(ret, vulnerability);
		} else {
			oscap_setxmlerr("Could not parse CVRF file: Missing or invalid"
					"Vulnerability element\n");
		}
		xmlTextReaderNextElement(reader);
	}

	return ret;
}

struct cvrf_doc_publisher *cvrf_doc_publisher_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_doc_publisher *publisher = cvrf_doc_publisher_new();
	if (publisher == NULL)
		return NULL;

	if (xmlTextReaderIsEmptyElement(reader))
		return publisher;

	publisher->publisher_type = cvrf_doc_publisher_type_parse(reader, "Type");
	xmlTextReaderNextElement(reader);

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CONTACT_DETAILS)) {
		publisher->contact_details = oscap_element_string_copy(reader);
		xmlTextReaderNextElement(reader);
	}
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ISSUING_AUTHORITY)) {
		publisher->issuing_authority = oscap_element_string_copy(reader);
	}
	xmlTextReaderNextNode(reader);

	return publisher;
}

struct cvrf_doc_tracking *cvrf_doc_tracking_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_doc_tracking *tracking = cvrf_doc_tracking_new();
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

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_IDENTIFICATION)) {
			xmlTextReaderNextElement(reader);
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_ID)) {
				tracking->tracking_id = oscap_element_string_copy(reader);
			}
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_ALIAS)) {
				tracking->tracking_alias = oscap_element_string_copy(reader);
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_STATUS)) {
			tracking->tracking_status = cvrf_doc_status_type_parse(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_VERSION)) {
			tracking->tracking_version = (int)oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "InitialReleaseDate")) {
			tracking->init_release_date = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "CurrentReleaseDate")) {
			tracking->cur_release_date = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR)) {
			xmlTextReaderNextElement(reader);
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR_ENGINE)) {
				tracking->generator_engine = oscap_element_string_copy(reader);
				xmlTextReaderNextElement(reader);
			}
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR_DATE)) {
				tracking->generator_date = oscap_element_string_copy(reader);
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

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_TREE) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME)) {
			full_name = cvrf_product_name_new();
			full_name->product_id = xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_ID);
			full_name->cpe = oscap_element_string_copy(reader);
			tree->full_name = full_name;
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH)) {
			while(xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH) == 0) {
				branch = cvrf_branch_parse(reader);
				if (branch) {
					oscap_list_add(tree->branches, branch);
				} else {
					oscap_setxmlerr("Could not parse CVRF file: Missing or invalid"
							"Branch element\n");
				}
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_RELATIONSHIP)) {
			relation = cvrf_relationship_parse(reader);
			if (relation) {
				oscap_list_add(tree->relationships, relation);
			} else {
				oscap_setxmlerr("Could not parse CVRF file: Missing or invalid "
						"Relationship element\n");
			}
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

	branch->branch_name = xmlTextReaderGetAttribute(reader, ATTR_BRANCH_NAME);
	branch->branch_type = cvrf_branch_type_parse(reader, "Type");
	xmlTextReaderNextElement(reader);

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME)) {
		full_name = cvrf_product_name_new();
		full_name->product_id = (char *)xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_ID);
		full_name->cpe = oscap_element_string_copy(reader);
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
			} else {
				oscap_setxmlerr("Could not parse CVRF file: Missing or invalid"
						"Branch element\n");
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

	relation->product_reference = xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_REFERENCE);
	relation->relation_type = cvrf_relationship_type_parse(reader, "RelationType");
	relation->relates_to_ref = xmlTextReaderGetAttribute(reader, ATTR_RELATES_TO_REF);
	xmlTextReaderNextElement(reader);

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME)) {
		full_name = cvrf_product_name_new();
		full_name->product_id = (char *)xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_ID);
		full_name->cpe = oscap_element_string_copy(reader);
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
	struct cvrf_threat *threat = NULL;

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
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_ID)) {
			vulnerability->vulnerability_id = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_DISCOVERY)) {
			vulnerability->discovery_date = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_RELEASE)) {
			vulnerability->release_date = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_CVE)) {
			vulnerability->cve_id = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_CWE)) {
			vulnerability->cwe_id = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STATUSES)) {
			xmlTextReaderNextElement(reader);
			while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STATUS) == 0) {
				stat = cvrf_product_status_parse(reader);
				if (stat) {
					cvrf_vulnerability_add_cvrf_product_status(vulnerability, stat);
				} else {
					oscap_setxmlerr("Could not parse CVRF file: Missing or invalid"
							"ProductStatus element\n");
				}
				xmlTextReaderNextNode(reader);
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STATUS)) {
			stat = cvrf_product_status_parse(reader);
			if (stat != NULL)
				cvrf_vulnerability_add_cvrf_product_status(vulnerability, stat);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATIONS)) {
			xmlTextReaderNextElement(reader);
			while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATION) == 0) {
				remed = cvrf_remediation_parse(reader);
				if (remed != NULL) {
					cvrf_vulnerability_add_remediation(vulnerability, remed);
				} else {
					oscap_setxmlerr("Could not parse CVRF file: Missing or invalid"
							"Remediation element\n");
				}
				xmlTextReaderNextNode(reader);
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_THREATS)) {
					xmlTextReaderNextElement(reader);
					while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_THREAT) == 0) {
						threat = cvrf_threat_parse(reader);
						if (threat != NULL) {
							cvrf_vulnerability_add_threat(vulnerability, threat);
						} else {
							oscap_setxmlerr("Could not parse CVRF file: Missing or invalid"
									"Threat element\n");
						}
						xmlTextReaderNextNode(reader);
					}
				}
		xmlTextReaderNextNode(reader);
	}

	return vulnerability;
}

struct cvrf_threat *cvrf_threat_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_threat *threat = NULL;
	threat = cvrf_threat_new();
	if (threat == NULL)
		return NULL;

	threat->threat_type = cvrf_threat_type_parse(reader, "Type");
	threat->threat_date = xmlTextReaderGetAttribute(reader, BAD_CAST "Date");
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_THREAT) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "Description")) {
			threat->threat_description = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_ID)) {
			oscap_stringlist_add_string(threat->threat_product_ids, oscap_element_string_copy(reader));
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GROUP_ID)) {
			oscap_stringlist_add_string(threat->threat_group_ids, oscap_element_string_copy(reader));
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);

	return threat;
}

struct cvrf_remediation *cvrf_remediation_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_remediation *remed = NULL;
	remed = cvrf_remediation_new();
	if (remed == NULL)
		return NULL;

	remed->remed_type = cvrf_remediation_type_parse(reader, "Type");
	remed->remed_date = xmlTextReaderGetAttribute(reader, BAD_CAST "Date");
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATION) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATION_DESC)) {
			remed->remed_description = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATION_URL)) {
			remed->remed_URL = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_ID)) {
			oscap_stringlist_add_string(remed->remed_product_ids, oscap_element_string_copy(reader));
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GROUP_ID)) {
			oscap_stringlist_add_string(remed->remed_group_ids, oscap_element_string_copy(reader));
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "Entitlement")) {
			remed->remed_entitlement = oscap_element_string_copy(reader);
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

	stat->status_type = cvrf_product_status_type_parse(reader, "Type");
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


void cvrf_stringlist_to_dom(struct oscap_stringlist *list, const char *tag_name, xmlNode *parent) {
	if (oscap_list_get_itemcount(list) == 0)
		return;

	struct oscap_string_iterator *iterator = oscap_stringlist_get_strings(list);
	while (oscap_string_iterator_has_more(iterator)) {
		const char *string = oscap_string_iterator_next(iterator);
		xmlNode *elm_node = xmlNewTextChild(parent, NULL, BAD_CAST tag_name, BAD_CAST string);
	}
	oscap_string_iterator_free(iterator);
}

void cvrf_element_to_dom(const char *elm_name, const char *elm_value, xmlNode *parent) {
	if (elm_value == NULL)
		return;

	xmlNode *elm_node = xmlNewTextChild(parent, NULL, BAD_CAST elm_name, BAD_CAST elm_value);
}

/*
void cvrf_index_export_xml(struct cvrf_index *index, struct oscap_source *export_source) {

	__attribute__nonnull__(index);
	__attribute__nonnull__(export_source);

	const char *filepath = oscap_source_get_filepath(export_source);
	struct xmlTextWriterPtr *writer = xmlNewTextWriterFilename(filepath, 0);
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
		cvrf_model_export_xml(model, writer);
	}
	cvrf_model_iterator_free(models);

	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	oscap_source_free(export_source);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}*/


void cvrf_product_status_to_dom(const struct cvrf_product_status *stat, xmlNode *status_node) {
	const char *status_type = cvrf_product_status_type_get_text(stat->status_type);
	xmlNewProp(status_node, BAD_CAST "Type", BAD_CAST status_type);

	cvrf_stringlist_to_dom(stat->product_ids, "ProductID", status_node);
}

void cvrf_remediation_to_dom(const struct cvrf_remediation *remed, xmlNode *remed_node) {

	const char *remed_type = cvrf_remediation_type_get_text(remed->remed_type);
	xmlNewProp(remed_node, BAD_CAST "Type", BAD_CAST remed_type);

	cvrf_element_to_dom("Description", remed->remed_description, remed_node);
	cvrf_element_to_dom("URL", remed->remed_URL, remed_node);
	cvrf_element_to_dom("Entitlement", remed->remed_entitlement, remed_node);
	cvrf_stringlist_to_dom(remed->remed_product_ids, "ProductID", remed_node);
	cvrf_stringlist_to_dom(remed->remed_group_ids, "GroupID", remed_node);
}

void cvrf_threat_to_dom(const struct cvrf_threat *threat, xmlNode *threat_node) {

	const char *threat_type = cvrf_threat_type_get_text(threat->threat_type);
	xmlNewProp(threat_node, BAD_CAST "Type", BAD_CAST threat_type);
	if (threat->threat_date)
		xmlNewProp(threat_node, BAD_CAST "Date", BAD_CAST threat->threat_date);

	cvrf_element_to_dom("Description", threat->threat_description, threat_node);
	cvrf_stringlist_to_dom(threat->threat_product_ids, "ProductID", threat_node);
	cvrf_stringlist_to_dom(threat->threat_group_ids, "GroupID", threat_node);
}

void cvrf_vulnerability_to_dom(const struct cvrf_vulnerability *vuln, xmlNode *vuln_node) {

	//xmlNewProp(vuln_node, ATTR_VULNERABILITY_ORDINAL, BAD_CAST vuln->ordinal);

	cvrf_element_to_dom("Title", vuln->vulnerability_title, vuln_node);
	cvrf_element_to_dom("ID", vuln->vulnerability_id, vuln_node);
	cvrf_element_to_dom("DiscoveryDate", vuln->discovery_date, vuln_node);
	cvrf_element_to_dom("ReleaseDate", vuln->release_date, vuln_node);
	cvrf_element_to_dom("CVE", vuln->cve_id, vuln_node);
	cvrf_element_to_dom("CWE", vuln->cwe_id, vuln_node);

	if (oscap_list_get_itemcount(vuln->cvrf_product_statuses) > 0) {
		xmlNode *statuses_node = xmlNewTextChild(vuln_node, NULL, TAG_PRODUCT_STATUSES, NULL);
		struct cvrf_product_status_iterator *statuses = cvrf_vulnerability_get_cvrf_product_statuses(vuln);
		while (cvrf_product_status_iterator_has_more(statuses)) {
			xmlNode *status_node = xmlNewTextChild(statuses_node, NULL, TAG_PRODUCT_STATUS, NULL);
			cvrf_product_status_to_dom(cvrf_product_status_iterator_next(statuses), status_node);
		}
		cvrf_product_status_iterator_free(statuses);
	}
	if (oscap_list_get_itemcount(vuln->threats) > 0) {
		xmlNode *threats_node = xmlNewTextChild(vuln_node, NULL, TAG_THREATS, NULL);
		struct cvrf_threat_iterator *threats = cvrf_vulnerability_get_threats(vuln);
		while (cvrf_threat_iterator_has_more(threats)) {
			xmlNode *threat_node = xmlNewTextChild(threats_node, NULL, TAG_THREAT, NULL);
			cvrf_threat_to_dom(cvrf_threat_iterator_next(threats), threat_node);
		}
		cvrf_threat_iterator_free(threats);

	}
	if (oscap_list_get_itemcount(vuln->remediations) > 0) {
		xmlNode *remediations_node = xmlNewTextChild(vuln_node, NULL, TAG_REMEDIATIONS, NULL);
		struct cvrf_remediation_iterator *remediations = cvrf_vulnerability_get_remediations(vuln);
		while (cvrf_remediation_iterator_has_more(remediations)) {
			xmlNode *remediation_node = xmlNewTextChild(remediations_node, NULL, TAG_REMEDIATION, NULL);
			cvrf_remediation_to_dom(cvrf_remediation_iterator_next(remediations), remediation_node);
		}
		cvrf_remediation_iterator_free(remediations);
	}
}

xmlNode *cvrf_model_to_dom(struct cvrf_model *model, xmlDocPtr doc, xmlNode *parent, void *user_args) {

	xmlNode *root_node = NULL;
	if (parent == NULL) {
		root_node = xmlNewNode(NULL, BAD_CAST "cvrfdoc");
		xmlDocSetRootElement(doc, root_node);
	} else {
		root_node = xmlNewTextChild(parent, NULL, BAD_CAST "cvrfdoc", NULL);
	}
	xmlNs *cvrf_ns = xmlNewNs(root_node, CVRF_NS, NULL);

	xmlNode *title_node = xmlNewTextChild(root_node, NULL, TAG_DOC_TITLE, BAD_CAST model->doc_title);
	xmlNode *type_node = xmlNewTextChild(root_node, NULL, TAG_DOC_TYPE, BAD_CAST model->doc_type);

	xmlNode *doc_publisher = xmlNewTextChild(root_node, NULL, TAG_PUBLISHER, NULL);
	cvrf_doc_publisher_to_dom(cvrf_document_get_publisher(model->document), doc_publisher);

	xmlNode *doc_tracking = xmlNewTextChild(root_node, NULL, TAG_DOCUMENT_TRACKING, NULL);
	cvrf_doc_tracking_to_dom(cvrf_document_get_tracking(model->document), doc_tracking);

	xmlNode *product_tree = xmlNewTextChild(root_node, NULL, TAG_PRODUCT_TREE, NULL);
	cvrf_product_tree_to_dom(model->tree, product_tree);

	struct cvrf_vulnerability_iterator *vulnerabilities = cvrf_model_get_vulnerabilities(model);
	while (cvrf_vulnerability_iterator_has_more(vulnerabilities)) {
		xmlNode *vuln_node = xmlNewTextChild(root_node, NULL, TAG_VULNERABILITY, NULL);
		cvrf_vulnerability_to_dom(cvrf_vulnerability_iterator_next(vulnerabilities), vuln_node);
	}
	cvrf_vulnerability_iterator_free(vulnerabilities);

	return root_node;
}

void cvrf_doc_publisher_to_dom(struct cvrf_doc_publisher *publisher, xmlNode *pub_node) {

	const char *publisher_type = cvrf_doc_publisher_type_get_text(publisher->publisher_type);
	xmlNewProp(pub_node, BAD_CAST "Type", BAD_CAST publisher_type);

	xmlNode *contact_node = xmlNewTextChild(pub_node, NULL, TAG_CONTACT_DETAILS,
			BAD_CAST publisher->contact_details);
	xmlNode *issuer_node = xmlNewTextChild(pub_node, NULL, TAG_ISSUING_AUTHORITY,
			BAD_CAST publisher->issuing_authority);
}

void cvrf_doc_tracking_to_dom(struct cvrf_doc_tracking *tracking, xmlNode *tracking_node) {

	if (tracking->tracking_id) {
		xmlNode *ident_node = xmlNewTextChild(tracking_node, NULL, TAG_IDENTIFICATION, NULL);
		cvrf_element_to_dom("ID", tracking->tracking_id, ident_node);
		cvrf_element_to_dom("Alias", tracking->tracking_alias, ident_node);
	}

	const char *tracking_status = cvrf_doc_status_type_get_text(tracking->tracking_status);
	xmlNode *status_node = xmlNewTextChild(tracking_node, NULL, BAD_CAST "Status",
			BAD_CAST tracking_status);

	cvrf_element_to_dom("InitialReleaseDate", tracking->init_release_date, tracking_node);
	cvrf_element_to_dom("CurrentReleaseDate", tracking->cur_release_date, tracking_node);

	if (tracking->generator_engine) {
		xmlNode *generator_node = xmlNewTextChild(tracking_node, NULL, BAD_CAST "Generator", NULL);
		cvrf_element_to_dom("Engine", tracking->generator_engine, generator_node);
		cvrf_element_to_dom("Date", tracking->generator_date, generator_node);
	}
}

void cvrf_product_name_to_dom(struct cvrf_product_name *full_name, xmlNode *parent) {

	if (full_name->cpe == NULL)
		return;

	xmlNode *name_node = xmlNewTextChild(parent, NULL, TAG_PRODUCT_NAME,
			BAD_CAST full_name->cpe);
	xmlNewProp(name_node, ATTR_PRODUCT_ID, BAD_CAST full_name->product_id);
}

void cvrf_product_tree_to_dom(struct cvrf_product_tree *tree, xmlNode *tree_node) {

	cvrf_product_name_to_dom(tree->full_name, tree_node);

	struct oscap_iterator *branches = cvrf_product_tree_get_branches(tree);
	while (oscap_iterator_has_more(branches)) {
		xmlNode *branch_node = xmlNewTextChild(tree_node, NULL, TAG_BRANCH, NULL);
		cvrf_branch_to_dom(oscap_iterator_next(branches), branch_node);
	}
	oscap_iterator_free(branches);

	struct cvrf_relationship_iterator *relationships = cvrf_product_tree_get_relationships(tree);
	while (cvrf_relationship_iterator_has_more(relationships)) {
		xmlNode *relation_node = xmlNewTextChild(tree_node, NULL, TAG_RELATIONSHIP, NULL);
		cvrf_relationship_to_dom(cvrf_relationship_iterator_next(relationships), relation_node);
	}
	cvrf_relationship_iterator_free(relationships);
}

void cvrf_branch_to_dom(struct cvrf_branch *branch, xmlNode *branch_node) {

	const char *branch_type = cvrf_branch_type_get_text(branch->branch_type);
	xmlNewProp(branch_node, BAD_CAST "Type", BAD_CAST branch_type);
	xmlNewProp(branch_node, BAD_CAST "Name", BAD_CAST branch->branch_name);

	if (branch->branch_type == CVRF_BRANCH_PRODUCT_FAMILY) {
		struct oscap_iterator *subbranches = cvrf_branch_get_subbranches(branch);
		while (oscap_iterator_has_more(subbranches)) {
			xmlNode *subbranch_node = xmlNewTextChild(branch_node, NULL, TAG_BRANCH, NULL);
			cvrf_branch_to_dom(oscap_iterator_next(subbranches), subbranch_node);
		}
		oscap_iterator_free(subbranches);
	} else {
		cvrf_product_name_to_dom(branch->full_name, branch_node);
	}
}

void cvrf_relationship_to_dom(const struct cvrf_relationship *relation, xmlNode *relation_node) {

	const char *relation_type = cvrf_relationship_type_get_text(relation->relation_type);
	xmlNewProp(relation_node, ATTR_PRODUCT_REFERENCE, BAD_CAST relation->product_reference);
	xmlNewProp(relation_node, BAD_CAST "RelationType", BAD_CAST relation_type);
	xmlNewProp(relation_node, ATTR_RELATES_TO_REF, BAD_CAST relation->relates_to_ref);

	cvrf_product_name_to_dom(relation->full_name, relation_node);
}







/* End of export functions
 * */
/***************************************************************************/

