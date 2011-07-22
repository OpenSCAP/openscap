/** 
 * @addtogroup CVSS
 * @{
 *
 * @file cvss.h
 *  \brief Interface to Common Vulnerability Scoring System Version 2
 *
 */

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
 */


#ifndef _CVSSCALC_H_
#define _CVSSCALC_H_

#include <stdbool.h>
#include <time.h>

/** 
 * @struct cvss_entry
 * Structure holding CVSS model
 */
struct cvss_entry;

/************************************************************/
/**
 * @name Getters
 * Return value is pointer to structure's member. Do not free unless you null the pointer in the structure. 
 * Use remove function otherwise.
 * @{
 * */

/**
 * Get score from CVSS entry
 * @param entry CVSS entry
 * @memberof cvss_entry
 * @return CVSS score
 */
const char* cvss_entry_get_score(const struct cvss_entry * entry);
/**
 * Get access vector from CVSS entry
 * @param entry CVSS entry
 * @memberof cvss_entry
 * @return CVSS access vector
 */
const char* cvss_entry_get_AV(const struct cvss_entry * entry);
/**
 * Get access complexity from CVSS entry
 * @param entry CVSS entry
 * @memberof cvss_entry
 * @return CVSS access complexity
 */
const char* cvss_entry_get_AC(const struct cvss_entry * entry);
/**
 * Get authentication from CVSS entry
 * @param entry CVSS entry
 * @memberof cvss_entry
 * @return CVSS authentication
 */
const char* cvss_entry_get_authentication(const struct cvss_entry * entry);
/**
 * Get confidentiality impact from CVSS entry
 * @param entry CVSS entry
 * @memberof cvss_entry
 * @return CVSS conidentiality impact
 */
const char* cvss_entry_get_imp_confidentiality(const struct cvss_entry * entry);
/**
 * Get integrity impact from CVSS entry
 * @param entry CVSS entry
 * @memberof cvss_entry
 * @return CVSS integrity impact
 */
const char* cvss_entry_get_imp_integrity(const struct cvss_entry * entry);
/**
 * Get availability impact from CVSS entry
 * @param entry CVSS entry
 * @memberof cvss_entry
 * @return CVSS availability impact
 */
const char* cvss_entry_get_imp_availability(const struct cvss_entry * entry);
/**
 * Get source from CVSS entry
 * @param entry CVSS entry
 * @memberof cvss_entry
 * @return CVSS source
 */
const char* cvss_entry_get_source(const struct cvss_entry * entry);
/**
 * Get generated date-time from CVSS entry
 * @param entry CVSS entry
 * @memberof cvss_entry
 * @return CVSS generated date-time
 */
const char* cvss_entry_get_generated(const struct cvss_entry * entry);

/************************************************************/
/** @} End of Getters group */

/************************************************************/
/**
 * @name Setters
 * For lists use add functions. Parameters of set functions are duplicated in memory and need to 
 * be freed by caller.
 * @{
 */

/**
 * Set of CVSS entry
 * @param entry CVSS entry
 * @param new_ new of CVSS
 * @memberof cvss_entry
 * @return true if set, false otherwise
 */
bool cvss_entry_set_score(struct cvss_entry *entry, const char *new_score);
/**
 * Set access vector of CVSS entry
 * @param entry CVSS entry
 * @param new_AV new access vector of CVSS
 * @memberof cvss_entry
 * @return true if set, false otherwise
 */
bool cvss_entry_set_AV(struct cvss_entry *entry, const char *new_AV);
/**
 * Set access complexity of CVSS entry
 * @param entry CVSS entry
 * @param new_AC new access complexity of CVSS
 * @memberof cvss_entry
 * @return true if set, false otherwise
 */
bool cvss_entry_set_AC(struct cvss_entry *entry, const char *new_AC);
/**
 * Set authentication of CVSS entry
 * @param entry CVSS entry
 * @param new_authentication new authentication of CVSS
 * @memberof cvss_entry
 * @return true if set, false otherwise
 */
