/*
 * Copyright 2008-2009 Red Hat Inc., Durham, North Carolina.
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
 *      Peter Vrabec   <pvrabec@redhat.com>
 *      Brandon Dixon  <Brandon.Dixon@g2-inc.com>
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */
/**
 * @addtogroup CVSS
 * @{
 *
 * @file cvss_score.h
 *  \brief Interface to Common Vulnerability Scoring System Version 2
 *
 */


#ifndef _CVSSCALC_H_
#define _CVSSCALC_H_

#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include "oscap_export.h"


/// Get supported version of CVSS XML
OSCAP_API const char *cvss_model_supported(void);

/// CVSS score category
enum cvss_category {
    CVSS_NONE          = 0x0000,
    CVSS_BASE          = 0x0100,
    CVSS_TEMPORAL      = 0x0200,
    CVSS_ENVIRONMENTAL = 0x0300,
};

/// CVSS access vector
enum cvss_access_vector {
    CVSS_AV_NOT_SET,
    CVSS_AV_LOCAL,
    CVSS_AV_ADJACENT_NETWORK,
    CVSS_AV_NETWORK,
    CVSS_AV_END_
};

/// CVSS access complexity
enum cvss_access_complexity {
    CVSS_AC_NOT_SET,
    CVSS_AC_HIGH,
    CVSS_AC_MEDIUM,
    CVSS_AC_LOW,
    CVSS_AC_END_
};

/// CVSS Authentication
enum cvss_authentication {
    CVSS_AU_NOT_SET,
    CVSS_AU_MULTIPLE,
    CVSS_AU_SINGLE,
    CVSS_AU_NONE,
    CVSS_AU_END_
};

/// CVSS Confidentiality/Integrity/Availibility impact
enum cvss_cia_impact {
    CVSS_IMP_NOT_SET,
    CVSS_IMP_NONE,
    CVSS_IMP_PARTIAL,
    CVSS_IMP_COMPLETE,
    CVSS_IMP_END_
};

/// CVSS Exploitability
enum cvss_exploitability {
    CVSS_E_NOT_DEFINED,
    CVSS_E_UNPROVEN,
    CVSS_E_PROOF_OF_CONCEPT,
    CVSS_E_FUNCTIONAL,
    CVSS_E_HIGH,
    CVSS_E_END_
};

/// CVSS Remediation Level
enum cvss_remediation_level {
    CVSS_RL_NOT_DEFINED,
    CVSS_RL_OFFICIAL_FIX,
    CVSS_RL_TEMPORARY_FIX,
    CVSS_RL_WORKAROUND,
    CVSS_RL_UNAVAILABLE,
    CVSS_RL_END_
};

/// CVSS Report Confidence
enum cvss_report_confidence {
    CVSS_RC_NOT_DEFINED,
    CVSS_RC_UNCONFIRMED,
    CVSS_RC_UNCORROBORATED,
    CVSS_RC_CONFIRMED,
    CVSS_RC_END_
};

/// CVSS Collateral Damage Potential
enum cvss_collateral_damage_potential {
    CVSS_CDP_NOT_DEFINED,
    CVSS_CDP_NONE,
    CVSS_CDP_LOW,
    CVSS_CDP_LOW_MEDIUM,
    CVSS_CDP_MEDIUM_HIGH,
    CVSS_CDP_HIGH,
    CVSS_CDP_END_
};

/// CVSS Target Distribution
enum cvss_target_distribution {
    CVSS_TD_NOT_DEFINED,
    CVSS_TD_NONE,
    CVSS_TD_LOW,
    CVSS_TD_MEDIUM,
    CVSS_TD_HIGH,
    CVSS_TD_END_
};

/// CVSS Confidentiality/Integrity/Availibility requirement
enum cvss_cia_requirement {
    CVSS_REQ_NOT_DEFINED,
    CVSS_REQ_LOW,
    CVSS_REQ_MEDIUM,
    CVSS_REQ_HIGH,
    CVSS_REQ_END_
};

/**
 * @struct cvss_impact
 * CVSS impact
 *
 * Contains a base metric and optionally temporal and/or environmental metric.
 */
struct cvss_impact;

/**
 * @struct cvss_metrics
 * CVSS metrics
 *
 * base, temporal or environmental metrics
 */
struct cvss_metrics;

/// Round @a x to one decimal place as described in CVSS standard
OSCAP_API float cvss_round(float x);

