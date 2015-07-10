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
#include <libgen.h>

/* openscap common */
#include <oscap.h>
#include <oscap_error.h>
#include <oscap_text.h>

#include <oval_definitions.h>
#include <oval_probe.h>
#include <cvss_score.h>
#include <xccdf_benchmark.h>
#include <cpe_dict.h>
#include <cpe_name.h>
#include <cve_nvd.h>

#define OSCAP_PRODUCTNAME "cpe:/a:open-scap:oscap"
#define OSCAP_ERR_MSG "OpenSCAP Error:"

struct oscap_action;
struct oscap_module;

typedef int(*oscap_tool_func)(const struct oscap_action* action);
typedef bool(*oscap_option_func)(int argc, char **argv, struct oscap_action* action);

struct oscap_module {
    const char *name;
    const char *usage;
    const char *usage_extra;
    const char *summary;
    const char *help;
    bool hidden;
    struct oscap_module *parent;
    struct oscap_module **submodules;
    oscap_tool_func func;
    oscap_option_func opt_parser;
    void *user;
};

// standard oscap CLI exit statuses
enum oscap_exitcode {
    OSCAP_OK             =   0, // successful exit
    OSCAP_ERROR          =   1, // an error occured
    OSCAP_FAIL           =   2, // a process (e.g. scan or validation) failed
    OSCAP_ERR_FETCH      =   1, // cold not fetch input file (same as error for now)
    OSCAP_BADARGS        = 100, // bad commandline arguments
    OSCAP_BADMODULE      = 101, // unrecognized module
    OSCAP_UNIMPL_MOD     = 110, // module functionality not implemented
    OSCAP_UNIMPL         = 111, // functionality not implemented
    // end of list
    OSCAP_EXITCODES_END_ = 120  // any code returned shall not be higher than this
};

struct cvss_impact;

struct ds_action {
	char* file;
	char* target;
	char* xccdf_result;
	char** oval_results;
	size_t oval_result_count;
};

struct cpe_action {
	char * name;
	char * dict;
};

struct cve_action {
        char * file;
        char * cve;
};

struct oscap_action {
        struct oscap_module *module;
	/* files */
        char *f_xccdf;
	char *f_datastream_id;
	char *f_xccdf_id;
	char *f_oval_id;
	char *f_benchmark_id;
	char *f_report_id;
        char *f_oval;
        char **f_ovals;
	char *f_syschar;
	char *f_directives;
        char *f_results;
	char *f_results_arf;
        char *f_report;
	char *f_variables;
	/* others */
        char *profile;
        char *show;
        char *format;
        const char *tmpl;
        char *id;
        char *oval_template;
        char *cvss_vector;
        int hide_profile_info;
        char *stylesheet;
	char *tailoring_file;
	char *tailoring_id;
	char *cpe;

        struct cvss_impact *cvss_impact;
	struct ds_action* ds_action;
	struct cpe_action * cpe_action;
	struct cve_action * cve_action;
	char *file;

	int verbosity;
        int doctype;
	int force;
	int validate;
	int schematron;
	int remote_resources;
	int progress;
	int oval_results;
	int remediate;
	char *sce_template;
	int check_engine_results;
	int export_variables;
        int list_dynamic;
	char *probe_root;
};

int app_xslt(const char *infile, const char *xsltfile, const char *outfile, const char **params);
int reporter(const char *file, int line, const char *msg, void *arg);

int oscap_module_process(struct oscap_module *module, int argc, char **argv);
bool oscap_module_usage(struct oscap_module *module, FILE *out, const char *err, ...);
int oscap_module_call(struct oscap_action *action);

void oscap_print_error(void);

extern struct oscap_module OSCAP_ROOT_MODULE;
extern struct oscap_module OSCAP_DS_MODULE;
extern struct oscap_module OSCAP_XCCDF_MODULE;
extern struct oscap_module OSCAP_CVSS_MODULE;
extern struct oscap_module OSCAP_OVAL_MODULE;
extern struct oscap_module OSCAP_CVE_MODULE;
extern struct oscap_module OSCAP_CPE_MODULE;
extern struct oscap_module OSCAP_INFO_MODULE;

