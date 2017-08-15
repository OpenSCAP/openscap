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
 * Authors:
 * 		Katarina Jankov <kj226@cornell.edu>
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

/*-----------------------------------------------------------------------------------------*\
|									CVRF Enum Definitions									|
\*-----------------------------------------------------------------------------------------*/

/************************************************************************************************
 * Type attribute of a DocumentPublisher element and Party attribute of an Involvement element
 * (these must match in the same document)
 */
typedef enum {
	CVRF_DOC_PUBLISHER_UNKNOWN = 0, ///< Missing or invalid DocumentPublisher type
	CVRF_DOC_PUBLISHER_VENDOR,		///< Developers of information system products and services
	CVRF_DOC_PUBLISHER_DISCOVERER,  ///< Individuals or organizations that find vulnerabilities
	CVRF_DOC_PUBLISHER_COORDINATOR, ///< Entities that manage response to Vendors' vulnerabilities
	CVRF_DOC_PUBLISHER_USER,		///< Users of the Vendors' products
	CVRF_DOC_PUBLISHER_OTHER,		///< Includes translators, forwarders, misc. contributors, etc.
} cvrf_doc_publisher_type_t;

/**
 * @memberof cvrf_doc_publisher
 * Retrieve type of publisher for the CVRF document
 * @param publisher CVRF DocumentPublisher structure
 * @return Enum representing Type attribute of the DocumentPublisher element
 */
cvrf_doc_publisher_type_t cvrf_doc_publisher_get_type(struct cvrf_doc_publisher *publisher);

/**
 * @memberof cvrf_involvement
 * Retrieve type of publisher for the CVRF document
 * @param involve CVRF Involvement structure
 * @return Enum representing Party attribute of the Involvement element
 */
cvrf_doc_publisher_type_t cvrf_involvement_get_party(struct cvrf_involvement *involve);

/**
 * Parses text value of the attribute and finds the appropriate DocumentPublisher type
 * @param reader XML text reader
 * @return Enum representation of the Type attribute of the DocumentPublisher element
 */
cvrf_doc_publisher_type_t cvrf_doc_publisher_type_parse(xmlTextReaderPtr reader);

/**
 * Parses text value of the attribute and finds the appropriate DocumentPublisher type
 * Involvement 'Party' attribute uses the same enumerated list as DocumentPublisher type
 * @param reader XML text reader
 * @return Enum representation of the Party attribute of the Involvement element
 */
cvrf_doc_publisher_type_t cvrf_involvement_party_parse(xmlTextReaderPtr reader);

/**
 * Find the string value of the DocumentPublisher type mapped to the provided enum value
 * @param type Enum representation of DocumentPublisher's Type attribute
 * @return String representation of DocumentPublisher's Type attribute
 */
const char *cvrf_doc_publisher_type_get_text(cvrf_doc_publisher_type_t type);

/************************************************************************************************
 * Type represented as a child node of the DocumentTracking element
 * Refers to stage of completeness of the document and likelihood of it changing
 */
typedef enum {
	CVRF_DOC_STATUS_UNKNOWN = 0, 	///< Missing or invalid Status type
	CVRF_DOC_STATUS_DRAFT,			///< Pre-release or for internal use only
	CVRF_DOC_STATUS_INTERIM,		///< Issuer considers document subject to change
	CVRF_DOC_STATUS_FINAL,			///< Issuer considers document unlikely to change
} cvrf_doc_status_type_t;

/**
 * @memberof cvrf_doc_tracking
 * Retrieve completeness status of the CVRF document
 * @param tracking CVRF DocumentTracking structure
 * @return Enum representing Status child element of the DocumentTracking element
 */
cvrf_doc_status_type_t cvrf_doc_tracking_get_status(struct cvrf_doc_tracking *tracking);

/**
 * Parses text value of the Status element and finds the appropriate Status type
 * @param reader XML text reader
 * @return Enum representation of DocumentTracking's Status element
 */
cvrf_doc_status_type_t cvrf_doc_status_type_parse(xmlTextReaderPtr reader);

/**
 * Find the string value of the DocumentTracking Status type mapped to the provided enum value
 * @param type Enum representation of DocumentTracking's Status element
 * @return String representation ofDocumentTracking's Status element
 */
const char *cvrf_doc_status_type_get_text(cvrf_doc_status_type_t type);

