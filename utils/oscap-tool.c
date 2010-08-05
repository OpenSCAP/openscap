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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#include "oscap-tool.h"
#include <assert.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>

/* Header files for curl */
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

/**
 * @param url URL or PATH to file
 * @return path to local file
 */
static char *app_curl_download(char *url)
{

	struct stat buf;
	/* Is the file local ? */
	if (lstat(url, &buf) == 0)
		return strdup(url);

	/* Remote file will be stored in this xml */
	char *outfile = strdup("definition_file.xml");

	CURL *curl;
	FILE *fp;
	CURLcode res;
	/* Initialize CURL for download remote file */
	curl = curl_easy_init();
	if (!curl)
		return NULL;

	fp = fopen(outfile, "wb");
	/* Set options for download file to *fp* from *url* */
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);
	if (res != 0) {
		curl_easy_cleanup(curl);
		fclose(fp);
		return NULL;
	}

	curl_easy_cleanup(curl);
	fclose(fp);

	return outfile;
}

static void oscap_action_init(struct oscap_action *action)
{
	assert(action != NULL);
    memset(action, 0, sizeof(*action));
    action->verbosity = 1;
}

static void oscap_action_release(struct oscap_action *action)
{
	assert(action != NULL);
    free(action->f_xccdf);
    free(action->f_oval);
    free(action->urls_oval);
#ifdef ENABLE_CVSS
    free(action->cvss_metrics);
#endif
}

static bool oscap_fetch(struct oscap_module *module, char **dest, char *src)
{
    assert(module != NULL);
    assert(dest != NULL);

    if (src == NULL) return true;

    *dest = app_curl_download(src);
    if (*dest == NULL) oscap_module_usage(module, stderr, "Could not retrive '%s'", src);
    return *dest != NULL;
}

static bool oscap_action_postprocess(struct oscap_action *action)
{
    return oscap_fetch(action->module, &action->f_oval,  action->url_oval)
        && oscap_fetch(action->module, &action->f_xccdf, action->url_xccdf);
}


static void check_module(struct oscap_module *module)
{
    assert(module != NULL);
    assert(module->name != NULL);
    if (module != &OSCAP_ROOT_MODULE)
        assert(module->parent != NULL);
    if (module->submodules)
        for (struct oscap_module** sub = module->submodules; *sub != NULL; ++(sub))
            assert((*sub)->parent == module);
}

static void oscap_module_backtrace(struct oscap_module *module, FILE *out, const char *delim)
{
    assert(module != NULL);
    if (module->parent) {
        oscap_module_backtrace(module->parent, out, delim);
        fprintf(out, "%s", delim);
    }
    fprintf(out, "%s", module->name);
}

static void oscap_module_print_usage(struct oscap_module *module, FILE *out, bool full)
{
    assert(module != NULL);
    if (module->parent != NULL) {
        oscap_module_print_usage(module->parent, out, false);
        fprintf(out, " ");
    }
    fprintf(out, "%s", module->name);
    if (module->usage) fprintf(out, " %s", module->usage);
    if (full) {
        if (module->usage_extra) fprintf(out, " %s", module->usage_extra);
        else if (module->submodules) fprintf(out, " command");
    }
}

static inline void oscap_module_print_summary(struct oscap_module *module, FILE *out)
{
    assert(module != NULL);
    if (!module->hidden) {
        fprintf(out, "    %s", module->name);
        if (module->summary) fprintf(out, "\r\t\t\t\t - %s", module->summary);
        fprintf(out, "\n");
    }
}

static int oscap_print_submodules(struct oscap_module *module, FILE *out, const char *delim, bool all)
{
    if (module->submodules) {
        for (struct oscap_module** sub = module->submodules; *sub != NULL; ++(sub))
            if (all || !(*sub)->hidden)
                fprintf(out, "%s%s", (*sub)->name, delim);
        return 0;
    }
    return 1;
}

