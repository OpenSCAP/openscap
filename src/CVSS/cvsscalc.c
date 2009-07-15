/*! \file cvsscalc.c
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

#include "config.h"

#include <stdlib.h>
#include <math.h>

#include "cvsscalc.h"


/**
 *   A test class. A more elaborate class description.
 */
static double _round(double d, double fact)
{
	return round(d / fact) * fact;
}

#define GETMAPVAL(var, arr, enm)			\
	if (enm >= (sizeof (arr) / sizeof (arr[0])))	\
		return 1;				\
	var = arr[enm]

/* Access Vector */
static const double ava[] = {
	[AV_LOCAL] = 0.395,
	[AV_ADJACENT_NETWORK] = 0.646,
	[AV_NETWORK] = 1.0
};

/* Access Complexity */
static const double aca[] = {
	[AC_HIGH] = 0.35,
	[AC_MEDIUM] = 0.61,
	[AC_LOW] = 0.71
};

/* Authentication */
static const double aua[] = {
	[AU_NONE] = 0.704,
	[AU_SINGLE_INSTANCE] = 0.56,
	[AU_MULTIPLE_INSTANCE] = 0.45
};

/* Conf Impact */
static const double cia[] = {
	[CI_NONE] = 0.0,
	[CI_PARTIAL] = 0.275,
	[CI_COMPLETE] = 0.660
};

/* Integ Impact */
static const double iia[] = {
	[II_NONE] = 0.0,
	[II_PARTIAL] = 0.275,
	[II_COMPLETE] = 0.660
};

/* Avail Impact */
static const double aia[] = {
	[AI_NONE] = 0.0,
	[AI_PARTIAL] = 0.275,
	[AI_COMPLETE] = 0.660
};

/* Exploitability */
static const double exa[] = {
	[EX_UNPROVEN] = 0.85,
	[EX_PROOF_OF_CONCEPT] = 0.90,
	[EX_FUNCTIONAL] = 0.95,
	[EX_HIGH] = 1.0,
	[EX_NOT_DEFINED] = 1.0
};

/* Remediation Level */
static const double rla[] = {
	[RL_OFFICIAL_FIX] = 0.87,
	[RL_TEMPORARY_FIX] = 0.90,
	[RL_WORKAROUND] = 0.95,
	[RL_UNAVAILABLE] = 1.0,
	[RL_NOT_DEFINED] = 1.0
};

/* Report Confidence */
static const double rca[] = {
	[RC_UNCONFIRMED] = 0.90,
	[RC_UNCORROBORATED] = 0.95,
	[RC_CONFIRMED] = 1.0,
	[RC_NOT_DEFINED] = 1.0
};

/* Collateral Damage Potential */
static const double cda[] = {
	[CD_NONE] = 0.0,
	[CD_LOW] = 0.1,
	[CD_LOW_MEDIUM] = 0.3,
	[CD_MEDIUM_HIGH] = 0.4,
	[CD_HIGH] = 0.5,
	[CD_NOT_DEFINED] = 0.0
};

/* Target Distribution */
static const double tda[] = {
	[TD_NONE] = 0.0,
	[TD_LOW] = 0.25,
	[TD_MEDIUM] = 0.75,
	[TD_HIGH] = 1.0,
	[TD_NOT_DEFINED] = 1.0
};

/* Conf Req */
static const double cra[] = {
	[CR_LOW] = 0.5,
	[CR_MEDIUM] = 1.0,
	[CR_HIGH] = 1.51,
	[CR_NOT_DEFINED] = 1.0
};

/* Integ Req */
static const double ira[] = {
	[IR_LOW] = 0.5,
	[IR_MEDIUM] = 1.0,
	[IR_HIGH] = 1.51,
	[IR_NOT_DEFINED] = 1.0
};

/* Avail Req */
static const double ara[] = {
	[AR_LOW] = 0.5,
	[AR_MEDIUM] = 1.0,
	[AR_HIGH] = 1.51,
	[AR_NOT_DEFINED] = 1.0
};

