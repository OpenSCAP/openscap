/** 
 * @addtogroup CVE
 * @{
 *
 * @file cve.h
 * \brief Interface to Common Vulnerability and Exposure dictionary
 *   
 *   See details at:
 *     http://cve.mitre.org/
 *     http://nvd.nist.gov/download.cfm
 *  
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 *      Maros Barabas  <mbarabas@redhat.com>
 *      Tomas Heinrich <theinric@redhat.com>
 */

#ifndef _CVE_H_
#define _CVE_H_

#include <stdbool.h>
#include "oscap.h"
#include "cpeuri.h"

/** 
 * @struct cve_model
 * Structure holding CVE model
 */
struct cve_model;
/** 
 * @struct cve_entry
 * Structure holding CVE entry data
 */
struct cve_entry;
/** 
 * @struct cve_summary
 * Structure holding CVE summary data
 */
struct cve_summary;
/** 
 * @struct cve_product
 * Structure holding CVE product data
 */
struct cve_product;
/** 
 * @struct cve_configuration
 * Structure CVE vulnerable configuration data
 */
struct cve_configuration;
/** 
 * @struct cwe_entry
 * Structure holding CWE data
 */
struct cwe_entry;
/** 
 * @struct cve_reference
 * Structure holding CVE reference data
 */
struct cve_reference;

/**
 * @name Get functions
 * Functions for getting attributes from CVE model structures. Return value is pointer to structure's member. Do not 
 * free unless you null the pointer in the structure. Use remove function otherwise.
 * @{
 * */

/**
 * Get en iterator to CVE entries
 * @param cve_model CVE model
 * @relates cve_entry
 */
struct cve_entry_iterator* cve_model_get_entries(const struct cve_model* cve_model);

/**
 * Get CVE entry ID
 * @param item CVE entry
 * @relates cve_entry
 */
const char* cve_entry_get_id(const struct cve_entry* item);

/**
 * Get CVE entry CWE
 * @param item CVE entry
 * @relates cve_entry
 */
const char* cve_entry_get_cwe(const struct cve_entry* item);

/**
 * Get CVE entry summary
 * @param item CVE entry
 * @relates cve_entry
 */
struct cve_summary_iterator* cve_entry_get_summaries(const struct cve_entry* item);

/**
 * Get an iterator to CVE entry's references
 * @param item CVE entry
 * @relates cve_entry
 */
struct cve_reference_iterator* cve_entry_get_references(const struct cve_entry* item);

/**
 * Get CVE reference values
 * @param ref CVE reference
 * @relates cve_reference
 */
const char* cve_reference_get_value(const struct cve_reference* ref);

/**
 * Get CVE reference href
 * @param ref CVE reference
 * @relates cve_reference
 */
const char* cve_reference_get_href(const struct cve_reference* ref);

/**
 * Get CVE reference type
 * @param ref CVE reference
 * @relates cve_reference
 */
const char* cve_reference_get_type(const struct cve_reference* ref);

/**
 * Get CVE reference source
 * @param ref CVE reference
 * @relates cve_reference
 */
const char* cve_reference_get_source(const struct cve_reference * ref);

/**
 * Get value from CVE summary
 * @param summary CVE summary
 * @relates cve_summary
 */
const char* cve_summary_get_summary(const struct cve_summary * summary);
/**
 * Get CVE product value
 * @param prodct CVE product
 * @relates cve_
 */
const char* cve_product_get_value(const struct cve_product * product);
/**
 * Get CVE entry value
 * @param entry CVE entry
 * @relates cve_entry
 */
const char* cwe_entry_get_value(const struct cwe_entry * entry);
/**
 * Get CVE configuration id
 * @param conf CVE vulnerable configuration
 * @relates cve_configuration
 */
const char* cve_configuration_get_id(const struct cve_configuration * conf);
/**
 * Get CVE entry published date
 * @param entry CVE entry
 * @relates cve_entry
 */
const char* cve_entry_get_published(const struct cve_entry * entry);
/**
 * Get CVE entry modified
 * @param entry CVE entry
 * @relates cve_entry
 */
const char* cve_entry_get_modified(const struct cve_entry * entry);
/**
 * Get CVE entry protection
 * @param entry CVE entry
 * @relates cve_entry
 */
const char* cve_entry_get_sec_protection(const struct cve_entry * entry);

/**
 * Get CVE entry products
 * @param entry CVE entry
 * @relates cve_entry
 */
struct cve_product_iterator * cve_entry_get_products(const struct cve_entry * entry);
/**
 * Get CVE .
 * @param entry CVE entry
 * @relates cve_entry
 */
struct cve_configuration_iterator * cve_entry_get_configurations(const struct cve_entry * entry);
/**
 * Get CVE configuration test expression.
 * @param conf CVE configuration
 * @relates cve_configuration
 */
const struct cpe_testexpr * cve_configuration_get_expr(const struct cve_configuration * conf);

/*@}*/

/**
 * @name Add functions
 * Functions to add member to list. Return value is true if added succesfuly or false in case of error. 
 * @{
 * */

