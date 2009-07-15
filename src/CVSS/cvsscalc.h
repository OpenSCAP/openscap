/*! 
 *  \addtogroup CVSS
 *  \{
 *
 *  \file cvsscalc.h
 *  \brief Interface to Common Vulnerability Scoring System Version 2
 *  
 *  See details at http://nvd.nist.gov/cvss.cfm
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
 *      Peter Vrabec   <pvrabec@redhat.com>
 *      Brandon Dixon  <Brandon.Dixon@g2-inc.com>
 */


#ifndef _CVSSCALC_H_
#define _CVSSCALC_H_

//! Access Vector
/*! This metric reflects how the vulnerability is exploited. */
enum cvss_access_vector {
	AV_LOCAL,            /*!< requires local access */
	AV_ADJACENT_NETWORK, /*!< adjacent network accessible */
	AV_NETWORK           /*!< network accessible */
};

//! Access Complexity
/*! This metric measures the complexity of the attack required to exploit the 
 *  vulnerability once an attacker has gained access to the target system. 
 */
enum cvss_access_complexity {
	AC_HIGH,
	AC_MEDIUM,
	AC_LOW
};

//! Authentication 
/*! This metric measures the complexity of the attack required to exploit 
 *  the vulnerability once an attacker has gained access to the target system. 
 */
enum cvss_authentication {
	AU_NONE,             /*!< requires multiple authentication instances */
	AU_SINGLE_INSTANCE,  /*!< requires single instance of authentication */
	AU_MULTIPLE_INSTANCE /*!< requires no authentication */
};

//! Confidentiality Impact
/*! This metric measures the impact on confidentiality of a successfully 
 *  exploited vulnerability.
 */
enum cvss_conf_impact {
	CI_NONE,
	CI_PARTIAL,
	CI_COMPLETE
};

//! Integrity Impact
/*! This metric measures the impact to integrity of a successfully exploited 
 *  vulnerability.
 */
enum cvss_integ_impact {
	II_NONE,
	II_PARTIAL,
	II_COMPLETE
};

//! Availability Impact
/*! This metric measures the impact to availability of a successfully 
 *  exploited vulnerability. 
 */
enum cvss_avail_impact {
	AI_NONE,
	AI_PARTIAL,
	AI_COMPLETE
};

//! Exploitability
/*! This metric measures the current state of exploit techniques or code 
 *  availability. 
 */
enum cvss_exploitability {
	EX_UNPROVEN,
	EX_PROOF_OF_CONCEPT,
	EX_FUNCTIONAL,
	EX_HIGH,
	EX_NOT_DEFINED
};

//! Remediation Level
/*! The remediation level of a vulnerability is an important factor for 
 *  prioritization.
 */
enum cvss_remediation_level {
	RL_OFFICIAL_FIX,
	RL_TEMPORARY_FIX,
	RL_WORKAROUND,
	RL_UNAVAILABLE,
	RL_NOT_DEFINED
};

//! Report Confidence
/*! This metric measures the degree of confidence in the existence 
 *  of the vulnerability and the credibility of the known 
 *  technical details. 
 */
enum cvss_report_confidence {
	RC_UNCONFIRMED,
	RC_UNCORROBORATED,
	RC_CONFIRMED,
	RC_NOT_DEFINED
};

//! Collateral Damage Potential 
/*! This metric measures the potential for loss of life or physical assets 
 *  through damage or theft of property or equipment. 
 */
enum cvss_collateral_damage_potential {
	CD_NONE,
	CD_LOW,
	CD_LOW_MEDIUM,
	CD_MEDIUM_HIGH,
	CD_HIGH,
	CD_NOT_DEFINED
};

//! Target Distribution 
/*! This metric measures the proportion of vulnerable systems. It is meant 
 *  as an environment-specific indicator in order to approximate the percentage 
 *  of systems that could be affected by the vulnerability.
 */
enum cvss_target_distribution {
	TD_NONE,
	TD_LOW,
	TD_MEDIUM,
	TD_HIGH,
	TD_NOT_DEFINED
};

//! Confidentiality Requirement
/*! This metric enable the analyst to customize the CVSS score depending on 
 *  the importance of the affected IT asset to a user’s organization, 
 *  measured in term of confidentiality.
 */
enum cvss_conf_req {
	CR_LOW,
	CR_MEDIUM,
	CR_HIGH,
	CR_NOT_DEFINED
};

//! Integrity Requirement
/*! This metric enable the analyst to customize the CVSS score depending on 
 *  the importance of the affected IT asset to a user’s organization, 
 *  measured in term of integrity.
 */
enum cvss_integ_req {
	IR_LOW,
	IR_MEDIUM,
	IR_HIGH,
	IR_NOT_DEFINED
};

//! Availabublity Requirement
/*! This metric enable the analyst to customize the CVSS score depending on 
 *  the importance of the affected IT asset to a user’s organization, 
 *  measured in term of availability.
 */
enum cvss_avail_req {
	AR_LOW,
	AR_MEDIUM,
	AR_HIGH,
	AR_NOT_DEFINED
};

/*! \fn cvss_base_score(enum cvss_access_vector ave, enum cvss_access_complexity ace, enum cvss_authentication aue,
 *                      enum cvss_conf_impact   cie, enum cvss_integ_impact      iie, enum cvss_avail_impact    aie,
 *                      double *base_score,
 *                      double *impact_score,
 *                      double *exploitability_score)
 */
int cvss_base_score(enum cvss_access_vector ave, enum cvss_access_complexity ace, enum cvss_authentication aue,
                    enum cvss_conf_impact   cie, enum cvss_integ_impact      iie, enum cvss_avail_impact   aie, 
                    double *base_score, 
                    double *impact_score, 
                    double *exploitability_score);

/*! \fn cvss_temp_score(enum cvss_exploitability   exe, enum cvss_remediation_level rle, 
 *                      enum cvss_report_confidence rce, double             base_score, 
 *                      double             *temporal_score);
 */
int cvss_temp_score(enum cvss_exploitability    exe, enum cvss_remediation_level rle, 
                    enum cvss_report_confidence rce, double             base_score, 
                    double             *temporal_score);

/*! \fn cvss_env_score(enum cvss_collateral_damage_potential cde, enum cvss_target_distribution   tde, 
 *                     enum cvss_conf_req                    cre, enum cvss_integ_req             ire, 
 *                     enum cvss_avail_req                   are, double *enviromental_score,
 *	                   enum cvss_access_vector               ave, enum cvss_access_complexity     ace, 
 *                     enum cvss_authentication              aue, enum cvss_conf_impact           cie, 
 *                     enum cvss_integ_impact                iie, enum cvss_avail_impact          aie,	     
 *                     enum cvss_exploitability              exe, enum cvss_remediation_level     rle, 
 *                     enum cvss_report_confidence           rce);
 */
int cvss_env_score(enum cvss_collateral_damage_potential cde, enum cvss_target_distribution   tde, 
                   enum cvss_conf_req                    cre, enum cvss_integ_req             ire, 
                   enum cvss_avail_req                   are, double *enviromental_score,
                   enum cvss_access_vector               ave, enum cvss_access_complexity     ace, 
                   enum cvss_authentication              aue, enum cvss_conf_impact           cie, 
                   enum cvss_integ_impact                iie, enum cvss_avail_impact          aie,	     
                   enum cvss_exploitability              exe, enum cvss_remediation_level     rle, 
                   enum cvss_report_confidence           rce);

#endif /* _CVSSCALC_H_ */
