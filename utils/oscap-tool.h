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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      Maros Barabas <mbarabas@redhat.com>
 */

#pragma once
#ifndef OSCAP_TOOL_H_
#define OSCAP_TOOL_H_

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

/* openscap common */
#include <oscap.h>
#include <oscap_error.h>
#include <oscap_text.h>

#include <oval_definitions.h>
#if defined(OVAL_PROBES_ENABLED)
# include <oval_probe.h>
#endif
#include <xccdf_benchmark.h>
#include <xccdf_session.h>
#include <cpe_dict.h>
#include <cpe_name.h>

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
    OSCAP_ERROR          =   1, // an error occurred
    OSCAP_FAIL           =   2, // a process (e.g. scan or validation) failed
    OSCAP_ERR_FETCH      =   1, // cold not fetch input file (same as error for now)
    OSCAP_BADARGS        = 100, // bad commandline arguments
    OSCAP_BADMODULE      = 101, // unrecognized module
    OSCAP_UNIMPL_MOD     = 110, // module functionality not implemented
    OSCAP_UNIMPL         = 111, // functionality not implemented
    // end of list
    OSCAP_EXITCODES_END_ = 120  // any code returned shall not be higher than this
};

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
	char *f_results_stig;
	char *f_results_arf;
        char *f_report;
	char *f_variables;
	char *f_verbose_log;
	/* others */
        char *profile;
	struct oscap_stringlist *rules;
	struct oscap_stringlist *skip_rules;
        char *format;
        char *id;
        int hide_profile_info;
        char *stylesheet;
	char *tailoring_file;
	char *tailoring_id;
	char *cpe;

	struct ds_action* ds_action;
	struct cpe_action * cpe_action;
	struct cve_action * cve_action;
	struct cvrf_action * cvrf_action;
	char *file;

	int verbosity;
	int show_profiles_only;
	int provide_machine_readable_output;
	int doctype;
	int force;
	int validate;
	int validate_signature;
	int enforce_signature;
	int schematron;
	int remote_resources;
	int progress;
	int oval_results;
	int without_sys_chars;
	int thin_results;
	int remediate;
	int check_engine_results;
	int export_variables;
        int list_dynamic;
	char *verbosity_level;
	char *fix_type;
	char *local_files;
	char *reference;
	int references;
	int raw;
};

int app_xslt(const char *infile, const char *xsltfile, const char *outfile, const char **params);
int reporter(const char *file, int line, const char *msg, void *arg);

int oscap_module_process(struct oscap_module *module, int argc, char **argv);
bool oscap_module_usage(struct oscap_module *module, FILE *out, const char *err, ...);
int oscap_module_call(struct oscap_action *action);

void oscap_print_error(void);
bool check_verbose_options(struct oscap_action *action);
void download_reporting_callback(bool warning, const char *format, ...);

void report_missing_profile(const char *profile_suffix, const char *source_file);
void report_multiple_profile_matches(const char *profile_suffix, const char *source_file);

int xccdf_set_profile_or_report_bad_id(struct xccdf_session *session, const char *profile_id, const char *source_file);
int evaluate_suffix_match_result_with_custom_reports(int suffix_match_result, const char *profile_suffix, const char *source_file, void (* report_missing)(const char *, const char *), void (* report_multiple)(const char *, const char *));
int evaluate_suffix_match_result(int suffix_match_result, const char *profile_suffix, const char *source_file);

extern struct oscap_module OSCAP_ROOT_MODULE;
extern struct oscap_module OSCAP_DS_MODULE;
extern struct oscap_module OSCAP_XCCDF_MODULE;
extern struct oscap_module OSCAP_OVAL_MODULE;
extern struct oscap_module OSCAP_CVE_MODULE;
extern struct oscap_module OSCAP_CPE_MODULE;
extern struct oscap_module OSCAP_INFO_MODULE;

#ifndef HAVE_GETOPT_H

#define __getopt_argv_const const
#define no_argument		0
#define required_argument	1
#define optional_argument	2

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

struct option
{
	const char *name;
	int has_arg;
	int *flag;
	int val;
};

getopt_long(int ___argc, char *__getopt_argv_const *___argv,
	const char *__shortopts,
	const struct option *__longopts, int *__longind);

#endif

#endif //OSCAP_TOOL_H_
