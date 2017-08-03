/*
 * Copyright 2017 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
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
	char *doc_distribution;
	char *aggregate_severity;
	char *namespace;
	struct cvrf_doc_tracking *tracking;
	struct cvrf_doc_publisher *publisher;
	struct oscap_list *doc_references;
};
OSCAP_ACCESSOR_STRING(cvrf_document, doc_distribution)
OSCAP_ACCESSOR_STRING(cvrf_document, aggregate_severity)
OSCAP_ACCESSOR_STRING(cvrf_document, namespace)

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
struct oscap_iterator *cvrf_document_get_references(struct cvrf_document *doc) {
	return oscap_iterator_new(doc->doc_references);
}


struct cvrf_document *cvrf_document_new() {

	struct cvrf_document *ret;

	ret = oscap_alloc(sizeof(struct cvrf_document));
	if (ret == NULL)
		return NULL;

	ret->doc_distribution = NULL;
	ret->aggregate_severity = NULL;
	ret->namespace = NULL;
	ret->tracking = cvrf_doc_tracking_new();
	ret->publisher = cvrf_doc_publisher_new();
	ret->doc_references = oscap_list_new();

	return ret;
}

void cvrf_document_free(struct cvrf_document *doc) {

	if (doc == NULL)
		return;

	oscap_free(doc->doc_distribution);
	oscap_free(doc->aggregate_severity);
	oscap_free(doc->namespace);
	cvrf_doc_tracking_free(doc->tracking);
	cvrf_doc_publisher_free(doc->publisher);
	oscap_list_free(doc->doc_references, (oscap_destruct_func) cvrf_reference_free);
	oscap_free(doc);
}


/***************************************************************************
 * CVRF DocumentPublisher
 */

struct cvrf_doc_publisher {
	cvrf_doc_publisher_type_t publisher_type;
	char *vendor_id;
	char *contact_details;
	char *issuing_authority;
};
OSCAP_ACCESSOR_STRING(cvrf_doc_publisher, vendor_id)
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

	ret->vendor_id = NULL;
	ret->contact_details = NULL;
	ret->issuing_authority = NULL;

	return ret;
}

void cvrf_doc_publisher_free(struct cvrf_doc_publisher *publisher) {

	if (publisher == NULL)
		return;

	oscap_free(publisher->vendor_id);
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
	char *tracking_version;
	struct oscap_list *revision_history;
	char *init_release_date;
	char *cur_release_date;
	// Generator
	char *generator_engine;
	char *generator_date;
};
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, tracking_id)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, tracking_alias)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, tracking_version)
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
	ret->tracking_version = NULL;
	ret->revision_history = oscap_list_new();
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
	oscap_free(tracking->tracking_version);
	oscap_list_free(tracking->revision_history, (oscap_destruct_func) cvrf_revision_free);
	oscap_free(tracking->init_release_date);
	oscap_free(tracking->cur_release_date);
	oscap_free(tracking->generator_engine);
	oscap_free(tracking->generator_date);
	oscap_free(tracking);
}


/***************************************************************************
 * CVRF Revision
 */

struct cvrf_revision {
	char *number;
	char *date;
	char *description;
};
OSCAP_ACCESSOR_STRING(cvrf_revision, number)
OSCAP_ACCESSOR_STRING(cvrf_revision, date)
OSCAP_ACCESSOR_STRING(cvrf_revision, description)

struct cvrf_revision *cvrf_revision_new() {

	struct cvrf_revision *ret;
	ret = oscap_alloc(sizeof(struct cvrf_revision));
	if (ret == NULL)
		return NULL;

	ret->number = NULL;
	ret->date = NULL;
	ret->description = NULL;

	return ret;
}

void cvrf_revision_free(struct cvrf_revision *revision) {

	if (revision == NULL)
		return;

	oscap_free(revision->number);
	oscap_free(revision->date);
	oscap_free(revision->description);
	oscap_free(revision);
}

/***************************************************************************
 * CVRF References
 */

struct cvrf_reference {
	cvrf_reference_type_t ref_type;
	char *ref_url;
	char *ref_description;
};
OSCAP_ACCESSOR_STRING(cvrf_reference, ref_url)
OSCAP_ACCESSOR_STRING(cvrf_reference, ref_description)

cvrf_reference_type_t cvrf_reference_get_reference_type(struct cvrf_reference *reference) {
	return reference->ref_type;
}

struct cvrf_reference *cvrf_reference_new() {
	struct cvrf_reference *ret;
	ret = oscap_alloc(sizeof(struct cvrf_reference));
	if (ret == NULL)
		return NULL;

