/*! \file cve.h
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

/**
 * Structure holding CVE Reference data
 */
typedef struct cve_reference {
	char *summary;		///< summary
	char *href;		///< href
	char *refType;		///< reference type
	char *source;		///< source

	struct cve_reference *next;
} cve_reference_t;

/**
 * Structure holding Common Vulnerabilities and Exposures data
 */
typedef struct cve_info {
	char *id;		///< id 
	char *pub;		///< published datetime
	char *mod;		///< last modified datetime
	char *cwe;		///< cwe
	char *summary;		///< summary

	char *score;		///< score
	char *vector;		///< access vector
	char *complexity;	///< access complexity
	char *authentication;	///< authentication
	char *confidentiality;	///< confidentiality impact
	char *integrity;	///< integrity impact
	char *availability;	///< availability impact
	char *source;		///< source
	char *generated;	///< generated on datetime

	cve_reference_t *refs;	///< cve references

	struct cve_info *next;	///< next cve info
} cve_info_t;

/**
 * Create new CVE Reference structure.
 *
 * @note The returned memory must be freed by the caller.
 * @return new zeroed CVE Reference structure
 * @retval NULL on failure
 */
cve_reference_t *cveReferenceNew();

/**
 * Free the CVE Reference structure and its data.
 *
 * @param reference CVE Reference to be freed
 */
void cveReferenceDel(cve_reference_t * reference);

/**
 * Free the whole CVE Reference list.
 *
 * @param ref root of the CVE Reference list to be freed
 */
void cveReferenceDelAll(cve_reference_t * ref);

/**
 * Create new CVE structure.
 *
 * @note The returned memory must be freed by the caller.
 * @return new zeroed CVE structure
 * @retval NULL on failure
 */
cve_info_t *cveNew();

/**
 * Free the CVE structure and its data.
 *
 * @param cve CVE to be freed
 */
void cveDel(cve_info_t * cve);

/**
 * Free the whole CVE list.
 *
 * @param cve root of the CVE list to be freed
 */
void cveDelAll(cve_info_t * cve);

/**
 * Parses the specified XML file and creates a list of CVE data structures.
 * The returned list can be freed with cveDelAll().
 *
 * @param xmlfile path to the file to be parsed
 * @param outCveList address of the pointer to which the root element of the list is to be stored
 * @return non-negative value indicates the number of CVEs in the list, negative value indicates an error
 */
int cveParse(char *xmlfile, cve_info_t ** outCveList);

#endif				/* _CVE_H_ */