/// @memberof cvss_impact
OSCAP_API struct cvss_impact *cvss_impact_new(void);
/// @memberof cvss_impact
OSCAP_API struct cvss_impact *cvss_impact_new_from_vector(const char *cvss_vector);
/// @memberof cvss_impact
OSCAP_API struct cvss_impact *cvss_impact_clone(const struct cvss_impact* impact);
/// @memberof cvss_impact
//struct cvss_impact *cvss_impact_new_parse(const char *filename);
/// @memberof cvss_impact
OSCAP_API void cvss_impact_free(struct cvss_impact* impact);
/**
 * Write out a human-readable textual description of CVSS impact contents.
 * @param impact Impact to describe
 * @param f file handle to write the description to
 * @memberof cvss_impact
 */
OSCAP_API void cvss_impact_describe(const struct cvss_impact *impact, FILE *f);

/// @memberof cvss_impact
OSCAP_API struct cvss_metrics *cvss_impact_get_base_metrics(const struct cvss_impact* impact);
/// @memberof cvss_impact
OSCAP_API struct cvss_metrics *cvss_impact_get_temporal_metrics(const struct cvss_impact* impact);
/// @memberof cvss_impact
OSCAP_API struct cvss_metrics *cvss_impact_get_environmental_metrics(const struct cvss_impact* impact);
/// Set base, temporal, or environmental metrics (type is determined from the metrics itself)
/// @memberof cvss_impact
OSCAP_API bool cvss_impact_set_metrics(struct cvss_impact* impact, struct cvss_metrics *metrics);
/// @memberof cvss_impact
OSCAP_API char *cvss_impact_to_vector(const struct cvss_impact* impact);

/**
 * @name Score calculators
 * Functions to calculate CVSS score.
 *
 * Functions return special float value of NAN on failure.
 *
 * Particularly interesting are:
 *   - cvss_impact_base_score()
 *   - cvss_impact_temporal_score()
 *   - cvss_impact_environmental_score()
 * @{
 */

/**
 * Calculate exploitability subscore of base score.
 *
 * Requires base metrics to be set.
 * @see cvss_impact_base_score()
 * @see cvss_impact_adjusted_base_score()
 * @memberof cvss_impact
 */
OSCAP_API float cvss_impact_base_exploitability_subscore(const struct cvss_impact* impact);

/**
 * Calculate impact subscore of base score.
 *
 * Requires base metrics to be set.
 * @see cvss_impact_base_adjusted_impact_subscore()
 * @see cvss_impact_base_score()
 * @memberof cvss_impact
 */
OSCAP_API float cvss_impact_base_impact_subscore(const struct cvss_impact* impact);

/**
 * Calculate base score.
 *
 * The base metric group captures the characteristics of a vulnerability that
 * are constant with time and across user environments.
 *
 * Requires base metrics to be set.
 * @see cvss_impact_base_exploitability_subscore()
 * @see cvss_impact_base_impact_subscore()
 * @see cvss_impact_base_adjusted_impact_subscore()
 * @memberof cvss_impact
 */
OSCAP_API float cvss_impact_base_score(const struct cvss_impact* impact);

/**
 * Calculate temporal multiplier.
 *
 * Multiply base score by this number and round to one decimal place to get
 * temporal score. This function is intended to get the multiplier itself.
 * To calculate temporal score, use cvss_impact_temporal_score() or
 * cvss_impact_adjusted_temporal_score() instead.
 *
 * Requires temporal metrics to be set.
 * @see cvss_impact_temporal_score()
 * @see cvss_impact_adjusted_temporal_score()
 * @memberof cvss_impact
 */
OSCAP_API float cvss_impact_temporal_multiplier(const struct cvss_impact* impact);

/**
 * Calculate temporal score.
 *
 * Temporal metrics capture how the threat posed by a vulnerability may change over time.
 *
 * Requires base and temporal metrics to be set.
 * @see cvss_impact_adjusted_temporal_score()
 * @memberof cvss_impact
 */
OSCAP_API float cvss_impact_temporal_score(const struct cvss_impact* impact);

/**
 * Calculate impact subscore of base score adjusted to particular environment.
 *
 * Requires base and environmental metrics to be set.
 * @see cvss_impact_base_impact_subscore()
 * @see cvss_impact_adjusted_base_score()
 * @memberof cvss_impact
 */
OSCAP_API float cvss_impact_base_adjusted_impact_subscore(const struct cvss_impact* impact);

/**
 * Calculate base score adjusted to particular environment.
 *
 * Requires base and environmental metrics to be set.
 * @see cvss_impact_base_score()
 * @memberof cvss_impact
 */
OSCAP_API float cvss_impact_adjusted_base_score(const struct cvss_impact* impact);

