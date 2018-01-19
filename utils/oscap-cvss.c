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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include <assert.h>
#include <math.h>

#include <cvss_score.h>
#include <xccdf_session.h>

#include "oscap-tool.h"

static bool getopt_cvss(int argc, char **argv, struct oscap_action *action);
static int app_cvss_score(const struct oscap_action *action);
static int app_cvss_describe(const struct oscap_action *action);

#define CVSS_SUBMODULES_NUM 3 /* See actual CVSS_SUBMODULES array
				initialization below. */
static struct oscap_module* CVSS_SUBMODULES[CVSS_SUBMODULES_NUM];

struct oscap_module OSCAP_CVSS_MODULE = {
    .name = "cvss",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "Common Vulnerability Scoring System",
    .submodules = CVSS_SUBMODULES
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

static struct oscap_module CVSS_DESCRIBE_MODULE = {
    .name = "describe",
    .parent = &OSCAP_CVSS_MODULE,
    .summary = "Describe a CVSS vector",
    .usage = "vector",
    .help = "Describes individual components of a CVSS vector\n",
    .opt_parser = getopt_cvss,
    .func = app_cvss_describe
};

static struct oscap_module* CVSS_SUBMODULES[CVSS_SUBMODULES_NUM] = {
    &CVSS_SCORE_MODULE,
    &CVSS_DESCRIBE_MODULE,
    NULL
};

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

static int app_cvss_describe(const struct oscap_action *action)
{
    assert(action->cvss_vector);

    struct cvss_impact *impact = cvss_impact_new_from_vector(action->cvss_vector);

    if (impact) {
        cvss_impact_describe(impact, stdout);
        cvss_impact_free(impact);
        return OSCAP_OK;
    }
    else {
        fprintf(stderr, "Invalid input CVSS vector\n");
        return OSCAP_ERROR;
    }
}

bool getopt_cvss(int argc, char **argv, struct oscap_action *action)
{
    if (optind < argc) action->cvss_vector = argv[optind];

	if ((action->module == &CVSS_SCORE_MODULE) && action->cvss_vector == NULL)
        return oscap_module_usage(action->module, stderr, "CVSS vector not supplied");

	return true;
}

