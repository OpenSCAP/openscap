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
struct cve_reference;
struct cve_model;
struct cve_entry;
struct cve_summary;
struct cve_product;
struct cvss_entry;
struct cwe_entry;
struct cve_configuration;


/** @struct cve_info_iterator
 * Iterator over CVE entries.
 * @see oscap_iterator
 */
struct cve_info_iterator;
/// @relates cve_info_iterator
struct cve_info* cve_info_iterator_next(struct cve_info_iterator* it);
/// @relates cve_info_iterator
bool cve_info_iterator_has_more(struct cve_info_iterator* it);
/// @relates cve_info_iterator
void cve_info_iterator_free(struct cve_info_iterator* it);

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
struct cve_info_iterator* cve_get_entries(const struct cve* cve);

/**
 * Get CVE entry by its ID.
 * @relates cve
 */
struct cve_info* cve_get_entry(const struct cve* cve, const char* id);


/**
 * Get CVE entry ID.
 * @relates cve_info
 */
const char* cve_info_get_id(const struct cve_info* info);

/**
 * Get CVE entry publication date.
 * @relates cve_info
 */
const char* cve_info_get_pub(const struct cve_info* info);

/**
 * Get CVE entry last modification date.
 * @relates cve_info
 */
const char* cve_info_get_mod(const struct cve_info* info);

/**
 * Get CVE entry CWE.
 * @relates cve_info
 */
const char* cve_info_get_cwe(const struct cve_info* info);

/**
 * Get CVE entry summary.
 * @relates cve_info
 */
const char* cve_info_get_summary(const struct cve_info* info);

/**
 * Get CVE entry score.
 * @relates cve_info
 */
const char* cve_info_get_score(const struct cve_info* info);

/**
 * Get CVE entry access vector.
 * @relates cve_info
 */
const char* cve_info_get_vector(const struct cve_info* info);

/**
 * Get CVE entry access complexity.
 * @relates cve_info
 */
const char* cve_info_get_complexity(const struct cve_info* info);

/**
 * Get CVE entry authentication.
 * @relates cve_info
 */
const char* cve_info_get_authentication(const struct cve_info* info);

/**
 * Get CVE entry confidentiality impact.
 * @relates cve_info
 */
const char* cve_info_get_confidentiality(const struct cve_info* info);

/**
 * Get CVE entry integrity impact.
 * @relates cve_info
 */
const char* cve_info_get_integrity(const struct cve_info* info);

/**
 * Get CVE entry availibility impact.
 * @relates cve_info
 */
const char* cve_info_get_availability(const struct cve_info* info);

/**
 * Get CVE entry source.
 * @relates cve_info
 */
const char* cve_info_get_source(const struct cve_info* info);

/**
 * Get CVE entry generation datetime.
 * @relates cve_info
 */
const char* cve_info_get_generated(const struct cve_info* info);

/**
 * Get an iterator to CVE entry's references.
 * @relates cve_info
 */
struct cve_reference_iterator* cve_info_get_references(const struct cve_info* info);


/**
 * Get CVE reference summary.
 * @relates cve_reference
 */
const char* cve_reference_get_summary(const struct cve_reference* ref);

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

void cvss_entry_free(struct cvss_entry * entry);

void cwe_entry_free(struct cwe_entry * entry);

void cve_configuration_free(struct cve_configuration * conf);

#endif				/* _CVE_H_ */