bool oscap_module_usage(struct oscap_module *module, FILE *out, const char *err, ...)
{
    assert(module != NULL);

    if (err) {
        va_list ap;
        fprintf(out, "ERROR:    ");
        va_start(ap, err);
        vfprintf(out, err, ap);
        va_end(ap);
        fprintf(out, "\n");
    }
    fprintf(out, "Usage:    ");
    oscap_module_print_usage(module, out, true);
    if (module->submodules) { fprintf(out, "\nCommands: "); oscap_print_submodules(module, out, " ", false); }
    fprintf(out, "\nHelp:     ");
    oscap_module_backtrace(module, out, " ");
    fprintf(out, " -h\n");
    return (out != stderr);
}

static void oscap_module_print_help(struct oscap_module *module, FILE *out)
{
    assert(module != NULL);
    assert(out != NULL);

    oscap_module_backtrace(module, out, " -> ");
    fprintf(out, "\n\n");

    if (module->summary) fprintf(out, "%s\n\n", module->summary);

    fprintf(out, "Usage: ");
    oscap_module_print_usage(module, out, true);
    fprintf(out, "\n\n");

    if (module->help) fprintf(out, "%s\n\n", module->help);

    if (module->submodules) {
        fprintf(out, "Commands:\n");
        for (struct oscap_module** sub = module->submodules; *sub != NULL; ++(sub))
            oscap_module_print_summary(*sub, out);
        fprintf(out, "\n");
    }
}

static struct oscap_module *oscap_module_find(struct oscap_module **list, const char *name)
{
    if (name == NULL) return NULL;
    if (list == NULL) return NULL;
    while (*list != NULL && strcmp((*list)->name, name) != 0) ++list;
    return *list;
}

enum oscap_common_opts {
    OPT_NONE,
    OPT_LISTMODS,
    OPT_LISTALLMODS,
    OPT_HELP = 'h'
};

static enum oscap_common_opts oscap_parse_common_opts(int argc, char **argv)
{
    static const struct option opts[] = {
        { "help",                0, 0, OPT_HELP        },
        { "list-submodules",     0, 0, OPT_LISTMODS    },
        { "list-all-submodules", 0, 0, OPT_LISTALLMODS },
        { 0, 0, 0, 0 }
    };

    int optind_bak = optind;
    int opterr_bak = opterr;
    opterr = 0;
    int r = getopt_long(argc, argv, "+h", opts, NULL);
    opterr = opterr_bak;
    switch (r) {
        case -1: case '?': optind = optind_bak; return OPT_NONE;
        default: return r;
    }
}

int oscap_module_call(struct oscap_action *action)
{
    assert(action != NULL);
    assert(action->module != NULL);

    if (action->module->func) {
        if (oscap_action_postprocess(action))
            return action->module->func(action);
        else return 120;
    }
    return 123;
}

int oscap_module_process(struct oscap_module *module, int argc, char **argv)
{
    assert(module != NULL);
    assert(argv != NULL);

    int ret = 0;
    struct oscap_action action;
    oscap_action_init(&action);
    optind = 0;

    while (module) {
        check_module(module);

        ++optind; // skip current module key index
        action.module = module; // update current module

        switch (oscap_parse_common_opts(argc, argv)) {
            case OPT_HELP: oscap_module_print_help(module, stdout); goto cleanup;
            case OPT_LISTMODS: oscap_print_submodules(module, stdout, "\n", false); goto cleanup;
            case OPT_LISTALLMODS: oscap_print_submodules(module, stdout, "\n", true); goto cleanup;
            default: break;
        }

        if (module->opt_parser) {
            if (!module->opt_parser(argc, argv, &action)) {
                ret = 120;
                goto cleanup;
            }
            module = action.module; // module might have changed
        }

        if (module->func) {
            ret = oscap_module_call(&action);
            goto cleanup;
        }
        else if (module->submodules) {
            struct oscap_module *old_mod = module;
            module = oscap_module_find(module->submodules, argv[optind]);
            if (module == NULL) {
                if (argv[optind] != NULL)
                    fprintf(stderr, "No such module: %s\n", argv[optind]);
                else oscap_module_usage(old_mod, stderr, NULL);
                ret = 121;
            }
        }
        else {
            fprintf(stderr, "Module %s does not do anything\n", module->name);
            ret = 122;
            goto cleanup;
        }
    }

cleanup:
    oscap_action_release(&action);
    return ret;
}


