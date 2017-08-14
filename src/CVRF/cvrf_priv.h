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

OSCAP_HIDDEN_START;

/***************************************************************************************************
 * CVRF enum definitions
 */

#define CVRF_ATTRIBUTE_UNKNOWN (-1)

/**
 * @struct cvrf_item_spec
 * Maps a cvrf_item_type_t enum to its text representation within a CVRF XML file,
 * as well as the parent container (if one exists)
 */
struct cvrf_item_spec;

typedef enum {
	CVRF_DOCUMENT_PUBLISHER,
	CVRF_DOCUMENT_TRACKING,
	CVRF_REVISION,
	CVRF_DOCUMENT_NOTE,
	CVRF_DOCUMENT_REFERENCE,
	CVRF_ACKNOWLEDGMENT,
	CVRF_PRODUCT_TREE,
	CVRF_BRANCH,
	CVRF_GROUP,
	CVRF_RELATIONSHIP,
	CVRF_PRODUCT_NAME,
	CVRF_VULNERABILITY,
	CVRF_NOTE,
	CVRF_INVOLVEMENT,
	CVRF_SCORE_SET,
	CVRF_PRODUCT_STATUS,
	CVRF_THREAT,
	CVRF_REMEDIATION,
	CVRF_REFERENCE,
	CVRF_ITEM_UNKNOWN
} cvrf_item_type_t;

const char *cvrf_item_type_get_text(cvrf_item_type_t item_type);
cvrf_item_type_t cvrf_item_type_from_text(const char *item_name);
bool cvrf_is_valid_item_type(const char *item_name);
const char *cvrf_item_type_get_container(cvrf_item_type_t item_type);
bool cvrf_item_type_has_container(cvrf_item_type_t item_type);
int cvrf_item_parse_type_attribute(xmlTextReaderPtr reader, cvrf_item_type_t item);

typedef enum {
	CVRF_DOC_PUBLISHER_VENDOR,
	CVRF_DOC_PUBLISHER_DISCOVERER,
	CVRF_DOC_PUBLISHER_COORDINATOR,
	CVRF_DOC_PUBLISHER_USER,
	CVRF_DOC_PUBLISHER_OTHER,
} cvrf_doc_publisher_type_t;

cvrf_doc_publisher_type_t cvrf_doc_publisher_get_type(struct cvrf_doc_publisher *publisher);
cvrf_doc_publisher_type_t cvrf_doc_publisher_type_parse(xmlTextReaderPtr reader, char *attr_name);
const char *cvrf_doc_publisher_type_get_text(cvrf_doc_publisher_type_t doc_publisher_type);

typedef enum {
	CVRF_DOC_STATUS_DRAFT,
	CVRF_DOC_STATUS_INTERIM,
	CVRF_DOC_STATUS_FINAL,
} cvrf_doc_status_type_t;

cvrf_doc_status_type_t cvrf_doc_tracking_get_status(struct cvrf_doc_tracking *tracking);
const char *cvrf_doc_status_type_get_text(cvrf_doc_status_type_t doc_status_type);

typedef enum {
	CVRF_NOTE_GENERAL,
	CVRF_NOTE_DETAILS,
	CVRF_NOTE_DESCRIPTION,
	CVRF_NOTE_SUMMARY,
	CVRF_NOTE_FAQ,
	CVRF_NOTE_LEGAL_DISCLAIMER,
	CVRF_NOTE_OTHER,
} cvrf_note_type_t;

cvrf_note_type_t cvrf_note_get_note_type(const struct cvrf_note *note);
const char *cvrf_note_type_get_text(cvrf_note_type_t note_type);

typedef enum {
	CVRF_REFERENCE_EXTERNAL,
	CVRF_REFERENCE_SELF
} cvrf_reference_type_t;

cvrf_reference_type_t cvrf_reference_get_reference_type(struct cvrf_reference *reference);
const char *cvrf_reference_type_get_text(cvrf_reference_type_t reference_type);

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

cvrf_branch_type_t cvrf_branch_get_branch_type(struct cvrf_branch *branch);
const char *cvrf_branch_type_get_text(cvrf_branch_type_t branch_type);


typedef enum {
	CVRF_RELATIONSHIP_DEFAULT_COMPONENT,
	CVRF_RELATIONSHIP_OPTIONAL_COMPONENT,
	CVRF_RELATIONSHIP_EXTERNAL_COMPONENT,
	CVRF_RELATIONSHIP_INSTALLED_ON,
	CVRF_RELATIONSHIP_INSTALLED_WITH,
} cvrf_relationship_type_t;

cvrf_relationship_type_t cvrf_relationship_get_relation_type(struct cvrf_relationship *relation);
const char *cvrf_relationship_type_get_text(cvrf_relationship_type_t relationship_type);

typedef enum {
	CVRF_INVOLVEMENT_OPEN,
	CVRF_INVOLVEMENT_DISPUTED,
	CVRF_INVOLVEMENT_IN_PROGRESS,
	CVRF_INVOLVEMENT_COMPLETED,
	CVRF_INVOLVEMENT_CONTACT_ATTEMPTED,
	CVRF_INVOLVEMENT_NOT_CONTACTED,
} cvrf_involvement_status_type_t;

const char *cvrf_involvement_status_type_get_text(cvrf_involvement_status_type_t involvement_type);
cvrf_involvement_status_type_t cvrf_involvement_get_status_type(struct cvrf_involvement *involve);

