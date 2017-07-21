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
 * @struct cvrf_index
 * Represents an index of a CVRF feed or directory
 * Maintains a list of all CVRF files in the form of cvrf_model structures
 */
struct cvrf_index;

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
 * Contains at least one ProductStatus
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


/************************************************************************************/
/************************************************************************************/
/**
 * @name Getters
 * Return value is pointer to structure's member. Do not free unless you null the pointer in the structure.
 * Use remove function otherwise.
 * @{
 * */

const char *cvrf_index_get_source_url(const struct cvrf_index *index);


const char *cvrf_model_get_doc_title(const struct cvrf_model *model);

const char *cvrf_model_get_doc_type(const struct cvrf_model *model);

struct cvrf_product_tree *cvrf_model_get_product_tree(struct cvrf_model *model);

struct cvrf_vulnerability_iterator *cvrf_model_get_vulnerabilities(const struct cvrf_model *model);

struct cvrf_document *cvrf_model_get_document(struct cvrf_model *model);

const char *cvrf_model_get_identification(struct cvrf_model *model);


struct cvrf_doc_tracking *cvrf_document_get_tracking(struct cvrf_document *doc);


const char *cvrf_doc_tracking_get_tracking_id(const struct cvrf_doc_tracking *tracking);

const char *cvrf_doc_tracking_get_tracking_alias(const struct cvrf_doc_tracking *tracking);

const char *cvrf_doc_tracking_get_tracking_status(const struct cvrf_doc_tracking *tracking);

const char *cvrf_doc_tracking_get_init_release_date(const struct cvrf_doc_tracking *tracking);

const char *cvrf_doc_tracking_get_cur_release_date(const struct cvrf_doc_tracking *tracking);

const char *cvrf_doc_tracking_get_generator_engine(const struct cvrf_doc_tracking *tracking);

const char *cvrf_doc_tracking_get_generator_date(const struct cvrf_doc_tracking *tracking);


struct oscap_iterator *cvrf_product_tree_get_branches(struct cvrf_product_tree *tree);

struct cvrf_relationship_iterator *cvrf_product_tree_get_relationships(const struct cvrf_product_tree *tree);


const char *cvrf_branch_get_branch_type(const struct cvrf_branch *branch);

const char *cvrf_branch_get_branch_name(const struct cvrf_branch *branch);

struct cvrf_product_name *cvrf_branch_get_cvrf_product_name(struct cvrf_branch *branch);

struct oscap_iterator *cvrf_branch_get_subbranches(struct cvrf_branch *branch);


const char *cvrf_relationship_get_product_reference(const struct cvrf_relationship *relation);

const char *cvrf_relationship_get_relation_type(const struct cvrf_relationship *relation);

const char *cvrf_relationship_get_relates_to_ref(const struct cvrf_relationship *relation);

struct cvrf_product_name *cvrf_relationship_get_product_name(struct cvrf_relationship *relation);


const char *cvrf_product_name_get_product_id(const struct cvrf_product_name *full_name);

const char *cvrf_product_name_get_cpe(const struct cvrf_product_name *full_name);


const char *cvrf_vulnerability_get_vulnerability_title(const struct cvrf_vulnerability *vuln);

int cvrf_vulnerablity_get_ordinal(struct cvrf_vulnerability *vuln);

const char *cvrf_vulnerability_get_cve_id(const struct cvrf_vulnerability *vuln);

const char *cvrf_vulnerability_get_cwe_id(const struct cvrf_vulnerability *vuln);

struct cvrf_product_status_iterator *cvrf_vulnerability_get_cvrf_product_statuses(const struct cvrf_vulnerability *vuln);

struct cvrf_remediation_iterator *cvrf_vulnerability_get_remediations(const struct cvrf_vulnerability *vuln);

int cvrf_vulnerability_get_remediation_count(struct cvrf_vulnerability *vuln);


const char *cvrf_product_status_get_status(const struct cvrf_product_status *stat);

struct oscap_string_iterator *cvrf_product_status_get_ids(struct cvrf_product_status *stat);


struct cvrf_model *cvrf_eval_get_model(struct cvrf_model_eval *eval);

struct oscap_string_iterator *cvrf_model_eval_get_product_ids(struct cvrf_model_eval *eval);

const char *cvrf_model_eval_get_os_name(const struct cvrf_model_eval *eval);

const char *cvrf_model_eval_get_os_version(const struct cvrf_model_eval *eval);

/************************************************************************************/
/************************************************************************************/
/** @} End of Getters group */



/************************************************************************************/
/************************************************************************************/
/**
 * @name Setters
 * For lists use add functions. Parameters of set functions are duplicated in memory and need to
 * be freed by caller.
 * @{
 */

bool cvrf_index_set_source_url(struct cvrf_index *index, const char *source_url);

struct cvrf_model_iterator;
bool cvrf_index_add_model(struct cvrf_index *index, struct cvrf_model *model);
struct cvrf_model *cvrf_model_iterator_next(struct cvrf_model_iterator *it);
bool cvrf_model_iterator_has_more(struct cvrf_model_iterator *it);
void cvrf_model_iterator_free(struct cvrf_model_iterator *it);
void cvrf_model_iterator_reset(struct cvrf_model_iterator *it);
void cvrf_model_iterator_remove(struct cvrf_model_iterator *it);


bool cvrf_model_set_doc_title(struct cvrf_model *model, const char *doc_title);

bool cvrf_model_set_doc_type(struct cvrf_model *model, const char *doc_type);


