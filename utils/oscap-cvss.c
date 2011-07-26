/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 *      Maros Barabas  <mbarabas@redhat.com>
 */


/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <math.h>

/* CVSS */
#include <cvss.h>

#include "oscap-tool.h"

#define HELP_TOP "Metrics:\n"

#define HELP_BASE \
		"   --AV=[local|adjacent-network|network]\r\t\t\t\t\t - Access Vector\n" \
		"   --AC=[low|medium|high]\r\t\t\t\t\t - Access Complexity\n" \
		"   --AU=[none|single|multiple]\r\t\t\t\t\t - Authentication\n" \
		"   --CI=[none|partial|complete]\r\t\t\t\t\t - Confidentiality Impact\n" \
		"   --II=[none|partial|complete]\r\t\t\t\t\t - Integrity Impact\n" \
		"   --AI=[none|partial|complete]\r\t\t\t\t\t - Availability Impact\n"

#define HELP_BASE_OPT "   --base num\r\t\t\t\t\t\t\t\t\t - Base score\n"

#define HELP_TEMP \
		"   --EX=[unproven|proof-of-concept|functional|high|not-defined]\r\t\t\t\t\t\t\t\t\t - Exploitability\n" \
		"   --RL=[official-fix|temporary-fix|workaround|unavailable|not-defined]\r\t\t\t\t\t\t\t\t\t - Remediation Level\n" \
		"   --RC=[unconfirmed|uncorrporated|confirmed|not-defined]\r\t\t\t\t\t\t\t\t\t - Report Confidence\n"

#define HELP_ENV HELP_BASE HELP_TEMP \
		"   --CD=[none|low|low-medium|medium-high|high|not-defined]\r\t\t\t\t\t\t\t\t - Collateral Damage Potential\n" \
		"   --TD=[none|low|medium|high|not-defined]\r\t\t\t\t\t\t - Target Distribution\n" \
		"   --CR=[low|medium|high|not-defined]\r\t\t\t\t\t - Confidentiality Requirement\n" \
		"   --IR=[low|medium|high|not-defined]\r\t\t\t\t\t - Integrity Requirement\n" \
		"   --AR=[low|medium|high|not-defined]\r\t\t\t\t\t - Availability Requirement\n"

static bool getopt_cvss(int argc, char **argv, struct oscap_action *action);
static int app_cvss_base(const struct oscap_action *action);
static int app_cvss_temp(const struct oscap_action *action);
static int app_cvss_env(const struct oscap_action *action);
static int app_cvss_score(const struct oscap_action *action);

static struct oscap_module* CVSS_SUBMODULES[];

struct oscap_module OSCAP_CVSS_MODULE = {
    .name = "cvss",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "Common Vulnerability Scoring System",
    .submodules = CVSS_SUBMODULES
};

static struct oscap_module CVSS_BASE_MODULE = {
    .name = "base",
    .parent = &OSCAP_CVSS_MODULE,
    .summary = "CVSS Base score",
    .usage = "metrics",
    .help = HELP_TOP HELP_BASE,
    .opt_parser = getopt_cvss,
    .func = app_cvss_base
};

static struct oscap_module CVSS_TEMP_MODULE = {
    .name = "temporal",
    .parent = &OSCAP_CVSS_MODULE,
    .summary = "CVSS Temporal score",
    .usage = "metrics",
    .help = HELP_TOP HELP_TEMP HELP_BASE_OPT,
    .opt_parser = getopt_cvss,
    .func = app_cvss_temp
};

static struct oscap_module CVSS_ENV_MODULE = {
    .name = "environmental",
    .parent = &OSCAP_CVSS_MODULE,
    .summary = "CVSS Environmental score",
    .usage = "metrics",
    .help = HELP_TOP HELP_ENV,
    .opt_parser = getopt_cvss,
    .func = app_cvss_env
};

static struct oscap_module CVSS_SCORE_MODULE = {
    .name = "score",
    .parent = &OSCAP_CVSS_MODULE,
    .summary = "CVSS score from a CVSS vector",
    .usage = "vector",
    .help = "Calculates CVSS score\n"
            "(base / temporal / environmental, depends on supplied metrics).",
    .opt_parser = getopt_cvss,
    .func = app_cvss_score
};

static struct oscap_module* CVSS_SUBMODULES[] = {
    &CVSS_BASE_MODULE,
    &CVSS_TEMP_MODULE,
    &CVSS_ENV_MODULE,
    &CVSS_SCORE_MODULE,
    NULL
};

int app_cvss_base(const struct oscap_action *action)
{
    fprintf(stdout, "Base score: %.1f\n", cvss_impact_base_score(action->cvss_impact));
    return OSCAP_OK;
}

int app_cvss_temp(const struct oscap_action *action)
{
    fprintf(stdout, "Temporal score: %.1f\n",
        cvss_round(cvss_metrics_get_score(cvss_impact_get_base_metrics(action->cvss_impact)) * cvss_impact_temporal_multiplier(action->cvss_impact)));
    return OSCAP_OK;
}