/************************************************************************************************
 * Type attribute of the Note element
 * Category of information provided by a Note element
 */
typedef enum {
	CVRF_NOTE_UNKNOWN = 0,		///< Missing or invalid Note type
	CVRF_NOTE_GENERAL,			///< Note is a high-level, general discussion of something
	CVRF_NOTE_DETAILS,			///< Note is a low-level detailed discussion of something
	CVRF_NOTE_DESCRIPTION,		///< Note is a description of something
	CVRF_NOTE_SUMMARY,			///< Note is a summary of something
	CVRF_NOTE_FAQ,				///< Note is a list of frequently asked questions
	CVRF_NOTE_LEGAL_DISCLAIMER, ///< Note gives legal constraints and info about the document
	CVRF_NOTE_OTHER,			///< Note falls into a misc. category
} cvrf_note_type_t;

/**
 * @memberof cvrf_note
 * Category of information provided by a Note element
 * @param note CVRF Note structure
 * @return Enum representing Type attribute of the Note element
 */
cvrf_note_type_t cvrf_note_get_note_type(const struct cvrf_note *note);

/**
 * Parses text value of the attribute and finds the appropriate Note type
 * @param reader XML text reader
 * @return Enum representation of the Type attribute of the Note element
 */
cvrf_note_type_t cvrf_note_type_parse(xmlTextReaderPtr reader);

/**
 * Find the string value of the Note type mapped to the provided enum value
 * @param type Enum representation of the Note's Type attribute
 * @return String representation of the Note's Type attribute
 */
const char *cvrf_note_type_get_text(cvrf_note_type_t type);

/************************************************************************************************
 * Type attribute of the Reference element
 * Indicates whether the reference refers to a document or to an external source
 */
typedef enum {
	CVRF_REFERENCE_UNKNOWN = 0,		///< Missing or invalid Reference type
	CVRF_REFERENCE_EXTERNAL,		///< Reference is external to the document
	CVRF_REFERENCE_SELF,			///< Related document is a direct reference to itself
} cvrf_reference_type_t;

/**
 * @memberof cvrf_reference
 * Indicates whether reference refers to the document itself or to an external source
 * @param reference CVRF Reference structure
 * @return Enum representing Type attribute of the Reference element
 */
cvrf_reference_type_t cvrf_reference_get_reference_type(struct cvrf_reference *reference);

/**
 * Parses text value of the attribute and finds the appropriate Reference type
 * @param reader XML text reader
 * @return Enum representation of the Type attribute of the Reference element
 */
cvrf_reference_type_t cvrf_reference_type_parse(xmlTextReaderPtr reader);

/**
 * Find the string value of the Reference type mapped to the provided enum value
 * @param type Enum representation of the Reference's Type attribute
 * @return String representation of the Reference's Type attribute
 */
const char *cvrf_reference_type_get_text(cvrf_reference_type_t type);

/************************************************************************************************
 * Type attribute of the Branch element
 * Category and context for the information provided in the Name attribute
 */
typedef enum {
	CVRF_BRANCH_UNKNOWN = 0,		///< Missing or invalid Branch type
	CVRF_BRANCH_VENDOR,				///< Vendor who makes the product
	CVRF_BRANCH_PRODUCT_FAMILY,		///< Family that child products belong to
	CVRF_BRANCH_PRODUCT_NAME,		///< Name of the product
	CVRF_BRANCH_PRODUCT_VERSION,	///< Version of the product
	CVRF_BRANCH_PATCH_LEVEL,		///< Patch level of the product
	CVRF_BRANCH_SERVICE_PACK,		///< Service pack of the product
	CVRF_BRANCH_ARCHITECTURE,		///< Architecture for the product
	CVRF_BRANCH_LANGUAGE,			///< Language of the product
	CVRF_BRANCH_LEGACY,				///< Nonspecific legacy entry
	CVRF_BRANCH_SPECIFICATION,		///< Specification for the product
} cvrf_branch_type_t;

/**
 * @memberof cvrf_branch
 * Category of information given in the Name attribute of the Branch
 * @param branch CVRF Branch structure
 * @return Enum representing Type attribute of the Branch element
 */
cvrf_branch_type_t cvrf_branch_get_branch_type(struct cvrf_branch *branch);