struct cvrf_vulnerability_iterator;
bool cvrf_model_add_vulnerability(struct cvrf_model *model, struct cvrf_vulnerability *vuln);
struct cvrf_vulnerability *cvrf_vulnerability_iterator_next(struct cvrf_vulnerability_iterator *it);
bool cvrf_vulnerability_iterator_has_more(struct cvrf_vulnerability_iterator *it);
void cvrf_vulnerability_iterator_free(struct cvrf_vulnerability_iterator *it);
void cvrf_vulnerability_iterator_reset(struct cvrf_vulnerability_iterator *it);
void cvrf_vulnerability_iterator_remove(struct cvrf_vulnerability_iterator *it);


void cvrf_document_set_tracking(struct cvrf_document *doc, struct cvrf_doc_tracking *track);


bool cvrf_doc_tracking_set_tracking_id(struct cvrf_doc_tracking *tracking, const char *tracking_id);

bool cvrf_doc_tracking_set_tracking_alias(struct cvrf_doc_tracking *tracking, const char *tracking_alias);

bool cvrf_doc_tracking_set_tracking_status(struct cvrf_doc_tracking *tracking, const char *tracking_status);

bool cvrf_doc_tracking_set_init_release_date(struct cvrf_doc_tracking *tracking, const char *init_release_date);

bool cvrf_doc_tracking_set_cur_release_date(struct cvrf_doc_tracking *tracking, const char *cur_release_date);

bool cvrf_doc_tracking_set_generator_engine(struct cvrf_doc_tracking *tracking, const char *generator_engine);

bool cvrf_doc_tracking_set_generator_date(struct cvrf_doc_tracking *tracking, const char *generator_date);


struct cvrf_relationship_iterator;
struct cvrf_relationship *cvrf_relationship_iterator_next(struct cvrf_relationship_iterator *it);
bool cvrf_relationship_iterator_has_more(struct cvrf_relationship_iterator *it);
void cvrf_relationship_iterator_free(struct cvrf_relationship_iterator *it);
void cvrf_relationship_iterator_reset(struct cvrf_relationship_iterator *it);


bool cvrf_branch_set_branch_type(struct cvrf_branch *branch, const char *branch_type);

bool cvrf_branch_set_branch_name(struct cvrf_branch *branch, const char *branch_name);


bool cvrf_relationship_set_product_reference(struct cvrf_relationship *relation, const char *product_reference);

bool cvrf_relationship_set_relation_type(struct cvrf_relationship *relation, const char *relation_type);

bool cvrf_relationship_set_relates_to_ref(struct cvrf_relationship *relation, const char *relates_to_ref);



bool cvrf_product_name_set_product_id(struct cvrf_product_name *full_name, const char *product_id);

bool cvrf_product_name_set_cpe(struct cvrf_product_name *full_name, const char *cpe);


bool cvrf_vulnerability_set_vulnerability_title(struct cvrf_vulnerability *vuln, const char *vulnerability_title);

bool cvrf_vulnerability_set_cve_id(struct cvrf_vulnerability *vuln, const char *cve_id);

bool cvrf_vulnerability_set_cwe_id(struct cvrf_vulnerability *vuln, const char *cwe_id);

struct cvrf_product_status_iterator;
struct cvrf_product_status *cvrf_product_status_iterator_next(struct cvrf_product_status_iterator *it);
bool cvrf_product_status_iterator_has_more(struct cvrf_product_status_iterator *it);
void cvrf_product_status_iterator_free(struct cvrf_product_status_iterator *it);
void cvrf_product_status_iterator_reset(struct cvrf_product_status_iterator *it);

struct cvrf_remediation_iterator;
struct cvrf_remediation *cvrf_remediation_iterator_next(struct cvrf_remediation_iterator *it);
bool cvrf_remediation_iterator_has_more(struct cvrf_remediation_iterator *it);
void cvrf_remediation_iterator_free(struct cvrf_remediation_iterator *it);
void cvrf_remediation_iterator_reset(struct cvrf_remediation_iterator *it);


bool cvrf_product_status_set_status(struct cvrf_product_status *stat, const char *status);


void cvrf_eval_set_model(struct cvrf_model_eval *eval, struct cvrf_model *model);

bool cvrf_model_eval_set_os_name(struct cvrf_model_eval *eval, const char *os_name);

bool cvrf_model_eval_set_os_version(struct cvrf_model_eval *eval, const char *os_version);

/************************************************************************************/
/************************************************************************************/
/** @} End of Setters group */



/**
 * Get supported version of CVRF XML
 * @return version of XML file format
 * @memberof cvrf_model
 */
const char * cvrf_model_supported(void);


/**
 * New CVRF index
 * @memberof cvrf_index
 * @return New CVRF index
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



/************************************************************************************/
/************************************************************************************/
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

const char * cvrf_model_supported(void);


void cvrf_get_os_info(const char *input_file, struct cvrf_model_eval *eval);

void cvrf_export_results(const char *input_file, const char *export_file, const char *os_version);

void get_cvrf_product_id_by_OS(struct cvrf_model_eval *eval, struct cvrf_model *model);

const char *get_cvrf_product_id_from_branch(struct cvrf_model_eval *eval, struct cvrf_branch *branch);

bool cvrf_product_vulnerability_fixed(struct cvrf_vulnerability *vuln, char *product);

int cvrf_rpm_product_id_rpminfo(void);


/**@}*/

#endif				/* _CVRF_H_ */
