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
#include <getopt.h>
#include <errno.h>

#include "oscap-tool.h"

static bool getopt_root(int argc, char **argv, struct oscap_action *action);
static int print_versions(const struct oscap_action*);
extern struct oscap_module OSCAP_VERSION_MODULE;

struct oscap_module* OSCAP_ROOT_SUBMODULES[] = {
    &OSCAP_DS_MODULE,
#ifdef ENABLE_OVAL
    &OSCAP_OVAL_MODULE,
#endif
#ifdef ENABLE_XCCDF
    &OSCAP_XCCDF_MODULE,
#endif
#ifdef ENABLE_CVSS
    &OSCAP_CVSS_MODULE,
#endif
#ifdef ENABLE_CPE
    &OSCAP_CPE_MODULE,
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
    oscap_init();
    int ret = oscap_module_process(&OSCAP_ROOT_MODULE, argc, argv);
    oscap_cleanup();
    return ret;
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
#ifdef ENABLE_CPE
	fprintf(stdout, "CPE Version: \r\t\t%s\n", cpe_dict_model_supported());
#endif
#ifdef ENABLE_CVSS
	fprintf(stdout, "CVSS Version: \r\t\t%s\n", cvss_model_supported());
#endif
    return OSCAP_OK;
}

int app_validate_xml(const struct oscap_action *action)
{
	const char *xml_file;
	xmlChar *doc_version;
	int ret;
	int result = OSCAP_ERROR;

	switch (action->doctype) {
	case OSCAP_DOCUMENT_OVAL_DEFINITIONS:
	case OSCAP_DOCUMENT_OVAL_VARIABLES:
	case OSCAP_DOCUMENT_OVAL_SYSCHAR:
	case OSCAP_DOCUMENT_OVAL_RESULTS:
	case OSCAP_DOCUMENT_OVAL_DIRECTIVES:
		xml_file = action->f_oval;
		doc_version = oval_determine_document_schema_version((const char *) xml_file, action->doctype);
		break;
	default:
		xml_file = action->f_xccdf;
		doc_version = xccdf_version_info_get_version(xccdf_detect_version(action->f_xccdf));
	}

	if (!xml_file) {
		fprintf(stderr, "Missing xml file.\n");
		goto cleanup;
	}

	if (!doc_version) {
		goto cleanup;
	}

	ret=oscap_validate_document(xml_file, action->doctype, (const char *) doc_version, (action->verbosity >= 0 ? oscap_reporter_fd : NULL), stdout);
	if (ret==1) {
		fprintf(stdout, "%s\n", INVALID_DOCUMENT_MSG);
		result=OSCAP_FAIL;
		goto cleanup;
	}
	else if (ret==-1) {
		goto cleanup;
	}

	// schematron-based validation forced
	if (action->force) {
		const char *std = "oval", *filename = NULL;

		switch (action->doctype) {
			case OSCAP_DOCUMENT_OVAL_DEFINITIONS: filename = "oval-definitions-schematron.xsl";            break;
			case OSCAP_DOCUMENT_OVAL_SYSCHAR:     filename = "oval-system-characteristics-schematron.xsl"; break;
			case OSCAP_DOCUMENT_OVAL_RESULTS:     filename = "oval-results-schematron.xsl";                break;
			case OSCAP_DOCUMENT_OVAL_VARIABLES:   filename = "oval-variables-schematron.xsl";              break;
			case OSCAP_DOCUMENT_OVAL_DIRECTIVES:  filename = "oval-directives-schematron.xsl";             break;
		}

		if (filename) {
			size_t buffsize = 1024;
			char xslfile[buffsize];
			snprintf(xslfile, buffsize, "%s%s%s%s%s", std, OSCAP_OS_PATH_DELIM, doc_version, OSCAP_OS_PATH_DELIM, filename);
			xslfile[buffsize - 1] = '\0';

			const char *params[] = { NULL };
			if (!oscap_apply_xslt_var(xml_file, xslfile, NULL, params, "OSCAP_SCHEMA_PATH", OSCAP_SCHEMA_PATH)) {
				fprintf(stderr, "%s\n", "Error during schematron validation.");
				goto cleanup;
			}
		}
		else {
			fprintf(stderr, "%s\n", "Could not find schematron validation file for this document type.");
			goto cleanup;
		}
	}

	result=OSCAP_OK;

cleanup:
	if (oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	return result;
}

