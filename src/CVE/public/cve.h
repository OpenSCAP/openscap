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

/** @struct cve__iterator
 * Iterator over CVE entries.
 * @see oscap_iterator
 */
struct cwe_entry_iterator;
/* @relates cwe_entry_iterator */
struct cwe_entry* cwe_entry_iterator_next(struct cwe_entry_iterator* it);
/* @relates cwe_entry_iterator */
bool cwe_entry_iterator_has_more(struct cwe_entry_iterator* it);
/* @relates cwe_entry_iterator */
void cwe_entry_iterator_free(struct cwe_entry_iterator* it);

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
struct cve_model * ce_model_import(const struct oscap_import_source * source);

/**
 * Create a new CVE catalogue from a XML file.
 * @relates cve
 * @param fname XML file name
 */
struct cve* cve_new(const char* fname);

/**
 * Delete CVE catalogue.
 * @relates cve
 */
void cve_free(struct cve* cve);

/**
 * Get en iterator to CVE entries.
 * @relates cve
 */
struct cve_entry_iterator* cve_get_entries(const struct cve* cve);

/**
 * Get CVE entry by its ID.
 * @relates cve
 */
struct cve_entry* cve_get_entry(const struct cve* cve, const char* id);


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
 * Get CVE entry generation datetime.
 * @relates cve_entry
 */
const char* cve_entry_get_generated(const struct cve_entry* item);

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

void cve_model_export_xml(struct cve_model * cve, const struct oscap_export_target * target);

struct cve_model * cve_model_parse_xml(const struct oscap_import_source * source);

struct cve_model * cve_model_import(const struct oscap_import_source * source);

void cve_model_free(struct cve_model * cve_model);

void cve_entry_free(struct cve_entry * entry);

void cve_summary_free(struct cve_summary * summary);

void cve_product_free(struct cve_product * product);

void cve_reference_free(struct cve_reference * ref);

void cwe_entry_free(struct cwe_entry * entry);

void cve_configuration_free(struct cve_configuration * conf);

struct cve_entry * cve_entry_new();
struct cve_configuration * cve_configuration_new();
struct cwe_entry * cwe_entry_new();
struct cve_product * cve_product_new();
struct cve_summary * cve_summary_new();
struct cve_reference * cve_reference_new();
struct cve_model * cve_model_new();

#endif				/* _CVE_H_ */

