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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "oscap-tool.h"
#include <assert.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <cvss_score.h>
#include <oscap_debug.h>
#include "util.h"

#ifndef PATH_MAX
# define PATH_MAX 1024
#else
# if PATH_MAX < 1 || PATH_MAX > 4096
#  undef PATH_MAX
#  define PATH_MAX 2048
# endif
#endif /* PATH_MAX */

static void oscap_action_init(struct oscap_action *action)
{
    assert(action != NULL);
    memset(action, 0, sizeof(*action));
    action->validate = 1;
}

static void oscap_action_release(struct oscap_action *action)
{
	assert(action != NULL);
	free(action->f_ovals);
	cvss_impact_free(action->cvss_impact);
}

static size_t paramlist_size(const char **p) { size_t s = 0; if (!p) return s; while (p[s]) s += 2; return s; }

static size_t paramlist_cpy(const char **to, const char **p) {
    size_t s = 0;
    if (!p) return s;
    for (;p && p[s]; s += 2) to[s] = p[s], to[s+1] = p[s+1];
    to[s] = p[s];
    return s;
}

int app_xslt(const char *infile, const char *xsltfile, const char *outfile, const char **params)
{
	char pwd[PATH_MAX];

	if (getcwd(pwd, sizeof(pwd)) == NULL) {
		fprintf(stderr, "ERROR: %s\n", strerror(errno));
		return OSCAP_ERROR;
	}

	/* add params oscap-version & pwd */
	const char *stdparams[] = { "oscap-version", oscap_get_version(), "pwd", pwd, NULL };
	const size_t par_size = paramlist_size(params) + paramlist_size(stdparams) + 1;
	const char **par = malloc(par_size * sizeof(const char *));
	size_t s  = paramlist_cpy(par    , params);
        paramlist_cpy(par + s, stdparams);

	if (oscap_apply_xslt(infile, xsltfile, outfile, par)==-1) {
		fprintf(stderr, "%s: %s\n", OSCAP_ERR_MSG, oscap_err_desc());
		free(par);
		return OSCAP_ERROR;
	}

	free(par);
	return OSCAP_OK;
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
        if (module->summary) fprintf(out, " - %s", module->summary);
        fprintf(out, "\n");
    }
}

static int oscap_print_submodules(struct oscap_module *module, FILE *out, const char *delim, bool all)
{
    if (module->submodules) {
        for (struct oscap_module** sub = module->submodules; *sub != NULL; ++(sub))
            if (all || !(*sub)->hidden)
                fprintf(out, "%s%s", (*sub)->name, delim);
        return OSCAP_OK;
    }
    return OSCAP_BADMODULE;
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

static const char *common_opts_help =
	"Common options:\n"
	"   --verbose <verbosity_level>   - Turn on verbose mode at specified verbosity level.\n"
	"   --verbose-log-file <file>     - Write verbose informations into file.\n";

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

	fprintf(out, "%s\n", common_opts_help);
	if (module->help)
		fprintf(out, "%s\n\n", module->help);

    if (module->submodules) {
        fprintf(out, "Commands:\n");
        for (struct oscap_module** sub = module->submodules; *sub != NULL; ++(sub))
            oscap_module_print_summary(*sub, out);
        fprintf(out, "\n");
    }
}