/**
 * Parses text value of the attribute and finds the appropriate Branch type
 * @param reader XML text reader
 * @return Enum representation of the Type attribute of the Branch element
 */
cvrf_branch_type_t cvrf_branch_type_parse(xmlTextReaderPtr reader);

/**
 * Find the string value of the Branch type mapped to the provided enum value
 * @param type Enum representation of the Branch's Type attribute
 * @return String representation of the Branch's Type attribute
 */
const char *cvrf_branch_type_get_text(cvrf_branch_type_t type);

/************************************************************************************************
 * RelationType attribute of the Relationship element
 * Defines how the products named in the ProductReference and RelatesToProductReference attributes
 * are related
 * EX:
 * <Relationship ProductReference="A" RelationType="?" RelatesToProductReference="B">
 */
typedef enum {
	CVRF_RELATIONSHIP_UNKNOWN = 0,			///< Missing or invalid RelationType
	CVRF_RELATIONSHIP_DEFAULT_COMPONENT,	///< A is a default component of B
	CVRF_RELATIONSHIP_OPTIONAL_COMPONENT,	///< A is an optional component of B
	CVRF_RELATIONSHIP_EXTERNAL_COMPONENT,	///< A is an external component of B
	CVRF_RELATIONSHIP_INSTALLED_ON,			///< A is installed on B
	CVRF_RELATIONSHIP_INSTALLED_WITH,		///< A is installed with B
} cvrf_relationship_type_t;

/**
 * @memberof cvrf_relationship
 * Defines how items in ProductReference and RelatesToProductReference are related
 * @param relation CVRF Relationship structure
 * @return Enum representing RelationType attribute of the Relationship element
 */
cvrf_relationship_type_t cvrf_relationship_get_relation_type(struct cvrf_relationship *relation);

/**
 * Parses text value of the attribute and finds the appropriate RelationType
 * @param reader XML text reader
 * @return Enum representation of the RelationType attribute of the Relationship element
 */
cvrf_relationship_type_t cvrf_relationship_type_parse(xmlTextReaderPtr reader);

/**
 * Find the string value of the RelationType mapped to the provided enum value
 * @param type Enum representation of the Relationship's RelationType attribute
 * @return String representation of the Relationship's RelationType attribute
 */
const char *cvrf_relationship_type_get_text(cvrf_relationship_type_t type);

/************************************************************************************************
 * Status attribute of the Involvement element
 * Indicates level of involvement of the Party referenced in the Involvement's Party attribute
 */
typedef enum {
	CVRF_INVOLVEMENT_UNKNOWN = 0,		///< Missing or invalid Status attribute
	CVRF_INVOLVEMENT_OPEN,				///< Vendor acknowledges awareness of Vulnerability
	CVRF_INVOLVEMENT_DISPUTED,			///< Vendor disputes entire Vulnerability report
	CVRF_INVOLVEMENT_IN_PROGRESS,		///< Some remediations from Vendor may be available
	CVRF_INVOLVEMENT_COMPLETED,			///< Vendor believes investigation of Vulnerability is done
	CVRF_INVOLVEMENT_CONTACT_ATTEMPTED, ///< Document producer attempted to contact Vendor
	CVRF_INVOLVEMENT_NOT_CONTACTED,		///< Document producer has not attempted to contact Vendor
} cvrf_involvement_status_type_t;

/**
 * @memberof cvrf_involvement
 * Level of involvement of the Party
 * @param involve CVRF Involvement structure
 * @return Enum representing Status attribute of the Involvement element
 */
cvrf_involvement_status_type_t cvrf_involvement_get_status_type(struct cvrf_involvement *involve);

/**
 * Parses text value of the attribute and finds the appropriate Involvement Status type
 * @param reader XML text reader
 * @return Enum representation of the Status attribute of the Involvement element
 */
cvrf_involvement_status_type_t cvrf_involvement_status_type_parse(xmlTextReaderPtr reader);

/**
 * Find the string value of the Involvement Status type mapped to the provided enum value
 * @param type Enum representation of the Involvement's Status attribute
 * @return String representation of the Involvement's Status attribute
 */
const char *cvrf_involvement_status_type_get_text(cvrf_involvement_status_type_t type);


