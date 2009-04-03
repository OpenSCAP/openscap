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


int baseScore(AccessVector_t ave, AccessComplexity_t ace, Authentication_t aue,
              ConfImpact_t   cie, IntegImpact_t      iie, AvailImpact_t    aie, 
              double *base_score, 
              double *impact_score, 
              double *exploitability_score);

int tempScore(Exploitability_t   exe, RemediationLevel_t rle, 
              ReportConfidence_t rce, double             base_score, 
              double             *temporal_score);


int envScore(CollateralDamagePotential_t cde, TargetDistribution_t   tde, 
             ConfReq_t                   cre, IntegReq_t             ire, 
             AvailReq_t                  are, double *enviromental_score,
             AccessVector_t              ave, AccessComplexity_t     ace, 
             Authentication_t            aue, ConfImpact_t           cie, 
             IntegImpact_t               iie, AvailImpact_t          aie,
             Exploitability_t            exe, RemediationLevel_t     rle, 
             ReportConfidence_t          rce);