/**
 * Add entry to CVE model
 * @param model CVE model
 * @param new_entry New CVE entry
 * @relates cve_model
 * @return true if added, false otherwise
 */
bool cve_model_add_entry(struct cve_model *model, struct cve_entry *new_entry);

/*@}*/

/**
 * @name Set functions
 * Set functions assign values to members of structures except lists. For lists use add functions. 
 * Parameters of set functions are duplicated in memory and need to be freed by caller.
 * @{
 * */

/**
 * Set id of CVE entry
 * @param entry CVE entry
 * @param new_id id of CVE entry
 * @relates cve_entry
 * return true if set, false otherwise
 */
bool cve_entry_set_id(struct cve_entry *entry, const char *new_id);
/**
 * Set publish date of CVE entry
 * @param entry CVE entry
 * @param new_published date of CVE attribute 
 * @relates cve_entry
 * return true if set, false otherwise
 */
bool cve_entry_set_published(struct cve_entry *entry, const char *new_published);
/**
 * Set modified date of CVE entry
 * @param entry CVE entry
 * @param new_modified CVE modified date
 * @relates cve_entry
 * return true if set, false otherwise
 */
bool cve_entry_set_modified(struct cve_entry *entry, const char *new_modified);
/**
 * Set protection of CVE entry
 * @param entry CVE entry
 * @param new_protection CVE protection
 * @relates cve_entry
 * return true if set, false otherwise
 */
bool cve_entry_set_sec_protection(struct cve_entry *entry, const char *new_protection);
/**
 * Set cwe of CVE entry
 * @param entry CVE entry
 * @param cwe CWE of CVE
 * @relates cve_entry
 * return true if set, false otherwise
 */
bool cve_entry_set_cwe(struct cve_entry *entry, const char *cwe);
/**
 * Set value of CVE entry
 * @param entry CVE entry
 * @param new_value CVE value
 * @relates cve_entry
 * return true if set, false otherwise
 */
bool cwe_entry_set_value(struct cwe_entry *entry, const char *new_value);

/**
 * Set value of CVE reference
 * @param reference CVE reference
 * @param new_value CVE reference value
 * @relates cve_reference
 * return true if set, false otherwise
 */
bool cve_reference_set_value(struct cve_reference *reference, const char *new_value);
/**
 * Set href of CVE reference
 * @param reference CVE reference
 * @param new_href CVE reference href
 * @relates cve_reference href
 * return true if set, false otherwise
 */
bool cve_reference_set_href(struct cve_reference *reference, const char *new_href);
/**
 * Set type of CVE reference
 * @param reference CVE reference
 * @param new_type CVE reference type
 * @relates cve_reference type
 * return true if set, false otherwise
 */
bool cve_reference_set_type(struct cve_reference *reference, const char *new_type);
/**
 * Set source of CVE reference
 * @param reference CVE reference
 * @param new_source CVE reference source
 * @relates cve_reference
 * return true if set, false otherwise
 */
bool cve_reference_set_source(struct cve_reference *reference, const char *new_source);

/**
 * Set id of CVE configuration
 * @param conf CVE vulnerability configuration
 * @param new_id id of CVE configuration
 * @relates cve_configuration
 * @return true if set, false otherwise
 */
bool cve_configuration_set_id(struct cve_configuration * conf, const char *new_id);
/**
 * Set value of CVE product
 * @param product CVE product
 * @param new_value value of CVE product
 * @relates cve_product
 * @return true if set, false otherwise
 */
bool cve_product_set_value(struct cve_product *product, const char *new_value);
/**
 * Set summary of CVE summary
 * @param summary CVE summary
 * @param new_summary summary of CVE summary
 * @relates cve_summary
 * @return true if set, false otherwise
 */
bool cve_summary_set_summary(struct cve_summary *summary, const char *new_summary);

/*@}*/

/**
 * @name New functions
 * Constructors of CVE model structures. Free function returns new empty allocated structure.
 * If returns non NULL it need to be freed by the caller.
 * @{
 * */

/**
 * New CVE entry
 * @relates cve_entry
 * @return New CVE entry
 */
struct cve_entry * cve_entry_new(void);
/**
 * New CVE vulnerability configuration
 * @relates cve_configuration
 * @return New CVE vulnerability configuration
 */
struct cve_configuration * cve_configuration_new(void);
/**
 * New CWE entry
 * @relates cwe_entry
 * @return New CWE entry
 */
struct cwe_entry * cwe_entry_new(void);
/**
 * New CVE product
 * @relates cve_product
 * @return New CVE product
 */
struct cve_product * cve_product_new(void);
/**
 * New CVE summary
 * @relates cve_summary
 * @return New CVE summary
 */
struct cve_summary * cve_summary_new(void);
/**
 * New CVE reference
 * @relates cve_reference
 * @return New CVE reference
 */
struct cve_reference * cve_reference_new(void);
/**
 * New CVE model
 * @relates cve_model
 * @return New CVE model
 */