bool cvss_entry_set_authentication(struct cvss_entry *entry, const char *new_authentication);
/**
 * Set confidentiality impact of CVSS entry
 * @param entry CVSS entry
 * @param new_item new impact of CVSS
 * @memberof cvss_entry
 * @return true if set, false otherwise
 */
bool cvss_entry_set_imp_confidentiality(struct cvss_entry *entry, const char *new_item);
/**
 * Set integrity impact of CVSS entry
 * @param entry CVSS entry
 * @param new_item new impact of CVSS
 * @memberof cvss_entry
 * @return true if set, false otherwise
 */
bool cvss_entry_set_imp_integrity(struct cvss_entry *entry, const char *new_item);
/**
 * Set availability impact of CVSS entry
 * @param entry CVSS entry
 * @param new_item new impact of CVSS
 * @memberof cvss_entry
 * @return true if set, false otherwise
 */
bool cvss_entry_set_imp_availability(struct cvss_entry *entry, const char *new_item);
/**
 * Set source of CVSS entry
 * @param entry CVSS entry
 * @param new_ new of CVSS
 * @memberof cvss_entry
 * @return true if set, false otherwise
 */
bool cvss_entry_set_source(struct cvss_entry *entry, const char *new_source);
/**
 * Set generated date-time of CVSS entry
 * @param entry CVSS entry
 * @param new_generated new generated date-time of CVSS
 * @memberof cvss_entry
 * @return true if set, false otherwise
 */
bool cvss_entry_set_generated(struct cvss_entry *entry, const char *new_generated);

/************************************************************/
/** @} End of Setters group */

/**
 * New CVSS entry
 * @memberof cvss_entry
 * @return new cvss_entry
 */
struct cvss_entry * cvss_entry_new(void);

/**
 * Clone CVSS entry
 * @param old_entry CVSS entry
 * @memberof cvss_entry
 */
struct cvss_entry * cvss_entry_clone(struct cvss_entry * old_entry);

/**
 * Free CVSS entry
 * @param entry CVSS entry
 * @memberof cvss_entry
 */
void cvss_entry_free(struct cvss_entry * entry);

//! Access Vector
/*! This metric reflects how the vulnerability is exploited. */
typedef enum {
	AV_LOCAL,            /*!< requires local access */
	AV_ADJACENT_NETWORK, /*!< adjacent network accessible */
	AV_NETWORK           /*!< network accessible */
} cvss_access_vector_t;

//! Access Complexity
/*! This metric measures the complexity of the attack required to exploit the 
 *  vulnerability once an attacker has gained access to the target system. 
 */
typedef enum {
	AC_HIGH,
	AC_MEDIUM,
	AC_LOW
} cvss_access_complexity_t;

//! Authentication 
/*! This metric measures the complexity of the attack required to exploit 
 *  the vulnerability once an attacker has gained access to the target system. 
 */
typedef enum {
	AU_NONE,             /*!< requires multiple authentication instances */
	AU_SINGLE_INSTANCE,  /*!< requires single instance of authentication */
	AU_MULTIPLE_INSTANCE /*!< requires no authentication */
} cvss_authentication_t;

//! Confidentiality Impact
/*! This metric measures the impact on confidentiality of a successfully 
 *  exploited vulnerability.
 */
typedef enum {
	CI_NONE,
	CI_PARTIAL,
	CI_COMPLETE
} cvss_conf_impact_t;

//! Integrity Impact
/*! This metric measures the impact to integrity of a successfully exploited 
 *  vulnerability.
 */
typedef enum {
	II_NONE,
	II_PARTIAL,
	II_COMPLETE
} cvss_integ_impact_t;

//! Availability Impact
/*! This metric measures the impact to availability of a successfully 
 *  exploited vulnerability. 
 */
typedef enum {
	AI_NONE,
	AI_PARTIAL,
	AI_COMPLETE
} cvss_avail_impact_t;