typedef enum {
	CVRF_PRODUCT_STATUS_FIRST_AFFECTED,
	CVRF_PRODUCT_STATUS_KNOWN_AFFECTED,
	CVRF_PRODUCT_STATUS_KNOWN_NOT_AFFECTED,
	CVRF_PRODUCT_STATUS_FIRST_FIXED,
	CVRF_PRODUCT_STATUS_FIXED,
	CVRF_PRODUCT_STATUS_RECOMMENDED,
	CVRF_PRODUCT_STATUS_LAST_AFFECTED,
} cvrf_product_status_type_t;

cvrf_product_status_type_t cvrf_product_status_get_type(struct cvrf_product_status *stat);
const char *cvrf_product_status_type_get_text(cvrf_product_status_type_t product_status_type);


typedef enum {
	CVRF_THREAT_IMPACT,
	CVRF_THREAT_EXPLOIT_STATUS,
	CVRF_THREAT_TARGET_SET,
} cvrf_threat_type_t;

cvrf_threat_type_t cvrf_threat_get_threat_type(struct cvrf_threat *threat);
const char *cvrf_threat_type_get_text(cvrf_threat_type_t threat_type);


typedef enum {
	CVRF_REMEDIATION_WORKAROUND,
	CVRF_REMEDIATION_MITIGATION,
	CVRF_REMEDIATION_VENDOR_FIX,
	CVRF_REMEDIATION_NONE_AVAILABLE,
	CVRF_REMEDIATION_WILL_NOT_FIX,
} cvrf_remediation_type_t;

cvrf_remediation_type_t cvrf_remediation_get_type(struct cvrf_remediation *remed);
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
 * Parse structures in CVRF file that pertain to the document itself,
 * such as DocumentTracking, DocumentPublisher, and DocumentDistribution
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF document structure
 */
struct cvrf_document *cvrf_document_parse(xmlTextReaderPtr reader);

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
 * Parse CVRF Revision element
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Revision
 */
struct cvrf_revision *cvrf_revision_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF Note element
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Note
 */
struct cvrf_note *cvrf_note_parse(xmlTextReaderPtr reader);

/**
 * Parse a CVRF Reference element
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Reference
 */
struct cvrf_reference *cvrf_reference_parse(xmlTextReaderPtr reader);

/**
 * Parse a CVRF Acknowledgment element
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Acknowledgment
 */
struct cvrf_acknowledgment *cvrf_acknowledgment_parse(xmlTextReaderPtr reader);

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
 * Parse CVRF Group item
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Group
 */
struct cvrf_group *cvrf_group_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF FullProductName item
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Relationship
 */
struct cvrf_product_name *cvrf_product_name_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF vulnerability
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF vulnerability
 */
struct cvrf_vulnerability *cvrf_vulnerability_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF Involvement element
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Involvement
 */
struct cvrf_involvement *cvrf_involvement_parse(xmlTextReaderPtr reader);

/**
 * Parse a ScoreSet element of CVSSScoreSets container
 * @param reader XML Text Reader representing XML model
 * @return parsed ScoreSet element
 */
struct cvrf_score_set *cvrf_score_set_parse(xmlTextReaderPtr reader);

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


void cvrf_element_add_container(struct oscap_list *list, cvrf_item_type_t cvrf_type, xmlNode *parent);

void cvrf_element_add_stringlist(struct oscap_stringlist *list, const char *tag_name, xmlNode *parent);

void cvrf_element_add_attribute(const char *attr_name, const char *attr_value, xmlNode *element);

void cvrf_element_add_child(const char *elm_name, const char *elm_value, xmlNode *parent);

xmlNode *cvrf_element_to_dom(const char *elm_name, const char *elm_value);

struct oscap_source *cvrf_index_get_export_source(struct cvrf_index *index);

xmlNode *cvrf_index_to_dom(struct cvrf_index *index, xmlDocPtr doc, xmlNode *parent, void *user_args);

struct oscap_source *cvrf_model_get_export_source(struct cvrf_model *model);

xmlNode *cvrf_model_to_dom(struct cvrf_model *model, xmlDocPtr doc, xmlNode *parent, void *user_args);

xmlNode *cvrf_doc_publisher_to_dom(struct cvrf_doc_publisher *publisher);

xmlNode *cvrf_doc_tracking_to_dom(struct cvrf_doc_tracking *tracking);

xmlNode *cvrf_note_to_dom(struct cvrf_note *note);

xmlNode *cvrf_revision_to_dom(struct cvrf_revision *revision);

xmlNode *cvrf_reference_to_dom(struct cvrf_reference *ref);

xmlNode *cvrf_acknowledgment_to_dom(struct cvrf_acknowledgment *ack);

xmlNode *cvrf_product_name_to_dom(struct cvrf_product_name *full_name);

xmlNode *cvrf_product_tree_to_dom(struct cvrf_product_tree *tree);

xmlNode *cvrf_branch_to_dom(struct cvrf_branch *branch);

xmlNode *cvrf_relationship_to_dom(const struct cvrf_relationship *relation);

xmlNode *cvrf_group_to_dom(const struct cvrf_group *group);

xmlNode *cvrf_vulnerability_to_dom(const struct cvrf_vulnerability *vuln);

xmlNode *cvrf_involvement_to_dom(const struct cvrf_involvement *involve);

xmlNode *cvrf_score_set_to_dom(const struct cvrf_score_set *score_set);

xmlNode *cvrf_threat_to_dom(const struct cvrf_threat *threat);

xmlNode *cvrf_remediation_to_dom(const struct cvrf_remediation *remed);

xmlNode *cvrf_product_status_to_dom(const struct cvrf_product_status *stat);

OSCAP_HIDDEN_END;
#endif				/* _CVRF_PRIV_H_ */
