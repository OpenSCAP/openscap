/**
 * @addtogroup CVRF
 * @{
 *
 * @file cvrf.h
 * @brief Interface to Common Vulnerability Reporting Framework
 *
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
#ifndef _CVRF_H_
#define _CVRF_H_


#include <stdbool.h>
#include <time.h>
#include "oscap.h"
#include "oscap_text.h"
#include "cvss_score.h"
#include "cpe_name.h"
#include "oval_definitions.h"


/**
 * @struct cvrf_product_status
 * Structure holding CVRF ProductStatus data (within a Vulnerability)
 * Has status type and list of ProductIDs
 */
struct cvrf_product_status;

struct oscap_string_iterator *cvrf_product_status_get_ids(struct cvrf_product_status *stat);
bool cvrf_product_status_set_status(struct cvrf_product_status *stat, const char *status);

/**
 * @struct cvrf_threat
 * Structure holding CVRF Threat data
 * Contained within a list in CVRF Vulnerability structure
 * May contain one or more ProductID(s) and/or GroupID(s)
 */
struct cvrf_threat;

const char *cvrf_threat_get_threat_date(const struct cvrf_threat *threat);
const char *cvrf_threat_get_threat_description(const struct cvrf_threat *threat);
//cvrf_threat_type_t cvrf_threat_get_threat_type(struct cvrf_threat *threat);
struct oscap_string_iterator *cvrf_threat_get_product_ids(struct cvrf_threat *threat);
struct oscap_string_iterator *cvrf_threat_get_group_ids(struct cvrf_threat *threat);

bool cvrf_threat_set_threat_date(struct cvrf_threat *threat, const char *threat_date);
bool cvrf_threat_set_threat_description(struct cvrf_threat *threat, const char *threat_description);

/**
 * @struct cvrf_remediation
 * Structure holding remediation info for a particular vulnerability
 * May reference URL, ProductID, and/or GroupID
 */
struct cvrf_remediation;

//cvrf_remediation_type_t cvrf_remediation_get_type(struct cvrf_remediation *remed);
const char *cvrf_remediation_get_remed_date(const struct cvrf_remediation *remed);
const char *cvrf_remediation_get_remed_description(const struct cvrf_remediation *remed);
const char *cvrf_remediation_get_remed_URL(const struct cvrf_remediation *remed);
const char *cvrf_remediation_get_remed_entitlement(const struct cvrf_remediation *remed);
struct oscap_string_iterator *cvrf_remediation_get_product_ids(struct cvrf_remediation *remed);
struct oscap_string_iterator *cvrf_remediation_get_group_ids(struct cvrf_remediation *remed);

bool cvrf_remediation_set_remed_date(struct cvrf_remediation *remed, const char *date);
bool cvrf_remediation_set_remed_description(struct cvrf_remediation *remed, const char *description);
bool cvrf_remediation_set_remed_URL(struct cvrf_remediation *remed, const char *url);
bool cvrf_remediation_set_remed_entitlement(struct cvrf_remediation *remed, const char *entitlement);

/**
 * @struct cvrf_score_set
 *
 *
 */
struct cvrf_score_set;

const char *cvrf_score_set_get_vector(const struct cvrf_score_set *score_set);
struct oscap_string_iterator *cvrf_score_set_get_product_ids(struct cvrf_score_set *score_set);
struct cvss_impact *cvrf_score_set_get_impact(struct cvrf_score_set *score_set);
char *cvrf_score_set_get_score(const struct cvrf_score_set *score_set, enum cvss_category category);
char *cvrf_score_set_get_base_score(const struct cvrf_score_set *score_set);
char *cvrf_score_set_get_environmental_score(const struct cvrf_score_set *score_set);
char *cvrf_score_set_get_temporal_score(const struct cvrf_score_set *score_set);

bool cvrf_score_set_set_vector(struct cvrf_score_set *score_set, const char *vector);
void cvrf_score_set_add_metric(struct cvrf_score_set *score_set, enum cvss_category category, const char *score);

/**
 * @struct cvrf_vulnerability
 * Structure holding CVRF Vulnerability data
 * Contains at least one ProductStatus
 */
struct cvrf_vulnerability;

