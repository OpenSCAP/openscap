/*
 * Copyright 2010--2015 Red Hat Inc., Durham, North Carolina.
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
#include "check_engine_plugin.h"
#include "oscap_source.h"

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
    abort();
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
	printf("OpenSCAP command line tool (oscap) %s\n" "Copyright 2009--2015 Red Hat Inc., Durham, North Carolina.\n\n",
		oscap_get_version());

	printf("==== Supported specifications ====\n");
	printf("XCCDF Version: %s\n", xccdf_benchmark_supported());
	printf("OVAL Version: %s\n", oval_definition_model_supported());
	printf("CPE Version: %s\n", cpe_dict_model_supported());
	printf("CVSS Version: %s\n", cvss_model_supported());
	printf("CVE Version: %s\n", cve_model_supported());
	printf("Asset Identification Version: %s\n", "1.1");
	printf("Asset Reporting Format Version: %s\n", "1.1");
	printf("\n");

	printf("==== Capabilities added by auto-loaded plugins ====\n");

	const char * const *known_plugins = check_engine_plugin_get_known_plugins();
	bool known_plugin_found = false;
	while (*known_plugins) {
		struct check_engine_plugin_def *plugin = check_engine_plugin_load(*known_plugins);
		if (plugin) {
			printf("%s (from %s)\n", check_engine_plugin_get_capabilities(plugin), *known_plugins);
			check_engine_plugin_unload(plugin);
			known_plugin_found = true;
		}
		known_plugins++;
	}

	if (!known_plugin_found)
		printf("No plugins have been auto-loaded...\n");

	// We do not report failure when a known plugin doesn't load properly, that's because they
	// are optional and we don't know if it's not there or if it just failed to load.
	oscap_clearerr();

	printf("\n");

	printf("==== Paths ====\n");
	printf("Schema files: %s\n", oscap_path_to_schemas());
	printf("Default CPE files: %s\n", oscap_path_to_cpe());
	printf("Probes: %s\n", oval_probe_ext_getdir());
	printf("\n");

	printf("==== Inbuilt CPE names ====\n");
	char default_cpe_path[PATH_MAX];
	snprintf(default_cpe_path, PATH_MAX, "%s/openscap-cpe-dict.xml", oscap_path_to_cpe());
	struct oscap_source *source = oscap_source_new_from_file(default_cpe_path);
	struct cpe_dict_model* cpe_dict = cpe_dict_model_import_source(source);
	oscap_source_free(source);
	if (cpe_dict != NULL) {

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
	}
	printf("\n");

	printf("==== Supported OVAL objects and associated OpenSCAP probes ====\n");
	oval_probe_meta_list(stdout, OVAL_PROBEMETA_LIST_DYNAMIC);

	return OSCAP_OK;
}

int reporter(const char *file, int line, const char *msg, void *arg) {
	fprintf(stderr, "File '%s' line %d: %s", file, line, msg);
	return 0;
}

