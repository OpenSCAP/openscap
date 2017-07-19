/**
 * @file cve_priv.h
 */

#ifndef CVRF_PRIV_H_
#define CVRF_PRIV_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "../common/list.h"
#include "../common/elements.h"

/**
 * @struct cvrf_model
 * Structure holding CVRF model
 * Top level structure; contains ProductTree and list of Vulnerabilities
 */
struct cvrf_model;

/**
 * @struct cvrf_document
 *
 */
struct cvrf_document;

/**
 * @struct cvrf_doc_tracking
 *
 */
struct cvrf_doc_tracking;

/**
 *@struct cvrf_product_tree
 * Structure holding CVRF ProductTree data
 * Holds at least one CVRF branch
 */
struct cvrf_product_tree;

/**
 * @struct cvrf_branch
 * Structure holding CVRF branch data
 * Belongs to a ProductTree; may have its own sub-branches
 */
struct cvrf_branch;

/**
 * @struct cvrf_relationship
 * Structure holding data for Relationships within ProductTree
 */
struct cvrf_relationship;

/**
 * @struct cvrf_product_name
 * Structure holding CVRF product name data
 * ProductID and CPE data for CVRF branches and ProductTrees
 */
struct cvrf_product_name;

/**
 * @struct cvrf_vulnerability
 * Structure holding CVRF Vulnerability data
 * Contains at least on ProductStatus
 */
struct cvrf_vulnerability;

/**
 * @struct cvrf_remediation
 * Structure holding remediation info for a particular vulnerability
 * May reference URL, ProductID, and/or GroupID
 */
struct cvrf_remediation;

/**
 * @struct cvrf_product_status
 * Structure holding CVRF ProductStatus data (within a Vulnerability)
 * Has status type and list of ProductIDs
 */
struct cvrf_product_status;

/**
 * @struct cvrf_model_eval
 *
 *
 */
struct cvrf_model_eval;


/**
 * Parse CVRF model from XML (private function)
 * @param file OSCAP import source
 * @return New parsed CVRF model from source
 */
struct cvrf_model *cvrf_model_parse_xml(const char *file);

/**
 * Parse CVRF model
 * @param reader XML Text Reader representing XML model
 * @return parsed CVRF model
 */
struct cvrf_model *cvrf_model_parse(xmlTextReaderPtr reader);

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



/**
 *
 * @param cvrf CVRF model to export
 * @param file OSCAP export target
 */
void cvrf_model_export_xml(struct cvrf_model *cvrf, const char *file);

/**
 *
 * @param tracking CVRF DocumentTracking element to export
 * @param writer XML Text Writer representing XML model
 */
void cvrf_doc_tracking_export(const struct cvrf_doc_tracking *tracking, xmlTextWriterPtr writer);

/**
 *
 * @param cvrf CVRF model
 * @param writer XML Text Writer representing XML model
 */
void cvrf_export(const struct cvrf_model *cvrf, xmlTextWriterPtr writer);

/**
 * @param tree ProductTree within CVRF model
 * @param writer XML Text Writer representing XML model
 */
void cvrf_product_tree_export(const struct cvrf_product_tree *tree, xmlTextWriterPtr writer);

/**
 *
 * @param branch a CVRF branch of a ProductTree
 * @param writer XML Text Writer representing XML model
 */
void cvrf_branch_export(const struct cvrf_branch *branch, xmlTextWriterPtr writer);

/**
 *
 * @param relation A CVRF relationship item of ProductTree
 * @param writer XML Text Writer representing XML model
 */
void cvrf_relationship_export(const struct cvrf_relationship *relation, xmlTextWriterPtr writer);

/**
 *
 * @param vuln a CVRF Vulnerability item within CVRF model
 * @param writer XML Text Writer representing XML model
 */
void cvrf_vulnerability_export(const struct cvrf_vulnerability *vuln, xmlTextWriterPtr writer);

/**
 *
 * @param remed a CVRF Remediation for a CVRF Vulnerability item
 * @param writer XML Text Writer representing XML model
 */
void cvrf_remediation_export(const struct cvrf_remediation *remed, xmlTextWriterPtr writer);


/**
 * @param stat Product status within CVRF Vulnerability
 * @param writer XML Text Writer representing XML model
 */
void cvrf_product_status_export(const struct cvrf_product_status *stat, xmlTextWriterPtr writer);



#endif				/* _CVRF_PRIV_H_ */