const char *cvrf_vulnerability_get_vulnerability_title(const struct cvrf_vulnerability *vuln);
const char *cvrf_vulnerability_get_vulnerability_id(const struct cvrf_vulnerability *vuln);
const char *cvrf_vulnerability_get_discovery_date(const struct cvrf_vulnerability *vuln);
const char *cvrf_vulnerability_get_release_date(const struct cvrf_vulnerability *vuln);
const char *cvrf_vulnerability_get_cve_id(const struct cvrf_vulnerability *vuln);
int cvrf_vulnerablity_get_ordinal(struct cvrf_vulnerability *vuln);
int cvrf_vulnerability_get_remediation_count(struct cvrf_vulnerability *vuln);
struct oscap_string_iterator *cvrf_vulnerability_get_cwe_ids(struct cvrf_vulnerability *vuln);
struct oscap_iterator *cvrf_vulnerability_get_references(struct cvrf_vulnerability *vuln);

bool cvrf_vulnerability_set_vulnerability_title(struct cvrf_vulnerability *vuln, const char *vulnerability_title);
bool cvrf_vulnerability_set_vulnerability_id(struct cvrf_vulnerability *vuln, const char *id);
bool cvrf_vulnerability_set_discovery_date(struct cvrf_vulnerability *vuln, const char *discovery_date);
bool cvrf_vulnerability_set_release_date(struct cvrf_vulnerability *vuln, const char *release_date);
bool cvrf_vulnerability_set_cve_id(struct cvrf_vulnerability *vuln, const char *cve_id);

struct cvrf_score_set_iterator;
bool cvrf_vulnerability_add_score_set(struct cvrf_vulnerability *vuln, struct cvrf_score_set *score_set);
struct cvrf_score_set_iterator *cvrf_vulnerability_get_score_sets(const struct cvrf_vulnerability *vuln);
struct cvrf_score_set *cvrf_score_set_iterator_next(struct cvrf_score_set_iterator *it);
bool cvrf_score_set_iterator_has_more(struct cvrf_score_set_iterator *it);
void cvrf_score_set_iterator_free(struct cvrf_score_set_iterator *it);
void cvrf_score_set_iterator_reset(struct cvrf_score_set_iterator *it);
void cvrf_score_set_iterator_remove(struct cvrf_score_set_iterator *it);

struct cvrf_product_status_iterator;
bool cvrf_vulnerability_add_cvrf_product_status(struct cvrf_vulnerability *vuln, struct cvrf_product_status *stat);
struct cvrf_product_status_iterator *cvrf_vulnerability_get_product_statuses(const struct cvrf_vulnerability *vuln);
struct cvrf_product_status *cvrf_product_status_iterator_next(struct cvrf_product_status_iterator *it);
bool cvrf_product_status_iterator_has_more(struct cvrf_product_status_iterator *it);
void cvrf_product_status_iterator_free(struct cvrf_product_status_iterator *it);
void cvrf_product_status_iterator_reset(struct cvrf_product_status_iterator *it);
void cvrf_product_status_iterator_remove(struct cvrf_product_status_iterator *it);

struct cvrf_remediation_iterator;
bool cvrf_vulnerability_add_remediation(struct cvrf_vulnerability *vuln, struct cvrf_remediation *remed);
struct cvrf_remediation_iterator *cvrf_vulnerability_get_remediations(const struct cvrf_vulnerability *vuln);
struct cvrf_remediation *cvrf_remediation_iterator_next(struct cvrf_remediation_iterator *it);
bool cvrf_remediation_iterator_has_more(struct cvrf_remediation_iterator *it);
void cvrf_remediation_iterator_free(struct cvrf_remediation_iterator *it);
void cvrf_remediation_iterator_reset(struct cvrf_remediation_iterator *it);
void cvrf_remediation_iterator_remove(struct cvrf_remediation_iterator *it);

struct cvrf_threat_iterator;
bool cvrf_vulnerability_add_threat(struct cvrf_vulnerability *vuln, struct cvrf_threat *threat);
struct cvrf_threat_iterator *cvrf_vulnerability_get_threats(const struct cvrf_vulnerability *vuln);
struct cvrf_threat *cvrf_threat_iterator_next(struct cvrf_threat_iterator *it);
bool cvrf_threat_iterator_has_more(struct cvrf_threat_iterator *it);
void cvrf_threat_iterator_free(struct cvrf_threat_iterator *it);
void cvrf_threat_iterator_reset(struct cvrf_threat_iterator *it);
void cvrf_threat_iterator_remove(struct cvrf_threat_iterator *it);

/**
 * @struct cvrf_product_name
 * Structure holding CVRF product name data
 * ProductID and CPE data for CVRF branches and ProductTrees
 */
struct cvrf_product_name;

