/*
 * Copyright 2010,2011,2012 Red Hat Inc., Durham, North Carolina.
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
#include <errno.h>
#include <limits.h>

#include "oscap-tool.h"

static bool getopt_root(int argc, char **argv, struct oscap_action *action);
static int print_versions(const struct oscap_action*);
extern struct oscap_module OSCAP_VERSION_MODULE;

struct oscap_module* OSCAP_ROOT_SUBMODULES[] = {
    &OSCAP_DS_MODULE,
    &OSCAP_OVAL_MODULE,
    &OSCAP_XCCDF_MODULE,
    &OSCAP_CVSS_MODULE,
    &OSCAP_CPE_MODULE,
    &OSCAP_CVE_MODULE,
    &OSCAP_VERSION_MODULE,
    &OSCAP_INFO_MODULE,
    NULL
};

struct oscap_module OSCAP_ROOT_MODULE = {
    .name = "oscap",
    .usage = "[options]",
    .usage_extra = "module operation [operation-options-and-arguments]",
    .summary = "OpenSCAP command-line tool",
    .help =
		"oscap options:\n"
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
		case 'q':
			printf("Warning: '-q' is obsoleted option, please use '#oscap ... >/dev/null 2>&1' instead\n");
		break;
		case 'V': action->module = &OSCAP_VERSION_MODULE; break;
        case '?': return oscap_module_usage(action->module, stderr, NULL);
		}
	}
    return true;
}


static int print_versions(const struct oscap_action *action)
{
	printf("OSCAP util (oscap) %s\n" "Copyright 2009-2012 Red Hat Inc., Durham, North Carolina.\n\n", oscap_get_version());

	printf("==== Supported specifications ====\n");
	printf("XCCDF Version: %s\n", xccdf_benchmark_supported());
	printf("OVAL Version: %s\n", oval_definition_model_supported());
	printf("CPE Version: %s\n", cpe_dict_model_supported());
	printf("CVSS Version: %s\n", cvss_model_supported());
	printf("CVE Version: %s\n", cve_model_supported());
	printf("Asset Identification Version: %s\n", "1.1");
	printf("Asset Reporting Format Version: %s\n", "1.1");
#ifdef ENABLE_SCE
	printf("Script check engine: %s\n", "1.0");
#endif
	printf("\n");

	printf("==== Paths ====\n");
	printf("Schema files: %s\n", oscap_path_to_schemas());
	printf("Schematron files: %s\n", oscap_path_to_schematron());
	printf("Default CPE files: %s\n", oscap_path_to_cpe());
	printf("Probes: %s\n", oval_probe_ext_getdir());
	printf("\n");

	printf("==== Inbuilt CPE names ====\n");
	char default_cpe_path[PATH_MAX];
	snprintf(default_cpe_path, PATH_MAX, "%s/openscap-cpe-dict.xml", oscap_path_to_cpe());
	struct cpe_dict_model* cpe_dict = cpe_dict_model_import(default_cpe_path);

	struct cpe_item_iterator* cpe_items = cpe_dict_model_get_items(cpe_dict);
	while (cpe_item_iterator_has_more(cpe_items))
	{
		struct cpe_item* cpe_item = cpe_item_iterator_next(cpe_items);

		struct oscap_text_iterator* titles = cpe_item_get_titles(cpe_item);
		char* str_title = oscap_textlist_get_preferred_plaintext(titles, NULL);
		oscap_text_iterator_free(titles);

		struct cpe_name* name = cpe_item_get_name(cpe_item);
		char * str_name = cpe_name_get_as_format(name, CPE_FORMAT_URI);

		printf("%s - %s\n", str_title, str_name);

		free(str_name);
		free(str_title);
	}
	cpe_item_iterator_free(cpe_items);
	cpe_dict_model_free(cpe_dict);
	printf("\n");

	printf("==== Supported OVAL objects and associated OpenSCAP probes ====\n");
	oval_probe_meta_list(stdout, OVAL_PROBEMETA_LIST_DYNAMIC);

	return OSCAP_OK;
}

void validation_failed(const char *xmlfile, oscap_document_type_t doc_type, const char *version) {

	char *doc_name;

	switch (doc_type) {
        case OSCAP_DOCUMENT_OVAL_DEFINITIONS:
		doc_name = "OVAL Definition";
                break;
        case OSCAP_DOCUMENT_OVAL_DIRECTIVES:
		doc_name = "OVAL Directives";
                break;
        case OSCAP_DOCUMENT_OVAL_RESULTS:
		doc_name = "OVAL Results";
                break;
        case OSCAP_DOCUMENT_OVAL_SYSCHAR:
		doc_name = "OVAL Definitions";
                break;
        case OSCAP_DOCUMENT_OVAL_VARIABLES:
		doc_name = "OVAL Variables";
                break;
        case OSCAP_DOCUMENT_SDS:
		doc_name = "SCAP Source Datastream";
                break;
        case OSCAP_DOCUMENT_XCCDF:
		doc_name = "XCCDF Checklist";
                break;
        case OSCAP_DOCUMENT_SCE_RESULT:
		doc_name = "SCE Results";
                break;
        case OSCAP_DOCUMENT_CPE_DICTIONARY:
		doc_name = "CPE Dictionary";
                break;
        case OSCAP_DOCUMENT_CPE_LANGUAGE:
		doc_name = "CPE Language";
                break;
        case OSCAP_DOCUMENT_ARF:
		doc_name = "ARF Result Datastream";
                break;
        case OSCAP_DOCUMENT_CVE_FEED:
		doc_name = "CVE NVD Feed";
                break;
        default:
		fprintf(stdout, "Unrecognized document type.\n");
		return;
	}

	fprintf(stdout, "Invalid %s content(%s) in %s.\n", doc_name, version, xmlfile);
}

int reporter(const char *file, int line, const char *msg, void *arg) {
	fprintf(stdout, "File '%s' line %d: %s", file, line, msg);
	return 0;
}