/************************************************************************************************
 * Type attribute of the Status element
 * Indicates the status of products with regards to a Vulnerability: whether this Vulnerability
 * is known to affect the product, whether a fix exists for the version in the release, etc.
 */
typedef enum {
	CVRF_PRODUCT_STATUS_UNKNOWN = 0,		///< Missing or invalid Status type
	CVRF_PRODUCT_STATUS_FIRST_AFFECTED,		///< First version in release is affected by Vulnerability
	CVRF_PRODUCT_STATUS_KNOWN_AFFECTED,		///< Version is known to be affected by the Vulnerability
	CVRF_PRODUCT_STATUS_KNOWN_NOT_AFFECTED, ///< Version is not known to be affected by the Vulnerability
	CVRF_PRODUCT_STATUS_FIRST_FIXED,		///< Version contains the first fix for the Vulnerability
	CVRF_PRODUCT_STATUS_FIXED,				///< Version contains a fix for the Vulnerability
	CVRF_PRODUCT_STATUS_RECOMMENDED,		///< Version contains vendor-recommended fix for the Vulnerability
	CVRF_PRODUCT_STATUS_LAST_AFFECTED,		///< Last version in the release affected by the Vulnerability
} cvrf_product_status_type_t;


/**
 * @memberof cvrf_product_status
 * Indicates the status of products with regards to a Vulnerability
 * @param stat CVRF Status structure
 * @return Enum representing Type attribute of the Status element
 */
cvrf_product_status_type_t cvrf_product_status_get_type(struct cvrf_product_status *stat);

/**
 * Parses text value of the attribute and finds the appropriate Status type
 * @param reader XML text reader
 * @return Enum representation of the Type attribute of the Status element
 */
cvrf_product_status_type_t cvrf_product_status_type_parse(xmlTextReaderPtr reader);

/**
 * Find the string value of the ProductStatus type mapped to the provided enum value
 * @param type Enum representation of the Status' Type attribute
 * @return String representation of the Status' Type attribute
 */
const char *cvrf_product_status_type_get_text(cvrf_product_status_type_t product_status_type);

/************************************************************************************************
 * Type attribute of the Threat element
 * Category of information the Threat provides about the Vulnerability; gives context about
 * impact and damage done by the Vulnerability
 */
typedef enum {
	CVRF_THREAT_UNKNOWN = 0,	///< Missing or invalid Threat type
	CVRF_THREAT_IMPACT,			///< Estimation of impact on user if the Vulnerability were exploited
	CVRF_THREAT_EXPLOIT_STATUS, ///< Degree to which exploit for the Vulnerability is known
	CVRF_THREAT_TARGET_SET,		///< Description of known victim population for the Vulnerability
} cvrf_threat_type_t;

/**
 * @memberof cvrf_threat
 * Category of information provided by the Threat about the Vulnerability
 * @param threat CVRF Threat structure
 * @return Enum representing Type attribute of the Threat element
 */
cvrf_threat_type_t cvrf_threat_get_threat_type(struct cvrf_threat *threat);

/**
 * Parses text value of the attribute and finds the appropriate Threat type
 * @param reader XML text reader
 * @return Enum representation of the Type attribute of the Threat element
 */
cvrf_threat_type_t cvrf_threat_type_parse(xmlTextReaderPtr reader);

/**
 * Find the string value of the Threat type mapped to the provided enum value
 * @param type Enum representation of the Threat's Type attribute
 * @return String representation of the Threat's Type attribute
 */
const char *cvrf_threat_type_get_text(cvrf_threat_type_t threat_type);

/************************************************************************************************
 * Type attribute of the Remediation element
 * Category of and status about ways to avoid, mitigate, or resolve a Vulnerability
 */
typedef enum {
	CVRF_REMEDIATION_UNKNOWN = 0,		///< Missing or invalid Remediation type
	CVRF_REMEDIATION_WORKAROUND,		///< Info about configuration/deployment to avoid exposure
	CVRF_REMEDIATION_MITIGATION,		///< Info about configuration/deployment to reduce risk
	CVRF_REMEDIATION_VENDOR_FIX,		///< Info about official fix issued by author of the product
	CVRF_REMEDIATION_NONE_AVAILABLE,	///< No fix is available for this Vulnerability
	CVRF_REMEDIATION_WILL_NOT_FIX,		///< No fix is or will ever be available for this Vulnerability
} cvrf_remediation_type_t;