/**
 * Calculate temporal score adjusted to particular environment.
 *
 * Requires base, temporal and environmental metrics to be set.
 * @see cvss_impact_temporal_score()
 * @memberof cvss_impact
 */
OSCAP_API float cvss_impact_adjusted_temporal_score(const struct cvss_impact* impact);

/**
 * Calculate environmental score.
 *
 * Different environments can have an immense bearing on the risk that a vulnerability poses to
 * an organization and its stakeholders. The CVSS environmental metric group captures the characteristics
 * of a vulnerability that are associated with a user’s IT environment.
 *
 * Requires base, temporal and environmental metrics to be set.
 * @see cvss_impact_adjusted_temporal_score()
 * @memberof cvss_impact
 */
OSCAP_API float cvss_impact_environmental_score(const struct cvss_impact* impact);

/** @} */

/// @memberof cvss_metrics
OSCAP_API struct cvss_metrics *cvss_metrics_new(enum cvss_category category);
/// @memberof cvss_metrics
OSCAP_API struct cvss_metrics *cvss_metrics_clone(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API void cvss_metrics_free(struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_category cvss_metrics_get_category(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API const char *cvss_metrics_get_source(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_source(struct cvss_metrics* metrics, const char *new_source);
/// @memberof cvss_metrics
OSCAP_API const char *cvss_metrics_get_generated_on_datetime(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_generated_on_datetime(struct cvss_metrics* metrics, const char *new_datetime);
/// @memberof cvss_metrics
OSCAP_API const char *cvss_metrics_get_upgraded_from_version(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_upgraded_from_version(struct cvss_metrics* metrics, const char *new_upgraded_from_version);
/// @memberof cvss_metrics
OSCAP_API float cvss_metrics_get_score(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_score(struct cvss_metrics* metrics, float score);
/**
 * Validate CVSS metrics completeness
 * @memberof cvss_metrics
 */
OSCAP_API bool cvss_metrics_is_valid(const struct cvss_metrics* metrics);

/**
 * @name Vector values
 * Functions to get or set individual CVSS vector values.
 * Functions check for correct type of metrics (base/temporal/environmental).
 * Setters return false and getters undefined/default value when attempted
 * to query wrong type of metrics.
 * @todo Getters/setters for the "approximated" flag
 * @{
 */

/// @memberof cvss_metrics
OSCAP_API enum cvss_access_vector cvss_metrics_get_access_vector(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_access_complexity cvss_metrics_get_access_complexity(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_authentication cvss_metrics_get_authentication(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_cia_impact cvss_metrics_get_confidentiality_impact(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_cia_impact cvss_metrics_get_integrity_impact(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_cia_impact cvss_metrics_get_availability_impact(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_exploitability cvss_metrics_get_exploitability(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_remediation_level cvss_metrics_get_remediation_level(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_report_confidence cvss_metrics_get_report_confidence(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_collateral_damage_potential cvss_metrics_get_collateral_damage_potential(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_target_distribution cvss_metrics_get_target_distribution(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_cia_requirement cvss_metrics_get_confidentiality_requirement(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_cia_requirement cvss_metrics_get_integrity_requirement(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
OSCAP_API enum cvss_cia_requirement cvss_metrics_get_availability_requirement(const struct cvss_metrics* metrics);

/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_access_vector(struct cvss_metrics* metrics, enum cvss_access_vector);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_access_complexity(struct cvss_metrics* metrics, enum cvss_access_complexity);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_authentication(struct cvss_metrics* metrics, enum cvss_authentication);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_confidentiality_impact(struct cvss_metrics* metrics, enum cvss_cia_impact);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_integrity_impact(struct cvss_metrics* metrics, enum cvss_cia_impact);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_availability_impact(struct cvss_metrics* metrics, enum cvss_cia_impact);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_exploitability(struct cvss_metrics* metrics, enum cvss_exploitability);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_remediation_level(struct cvss_metrics* metrics, enum cvss_remediation_level);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_report_confidence(struct cvss_metrics* metrics, enum cvss_report_confidence);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_collateral_damage_potential(struct cvss_metrics* metrics, enum cvss_collateral_damage_potential);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_target_distribution(struct cvss_metrics* metrics, enum cvss_target_distribution);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_confidentiality_requirement(struct cvss_metrics* metrics, enum cvss_cia_requirement);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_integrity_requirement(struct cvss_metrics* metrics, enum cvss_cia_requirement);
/// @memberof cvss_metrics
OSCAP_API bool cvss_metrics_set_availability_requirement(struct cvss_metrics* metrics, enum cvss_cia_requirement);


/** @} */

/**@}*/
#endif // _CVSSCALC_H_
