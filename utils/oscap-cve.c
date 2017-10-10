/*
 * Copyright 2010--2014 Red Hat Inc., Durham, North Carolina.
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
 *      Peter Vrabec  <pvrabec@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <math.h>

#include <cve_nvd.h>
#include <oscap_source.h>
#include <xccdf_session.h>

#include "oscap-tool.h"

static bool getopt_cve(int argc, char **argv, struct oscap_action *action);
static int app_cve_validate(const struct oscap_action *action);
static int app_cve_find(const struct oscap_action *action);

#define CVE_SUBMODULES_NUM 3 /* See actual CVE_SUBMODULES array
				initialization below. */
static struct oscap_module* CVE_SUBMODULES[CVE_SUBMODULES_NUM];

struct oscap_module OSCAP_CVE_MODULE = {
    .name = "cve",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "Common Vulnerabilities and Exposures",
    .submodules = CVE_SUBMODULES
};

static struct oscap_module CVE_VALIDATE_MODULE = {
    .name = "validate",
    .parent = &OSCAP_CVE_MODULE,
    .summary = "Validate CVE NVD feed",
    .usage = "nvd-feed.xml",
    .help = "Validate CVE NVD feed.",
    .opt_parser = getopt_cve,
    .func = app_cve_validate
};

static struct oscap_module CVE_FIND_MODULE = {
    .name = "find",
    .parent = &OSCAP_CVE_MODULE,
    .summary = "Find particular CVE in CVE NVD feed",
    .usage = "CVE nvd-feed.xml",
    .help = "Find particular CVE in CVE NVD feed.",
    .opt_parser = getopt_cve,
    .func = app_cve_find
};

static struct oscap_module* CVE_SUBMODULES[CVE_SUBMODULES_NUM] = {
    &CVE_VALIDATE_MODULE,
    &CVE_FIND_MODULE,
    NULL
};

static int app_cve_validate(const struct oscap_action *action)
{
	int ret;
        int result;

	struct oscap_source *source = oscap_source_new_from_file(action->cve_action->file);
	ret = oscap_source_validate(source, reporter, (void *) action);
	oscap_source_free(source);
        if (ret==-1) {
                result=OSCAP_ERROR;
                goto cleanup;
        }
        else if (ret==1) {
                result=OSCAP_FAIL;
        }
        else
                result=OSCAP_OK;

cleanup:
        if (oscap_err())
                fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

        free(action->cve_action);
        return result;
}

static int app_cve_find(const struct oscap_action *action)
{
        struct cve_model *model = NULL;
        struct cve_entry *entry = NULL;
	struct cve_entry_iterator *entry_it;
	const struct cvss_impact *cvss;
        struct cvss_metrics *metrics;
        float base_score;
	char * vector;
	int result;
	struct cve_product_iterator *prod_it;
	struct cve_product *product;

	model = cve_model_import(action->cve_action->file);
	if(!model) {
		result=OSCAP_ERROR;
		goto cleanup;
	}

	entry_it = cve_model_get_entries(model);
	while (cve_entry_iterator_has_more(entry_it)) {
		entry = cve_entry_iterator_next(entry_it);
		if (!strcmp(cve_entry_get_id(entry), action->cve_action->cve))
			break;
		entry = NULL;
	}
	cve_entry_iterator_free(entry_it);

	if (!entry) {
		result=OSCAP_FAIL;
		goto cleanup;
	}

	printf("ID: %s\n", cve_entry_get_id(entry));

	/* cvss content */
	cvss = cve_entry_get_cvss(entry);
	if (cvss) {
		metrics = cvss_impact_get_base_metrics(cvss);
		base_score = cvss_metrics_get_score(metrics);
		vector =  cvss_impact_to_vector(cvss);
		printf("Base Score: %.1f\n", base_score);
		printf("Vector String:\n\t%s\n", vector);
		free(vector);
	}

	/* vulnerable-software-list */
	printf("Vulnerable Software:\n");
	prod_it=cve_entry_get_products(entry);
	while(cve_product_iterator_has_more(prod_it)) {
		product = cve_product_iterator_next(prod_it);
		printf("\t%s\n", cve_product_get_value(product));
	}
	cve_product_iterator_free(prod_it);

	result=OSCAP_OK;

cleanup:
        if (oscap_err())
                fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

        if (model)
		cve_model_free(model);
        free(action->cve_action);
        return result;
}

bool getopt_cve(int argc, char **argv, struct oscap_action *action)
{
        if( (action->module == &CVE_VALIDATE_MODULE)) {
                if( argc != 4 ) {
                        oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
                        return false;
                }
                action->doctype = OSCAP_DOCUMENT_CVE_FEED;
                action->cve_action = malloc(sizeof(struct cve_action));
                action->cve_action->file=argv[3];
        }
	else if (action->module == &CVE_FIND_MODULE) {
	        if( argc != 5 ) {
                        oscap_module_usage(action->module, stderr, "Wrong number of parameters.\n");
                        return false;
                }
		action->doctype = OSCAP_DOCUMENT_CVE_FEED;
		action->cve_action = malloc(sizeof(struct cve_action));
		action->cve_action->cve=argv[3];
		action->cve_action->file=argv[4];
	}

	return true;
}