	ret->ref_url = NULL;
	ret->ref_description = NULL;

	return ret;
}

void cvrf_reference_free(struct cvrf_reference *ref) {

	if (ref == NULL)
		return;

	oscap_free(ref->ref_url);
	oscap_free(ref->ref_description);
	oscap_free(ref);
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
	struct oscap_stringlist *cwe_ids;

	struct oscap_list *product_statuses;
	struct oscap_list *threats;
	struct oscap_list *remediations;
	struct oscap_list *references;

};
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, vulnerability_title)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, vulnerability_id)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, discovery_date)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, release_date)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, cve_id)
OSCAP_IGETINS_GEN(cvrf_product_status, cvrf_vulnerability, product_statuses, cvrf_product_status)
OSCAP_ITERATOR_REMOVE_F(cvrf_product_status)
OSCAP_IGETINS_GEN(cvrf_remediation, cvrf_vulnerability, remediations, remediation)
OSCAP_ITERATOR_REMOVE_F(cvrf_remediation)
OSCAP_IGETINS_GEN(cvrf_threat, cvrf_vulnerability, threats, threat)
OSCAP_ITERATOR_REMOVE_F(cvrf_threat)

struct oscap_string_iterator *cvrf_vulnerability_get_cwe_ids(struct cvrf_vulnerability *vuln) {
	return oscap_stringlist_get_strings(vuln->cwe_ids);
}
struct oscap_iterator *cvrf_vulnerability_get_references(struct cvrf_vulnerability *vuln) {
	return oscap_iterator_new(vuln->references);
}
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
	ret->cwe_ids = oscap_stringlist_new();
	ret->product_statuses = oscap_list_new();
	ret->threats = oscap_list_new();
	ret->remediations = oscap_list_new();
	ret->references = oscap_list_new();

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
	oscap_stringlist_free(vulnerability->cwe_ids);
	oscap_list_free(vulnerability->product_statuses, (oscap_destruct_func) cvrf_product_status_free);
	oscap_list_free(vulnerability->threats, (oscap_destruct_func) cvrf_threat_free);
	oscap_list_free(vulnerability->remediations, (oscap_destruct_func) cvrf_remediation_free);
	oscap_list_free(vulnerability->references, (oscap_destruct_func) cvrf_reference_free);
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
#define ATTR_LANG BAD_CAST "xml:lang"
#define TAG_DISTRIBUTION BAD_CAST "DocumentDistribution"
#define TAG_AGGREGATE_SEVERITY BAD_CAST "AggregateSeverity"
#define ATTR_NAMESPACE BAD_CAST "Namespace"
// DocumentPublisher
#define TAG_PUBLISHER BAD_CAST "DocumentPublisher"
#define ATTR_VENDOR_ID BAD_CAST "VendorID"
#define TAG_CONTACT_DETAILS BAD_CAST "ContactDetails"
#define TAG_ISSUING_AUTHORITY BAD_CAST "IssuingAuthority"
//Document
#define TAG_DOCUMENT_TRACKING BAD_CAST "DocumentTracking"
#define TAG_IDENTIFICATION BAD_CAST "Identification"
#define TAG_TRACKING_ALIAS BAD_CAST "Alias"
#define TAG_REVISION_HISTORY BAD_CAST "RevisionHistory"
#define TAG_REVISION BAD_CAST "Revision"
#define TAG_GENERATOR BAD_CAST "Generator"
#define TAG_GENERATOR_ENGINE BAD_CAST "Engine"
// Reference
#define TAG_DOCUMENT_REFERENCES BAD_CAST "DocumentReferences"
#define TAG_REFERENCES BAD_CAST "References"
#define TAG_REFERENCE BAD_CAST "Reference"
// Product Tree
#define TAG_PRODUCT_TREE BAD_CAST "ProductTree"
#define TAG_BRANCH BAD_CAST "Branch"
#define ATTR_BRANCH_NAME BAD_CAST "Name"
#define TAG_PRODUCT_NAME BAD_CAST "FullProductName"
//Relationship
#define TAG_RELATIONSHIP BAD_CAST "Relationship"
#define ATTR_PRODUCT_REFERENCE BAD_CAST "ProductReference"
#define ATTR_RELATES_TO_REF BAD_CAST "RelatesToProductReference"
// Vulnerabilities
#define TAG_VULNERABILITY BAD_CAST "Vulnerability"
#define ATTR_ORDINAL BAD_CAST "Ordinal"
#define TAG_DISCOVERY_DATE BAD_CAST "DiscoveryDate"
#define TAG_RELEASE_DATE BAD_CAST "ReleaseDate"
#define TAG_VULNERABILITY_CVE BAD_CAST "CVE"
#define TAG_VULNERABILITY_CWE BAD_CAST "CWE"
#define TAG_PRODUCT_STATUSES BAD_CAST "ProductStatuses"
// Remediations
#define TAG_REMEDIATIONS BAD_CAST "Remediations"
#define TAG_REMEDIATION BAD_CAST "Remediation"
// Threats
#define TAG_THREATS BAD_CAST "Threats"
#define TAG_THREAT BAD_CAST "Threat"
// General tags
#define TAG_DATE BAD_CAST "Date"
#define TAG_DESCRIPTION BAD_CAST "Description"
#define TAG_GROUP_ID BAD_CAST "GroupID"
#define TAG_ID BAD_CAST "ID"
#define TAG_NUMBER BAD_CAST "Number"
#define TAG_PRODUCT_ID BAD_CAST "ProductID"
#define TAG_STATUS BAD_CAST "Status"
#define TAG_TITLE BAD_CAST "Title"
#define ATTR_TYPE BAD_CAST "Type"
#define TAG_URL BAD_CAST "URL"
#define TAG_VERSION BAD_CAST "Version"
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