/**
 * @memberof cvrf_remediation
 * Level of resolution currently obtainable for this Vulnerability
 * @param remed CVRF Remediation structure
 * @return Enum representing Type attribute of the Remediation element
 */
cvrf_remediation_type_t cvrf_remediation_get_type(struct cvrf_remediation *remed);

/**
 * Parses text value of the attribute and finds the appropriate Remediation type
 * @param reader XML text reader
 * @return Enum representation of the Type attribute of the Remediation element
 */
cvrf_remediation_type_t cvrf_remediation_type_parse(xmlTextReaderPtr reader);

/**
 * Find the string value of the Remediation type mapped to the provided enum value
 * @param type Enum representation of the Remediation's Type attribute
 * @return String representation of the Remediation's Type attribute
 */
const char *cvrf_remediation_type_get_text(cvrf_remediation_type_t remediation_type);


/************************************************************************************************
 * @struct cvrf_item_spec
 * Maps a cvrf_item_type_t enum to its text representation within a CVRF XML file,
 * as well as the parent container (if one exists)
 */
struct cvrf_item_spec;

typedef enum {
	CVRF_ITEM_UNKNOWN = 0,
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
} cvrf_item_type_t;

/**
 * Find the string representation of the item's name mapped to the provided enum value
 * @param type CVRF enumerated item type
 * @return String representation of the item's tag name
 */
const char *cvrf_item_type_get_text(cvrf_item_type_t type);

/**
 * Find the enum representation of the item's name mapped to the provided string
 * @param item String representation of the item's name
 * @return CVRF enumerated item type corresponding to the input
 */
cvrf_item_type_t cvrf_item_type_from_text(const char *item);

/**
 * @param item String representation of the item's name
 * @return true if the input maps to a valid item type
 */
bool cvrf_is_valid_item_type(const char *item);

/**
 * Find the string representation of the container name mapped to the provided enum value
 * @param type CVRF enumerated item type
 * @return String representation of the item's container name; NULL if it has no container
 */
const char *cvrf_item_type_get_container(cvrf_item_type_t type);

/**
 * Indicates whether the given type  has a unique parent node that contains only elements
 * of the given type (a container)
 * @param type CVRF enumerated item type
 * @return true if the given type has a container
 */
bool cvrf_item_type_has_container(cvrf_item_type_t type);


/*-----------------------------------------------------------------------------------------*\
|							CVRF Parsing & Serialization Functions							|
\*-----------------------------------------------------------------------------------------*/

/**
 * Parse CVRF Remediation
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Remediation
 */
struct cvrf_remediation *cvrf_remediation_parse(xmlTextReaderPtr reader);

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
 * Parse Product Status within CVRF vulnerability
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Product Status
 */
struct cvrf_product_status *cvrf_product_status_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF Involvement element
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Involvement
 */
struct cvrf_involvement *cvrf_involvement_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF vulnerability
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF vulnerability
 */
struct cvrf_vulnerability *cvrf_vulnerability_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF FullProductName item
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Relationship
 */
struct cvrf_product_name *cvrf_product_name_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF Group item
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Group
 */
struct cvrf_group *cvrf_group_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF Relationship item
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Relationship
 */
struct cvrf_relationship *cvrf_relationship_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF branch
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF branch
 */
struct cvrf_branch *cvrf_branch_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF product tree
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF product tree
 */
struct cvrf_product_tree *cvrf_product_tree_parse(xmlTextReaderPtr reader);

/**
 * Parse a CVRF Acknowledgment element
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Acknowledgment
 */
struct cvrf_acknowledgment *cvrf_acknowledgment_parse(xmlTextReaderPtr reader);

/**
 * Parse a CVRF Reference element
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Reference
 */
struct cvrf_reference *cvrf_reference_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF Note element
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Note
 */
struct cvrf_note *cvrf_note_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF Revision element
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF Revision
 */
struct cvrf_revision *cvrf_revision_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF DocumentTracking
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF DocumentTracking
 */
struct cvrf_doc_tracking *cvrf_doc_tracking_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF DocumentPublisher
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF DocumentPublisher
 */
struct cvrf_doc_publisher *cvrf_doc_publisher_parse(xmlTextReaderPtr reader);

