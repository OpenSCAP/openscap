/*! 
 *  \addtogroup CVE
 *  \{
 *
 *  \file cve.h
 *  \brief Interface to Common Vulnerability and Exposure dictionary
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
 * @struct cve_reference
 * Structure holding CVE Reference data
 */
struct cve_model;
struct cve_entry;
struct cve_summary;
struct cve_product;
struct cve_configuration;
struct cwe_entry;
struct cve_reference;

/** @struct cve_entry_iterator
 * Iterator over CVE entries.
 * @see oscap_iterator
 */
struct cve_entry_iterator;
/* @relates cve_entry_iterator */
struct cve_entry* cve_entry_iterator_next(struct cve_entry_iterator* it);
/* @relates cve_entry_iterator */
bool cve_entry_iterator_has_more(struct cve_entry_iterator* it);
/* @relates cve_entry_iterator */
void cve_entry_iterator_free(struct cve_entry_iterator* it);

/** @struct cve_summary_iterator
 * Iterator over CVE entries.
 * @see oscap_iterator
 */
struct cve_summary_iterator;
/* @relates cve_summary_iterator */
struct cve_summary* cve_summary_iterator_next(struct cve_summary_iterator* it);
/* @relates cve_summary_iterator */
bool cve_summary_iterator_has_more(struct cve_summary_iterator* it);
/* @relates cve_summary_iterator */
void cve_summary_iterator_free(struct cve_summary_iterator* it);

/** @struct cve_product_iterator
 * Iterator over CVE entries.
 * @see oscap_iterator
 */
struct cve_product_iterator;
/* @relates cve_product_iterator */
struct cve_product* cve_product_iterator_next(struct cve_product_iterator* it);
/* @relates cve_product_iterator */
bool cve_product_iterator_has_more(struct cve_product_iterator* it);
/* @relates cve_product_iterator */
void cve_product_iterator_free(struct cve_product_iterator* it);

/** @struct cve_configuration_iterator
 * Iterator over CVE entries.
 * @see oscap_iterator
 */
struct cve_configuration_iterator;
/* @relates cve_configuration_iterator */
struct cve_configuration* cve_configuration_iterator_next(struct cve_configuration_iterator* it);
/* @relates cve_configuration_iterator */
bool cve_configuration_iterator_has_more(struct cve_configuration_iterator* it);
/* @relates cve_configuration_iterator */
void cve_configuration_iterator_free(struct cve_configuration_iterator* it);

/** @struct cve_reference_iterator
 * Iterator over CVE references.
 * @see oscap_iterator
 */
struct cve_reference_iterator;
/// @relates cve_reference_iterator
struct cve_reference* cve_reference_iterator_next(struct cve_reference_iterator* it);
/// @relates cve_reference_iterator
bool cve_reference_iterator_has_more(struct cve_reference_iterator* it);
/// @relates cve_reference_iterator
void cve_reference_iterator_free(struct cve_reference_iterator* it);

/*
 * Parses the specified XML file and creates a list of CVE data structures.
 * The returned list can be freed with cveDelAll().
 *
 * @param source oscap_import_source defining importing file
 * @return non-negative value indicates the number of CVEs in the list, negative value indicates an error
 */
struct cve_model * cve_model_import(const struct oscap_import_source * source);

/**
 * Get en iterator to CVE entries.
 * @relates cve
 */
struct cve_entry_iterator* cve_model_get_entries(const struct cve_model* cve_model);

/**
 * Get CVE entry ID.
 * @relates cve_entry
 */
const char* cve_entry_get_id(const struct cve_entry* item);

/**
 * Get CVE entry CWE.
 * @relates cve_entry
 */
const char* cve_entry_get_cwe(const struct cve_entry* item);

/**
 * Get CVE entry summary.
 * @relates cve_entry
 */
struct cve_summary_iterator* cve_entry_get_summaries(const struct cve_entry* item);

/**
 * Get an iterator to CVE entry's references.
 * @relates cve_entry
 */
struct cve_reference_iterator* cve_entry_get_references(const struct cve_entry* item);


/**
 * Get CVE reference summary.
 * @relates cve_reference
 */
const char* cve_reference_get_value(const struct cve_reference* ref);

/**
 * Get CVE reference summary.
 * @relates cve_reference
 */
const char* cve_reference_get_href(const struct cve_reference* ref);

/**
 * Get CVE reference summary.
 * @relates cve_reference
 */
const char* cve_reference_get_type(const struct cve_reference* ref);

/**
 * Get CVE reference summary.
 * @relates cve_reference
 */
const char* cve_reference_get_source(const struct cve_reference * ref);


const char* cve_summary_get_summary(const struct cve_summary * summary);

const char* cve_product_get_value(const struct cve_product * product);
const char* cwe_entry_get_value(const struct cwe_entry * entry);
const char* cve_configuration_get_id(const struct cve_configuration * conf);
const char* cve_entry_get_published(const struct cve_entry * entry);
const char* cve_entry_get_modified(const struct cve_entry * entry);
const char* cve_entry_get_sec_protection(const struct cve_entry * entry);

struct cve_product_iterator * cve_entry_get_products(const struct cve_entry * entry);
struct cve_configuration_iterator * cve_entry_get_configurations(const struct cve_entry * entry);
const struct cpe_testexpr * cve_configuration_get_expr(const struct cve_configuration * conf);

void cve_model_export(struct cve_model * cve, const struct oscap_export_target * target);

struct cve_model * cve_model_import(const struct oscap_import_source * source);

void cve_model_free(struct cve_model * cve_model);

void cve_entry_free(struct cve_entry * entry);

void cve_summary_free(struct cve_summary * summary);

void cve_product_free(struct cve_product * product);

void cve_reference_free(struct cve_reference * ref);

void cwe_entry_free(struct cwe_entry * entry);

void cve_configuration_free(struct cve_configuration * conf);

struct cve_entry * cve_entry_new(void);
struct cve_configuration * cve_configuration_new(void);
struct cwe_entry * cwe_entry_new(void);
struct cve_product * cve_product_new(void);
struct cve_summary * cve_summary_new(void);
struct cve_reference * cve_reference_new(void);
struct cve_model * cve_model_new(void);

bool cve_model_add_entry(struct cve_model *motel, struct cve_entry *new_entry);

bool cve_reference_set_value(struct cve_reference *reference, const char *new_value);
bool cve_reference_set_href(struct cve_reference *reference, const char *new_href);
bool cve_reference_set_type(struct cve_reference *reference, const char *new_type);
bool cve_reference_set_source(struct cve_reference *reference, const char *new_source);

bool cve_summary_set_summary(struct cve_summary *summary, const char *new_summary);

bool cve_product_set_value(struct cve_product *product, const char *new_value);

bool cve_entry_set_id(struct cve_entry *entry, const char *new_id);
bool cve_entry_set_published(struct cve_entry *entry, const char *new_published);
bool cve_entry_set_modified(struct cve_entry *entry, const char *new_modified);
bool cve_entry_set_sec_protection(struct cve_entry *entry, const char *new_protection);
bool cve_entry_set_cwe(struct cve_entry *entry, const char *cwe);

bool cwe_entry_set_value(struct cwe_entry *entry, const char *new_value);

bool cve_configuration_set_id(struct cve_configuration * conf, const char *new_id);

/*
 * bool cve__set_(struct cve_ *, const char *new_);
*/
#endif				/* _CVE_H_ */

