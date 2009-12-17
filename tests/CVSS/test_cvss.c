#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cvss.h>

#define EPS (0.000001)

void print_usage(FILE *, char *);

double base_score(cvss_access_vector_t, 
		  cvss_access_complexity_t, 
		  cvss_authentication_t,
		  cvss_conf_impact_t, 
		  cvss_integ_impact_t,
		  cvss_avail_impact_t);

double temp_score(cvss_exploitability_t, 
		  cvss_remediation_level_t, 
		  cvss_report_confidence_t, 
		  double);

double env_score(cvss_collateral_damage_potential_t, 
		 cvss_target_distribution_t, 
		 cvss_conf_req_t, 
		 cvss_integ_req_t, 
		 cvss_avail_req_t, 
		 cvss_access_vector_t, 
		 cvss_access_complexity_t, 
		 cvss_authentication_t, 
		 cvss_conf_impact_t, 
		 cvss_integ_impact_t, 
		 cvss_avail_impact_t,	     
		 cvss_exploitability_t, 
		 cvss_remediation_level_t, 
		 cvss_report_confidence_t);

int main(int argc, char *argv[])
{
  double base_score_a, base_score_b;
  double temp_score_a, temp_score_b;
  double env_score_a, env_score_b;
  cvss_access_vector_t ave; 
  cvss_access_complexity_t ace;
  cvss_authentication_t aue;
  cvss_conf_impact_t cie;
  cvss_integ_impact_t iie;
  cvss_avail_impact_t aie;
  cvss_exploitability_t exe;
  cvss_remediation_level_t rle;
  cvss_report_confidence_t rce;
  cvss_collateral_damage_potential_t cde;
  cvss_target_distribution_t tde;
  cvss_conf_req_t cre;
  cvss_integ_req_t ire;
  cvss_avail_req_t are;
  int ret_val = 0;

  if (argc == 1) {
    print_usage(stdout, argv[0]);
    ret_val = 0;
  }

  else if (argc == 8 && !strcmp(argv[1], "--base")) {

    if (!strcmp(argv[2], "AV_LOCAL")) ave = AV_LOCAL;
    if (!strcmp(argv[2], "AV_ADJACENT_NETWORK")) ave = AV_ADJACENT_NETWORK;
    if (!strcmp(argv[2], "AV_NETWORK")) ave = AV_NETWORK;
    
    if (!strcmp(argv[3], "AC_HIGH")) ace = AC_HIGH;
    if (!strcmp(argv[3], "AC_MEDIUM")) ace = AC_MEDIUM;
    if (!strcmp(argv[3], "AC_LOW")) ace = AC_LOW;
    
    if (!strcmp(argv[4], "AU_NONE")) aue = AU_NONE;
    if (!strcmp(argv[4], "AU_SINGLE_INSTANCE")) aue = AU_SINGLE_INSTANCE;
    if (!strcmp(argv[4], "AU_MULTIPLE_INSTANCE")) aue = AU_MULTIPLE_INSTANCE;
    
    if (!strcmp(argv[5], "CI_NONE")) cie = CI_NONE;
    if (!strcmp(argv[5], "CI_PARTIAL")) cie = CI_PARTIAL;
    if (!strcmp(argv[5], "CI_COMPLETE")) cie = CI_COMPLETE;
    
    if (!strcmp(argv[6], "II_NONE")) iie = II_NONE;
    if (!strcmp(argv[6], "II_PARTIAL")) iie = II_PARTIAL;
    if (!strcmp(argv[6], "II_COMPLETE")) iie = II_COMPLETE;
    
    if (!strcmp(argv[7], "AI_NONE")) aie = AI_NONE;
    if (!strcmp(argv[7], "AI_PARTIAL")) aie = AI_PARTIAL;
    if (!strcmp(argv[7], "AI_COMPLETE")) aie = AI_COMPLETE;
    
    cvss_base_score(ave, ace, aue, cie, iie, aie, &base_score_a, NULL, NULL);
    base_score_b = base_score(ave, ace, aue, cie, iie, aie);

    ret_val = fabs(base_score_a - base_score_b) < EPS ? 0 : 1;

    fprintf(ret_val ? stderr : stdout,
	    "BS=%f [%f] (%s %s %s %s %s %s)\n", base_score_a, base_score_b,
	    argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
  }

  else if (argc == 6 && !strcmp(argv[1], "--temporal")) {

    if (!strcmp(argv[2], "EX_UNPROVEN")) exe = EX_UNPROVEN;
    if (!strcmp(argv[2], "EN_PROOF_OF_CONCEPT")) exe = EX_PROOF_OF_CONCEPT;
    if (!strcmp(argv[2], "EX_FUNCTIONAL")) exe = EX_FUNCTIONAL;
    if (!strcmp(argv[2], "EX_HIGH")) exe = EX_HIGH;
    if (!strcmp(argv[2], "EX_NOT_DEFINED")) exe = EX_NOT_DEFINED;

    if (!strcmp(argv[3], "RL_OFFICIAL_FIX")) rle = RL_OFFICIAL_FIX;
    if (!strcmp(argv[3], "RL_TEMPORARY_FIX")) rle = RL_TEMPORARY_FIX;
    if (!strcmp(argv[3], "RL_WORKAROUND")) rle = RL_WORKAROUND;
    if (!strcmp(argv[3], "RL_UNAVAILABLE")) rle = RL_UNAVAILABLE;
    if (!strcmp(argv[3], "RL_NOT_DEFINED")) rle = RL_NOT_DEFINED;

    if (!strcmp(argv[4], "RC_UNCONFIRMED")) rce = RC_UNCONFIRMED;
    if (!strcmp(argv[4], "RC_UNCORROBORATED")) rce = RC_UNCORROBORATED;
    if (!strcmp(argv[4], "RC_CONFIRMED")) rce = RC_CONFIRMED;
    if (!strcmp(argv[4], "RC_NOT_DEFINED")) rce = RC_NOT_DEFINED;
    
    cvss_temp_score(exe, rle, rce, atof(argv[5]), &temp_score_a);
    temp_score_b = temp_score(exe, rle, rce, atof(argv[5]));

    ret_val = fabs(temp_score_a - temp_score_b) < EPS ? 0 : 1;

    fprintf(ret_val ? stderr : stdout,
	    "TS=%f [%f] (%s %s %s %s)\n", temp_score_a, temp_score_b,
	    argv[2], argv[3], argv[4], argv[5]);
  }  

  else if (argc == 16 && !strcmp(argv[1], "--enviromental")) {

    if (!strcmp(argv[2], "CD_NONE")) cde = CD_NONE;
    if (!strcmp(argv[2], "CD_LOW")) cde = CD_LOW;
    if (!strcmp(argv[2], "CD_LOW_MEDIUM")) cde = CD_LOW_MEDIUM;
    if (!strcmp(argv[2], "CD_MEDIUM_HIGH")) cde = CD_MEDIUM_HIGH;
    if (!strcmp(argv[2], "CD_HIGH")) cde = CD_HIGH;
    if (!strcmp(argv[2], "CD_NOT_DEFINED")) cde = CD_NOT_DEFINED;

    if (!strcmp(argv[3], "TD_NONE")) tde = TD_NONE;
    if (!strcmp(argv[3], "TD_LOW")) tde = TD_LOW;
    if (!strcmp(argv[3], "TD_MEDIUM")) tde = TD_MEDIUM;
    if (!strcmp(argv[3], "TD_HIGH")) tde = TD_HIGH;
    if (!strcmp(argv[3], "TD_NOT_DEFINED")) tde = TD_NOT_DEFINED;
	
    if (!strcmp(argv[4], "CR_LOW")) cre = CR_LOW;
    if (!strcmp(argv[4], "CR_MEDIUM")) cre = CR_MEDIUM;
    if (!strcmp(argv[4], "CR_HIGH")) cre = CR_HIGH;
    if (!strcmp(argv[4], "CR_NOT_DEFINED")) cre = CR_NOT_DEFINED;

    if (!strcmp(argv[5], "IR_LOW")) ire = IR_LOW;
    if (!strcmp(argv[5], "IR_MEDIUM")) ire = IR_MEDIUM;
    if (!strcmp(argv[5], "IR_HIGH")) ire = IR_HIGH;
    if (!strcmp(argv[5], "IR_NOT_DEFINED")) ire = IR_NOT_DEFINED;

    if (!strcmp(argv[6], "AR_LOW")) are = AR_LOW;
    if (!strcmp(argv[6], "AR_MEDIUM")) are = AR_MEDIUM;
    if (!strcmp(argv[6], "AR_HIGH")) are = AR_HIGH;
    if (!strcmp(argv[6], "AR_NOT_DEFINED")) are = AR_NOT_DEFINED;

    if (!strcmp(argv[7], "AV_LOCAL")) ave = AV_LOCAL;
    if (!strcmp(argv[7], "AV_ADJACENT_NETWORK")) ave = AV_ADJACENT_NETWORK;
    if (!strcmp(argv[7], "AV_NETWORK")) ave = AV_NETWORK;
    
    if (!strcmp(argv[8], "AC_HIGH")) ace = AC_HIGH;
    if (!strcmp(argv[8], "AC_MEDIUM")) ace = AC_MEDIUM;
    if (!strcmp(argv[8], "AC_LOW")) ace = AC_LOW;
    
    if (!strcmp(argv[9], "AU_NONE")) aue = AU_NONE;
    if (!strcmp(argv[9], "AU_SINGLE_INSTANCE")) aue = AU_SINGLE_INSTANCE;
    if (!strcmp(argv[9], "AU_MULTIPLE_INSTANCE")) aue = AU_MULTIPLE_INSTANCE;
    
    if (!strcmp(argv[10], "CI_NONE")) cie = CI_NONE;
    if (!strcmp(argv[10], "CI_PARTIAL")) cie = CI_PARTIAL;
    if (!strcmp(argv[10], "CI_COMPLETE")) cie = CI_COMPLETE;
    
    if (!strcmp(argv[11], "II_NONE")) iie = II_NONE;
    if (!strcmp(argv[11], "II_PARTIAL")) iie = II_PARTIAL;
    if (!strcmp(argv[11], "II_COMPLETE")) iie = II_COMPLETE;
    
    if (!strcmp(argv[12], "AI_NONE")) aie = AI_NONE;
    if (!strcmp(argv[12], "AI_PARTIAL")) aie = AI_PARTIAL;
    if (!strcmp(argv[12], "AI_COMPLETE")) aie = AI_COMPLETE;
  
    if (!strcmp(argv[13], "EX_UNPROVEN")) exe = EX_UNPROVEN;
    if (!strcmp(argv[13], "EN_PROOF_OF_CONCEPT")) exe = EX_PROOF_OF_CONCEPT;
    if (!strcmp(argv[13], "EX_FUNCTIONAL")) exe = EX_FUNCTIONAL;
    if (!strcmp(argv[13], "EX_HIGH")) exe = EX_HIGH;
    if (!strcmp(argv[13], "EX_NOT_DEFINED")) exe = EX_NOT_DEFINED;

    if (!strcmp(argv[14], "RL_OFFICIAL_FIX")) rle = RL_OFFICIAL_FIX;
    if (!strcmp(argv[14], "RL_TEMPORARY_FIX")) rle = RL_TEMPORARY_FIX;
    if (!strcmp(argv[14], "RL_WORKAROUND")) rle = RL_WORKAROUND;
    if (!strcmp(argv[14], "RL_UNAVAILABLE")) rle = RL_UNAVAILABLE;
    if (!strcmp(argv[14], "RL_NOT_DEFINED")) rle = RL_NOT_DEFINED;

    if (!strcmp(argv[15], "RC_UNCONFIRMED")) rce = RC_UNCONFIRMED;
    if (!strcmp(argv[15], "RC_UNCORROBORATED")) rce = RC_UNCORROBORATED;
    if (!strcmp(argv[15], "RC_CONFIRMED")) rce = RC_CONFIRMED;
    if (!strcmp(argv[15], "RC_NOT_DEFINED")) rce = RC_NOT_DEFINED;
    
    cvss_env_score(cde, tde, cre, ire, are, &env_score_a, ave, ace, aue, cie, iie, aie, exe, rle, rce);
    env_score_b = env_score(cde, tde, cre, ire, are, ave, ace, aue, cie, iie, aie, exe, rle, rce);

    ret_val = fabs(env_score_a - env_score_b) < EPS ? 0 : 1;

    fprintf(ret_val ? stderr : stdout,
	    "ES=%f [%f] (%s %s %s %s %s %s %s %s %s %s %s %s %s %s)\n",
	    env_score_a, env_score_b,
	    argv[2], argv[3], argv[4], argv[5], argv[6], argv[7],
	    argv[8], argv[9], argv[10], argv[11], argv[12], argv[13],
	    argv[14], argv[15]);
  }  

  else {
    print_usage(stderr, argv[0]);
    ret_val = 1;
  }

  return ret_val;
}

void print_usage(FILE *out, char *program_name) 
{
  fprintf(out, "Usage: \n\n"
	  "%s --base AVE ACE AUE CIE IIE AIE\n"
	  "%s --temporary EXE RLE RCE BASE_SCORE\n"
	  "%s --enviromental CDE TDE CRE IRE ARE AVE ACE AUE CIE IIE AIE EXE RLE RCE\n",
	  program_name, program_name, program_name);
}

double base_score(cvss_access_vector_t ave, 
		  cvss_access_complexity_t ace, 
		  cvss_authentication_t aue,
		  cvss_conf_impact_t cie, 
		  cvss_integ_impact_t iie, 
		  cvss_avail_impact_t aie) 
{
  double ave_d = 0, ace_d = 0, aue_d = 0, cie_d = 0,
    iie_d = 0, aie_d = 0, exp_d = 0, imp_d = 0, score = 0;

  switch (ave) {
  case AV_LOCAL:
    ave_d=0.395; 
    break;
  case AV_ADJACENT_NETWORK:
    ave_d=0.646;
    break;
  case AV_NETWORK:
    ave_d=1.0;
    break;
  }

  switch (ace) {
  case AC_HIGH:
    ace_d=0.35;
    break;
  case AC_MEDIUM:
    ace_d=0.61;
    break;
  case AC_LOW:
    ace_d=0.71;
    break;
  }
  
  switch (aue) {
  case AU_NONE:
    aue_d=0.704;
    break;
  case AU_SINGLE_INSTANCE:
    aue_d=0.56;
    break;
  case AU_MULTIPLE_INSTANCE:
    aue_d=0.45;
    break;
  }

  switch (cie) {
  case CI_NONE:
    cie_d=0;
    break;
  case CI_PARTIAL:
    cie_d=0.275;
    break;
  case CI_COMPLETE:
    cie_d=0.660;
    break;
  }

  switch (iie) {
  case II_NONE:
    iie_d=0;
    break;
  case II_PARTIAL:
    iie_d=0.275;
    break;
  case II_COMPLETE:
    iie_d=0.660;
    break;
  }

  switch (aie) {
  case AI_NONE:
    aie_d=0;
    break;
  case AI_PARTIAL:
    aie_d=0.275;
    break;
  case AI_COMPLETE:
    aie_d=0.660;
    break;
  }

  exp_d = 20.0 * ave_d * ace_d * aue_d;
  imp_d = 10.41 *(1.0 - (1.0 - cie_d) * (1.0 - iie_d) * (1.0 - aie_d));
  score = ((0.6 * imp_d) + (0.4 * exp_d) - 1.5) * (fabs(imp_d) < EPS ? 0.0 : 1.176);
  
  score = round(score / 0.1) * 0.1;

  return score;
}

double temp_score(cvss_exploitability_t exe, 
		  cvss_remediation_level_t rle, 
		  cvss_report_confidence_t rce, 
		  double b_score) 
{
  double exe_d = 0, rle_d = 0, rce_d = 0, score = 0;

  switch (exe) {
  case EX_UNPROVEN:
    exe_d = 0.85;
    break;
  case EX_PROOF_OF_CONCEPT:
    exe_d = 0.9;
    break;
  case EX_FUNCTIONAL:
    exe_d = 0.95;
    break;
  case EX_HIGH:
    exe_d = 1.0;
    break;
  case EX_NOT_DEFINED:
    exe_d = 1.0;
    break;
  }

  switch (rle) {
  case RL_OFFICIAL_FIX:
    rle_d = 0.87;
    break;
  case RL_TEMPORARY_FIX:
    rle_d = 0.9;
    break;
  case RL_WORKAROUND:
    rle_d = 0.95;
    break;
  case RL_UNAVAILABLE:
    rle_d = 1.0;
    break;
  case RL_NOT_DEFINED:
    rle_d = 1.0;
    break;
  }
  
  switch (rce) {
  case RC_UNCONFIRMED:
    rce_d = 0.9;
    break;
  case RC_UNCORROBORATED:
    rce_d = 0.95;
    break;
  case RC_CONFIRMED:
    rce_d = 1.0;
    break;
  case RC_NOT_DEFINED:
    rce_d = 1.0;
    break;
  }  

  score = round((b_score * exe_d * rle_d * rce_d) / 0.1) * 0.1;
  
  return score;
}

double env_score(cvss_collateral_damage_potential_t cde, 
		 cvss_target_distribution_t tde, 
		 cvss_conf_req_t cre, 
		 cvss_integ_req_t ire, 
		 cvss_avail_req_t are, 
		 cvss_access_vector_t ave, 
		 cvss_access_complexity_t ace, 
		 cvss_authentication_t aue, 
		 cvss_conf_impact_t cie, 
		 cvss_integ_impact_t iie, 
		 cvss_avail_impact_t aie,	     
		 cvss_exploitability_t exe, 
		 cvss_remediation_level_t rle, 
		 cvss_report_confidence_t rce) 
{
  double cde_d = 0, tde_d = 0, cre_d = 0, ire_d = 0, are_d = 0, ave_d = 0, 
    ace_d = 0, aue_d = 0, cie_d = 0, iie_d = 0, aie_d = 0, exe_d = 0, 
    rle_d = 0, rce_d = 0, bas_d = 0, score = 0, exp_d = 0, imp_d = 0,
    adt_d = 0;
  
  switch (cde) {
  case CD_NONE:
    cde_d = 0.0;
    break;
  case CD_LOW:
    cde_d = 0.1;
    break;
  case CD_LOW_MEDIUM:
    cde_d = 0.3;
    break;
  case CD_MEDIUM_HIGH:
    cde_d = 0.4;
    break;
  case CD_HIGH:
    cde_d = 0.5;
    break;
  case CD_NOT_DEFINED:
    cde_d = 0.0;
    break;
  }

  switch (tde) {
  case TD_NONE:
    tde_d = 0.0;
    break;
  case TD_LOW:
    tde_d = 0.25;
    break;
  case TD_MEDIUM:
    tde_d = 0.75;
    break;
  case TD_HIGH:
    tde_d = 1.0;
    break;
  case TD_NOT_DEFINED:
    tde_d = 1.0;
    break;
  }

  switch (cre) {
  case CR_LOW:
    cre_d = 0.5;
    break;
  case CR_MEDIUM:
    cre_d = 1.0;
    break;
  case CR_HIGH:
    cre_d = 1.51;
    break;
  case CR_NOT_DEFINED:
    cre_d = 1.0;
    break;
  }

  switch (ire) {
  case IR_LOW:
    ire_d = 0.5;
    break;
  case IR_MEDIUM:
    ire_d = 1.0;
    break;
  case IR_HIGH:
    ire_d = 1.51;
    break;
  case IR_NOT_DEFINED:
    ire_d = 1.0;
    break;
  }
  
  switch (are) {
  case AR_LOW:
    are_d = 0.5;
    break;
  case AR_MEDIUM:
    are_d = 1.0;
    break;
  case AR_HIGH:
    are_d = 1.51;
    break;
  case AR_NOT_DEFINED:
    are_d = 1.0;
    break;
  }

  switch (ave) {
  case AV_LOCAL:
    ave_d=0.395; 
    break;
  case AV_ADJACENT_NETWORK:
    ave_d=0.646;
    break;
  case AV_NETWORK:
    ave_d=1.0;
    break;
  }

  switch (ace) {
  case AC_HIGH:
    ace_d=0.35;
    break;
  case AC_MEDIUM:
    ace_d=0.61;
    break;
  case AC_LOW:
    ace_d=0.71;
    break;
  }
  
  switch (aue) {
  case AU_NONE:
    aue_d=0.704;
    break;
  case AU_SINGLE_INSTANCE:
    aue_d=0.56;
    break;
  case AU_MULTIPLE_INSTANCE:
    aue_d=0.45;
    break;
  }

  switch (cie) {
  case CI_NONE:
    cie_d=0;
    break;
  case CI_PARTIAL:
    cie_d=0.275;
    break;
  case CI_COMPLETE:
    cie_d=0.660;
    break;
  }

  switch (iie) {
  case II_NONE:
    iie_d=0;
    break;
  case II_PARTIAL:
    iie_d=0.275;
    break;
  case II_COMPLETE:
    iie_d=0.660;
    break;
  }

  switch (aie) {
  case AI_NONE:
    aie_d=0;
    break;
  case AI_PARTIAL:
    aie_d=0.275;
    break;
  case AI_COMPLETE:
    aie_d=0.660;
    break;
  }

  switch (exe) {
  case EX_UNPROVEN:
    exe_d = 0.85;
    break;
  case EX_PROOF_OF_CONCEPT:
    exe_d = 0.9;
    break;
  case EX_FUNCTIONAL:
    exe_d = 0.95;
    break;
  case EX_HIGH:
    exe_d = 1.0;
    break;
  case EX_NOT_DEFINED:
    exe_d = 1.0;
    break;
  }

  switch (rle) {
  case RL_OFFICIAL_FIX:
    rle_d = 0.87;
    break;
  case RL_TEMPORARY_FIX:
    rle_d = 0.9;
    break;
  case RL_WORKAROUND:
    rle_d = 0.95;
    break;
  case RL_UNAVAILABLE:
    rle_d = 1.0;
    break;
  case RL_NOT_DEFINED:
    rle_d = 1.0;
    break;
  }
  
  switch (rce) {
  case RC_UNCONFIRMED:
    rce_d = 0.9;
    break;
  case RC_UNCORROBORATED:
    rce_d = 0.95;
    break;
  case RC_CONFIRMED:
    rce_d = 1.0;
    break;
  case RC_NOT_DEFINED:
    rce_d = 1.0;
    break;
  }
  
  exp_d = 20.00  * ave_d * ace_d * aue_d;
  imp_d = 10.41 * (1.00  - (1.00  - cie_d * cre_d) * (1.00  - iie_d * ire_d) * (1.00  - aie_d * are_d));
  imp_d = imp_d > 10.00  ? 10.00  : imp_d;

  bas_d = ((0.6 * imp_d) + (0.4 * exp_d) - 1.5) * (fabs(imp_d) < EPS ? 0.00  : 1.176);  
  bas_d = round(bas_d / 0.1) * 0.1;
  
  adt_d = temp_score(exe, rle, rce, bas_d);

  score = round(((adt_d + (10.00  - adt_d) * cde_d) * tde_d) / 0.1) * 0.1;

  return score;
}