/**
 * Parse structures in CVRF file that pertain to the document itself,
 * such as DocumentTracking, DocumentPublisher, and DocumentDistribution
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF document structure
 */
struct cvrf_document *cvrf_document_parse(xmlTextReaderPtr reader);

/**
 * Parse CVRF model
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF model
 */
struct cvrf_model *cvrf_model_parse(xmlTextReaderPtr reader);

/**
 * Parse all CVRF models from all files listed in an index file
 * @param index_source OSCAP source of index file containing list of all CVRF files
 * @return New CVRF index structure containing all CVRF models
 */
struct cvrf_index *cvrf_index_parse_xml(struct oscap_source *index_source);


/**
 * Helper function that calls the appropriate to DOM function for each item in the list using the provided
 * CVRF item type, and adds them all to a container node
 *@param list List representing all elements held in the container
 *@param cvrf_type Type of CVRF items held in the list
 *@param parent Parent node to which to add the child container node
 */
void cvrf_element_add_container(struct oscap_list *list, cvrf_item_type_t cvrf_type, xmlNode *parent);

/**
 * Helper function that creates an xmlNode element for each item in the list and adds them all
 * to the provided parent node
 * @param list Stringlist representing elements that have no child elements and no attributes
 * @param tag_name Tag name of elements in the list
 * @param parent Parent node to which to add all child nodes
 */
void cvrf_element_add_stringlist(struct oscap_stringlist *list, const char *tag_name, xmlNode *parent);

/**
 * Helper function that creates an xmlProp for the provided attribute name and value, and adds
 * it to the provided parent element
 * @param attr_name Tag name of the attribute
 * @paran attr_value Value of the attribute
 * @param element xmlNode to which to add the xmlProp attribute
 */
void cvrf_element_add_attribute(const char *attr_name, const char *attr_value, xmlNode *element);

/**
 * Helper function that calls cvrf_element_to_dom and then adds the resulting xmlNode
 * to a parent node
 * @param elm_name Tag name of the new element
 * @param elm_value Text value contained within the tags of the element
 * @param parent Parent node to which to add the child element
 */
void cvrf_element_add_child(const char *elm_name, const char *elm_value, xmlNode *parent);

/**
 * Helper function that contructs an xmlNode from a tagname and respective value
 * @param elm_name Tag name of the new element
 * @param elm_value Text value contained within the tags of the element
 * @return xmlNode representing the new child element
 */
xmlNode *cvrf_element_to_dom(const char *elm_name, const char *elm_value);

struct oscap_source *cvrf_index_get_export_source(struct cvrf_index *index);

struct oscap_source *cvrf_model_get_export_source(struct cvrf_model *model);

/**
 * Parent: Remediations container in a CVRF Vulnerability element
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure to be exported to XML
 * @return xmlNode representing Remediation element
 */
xmlNode *cvrf_remediation_to_dom(const struct cvrf_remediation *remed);

/**
 * Parent: Threats container in a CVRF Vulnerability element
 * @memberof cvrf_threat
 * @param threat CVRF Threat structure to be exported to XML
 * @return xmlNode representing Threat element
 */
xmlNode *cvrf_threat_to_dom(const struct cvrf_threat *threat);

/**
 * Parent: CVSSScoreSets container in a CVRF Vulnerability element
 * @memberof cvrf_score_set
 * @param score_set CVRF ScoreSet structure to be exported to XML
 * @return xmlNode representing ScoreSet element
 */
xmlNode *cvrf_score_set_to_dom(const struct cvrf_score_set *score_set);

/**
 * Parent: ProductStatuses container in a CVRF Vulnerability element
 * @memberof cvrf_product_status
 * @param stat CVRF Status structure to be exported to XML
 * @return xmlNode representing Status element
 */
xmlNode *cvrf_product_status_to_dom(const struct cvrf_product_status *stat);

/**
 * Parent: Involvements container in a CVRF Vulnerability element
 * @memberof cvrf_involvement
 * @param involve CVRF Involvement structure to be exported to XML
 * @return xmlNode representing Involvement element
 */
xmlNode *cvrf_involvement_to_dom(const struct cvrf_involvement *involve);

/**
 * Parent: CVRF Model root node
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure to be exported to XML
 * @return xmlNode representing Vulnerability element
 */
xmlNode *cvrf_vulnerability_to_dom(const struct cvrf_vulnerability *vuln);