static void oscap_module_tree(struct oscap_module *module, FILE* out, int indent)
{
    assert(module);
    if (module->hidden) return;
    for (int i = 0; i < indent; ++i) fprintf(out, "    ");
    fprintf(out, "%s - %s\n", module->name, module->summary);
    if (module->submodules) {
        for (struct oscap_module** sub = module->submodules; *sub != NULL; ++(sub))
            oscap_module_tree(*sub, out, indent + 1);
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
    OPT_MODTREE,
	OPT_HELP = 'h',
	OPT_VERBOSE,
	OPT_VERBOSE_LOG_FILE
};

static enum oscap_common_opts oscap_parse_common_opts(int argc, char **argv, struct oscap_action *action)
{
    static const struct option opts[] = {
        { "help",                0, 0, OPT_HELP        },
        { "list-submodules",     0, 0, OPT_LISTMODS    },
        { "list-all-submodules", 0, 0, OPT_LISTALLMODS },
        { "module-tree",         0, 0, OPT_MODTREE     },
		{ "verbose", required_argument, NULL, OPT_VERBOSE },
		{ "verbose-log-file", required_argument, NULL, OPT_VERBOSE_LOG_FILE },
        { 0, 0, 0, 0 }
    };

	int r;
	int optind_bak = optind;
	int opterr_bak = opterr;
	opterr = 0;
	while ((r = getopt_long(argc, argv, "+h", opts, NULL)) != -1) {
		switch (r) {
		case OPT_VERBOSE:
			optind_bak += 2;
			action->verbosity_level = optarg;
			break;
		case OPT_VERBOSE_LOG_FILE:
			optind_bak += 2;
			action->f_verbose_log = optarg;
			break;
		case 0:
			break;
		case '?':
			optind = optind_bak;
			opterr = opterr_bak;
			return OPT_NONE;
		default:
			return r;
		}
	}
	return OPT_NONE;
}

int oscap_module_call(struct oscap_action *action)
{
    assert(action != NULL);
    assert(action->module != NULL);

    if (action->module->func) {
		return action->module->func(action);
    }
    return OSCAP_UNIMPL_MOD;
}

static void getopt_parse_env(struct oscap_module *module, int *argc, char ***argv)
{
	int ofs, nargc, eargc;
	char *opt, *opts, *state, **nargv, **eargv;
	const char *delim = ",";

	if (!module->parent
	    || strcmp(module->parent->name, "oval"))
		return;

	opts = getenv("OSCAP_OVAL_COMMAND_OPTIONS");
	if (opts == NULL)
		return;

	/* parse the args supplied from the env var */
	eargv = NULL;
	eargc = 0;
	opts = strdup(opts);
	opt = oscap_strtok_r(opts, delim, &state);
	while (opt != NULL) {
		eargc++;
		eargv = realloc(eargv, eargc * sizeof(char *));
		eargv[eargc - 1] = strdup(opt);
		opt = oscap_strtok_r(NULL, delim, &state);
	}

	nargc = *argc + eargc;
	nargv = malloc((nargc + 1) * sizeof(char *));
	/* copy main args up to the command name */
	for (ofs = 0; strcmp((*argv)[ofs], OSCAP_OVAL_MODULE.name); ofs++);
	ofs += 2;
	memcpy(nargv, *argv, ofs * sizeof(char *));
	/* copy env args */
	memcpy(nargv + ofs, eargv, eargc * sizeof(char *));
	/* copy rest of the main args */
	memcpy(nargv + ofs + eargc, *argv + ofs, (*argc - ofs) * sizeof(char *));
	nargv[nargc] = NULL;

	*argc = nargc;
	*argv = nargv;
	free(opts);
	free(eargv);
}

int oscap_module_process(struct oscap_module *module, int argc, char **argv)
{
    assert(module != NULL);
    assert(argv != NULL);

    int ret = OSCAP_OK;
    struct oscap_action action;
    oscap_action_init(&action);
    optind = 0;

    while (module) {
        check_module(module);

        ++optind; // skip current module key index
        action.module = module; // update current module

	getopt_parse_env(module, &argc, &argv);

        switch (oscap_parse_common_opts(argc, argv, &action)) {
            case OPT_HELP: oscap_module_print_help(module, stdout); goto cleanup;
            case OPT_LISTMODS: oscap_print_submodules(module, stdout, "\n", false); goto cleanup;
            case OPT_LISTALLMODS: oscap_print_submodules(module, stdout, "\n", true); goto cleanup;
            case OPT_MODTREE: oscap_module_tree(module, stdout, 0); goto cleanup;
            default: break;
        }

        if (module->opt_parser) {
            if (!module->opt_parser(argc, argv, &action)) {
                ret = OSCAP_BADARGS;
                goto cleanup;
            }
            module = action.module; // module might have changed
        }

        if (module->func) {
			if (!check_verbose_options(&action)) {
				goto cleanup;
			}
			if (!oscap_set_verbose(action.verbosity_level, action.f_verbose_log)) {
				goto cleanup;
			}
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
                ret = OSCAP_BADMODULE;
            }
        }
        else {
            fprintf(stderr, "Module %s does not do anything\n", module->name);
            ret = OSCAP_UNIMPL_MOD;
            goto cleanup;
        }
    }

cleanup:
    oscap_action_release(&action);
    return ret;
}

void oscap_print_error(void)
{
	if (oscap_err()) {
		char *err = oscap_err_get_full_error();
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, err);
		free(err);
	}
}

bool check_verbose_options(struct oscap_action *action)
{
	if (action->verbosity_level == NULL && action->f_verbose_log != NULL) {
		oscap_module_usage(action->module, stderr,
			"Verbosity level is not specified! Please provide --verbose VERBOSITY_LEVEL option together with --verbose-log-file.");
		return false;
	}
	if (action->verbosity_level != NULL && oscap_verbosity_level_from_cstr(action->verbosity_level) == -1) {
		oscap_module_usage(action->module, stderr,
			"Invalid verbosity level! Verbosity level must be one of: DEVEL, INFO, WARNING, ERROR.");
		return false;
	}
	return true;
}

void download_reporting_callback(bool warning, const char *format, ...)
{
	FILE *dest = stderr;
	va_list argptr;
	va_start(argptr, format);
	vfprintf(dest, format, argptr);
	va_end(argptr);
	fflush(dest);
}