//! Exploitability
/*! This metric measures the current state of exploit techniques or code 
 *  availability. 
 */
typedef enum {
	EX_UNPROVEN,
	EX_PROOF_OF_CONCEPT,
	EX_FUNCTIONAL,
	EX_HIGH,
	EX_NOT_DEFINED
} cvss_exploitability_t;

//! Remediation Level
/*! The remediation level of a vulnerability is an important factor for 
 *  prioritization.
 */
typedef enum {
	RL_OFFICIAL_FIX,
	RL_TEMPORARY_FIX,
	RL_WORKAROUND,
	RL_UNAVAILABLE,
	RL_NOT_DEFINED
} cvss_remediation_level_t;

//! Report Confidence
/*! This metric measures the degree of confidence in the existence 
 *  of the vulnerability and the credibility of the known 
 *  technical details. 
 */
typedef enum {
	RC_UNCONFIRMED,
	RC_UNCORROBORATED,
	RC_CONFIRMED,
	RC_NOT_DEFINED
} cvss_report_confidence_t;

//! Collateral Damage Potential 
/*! This metric measures the potential for loss of life or physical assets 
 *  through damage or theft of property or equipment. 
 */
typedef enum {
	CD_NONE,
	CD_LOW,
	CD_LOW_MEDIUM,
	CD_MEDIUM_HIGH,
	CD_HIGH,
	CD_NOT_DEFINED
} cvss_collateral_damage_potential_t;

//! Target Distribution 
/*! This metric measures the proportion of vulnerable systems. It is meant 
 *  as an environment-specific indicator in order to approximate the percentage 
 *  of systems that could be affected by the vulnerability.
 */
typedef enum {
	TD_NONE,
	TD_LOW,
	TD_MEDIUM,
	TD_HIGH,
	TD_NOT_DEFINED
} cvss_target_distribution_t;

//! Confidentiality Requirement
/*! This metric enable the analyst to customize the CVSS score depending on 
 *  the importance of the affected IT asset to a user’s organization, 
 *  measured in term of confidentiality.
 */
typedef enum {
	CR_LOW,
	CR_MEDIUM,
	CR_HIGH,
	CR_NOT_DEFINED
} cvss_conf_req_t;

//! Integrity Requirement
/*! This metric enable the analyst to customize the CVSS score depending on 
 *  the importance of the affected IT asset to a user’s organization, 
 *  measured in term of integrity.
 */
typedef enum {
	IR_LOW,
	IR_MEDIUM,
	IR_HIGH,
	IR_NOT_DEFINED
} cvss_integ_req_t;

//! Availabublity Requirement
/*! This metric enable the analyst to customize the CVSS score depending on 
 *  the importance of the affected IT asset to a user’s organization, 
 *  measured in term of availability.
 */
typedef enum {
	AR_LOW,
	AR_MEDIUM,
	AR_HIGH,
	AR_NOT_DEFINED
} cvss_avail_req_t;


/************************************************************/
/**
 * @name Evaluators
 * @{
 * */


/*! \fn cvss_base_score(cvss_access_vector_t ave, cvss_access_complexity_t ace, cvss_authentication_t aue,
 *                      cvss_conf_impact_t   cie, cvss_integ_impact_t      iie, cvss_avail_impact_t    aie,
 *                      double *base_score,
 *                      double *impact_score,
 *                      double *exploitability_score)
 */
int cvss_base_score(cvss_access_vector_t ave, cvss_access_complexity_t ace, cvss_authentication_t aue,
                    cvss_conf_impact_t   cie, cvss_integ_impact_t      iie, cvss_avail_impact_t   aie, 
                    double *base_score, 
                    double *impact_score, 
                    double *exploitability_score);

/*! \fn cvss_temp_score(cvss_exploitability_t   exe, cvss_remediation_level_t rle, 
 *                      cvss_report_confidence_t rce, double             base_score, 
 *                      double             *temporal_score);
 */