/**
 * Parent: CVRF ProductTree, Branch, Relationship, or Group element
 * @memberof cvrf_product_name
 * @param full_name CVRF FullProductName structure to be exported to XML
 * @return xmlNode representing FullProductName element
 */
xmlNode *cvrf_product_name_to_dom(struct cvrf_product_name *full_name);

/**
 * Parent: CVRF ProductGroups container
 * @memberof cvrf_group
 * @param group CVRF Group structure to be exported to XML
 * @return xmlNode representing Group element
 */
xmlNode *cvrf_group_to_dom(const struct cvrf_group *group);

/**
 * Parent: CVRF ProductTree element
 * @memberof cvrf_relationship
 * @param relation CVRF Relationship structure to be exported to XML
 * @return xmlNode representing Relationship element
 */
xmlNode *cvrf_relationship_to_dom(const struct cvrf_relationship *relation);

/**
 * Parent: CVRF ProductTree element or another Branch element
 * @memberof cvrf_branch
 * @param branch CVRF Branch structure to be exported to XML
 * @return xmlNode representing Branch element
 */
xmlNode *cvrf_branch_to_dom(struct cvrf_branch *branch);

/**
 * Parent: CVRF Model root node
 * @memberof cvrf_product_tree
 * @param tree CVRF ProductTree structure to be exported to XML
 * @return xmlNode representing ProductTree element
 */
xmlNode *cvrf_product_tree_to_dom(struct cvrf_product_tree *tree);

/**
 * Parent: Acknowledgements container
 * @memberof cvrf_acknowledgment
 * @param ack CVRF Acknowledgment structure to be exported to XML
 * @return xmlNode representing Acknowledgment element
 */
xmlNode *cvrf_acknowledgment_to_dom(struct cvrf_acknowledgment *ack);

/**
 * Parent: References or DocumentReferences container
 * @memberof cvrf_reference
 * @param ref CVRF Reference structure to be exported to XML
 * @return xmlNode representing Reference element
 */
xmlNode *cvrf_reference_to_dom(struct cvrf_reference *ref);

/**
 * Parent: Notes or DocumentNotes container (of root node or Vulnerability node,
 * respectively)
 * @memberof cvrf_note
 * @param note CVRF Note structure to be exported to XML
 * @return xmlNode representing Note element
 */
xmlNode *cvrf_note_to_dom(struct cvrf_note *note);

/**
 * Parent: RevisionHistory container in CVRF DocumentTracking element
 * @memberof cvrf_revision
 * @param revision CVRF Revision structure to be exported to XML
 * @return xmlNode representing Revision element
 */
xmlNode *cvrf_revision_to_dom(struct cvrf_revision *revision);

/**
 * Parent: CVRF Model root node
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure to be exported to XML
 * @return xmlNode representing DocumentTracking element
 */
xmlNode *cvrf_doc_tracking_to_dom(struct cvrf_doc_tracking *tracking);

/**
 * Parent: CVRF Model root node
 * @memberof cvrf_doc_publisher
 * @param publisher CVRF DocumentPublisher structure to be exported to XML
 * @return xmlNode representing DocumentPublisher element
 */
xmlNode *cvrf_doc_publisher_to_dom(struct cvrf_doc_publisher *publisher);

/**
 * @memberof cvrf_mode
 * @param model CVRF Model structure to be exported to DOM
 * @param doc XML document to which to export the model
 * @param parent Parent node of the CVRF Model node; NULL if Model is the root
 * @param user_args Additional arguments provided by the caller
 * @return xmlNode representing the CVRF Model structure
 */
xmlNode *cvrf_model_to_dom(struct cvrf_model *model, xmlDocPtr doc, xmlNode *parent, void *user_args);

/**
 * @memberof index CVRF Index structure to be exported to DOM
 * @param doc XML document to which to export all the models in the Index structure
 * @param parent Parent node of the CVRF Index node; NULL if Index is the root
 * @param user_args Additional arguments provided by the caller
 * @return xmlNode representing the CVRF Index structure
 */
xmlNode *cvrf_index_to_dom(struct cvrf_index *index, xmlDocPtr doc, xmlNode *parent, void *user_args);

OSCAP_HIDDEN_END;
#endif				/* _CVRF_PRIV_H_ */
