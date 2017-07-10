/**
 * @addtogroup CVRF
 * @{
 *
 * @file cvrf.h
 * @brief Interface to Common Vulnerability and Exposure dictionary
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


/**@}*/

#endif				/* _CVRF_H_ */
