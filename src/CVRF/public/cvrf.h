/**
 * @addtogroup CVRF
 * @{
 *
 * @file cvrf.h
 * @brief Interface to Common Vulnerability Reporting Framework
 *
 * Copyright 2017 Red Hat Inc., Durham, North Carolina.
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
 * 		Katarina Jankov <kj226@cornell.edu>
 *
 */
#ifndef _CVRF_H_
#define _CVRF_H_

#include <stdbool.h>
#include "oscap.h"
#include "oscap_source.h"
#include "cvss_score.h"


/************************************************************************************************
 * @struct cvrf_product_status
 * Structure holding CVRF Status element (within a ProductStatuses container)
 * Contains a list of ProductIDs and their status with respect to the parent Vulnerability
 *
 * PARENT NODE: ProductStatuses container (which is child of Vulnerability)
 * REQUIRED: Type attribute [max. 1], ProductID(s) [min. 1, max. unbounded]
 */
struct cvrf_product_status;

/**
 * New Status member of a ProductStatuses container within a Vulnerability element
 * @memberof cvrf_product_status
 * @return New CVRF ProductStatus
 */
struct cvrf_product_status *cvrf_product_status_new(void);

/**
 * Deallocates memory for a Status element of the ProductStatuses container
 * @memberof cvrf_product_status
 * @param status The CVRF Status element to be freed
 */
void cvrf_product_status_free(struct cvrf_product_status *status);

/**
 * @memberof cvrf_product_status
 * @param stat Original Status structure to be cloned
 * @return New cloned Status structure with same data as the original
 */
struct cvrf_product_status *cvrf_product_status_clone(const struct cvrf_product_status *stat);

/**
 * @memberof cvrf_product_status
 * @param stat Status structure with stringlist of ProductIDs
 * @return Iterator for the stringlist of ProductIDs
 */
struct oscap_string_iterator *cvrf_product_status_get_ids(struct cvrf_product_status *stat);


/************************************************************************************************
 * @struct cvrf_threat
 * Gives context about the type of Vulnerability and how much threat it poses
 * Contained within a list representing a Threats container in Vulnerability structure
 *
 * PARENT NODE: Threats container (which is child of Vulnerability)
 * REQUIRED: Type attribute [max 1], Description element [max. 1]
 * OPTIONAL: ProductID(s) [min. 0, max. unbounded], GroupID(s) [min. 0, max.unbounded]
 */
struct cvrf_threat;

/**
 * New CVRF Threat element
 * @memberof cvrf_threat
 * @return New CVRF Threat structure
 */
struct cvrf_threat *cvrf_threat_new(void);

/**
 * Deallocates memory for a Threat element of the Threats container
 * @memberof cvrf_threat
 * @param threat The CVRF Threat structure to be freed
 */
void cvrf_threat_free(struct cvrf_threat *threat);

/**
 * @memberof cvrf_threat
 * @param threat Original Threat structure to be cloned
 * @return New cloned Threat structure with same data as the original
 */
struct cvrf_threat *cvrf_threat_clone(const struct cvrf_threat *threat);

/**
 * @memberof cvrf_threat
 * @param threat CVRF Threat structure
 * Datetime that the Threat was identified and documented
 * @return contents of Date attribute within a Threat element
 */
const char *cvrf_threat_get_date(const struct cvrf_threat *threat);

/**
 * @memberof cvrf_threat
 * @param threat CVRF Threat structure
 * Description contains a human-readable explanation of the threat
 * @return contents of Description element within a Threat element
 */
const char *cvrf_threat_get_description(const struct cvrf_threat *threat);

/**
 * @memberof cvrf_threat
 * @param threat Threat structure with stringlist of ProductIDs
 * Stringlist can have from zero to unbounded number of ProductIDs
 * ProductIDs reference the ProductID of a FullProductName element in the document
 * @return Iterator for the stringlist of ProductIDs
 */
struct oscap_string_iterator *cvrf_threat_get_product_ids(struct cvrf_threat *threat);

/**
 * @memberof cvrf_threat
 * @param threat Threat structure with stringlist of GroupIDs
 * Stringlist can have from zero to unbounded number of GroupIDs
 * GroupIDs reference the GroupID of a Group element in the document
 * @return Iterator for the stringlist of GroupIDs
 */
struct oscap_string_iterator *cvrf_threat_get_group_ids(struct cvrf_threat *threat);

/**
 * @memberof cvrf_threat
 * @param threat CVRF Threat structure
 * @param date Datetime that the Threat was identified and documented
 * @return true on success
 */
bool cvrf_threat_set_date(struct cvrf_threat *threat, const char *date);

/**
 * @memberof cvrf_threat
 * @param threat CVRF Threat structure
 * @param description A human-readable explanation of the threat
 * @return true on success
 */
bool cvrf_threat_set_description(struct cvrf_threat *threat, const char *description);


/************************************************************************************************
 * @struct cvrf_remediation
 * Structure holding info for handling or fixing a particular Vulnerability
 *
 * PARENT NODE: Remediations container (which is child of Vulnerability)
 * REQUIRED: Type attribute [max: 1], Description element [max: 1]
 * OPTIONAL: ProductID(s) [min: 0, max: unbounded], GroupID(s) [min: 0, max: unbounded],
 *		Entitlement element [min: 0, max: 1], URL element [min: 0, max: 1]
 */
struct cvrf_remediation;

/**
 * New CVRF Remediation
 * @memberof cvrf_remediation
 * @return New CVRF Remediation
 */
struct cvrf_remediation *cvrf_remediation_new(void);

/**
 * Deallocates memory for a Remediation element of the Remediations container
 * @memberof cvrf_remediation
 * @param remed The CVRF Remediation structure to be freed
 */
void cvrf_remediation_free(struct cvrf_remediation *remed);

/**
 * @memberof cvrf_remediation
 * @param remed Original Remediation structure to be cloned
 * @return New cloned Remediation structure with same data as the original
 */
struct cvrf_remediation *cvrf_remediation_clone(const struct cvrf_remediation *remed);

/**
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure
 * Date that the Remediation was created
 * @return contents of Date attribute of the Remediation attribute
 */
const char *cvrf_remediation_get_date(const struct cvrf_remediation *remed);

/**
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure
 * Description contains a human-readable explanation of Remediation process
 * @return contents of Description element within a Remediation element
 */
const char *cvrf_remediation_get_description(const struct cvrf_remediation *remed);

/**
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure
 * Optional URL link to a Remediation for a particular Vulnerability
 * @return contents of URL element within a Remediation element
 */
const char *cvrf_remediation_get_url(const struct cvrf_remediation *remed);

/**
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure
 * Contains vendor-defined constraints for products used to resolve the Vulnerability
 * @return contents of Entitlement element within a Remediation element
 */
const char *cvrf_remediation_get_entitlement(const struct cvrf_remediation *remed);

/**
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure
 * Stringlist can have from zero to unbounded number of ProductIDs
 * ProductIDs reference the ProductID of a FullProductName element in the document
 * @return Iterator for the stringlist of ProductIDs
 */
struct oscap_string_iterator *cvrf_remediation_get_product_ids(struct cvrf_remediation *remed);

/**
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure
 * Stringlist can have from zero to unbounded number of GroupIDs
 * GroupIDs reference the GroupID of a Group element in the document
 * @return Iterator for the stringlist of GroupIDs
 */
struct oscap_string_iterator *cvrf_remediation_get_group_ids(struct cvrf_remediation *remed);

/**
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure
 * @param date Date that the Remediation was created
 * @return true on success
 */
bool cvrf_remediation_set_date(struct cvrf_remediation *remed, const char *date);

/**
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure
 * @param description a human-readable explanation of Remediation process
 * @return true on success
 */
bool cvrf_remediation_set_description(struct cvrf_remediation *remed, const char *description);

/**
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure
 * @param url Optional URL link to a Remediation for a particular Vulnerability
 * @return true on success
 */
bool cvrf_remediation_set_url(struct cvrf_remediation *remed, const char *url);

/**
 * @memberof cvrf_remediation
 * @param remed CVRF Remediation structure
 * @param entitlement vendor-defined constraints for products used to resolve the Vulnerability
 * @return true on success
 */
bool cvrf_remediation_set_entitlement(struct cvrf_remediation *remed, const char *entitlement);


/************************************************************************************************
 * @struct cvrf_score_set
 * Contains CVSS metrics for a given Vulnerability. No ProductID indicates that it pertains to all
 * Products in a given CVRF file
 *
 * PARENT NODE: CVSSScoreSets container (which is child of Vulnerability)
 * REQUIRED: BaseScore element [max: 1]
 * OPTIONAL: TemporalScore element [min: 0, max:1], EnvironmentalScore element [min: 0, max: 1],
 *		 ProductID(s) [min: 0, max: unbounded], Vector element [min: 0, max: 1]
 */
struct cvrf_score_set;

/**
 * New ScoreSet member of a CVSSScoreSets container within a Vulnerability element
 * @memberof cvrf_score_set
 * @return New CVRF ScoreSet structure
 */
struct cvrf_score_set *cvrf_score_set_new(void);

/**
 * Deallocates memory for a ScoreSet element of the CVSSScoreSets container
 * @memberof cvrf_score_set
 * @param score_set The CVRF ScoreSet structure to be freed
 */
void cvrf_score_set_free(struct cvrf_score_set *score_set);

/**
 * @memberof cvrf_score_set
 * @param score_set Original ScoreSet structure to be cloned
 * @return New cloned ScoreSet structure with same data as the original
 */
struct cvrf_score_set *cvrf_score_set_clone(const struct cvrf_score_set *score_set);

/**
 * @memberof cvrf_score_set
 * @param score_set ScoreSet structure
 * Contains official CVSS notation of values used to compute all the scores
 * @return contents of Vector element within a ScoreSet element
 */
const char *cvrf_score_set_get_vector(const struct cvrf_score_set *score_set);

/**
 * @memberof cvrf_score_set
 * @param score_set ScoreSet structure with stringlist of ProductIDs
 * Stringlist can have from zero to unbounded number of ProductIDs
 * ProductIDs reference the ProductID of a FullProductName element in the document
 * @return Iterator for the stringlist of ProductIDs
 */
struct oscap_string_iterator *cvrf_score_set_get_product_ids(struct cvrf_score_set *score_set);

/**
 * @memberof cvrf_score_set
 * @param score_set ScoreSet structure
 * @return CVSS impact structure generated by parsing XML ScoreSet element in CVRF document
 */
struct cvss_impact *cvrf_score_set_get_impact(const struct cvrf_score_set *score_set);

/**
 * @memberof cvrf_score_set
 * @param score_set ScoreSet structure
 * Range: 0.0 - 10.0
 * @return string representation of BaseScore element within a ScoreSet element
 */
char *cvrf_score_set_get_base_score(const struct cvrf_score_set *score_set);

/**
 * @memberof cvrf_score_set
 * @param score_set ScoreSet structure
 * Range: 0.0 - 10.0
 * @return string representation of EnvironmentalScore element within a ScoreSet element
 */
char *cvrf_score_set_get_environmental_score(const struct cvrf_score_set *score_set);

/**
 * @memberof cvrf_score_set
 * @param score_set ScoreSet structure
 * Range: 0.0 - 10.0
 * @return string representation of TemporalScore element within a ScoreSet element
 */