static void cvrf_set_parsing_error(const char *element) {
	oscap_seterr(OSCAP_EFAMILY_XML, "Could not parse CVRF file: Missing or invalid"
		"%s element\n", element);
}

static void cvrf_parse_container(xmlTextReaderPtr reader, struct oscap_list *list,
		cvrf_item_type_t item_type) {

	xmlTextReaderNextElement(reader);
	const char *tag = cvrf_item_type_get_text(item_type);
	bool no_err;
	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST tag) == 0) {
		if (item_type == CVRF_REVISION) {
			no_err = oscap_list_add(list, cvrf_revision_parse(reader));
		} else if (item_type == CVRF_REFERENCE || item_type == CVRF_DOCUMENT_REFERENCE) {
			no_err = oscap_list_add(list, cvrf_reference_parse(reader));
		} else if (item_type == CVRF_PRODUCT_STATUS) {
			no_err = oscap_list_add(list, cvrf_product_status_parse(reader));
		} else if (item_type ==  CVRF_REMEDIATION) {
			no_err = oscap_list_add(list, cvrf_remediation_parse(reader));
		} else if (item_type == CVRF_THREAT) {
			no_err = oscap_list_add(list, cvrf_threat_parse(reader));
		} else if (item_type == CVRF_VULNERABILITY) {
			no_err = oscap_list_add(list, cvrf_vulnerability_parse(reader));
		}
		xmlTextReaderNextNode(reader);

		if (!no_err) {
			cvrf_set_parsing_error(tag);
			break;
		}
	}
}

static char *cvrf_parse_element(xmlTextReaderPtr reader, const char *tagname, bool next_elm) {
	char *elm_value = NULL;
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST tagname)) {
		elm_value = oscap_element_string_copy(reader);
	}
	if (next_elm)
		xmlTextReaderNextElement(reader);

	return elm_value;
}

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
	struct cvrf_document *doc = ret->document;
	xmlTextReaderNextElement(reader);

	ret->doc_title = cvrf_parse_element(reader, "DocumentTitle", true);
	ret->doc_type = cvrf_parse_element(reader, "DocumentType", true);
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PUBLISHER)) {
		struct cvrf_doc_publisher *publisher = cvrf_doc_publisher_parse(reader);
		cvrf_document_set_publisher(doc, publisher);
		xmlTextReaderNextElement(reader);
	}
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DOCUMENT_TRACKING)) {
		struct cvrf_doc_tracking *tracking = cvrf_doc_tracking_parse(reader);
		cvrf_document_set_tracking(doc, tracking);
		xmlTextReaderNextElement(reader);
	}
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DISTRIBUTION)) {
		doc->doc_distribution = oscap_element_string_copy(reader);
		xmlTextReaderNextElement(reader);
	}
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_AGGREGATE_SEVERITY)) {
		doc->namespace = (char *)xmlTextReaderGetAttribute(reader, ATTR_NAMESPACE);
		doc->aggregate_severity = oscap_element_string_copy(reader);
		xmlTextReaderNextElement(reader);
	}
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DOCUMENT_REFERENCES)) {
		cvrf_parse_container(reader, doc->doc_references, CVRF_DOCUMENT_REFERENCE);
		xmlTextReaderNextElement(reader);
	}

	ret->tree = cvrf_product_tree_parse(reader);
	cvrf_parse_container(reader, ret->vulnerabilities, CVRF_VULNERABILITY);

	xmlFreeTextReader(reader);
	return ret;
}