const char *cvrf_product_name_get_product_id(const struct cvrf_product_name *full_name);
const char *cvrf_product_name_get_cpe(const struct cvrf_product_name *full_name);

bool cvrf_product_name_set_product_id(struct cvrf_product_name *full_name, const char *product_id);
bool cvrf_product_name_set_cpe(struct cvrf_product_name *full_name, const char *cpe);

/**
 * @struct cvrf_relationship
 * Structure holding data for Relationships within ProductTree
 */
struct cvrf_relationship;

const char *cvrf_relationship_get_product_reference(const struct cvrf_relationship *relation);
const char *cvrf_relationship_get_relates_to_ref(const struct cvrf_relationship *relation);
struct cvrf_product_name *cvrf_relationship_get_product_name(struct cvrf_relationship *relation);

bool cvrf_relationship_set_product_reference(struct cvrf_relationship *relation, const char *product_reference);
bool cvrf_relationship_set_relates_to_ref(struct cvrf_relationship *relation, const char *relates_to_ref);

/**
 * @struct cvrf_branch
 * Structure holding CVRF branch data
 * Belongs to a ProductTree; may have its own sub-branches
 */
struct cvrf_branch;

const char *cvrf_branch_get_branch_name(const struct cvrf_branch *branch);
struct cvrf_product_name *cvrf_branch_get_cvrf_product_name(struct cvrf_branch *branch);
struct oscap_iterator *cvrf_branch_get_subbranches(struct cvrf_branch *branch);

bool cvrf_branch_set_branch_name(struct cvrf_branch *branch, const char *branch_name);

/**
 *@struct cvrf_product_tree
 * Structure holding CVRF ProductTree data
 * Holds at least one CVRF branch
 */
struct cvrf_product_tree;

struct oscap_iterator *cvrf_product_tree_get_branches(struct cvrf_product_tree *tree);
struct cvrf_relationship_iterator *cvrf_product_tree_get_relationships(const struct cvrf_product_tree *tree);

struct cvrf_relationship_iterator;
bool cvrf_product_tree_add_relationship(struct cvrf_product_tree *tree, struct cvrf_relationship *relation);
struct cvrf_relationship *cvrf_relationship_iterator_next(struct cvrf_relationship_iterator *it);
bool cvrf_relationship_iterator_has_more(struct cvrf_relationship_iterator *it);
void cvrf_relationship_iterator_free(struct cvrf_relationship_iterator *it);
void cvrf_relationship_iterator_reset(struct cvrf_relationship_iterator *it);
void cvrf_relationship_iterator_remove(struct cvrf_relationship_iterator *it);


/**
 * @struct cvrf_revision
 *
 */
struct cvrf_revision;

const char *cvrf_revision_get_number(const struct cvrf_revision *revision);
const char *cvrf_revision_get_date(const struct cvrf_revision *revision);
const char *cvrf_revision_get_description(const struct cvrf_revision *revision);

bool cvrf_revision_set_number(struct cvrf_revision *revision, const char *number);
bool cvrf_revision_set_date(struct cvrf_revision *revision, const char *date);
bool cvrf_revision_set_description(struct cvrf_revision *revision, const char *description);

/**
 * @struct cvrf_doc_tracking
 *
 */
struct cvrf_doc_tracking;

const char *cvrf_doc_tracking_get_tracking_id(const struct cvrf_doc_tracking *tracking);
struct oscap_string_iterator *cvrf_doc_tracking_get_aliases(struct cvrf_doc_tracking *tracking);
const char *cvrf_doc_tracking_get_tracking_version(const struct cvrf_doc_tracking *tracking);
const char *cvrf_doc_tracking_get_init_release_date(const struct cvrf_doc_tracking *tracking);
const char *cvrf_doc_tracking_get_cur_release_date(const struct cvrf_doc_tracking *tracking);
const char *cvrf_doc_tracking_get_generator_engine(const struct cvrf_doc_tracking *tracking);
const char *cvrf_doc_tracking_get_generator_date(const struct cvrf_doc_tracking *tracking);

bool cvrf_doc_tracking_set_tracking_id(struct cvrf_doc_tracking *tracking, const char *tracking_id);
bool cvrf_doc_tracking_set_tracking_version(struct cvrf_doc_tracking *tracking, const char *tracking_version);
bool cvrf_doc_tracking_set_init_release_date(struct cvrf_doc_tracking *tracking, const char *init_release_date);
bool cvrf_doc_tracking_set_cur_release_date(struct cvrf_doc_tracking *tracking, const char *cur_release_date);
bool cvrf_doc_tracking_set_generator_engine(struct cvrf_doc_tracking *tracking, const char *generator_engine);
bool cvrf_doc_tracking_set_generator_date(struct cvrf_doc_tracking *tracking, const char *generator_date);

