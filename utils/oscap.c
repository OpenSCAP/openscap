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
#include <errno.h>

#include "oscap-tool.h"

static bool getopt_root(int argc, char **argv, struct oscap_action *action);
static int print_versions(const struct oscap_action*);
extern struct oscap_module OSCAP_VERSION_MODULE;

struct oscap_module* OSCAP_ROOT_SUBMODULES[] = {
#ifdef ENABLE_OVAL
    &OSCAP_OVAL_MODULE,
#endif
#ifdef ENABLE_XCCDF
    &OSCAP_XCCDF_MODULE,
#endif
#ifdef ENABLE_CVSS
    &OSCAP_CVSS_MODULE,
#endif
    &OSCAP_VERSION_MODULE,
    NULL
};

struct oscap_module OSCAP_ROOT_MODULE = {
    .name = "oscap",
    .usage = "[general-options]",
    .usage_extra = "module operation [operation-options-and-arguments]",
    .summary = "OpenSCAP command-line tool",
    .help =
		"General options:\n"
		"   -h --help\r\t\t\t\t - show this help\n"
		"   -q --quiet\r\t\t\t\t - quiet mode\n"
		"   -V --version\r\t\t\t\t - print info about supported SCAP versions",
    .opt_parser = getopt_root,
    .submodules = OSCAP_ROOT_SUBMODULES
};

struct oscap_module OSCAP_VERSION_MODULE = {
    .name = "version",
    .hidden = true,
    .parent = &OSCAP_ROOT_MODULE,
    .func = print_versions
};

int main(int argc, char **argv)
{
    return oscap_module_process(&OSCAP_ROOT_MODULE, argc, argv);
}

bool getopt_root(int argc, char **argv, struct oscap_action *action)
{
	while (1) {
		static struct option long_options[] = {
			{"quiet", 0, 0, 'q'},
			{"version", 0, 0, 'V'},
			{0, 0, 0, 0}
		};

		int c = getopt_long(argc, argv, "+qV", long_options, NULL);
		if (c == -1) break;

		switch (c) {
		case 'q': action->verbosity = -1; break;
		case 'V': action->module = &OSCAP_VERSION_MODULE; break;
        case '?': return oscap_module_usage(action->module, stderr, NULL);
		}
	}
    return true;
}


static int print_versions(const struct oscap_action *action)
{
	fprintf(stdout,
		"OSCAP util (oscap) %s\n" "Copyright 2009,2010 Red Hat Inc., Durham, North Carolina.\n\n", oscap_get_version());
#ifdef ENABLE_XCCDF
	fprintf(stdout, "OVAL Version: \r\t\t%s\n", oval_definition_model_supported());
#endif
#ifdef ENABLE_XCCDF
	fprintf(stdout, "XCCDF Version: \r\t\t%s\n", xccdf_benchmark_supported());
#endif
#ifdef ENABLE_CVSS
	fprintf(stdout, "CVSS Version: \r\t\t%s\n", cvss_model_supported());
#endif
    return OSCAP_OK;
}

int app_validate_xml(const struct oscap_action *action)
{
	const char *xml_file = action->f_oval;
	if (!xml_file)
		xml_file = action->f_xccdf;
	if (!xml_file)
		return OSCAP_ERROR;

	if (!oscap_validate_document
	    (xml_file, action->doctype, action->file_version, (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout)) {
		if (oscap_err()) {
			fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			return OSCAP_FAIL;
		}
		return OSCAP_ERROR;
	}
	return OSCAP_OK;
}

