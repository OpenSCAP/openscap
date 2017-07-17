/**
 * @addtogroup CVRF
 * @{
 *
 * @file cvrf.h
 * @brief Interface to Common Vulnerability Reporting Framework
 */

#ifndef _CVRF_H_
#define _CVRF_H_

#include <stdbool.h>
#include <time.h>
#include "oscap.h"
#include "cpe_name.h"

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
 * @struct cvrf_product_status
 * Structure holding CVRF ProductStatus data (within a Vulnerability)
 * Has status type and list of ProductIDs
 */
struct cvrf_product_status;


/************************************************************/
/**
 * @name Getters
 * Return value is pointer to structure's member. Do not free unless you null the pointer in the structure.
 * Use remove function otherwise.
 * @{
 * */

const char *cvrf_model_get_doc_title(const struct cvrf_model *model);

const char *cvrf_model_get_doc_type(const struct cvrf_model *model);

struct cvrf_product_tree *cvrf_model_get_product_tree(struct cvrf_model *model);

struct cvrf_vulnerability_iterator *cvrf_model_get_vulnerabilities(const struct cvrf_model *model);


struct cvrf_doc_tracking *cvrf_document_get_tracking(struct cvrf_document *doc);


const char *cvrf_doc_tracking_get_tracking_id(const struct cvrf_doc_tracking *tracking);

const char *cvrf_doc_tracking_get_tracking_alias(const struct cvrf_doc_tracking *tracking);

const char *cvrf_doc_tracking_get_tracking_status(const struct cvrf_doc_tracking *tracking);

const char *cvrf_doc_tracking_get_init_release_date(const struct cvrf_doc_tracking *tracking);

const char *cvrf_doc_tracking_get_cur_release_date(const struct cvrf_doc_tracking *tracking);


struct oscap_list_iterator *cvrf_product_tree_get_branches(struct cvrf_product_tree *tree);

struct cvrf_relationship_iterator *cvrf_product_tree_get_relationships(const struct cvrf_product_tree *tree);


const char *cvrf_branch_get_branch_type(const struct cvrf_branch *branch);

const char *cvrf_branch_get_branch_name(const struct cvrf_branch *branch);

struct oscap_list_iterator *cvrf_branch_get_subbranches(struct cvrf_branch *branch);


const char *cvrf_relationship_get_product_reference(const struct cvrf_relationship *relation);

const char *cvrf_relationship_get_relation_type(const struct cvrf_relationship *relation);

const char *cvrf_relationship_get_relates_to_ref(const struct cvrf_relationship *relation);

struct cvrf_product_name *cvrf_relationship_get_product_name(struct cvrf_relationship *relation);


const char *cvrf_product_name_get_product_id(const struct cvrf_product_name *full_name);

const char *cvrf_product_name_get_cpe(const struct cvrf_product_name *full_name);


const char *cvrf_vulnerability_get_vulnerability_title(const struct cvrf_vulnerability *vuln);

const char *cvrf_vulnerability_get_cve_id(const struct cvrf_vulnerability *vuln);

const char *cvrf_vulnerability_get_cwe_id(const struct cvrf_vulnerability *vuln);

struct cvrf_product_status_iterator *cvrf_vulnerability_get_cvrf_product_statuses(const struct cvrf_vulnerability *vuln);


const char *cvrf_product_status_get_status(struct cvrf_product_status *stat);

struct oscap_string_iterator *cvrf_product_status_get_ids(struct cvrf_product_status *stat);

/************************************************************/
/** @} End of Getters group */



/************************************************************/
/**
 * @name Setters
 * For lists use add functions. Parameters of set functions are duplicated in memory and need to
 * be freed by caller.
 * @{
 */

struct cvrf_vulnerability_iterator;
struct cvrf_vulnerability *cvrf_vulnerability_iterator_next(struct cvrf_vulnerability_iterator *it);
bool cvrf_vulnerability_iterator_has_more(struct cvrf_vulnerability_iterator *it);
void cvrf_vulnerability_iterator_free(struct cvrf_vulnerability_iterator *it);

struct cvrf_relationship_iterator;
struct cvrf_relationship *cvrf_relationship_iterator_next(struct cvrf_relationship_iterator *it);
bool cvrf_relationship_iterator_has_more(struct cvrf_relationship_iterator *it);
void cvrf_relationship_iterator_free(struct cvrf_relationship_iterator *it);

struct cvrf_product_status_iterator;
struct cvrf_product_status *cvrf_product_status_iterator_next(struct cvrf_product_status_iterator *it);
bool cvrf_product_status_iterator_has_more(struct cvrf_product_status_iterator *it);
void cvrf_product_status_iterator_free(struct cvrf_product_status_iterator *it);

/************************************************************/
/** @} End of Setters group */



/**
 * Get supported version of CVRF XML
 * @return version of XML file format
 * @memberof cvrf_model
 */
const char * cvrf_model_supported(void);

/**
 * New CVRF model
 * @memberof cvrf_model
 * @return New CVRF model
 */
struct cvrf_model *cvrf_model_new(void);

/**
 *
 *@memberof cvrf_document
 *@return New CVRF Document structure
 */
struct cvrf_document *cvrf_document_new(void);

/**
 *
 *@memberof cvrf_doc_tracking
 *@return New CVRF DocumentTracking
 */
struct cvrf_doc_tracking *cvrf_doc_tracking_new(void);

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
 * New ProductStatus member of a CVRF Vulnerability
 * @memberof cvrf_product_status
 * @return New CVRF ProductStatus
 */
struct cvrf_product_status *cvrf_product_status_new(void);


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
void cvrf_doc_tracking_free(struct cvrf_doc_tracking *tracking);

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
void cvrf_product_status_free(struct cvrf_product_status *status);



/**
 * Export CVRF model to XML file
 * @memberof cvrf_model
 * @param cvrf CVRF model
 * @param file OSCAP export target
 */
void cvrf_model_export(struct cvrf_model *cvrf, const char *file);

/**
 * Parses the specified XML file and creates a list of CVRF data structures.
 * @memberof cvrf_model
 * @param file filename
 * @return New CVRF model structure
 */
struct cvrf_model *cvrf_model_import(const char *file);


/**@}*/

#endif				/* _CVRF_H_ */