struct cvrf_doc_publisher *cvrf_doc_publisher_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_doc_publisher *publisher = cvrf_doc_publisher_new();
	if (xmlTextReaderIsEmptyElement(reader))
		return publisher;

	publisher->publisher_type = cvrf_doc_publisher_type_parse(reader, "Type");
	publisher->vendor_id = (char *)xmlTextReaderGetAttribute(reader, ATTR_VENDOR_ID);
	xmlTextReaderNextElement(reader);

	publisher->contact_details = cvrf_parse_element(reader, "ContactDetails", true);
	publisher->issuing_authority = cvrf_parse_element(reader, "IssuingAuthority", false);
	xmlTextReaderNextNode(reader);

	return publisher;
}

struct cvrf_doc_tracking *cvrf_doc_tracking_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_doc_tracking *tracking = cvrf_doc_tracking_new();
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
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ID)) {
				tracking->tracking_id = oscap_element_string_copy(reader);
			}
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TRACKING_ALIAS)) {
				tracking->tracking_alias = oscap_element_string_copy(reader);
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_STATUS)) {
			tracking->tracking_status = cvrf_doc_status_type_parse(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VERSION)) {
			tracking->tracking_version = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REVISION_HISTORY)) {
			cvrf_parse_container(reader, tracking->revision_history, CVRF_REVISION);
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
			if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DATE)) {
				tracking->generator_date = oscap_element_string_copy(reader);
			}
		}

		xmlTextReaderNextNode(reader);
	}

	return tracking;
}

struct cvrf_revision *cvrf_revision_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_revision *revision = cvrf_revision_new();
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REVISION) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_NUMBER)) {
			revision->number = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DATE)) {
			revision->date = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION)) {
			revision->description = oscap_element_string_copy(reader);
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);
	return revision;
}

struct cvrf_reference *cvrf_reference_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_reference *ref = cvrf_reference_new();
	if (xmlTextReaderGetAttribute(reader, BAD_CAST "Type") != NULL)
			ref->ref_type = cvrf_reference_type_parse(reader, "Type");

	xmlTextReaderNextElement(reader);
	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCE) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}
		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_URL)) {
			ref->ref_url = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION)) {
			ref->ref_description = oscap_element_string_copy(reader);
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);

	return ref;
}

struct cvrf_product_tree *cvrf_product_tree_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_product_tree *tree = cvrf_product_tree_new();
	if (xmlTextReaderIsEmptyElement(reader))
		return tree;
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_TREE) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME)) {
			tree->full_name = cvrf_product_name_parse(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH)) {
			while(xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH) == 0) {
				if (!oscap_list_add(tree->branches, cvrf_branch_parse(reader))) {
					cvrf_set_parsing_error("Branch");
				}
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_RELATIONSHIP)) {
			if (!oscap_list_add(tree->relationships, cvrf_relationship_parse(reader))) {
				cvrf_set_parsing_error("Relationship");
			}
		}
		xmlTextReaderNextNode(reader);
	}
	return tree;
}

struct cvrf_branch *cvrf_branch_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_branch *branch = cvrf_branch_new();
	branch->branch_name = (char *)xmlTextReaderGetAttribute(reader, ATTR_BRANCH_NAME);
	branch->branch_type = cvrf_branch_type_parse(reader, "Type");
	xmlTextReaderNextElement(reader);

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME)) {
		branch->full_name = cvrf_product_name_parse(reader);
		xmlTextReaderNextNode(reader);
		xmlTextReaderNextNode(reader);
	}
	else {
		while(xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH) == 0) {
			if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
				xmlTextReaderNextNode(reader);
				continue;
			}
			if (!oscap_list_add(branch->subbranches, cvrf_branch_parse(reader))) {
				cvrf_set_parsing_error("Branch");
			}
			xmlTextReaderNextNode(reader);
		}
	}
	xmlTextReaderNextNode(reader);
	return branch;
}

struct cvrf_relationship *cvrf_relationship_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_relationship *relation = cvrf_relationship_new();
	relation->product_reference = (char *)xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_REFERENCE);
	relation->relation_type = cvrf_relationship_type_parse(reader, "RelationType");
	relation->relates_to_ref = (char *)xmlTextReaderGetAttribute(reader, ATTR_RELATES_TO_REF);
	xmlTextReaderNextElement(reader);

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME)) {
		relation->full_name = cvrf_product_name_parse(reader);
	}
	xmlTextReaderNextNode(reader);

	return relation;
}