char *cvrf_score_set_get_temporal_score(const struct cvrf_score_set *score_set);

/**
 * @memberof cvrf_score_set
 * @param score_set ScoreSet structure
 * @param vector Official CVSS notation of values used to compute all the scores
 * @return true on success
 */
bool cvrf_score_set_set_vector(struct cvrf_score_set *score_set, const char *vector);

/**
 * @memberof cvrf_score_set
 * @param score_set ScoreSet structure
 * @param impact CVSS impact structure with scores for parent Vulnerability
 * @return true on success
 */
bool cvrf_score_set_set_impact(struct cvrf_score_set *score_set, struct cvss_impact *impact);

/**
 * @memberof cvrf_score_set
 * @param score_set ScoreSet structure
 * @param category CVSS_BASE, CVSS_TEMPORAL, or CVSS_ENVIRONMENTAL
 * @param score String representation of the CVSS score
 * @return true on success
 */
bool cvrf_score_set_add_metric(struct cvrf_score_set *score_set, enum cvss_category category, const char *score);


/************************************************************************************************
 * @struct cvrf_involvement
 * Gives information about the Document Publisher's level of involvement
 * The 'Party' attribute gives the type of publisher (identical to DocumentPublisher Type)
 *
 * PARENT NODE: Involvements container
 * REQUIRED: Status attribute [max: 1], Party attribute [max: 1]
 * OPTIONAL: Description element [min: 0, max: 1]
 */
struct cvrf_involvement;

/**
 * New CVRF Involvement
 * @memberof cvrf_involvement
 * @return New CVRF Involvement
 */
struct cvrf_involvement *cvrf_involvement_new(void);

/**
 * Deallocates memory for an Involvement element
 * @memberof cvrf_involvement
 * @param involve The CVRF Involvement structure to be freed
 */
void cvrf_involvement_free(struct cvrf_involvement *involve);

/**
 * @memberof cvrf_involvement
 * @param involve Original Involvement structure to be cloned
 * @return New cloned Involvement structure with same data as the original
 */
struct cvrf_involvement *cvrf_involvement_clone(const struct cvrf_involvement *involve);

/**
 * @memberof cvrf_involvement
 * @param involve CVRF Involvement structure
 * Gives context about the involvement or about the Party's contribution
 * @return contents of Description element within Involvement element
 */
const char *cvrf_involvement_get_description(const struct cvrf_involvement *involve);

/**
 * @memberof cvrf_involvement
 * @param involve CVRF Involvement structure
 * @param description Gives context about the involvement or about the Party's contribution
 * @return true on success
 */
bool cvrf_involvement_set_description(struct cvrf_involvement *involve, const char *description);

/************************************************************************************************
 * @struct cvrf_vulnerability_cwe
 *
 * PARENT NODE: Vulnerability
 * REQUIRED: ID attribute [max: 1], CWE string content [max: 1]
 */
struct cvrf_vulnerability_cwe;

/**
 * New CVRF CWE structure
 * @memberof cvrf_vulnerability_cwe
 * @return New CWE structure
 */
struct cvrf_vulnerability_cwe *cvrf_vulnerability_cwe_new(void);

/**
 * Deallocates memory for a CWE element
 * @memberof cvrf_vulnerability_cwe
 * @param cwe The CWE structure to be freed
 */
void cvrf_vulnerability_cwe_free(struct cvrf_vulnerability_cwe *cwe);

/**
 * @memberof cvrf_vulnerability_cwe
 * @param cwe Original CWE structure to be cloned
 * @return New cloned CWE structure with same data as the original
 */
struct cvrf_vulnerability_cwe *cvrf_vulnerability_cwe_clone(const struct cvrf_vulnerability_cwe *cwe);

/**
 * @memberof cvrf_vulnerability_cwe
 * @param vuln_cwe CWE structure
 *
 * @return contents of a the CWE element
 */
const char *cvrf_vulnerability_cwe_get_cwe(const struct cvrf_vulnerability_cwe *vuln_cwe);

/**
 * @memberof cvrf_vulnerability_cwe
 * @param vuln_cwe CWE structure
 *
 * @return ID attribute of a CWE element
 */
const char *cvrf_vulnerability_cwe_get_id(const struct cvrf_vulnerability_cwe *vuln_cwe);


/**
 * @memberof cvrf_vulnerability_cwe
 * @param vuln_cwe CWE structure
 * @param cwe CWE name and content of the CWE element
 * @return true on success
 */
bool cvrf_vulnerability_cwe_set_cwe(struct cvrf_vulnerability_cwe *vuln_cwe, const char *cwe);

/**
 * @memberof cvrf_vulnerability_cwe
 * @param vuln_cwe CWE structure
 * @param id Unique identifier for a CWE
 * @return true on success
 */
bool cvrf_vulnerability_cwe_set_id(struct cvrf_vulnerability_cwe *vuln_cwe, const char *id);

/************************************************************************************************
 * @struct cvrf_vulnerability
 * Provides information about Vulnerabilities for products and packages referenced in
 * ProductTree branches and relationships
 *
 * PARENT NODE: cvrfdoc
 * REQUIRED: Ordinal attribute [max: 1]
 * OPTIONAL: Title element [min: 0, max:1], ID element [min: 0, max: 1], DiscoveryDate element [min: 0, max: 1],
 *		ReleaseDate element [min: 0, max: 1], Description element [min: 0, max: 1], CVE element [min: 0, max: 1],
 *		CWE ID(s) [min: 0, max: unbounded]
 *
 * CONTAINERS: Notes, Involvements, ProductStatuses, Threats, CVSSScoreSets, Remediations,
 * 		References, Acknowledgments [min: 0, max: 1 for all containers]
 */
struct cvrf_vulnerability;

/**
 * New CVRF Vulnerability
 * @memberof cvrf_vulnerability
 * @return New CVRF Vulnerability
 */
struct cvrf_vulnerability *cvrf_vulnerability_new(void);

/**
 * Deallocates memory for a Vulnerability element
 * @memberof cvrf_vulnerability
 * @param vulnerability The CVRF Vulnerability structure to be freed
 */
void cvrf_vulnerability_free(struct cvrf_vulnerability *vulnerability);

/**
 * @memberof cvrf_vulnerability
 * @param vuln Original Vulnerability structure to be cloned
 * @return New cloned Vulnerability structure with same data as the original
 */