/**
 * @struct cvrf_doc_publisher
 *
 */
struct cvrf_doc_publisher;

const char *cvrf_doc_publisher_get_vendor_id(const struct cvrf_doc_publisher *publisher);
const char *cvrf_doc_publisher_get_contact_details(const struct cvrf_doc_publisher *publisher);
const char *cvrf_doc_publisher_get_issuing_authority(const struct cvrf_doc_publisher *publisher);

bool cvrf_doc_publisher_set_vendor_id(struct cvrf_doc_publisher *publisher, const char *vendor_id);
bool cvrf_doc_publisher_set_contact_details(struct cvrf_doc_publisher *publisher, const char *contact_details);
bool cvrf_doc_publisher_set_issuing_authority(struct cvrf_doc_publisher *publisher, const char *issuing_authority);

/**
 * @struct cvrf_reference
 *
 */
struct cvrf_reference;

const char *cvrf_reference_get_ref_url(const struct cvrf_reference *reference);
const char *cvrf_reference_get_ref_description(const struct cvrf_reference *reference);

bool cvrf_reference_set_ref_url(struct cvrf_reference *reference, const char *url);
bool cvrf_reference_set_ref_description(struct cvrf_reference *reference, const char *description);

/**
 * @struct cvrf_document
 *
 */
struct cvrf_document;

const char *cvrf_document_get_doc_distribution(const struct cvrf_document *doc);
const char *cvrf_document_get_aggregate_severity(const struct cvrf_document *doc);
const char *cvrf_document_get_namespace(const struct cvrf_document *doc);
struct cvrf_doc_tracking *cvrf_document_get_tracking(struct cvrf_document *doc);
struct cvrf_doc_publisher *cvrf_document_get_publisher(struct cvrf_document *doc);
struct oscap_iterator *cvrf_document_get_references(struct cvrf_document *doc);

bool cvrf_document_set_doc_distribution(struct cvrf_document *doc, const char *distribution);
bool cvrf_document_set_aggregate_severity(struct cvrf_document *doc, const char *severity);
bool cvrf_document_set_namespace(struct cvrf_document *doc, const char *ns);
void cvrf_document_set_publisher(struct cvrf_document *doc, struct cvrf_doc_publisher *publisher);
void cvrf_document_set_tracking(struct cvrf_document *doc, struct cvrf_doc_tracking *track);

/**
 * @struct cvrf_model
 * Structure holding CVRF model
 * Top level structure; contains ProductTree and list of Vulnerabilities
 */
struct cvrf_model;

const char *cvrf_model_get_doc_title(const struct cvrf_model *model);
const char *cvrf_model_get_doc_type(const struct cvrf_model *model);
struct cvrf_product_tree *cvrf_model_get_product_tree(struct cvrf_model *model);
struct cvrf_vulnerability_iterator *cvrf_model_get_vulnerabilities(const struct cvrf_model *model);
struct cvrf_document *cvrf_model_get_document(struct cvrf_model *model);
const char *cvrf_model_get_identification(struct cvrf_model *model);

bool cvrf_model_set_doc_title(struct cvrf_model *model, const char *doc_title);
bool cvrf_model_set_doc_type(struct cvrf_model *model, const char *doc_type);

struct cvrf_vulnerability_iterator;
bool cvrf_model_add_vulnerability(struct cvrf_model *model, struct cvrf_vulnerability *vuln);
struct cvrf_vulnerability *cvrf_vulnerability_iterator_next(struct cvrf_vulnerability_iterator *it);
bool cvrf_vulnerability_iterator_has_more(struct cvrf_vulnerability_iterator *it);
void cvrf_vulnerability_iterator_free(struct cvrf_vulnerability_iterator *it);
void cvrf_vulnerability_iterator_reset(struct cvrf_vulnerability_iterator *it);
void cvrf_vulnerability_iterator_remove(struct cvrf_vulnerability_iterator *it);

/**
 * @struct cvrf_index
 * Represents an index of a CVRF feed or directory
 * Maintains a list of all CVRF files in the form of cvrf_model structures
 */
struct cvrf_index;

const char *cvrf_index_get_source_url(const struct cvrf_index *index);
const char *cvrf_index_get_index_file(const struct cvrf_index *index);
struct cvrf_model_iterator *cvrf_index_get_models(const struct cvrf_index *index);