struct cvrf_product_name *cvrf_product_name_parse(xmlTextReaderPtr reader) {
	struct cvrf_product_name *full_name = cvrf_product_name_new();
	full_name->product_id = (char *)xmlTextReaderGetAttribute(reader, TAG_PRODUCT_ID);
	full_name->cpe = oscap_element_string_copy(reader);
	xmlTextReaderNextNode(reader);
	return full_name;
}

struct cvrf_vulnerability *cvrf_vulnerability_parse(xmlTextReaderPtr reader) {

	__attribute__nonnull__(reader);

	struct cvrf_vulnerability *vuln = cvrf_vulnerability_new();
	vuln->ordinal = (int)oscap_strtol((char *)xmlTextReaderGetAttribute(reader, ATTR_ORDINAL), NULL, 10);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TITLE)) {
			vuln->vulnerability_title = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ID)) {
			vuln->vulnerability_id = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DISCOVERY_DATE)) {
			vuln->discovery_date = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_RELEASE_DATE)) {
			vuln->release_date = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_CVE)) {
			vuln->cve_id = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_CWE)) {
			oscap_stringlist_add_string(vuln->cwe_ids, oscap_element_string_copy(reader));
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STATUSES)) {
			cvrf_parse_container(reader, vuln->product_statuses, CVRF_PRODUCT_STATUS);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_STATUS)) {
			struct cvrf_product_status *stat = cvrf_product_status_parse(reader);
			if (stat != NULL)
				cvrf_vulnerability_add_cvrf_product_status(vuln, stat);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATIONS)) {
			cvrf_parse_container(reader, vuln->remediations, CVRF_REMEDIATION);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_THREATS)) {
			cvrf_parse_container(reader, vuln->threats, CVRF_THREAT);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCES)) {
			cvrf_parse_container(reader, vuln->references, CVRF_REFERENCE);
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);
	return vuln;
}

