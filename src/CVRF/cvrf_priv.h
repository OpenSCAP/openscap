/**
 * @file cve_priv.h
 */

#ifndef CVRF_PRIV_H_
#define CVRF_PRIV_H_

#include <libxml/xmlreader.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include <libxml/xpathInternals.h>

#include "../common/list.h"
#include "../common/elements.h"
#include "cvrf.h"

/***************************************************************************************************
 * CVRF enum definitions
 */

typedef enum {
	CVRF_DOC_PUBLISHER_VENDOR,
	CVRF_DOC_PUBLISHER_DISCOVERER,
	CVRF_DOC_PUBLISHER_COORDINATOR,
	CVRF_DOC_PUBLISHER_USER,
	CVRF_DOC_PUBLISHER_OTHER,
} cvrf_doc_publisher_type_t;

cvrf_doc_publisher_type_t cvrf_doc_publisher_type_parse(xmlTextReaderPtr reader, char *attname);
const char *cvrf_doc_publisher_type_get_text(cvrf_doc_publisher_type_t doc_publisher_type);


typedef enum {
	CVRF_DOC_STATUS_DRAFT,
	CVRF_DOC_STATUS_INTERIM,
	CVRF_DOC_STATUS_FINAL,
} cvrf_doc_status_type_t;

cvrf_doc_status_type_t cvrf_doc_status_type_parse(xmlTextReaderPtr reader);
const char *cvrf_doc_status_type_get_text(cvrf_doc_status_type_t doc_status_type);


typedef enum {
	CVRF_BRANCH_VENDOR,
	CVRF_BRANCH_PRODUCT_FAMILY,
	CVRF_BRANCH_PRODUCT_NAME,
	CVRF_BRANCH_PRODUCT_VERSION,
	CVRF_BRANCH_PATCH_LEVEL,
	CVRF_BRANCH_SERVICE_PACK,
	CVRF_BRANCH_ARCHITECTURE,
	CVRF_BRANCH_LANGUAGE,
	CVRF_BRANCH_LEGACY,
	CVRF_BRANCH_SPECIFICATION,
} cvrf_branch_type_t;

cvrf_branch_type_t cvrf_branch_type_parse(xmlTextReaderPtr reader, char *attname);
const char *cvrf_branch_type_get_text(cvrf_branch_type_t branch_type);


typedef enum {
	CVRF_RELATIONSHIP_DEFAULT_COMPONENT,
	CVRF_RELATIONSHIP_OPTIONAL_COMPONENT,
	CVRF_RELATIONSHIP_EXTERNAL_COMPONENT,
	CVRF_RELATIONSHIP_INSTALLED_ON,
	CVRF_RELATIONSHIP_INSTALLED_WITH,
} cvrf_relationship_type_t;

cvrf_relationship_type_t cvrf_relationship_type_parse(xmlTextReaderPtr reader, char *attname);
const char *cvrf_relationship_type_get_text(cvrf_relationship_type_t relationship_type);


typedef enum {
	CVRF_PRODUCT_STATUS_FIRST_AFFECTED,
	CVRF_PRODUCT_STATUS_KNOWN_AFFECTED,
	CVRF_PRODUCT_STATUS_KNOWN_NOT_AFFECTED,
	CVRF_PRODUCT_STATUS_FIRST_FIXED,
	CVRF_PRODUCT_STATUS_FIXED,
	CVRF_PRODUCT_STATUS_RECOMMENDED,
	CVRF_PRODUCT_STATUS_LAST_AFFECTED,
} cvrf_product_status_type_t;

cvrf_product_status_type_t cvrf_product_status_type_parse(xmlTextReaderPtr reader, char *attname);
const char *cvrf_product_status_type_get_text(cvrf_product_status_type_t product_status_type);


typedef enum {
	CVRF_THREAT_IMPACT,
	CVRF_THREAT_EXPLOIT_STATUS,
	CVRF_THREAT_TARGET_SET,
} cvrf_threat_type_t;