int cvss_temp_score(cvss_exploitability_t    exe, cvss_remediation_level_t rle, 
                    cvss_report_confidence_t rce, double             base_score, 
                    double             *temporal_score);

/*! \fn cvss_env_score(cvss_collateral_damage_potential_t cde, cvss_target_distribution_t   tde, 
 *                     cvss_conf_req_t                    cre, cvss_integ_req_t             ire, 
 *                     cvss_avail_req_t                   are, cvss_access_vector_t         ave, 
 *                     cvss_access_complexity_t           ace, cvss_authentication_t        aue, 
 *                     cvss_conf_impact_t                 cie, cvss_integ_impact_t          iie, 
 *                     cvss_avail_impact_t                aie, cvss_exploitability_t        exe, 
 *                     cvss_remediation_level_t           rle, cvss_report_confidence_t           rce);
 *                     double *enviromental_score
 */
int cvss_env_score(cvss_collateral_damage_potential_t cde, cvss_target_distribution_t   tde, 
                   cvss_conf_req_t                    cre, cvss_integ_req_t             ire, 
                   cvss_avail_req_t                   are, cvss_access_vector_t         ave, 
		   cvss_access_complexity_t           ace, cvss_authentication_t        aue, 
		   cvss_conf_impact_t                 cie, cvss_integ_impact_t          iie, 
		   cvss_avail_impact_t                aie, cvss_exploitability_t        exe, 
		   cvss_remediation_level_t           rle, cvss_report_confidence_t     rce, 
		   double *enviromental_score);
/**
 * Get Base score of CVSS entry
 * @return 0 if everything done
 */
int cvss_base_score_struct(const struct cvss_entry * entry,
                           double *base_score,
                           double *impact_score,
                           double *exploitability_score);

/**
 * Get supported version of CVSS XML
 * @return version of XML file format
 * @memberof cvss_entry
 */
const char * cvss_model_supported(void);

/************************************************************/
/** @} End of Evaluators group */



/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////  NEW API  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

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


/// @memberof cvss_impact
struct cvss_impact *cvss_impact_new(void);
/// @memberof cvss_impact
struct cvss_impact *cvss_impact_new_from_vector(const char *cvss_vector);
/// @memberof cvss_impact
//struct cvss_impact *cvss_impact_new_from_xml(const char *filename);
/// @memberof cvss_impact
void cvss_impact_free(struct cvss_impact* impact);

/// @memberof cvss_impact
struct cvss_metrics *cvss_impact_get_base_metrics(const struct cvss_impact* impact);
/// @memberof cvss_impact
struct cvss_metrics *cvss_impact_get_temporal_metrics(const struct cvss_impact* impact);
/// @memberof cvss_impact
struct cvss_metrics *cvss_impact_get_environmental_metrics(const struct cvss_impact* impact);
/// Set base, temporal, or environmental metrics (type is determined from the metrics itself)
/// @memberof cvss_impact
bool cvss_impact_set_metrics(struct cvss_impact* impact, struct cvss_metrics *metrics);
/// @memberof cvss_impact
char *cvss_impact_to_vector(const struct cvss_impact* impact);