int cvss_base_score(enum cvss_access_vector ave, enum cvss_access_complexity ace, enum cvss_authentication aue, 
                    enum cvss_conf_impact   cie, enum cvss_integ_impact      iie, enum cvss_avail_impact    aie, 
                    double *base_score,
                    double *impact_score, 
                    double *exploitability_score)
{
	double av, ac, au, ci, ii, ai, bs, is, es;

	GETMAPVAL(av, ava, ave);
	GETMAPVAL(ac, aca, ace);
	GETMAPVAL(au, aua, aue);
	GETMAPVAL(ci, cia, cie);
	GETMAPVAL(ii, iia, iie);
	GETMAPVAL(ai, aia, aie);

	is = _round(10.41 * (1 - (1 - ci) * (1 - ii) * (1 - ai)), 0.1);
	es = _round(20 * av * ac * au, 0.1);
	bs = _round((0.6 * is + 0.4 * es - 1.5) * (is ? 1.176 : 0.0), 0.1);

	if (base_score != NULL)
		*base_score = bs;

	if (impact_score != NULL)
		*impact_score = is;

	if (exploitability_score != NULL)
		*exploitability_score = es;

	return 0;
}

int cvss_temp_score(enum cvss_exploitability    exe, enum cvss_remediation_level rle, 
                    enum cvss_report_confidence rce, double      base_score, 
                    double *temporal_score)
{
	double ex, rl, rc, ts;

	GETMAPVAL(ex, exa, exe);
	GETMAPVAL(rl, rla, rle);
	GETMAPVAL(rc, rca, rce);

	ts = _round(base_score * ex * rl * rc, 0.1);

	if(temporal_score != NULL)
		*temporal_score = ts;

	return 0;
}

int cvss_env_score(enum cvss_collateral_damage_potential cde, enum cvss_target_distribution   tde, 
                   enum cvss_conf_req                    cre, enum cvss_integ_req             ire, 
                   enum cvss_avail_req                   are, double *enviromental_score,
                   enum cvss_access_vector               ave, enum cvss_access_complexity     ace, 
                   enum cvss_authentication              aue, enum cvss_conf_impact           cie, 
                   enum cvss_integ_impact                iie, enum cvss_avail_impact          aie,	     
                   enum cvss_exploitability              exe, enum cvss_remediation_level     rle, 
                   enum cvss_report_confidence           rce)
{
	double cd, td, cr, ir, ar, ais, aes, abss, ats, envs;
	double av, ac, au, ci, ii, ai, ex, rl, rc;

	GETMAPVAL(cd, cda, cde);
	GETMAPVAL(td, tda, tde);
	GETMAPVAL(cr, cra, cre);
	GETMAPVAL(ir, ira, ire);
	GETMAPVAL(ar, ara, are);
	GETMAPVAL(av, ava, ave);
	GETMAPVAL(ac, aca, ace);
	GETMAPVAL(au, aua, aue);
	GETMAPVAL(ci, cia, cie);
	GETMAPVAL(ii, iia, iie);
	GETMAPVAL(ai, aia, aie);
	GETMAPVAL(ex, exa, exe);
	GETMAPVAL(rl, rla, rle);
	GETMAPVAL(rc, rca, rce);

	ais = 10.41 * (1 - (1 - ci * cr) * (1 - ii * ir) * (1 - ai * ar));
	ais = ais > 10.0 ? 10.0 : ais;
	ais = _round(ais, 0.1);
	aes = 20 * av * ac * au;
	aes = _round(aes, 0.1);
	abss = (0.6 * ais + 0.4 * aes - 1.5) * (ais ? 1.176 : 0.0);
	abss = _round(abss, 0.1);
	ats = abss * ex * rl * rc;
	ats = _round(ats, 0.1);
	envs = (ats + (10 - ats) * cd) * td;
	envs = _round(envs, 0.1);

	if (enviromental_score != NULL)
		*enviromental_score = envs;

	return 0;
}