cvrf_threat_type_t cvrf_threat_type_parse(xmlTextReaderPtr reader, char *attname);
const char *cvrf_threat_type_get_text(cvrf_threat_type_t threat_type);


typedef enum {
	CVRF_REMEDIATION_WORKAROUND,
	CVRF_REMEDIATION_MITIGATION,
	CVRF_REMEDIATION_VENDOR_FIX,
	CVRF_REMEDIATION_NONE_AVAILABLE,
	CVRF_REMEDIATION_WILL_NOT_FIX,
} cvrf_remediation_type_t;

cvrf_remediation_type_t cvrf_remediation_type_parse(xmlTextReaderPtr reader, char *attname);
const char *cvrf_remediation_type_get_text(cvrf_remediation_type_t remediation_type);


/***************************************************************************************************
 * CVRF parsing functions
 */

/**
 * Parse all CVRF models from all files listed in an index file
 * @param index_source OSCAP source of index file containing list of all CVRF files
 * @return New CVRF index structure containing all CVRF models
 */
struct cvrf_index *cvrf_index_parse_xml(struct oscap_source *index_source);

/**
 * Parse CVRF model
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF model
 */
struct cvrf_model *cvrf_model_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF DocumentPublisher
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF DocumentPublisher
 */
struct cvrf_doc_publisher *cvrf_doc_publisher_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF DocumentTracking
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF DocumentTracking
 */
struct cvrf_doc_tracking *cvrf_doc_tracking_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF product tree
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF product tree
 */
struct cvrf_product_tree *cvrf_product_tree_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF branch
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF branch
 */
struct cvrf_branch *cvrf_branch_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF Relationship item
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Relationship
 */
struct cvrf_relationship *cvrf_relationship_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF vulnerability
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF vulnerability
 */
struct cvrf_vulnerability *cvrf_vulnerability_parse(xmlTextReaderPtr reader);

/**
 * Parse a CVRF Threat item
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Threat
 */
struct cvrf_threat *cvrf_threat_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF Remediation
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Remediation
 */
struct cvrf_remediation *cvrf_remediation_parse(xmlTextReaderPtr reader);

/**
 * Parse Product Status within CVRF vulnerability
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Product Status
 */
struct cvrf_product_status *cvrf_product_status_parse(xmlTextReaderPtr reader);


void cvrf_stringlist_to_dom(struct oscap_stringlist *list, const char *tag_name, xmlNode *parent);

void cvrf_element_to_dom(const char *elm_name, const char *elm_value, xmlNode *parent);

xmlNode *cvrf_index_to_dom(struct cvrf_index *index, xmlDocPtr doc, xmlNode *parent, void *user_args);

xmlNode *cvrf_model_to_dom(struct cvrf_model *model, xmlDocPtr doc, xmlNode *parent, void *user_args);

void cvrf_doc_publisher_to_dom(struct cvrf_doc_publisher *publisher, xmlNode *pub_node);

void cvrf_doc_tracking_to_dom(struct cvrf_doc_tracking *tracking, xmlNode *tracking_node);

void cvrf_product_name_to_dom(struct cvrf_product_name *full_name, xmlNode *parent);

void cvrf_product_tree_to_dom(struct cvrf_product_tree *tree, xmlNode *tree_node);

void cvrf_branch_to_dom(struct cvrf_branch *branch, xmlNode *branch_node);

void cvrf_relationship_to_dom(const struct cvrf_relationship *relation, xmlNode *relation_node);

void cvrf_vulnerability_to_dom(const struct cvrf_vulnerability *vuln, xmlNode *vuln_node);

void cvrf_threat_to_dom(const struct cvrf_threat *threat, xmlNode *threat_node);

void cvrf_remediation_to_dom(const struct cvrf_remediation *remed, xmlNode *remed_node);

void cvrf_product_status_to_dom(const struct cvrf_product_status *stat, xmlNode *status_node);


#endif				/* _CVRF_PRIV_H_ */