/// @memberof cvss_metrics
struct cvss_metrics *cvss_metrics_new(enum cvss_category category);
/// @memberof cvss_metrics
void cvss_metrics_free(struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_category cvss_metrics_get_category(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
const char *cvss_metrics_get_source(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
bool cvss_metrics_set_source(struct cvss_metrics* metrics, const char *new_source);
/// @memberof cvss_metrics
const char *cvss_metrics_get_generated_on_datetime(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
bool cvss_metrics_set_generated_on_datetime(struct cvss_metrics* metrics, const char *new_datetime);
/// @memberof cvss_metrics
const char *cvss_metrics_get_upgraded_from_version(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
bool cvss_metrics_set_upgraded_from_version(struct cvss_metrics* metrics, const char *new_upgraded_from_version);
/// @memberof cvss_metrics
float cvss_metrics_get_score(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
bool cvss_metrics_set_score(struct cvss_metrics* metrics, float score);

/**
 * @name Vector values
 * Sunctions to get or set individual CVSS vector values.
 * Functions check for correct type of metrics (base/temporal/environmental).
 * Setters return false and getters undefined/default value when attempted
 * to query wrong type of metrics.
 * @todo Getters/setters for the "approximated" flag
 * @{
 */

/// @memberof cvss_metrics
enum cvss_access_vector cvss_metrics_get_access_vector(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_access_complexity cvss_metrics_get_access_complexity(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_authentication cvss_metrics_get_authentication(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_cia_impact cvss_metrics_get_confidentiality_impact(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_cia_impact cvss_metrics_get_integrity_impact(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_cia_impact cvss_metrics_get_availability_impact(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_exploitability cvss_metrics_get_exploitability(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_remediation_level cvss_metrics_get_remediation_level(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_report_confidence cvss_metrics_get_report_confidence(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_collateral_damage_potential cvss_metrics_get_collateral_damage_potential(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_target_distribution cvss_metrics_get_target_distribution(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_cia_requirement cvss_metrics_get_confidentiality_requirement(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_cia_requirement cvss_metrics_get_integrity_requirement(const struct cvss_metrics* metrics);
/// @memberof cvss_metrics
enum cvss_cia_requirement cvss_metrics_get_availability_requirement(const struct cvss_metrics* metrics);

/// @memberof cvss_metrics
bool cvss_metrics_set_access_vector(struct cvss_metrics* metrics, enum cvss_access_vector);
/// @memberof cvss_metrics
bool cvss_metrics_set_access_complexity(struct cvss_metrics* metrics, enum cvss_access_complexity);
/// @memberof cvss_metrics
bool cvss_metrics_set_authentication(struct cvss_metrics* metrics, enum cvss_authentication);
/// @memberof cvss_metrics
bool cvss_metrics_set_confidentiality_impact(struct cvss_metrics* metrics, enum cvss_cia_impact);
/// @memberof cvss_metrics
bool cvss_metrics_set_integrity_impact(struct cvss_metrics* metrics, enum cvss_cia_impact);
/// @memberof cvss_metrics
bool cvss_metrics_set_availability_impact(struct cvss_metrics* metrics, enum cvss_cia_impact);
/// @memberof cvss_metrics
bool cvss_metrics_set_exploitability(struct cvss_metrics* metrics, enum cvss_exploitability);
/// @memberof cvss_metrics
bool cvss_metrics_set_remediation_level(struct cvss_metrics* metrics, enum cvss_remediation_level);
/// @memberof cvss_metrics
bool cvss_metrics_set_report_confidence(struct cvss_metrics* metrics, enum cvss_report_confidence);
/// @memberof cvss_metrics
bool cvss_metrics_set_collateral_damage_potential(struct cvss_metrics* metrics, enum cvss_collateral_damage_potential);
/// @memberof cvss_metrics
bool cvss_metrics_set_target_distribution(struct cvss_metrics* metrics, enum cvss_target_distribution);
/// @memberof cvss_metrics
bool cvss_metrics_set_confidentiality_requirement(struct cvss_metrics* metrics, enum cvss_cia_requirement);
/// @memberof cvss_metrics
bool cvss_metrics_set_integrity_requirement(struct cvss_metrics* metrics, enum cvss_cia_requirement);
/// @memberof cvss_metrics
bool cvss_metrics_set_availability_requirement(struct cvss_metrics* metrics, enum cvss_cia_requirement);

/*
access_vector access_vector
access_complexity access_complexity
authentication authentication
confidentiality_impact cia_impact
integrity_impact cia_impact
availability_impact cia_impact
exploitability exploitability
remediation_level remediation_level
report_confidence report_confidence
collateral_damage_potential collateral_damage_potential
target_distribution target_distribution
confidentiality_requirement cia_requirement
integrity_requirement cia_requirement
availability_requirement cia_requirement
*/

/** @} */

/*@}*/
#endif /* _CVSSCALC_H_ */