bool cvrf_index_set_source_url(struct cvrf_index *index, const char *source_url);
bool cvrf_index_set_index_file(struct cvrf_index *index, const char *index_file);

struct cvrf_model_iterator;
bool cvrf_index_add_model(struct cvrf_index *index, struct cvrf_model *model);
struct cvrf_model *cvrf_model_iterator_next(struct cvrf_model_iterator *it);
bool cvrf_model_iterator_has_more(struct cvrf_model_iterator *it);
void cvrf_model_iterator_free(struct cvrf_model_iterator *it);
void cvrf_model_iterator_reset(struct cvrf_model_iterator *it);
void cvrf_model_iterator_remove(struct cvrf_model_iterator *it);



/**
 * @struct cvrf_model_eval
 *
 *
 */
struct cvrf_model_eval;

struct cvrf_model *cvrf_eval_get_model(struct cvrf_model_eval *eval);
struct oscap_string_iterator *cvrf_model_eval_get_product_ids(struct cvrf_model_eval *eval);
const char *cvrf_model_eval_get_os_name(const struct cvrf_model_eval *eval);
const char *cvrf_model_eval_get_os_version(const struct cvrf_model_eval *eval);


void cvrf_eval_set_model(struct cvrf_model_eval *eval, struct cvrf_model *model);
bool cvrf_model_eval_set_os_name(struct cvrf_model_eval *eval, const char *os_name);
bool cvrf_model_eval_set_os_version(struct cvrf_model_eval *eval, const char *os_version);


/**
 * @struct cvrf_rpm_attributes
 *
 */
struct cvrf_rpm_attributes;

const char *cvrf_rpm_attributes_get_full_package_name(const struct cvrf_rpm_attributes *attributes);
const char *cvrf_rpm_attributes_get_rpm_name(const struct cvrf_rpm_attributes *attributes);
const char *cvrf_rpm_attributes_get_evr_format(const struct cvrf_rpm_attributes *attributes);

bool cvrf_rpm_attributes_set_full_package_name(struct cvrf_rpm_attributes *attributes, const char *full_package);
bool cvrf_rpm_attributes_set_rpm_name(struct cvrf_rpm_attributes *attributes, const char *rpm_name);
bool cvrf_rpm_attributes_set_evr_format(struct cvrf_rpm_attributes *attributes, const char *evr_format);


/************************************************************************************/
/************************************************************************************/




/**
 * Get supported version of CVRF XML
 * @return version of XML file format
 * @memberof cvrf_model
 */
const char * cvrf_model_supported(void);


/**
 * New index structure holding all CVRF models
 * @memberof cvrf_index
 * @return New CVRF index structure
 */
struct cvrf_index *cvrf_index_new(void);

/**
 * New CVRF model
 * @memberof cvrf_model
 * @return New CVRF model
 */
struct cvrf_model *cvrf_model_new(void);

/**
 *
 * @memberof cvrf_document
 * @return New CVRF Document structure
 */
struct cvrf_document *cvrf_document_new(void);

/**
 *
 * @memberof cvrf_doc_publisher
 * @return New CVRF DocumentPublisher structure
 */
struct cvrf_doc_publisher *cvrf_doc_publisher_new(void);

/**
 *
 * @memberof cvrf_doc_tracking
 * @return New CVRF DocumentTracking
 */
struct cvrf_doc_tracking *cvrf_doc_tracking_new(void);

/**
 *
 * @memberof cvrf_revision
 * @return New CVRF Revision
 */
struct cvrf_revision *cvrf_revision_new(void);

/**
 *
 * @memberof cvrf_reference
 * @return New CVRF Reference
 */
struct cvrf_reference *cvrf_reference_new(void);

/**
 * New ProductTree structure
 * @memberof cvrf_product_tree
 * @return New CVRF ProductTree
 */
struct cvrf_product_tree *cvrf_product_tree_new(void);

/**
 * New CVRF branch of ProductTree or sub-branch
 * @memberof cvrf_branch
 * @return New CVRF branch
 */
struct cvrf_branch *cvrf_branch_new(void);

/**
 * New CVRF Relationship item within ProductTree
 * @memberof cvrf_relationship
 * @return New CVRF Relationship structure
 */
struct cvrf_relationship *cvrf_relationship_new(void);

/**
 * New FullProductName of Branch or ProductTree
 * @memberof cvrf_product_name
 * @return New FullProductName
 */
