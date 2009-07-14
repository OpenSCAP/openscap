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
 *      Tomas Heinrich <theinric@redhat.com>
 */

#ifndef _CVE_H_
#define _CVE_H_

#include <stdbool.h>

/** @struct cve
 * Structure holding all the cve_info entries.
 */
struct cve;

/** @struct cve_info
 * Structure holding Common Vulnerabilities and Exposures data
 */
struct cve_info;

/** @struct cve_reference
 * Structure holding CVE Reference data
 */
struct cve_reference;



/** @struct cve_info_iterator
 * Iterator over CVE entries.
 * @see oscap_iterator
 */
struct cve_info_iterator;
/// @relates cve_info_iterator
struct cve_info* cve_info_iterator_next(struct cve_info_iterator* it);
/// @relates cve_info_iterator
bool cve_info_iterator_has_more(struct cve_info_iterator* it);

/** @struct cve_reference_iterator
 * Iterator over CVE references.
 * @see oscap_iterator
 */
struct cve_reference_iterator;
/// @relates cve_reference_iterator
struct cve_reference* cve_reference_iterator_next(struct cve_reference_iterator* it);
/// @relates cve_reference_iterator
bool cve_reference_iterator_has_more(struct cve_reference_iterator* it);



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
void cve_delete(struct cve* cve);

/**
 * Get en iterator to CVE entries.
 * @relates cve
 */
struct cve_info_iterator* cve_entries(const struct cve* cve);

/**
 * Get CVE entry by its ID.
 * @relates cve
 */
struct cve_info* cve_entry_by_id(const struct cve* cve, const char* id);


/**
 * Get CVE entry ID.
 * @relates cve_info
 */
const char* cve_info_id(const struct cve_info* info);

/**
 * Get CVE entry publication date.
 * @relates cve_info
 */
const char* cve_info_pub(const struct cve_info* info);

/**
 * Get CVE entry last modification date.
 * @relates cve_info
 */
const char* cve_info_mod(const struct cve_info* info);

/**
 * Get CVE entry CWE.
 * @relates cve_info
 */
const char* cve_info_cwe(const struct cve_info* info);

/**
 * Get CVE entry summary.
 * @relates cve_info
 */
const char* cve_info_summary(const struct cve_info* info);

/**
 * Get CVE entry score.
 * @relates cve_info
 */
const char* cve_info_score(const struct cve_info* info);

/**
 * Get CVE entry access vector.
 * @relates cve_info
 */
const char* cve_info_vector(const struct cve_info* info);

/**
 * Get CVE entry access complexity.
 * @relates cve_info
 */
const char* cve_info_complexity(const struct cve_info* info);

/**
 * Get CVE entry authentication.
 * @relates cve_info
 */
const char* cve_info_authentication(const struct cve_info* info);

/**
 * Get CVE entry confidentiality impact.
 * @relates cve_info
 */
const char* cve_info_confidentiality(const struct cve_info* info);

/**
 * Get CVE entry integrity impact.
 * @relates cve_info
 */
const char* cve_info_integrity(const struct cve_info* info);

/**
 * Get CVE entry availibility impact.
 * @relates cve_info
 */
const char* cve_info_availability(const struct cve_info* info);

/**
 * Get CVE entry source.
 * @relates cve_info
 */
const char* cve_info_source(const struct cve_info* info);

/**
 * Get CVE entry generation datetime.
 * @relates cve_info
 */
const char* cve_info_generated(const struct cve_info* info);

/**
 * Get an iterator to CVE entry's references.
 * @relates cve_info
 */
struct cve_reference_iterator* cve_info_references(const struct cve_info* info);


/**
 * Get CVE reference summary.
 * @relates cve_reference
 */
const char* cve_reference_summary(const struct cve_reference* ref);

/**
 * Get CVE reference summary.
 * @relates cve_reference
 */
const char* cve_reference_href(const struct cve_reference* ref);

/**
 * Get CVE reference summary.
 * @relates cve_reference
 */
const char* cve_reference_type(const struct cve_reference* ref);

/**
 * Get CVE reference summary.
 * @relates cve_reference
 */
const char* cve_reference_source(const struct cve_reference* ref);

#endif				/* _CVE_H_ */