int app_cvss_env(const struct oscap_action *action)
{
    fprintf(stdout, "Environmental score: %.1f\n", cvss_impact_environmental_score(action->cvss_impact));
    return OSCAP_OK;
}

static inline bool print_score(const char *type, float score)
{
    if (score >= 0.0 && score <= 10.0) {
        printf("%15s %4.1f\n", type, score);
        return true;
    }
    else return false;
}

int app_cvss_score(const struct oscap_action *action)
{
    assert(action->cvss_vector);

    bool ok = false;
    struct cvss_impact *impact = cvss_impact_new_from_vector(action->cvss_vector);

    if (impact == NULL) goto err;

    ok |= print_score("base",          cvss_impact_base_score(impact));
    ok |= print_score("temporal",      cvss_impact_temporal_score(impact));
    ok |= print_score("environmental", cvss_impact_environmental_score(impact));

    if (!ok) goto err;

    cvss_impact_free(impact);
    return OSCAP_OK;

err:
    cvss_impact_free(impact);
    fprintf(stderr, "Invalid input CVSS vector\n");
    return OSCAP_ERROR;
}

bool getopt_cvss(int argc, char **argv, struct oscap_action *action)
{

	/* Command-options */
	struct option long_options[] = {
		{"AV", 1, 0, 0},
		{"AC", 1, 0, 1},
		{"AU", 1, 0, 2},
		{"CI", 1, 0, 3},
		{"II", 1, 0, 4},
		{"AI", 1, 0, 5},
		{"EX", 1, 0, 6},
		{"RL", 1, 0, 7},
		{"RC", 1, 0, 8},
		{"CD", 1, 0, 9},
		{"TD", 1, 0, 10},
		{"CR", 1, 0, 11},
		{"IR", 1, 0, 12},
		{"AR", 1, 0, 13},
		{"base", 1, 0, 14},
		{0, 0, 0, 0}
	};

	int c;

    struct cvss_metrics *base_m, *temp_m, *env_m;
    action->cvss_impact = cvss_impact_new();
    cvss_impact_set_metrics(action->cvss_impact, (base_m = cvss_metrics_new(CVSS_BASE)));
    cvss_impact_set_metrics(action->cvss_impact, (temp_m = cvss_metrics_new(CVSS_TEMPORAL)));
    cvss_impact_set_metrics(action->cvss_impact, (env_m  = cvss_metrics_new(CVSS_ENVIRONMENTAL)));

	while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
		switch (c) {
		case 0:
			if (!strcmp(optarg, "local"))                 cvss_metrics_set_access_vector(base_m, CVSS_AV_LOCAL);
			else if (!strcmp(optarg, "adjacent-network")) cvss_metrics_set_access_vector(base_m, CVSS_AV_ADJACENT_NETWORK);
			else if (!strcmp(optarg, "network"))          cvss_metrics_set_access_vector(base_m, CVSS_AV_NETWORK);
			else return false;
			break;
		case 1:
			if (!strcmp(optarg, "low"))         cvss_metrics_set_access_complexity(base_m, CVSS_AC_LOW);
			else if (!strcmp(optarg, "medium")) cvss_metrics_set_access_complexity(base_m, CVSS_AC_MEDIUM);
			else if (!strcmp(optarg, "high"))   cvss_metrics_set_access_complexity(base_m, CVSS_AC_HIGH);
			else return false;
			break;
		case 2:
			if (!strcmp(optarg, "none"))          cvss_metrics_set_authentication(base_m, CVSS_AU_NONE);
			else if (!strcmp(optarg, "single"))   cvss_metrics_set_authentication(base_m, CVSS_AU_SINGLE);
			else if (!strcmp(optarg, "multiple")) cvss_metrics_set_authentication(base_m, CVSS_AU_MULTIPLE);
			else return false;
			break;
		case 3: case 4: case 5: {
            bool(*func)(struct cvss_metrics*, enum cvss_cia_impact);
            if (c == 3) func = cvss_metrics_set_confidentiality_impact;
            if (c == 4) func = cvss_metrics_set_integrity_impact;
            if (c == 5) func = cvss_metrics_set_availability_impact;

			if (!strcmp(optarg, "none"))          func(base_m, CVSS_IMP_NONE);
			else if (!strcmp(optarg, "partial"))  func(base_m, CVSS_IMP_PARTIAL);
			else if (!strcmp(optarg, "complete")) func(base_m, CVSS_IMP_COMPLETE);
			else return false;
			break;
        }
		case 6:
			if (!strcmp(optarg, "unproven"))              cvss_metrics_set_exploitability(temp_m, CVSS_E_UNPROVEN);
			else if (!strcmp(optarg, "proof-of-concept")) cvss_metrics_set_exploitability(temp_m, CVSS_E_PROOF_OF_CONCEPT);
			else if (!strcmp(optarg, "functional"))       cvss_metrics_set_exploitability(temp_m, CVSS_E_FUNCTIONAL);
			else if (!strcmp(optarg, "high"))             cvss_metrics_set_exploitability(temp_m, CVSS_E_HIGH);
			else if (!strcmp(optarg, "not-defined"))      cvss_metrics_set_exploitability(temp_m, CVSS_E_NOT_DEFINED);
			else return false;
			break;
		case 7:
			if (!strcmp(optarg, "official-fix"))       cvss_metrics_set_remediation_level(temp_m, CVSS_RL_OFFICIAL_FIX);
			else if (!strcmp(optarg, "temporary-fix")) cvss_metrics_set_remediation_level(temp_m, CVSS_RL_TEMPORARY_FIX);
			else if (!strcmp(optarg, "workaround"))    cvss_metrics_set_remediation_level(temp_m, CVSS_RL_WORKAROUND);
			else if (!strcmp(optarg, "unavailable"))   cvss_metrics_set_remediation_level(temp_m, CVSS_RL_UNAVAILABLE);
			else if (!strcmp(optarg, "not-defined"))   cvss_metrics_set_remediation_level(temp_m, CVSS_RL_NOT_DEFINED);
			else return false;
			break;
		case 8:
			if (!strcmp(optarg, "unconfirmed"))        cvss_metrics_set_report_confidence(temp_m, CVSS_RC_UNCONFIRMED);
			else if (!strcmp(optarg, "uncorrporated")) cvss_metrics_set_report_confidence(temp_m, CVSS_RC_UNCORROBORATED);
			else if (!strcmp(optarg, "confirmed"))     cvss_metrics_set_report_confidence(temp_m, CVSS_RC_CONFIRMED);
			else if (!strcmp(optarg, "not-defined"))   cvss_metrics_set_report_confidence(temp_m, CVSS_RC_NOT_DEFINED);
			else return false;
			break;
		case 9:
			if (!strcmp(optarg, "none"))             cvss_metrics_set_collateral_damage_potential(env_m, CVSS_CDP_NONE);
			else if (!strcmp(optarg, "low"))         cvss_metrics_set_collateral_damage_potential(env_m, CVSS_CDP_LOW);
			else if (!strcmp(optarg, "low-medium"))  cvss_metrics_set_collateral_damage_potential(env_m, CVSS_CDP_LOW_MEDIUM);
			else if (!strcmp(optarg, "medium-high")) cvss_metrics_set_collateral_damage_potential(env_m, CVSS_CDP_MEDIUM_HIGH);
			else if (!strcmp(optarg, "high"))        cvss_metrics_set_collateral_damage_potential(env_m, CVSS_CDP_HIGH);
			else if (!strcmp(optarg, "not-defined")) cvss_metrics_set_collateral_damage_potential(env_m, CVSS_CDP_NOT_DEFINED);
			else return false;
			break;
		case 10:
			if (!strcmp(optarg, "none"))             cvss_metrics_set_target_distribution(env_m, CVSS_TD_NONE);
			else if (!strcmp(optarg, "low"))         cvss_metrics_set_target_distribution(env_m, CVSS_TD_LOW);
			else if (!strcmp(optarg, "medium"))      cvss_metrics_set_target_distribution(env_m, CVSS_TD_MEDIUM);
			else if (!strcmp(optarg, "high"))        cvss_metrics_set_target_distribution(env_m, CVSS_TD_HIGH);
			else if (!strcmp(optarg, "not-defined")) cvss_metrics_set_target_distribution(env_m, CVSS_TD_NOT_DEFINED);
			else return false;
			break;
		case 11: case 12: case 13: {
            bool(*func)(struct cvss_metrics*, enum cvss_cia_requirement);
            if (c == 11) func = cvss_metrics_set_confidentiality_requirement;
            if (c == 12) func = cvss_metrics_set_integrity_requirement;
            if (c == 13) func = cvss_metrics_set_availability_requirement;

			if (!strcmp(optarg, "low"))              func(env_m, CVSS_REQ_LOW);
			else if (!strcmp(optarg, "medium"))      func(env_m, CVSS_REQ_MEDIUM);
			else if (!strcmp(optarg, "high"))        func(env_m, CVSS_REQ_HIGH);
			else if (!strcmp(optarg, "not-defined")) func(env_m, CVSS_REQ_NOT_DEFINED);
			else return false;
			break;
        }
		case 14:
			cvss_metrics_set_score(base_m, atof(optarg));
			break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

    if (optind < argc) action->cvss_vector = argv[optind];

	if ((action->module == &CVSS_ENV_MODULE || action->module == &CVSS_BASE_MODULE) && !cvss_metrics_is_valid(base_m))
        return oscap_module_usage(action->module, stderr, "Required metrics were not specified");

	if ((action->module == &CVSS_TEMP_MODULE) && isnan(cvss_metrics_get_score(base_m)))
        return oscap_module_usage(action->module, stderr, "Base score was not specified");

	if ((action->module == &CVSS_SCORE_MODULE) && action->cvss_vector == NULL)
        return oscap_module_usage(action->module, stderr, "CVSS vector not supplied");

	return true;
}