struct cvrf_threat *cvrf_threat_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_threat *threat = cvrf_threat_new();
	threat->threat_type = cvrf_threat_type_parse(reader, "Type");
	threat->threat_date = (char *)xmlTextReaderGetAttribute(reader, TAG_DATE);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_THREAT) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION)) {
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

	struct cvrf_remediation *remed = cvrf_remediation_new();
	remed->remed_type = cvrf_remediation_type_parse(reader, "Type");
	remed->remed_date = (char *)xmlTextReaderGetAttribute(reader, TAG_DATE);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATION) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION)) {
			remed->remed_description = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_URL)) {
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

	struct cvrf_product_status *stat = cvrf_product_status_new();
	if (stat == NULL)
		return NULL;

	stat->status_type = cvrf_product_status_type_parse(reader, "Type");
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_STATUS) != 0) {
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

static xmlNode *cvrf_list_to_dom(struct oscap_list *list, xmlNode *parent, cvrf_item_type_t cvrf_type) {

	if (oscap_list_get_itemcount(list) == 0)
		return NULL;

	if (parent == NULL) {
		const char *container_tag = cvrf_item_type_get_container(cvrf_type);
		parent = xmlNewNode(NULL, BAD_CAST container_tag);
	}
	xmlNode *child = NULL;
	struct oscap_iterator *it = oscap_iterator_new(list);
	while (oscap_iterator_has_more(it)) {
		if (cvrf_type == CVRF_REVISION) {
			child = cvrf_revision_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_REFERENCE || cvrf_type == CVRF_DOCUMENT_REFERENCE) {
			child = cvrf_reference_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_REMEDIATION) {
			child = cvrf_remediation_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_THREAT) {
			child = cvrf_threat_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_PRODUCT_STATUS) {
			child = cvrf_product_status_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_BRANCH) {
			child = cvrf_branch_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_RELATIONSHIP) {
			child = cvrf_relationship_to_dom(oscap_iterator_next(it));
		}
		xmlAddChild(parent, child);
	}
	oscap_iterator_free(it);
	return parent;
}

void cvrf_element_add_stringlist(struct oscap_stringlist *list, const char *tag_name, xmlNode *parent) {
	if (oscap_list_get_itemcount((struct oscap_list *)list) == 0)
		return;

	struct oscap_string_iterator *iterator = oscap_stringlist_get_strings(list);
	while (oscap_string_iterator_has_more(iterator)) {
		const char *string = oscap_string_iterator_next(iterator);
		xmlNewTextChild(parent, NULL, BAD_CAST tag_name, BAD_CAST string);
	}
	oscap_string_iterator_free(iterator);
}

void cvrf_element_add_child(const char *elm_name, const char *elm_value, xmlNode *parent) {
	if (elm_value == NULL)
		return;

	xmlNode *child = cvrf_element_to_dom(elm_name, elm_value);
	xmlAddChild(parent, child);
}

xmlNode *cvrf_element_to_dom(const char *elm_name, const char *elm_value) {
	if (elm_value == NULL)
		return NULL;

	xmlNode *elm_node = xmlNewNode(NULL, BAD_CAST elm_name);
	xmlNodeAddContent(elm_node, BAD_CAST elm_value);
	return elm_node;
}

xmlNode *cvrf_index_to_dom(struct cvrf_index *index, xmlDocPtr doc, xmlNode *parent, void *user_args) {

	xmlNode *index_node = NULL;
	if (parent == NULL) {
		index_node = xmlNewNode(NULL, BAD_CAST "Index");
		xmlDocSetRootElement(doc, index_node);
	} else {
		index_node = xmlNewTextChild(parent, NULL, BAD_CAST "Index", NULL);
	}

	struct cvrf_model_iterator *models = cvrf_index_get_models(index);
	while (cvrf_model_iterator_has_more(models)) {
		struct cvrf_model *model = cvrf_model_iterator_next(models);
		cvrf_model_to_dom(model, doc, index_node, user_args);
	}
	cvrf_model_iterator_free(models);

	return index_node;
}

xmlNode *cvrf_model_to_dom(struct cvrf_model *model, xmlDocPtr doc, xmlNode *parent, void *user_args) {

	xmlNode *root_node = NULL;
	if (parent == NULL) {
		root_node = xmlNewNode(NULL, BAD_CAST "cvrfdoc");
		xmlDocSetRootElement(doc, root_node);
	} else {
		root_node = xmlNewTextChild(parent, NULL, BAD_CAST "cvrfdoc", NULL);
	}
	xmlNewNs(root_node, CVRF_NS, NULL);
	xmlNewNs(root_node, CVRF_NS, BAD_CAST "cvrf");
	struct cvrf_document *cvrf_doc = model->document;

	xmlNode *title_node = xmlNewTextChild(root_node, NULL, TAG_DOC_TITLE, BAD_CAST model->doc_title);
	xmlNewProp(title_node, ATTR_LANG, BAD_CAST "en");
	cvrf_element_add_child("DocumentType", model->doc_type, root_node);

	xmlAddChild(root_node, cvrf_doc_publisher_to_dom(cvrf_doc->publisher));
	xmlAddChild(root_node, cvrf_doc_tracking_to_dom(cvrf_doc->tracking));

	xmlNode *distribution = xmlNewTextChild(root_node, NULL, TAG_DISTRIBUTION, BAD_CAST cvrf_doc->doc_distribution);
	xmlNewProp(distribution, ATTR_LANG, BAD_CAST "en");
	xmlNode *severity = xmlNewTextChild(root_node, NULL, TAG_AGGREGATE_SEVERITY, BAD_CAST cvrf_doc->aggregate_severity);
	xmlNewProp(severity, ATTR_NAMESPACE, BAD_CAST cvrf_doc->namespace);
	xmlAddChild(root_node, cvrf_list_to_dom(cvrf_doc->doc_references, NULL, CVRF_DOCUMENT_REFERENCE));

	xmlAddChild(root_node, cvrf_product_tree_to_dom(model->tree));
	struct cvrf_vulnerability_iterator *vulnerabilities = cvrf_model_get_vulnerabilities(model);
	while (cvrf_vulnerability_iterator_has_more(vulnerabilities)) {
		xmlNode *vuln_node = cvrf_vulnerability_to_dom(cvrf_vulnerability_iterator_next(vulnerabilities));
		xmlAddChild(root_node, vuln_node);
	}
	cvrf_vulnerability_iterator_free(vulnerabilities);

	return root_node;
}

xmlNode *cvrf_doc_publisher_to_dom(struct cvrf_doc_publisher *publisher) {

	xmlNode *pub_node = xmlNewNode(NULL, TAG_PUBLISHER);
	const char *publisher_type = cvrf_doc_publisher_type_get_text(publisher->publisher_type);
	xmlNewProp(pub_node, BAD_CAST "Type", BAD_CAST publisher_type);

	cvrf_element_add_child("ContactDetails", publisher->contact_details, pub_node);
	cvrf_element_add_child("IssuingAuthority", publisher->issuing_authority, pub_node);
	return pub_node;
}

xmlNode *cvrf_doc_tracking_to_dom(struct cvrf_doc_tracking *tracking) {

	xmlNode *tracking_node = xmlNewNode(NULL, TAG_DOCUMENT_TRACKING);
	if (tracking->tracking_id) {
		xmlNode *ident_node = xmlNewTextChild(tracking_node, NULL, TAG_IDENTIFICATION, NULL);
		cvrf_element_add_child("ID", tracking->tracking_id, ident_node);
		cvrf_element_add_child("Alias", tracking->tracking_alias, ident_node);
	}

	const char *tracking_status = cvrf_doc_status_type_get_text(tracking->tracking_status);
	cvrf_element_add_child("Status", tracking_status, tracking_node);
	cvrf_element_add_child("Version", tracking->tracking_version, tracking_node);
	xmlNode *revisions_node = cvrf_list_to_dom(tracking->revision_history, NULL, CVRF_REVISION);
	xmlAddChild(tracking_node, revisions_node);
	cvrf_element_add_child("InitialReleaseDate", tracking->init_release_date, tracking_node);
	cvrf_element_add_child("CurrentReleaseDate", tracking->cur_release_date, tracking_node);

	if (tracking->generator_engine) {
		xmlNode *generator_node = xmlNewTextChild(tracking_node, NULL, BAD_CAST "Generator", NULL);
		cvrf_element_add_child("Engine", tracking->generator_engine, generator_node);
		cvrf_element_add_child("Date", tracking->generator_date, generator_node);
	}
	return tracking_node;
}

xmlNode *cvrf_revision_to_dom(struct cvrf_revision *revision) {

	xmlNode *revision_node = xmlNewNode(NULL, TAG_REVISION);
	cvrf_element_add_child("Number", revision->number, revision_node);
	cvrf_element_add_child("Date", revision->date, revision_node);
	cvrf_element_add_child("Description", revision->description, revision_node);
	return revision_node;
}

xmlNode *cvrf_reference_to_dom(struct cvrf_reference *ref) {

	xmlNode *ref_node = xmlNewNode(NULL, TAG_REFERENCE);
	const char *ref_type = cvrf_reference_type_get_text(ref->ref_type);
	if (ref_type)
		xmlNewProp(ref_node, BAD_CAST "Type", BAD_CAST ref_type);
	cvrf_element_add_child("URL", ref->ref_url, ref_node);
	cvrf_element_add_child("Description", ref->ref_description, ref_node);
	return ref_node;
}

xmlNode *cvrf_product_name_to_dom(struct cvrf_product_name *full_name) {

	if (full_name->cpe == NULL)
		return NULL;

	xmlNode *name_node = cvrf_element_to_dom("FullProductName", full_name->cpe);
	xmlNewProp(name_node, TAG_PRODUCT_ID, BAD_CAST full_name->product_id);
	return name_node;
}

xmlNode *cvrf_product_tree_to_dom(struct cvrf_product_tree *tree) {

	xmlNode *tree_node = xmlNewNode(NULL, TAG_PRODUCT_TREE);
	xmlNewNs(tree_node, PROD_NS, NULL);

	xmlAddChild(tree_node, cvrf_product_name_to_dom(tree->full_name));
	cvrf_list_to_dom(tree->branches, tree_node, CVRF_BRANCH);
	cvrf_list_to_dom(tree->relationships, tree_node, CVRF_RELATIONSHIP);
	return tree_node;
}

xmlNode *cvrf_branch_to_dom(struct cvrf_branch *branch) {

	xmlNode *branch_node = xmlNewNode(NULL, TAG_BRANCH);
	const char *branch_type = cvrf_branch_type_get_text(branch->branch_type);
	xmlNewProp(branch_node, ATTR_TYPE, BAD_CAST branch_type);
	xmlNewProp(branch_node, BAD_CAST "Name", BAD_CAST branch->branch_name);

	if (branch->branch_type == CVRF_BRANCH_PRODUCT_FAMILY) {
		cvrf_list_to_dom(branch->subbranches, branch_node, CVRF_BRANCH);
	} else {
		xmlAddChild(branch_node, cvrf_product_name_to_dom(branch->full_name));
	}
	return branch_node;
}

xmlNode *cvrf_relationship_to_dom(const struct cvrf_relationship *relation) {

	xmlNode *relation_node = xmlNewNode(NULL, TAG_RELATIONSHIP);
	const char *relation_type = cvrf_relationship_type_get_text(relation->relation_type);
	xmlNewProp(relation_node, ATTR_PRODUCT_REFERENCE, BAD_CAST relation->product_reference);
	xmlNewProp(relation_node, BAD_CAST "RelationType", BAD_CAST relation_type);
	xmlNewProp(relation_node, ATTR_RELATES_TO_REF, BAD_CAST relation->relates_to_ref);

	xmlAddChild(relation_node, cvrf_product_name_to_dom(relation->full_name));
	return relation_node;
}

xmlNode *cvrf_vulnerability_to_dom(const struct cvrf_vulnerability *vuln) {

	xmlNode *vuln_node = xmlNewNode(NULL, TAG_VULNERABILITY);
	char *ordinal = oscap_sprintf("%d", vuln->ordinal);
	xmlNewProp(vuln_node, ATTR_ORDINAL, BAD_CAST ordinal);
	xmlNewNs(vuln_node, VULN_NS, NULL);

	cvrf_element_add_child("Title", vuln->vulnerability_title, vuln_node);
	cvrf_element_add_child("ID", vuln->vulnerability_id, vuln_node);
	cvrf_element_add_child("DiscoveryDate", vuln->discovery_date, vuln_node);
	cvrf_element_add_child("ReleaseDate", vuln->release_date, vuln_node);
	cvrf_element_add_child("CVE", vuln->cve_id, vuln_node);
	cvrf_element_add_stringlist(vuln->cwe_ids, "CWE", vuln_node);

	xmlNode *statuses_node = cvrf_list_to_dom(vuln->product_statuses, NULL, CVRF_PRODUCT_STATUS);
	xmlAddChild(vuln_node, statuses_node);
	xmlNode *threat_node = cvrf_list_to_dom(vuln->threats, NULL, CVRF_THREAT);
	xmlAddChild(vuln_node, threat_node);
	xmlNode *remed_node = cvrf_list_to_dom(vuln->remediations, NULL, CVRF_REMEDIATION);
	xmlAddChild(vuln_node, remed_node);
	xmlNode *refs_node = cvrf_list_to_dom(vuln->references, NULL, CVRF_REFERENCE);
	xmlAddChild(vuln_node, refs_node);

	oscap_free(ordinal);
	return vuln_node;
}

xmlNode *cvrf_product_status_to_dom(const struct cvrf_product_status *stat) {
	xmlNode *status_node = xmlNewNode(NULL, TAG_STATUS);
	const char *status_type = cvrf_product_status_type_get_text(stat->status_type);
	xmlNewProp(status_node, BAD_CAST "Type", BAD_CAST status_type);
	cvrf_element_add_stringlist(stat->product_ids, "ProductID", status_node);
	return status_node;
}

xmlNode *cvrf_remediation_to_dom(const struct cvrf_remediation *remed) {

	xmlNode *remed_node = xmlNewNode(NULL, TAG_REMEDIATION);
	const char *remed_type = cvrf_remediation_type_get_text(remed->remed_type);
	xmlNewProp(remed_node, BAD_CAST "Type", BAD_CAST remed_type);

	xmlNode *desc_node = cvrf_element_to_dom("Description", remed->remed_description);
	xmlNewProp(desc_node, ATTR_LANG, BAD_CAST "en");
	xmlAddChild(remed_node, desc_node);
	cvrf_element_add_child("URL", remed->remed_URL, remed_node);
	cvrf_element_add_child("Entitlement", remed->remed_entitlement, remed_node);
	cvrf_element_add_stringlist(remed->remed_product_ids, "ProductID", remed_node);
	cvrf_element_add_stringlist(remed->remed_group_ids, "GroupID", remed_node);
	return remed_node;
}

xmlNode *cvrf_threat_to_dom(const struct cvrf_threat *threat) {

	xmlNode *threat_node = xmlNewNode(NULL, TAG_THREAT);
	const char *threat_type = cvrf_threat_type_get_text(threat->threat_type);
	xmlNewProp(threat_node, BAD_CAST "Type", BAD_CAST threat_type);
	if (threat->threat_date)
		xmlNewProp(threat_node, BAD_CAST "Date", BAD_CAST threat->threat_date);

	cvrf_element_add_child("Description", threat->threat_description, threat_node);
	cvrf_element_add_stringlist(threat->threat_product_ids, "ProductID", threat_node);
	cvrf_element_add_stringlist(threat->threat_group_ids, "GroupID", threat_node);
	return threat_node;
}


/* End of export functions
 * */
/***************************************************************************/

