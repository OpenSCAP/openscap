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
 *      Maros Barabas <mbarabas@redhat.com>
 */


/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

/* openscap common */
#include <oscap.h>
#include <error.h>
#include <text.h>
#include <reporter.h>

#ifdef ENABLE_OVAL
#include <oval_definitions.h>
#endif
#ifdef ENABLE_CVSS
#include <cvss.h>
#endif
#ifdef ENABLE_XCCDF
#include <xccdf.h>
#endif

typedef enum {
        OSCAP_STD_UNKNOWN,
        OSCAP_STD_XCCDF,
        OSCAP_STD_OVAL,
        OSCAP_STD_CVSS
} oscap_standard_t;

typedef enum {
        OSCAP_OP_UNKNOWN,
        OSCAP_OP_COLLECT,
        OSCAP_OP_EVAL,
        OSCAP_OP_VALIDATE_XML,
        OSCAP_OP_BASE,
        OSCAP_OP_TEMP,
		OSCAP_OP_RESOLVE,
        OSCAP_OP_ENV
} oscap_operation_t;

#ifdef ENABLE_CVSS
struct cvss_metrics {
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
        double base;
};
#endif

struct oscap_action {
        oscap_standard_t std;
        oscap_operation_t op;
        oscap_document_type_t doctype;
        char *f_xccdf;
        char *f_oval;
        char *f_results;
        char *url_xccdf;
        char *url_oval;
        char *profile;
        char *file_version;
#ifdef ENABLE_CVSS
        struct cvss_metrics *cvss_metrics;
#endif
	bool force;
};

#ifdef ENABLE_XCCDF
void print_xccdf_usage(const char *pname, FILE * out, char *msg);
int app_evaluate_xccdf(const struct oscap_action *action);
int app_xccdf_resolve(const struct oscap_action *action);
int getopt_xccdf(int argc, char **argv, struct oscap_action *action);
#endif

#ifdef ENABLE_CVSS
void print_cvss_usage(const char *pname, FILE * out, char *msg);
int getopt_cvss(int argc, char **argv, struct oscap_action *action);
#endif

void print_oval_usage(const char *pname, FILE * out, char *msg);
int app_collect_oval(const struct oscap_action *action);
int app_evaluate_oval(const struct oscap_action *action);
int getopt_oval(int argc, char **argv, struct oscap_action *action);

