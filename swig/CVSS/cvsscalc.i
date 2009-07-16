/*
 * Common Vulnerability Scoring System Calculator
 * (http://nvd.nist.gov/cvss.cfm)
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
 *      Maros Barabas   <mbarabas@redhat.com>
 *
 */

%module cvsscalc
%{
 #include "../../src/CVSS/cvsscalc.h"
%}


/* Included file: ../../src/CVSS/cvsscalc.h
 * Read cvsscalc.h for more information and 
 * documentation
 *
 */

enum cvss_access_vector {
        AV_LOCAL,            /*!< requires local access */
        AV_ADJACENT_NETWORK, /*!< adjacent network accessible */
        AV_NETWORK           /*!< network accessible */
};
enum cvss_access_complexity {
        AC_HIGH,
        AC_MEDIUM,
        AC_LOW
};
enum cvss_authentication {
        AU_NONE,             /*!< requires multiple authentication instances */
        AU_SINGLE_INSTANCE,  /*!< requires single instance of authentication */
        AU_MULTIPLE_INSTANCE /*!< requires no authentication */
};
enum cvss_conf_impact {
        CI_NONE,
        CI_PARTIAL,
        CI_COMPLETE
};
enum cvss_integ_impact {
        II_NONE,
        II_PARTIAL,
        II_COMPLETE
};
enum cvss_avail_impact {
        AI_NONE,
        AI_PARTIAL,
        AI_COMPLETE
};
enum cvss_exploitability {
        EX_UNPROVEN,
        EX_PROOF_OF_CONCEPT,
        EX_FUNCTIONAL,
        EX_HIGH,
        EX_NOT_DEFINED
};
enum cvss_remediation_level {
        RL_OFFICIAL_FIX,
        RL_TEMPORARY_FIX,
        RL_WORKAROUND,
        RL_UNAVAILABLE,
        RL_NOT_DEFINED
};
enum cvss_report_confidence {
        RC_UNCONFIRMED,
        RC_UNCORROBORATED,
        RC_CONFIRMED,
        RC_NOT_DEFINED
};
enum cvss_collateral_damage_potential {
        CD_NONE,
        CD_LOW,
        CD_LOW_MEDIUM,
        CD_MEDIUM_HIGH,
        CD_HIGH,
        CD_NOT_DEFINED
};
enum cvss_target_distribution {
        TD_NONE,
        TD_LOW,
        TD_MEDIUM,
        TD_HIGH,
        TD_NOT_DEFINED
};
enum cvss_conf_req {
        CR_LOW,
        CR_MEDIUM,
        CR_HIGH,
        CR_NOT_DEFINED
};
enum cvss_integ_req {
        IR_LOW,
        IR_MEDIUM,
        IR_HIGH,
        IR_NOT_DEFINED
};
enum cvss_avail_req {
        AR_LOW,
        AR_MEDIUM,
        AR_HIGH,
        AR_NOT_DEFINED
};
int cvss_base_score(enum cvss_access_vector ave, enum cvss_access_complexity ace, enum cvss_authentication aue,
                    enum cvss_conf_impact   cie, enum cvss_integ_impact      iie, enum cvss_avail_impact   aie, 
                    double *base_score, 
                    double *impact_score, 
                    double *exploitability_score);
int cvss_temp_score(enum cvss_exploitability    exe, enum cvss_remediation_level rle, 
                    enum cvss_report_confidence rce, double             base_score, 
                    double             *temporal_score);
int cvss_env_score(enum cvss_collateral_damage_potential cde, enum cvss_target_distribution   tde, 
                   enum cvss_conf_req                    cre, enum cvss_integ_req             ire, 
                   enum cvss_avail_req                   are, double *enviromental_score,
                   enum cvss_access_vector               ave, enum cvss_access_complexity     ace, 
                   enum cvss_authentication              aue, enum cvss_conf_impact           cie, 
                   enum cvss_integ_impact                iie, enum cvss_avail_impact          aie, 
                   enum cvss_exploitability              exe, enum cvss_remediation_level     rle, 
                   enum cvss_report_confidence           rce);
/* End cvsscalc.h */