struct cvrf_vulnerability *cvrf_vulnerability_clone(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure to be filtered
 * @param prod ProductID of the CPE; this is the prefix of package ProductIDs in Statuses
 * @return 0 on success, -1 on failure
 */
int cvrf_vulnerability_filter_by_product(struct cvrf_vulnerability *vuln, const char *prod);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * Index and ordering of a Vulnerability element within a document
 * @return Ordinal attribute of a Vulnerability element
 */
int cvrf_vulnerability_get_ordinal(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * Canonical name of Vulnerability; should match nomenclature of references elsewhere
 * @return contents of a Title element within a Vulnerability element
 */
const char *cvrf_vulnerability_get_title(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * Unique ID of the Vulnerability given by the document producer
 * @return contents of an ID element within a Vulnerability element
 */
const char *cvrf_vulnerability_get_system_id(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * Gives name or numbering system of the Vulnerability ID
 * @return contents of SystemName attribute within an ID element
 */
const char *cvrf_vulnerability_get_system_name(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * DateTime value of the original discovery time for this Vulnerability
 * @return contents of DiscoveryDate element within a Vulnerability element
 */
const char *cvrf_vulnerability_get_discovery_date(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * DateTime value of the original public release time for this Vulnerability
 * @return contents of ReleaseDate element within a Vulnerability element
 */
const char *cvrf_vulnerability_get_release_date(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * MITRE standard CVE ID used to track Vulnerabilities
 * @return contents of CVE element within a Vulnerability element
 */
const char *cvrf_vulnerability_get_cve_id(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param ordinal Index of a Vulnerability element within a document
 * @return true on success
 */
bool cvrf_vulnerability_set_ordinal(struct cvrf_vulnerability *vuln, int ordinal);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param vulnerability_title Canonical name of Vulnerability
 * @return true on success
 */
bool cvrf_vulnerability_set_title(struct cvrf_vulnerability *vuln, const char *vulnerability_title);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param id Unique ID of the Vulnerability given by the document producer
 * @return true on success
 */
bool cvrf_vulnerability_set_system_id(struct cvrf_vulnerability *vuln, const char *id);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param sys_name Name or numbering system of the Vulnerability ID
 * @return true on success
 */
bool cvrf_vulnerability_set_system_name(struct cvrf_vulnerability *vuln, const char *sys_name);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param Original discovery time for this Vulnerability
 * @return true on success
 */
bool cvrf_vulnerability_set_discovery_date(struct cvrf_vulnerability *vuln, const char *discovery_date);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param release_date Original public release time for this Vulnerability
 * @return true on success
 */
bool cvrf_vulnerability_set_release_date(struct cvrf_vulnerability *vuln, const char *release_date);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param cve_id MITRE standard CVE ID used to track Vulnerabilities
 * @return true on success
 */
bool cvrf_vulnerability_set_cve_id(struct cvrf_vulnerability *vuln, const char *cve_id);


/*-------------------------------------------------------------------------*\
|				Iterators of child elements of Vulnerability				|
\*-------------------------------------------------------------------------*/

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @return Iterator for the list of Reference elements in the References container
 */
struct oscap_iterator *cvrf_vulnerability_get_references(struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @return Iterator for the list of Acknowledgment elements in the Acknowledgments container
 */
struct oscap_iterator *cvrf_vulnerability_get_acknowledgments(struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @return Iterator for the list of Note elements in the Notes container
 */
struct oscap_iterator *cvrf_vulnerability_get_notes(struct cvrf_vulnerability *vuln);

/*******************************************
 * @struct cvrf_vulnerability_cwe_iterator
 * Iterator representing all CWE elements in the Vulnerability
 * Contained as a list within the CVRF Vulnerability structure
 */
struct cvrf_vulnerability_cwe_iterator;

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param vulnerability_cwe CWE structure to be added to the Vulnerability
 * @return true on success
 */
bool cvrf_vulnerability_add_vulnerability_cwe(struct cvrf_vulnerability *vuln, struct cvrf_vulnerability_cwe *vulnerability_cwe);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @return Iterator representing list of all CWEs in the Vulnerability
 */
struct cvrf_vulnerability_cwe_iterator *cvrf_vulnerability_get_vulnerability_cwes(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability_cwe_iterator
 * @param it CWE iterator structure
 * @return Next CWE in list of all CWEs in the Vulnerability
 */
struct cvrf_vulnerability_cwe *cvrf_vulnerability_cwe_iterator_next(struct cvrf_vulnerability_cwe_iterator *it);

/**
 * @memberof cvrf_vulnerability_cwe_iterator
 * @param it CWE iterator structure
 * @return true if the iterator has another CWE element left
 */
bool cvrf_vulnerability_cwe_iterator_has_more(struct cvrf_vulnerability_cwe_iterator *it);

/**
 * Deallocate memory for the CWE Iterator structure
 * @memberof cvrf_vulnerability_cwe_iterator
 * @param it CWE iterator structure
 */
void cvrf_vulnerability_cwe_iterator_free(struct cvrf_vulnerability_cwe_iterator *it);

/**
 * Restart iterator at the first CWE in the Vulnerability
 * @memberof cvrf_vulnerability_cwe_iterator
 * @param it CWE iterator structure
 */
void cvrf_vulnerability_cwe_iterator_reset(struct cvrf_vulnerability_cwe_iterator *it);

/**
 * Detaches and frees the CWE iterator structure
 * @memberof cvrf_vulnerability_cwe_iterator
 * @param it CWE iterator structure
 */
void cvrf_vulnerability_cwe_iterator_remove(struct cvrf_vulnerability_cwe_iterator *it);

/*******************************************
 * @struct cvrf_involvement_iterator
 * Iterator representing all Involvement elements in the Involvements container
 * Contained as a list within the CVRF Vulnerability structure
 */
struct cvrf_involvement_iterator;

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param involvement CVRF Involvement structure to be added to the Vulnerability
 * @return true on success
 */
bool cvrf_vulnerability_add_involvement(struct cvrf_vulnerability *vuln, struct cvrf_involvement *involvement);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @return Iterator representing list of all Involvements in the Vulnerability
 */
struct cvrf_involvement_iterator *cvrf_vulnerability_get_involvements(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_involvement_iterator
 * @param it CVRF Involvement iterator structure
 * @return Next CVRF Involvement in list of all Involvements in the Vulnerability
 */
struct cvrf_involvement *cvrf_involvement_iterator_next(struct cvrf_involvement_iterator *it);

/**
 * @memberof cvrf_involvement_iterator
 * @param it CVRF Involvement iterator structure
 * @return true if the iterator has another Involvement element left
 */
bool cvrf_involvement_iterator_has_more(struct cvrf_involvement_iterator *it);

/**
 * Deallocate memory for the CVRF Involvement Iterator structure
 * @memberof cvrf_involvement_iterator
 * @param it CVRF Involvement iterator structure
 */
void cvrf_involvement_iterator_free(struct cvrf_involvement_iterator *it);

/**
 * Restart iterator at the first Involvement in the Vulnerability
 * @memberof cvrf_involvement_iterator
 * @param it CVRF Involvement iterator structure
 */
void cvrf_involvement_iterator_reset(struct cvrf_involvement_iterator *it);

/**
 * Detaches and frees the Involvement iterator structure
 * @memberof cvrf_involvement_iterator
 * @param it CVRF Involvement iterator structure
 */
void cvrf_involvement_iterator_remove(struct cvrf_involvement_iterator *it);

/*******************************************
 * @struct cvrf_score_set_iterator
 * Iterator representing all ScoreSet elements in the CVSSScoreSets container
 * Contained as a list within the CVRF Vulnerability structure
 */
struct cvrf_score_set_iterator;

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param vuln CVRF ScoreSet structure to be added to the Vulnerability
 * @return true on success
 */
bool cvrf_vulnerability_add_score_set(struct cvrf_vulnerability *vuln, struct cvrf_score_set *score_set);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @return Iterator representing list of all ScoreSets in the Vulnerability
 */
struct cvrf_score_set_iterator *cvrf_vulnerability_get_score_sets(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_score_set_iterator
 * @param it CVRF ScoreSet iterator structure
 * @return Next CVRF ScoreSet in list of all CVSSScoreSets in the Vulnerability
 */
struct cvrf_score_set *cvrf_score_set_iterator_next(struct cvrf_score_set_iterator *it);

/**
 * @memberof cvrf_score_set_iterator
 * @param it CVRF ScoreSet iterator structure
 * @return true if the iterator has another ScoreSet element left
 */
bool cvrf_score_set_iterator_has_more(struct cvrf_score_set_iterator *it);

/**
 * Deallocate memory for the CVRF ScoreSet Iterator structure
 * @memberof cvrf_score_set_iterator
 * @param it CVRF ScoreSet iterator structure
 */
void cvrf_score_set_iterator_free(struct cvrf_score_set_iterator *it);

/**
 * Restart iterator at the first ScoreSet in the Vulnerability
 * @memberof cvrf_score_set_iterator
 * @param it CVRF ScoreSet iterator structure
 */
void cvrf_score_set_iterator_reset(struct cvrf_score_set_iterator *it);

/**
 * Detaches and frees the ScoreSet iterator structure
 * @memberof cvrf_score_set_iterator
 * @param it CVRF ScoreSet iterator structure
 */
void cvrf_score_set_iterator_remove(struct cvrf_score_set_iterator *it);

/*******************************************
 * @struct cvrf_product_status_iterator
 * Iterator representing all Status elements in the ProductStatuses container
 * Contained as a list within the CVRF Vulnerability structure
 */
struct cvrf_product_status_iterator;

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param vuln CVRF Status structure to be added to the Vulnerability
 * @return true on success
 */
bool cvrf_vulnerability_add_cvrf_product_status(struct cvrf_vulnerability *vuln, struct cvrf_product_status *stat);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @return Iterator representing list of all Statuses in the Vulnerability
 */
struct cvrf_product_status_iterator *cvrf_vulnerability_get_product_statuses(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_product_status_iterator
 * @param it CVRF ProductStatus iterator structure
 * @return Next CVRF Status in list of all ProductStatuses in the Vulnerability
 */
struct cvrf_product_status *cvrf_product_status_iterator_next(struct cvrf_product_status_iterator *it);

/**
 * @memberof cvrf_product_status_iterator
 * @param it CVRF Status iterator structure
 * @return true if the iterator has another Status element left
 */
bool cvrf_product_status_iterator_has_more(struct cvrf_product_status_iterator *it);

/**
 * Deallocate memory for the CVRF Status Iterator structure
 * @memberof cvrf_product_status_iterator
 * @param it CVRF Status iterator structure
 */
void cvrf_product_status_iterator_free(struct cvrf_product_status_iterator *it);

/**
 * Restart iterator at the first Status in the Vulnerability
 * @memberof cvrf_product_status_iterator
 * @param it CVRF Status iterator structure
 */
void cvrf_product_status_iterator_reset(struct cvrf_product_status_iterator *it);

/**
 * Detaches and frees the Status iterator structure
 * @memberof cvrf_product_status_iterator
 * @param it CVRF Status iterator structure
 */
void cvrf_product_status_iterator_remove(struct cvrf_product_status_iterator *it);

/*******************************************
 * @struct cvrf_remediation_iterator
 * Iterator representing all Remediation elements in the Remediations container
 * Contained as a list within the CVRF Vulnerability structure
 */
struct cvrf_remediation_iterator;

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param vuln CVRF Remediation structure to be added to the Vulnerability
 * @return true on success
 */
bool cvrf_vulnerability_add_remediation(struct cvrf_vulnerability *vuln, struct cvrf_remediation *remed);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @return Iterator representing list of all Remediations in the Vulnerability
 */
struct cvrf_remediation_iterator *cvrf_vulnerability_get_remediations(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_remediation_iterator
 * @param it CVRF Remediation iterator structure
 * @return Next CVRF Remediation in list of all Remediations in the Vulnerability
 */
struct cvrf_remediation *cvrf_remediation_iterator_next(struct cvrf_remediation_iterator *it);

/**
 * @memberof cvrf_remediation_iterator
 * @param it CVRF Remediation iterator structure
 * @return true if the iterator has another Remediation element left
 */
bool cvrf_remediation_iterator_has_more(struct cvrf_remediation_iterator *it);

/**
 * Deallocate memory for the CVRF Remediation Iterator structure
 * @memberof cvrf_remediation_iterator
 * @param it CVRF Remediation iterator structure
 */
void cvrf_remediation_iterator_free(struct cvrf_remediation_iterator *it);

/**
 * Restart iterator at the first Remediation in the Vulnerability
 * @memberof cvrf_remediation_iterator
 * @param it CVRF Remediation iterator structure
 */
void cvrf_remediation_iterator_reset(struct cvrf_remediation_iterator *it);

/**
 * Detaches and frees the Remediation iterator structure
 * @memberof cvrf_remediation_iterator
 * @param it CVRF Remediation iterator structure
 */
void cvrf_remediation_iterator_remove(struct cvrf_remediation_iterator *it);

/*******************************************
 * @struct cvrf_threat_iterator
 * Iterator representing all Threat elements in the Threats container
 * Contained as a list within the CVRF Vulnerability structure
 */
struct cvrf_threat_iterator;

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @param vuln CVRF Threat structure to be added to the Vulnerability
 * @return true on success
 */
bool cvrf_vulnerability_add_threat(struct cvrf_vulnerability *vuln, struct cvrf_threat *threat);

/**
 * @memberof cvrf_vulnerability
 * @param vuln CVRF Vulnerability structure
 * @return Iterator representing list of all Threats in the Vulnerability
 */
struct cvrf_threat_iterator *cvrf_vulnerability_get_threats(const struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_threat_iterator
 * @param it CVRF Threat iterator structure
 * @return Next CVRF Threat in list of all Threats in the Vulnerability
 */
struct cvrf_threat *cvrf_threat_iterator_next(struct cvrf_threat_iterator *it);

/**
 * @memberof cvrf_threat_iterator
 * @param it CVRF Threat iterator structure
 * @return true if the iterator has another Threat element left
 */
bool cvrf_threat_iterator_has_more(struct cvrf_threat_iterator *it);

/**
 * Deallocate memory for the CVRF Threat Iterator structure
 * @memberof cvrf_threat_iterator
 * @param it CVRF Threat iterator structure
 */
void cvrf_threat_iterator_free(struct cvrf_threat_iterator *it);

/**
 * Restart iterator at the first Threat in the Vulnerability
 * @memberof cvrf_threat_iterator
 * @param it CVRF Threat iterator structure
 */
void cvrf_threat_iterator_reset(struct cvrf_threat_iterator *it);

/**
 * Detaches and frees the Threat iterator structure
 * @memberof cvrf_threat_iterator
 * @param it CVRF Threat iterator structure
 */
void cvrf_threat_iterator_remove(struct cvrf_threat_iterator *it);

/************************************************************************************************
 * @struct cvrf_product_name
 * Represents FullProductName element, which is a child of both Branch and Relationship elements
 * Links a CPE to its ProductID or a package ProductID. No two FullProductName elements can
 * have the same ProductID
 *
 * PARENT NODE: ProductTree, Branch, or Relationship
 * REQUIRED: ProductID attribute [max: 1], CPE element [max: 1]
 */
struct cvrf_product_name;

/**
 * New FullProductName of Branch or ProductTree
 * @memberof cvrf_product_name
 * @return New FullProductName
 */
struct cvrf_product_name *cvrf_product_name_new(void);

/**
 * Deallocates memory for a FullProductName element
 * @memberof cvrf_product_name
 * @param full_name The CVRF FullProductName structure to be freed
 */
void cvrf_product_name_free(struct cvrf_product_name *full_name);

/**
 * @memberof cvrf_product_name
 * @param full_name Original FullProductName structure to be cloned
 * @return New cloned FullProductName structure with same data as the original
 */
struct cvrf_product_name *cvrf_product_name_clone(const struct cvrf_product_name *full_name);

/**
 * @memberof cvrf_product_name
 * @param full_name CVRF FullProductName structure
 *
 * @return ProductID attribute within the FullProductName element
 */
const char *cvrf_product_name_get_product_id(const struct cvrf_product_name *full_name);

/**
 * @memberof cvrf_product_name
 * @param full_name CVRF FullProductName structure
 *
 * @return CPE information contained in the FullProductName element
 */
const char *cvrf_product_name_get_cpe(const struct cvrf_product_name *full_name);

/**
 * @memberof cvrf_product_name
 * @param full_name CVRF FullProductName structure
 *
 * @return true on success
 */
bool cvrf_product_name_set_product_id(struct cvrf_product_name *full_name, const char *product_id);

/**
 * @memberof cvrf_product_name
 * @param full_name CVRF FullProductName structure
 *
 * @return true on success
 */
bool cvrf_product_name_set_cpe(struct cvrf_product_name *full_name, const char *cpe);

/************************************************************************************************
 * @struct cvrf_group
 * Structure that categorically groups different products referenced in the document.
 * One ProductID is used per group member. The GroupID can be used as a reference for
 * other elements.
 *
 * PARENT NODE: ProductGroups container (which is a child of ProductTree)
 * REQUIRED: GroupID attribute [max: 1], ProductID element [min:2, max: unbounded]
 * OPTIONAL: Description element [min: 0, max: 1]
 */
struct cvrf_group;

/**
 * New CVRF Group child of ProductGroups container element
 * @memberof cvrf_group
 * @return New CVRF Group structure
 */
struct cvrf_group *cvrf_group_new(void);

/**
 * Deallocates memory for a Group element
 * @memberof cvrf_group
 * @param group The CVRF Group structure to be freed
 */
void cvrf_group_free(struct cvrf_group *group);

/**
 * @memberof cvrf_group
 * @param group Original Group structure to be cloned
 * @return New cloned Group structure with same data as the original
 */
struct cvrf_group *cvrf_group_clone(const struct cvrf_group *group);

/**
 * @memberof cvrf_group
 * @param group CVRF Group structure
 * Unique identifier for a Group; used for referencing the Group elsewhere in the document
 * @return contents of GroupID attribute of a Group element
 */
const char *cvrf_group_get_group_id(const struct cvrf_group *group);

/**
 * @memberof cvrf_group
 * @param group CVRF Group structure
 * Short and human-readable description about the Group
 * @return contents of Description element of a Group element
 */
const char *cvrf_group_get_description(const struct cvrf_group *group);

/**
 * @memberof cvrf_group
 * @param group CVRF Group structure
 * @param group_id Unique identifier for a Group for referencing elsewhere in the document
 * @return true on success
 */
bool cvrf_group_set_group_id(struct cvrf_group *group, const char *group_id);

/**
 * @memberof cvrf_group
 * @param group CVRF Group structure
 * @param description Short and human-readable description about the Group
 * @return true on success
 */
bool cvrf_group_set_description(struct cvrf_group *group, const char *description);

/**
 * @memberof cvrf_group
 * @param group CVRF Group structure
 * List of all ProductIDs representing the products that belong to this Group
 * @return Iterator for the stringlist of ProductIDs
 */
struct oscap_string_iterator *cvrf_group_get_product_ids(struct cvrf_group *group);


/************************************************************************************************
 * @struct cvrf_relationship
 * Structure within a parent ProductTree. Establishes a link between two products (FullProductNames)
 * referenced in the document. Thus, at least two FullProductName elements must exist in the document.
 *
 * PARENT NODE: ProductTree
 * REQUIRED: ProductReference attribute [max: 1], RelationType attribute [max: 1],
 * 		RelatesToProductReference attribute [max: 1], FullProductName element [max: 1]
 */
struct cvrf_relationship;

/**
 * New CVRF Relationship element within ProductTree
 * @memberof cvrf_relationship
 * @return New CVRF Relationship structure
 */
struct cvrf_relationship *cvrf_relationship_new(void);

/**
 * Deallocates memory for a Relationship element
 * @memberof cvrf_relationship
 * @param relationship The CVRF Relationship structure to be freed
 */
void cvrf_relationship_free(struct cvrf_relationship *relationship);

/**
 * @memberof cvrf_relationship
 * @param relation Original Relationship structure to be cloned
 * @return New cloned Relationship structure with same data as the original
 */
struct cvrf_relationship *cvrf_relationship_clone(const struct cvrf_relationship *relation);

/**
 * @memberof cvrf_relationship
 * @param relation CVRF Relationship structure
 * Reference to a package ProductID that is a component of the CPE in RelatesToProductReference
 * @return contents of ProductReference attribute of a Relationship element
 */
const char *cvrf_relationship_get_product_reference(const struct cvrf_relationship *relation);

/**
 * @memberof cvrf_relationship
 * @param relation CVRF Relationship structure
 * ProductID for a CPE referenced in preceding Branches in the ProductTree
 * @return contents of RelatesToProductReference attribute of a Relationship element
 */
const char *cvrf_relationship_get_relates_to_ref(const struct cvrf_relationship *relation);

/**
 * @memberof cvrf_relationship
 * @param relation CVRF Relationship structure
 * Typically combines references to the CPE and package ProductID named in Relationship attributes
 * @return FullProductName child element of a Relationship element
 */
struct cvrf_product_name *cvrf_relationship_get_product_name(const struct cvrf_relationship *relation);

/**
 * @memberof cvrf_relationship
 * @param relation CVRF Relationship structure
 * @param product_reference Combined reference to the CPE and package ProductID
 * @return true on success
 */
bool cvrf_relationship_set_product_reference(struct cvrf_relationship *relation, const char *product_reference);

/**
 * @memberof cvrf_relationship
 * @param relation CVRF Relationship structure
 * @param relates_to_ref ProductID for a CPE referenced in preceding Branches in the ProductTree
 * @return true on success
 */
bool cvrf_relationship_set_relates_to_ref(struct cvrf_relationship *relation, const char *relates_to_ref);

/**
 * @memberof cvrf_relationship
 * @param relation CVRF Relationship structure
 * @param name References to the CPE and package ProductID named in Relationship attributes
 * @return true on success
 */
bool cvrf_relationship_set_product_name(struct cvrf_relationship *relation, struct cvrf_product_name *name);

/************************************************************************************************
 * @struct cvrf_branch
 * Establishes hierarchy and relationships between different CPEs, products, and packages
 * Can only have one type of child element: a FullProductName or more Branches
 *
 * PARENT NODE: ProductTree or Branch
 * REQUIRED: Name attribute [max: 1], Type attribute [max: 1], EITHER FullProductName element [max: 1] OR
 * 		child Branch elements [min: 1, max: unbounded]
 */
struct cvrf_branch;

/**
 * New CVRF branch of ProductTree or sub-branch
 * @memberof cvrf_branch
 * @return New CVRF branch
 */
struct cvrf_branch *cvrf_branch_new(void);

/**
 * Deallocates memory for a Branch element
 * @memberof cvrf_branch
 * @param branch The CVRF Branch structure to be freed
 */
void cvrf_branch_free(struct cvrf_branch *branch);

/**
 * @memberof cvrf_branch
 * @param branch Original Branch structure to be cloned
 * @return New cloned Branch structure with same data as the original
 */
struct cvrf_branch *cvrf_branch_clone(const struct cvrf_branch *branch);

/**
 * @memberof cvrf_branch
 * @param branch CVRF Branch structure
 * Canonical description of the Branch; paired with Branch Type
 * @return contents of Name attribute within a Branch element
 */
const char *cvrf_branch_get_branch_name(const struct cvrf_branch *branch);

/**
 * @memberof cvrf_branch
 * @param branch CVRF Branch structure
 * Child element and endpoint of tree for branches not of the type 'Product Family'
 * @return FullProductName child element of a Branch element
 */
struct cvrf_product_name *cvrf_branch_get_product_name(const struct cvrf_branch *branch);

/**
 * @memberof cvrf_branch
 * @param branch CVRF Branch structure
 * Sub-branch children of 'Product Family'-type Branches
 * @return Iterator for sub-branch structures for this Branch element
 */
struct oscap_iterator *cvrf_branch_get_subbranches(struct cvrf_branch *branch);

/**
 * @memberof cvrf_branch
 * @param branch CVRF Branch structure
 * @param branch_name Canonical description of the Branch; paired with Branch Type
 * @return true on success
 */
bool cvrf_branch_set_branch_name(struct cvrf_branch *branch, const char *branch_name);

/**
 * @memberof cvrf_branch
 * @param branch CVRF Branch structure
 * @param name Child element and endpoint of tree for branches not of the type 'Product Family'
 * @return true on success
 */
bool cvrf_branch_set_product_name(struct cvrf_branch *branch, struct cvrf_product_name *name);

/************************************************************************************************
 *@struct cvrf_product_tree
 * Relates information about CPEs, their given ProductIDs, packages and their given ProductIDs through
 * Branch and Relationship child elements
 *
 * PARENT NODE: cvrfdoc
 * OPTIONAL: FullProductName element [min: 0, max: unbounded], Branch element [min: 0, max: unbounded],
 * 		Relationship element [min: 0, max: unbounded]
 */
struct cvrf_product_tree;

/**
 * New ProductTree structure
 * @memberof cvrf_product_tree
 * @return New CVRF ProductTree
 */
struct cvrf_product_tree *cvrf_product_tree_new(void);

/**
 * Deallocates memory for a ProductTree element and all its child Branches, Relationships,
 * and FullProductNames, if they exist
 * @memberof cvrf_product_tree
 * @param tree The CVRF ProductTree structure to be freed
 */
void cvrf_product_tree_free(struct cvrf_product_tree *tree);

/**
 * @memberof cvrf_product_tree
 * @param tree Original ProductTree structure to be cloned
 * @return New cloned ProductTree structure with same data as the original
 */
struct cvrf_product_tree *cvrf_product_tree_clone(const struct cvrf_product_tree *tree);

/**
 * Find the unique ProductID for the given CPE by searching the branches of the
 * ProductTree: find the ProductID attribute for the FullProductName element with
 * the appropriate CPE
 * @memberof cvrf_product_tree
 * @param tree ProductTree structure to be searched
 * @param cpe CPE string in the FullProductName element used to get the ProductID
 * @return ProductID for the given CPE
 */
const char *get_cvrf_product_id_from_cpe(struct cvrf_product_tree *tree, const char *cpe);

/**
 * Use the CPE name to find the matching ProductID, then filter the tree by removing
 * branches and relationship elements not related to that ProductID
 * @memberof cvrf_product_tree
 * @param tree ProductTree structure to be filtered
 * @param cpe CPE name used to filter all unrelated elements
 * @return 0 on success, -1 on failure
 */
int cvrf_product_tree_filter_by_cpe(struct cvrf_product_tree *tree, const char *cpe);

/*---------------------------------------------------------------------*\
|				Iterators of child elements of ProductTree				|
\*---------------------------------------------------------------------*/

struct oscap_iterator *cvrf_product_tree_get_branches(struct cvrf_product_tree *tree);

/*******************************************
 * @struct cvrf_product_name_iterator
 * Iterator representing all FullProductName elements in the ProductTree
 * Contained as a list within the CVRF ProductTree structure
 */
struct cvrf_product_name_iterator;

/**
 * @memberof cvrf_product_tree
 * @param tree CVRF ProductTree structure
 * @param full_name CVRF FullProductName structure to be added to the ProductTree
 * @return true on success
 */
bool cvrf_product_tree_add_product_name(struct cvrf_product_tree *tree, struct cvrf_product_name *full_name);

/**
 * @memberof cvrf_product_tree
 * @param tree CVRF ProductTree structure
 * @return Iterator representing list of all FullProductNames in the ProductTree
 */
struct cvrf_product_name_iterator *cvrf_product_tree_get_product_names(const struct cvrf_product_tree *tree);

/**
 * @memberof cvrf_product_name_iterator
 * @param it CVRF FullProductName iterator structure
 * @return Next CVRF FullProductName in list of all FullProductNames in the ProductTree
 */
struct cvrf_product_name *cvrf_product_name_iterator_next(struct cvrf_product_name_iterator *it);

/**
 * @memberof cvrf_product_name_iterator
 * @param it CVRF FullProductName iterator structure
 * @return true if the Iterator has another FullProductName element left
 */
bool cvrf_product_name_iterator_has_more(struct cvrf_product_name_iterator *it);

/**
 * Deallocate memory for the CVRF FullProductName Iterator structure
 * @memberof cvrf_product_name_iterator
 * @param it CVRF FullProductName iterator structure
 */
void cvrf_product_name_iterator_free(struct cvrf_product_name_iterator *it);

/**
 * Restart iterator at the first FullProductName element in the ProductTree
 * @memberof cvrf_product_name_iterator
 * @param it CVRF FullProductName iterator structure
 */
void cvrf_product_name_iterator_reset(struct cvrf_product_name_iterator *it);

/**
 * Detaches and frees the FullProductName iterator structure
 * @memberof cvrf_product_name_iterator
 * @param it CVRF FullProductName iterator structure
 */
void cvrf_product_name_iterator_remove(struct cvrf_product_name_iterator *it);

/*******************************************
 * @struct cvrf_relationship_iterator
 * Iterator representing all Relationship elements in the ProductTree
 * Contained as a list within the CVRF Relationship structure
 */
struct cvrf_relationship_iterator;

/**
 * @memberof cvrf_product_tree
 * @param tree CVRF ProductTree structure
 * @param relation CVRF Relationship structure to be added to the ProductTree
 * @return true on success
 */
bool cvrf_product_tree_add_relationship(struct cvrf_product_tree *tree, struct cvrf_relationship *relation);

/**
 * @memberof cvrf_product_tree
 * @param tree CVRF ProductTree structure
 * @return Iterator representing list of all Relationships in the ProductTree
 */
struct cvrf_relationship_iterator *cvrf_product_tree_get_relationships(const struct cvrf_product_tree *tree);

/**
 * @memberof cvrf_relationship_iterator
 * @param it CVRF Relationship iterator structure
 * @return Next CVRF Relationship in list of all Relationships in the ProductTree
 */
struct cvrf_relationship *cvrf_relationship_iterator_next(struct cvrf_relationship_iterator *it);

/**
 * @memberof cvrf_relationship_iterator
 * @param it CVRF Relationship iterator structure
 * @return true if the Iterator has another Relationship element left
 */
bool cvrf_relationship_iterator_has_more(struct cvrf_relationship_iterator *it);

/**
 * Deallocate memory for the CVRF Relationship Iterator structure
 * @memberof cvrf_relationship_iterator
 * @param it CVRF Relationship iterator structure
 */
void cvrf_relationship_iterator_free(struct cvrf_relationship_iterator *it);

/**
 * Restart iterator at the first Relationship element in the ProductTree
 * @memberof cvrf_relationship_iterator
 * @param it CVRF Relationship iterator structure
 */
void cvrf_relationship_iterator_reset(struct cvrf_relationship_iterator *it);

/**
 * Detaches and frees the Relationship iterator structure
 * @memberof cvrf_relationship_iterator
 * @param it CVRF Relationship iterator structure
 */
void cvrf_relationship_iterator_remove(struct cvrf_relationship_iterator *it);


/*******************************************
 * @struct cvrf_group_iterator
 * Iterator representing all Group elements in the ProductTree
 * Contained as a list within the CVRF ProductTree structure
 */
struct cvrf_group_iterator;

/**
 * @memberof cvrf_product_tree
 * @param tree CVRF ProductTree structure
 * @param group CVRF Group structure to be added to the ProductTree
 * @return true on success
 */
bool cvrf_product_tree_add_group(struct cvrf_product_tree *tree, struct cvrf_group *group);

/**
 * @memberof cvrf_product_tree
 * @param tree CVRF ProductTree structure
 * @return Iterator representing list of all Groups in the ProductTree
 */
struct cvrf_group_iterator *cvrf_product_tree_get_product_groups(const struct cvrf_product_tree *tree);

/**
 * @memberof cvrf_group_iterator
 * @param it CVRF Group iterator structure
 * @return Next CVRF Group in ProductGroups container in the ProductTree
 */
struct cvrf_group *cvrf_group_iterator_next(struct cvrf_group_iterator *it);

/**
 * @memberof cvrf_group_iterator
 * @param it CVRF Group iterator structure
 * @return true if the Iterator has another Group element left
 */
bool cvrf_group_iterator_has_more(struct cvrf_group_iterator *it);

/**
 * Deallocate memory for the CVRF Group Iterator structure
 * @memberof cvrf_group_iterator
 * @param it CVRF Group iterator structure
 */
void cvrf_group_iterator_free(struct cvrf_group_iterator *it);

/**
 * Restart iterator at the first Group element in the ProductTree
 * @memberof cvrf_group_iterator
 * @param it CVRF Group iterator structure
 */
void cvrf_group_iterator_reset(struct cvrf_group_iterator *it);

/**
 * Detaches and frees the Group iterator structure
 * @memberof cvrf_group_iterator
 * @param it CVRF Group iterator structure
 */
void cvrf_group_iterator_remove(struct cvrf_group_iterator *it);


/************************************************************************************************
 * @struct cvrf_acknowledgment
 * Metadata giving recognition of external parties who provided information, suggestions, or
 * other contributions
 *
 * PARENT NODE: Acknowledgments container (in cvrfdoc or in Vulnerability)
 * OPTIONAL: Name element(s) [min: 0, max: unbounded], Organization(s) element [min: 0, max: unbounded],
 * 		Description element [min: 0, max: 1], URL element(s) [min: 0, max: unbounded]
 */
struct cvrf_acknowledgment;

/**
 * New CVRF Acknowledgment element within Acknowledgments container
 * @memberof cvrf_acknowledgment
 * @return New CVRF Acknowledgment structure
 */
struct cvrf_acknowledgment *cvrf_acknowledgment_new(void);

/**
 * Deallocates memory for an Acknowledgment element of the Acknowledgments container
 * @memberof cvrf_acknowledgment
 * @param ack The CVRF Acknowledgment structure to be freed
 */
void cvrf_acknowledgment_free(struct cvrf_acknowledgment *ack);

/**
 * @memberof cvrf_acknowledgment
 * @param ack Original Acknowledgment structure to be cloned
 * @return New cloned Acknowledgment structure with same data as the original
 */
struct cvrf_acknowledgment *cvrf_acknowledgment_clone(const struct cvrf_acknowledgment *ack);

/**
 * @memberof cvrf_acknowledgment
 * @param ack CVRF Acknowledgment structure
 * Contextual information about acknowledgers or acknowledged parties
 * @return contents of Description element within an Acknowledgment element
 */
const char *cvrf_acknowledgment_get_description(const struct cvrf_acknowledgment *ack);

/**
 * @memberof cvrf_acknowledgment
 * @param ack CVRF Acknowledgment structure
 * @param description Contextual information about acknowledgers or acknowledged parties
 * @return true on success
 */
bool cvrf_acknowledgment_set_description(struct cvrf_acknowledgment *ack, const char *description);

/**
 * @memberof cvrf_acknowledgment
 * @param ack CVRF Acknowledgment structure
 * List of names of all parties who are being acknowledged
 * @return String iterator representing list of all names
 */
struct oscap_string_iterator *cvrf_acknowledgment_get_names(const struct cvrf_acknowledgment *ack);

/**
 * @memberof cvrf_acknowledgment
 * @param ack CVRF Acknowledgment structure
 * Organizations of parties and/or organization(s) itself being acknowledged
 * @return String iterator representing list of all organizations
 */
struct oscap_string_iterator *cvrf_acknowledgment_get_organizations(const struct cvrf_acknowledgment *ack);

/**
 * @memberof cvrf_acknowledgment
 * @param ack CVRF Acknowledgment structure
 * URLs to people or parties being acknowledged
 * @return String iterator representing list of all URLs
 */
struct oscap_string_iterator *cvrf_acknowledgment_get_urls(const struct cvrf_acknowledgment *ack);

/************************************************************************************************
 * @struct cvrf_note
 *
 * PARENT NODE: DocumentNotes container or Notes container
 * REQUIRED: Type attribute [min: 1, max: 1], Ordinal attribute [min: 1, max: 1],
 * OPTIONAL: Audience attribute [min: 0, max: 1], Title attribute [min: 0, max: 1]
 */
struct cvrf_note;

/**
 * New CVRF Note element within Notes or DocumentNotes container
 * @memberof cvrf_note
 * @return New CVRF Note structure
 */
struct cvrf_note *cvrf_note_new(void);

/**
 * Deallocates memory for a Note element of the Notes container
 * @memberof cvrf_note
 * @param note The CVRF Note structure to be freed
 */
void cvrf_note_free(struct cvrf_note *note);

/**
 * @memberof cvrf_note
 * @param note Original Note structure to be cloned
 * @return New cloned Note structure with same data as the original
 */
struct cvrf_note *cvrf_note_clone(const struct cvrf_note *note);

/**
 * @memberof cvrf_note
 * @param note CVRF Note structure
 * Index of a Note element within a Notes container
 * @return Ordinal attribute of the Note element
 */
int cvrf_note_get_ordinal(const struct cvrf_note *note);

/**
 * @memberof cvrf_note
 * @param note CVRF Note structure
 * Party to whom the information in this note is directed
 * @return Contents of Audience attribute of the Note element
 */
const char *cvrf_note_get_audience(const struct cvrf_note *note);

/**
 * @memberof cvrf_note
 * @param note CVRF Note structure
 * Concise description of the contents of the Note
 * @return Contents of Title attribute of the Note element
 */
const char *cvrf_note_get_title(const struct cvrf_note *note);

/**
 * @memberof cvrf_note
 * @param note CVRF Note structure
 * Information relating to the document that is contained in the Note
 * @return Contents of the Note element itself
 */
const char *cvrf_note_get_contents(const struct cvrf_note *note);

/**
 * @memberof cvrf_note
 * @param note CVRF Note structure
 * @param ordinal Index of a Note element within a Notes container
 * @return true on success
 */
bool cvrf_note_set_ordinal(struct cvrf_note *note, int ordinal);

/**
 * @memberof cvrf_note
 * @param note CVRF Note structure
 * @param audience Party to whom the information in this note is directed
 * @return true on success
 */
bool cvrf_note_set_audience(struct cvrf_note *note, const char *audience);

/**
 * @memberof cvrf_note
 * @param note CVRF Note structure
 * @param title Concise description of the contents of the Note
 * @return true on success
 */
bool cvrf_note_set_title(struct cvrf_note *note, const char *title);

/**
 * @memberof cvrf_note
 * @param note CVRF Note structure
 * @param contents Information that is contained in the Note
 * @return true on success
 */
bool cvrf_note_set_contents(struct cvrf_note *note, const char *contents);


/************************************************************************************************
 * @struct cvrf_revision
 * Metadata for tracking the changes made to a CVRF document
 *
 * PARENT NODE: RevisionHistory container (which is a child of DocumentTracking)
 * REQUIRED: Number element [min: 1, max: 1], Date element [min: 1, max: 1],
 * 		Description element [min: 1, max: 1]
 */
struct cvrf_revision;

/**
 * New CVRF Revision structure
 * @memberof cvrf_revision
 * @return New CVRF Revision
 */
struct cvrf_revision *cvrf_revision_new(void);

/**
 * Deallocates memory for a Revision element of the RevisionHistory container
 * @memberof cvrf_revision
 * @param revision The CVRF Revision structure to be freed
 */
void cvrf_revision_free(struct cvrf_revision *revision);

/**
 * @memberof cvrf_revision
 * @param revision Original Revision structure to be cloned
 * @return New cloned Revision structure with same data as the original
 */
struct cvrf_revision *cvrf_revision_clone(const struct cvrf_revision *revision);

/**
 * @memberof cvrf_revision
 * @param revision CVRF Revision structure
 * Numeric representation of document version in tokenized format "nn.nn.nn.nn"
 * @return contents of Number element within the Revision element
 */
const char *cvrf_revision_get_number(const struct cvrf_revision *revision);

/**
 * @memberof cvrf_revision
 * @param revision CVRF Revision structure
 * Datetime of when the Revision was made
 * @return contents of Date element within the Revision element
 */
const char *cvrf_revision_get_date(const struct cvrf_revision *revision);

/**
 * @memberof cvrf_revision
 * @param revision CVRF Revision structure
 * Short list of items changed and/or reasons for making the change
 * @return contents of Description element within the Revision element
 */
const char *cvrf_revision_get_description(const struct cvrf_revision *revision);

/**
 * @memberof cvrf_revision
 * @param revision CVRF Revision structure
 * @param number Numeric representation of document version in format "nn.nn.nn.nn"
 * @return true on success
 */
bool cvrf_revision_set_number(struct cvrf_revision *revision, const char *number);

/**
 * @memberof cvrf_revision
 * @param revision CVRF Revision structure
 * @param date Datetime of when the Revision was made
 * @return true on success
 */
bool cvrf_revision_set_date(struct cvrf_revision *revision, const char *date);

/**
 * @memberof cvrf_revision
 * @param revision CVRF Revision structure
 * @param description Short list of items changed and/or reasons for making the change
 * @return true on success
 */
bool cvrf_revision_set_description(struct cvrf_revision *revision, const char *description);

/************************************************************************************************
 * @struct cvrf_doc_tracking
 * Metadata for referencing origin, changes, release dates, IDs, and other information about
 * the document
 *
 * PARENT NODE: cvrfdoc
 * REQUIRED: Identification container [max: 1] { ID element [max: 1], Alias [min: 0, max: unbounded] },
 * 		Status element [max: 1], Version element [max: 1], RevisionHistory container [max: 1],
 * 		InitialReleaseDate [max: 1], CurrentReleaseDate [max: 1],
 * 		Generator container [max: 1] { Engine [min: 0, max: 1], Date [min: 0, max: 1] }
 */
struct cvrf_doc_tracking;

/**
 * New CVRF DocumentTracking structure
 * @memberof cvrf_doc_tracking
 * @return New CVRF DocumentTracking
 */
struct cvrf_doc_tracking *cvrf_doc_tracking_new(void);

/**
 * Deallocates memory for a DocumentTracking element
 * @memberof cvrf_doc_tracking
 * @param tracking The CVRF DocumentTracking structure to be freed
 */
void cvrf_doc_tracking_free(struct cvrf_doc_tracking *tracking);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking Original DocumentTracking structure to be cloned
 * @return New cloned DocumentTracking structure with same data as the original
 */
struct cvrf_doc_tracking *cvrf_doc_tracking_clone(const struct cvrf_doc_tracking *tracking);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * Short identifier to unambiguously reference the document in any context
 * @return contents of ID element within Identification element
 */
const char *cvrf_doc_tracking_get_tracking_id(const struct cvrf_doc_tracking *tracking);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * Optional alternative ID(s) for the document
 * @return Iterator for the stringlist of Alias elements within Identification element
 */
struct oscap_string_iterator *cvrf_doc_tracking_get_aliases(struct cvrf_doc_tracking *tracking);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * Counter to track the document in tokenized format "nn" - "nn.nn.nn.nn"
 * @return contents of @return contents of Version element within DocumentTracking element
 */
const char *cvrf_doc_tracking_get_version(const struct cvrf_doc_tracking *tracking);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * Datetime of the document's original release by the issuer
 * @return contents of InitialReleaseDate element within DocumentTracking element
 */
const char *cvrf_doc_tracking_get_init_release_date(const struct cvrf_doc_tracking *tracking);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * Datetime of the document's current release by the issuer
 * @return contents of CurrentReleaseDate element within DocumentTracking element
 */
const char *cvrf_doc_tracking_get_cur_release_date(const struct cvrf_doc_tracking *tracking);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * Name and optional version of the generator of the document
 * @return contents of Engine element within Generator element
 */
const char *cvrf_doc_tracking_get_generator_engine(const struct cvrf_doc_tracking *tracking);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * Datetime of when the document was generated
 * @return contents of Date element within Generator element
 */
const char *cvrf_doc_tracking_get_generator_date(const struct cvrf_doc_tracking *tracking);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * @param id Short identifier to unambiguously reference the document in any context
 * @return true on success
 */
bool cvrf_doc_tracking_set_tracking_id(struct cvrf_doc_tracking *tracking, const char *id);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * @param version Counter to track the document in tokenized format "nn" - "nn.nn.nn.nn"
 * @return true on success
 */
bool cvrf_doc_tracking_set_version(struct cvrf_doc_tracking *tracking, const char *version);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * @param init_release_date Datetime of the document's original release by the issuer
 * @return true on success
 */
bool cvrf_doc_tracking_set_init_release_date(struct cvrf_doc_tracking *tracking, const char *init_release_date);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * @param cur_release_date Datetime of the document's current release by the issuer
 * @return true on success
 */
bool cvrf_doc_tracking_set_cur_release_date(struct cvrf_doc_tracking *tracking, const char *cur_release_date);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * @param generator_engine Name and optional version of the generator of the document
 * @return true on success
 */
bool cvrf_doc_tracking_set_generator_engine(struct cvrf_doc_tracking *tracking, const char *generator_engine);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * @param generator_date Datetime of when the document was generated
 * @return true on success
 */
bool cvrf_doc_tracking_set_generator_date(struct cvrf_doc_tracking *tracking, const char *generator_date);


/*******************************************
 * @struct cvrf_revision_iterator
 * Iterator representing all Revision elements in the RevisionHistory container
 * Contained as a list within the CVRF DocumentTracking structure
 */
struct cvrf_revision_iterator;

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * @param revision CVRF Revision structure to be added to the DocumentTracking
 * @return true on success
 */
bool cvrf_doc_tracking_add_revision(struct cvrf_doc_tracking *tracking, struct cvrf_revision *revision);

/**
 * @memberof cvrf_doc_tracking
 * @param tracking CVRF DocumentTracking structure
 * @return Iterator representing list of all Revisions in the DocumentTracking
 */
struct cvrf_revision_iterator *cvrf_doc_tracking_get_revision_history(const struct cvrf_doc_tracking *tracking);

/**
 * @memberof cvrf_revision_iterator
 * @param it CVRF Revision iterator structure
 * @return Next CVRF Revision in list of all Revisions in the DocumentTracking
 */
struct cvrf_revision *cvrf_revision_iterator_next(struct cvrf_revision_iterator *it);

/**
 * @memberof cvrf_revision_iterator
 * @param it CVRF Revision iterator structure
 * @return true if the iterator has another Revision element left
 */
bool cvrf_revision_iterator_has_more(struct cvrf_revision_iterator *it);

/**
 * Deallocate memory for the CVRF Revision Iterator structure
 * @memberof cvrf_revision_iterator
 * @param it CVRF Revision iterator structure
 */
void cvrf_revision_iterator_free(struct cvrf_revision_iterator *it);

/**
 * Restart iterator at the first Revision in the DocumentTracking
 * @memberof cvrf_revision_iterator
 * @param it CVRF Revision iterator structure
 */
void cvrf_revision_iterator_reset(struct cvrf_revision_iterator *it);

/**
 * Detaches and frees the Revision iterator structure
 * @memberof cvrf_revision_iterator
 * @param it CVRF Revision iterator structure
 */
void cvrf_revision_iterator_remove(struct cvrf_revision_iterator *it);


/************************************************************************************************
 * @struct cvrf_doc_publisher
 * Metadata about the publisher of the CVRF document
 *
 * PARENT NODE: cvrfdoc
 * REQUIRED: Type attribute [max: 1]
 * OPTIONAL: VendorID attribute [min: 0, max: 1], ContactDetails element [min: 0, max: 1],
 * 		IssuingAuthority element [min: 0, max: 1]
 */
struct cvrf_doc_publisher;

/**
 * New CVRF DocumentPublisher structure
 * @memberof cvrf_doc_publisher
 * @return New CVRF DocumentPublisher structure
 */
struct cvrf_doc_publisher *cvrf_doc_publisher_new(void);

/**
 * Deallocates memory for a DocumentPublisher element
 * @memberof cvrf_doc_publisher
 * @param publisher The CVRF DocumentPublisher structure to be freed
 */
void cvrf_doc_publisher_free(struct cvrf_doc_publisher *publisher);

/**
 * @memberof cvrf_doc_publisher
 * @param revision Original DocumentPublisher structure to be cloned
 * @return New cloned DocumentPublisher structure with same data as the original
 */
struct cvrf_doc_publisher *cvrf_doc_publisher_clone(const struct cvrf_doc_publisher *publisher);

/**
 * @memberof cvrf_doc_publisher
 * @param publisher CVRF DocumentPublisher structure
 * Unique ID of the Vendor, if any, who published the document
 * @return Content of the VendorID attribute of the DocumentPublisher element
 */
const char *cvrf_doc_publisher_get_vendor_id(const struct cvrf_doc_publisher *publisher);

/**
 * @memberof cvrf_doc_publisher
 * @param publisher CVRF DocumentPublisher structure
 * Contains information needed to get contact with the document publisher
 * @return Content of the ContactDetails element of the DocumentPublisher element
 */
const char *cvrf_doc_publisher_get_contact_details(const struct cvrf_doc_publisher *publisher);

/**
 * @memberof cvrf_doc_publisher
 * @param publisher CVRF DocumentPublisher structure
 * Contains name of the issuer and their level of authority over the document's release
 * @return Content of the IssuingAuthority element of the DocumentPublisher element
 */
const char *cvrf_doc_publisher_get_issuing_authority(const struct cvrf_doc_publisher *publisher);

/**
 * @memberof cvrf_doc_publisher
 * @param publisher CVRF DocumentPublisher structure
 * @param vendor_id Unique ID of the Vendor, if any, who published the document
 * @return true on success
 */
bool cvrf_doc_publisher_set_vendor_id(struct cvrf_doc_publisher *publisher, const char *vendor_id);

/**
 * @memberof cvrf_doc_publisher
 * @param publisher CVRF DocumentPublisher structure
 * @param contact_details Information needed to get contact with the document publisher
 * @return true on success
 */
bool cvrf_doc_publisher_set_contact_details(struct cvrf_doc_publisher *publisher, const char *contact_details);

/**
 * @memberof cvrf_doc_publisher
 * @param publisher CVRF DocumentPublisher structure
 * @param issuing_authority Name of the issuer and their level of authority over the document's release
 * @return true on success
 */
bool cvrf_doc_publisher_set_issuing_authority(struct cvrf_doc_publisher *publisher, const char *issuing_authority);

/************************************************************************************************
 * @struct cvrf_reference
 * Refers to resources and information outside of the CVRF document itself
 *
 * PARENT NODE: DocumentReferences container (child of cvrfdoc) or References container (child of Vulnerability)
 * REQUIRED: Type attribute [max: 1], URL element [max: 1], Description element [max: 1]
 */
struct cvrf_reference;

/**
 * New CVRF Reference structure
 * @memberof cvrf_reference
 * @return New CVRF Reference
 */
struct cvrf_reference *cvrf_reference_new(void);

/**
 * Deallocates memory for a Reference element of the References container
 * @memberof cvrf_reference
 * @param reference The CVRF Reference structure to be freed
 */
void cvrf_reference_free(struct cvrf_reference *reference);

/**
 * @memberof cvrf_reference
 * @param revision Original Reference structure to be cloned
 * @return New cloned Reference structure with same data as the original
 */
struct cvrf_reference *cvrf_reference_clone(const struct cvrf_reference *ref);

/**
 * @memberof cvrf_reference
 * @param reference CVRF Reference structure
 * Contains fixed URL link to the advisory or other reference
 * @return Contents of URL element within the Reference element
 */
const char *cvrf_reference_get_url(const struct cvrf_reference *reference);

/**
 * @memberof cvrf_reference
 * @param reference CVRF Reference structure
 * Title or name of the Reference element
 * @return Contents of Description element within the Reference element
 */
const char *cvrf_reference_get_description(const struct cvrf_reference *reference);

/**
 * @memberof cvrf_reference
 * @param reference CVRF Reference structure
 * @param url Contains fixed URL link to the advisory or other reference
 * @return true on success
 */
bool cvrf_reference_set_url(struct cvrf_reference *reference, const char *url);

/**
 * @memberof cvrf_reference
 * @param reference CVRF Reference structure
 * @param description Title or name of the Reference element
 * @return true on success
 */
bool cvrf_reference_set_description(struct cvrf_reference *reference, const char *description);

/************************************************************************************************
 * @struct cvrf_document
 *
 */
struct cvrf_document;

/**
 * New CVRF Document structure
 * @memberof cvrf_document
 * @return New CVRF Document structure
 */
struct cvrf_document *cvrf_document_new(void);

/**
 * Deallocates memory for the CVRF Document structure and all its child DocumentTracking,
 * DocumentPublisher, DocumentReferences structures
 * @memberof cvrf_document
 * @param doc The CVRF Document structure to be freed
 */
void cvrf_document_free(struct cvrf_document *doc);

/**
 * @memberof cvrf_document
 * @param revision Original Document structure to be cloned
 * @return New cloned Document structure with same data as the original
 */
struct cvrf_document *cvrf_document_clone(const struct cvrf_document *doc);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * Contains legal restrains about reproducing and using the document
 * @return contents of the DocumentDistribution element within the root cvrfdoc element
 */
const char *cvrf_document_get_doc_distribution(const struct cvrf_document *doc);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * Conveys information about the severity of Vulnerabilities in the document
 * @return contents of the AggregateSeverity element within the root cvrfdoc element
 */
const char *cvrf_document_get_aggregate_severity(const struct cvrf_document *doc);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * Contains URL with link to Namespace of the AggregateSeverity element
 * @return contents of the Namespace attribute of the AggregateSeverity element
 */
const char *cvrf_document_get_namespace(const struct cvrf_document *doc);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * Structure with all elements for tracking and referencing the document itself
 * @return DocumentTracking structure of the cvrfdoc root element
 */
struct cvrf_doc_tracking *cvrf_document_get_tracking(const struct cvrf_document *doc);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * Structure with all elements that give information about the publisher
 * @return DocumentPublisher structure of the cvrfdoc root element
 */
struct cvrf_doc_publisher *cvrf_document_get_publisher(const struct cvrf_document *doc);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * Represents the DocumentNotes container that holds all Note elements
 * @return Iterator for the list of Notes within the DocumentNotes container
 */
struct oscap_iterator *cvrf_document_get_notes(struct cvrf_document *doc);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * Represents the DocumentReferences container that holds all Reference elements
 * @return Iterator for the list of References within the DocumentReferences container
 */
struct oscap_iterator *cvrf_document_get_references(struct cvrf_document *doc);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * Represents the Acknowledgments container that holds all Acknowledgment elements
 * @return Iterator for the Acknowledgment list within the Acknowledgments container
 */
struct oscap_iterator *cvrf_document_get_acknowledgments(struct cvrf_document *doc);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * @param distribution Legal restrains about reproducing and using the document
 * @return true on success
 */
bool cvrf_document_set_doc_distribution(struct cvrf_document *doc, const char *distribution);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * @param severity Information about the severity of Vulnerabilities in the document
 * @return true on success
 */
bool cvrf_document_set_aggregate_severity(struct cvrf_document *doc, const char *severity);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * @param ns URL with link to Namespace of the AggregateSeverity element
 * @return true on success
 */
bool cvrf_document_set_namespace(struct cvrf_document *doc, const char *ns);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * @param publisher Structure with all elements that give information about the publisher
 * @return true on success
 */
bool cvrf_document_set_publisher(struct cvrf_document *doc, struct cvrf_doc_publisher *publisher);

/**
 * @memberof cvrf_document
 * @param doc CVRF Document structure
 * @param track Structure with all elements for tracking and referencing the document itself
 * @return true on success
 */
bool cvrf_document_set_tracking(struct cvrf_document *doc, struct cvrf_doc_tracking *track);

/************************************************************************************************
 * @struct cvrf_model
 * Top level structure; organizationally divided into 3 components: Document, ProductTree, and
 * Vulnerability offshoots. This structure contains all information provided by a CVRF file.
 *
 * REQUIRED: DocumentTitle element [max: 1], DocumentType element [max: 1]
 * OPTIONAL: ProductTree element [min: 0, max: 1], Vulnerability [min: 0, max: unbounded]
 */
struct cvrf_model;

/**
 * New CVRF model
 * @memberof cvrf_model
 * @return New CVRF model
 */
struct cvrf_model *cvrf_model_new(void);

/**
 * Deallocates memory for the CVRF Model structure and all its child elements
 * @memberof cvrf_model
 * @param cvrf The CVRF Model structure to be freed
 */
void cvrf_model_free(struct cvrf_model *cvrf);

/**
 * @memberof cvrf_model
 * @param model Original CVRF Model structure to be cloned
 * @return New cloned CVRF Model structure with same data as the original
 */
struct cvrf_model *cvrf_model_clone(const struct cvrf_model *model);

/**
 * Removes all Branches, Relationships, and ProductIDs within Vulnerabilities that
 * do no pertain to the provided CPE
 * @memberof cvrf_model
 * @param cvrf The CVRF Model structure to be filtered
 * @param cpe CPE name by which to filter the Model
 * @return 0 on success, -1 on failure
 */
int cvrf_model_filter_by_cpe(struct cvrf_model *model, const char *cpe);

/**
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * Definitive canonical name for the document
 * @return Contents of DocumentTitle element within the root cvrfdoc element
 */
const char *cvrf_model_get_doc_title(const struct cvrf_model *model);

/**
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * Short canonical name of the document's type
 * @return Contents of DocumentType element within the root cvrfdoc element
 */
const char *cvrf_model_get_doc_type(const struct cvrf_model *model);

/**
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * @param doc_title Definitive canonical name for the document
 * @return true on success
 */
bool cvrf_model_set_doc_title(struct cvrf_model *model, const char *doc_title);

/**
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * @param doc_type Short canonical name of the document's type
 * @return true on success
 */
bool cvrf_model_set_doc_type(struct cvrf_model *model, const char *doc_type);

/**
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * Contains all Product names needed for referencing elsewhere in the document
 * @return ProductTree structure within the root cvrfdoc element
 */
struct cvrf_product_tree *cvrf_model_get_product_tree(struct cvrf_model *model);

/**
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * Structure that holds all document-related metadata, such as References,
 * DocumentTracking, DocumentPublisher, and Notes
 * @return Document structures within the root cvrfdoc element
 */
struct cvrf_document *cvrf_model_get_document(const struct cvrf_model *model);

/**
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * @return Unique ID used to track this document
 */
const char *cvrf_model_get_identification(struct cvrf_model *model);

/**
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * @param doc Structure that holds all document-related metadata
 * @return true on success
 */
bool cvrf_model_set_document(struct cvrf_model *model, struct cvrf_document *doc);

/*******************************************
 * @struct cvrf_vulnerability_iterator
 * Iterator representing all Vulnerabilities in the document
 * Contained as a list within the CVRF Model structure
 */
struct cvrf_vulnerability_iterator;

/**
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * @return Iterator representing list of all Vulnerabilities in the model
 */
struct cvrf_vulnerability_iterator *cvrf_model_get_vulnerabilities(const struct cvrf_model *model);

/**
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * @param vuln CVRF Vulnerability structure to be added to the model
 * @return true on success
 */
bool cvrf_model_add_vulnerability(struct cvrf_model *model, struct cvrf_vulnerability *vuln);

/**
 * @memberof cvrf_vulnerability_iterator
 * @param it CVRF Vulnerability iterator structure
 * @return Next CVRF Vulnerability in list of all Vulnerabilities in the document
 */
struct cvrf_vulnerability *cvrf_vulnerability_iterator_next(struct cvrf_vulnerability_iterator *it);

/**
 * @memberof cvrf_vulnerability_iterator
 * @param it CVRF Vulnerability iterator structure
 * @return true if the iterator has another Vulnerability element left
 */
bool cvrf_vulnerability_iterator_has_more(struct cvrf_vulnerability_iterator *it);

/**
 * Deallocate memory for the CVRF Vulnerability Iterator structure
 * @memberof cvrf_vulnerability_iterator
 * @param it CVRF Vulnerability iterator structure
 */
void cvrf_vulnerability_iterator_free(struct cvrf_vulnerability_iterator *it);

/**
 * Restart iterator at the first Vulnerability in the document
 * @memberof cvrf_vulnerability_iterator
 * @param it CVRF Vulnerability iterator structure
 */
void cvrf_vulnerability_iterator_reset(struct cvrf_vulnerability_iterator *it);

/**
 * Detaches and frees the Vulnerability iterator structure
 * @memberof cvrf_vulnerability_iterator
 * @param it CVRF Vulnerability iterator structure
 */
void cvrf_vulnerability_iterator_remove(struct cvrf_vulnerability_iterator *it);


/************************************************************************************************
 * @struct cvrf_index
 * Represents an index of a CVRF feed or directory
 * Maintains a list of all CVRF files in the form of cvrf_model structures
 */
struct cvrf_index;

/**
 * New index structure holding all CVRF models
 * @memberof cvrf_index
 * @return New CVRF index structure
 */
struct cvrf_index *cvrf_index_new(void);

/**
 * Deallocates memory for the CVRF Index structure and all the Models it contains
 * @memberof cvrf_index
 * @param index The CVRF Index structure to be freed
 */
void cvrf_index_free(struct cvrf_index *index);

/**
 * @memberof cvrf_index
 * @param index Original CVRF Index structure to be cloned
 * @return New cloned CVRF Index structure with same data as the original
 */
struct cvrf_index *cvrf_index_clone(const struct cvrf_index *index);

/**
 * @memberof cvrf_index
 * @param index CVRF Index structure
 * Reference to source URL used to find all CVRF files contained in the index
 * @return String representation of source URL, if one was used
 */
const char *cvrf_index_get_source_url(const struct cvrf_index *index);

/**
 * @memberof cvrf_index
 * @param index CVRF Index structure
 * Reference to path to local index file with list of all CVRF files contained in the index
 * @return String representation of source filepath
 */
const char *cvrf_index_get_index_file(const struct cvrf_index *index);

/**
 * @memberof cvrf_index
 * @param index CVRF Index structure
 * @param url Source URL used to find all CVRF files contained in the index
 * @return true on success
 */
bool cvrf_index_set_source_url(struct cvrf_index *index, const char *url);

/**
 * @memberof cvrf_index
 * @param index CVRF Index structure
 * @param index_file Filepath with list of all CVRF files contained in the index
 * @return true on success
 */
bool cvrf_index_set_index_file(struct cvrf_index *index, const char *index_file);

/*******************************************
 * @struct cvrf_model_iterator
 * Iterator representing all CVRF files from an index, feed, or stream
 * Contained as a list within the CVRF Index structure
 */
struct cvrf_model_iterator;

/**
 * @memberof cvrf_index
 * @param index CVRF Index structure
 * @param model CVRF Model structure to be added to the Index structure
 * @return true on success
 */
bool cvrf_index_add_model(struct cvrf_index *index, struct cvrf_model *model);

/**
 * @memberof cvrf_index
 * @param index CVRF Index structure
 * @return Iterator representing list of all Models in the Index
 */
struct cvrf_model_iterator *cvrf_index_get_models(const struct cvrf_index *index);

/**
 * @memberof cvrf_model_iterator
 * @param it CVRF Model iterator structure
 * @return Next CVRF Model in list of all Models in the Index
 */
struct cvrf_model *cvrf_model_iterator_next(struct cvrf_model_iterator *it);

/**
 * @memberof cvrf_model_iterator
 * @param it CVRF Model iterator structure
 * @return true if the iterator has another Model element left
 */
bool cvrf_model_iterator_has_more(struct cvrf_model_iterator *it);

/**
 * Deallocate memory for the CVRF Model Iterator structure
 * @memberof cvrf_model_iterator
 * @param it CVRF Model iterator structure
 */
void cvrf_model_iterator_free(struct cvrf_model_iterator *it);

/**
 * Restart iterator at the first Model contained in the Index
 * @memberof cvrf_model_iterator
 * @param it CVRF Model iterator structure
 */
void cvrf_model_iterator_reset(struct cvrf_model_iterator *it);

/**
 * Detaches and frees the Model iterator structure
 * @memberof cvrf_model_iterator
 * @param it CVRF Model iterator structure
 */
void cvrf_model_iterator_remove(struct cvrf_model_iterator *it);


/************************************************************************************************
 * @struct cvrf_session
 * Structure that holds and processes import source and import for CVRF models or indices
 * for later use during evaluation. Has OVAL definition structure that is used for
 * evaluation
 */
struct cvrf_session;

/**
 * @memberof cvrf_session
 * @param source OSCAP import source for a CVRF document
 * @return CVRF session structure generated from this import source
 */
struct cvrf_session *cvrf_session_new_from_source_model(struct oscap_source *source);

/**
 * @memberof cvrf_session
 * @param source OSCAP import source for an index of CVRF files
 * @return CVRF session structure generated from this import source
 */
struct cvrf_session *cvrf_session_new_from_source_index(struct oscap_source *source);

/**
 * Deallocates memory for a CVRF Session structure
 * @memberof cvrf_session
 * @param session CVRF session structure to be freed
 */
void cvrf_session_free(struct cvrf_session *session);

/**
 * @memberof cvrf_session
 * @param session CVRF session structure
 * @return CVRF model structure imported into the session, if any
 */
struct cvrf_model *cvrf_session_get_model(struct cvrf_session *session);

/**
 * @memberof cvrf_session
 * @param session CVRF session structure
 * @return CVRF index structure imported into the session, if any
 */
struct cvrf_index *cvrf_session_get_index(const struct cvrf_session *session);

/**
 * After filtering the CVRF model structure by the CPE, all the ProductIDs related
 * to that operating system should be held in this stringlist
 * @memberof cvrf_session
 * @param session CVRF session structure
 * @return Iterator for the list of ProductIDs maintained by the session structure
 */
struct oscap_string_iterator *cvrf_session_get_product_ids(struct cvrf_session *session);

/**
 * @memberof cvrf_session
 * @param session CVRF session structure
 * @return CPE used to evaluate the CVRF model(s)
 */
const char *cvrf_session_get_os_name(const struct cvrf_session *session);

/**
 * Add the CVRF model to be evaluated to the Session structure
 * @memberof cvrf_session
 * @param session CVRF session structure
 * @param model CVRF model to be added to the CVRF session structure
 */
void cvrf_session_set_model(struct cvrf_session *session, struct cvrf_model *model);

/**
 * Add the CVRF index to be evaluated to the Session structure
 * @memberof cvrf_session
 * @param session CVRF session structure
 * @param index CVRF index to be added to the CVRF session structure
 * @return true on success
 */
bool cvrf_session_set_index(struct cvrf_session *session, struct cvrf_index *index);

/**
 * Add the CPE name for filtering of relevant ProductIDs and CVRF elements
 * @memberof cvrf_session
 * @param session CVRF session structure
 * @param os_name CPE name to be added to the Session structure
 * @return true on success
 */
bool cvrf_session_set_os_name(struct cvrf_session *session, const char *os_name);

/************************************************************************************************
 * @struct cvrf_rpm_attributes
 * Stores dissected information about RPM package names from CVRF files- the EVR format and
 * package name. These are used to check the vulnerability of the system by comparing the
 * EVR in the CVRF file to the EVR on the system
 */
struct cvrf_rpm_attributes;

/**
 * Create a new CVRF RPM attributes structure
 * @memberof cvrf_rpm_attributes
 * @return New CVRF RPM attributes structure
 */
struct cvrf_rpm_attributes *cvrf_rpm_attributes_new(void);

/**
 * Deallocate memory for the CVRF RPM attributes structure
 * @memberof cvrf_rpm_attributes
 * @param attributes CVRF RPM attributes structure to be freed
 */
void cvrf_rpm_attributes_free(struct cvrf_rpm_attributes *attributes);

/**
 * @memberof cvrf_rpm_attributes
 * @param attributes CVRF RPM Attributes structure
 * @return Entire RPM package name
 */
const char *cvrf_rpm_attributes_get_full_package_name(const struct cvrf_rpm_attributes *attributes);

/**
 * Used to check if the RPM file exists on the system during evaluation
 * @memberof cvrf_rpm_attributes
 * @param attributes CVRF RPM Attributes structure
 * @return RPM package name (without release or version)
 */
const char *cvrf_rpm_attributes_get_rpm_name(const struct cvrf_rpm_attributes *attributes);

/**
 * Used to check if the system is vulnerable by comparing EVR from the system to EVR in the
 * CVRF file
 * @memberof cvrf_rpm_attributes
 * @param attributes CVRF RPM Attributes structure
 * @return EVR format of the RPM package
 */
const char *cvrf_rpm_attributes_get_evr_format(const struct cvrf_rpm_attributes *attributes);

/**
 * @memberof cvrf_rpm_attributes
 * @param attributes CVRF RPM Attributes structure
 * @param full_package Full length RPM package name
 * @return true on success
 */
bool cvrf_rpm_attributes_set_full_package_name(struct cvrf_rpm_attributes *attributes, const char *full_package);

/**
 * @memberof cvrf_rpm_attributes
 * @param attributes CVRF RPM Attributes structure
 * @param rpm_name Package name (without version or release) of an RPM package
 * @return true on success
 */
bool cvrf_rpm_attributes_set_rpm_name(struct cvrf_rpm_attributes *attributes, const char *rpm_name);

/**
 * @memberof cvrf_rpm_attributes
 * @param attributes CVRF RPM Attributes structure
 * @param evr_format String representation of the EVR of an RPM package
 * @return true on success
 */
bool cvrf_rpm_attributes_set_evr_format(struct cvrf_rpm_attributes *attributes, const char *evr_format);


/************************************************************************************************/
/************************************************************************************************/


/**
 * Get supported version of CVRF XML
 * @return version of XML file format
 * @memberof cvrf_model
 */
const char * cvrf_model_supported(void);

/**
 * Parses specified text index file and parses each filename in the list
 * into a CVRF model contained in the CVRF index structure
 * @memberof cvrf_index
 * @param index_source OSCAP source with path to CVRF file
 * @return New CVRF index containing all CVRF models
 */
struct cvrf_index *cvrf_index_import(struct oscap_source *index_source);

/**
 * Parses the specified XML file and creates a list of CVRF data structures.
 * @memberof cvrf_model
 * @param source OSCAP source with path to CVRF file
 * @return New CVRF model structure
 */
struct cvrf_model *cvrf_model_import(struct oscap_source *source);

/**
 * Export CVRF Index to the export source as an XML doc
 * @memberof cvrf_index
 * @param index CVRF index structure
 * @return Export target source for the Index, NULL on failure
 */
struct oscap_source *cvrf_index_get_export_source(struct cvrf_index *index);

/**
 * Export CVRF Model to the export source as an XML doc
 * @memberof cvrf_model
 * @param model CVRF Model structure
 * @return Export target source for the Model, NULL on failure
 */
struct oscap_source *cvrf_model_get_export_source(struct cvrf_model *model);

/**
 * Import and parse the CVRF Model from the provided source, filter it by CPE to find
 * appropriate packages, check these for vulnerabilities, and generate an XML document of the results
 * @memberof cvrf_session
 * @param import_source OSCAP source used to import the CVRF Model into the session
 * @param os_name CPE name used to find relevant RPM packages to check for vulnerabilities
 * @return OSCAP source export target for the results XML file
 */
struct oscap_source *cvrf_model_get_results_source(struct oscap_source *import_source, const char *os_name);

/**
 * Import and parse the CVRF Index from the provided source, filter it by CPE to find
 * appropriate packages, check these for vulnerabilities, and generate an XML document of the results
 * @memberof cvrf_session
 * @param import_source OSCAP source used to import the CVRF Index into the session
 * @param os_name CPE name used to find relevant RPM packages to check for vulnerabilities
 * @return OSCAP source export target for the results XML file
 */
struct oscap_source *cvrf_index_get_results_source(struct oscap_source *import_source, const char *os_name);

int cvrf_session_construct_definition_model(struct cvrf_session *session);


/**@}*/

#endif				/* _CVRF_H_ */