struct cve_model * cve_model_new(void);

/*@}*/

/**
 * @name Free functions
 * Destructors of CVE model structures. Functions free structures with all members recursively. 
 * For simple deletion of entity use remove functions.
 * @{
 * */

/**
 * Free CVE model
 * @param cve_model CVE model
 * @relates cve_model
 */
void cve_model_free(struct cve_model * cve_model);

/**
 * Free CVE entry
 * @param entry CVE entry
 * @relates cve_entry
 */
void cve_entry_free(struct cve_entry * entry);

/**
 * Free CVE summary
 * @param summary CVE summary
 * @relates cve_summary
 */
void cve_summary_free(struct cve_summary * summary);

/**
 * Free CVE product
 * @param product CVE product
 * @relates cve_product
 */
void cve_product_free(struct cve_product * product);

/**
 * Free CVE reference
 * @param ref CVE reference
 * @relates cve_reference
 */
void cve_reference_free(struct cve_reference * ref);

/**
 * Free CVE entry
 * @param entry CVE entry
 * @relates cve_entry
 */
void cwe_entry_free(struct cwe_entry * entry);

/**
 * Free CVE configuration
 * @param conf CVE vulnerability configuration
 * @relates cve_configuration
 */
void cve_configuration_free(struct cve_configuration * conf);

/*@}*/

/**
 * @name Iterator functions
 * Functions to iterate throught lists.
 * @{
 * */

/** 
 * @struct cve_entry_iterator
 * Iterator over CVE entries.
 * @see oscap_iterator
 * @relates cve_entry
 */
struct cve_entry_iterator;
/** 
 * @relates cve_entry_iterator 
 */
struct cve_entry* cve_entry_iterator_next(struct cve_entry_iterator* it);
/** 
 * @relates cve_entry_iterator 
 */
bool cve_entry_iterator_has_more(struct cve_entry_iterator* it);
/** 
 * @relates cve_entry_iterator 
 */
void cve_entry_iterator_free(struct cve_entry_iterator* it);

/** @struct cve_summary_iterator
 * Iterator over CVE summaries.
 * @see oscap_iterator
 * @relates cve_summary
 */
struct cve_summary_iterator;
/** 
 * @relates cve_summary_iterator
 */
struct cve_summary* cve_summary_iterator_next(struct cve_summary_iterator* it);
/** 
 * @relates cve_summary_iterator
 */ 
bool cve_summary_iterator_has_more(struct cve_summary_iterator* it);
/**
 * @relates cve_summary_iterator 
 */
void cve_summary_iterator_free(struct cve_summary_iterator* it);

/** @struct cve_product_iterator
 * Iterator over CVE products.
 * @see oscap_iterator
 * @relates cve_product
 */
struct cve_product_iterator;
/** 
 * @relates cve_product_iterator 
 */
struct cve_product* cve_product_iterator_next(struct cve_product_iterator* it);
/** 
 * @relates cve_product_iterator 
 */
bool cve_product_iterator_has_more(struct cve_product_iterator* it);
/** 
 * @relates cve_product_iterator 
 */
void cve_product_iterator_free(struct cve_product_iterator* it);

/** @struct cve_configuration_iterator
 * Iterator over CVE vulnerable configurations.
 * @see oscap_iterator
 * @relates cve_configuration
 */
struct cve_configuration_iterator;
/** 
 * @relates cve_configuration_iterator 
 */
struct cve_configuration* cve_configuration_iterator_next(struct cve_configuration_iterator* it);
/** 
 * @relates cve_configuration_iterator 
 */
bool cve_configuration_iterator_has_more(struct cve_configuration_iterator* it);
/** 
 * @relates cve_configuration_iterator
 */
void cve_configuration_iterator_free(struct cve_configuration_iterator* it);

/** @struct cve_reference_iterator
 * Iterator over CVE references.
 * @see oscap_iterator
 * @relates cve_reference
 */
struct cve_reference_iterator;
/**
 * @relates cve_reference_iterator
 */
struct cve_reference* cve_reference_iterator_next(struct cve_reference_iterator* it);
/**
 * @relates cve_reference_iterator
 */
bool cve_reference_iterator_has_more(struct cve_reference_iterator* it);
/**
 * @relates cve_reference_iterator
 */
void cve_reference_iterator_free(struct cve_reference_iterator* it);

/*@}*/

/**
 * @name Other functions
 * @{
 * */

/**
 * Export CVE model to XML file
 * @relates cve_model
 * @param cve CVE model
 * @param target OSCAP export target
 */
void cve_model_export(struct cve_model * cve, const struct oscap_export_target * target);

/**
 * Parses the specified XML file and creates a list of CVE data structures.
 * The returned list can be freed with cveDelAll().
 * @relates cve_model
 * @param source oscap_import_source defining importing file
 * @return non-negative value indicates the number of CVEs in the list, negative value indicates an error
 */
struct cve_model * cve_model_import(const struct oscap_import_source * source);

/*@}*/

/*@}*/

#endif				/* _CVE_H_ */

