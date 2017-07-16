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
 */
struct cvrf_model;

/**
 *@struct product_tree
 *
 */
struct product_tree;

/**
 * @struct cvrf_branch
 *
 */
struct cvrf_branch;

/**
 * @struct product_name
 *
 */
struct product_name;

/**
 * @struct cvrf_vulnerability
 *
 */
struct cvrf_vulnerability;


/************************************************************/
/**
 * @name Getters
 * Return value is pointer to structure's member. Do not free unless you null the pointer in the structure.
 * Use remove function otherwise.
 * @{
 * */

/************************************************************/
/** @} End of Getters group */


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
 * New ProductTree structure
 * @memberof product_tree
 * @return New CVRF ProductTree
 */
struct product_tree *product_tree_new(void);

/**
 * New CVRF branch of ProductTree or sub-branch
 * @memberof cvrf_branch
 * @return New CVRF branch
 */
struct cvrf_branch *cvrf_branch_new(void);

/**
 * New FullProductName of Branch or ProductTree
 * @memberof product_name
 * @return New FullProductName
 */
struct product_name *product_name_new(void);

/**
 * New CVRF Vulnerability
 * @memberof cvrf_vulnerability
 * @return New CVRF Vulnerability
 */
struct cvrf_vulnerability *cvrf_vulnerability_new(void);

/**
 * New ProductStatus member of a CVRF Vulnerability
 * @memberof product_status
 * @return New CVRF ProductStatus
 */
struct product_status *product_status_new(void);


/**
 *
 *
 */
void cvrf_model_free(struct cvrf_model *cvrf) ;

/**
 *
 *
 */
void product_tree_free(struct product_tree *tree);

/**
 *
 *
 */
void cvrf_branch_free(struct cvrf_branch *branch);

/**
 *
 *
 */
void product_name_free(struct product_name *full_name);

/**
 *
 *
 */
void cvrf_vulnerability_free(struct cvrf_vulnerability *vulnerability);

/**
 *
 *
 */
void product_status_free(struct product_status *status);



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