struct cvrf_product_name *cvrf_product_name_new(void);

/**
 * New CVRF Vulnerability
 * @memberof cvrf_vulnerability
 * @return New CVRF Vulnerability
 */
struct cvrf_vulnerability *cvrf_vulnerability_new(void);

/**
 * New ScoreSet element of CVSSScoreSets container
 * @memberof cvrf_score_set
 * @return New ScoreSet element
 */
struct cvrf_score_set *cvrf_score_set_new(void);

/**
 *
 *
 *
 */
struct cvrf_threat *cvrf_threat_new(void);

/**
 *
 *
 *
 */
struct cvrf_remediation *cvrf_remediation_new(void);

/**
 * New ProductStatus member of a CVRF Vulnerability
 * @memberof cvrf_product_status
 * @return New CVRF ProductStatus
 */
struct cvrf_product_status *cvrf_product_status_new(void);

/**
 *
 *
 */
struct cvrf_model_eval *cvrf_model_eval_new(void);

/**
 *
 *
 */
struct cvrf_rpm_attributes *cvrf_rpm_attributes_new(void);


/**
 *
 *
 */
void cvrf_index_free(struct cvrf_index *index);

/**
 *
 *
 */
void cvrf_model_free(struct cvrf_model *cvrf);

/**
 *
 *
 */
void cvrf_document_free(struct cvrf_document *doc);

/**
 *
 *
 */
void cvrf_doc_publisher_free(struct cvrf_doc_publisher *publisher);

/**
 *
 *
 */
void cvrf_doc_tracking_free(struct cvrf_doc_tracking *tracking);

/**
 *
 *
 */
void cvrf_revision_free(struct cvrf_revision *revision);

/**
 *
 *
 */
void cvrf_reference_free(struct cvrf_reference *reference);

/**
 *
 *
 */
void cvrf_product_tree_free(struct cvrf_product_tree *tree);

/**
 *
 *
 */
void cvrf_branch_free(struct cvrf_branch *branch);

/**
 *
 *
 */
void cvrf_relationship_free(struct cvrf_relationship *relationship);

/**
 *
 *
 */
void cvrf_product_name_free(struct cvrf_product_name *full_name);

/**
 *
 *
 */
void cvrf_vulnerability_free(struct cvrf_vulnerability *vulnerability);

/**
 *
 *
 */
void cvrf_score_set_free(struct cvrf_score_set *score_set);

/**
 *
 *
 */
void cvrf_threat_free(struct cvrf_threat *threat);

/**
 *
 *
 */
void cvrf_remediation_free(struct cvrf_remediation *remed);

/**
 *
 *
 */
void cvrf_product_status_free(struct cvrf_product_status *status);

/**
 *
 *
 */
void cvrf_model_eval_free(struct cvrf_model_eval *eval);

/**
 *
 *
 */
void cvrf_rpm_attributes_free(struct cvrf_rpm_attributes *attributes);


/************************************************************************************/
/************************************************************************************/


/**
 * Parses specified text index file and parses each filename in the list
 * into a CVRF model contained in the CVRF index structure
 * @memberof cvrf_index
 * @param index_source OSCAP source with path to CVRF file
 * @return New CVRF index containing all CVRF models
 */
struct cvrf_index *cvrf_index_import(struct oscap_source *index_source);

/**
 * Parses the specified XML file and creates a list of CVRF data structures.
 * @memberof cvrf_model
 * @param source OSCAP source with path to CVRF file
 * @return New CVRF model structure
 */
struct cvrf_model *cvrf_model_import(struct oscap_source *source);

/**
 * Export all CVRF models in CVRF index to XML file
 * @memberof cvrf_index
 * @param index CVRF index structure
 * @param export_file OSCAP export target
 * @return exit code of export
 */
int cvrf_index_export(struct cvrf_index *index, const char *export_file);

/**
 * Export CVRF model to XML file
 * @memberof cvrf_model
 * @param cvrf CVRF model
 * @param export_file OSCAP export target
 * @return exit code of export
 */
int cvrf_model_export(struct cvrf_model *cvrf, const char *export_file);


const char * cvrf_model_supported(void);




void cvrf_export_results(struct oscap_source *import_source, const char *export_file, const char *os_name);

bool cvrf_product_vulnerability_fixed(struct cvrf_vulnerability *vuln, char *product);

int cvrf_model_eval_construct_definition_model(struct cvrf_model_eval *eval);


/**@}*/

#endif				/* _CVRF_H_ */
