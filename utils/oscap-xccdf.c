/*
 * Copyright 2010,2011 Red Hat Inc., Durham, North Carolina.
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
 *      Peter Vrabec   <pvrabec@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <oval_probe.h>
#include <oval_agent_api.h>
#include <oval_agent_xccdf_api.h>
#include <oval_results.h>
#include <oval_variables.h>

#include <scap_ds.h>
#include <xccdf_benchmark.h>
#include <xccdf_policy.h>

#include <cpe_dict.h>
#include <cpe_lang.h>

#ifdef ENABLE_SCE
#include <sce_engine_api.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <syslog.h>

#include "oscap-tool.h"
#include "oscap.h"
#include "oscap_acquire.h"

static int app_evaluate_xccdf(const struct oscap_action *action);
static int app_xccdf_validate(const struct oscap_action *action);
static int app_xccdf_resolve(const struct oscap_action *action);
static int app_xccdf_export_oval_variables(const struct oscap_action *action);
static bool getopt_xccdf(int argc, char **argv, struct oscap_action *action);
static bool getopt_generate(int argc, char **argv, struct oscap_action *action);
static int xccdf_gen_report(const char *infile, const char *id, const char *outfile, const char *show, const char *oval_template, const char* sce_template, const char* profile);
static int app_xccdf_xslt(const struct oscap_action *action);

static struct oscap_module* XCCDF_SUBMODULES[];
static struct oscap_module* XCCDF_GEN_SUBMODULES[];

struct oscap_module OSCAP_XCCDF_MODULE = {
    .name = "xccdf",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "eXtensible Configuration Checklist Description Format",
    .usage_extra = "command [command-specific-options]",
    .submodules = XCCDF_SUBMODULES
};

static struct oscap_module XCCDF_RESOLVE = {
    .name = "resolve",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Resolve an XCCDF document",
    .usage = "[options] -o output-xccdf.xml input-xccdf.xml",
    .help =
        "Options:\n"
        "   --force or -f\r\t\t\t\t - Force resolving XCCDF document even if it is aleready marked as resolved.",
    .opt_parser = getopt_xccdf,
    .func = app_xccdf_resolve
};

static struct oscap_module XCCDF_VALIDATE_XML = {
    .name = "validate-xml",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Validate XCCDF XML content",
    .usage = "xccdf-file.xml",
    .opt_parser = getopt_xccdf,
    .func = app_xccdf_validate
};

static struct oscap_module XCCDF_VALIDATE = {
    .name = "validate",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Validate XCCDF XML content",
    .usage = "xccdf-file.xml",
    .opt_parser = getopt_xccdf,
    .func = app_xccdf_validate
};

static struct oscap_module XCCDF_EXPORT_OVAL_VARIABLES = {
    .name = "export-oval-variables",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Export XCCDF values as OVAL external-variables document(s)",
    .usage = "[options] <xccdf benchmark file> [oval definitions files]",
    .opt_parser = getopt_xccdf,
    .func = app_xccdf_export_oval_variables,
};

static struct oscap_module XCCDF_EVAL = {
    .name = "eval",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Perform evaluation driven by XCCDF file and use OVAL as checking engine",
    .usage = "[options] INPUT_FILE [oval-definitions-files]",
    .help =
		"INPUT_FILE - XCCDF file or a source data stream file\n\n"
        "Options:\n"
        "   --profile <name>\r\t\t\t\t - The name of Profile to be evaluated.\n"
        "   --cpe <name>\r\t\t\t\t - Use given CPE dictionary or language (autodetected)\n"
        "               \r\t\t\t\t   for applicability checks.\n"
        "   --oval-results\r\t\t\t\t - Save OVAL results as well.\n"
#ifdef ENABLE_SCE
        "   --sce-results\r\t\t\t\t - Save SCE results as well.\n"
#endif
        "   --export-variables\r\t\t\t\t - Export OVAL external variables provided by XCCDF.\n"
        "   --results <file>\r\t\t\t\t - Write XCCDF Results into file.\n"
        "   --results-arf <file>\r\t\t\t\t - Write ARF (result data stream) into file.\n"
        "   --report <file>\r\t\t\t\t - Write HTML report into file.\n"
        "   --skip-valid \r\t\t\t\t - Skip validation.\n"
	"   --fetch-remote-resources \r\t\t\t\t - Download remote content referenced by XCCDF.\n"
        "   --datastream-id <id> \r\t\t\t\t - ID of the datastream in the collection to use.\n"
        "                        \r\t\t\t\t   (only applicable for source datastreams)\n"
        "   --xccdf-id <id> \r\t\t\t\t - ID of XCCDF in the datastream that should be evaluated.\n"
        "                   \r\t\t\t\t   (only applicable for source datastreams)",
    .opt_parser = getopt_xccdf,
    .func = app_evaluate_xccdf
};

#define GEN_OPTS \
        "Generate options:\n" \
        "   --profile <profile-id>\r\t\t\t\t - Tailor XCCDF file with respect to a profile.\n" \
        "   --format <fmt>\r\t\t\t\t - Select output format. Can be html or docbook.\n"

static struct oscap_module XCCDF_GENERATE = {
    .name = "generate",
    .parent = &OSCAP_XCCDF_MODULE,
    .summary = "Convert XCCDF Benchmark to other formats",
    .usage = "[options]",
    .usage_extra = "<subcommand> [sub-options] benchmark-file.xml",
    .help = GEN_OPTS,
    .opt_parser = getopt_generate,
    .submodules = XCCDF_GEN_SUBMODULES
};

static struct oscap_module XCCDF_GEN_REPORT = {
    .name = "report",
    .parent = &XCCDF_GENERATE,
    .summary = "Generate results report",
    .usage = "[options] xccdf-file.xml",
    .help = GEN_OPTS
        "\nReport Options:\n"
        "   --result-id <id>\r\t\t\t\t - TestResult ID to be processed. Default is the most recent one.\n"
        "   --show <result-type*>\r\t\t\t\t - Rule results to show. Defaults to everything but notselected and notapplicable.\n"
        "   --output <file>\r\t\t\t\t - Write the document into file.\n"
        "   --oval-template <template-string> - Template which will be used to obtain OVAL result files.\n",
    .opt_parser = getopt_xccdf,
    .user = "xccdf-report.xsl",
    .func = app_xccdf_xslt
};

static struct oscap_module XCCDF_GEN_GUIDE = {
    .name = "guide",
    .parent = &XCCDF_GENERATE,
    .summary = "Generate security guide",
    .usage = "[options] xccdf-file.xml",
    .help = GEN_OPTS
        "\nGuide Options:\n"
        "   --output <file>\r\t\t\t\t - Write the document into file.\n"
        "   --hide-profile-info\r\t\t\t\t - Do not output additional information about selected profile.\n",
    .opt_parser = getopt_xccdf,
    .user = "security-guide.xsl",
    .func = app_xccdf_xslt
};

static struct oscap_module XCCDF_GEN_FIX = {
    .name = "fix",
    .parent = &XCCDF_GENERATE,
    .summary = "Generate a fix script from an XCCDF file",
    .usage = "[options] xccdf-file.xml",
    .help = GEN_OPTS
        "\nFix Options:\n"
        "   --output <file>\r\t\t\t\t - Write the script into file.\n"
        "   --result-id <id>\r\t\t\t\t - Fixes will be generated for failed rule-results of the specified TestResult.\n"
        "   --template <id|filename>\r\t\t\t\t - Fix template. (default: bash)\n",
    .opt_parser = getopt_xccdf,
    .user = "fix.xsl",
    .func = app_xccdf_xslt
};

static struct oscap_module XCCDF_GEN_CUSTOM = {
    .name = "custom",
    .parent = &XCCDF_GENERATE,
    .summary = "Generate a custom output (depending on given XSLT file) from an XCCDF file",
    .usage = "--stylesheet <file> [--output <file>] xccdf-file.xml",
    .help = GEN_OPTS
        "\nCustom Options:\n"
        "   --stylesheet <file>\r\t\t\t\t - Specify an absolute path to a custom stylesheet to format the output.\n"
        "   --output <file>\r\t\t\t\t - Write the document into file.\n",
    .opt_parser = getopt_xccdf,
    .user = NULL,
    .func = app_xccdf_xslt
};

static struct oscap_module* XCCDF_GEN_SUBMODULES[] = {
    &XCCDF_GEN_REPORT,
    &XCCDF_GEN_GUIDE,
    &XCCDF_GEN_FIX,
    &XCCDF_GEN_CUSTOM,
    NULL
};

static struct oscap_module* XCCDF_SUBMODULES[] = {
    &XCCDF_EVAL,
    &XCCDF_RESOLVE,
    &XCCDF_VALIDATE,
    &XCCDF_VALIDATE_XML,
    &XCCDF_EXPORT_OVAL_VARIABLES,
    &XCCDF_GENERATE,
    NULL
};

/**
 * XCCDF Result Colors:
 * PASS:green(32), FAIL:red(31), ERROR:lred(1;31), UNKNOWN:grey(1;30), NOT_APPLICABLE:white(1;37), NOT_CHECKED:white(1;37),
 * NOT_SELECTED:white(1;37), INFORMATIONAL:blue(34), FIXED:yellow(1;33)
 */
static const char * RESULT_COLORS[] = {"", "32", "31", "1;31", "1;30", "1;37", "1;37", "1;37", "34", "1;33" };

static char custom_stylesheet_path[PATH_MAX];

static int callback_scr_rule(struct xccdf_rule *rule, void *arg)
{
	const char * rule_id = xccdf_rule_get_id(rule);

	/* is rule selected? we print only selected rules */
        const struct xccdf_select *sel = xccdf_policy_get_select_by_id((struct xccdf_policy *) arg, rule_id);
        if (!xccdf_select_get_selected(sel))
		return 0;

	/* get the first title */
	const char * title = NULL;
	struct oscap_text_iterator * title_it = xccdf_rule_get_title(rule);
	if (oscap_text_iterator_has_more(title_it))
		title = oscap_text_get_text(oscap_text_iterator_next(title_it));
	oscap_text_iterator_free(title_it);

	/* get the first ident */
	const char * ident_id = NULL;
	struct xccdf_ident_iterator *idents = xccdf_rule_get_idents(rule);
	if (xccdf_ident_iterator_has_more(idents)) {
		const struct xccdf_ident *ident = xccdf_ident_iterator_next(idents);
		ident_id = xccdf_ident_get_id(ident);
	}
	xccdf_ident_iterator_free(idents);

	/* print */
	if (isatty(1))
		printf("Title\r\t\033[1m%s\033[0;0m\n", title);
	else
		printf("Title\r\t%s\n", title);
	printf("Rule\r\t%s\n", rule_id);
	printf("Ident\r\t%s\n", ident_id);
	printf("Result\r\t");
	fflush(stdout);

	return 0;
}

static int callback_scr_result(struct xccdf_rule_result *rule_result, void *arg)
{
	xccdf_test_result_type_t result = xccdf_rule_result_get_result(rule_result);

	/* is result from selected rule? we print only selected rules */
	if (result == XCCDF_RESULT_NOT_SELECTED)
		return 0;

	/* print result */
	const char * result_str = xccdf_test_result_type_get_text(result);
	if (isatty(1))
		printf("\033[%sm%s\033[0m\n\n", RESULT_COLORS[result], result_str);
	else
		printf("%s\n\n", result_str);

	return 0;
}

static int callback_syslog_result(struct xccdf_rule_result *rule_result, void *arg)
{
	xccdf_test_result_type_t result = xccdf_rule_result_get_result(rule_result);

	/* do we log it? */
	if ((result != XCCDF_RESULT_FAIL) && (result != XCCDF_RESULT_UNKNOWN))
		return 0;

	/* yes we do */
	const char * result_str = xccdf_test_result_type_get_text(result);
	const char * ident_id = NULL;
	int priority = LOG_NOTICE;

	/* get ident */
	struct xccdf_ident_iterator *idents = xccdf_rule_result_get_idents(rule_result);
	if (xccdf_ident_iterator_has_more(idents)) {
		const struct xccdf_ident *ident = xccdf_ident_iterator_next(idents);
		ident_id = xccdf_ident_get_id(ident);
	}
	xccdf_ident_iterator_free(idents);

	/* emit the message */
	syslog(priority, "Rule: %s, Ident: %s, Result: %s.", xccdf_rule_result_get_idref(rule_result), ident_id, result_str);

	return 0;
}

struct oscap_content_resource {
	char *href;	/* Coresponds with xccdf:check-content-ref/@href. */
	char *filename; /* Points to the filename on the filesystem. */
};

static void
oscap_content_resources_free(struct oscap_content_resource **resources)
{
	if (resources) {
		for (int i=0; resources[i]; i++) {
			free(resources[i]->filename);
			free(resources[i]->href);
			free(resources[i]);
		}
		free(resources);
	}
}

static struct oscap_content_resource **
xccdf_policy_get_oval_resources(struct xccdf_policy_model *policy_model, bool allow_remote_resources, const char *path, char **temp_dir)
{
	struct oscap_content_resource **resources = NULL;
	struct oscap_file_entry_list *files = NULL;
	struct oscap_file_entry_iterator *files_it = NULL;
	int idx = 0;
	char *tmp_path;
	char *printable_path;
	bool fetch_option_suggested = false;

	resources = malloc(sizeof(struct oscap_content_resource *));
	resources[idx] = NULL;

	files = xccdf_policy_model_get_systems_and_files(policy_model);
	files_it = oscap_file_entry_list_get_files(files);
	while (oscap_file_entry_iterator_has_more(files_it)) {
		struct oscap_file_entry *file_entry;
		struct stat sb;

		file_entry = (struct oscap_file_entry *) oscap_file_entry_iterator_next(files_it);

		// we only care about OVAL referenced files
		if (strcmp(oscap_file_entry_get_system(file_entry), "http://oval.mitre.org/XMLSchema/oval-definitions-5"))
			continue;

		tmp_path = malloc(PATH_MAX * sizeof(char));
		snprintf(tmp_path, PATH_MAX, "%s/%s", path, oscap_file_entry_get_file(file_entry));

		if (stat(tmp_path, &sb) == 0) {
			resources[idx] = malloc(sizeof(struct oscap_content_resource));
			resources[idx]->href = strdup(oscap_file_entry_get_file(file_entry));
			resources[idx]->filename = tmp_path;
			idx++;
			resources = realloc(resources, (idx + 1) * sizeof(struct oscap_content_resource *));
			resources[idx] = NULL;
		}
		else {
			if (oscap_acquire_url_is_supported(oscap_file_entry_get_file(file_entry))) {
				// Strip out the 'path' for printing the url.
				printable_path = (char *) oscap_file_entry_get_file(file_entry);

				if (allow_remote_resources) {
					if (*temp_dir == NULL)
						*temp_dir = oscap_acquire_temp_dir();
					if (*temp_dir == NULL) {
						oscap_file_entry_iterator_free(files_it);
						oscap_file_entry_list_free(files);
						free(tmp_path);
						oscap_content_resources_free(resources);
						return NULL;
					}

					char *file = oscap_acquire_url_download(*temp_dir, printable_path);
					if (file != NULL) {
						resources[idx] = malloc(sizeof(struct oscap_content_resource));
						resources[idx]->href = strdup(printable_path);
						resources[idx]->filename = file;
						idx++;
						resources = realloc(resources, (idx + 1) * sizeof(struct oscap_content_resource *));
						resources[idx] = NULL;
						free(tmp_path);
						continue;
					}
				}
				else if (!fetch_option_suggested) {
					printf("This content points out to the remote resources. Use `--fetch-remote-resources' option to download them.\n");
					fetch_option_suggested = true;
				}
			}
			else
				printable_path = tmp_path;
			fprintf(stderr, "WARNING: Skipping %s file which is referenced from XCCDF content\n", printable_path);
			free(tmp_path);
		}
	}
	oscap_file_entry_iterator_free(files_it);
	oscap_file_entry_list_free(files);
	return resources;
}

static struct oscap_content_resource **
command_line_get_oval_resources(char **oval_filenames)
{
	struct oscap_content_resource **resources = malloc(sizeof(struct oscap_content_resource *));
	resources[0] = NULL;

	for (int i = 0; oval_filenames[i];) {
		resources[i] = malloc(sizeof(struct oscap_content_resource));
		resources[i]->href = strdup(basename(oval_filenames[i]));
		resources[i]->filename = strdup(oval_filenames[i]);
		i++;
		resources = realloc(resources, (i + 1) * sizeof(struct oscap_content_resource *));
		resources[i] = NULL;
	}
	return resources;
}

/**
 * XCCDF Processing fucntion
 * @param action OSCAP Action structure
 * @param sess OVAL Agent Session
 */
int app_evaluate_xccdf(const struct oscap_action *action)
{

	struct xccdf_policy_iterator *policy_it = NULL;
	struct xccdf_policy *policy = NULL;
	struct xccdf_benchmark *benchmark = NULL;
	struct xccdf_policy_model *policy_model = NULL;
	char * xccdf_pathcopy = NULL;
        void **def_models = NULL;
        void **sessions = NULL;
	struct oscap_content_resource **contents = NULL;
	int idx = 0;
	char* f_results = NULL;

	char* temp_dir = NULL;

	char* xccdf_file = NULL;
	struct ds_sds_index* sds_idx = NULL;
	char* xccdf_doc_version = NULL;
	char** oval_result_files = NULL;
	int result = OSCAP_ERROR;
	float base_score = 0;
#ifdef ENABLE_SCE
	struct sce_parameters* sce_parameters = 0;
#endif
	int priority = LOG_NOTICE;

	/* syslog message */
	syslog(priority, "Evaluation started. Content: %s, Profile: %s.", action->f_xccdf, action->profile);

	oscap_document_type_t doc_type = 0;

	const bool sds_likely = oscap_determine_document_type(action->f_xccdf, &doc_type) == 0 && doc_type == OSCAP_DOCUMENT_SDS;
	const char* f_datastream_id = NULL;
	const char* f_component_id = NULL;

	if (sds_likely)
	{
		if (action->validate)
		{
			int ret;
			if ((ret = oscap_validate_document(action->f_xccdf, OSCAP_DOCUMENT_SDS, "1.2", reporter, (void*)action) != 0))
			{
				if (ret==1)
					validation_failed(action->f_xccdf, OSCAP_DOCUMENT_SDS, "1.2");
				goto cleanup;
			}
		}

		temp_dir = oscap_acquire_temp_dir();
		if (temp_dir == NULL)
			goto cleanup;

		sds_idx = ds_sds_index_import(action->f_xccdf);

		// The following convoluted and complex code looks for a pair of
		// datastream and checklist inside it that satisfies conditions
		// (if any) of having specific IDs.
		//
		// It is possible to just pass xccdf-id in which case we have to
		// search through all the datastreams.
		//
		// It is also possible to just pass datastream-id in which case we
		// take the checklist in given datastream.
		//
		// In case datastream-id is not passed AND xccdf-id is also not passed,
		// we look for the first datastream (topdown in XML) that has any
		// checklist.

		f_datastream_id = action->f_datastream_id;
		f_component_id = action->f_xccdf_id;

		struct ds_stream_index_iterator* streams_it = ds_sds_index_get_streams(sds_idx);
		while (ds_stream_index_iterator_has_more(streams_it) && (!f_datastream_id || !f_component_id))
		{
			struct ds_stream_index* stream_idx = ds_stream_index_iterator_next(streams_it);
			const char* stream_id = ds_stream_index_get_id(stream_idx);

			if (action->f_datastream_id == NULL || strcmp(stream_id, action->f_datastream_id) == 0)
			{
				struct oscap_string_iterator* checklists_it = ds_stream_index_get_checklists(stream_idx);
				while (oscap_string_iterator_has_more(checklists_it))
				{
					const char* checklist_id = oscap_string_iterator_next(checklists_it);

					if (action->f_xccdf_id == NULL || strcmp(checklist_id, action->f_xccdf_id) == 0)
					{
						f_component_id = checklist_id;
						f_datastream_id = ds_stream_index_get_id(stream_idx);
						break;
					}
				}
				oscap_string_iterator_free(checklists_it);
			}
		}
		ds_stream_index_iterator_free(streams_it);

		if (f_datastream_id == NULL || f_component_id == NULL)
		{
			fprintf(stdout, "Failed to locate a datastream with ID matching '%s' ID"
			                "and checklist inside matching '%s' ID.\n",
			                action->f_datastream_id == NULL ? "<any>" : action->f_datastream_id,
			                action->f_xccdf_id == NULL ? "<any>" : action->f_xccdf_id);
			goto cleanup;
		}

		if (ds_sds_decompose(action->f_xccdf, f_datastream_id, f_component_id, temp_dir, "xccdf.xml") != 0)
		{
			fprintf(stdout, "Failed to decompose source datastream in '%s'\n", action->f_xccdf);
			goto cleanup;
		}

		xccdf_file = malloc(PATH_MAX * sizeof(char));
		snprintf(xccdf_file, PATH_MAX, "%s/%s", temp_dir, "xccdf.xml");
	}
	else
	{
		xccdf_file = strdup(action->f_xccdf);
	}

	int ret;

	const char* full_validation = getenv("OSCAP_FULL_VALIDATION");

	/* Validate documents */
	if (action->validate && (!sds_likely || full_validation)) {
		xccdf_doc_version = xccdf_detect_version(xccdf_file);
		if (!xccdf_doc_version)
			goto cleanup;

		if ((ret=oscap_validate_document(xccdf_file, OSCAP_DOCUMENT_XCCDF, xccdf_doc_version, reporter, (void*) action))) {
			if (ret==1)
				validation_failed(xccdf_file, OSCAP_DOCUMENT_XCCDF, xccdf_doc_version);
			goto cleanup;
		}
	}

	/* Load XCCDF model and XCCDF Policy model */
	benchmark = xccdf_benchmark_import(xccdf_file);
	if (benchmark == NULL) {
		fprintf(stderr, "Failed to import the XCCDF content from '%s'.\n", xccdf_file);
		goto cleanup;
	}

	/* Create policy model */
	policy_model = xccdf_policy_model_new(benchmark);

	/* Select profile */
	if (action->profile != NULL) {
		policy = xccdf_policy_model_get_policy_by_id(policy_model, action->profile);
		if(policy == NULL) {
                        fprintf(stderr, "Profile \"%s\" was not found.\n", action->profile);
                        goto cleanup;
                }
	} else { /* Take first policy */
		policy_it = xccdf_policy_model_get_policies(policy_model);
		if (xccdf_policy_iterator_has_more(policy_it)) {
			policy = xccdf_policy_iterator_next(policy_it);
		}
		xccdf_policy_iterator_free(policy_it);

		if (policy == NULL) {
			fprintf(stderr, "No Policy to evaluate. \n");
			goto cleanup;
		}
	}

	/* Use custom CPE dict if given */
	if (action->cpe != NULL) {
		oscap_document_type_t cpe_doc_type;
		char* cpe_doc_version = NULL;

		if (oscap_determine_document_type(action->cpe, &cpe_doc_type) != 0) {
			fprintf(stderr, "Can't determine document type of '%s'. This file was "
			                "passed as a CPE resource via --cpe.\n", action->cpe);
			goto cleanup;
		}

		if (cpe_doc_type == OSCAP_DOCUMENT_CPE_DICTIONARY) {
			cpe_doc_version = cpe_dict_detect_version(action->cpe);
		}
		else if (cpe_doc_type == OSCAP_DOCUMENT_CPE_LANGUAGE) {
			cpe_doc_version = cpe_lang_model_detect_version(action->cpe);
		}
		else {
			fprintf(stderr, "Document '%s' passed as a CPE resource was not detected "
			                "to be of type CPE dictionary or CPE language.\n", action->cpe);
			goto cleanup;
		}

		if ((ret=oscap_validate_document(action->cpe, cpe_doc_type, cpe_doc_version, reporter, (void*) action))) {
			if (ret==1)
				validation_failed(action->cpe, cpe_doc_type, cpe_doc_version);
			free(cpe_doc_version);
			goto cleanup;
		}
		free(cpe_doc_version);

		xccdf_policy_model_add_cpe_autodetect(policy_model, action->cpe);
	}

	if (sds_likely)
	{
		struct ds_stream_index* stream_idx = ds_sds_index_get_stream(sds_idx, f_datastream_id);
		struct oscap_string_iterator* cpe_it = ds_stream_index_get_dictionaries(stream_idx);

		// This potentially allows us to skip yet another decompose if we are sure
		// there are no CPE dictionaries or language models inside the datastream.
		if (oscap_string_iterator_has_more(cpe_it))
		{
			// FIXME: Decomposing means that the source datastream will be parsed
			//        into DOM even though it has already been parsed once when the
			//        XCCDF was split from it. We should optimize this out someday!
			if (ds_sds_decompose_custom(action->f_xccdf, f_datastream_id, temp_dir,
			                            "dictionaries", NULL, NULL) != 0)
			{
				fprintf(stderr, "Can't decompose CPE dictionaries from datastream '%s' from file '%s'!\n",
						f_datastream_id, action->f_xccdf);
				goto cleanup;
			}

			while (oscap_string_iterator_has_more(cpe_it))
			{
				const char* cpe_filename = oscap_string_iterator_next(cpe_it);

				char* full_cpe_filename = malloc(PATH_MAX * sizeof(char));
				snprintf(full_cpe_filename, PATH_MAX, "%s/%s", temp_dir, cpe_filename);

				if (full_validation) {
					oscap_document_type_t cpe_doc_type;
					char* cpe_doc_version = NULL;

					if (oscap_determine_document_type(full_cpe_filename, &cpe_doc_type) != 0) {
						fprintf(stderr, "Can't determine document type of '%s'. This file was "
								"embedded in SDS '%s' and was split into that file as a CPE resource.\n",
						        full_cpe_filename, action->f_xccdf);
						goto cleanup;
					}

					if (cpe_doc_type == OSCAP_DOCUMENT_CPE_DICTIONARY) {
						cpe_doc_version = cpe_dict_detect_version(full_cpe_filename);
					}
					else if (cpe_doc_type == OSCAP_DOCUMENT_CPE_LANGUAGE) {
						cpe_doc_version = cpe_lang_model_detect_version(full_cpe_filename);
					}
					else {
						fprintf(stderr, "Document '%s' that was split from SDS '%s' and passed as a CPE "
						        "resource was not detected to be of type CPE dictionary or CPE language.\n",
						        full_cpe_filename, action->f_xccdf);
						goto cleanup;
					}

					if ((ret=oscap_validate_document(full_cpe_filename, cpe_doc_type, cpe_doc_version, reporter, (void*) action))) {
						if (ret==1)
							validation_failed(full_cpe_filename, cpe_doc_type, cpe_doc_version);
						free(cpe_doc_version);
						goto cleanup;
					}
					free(cpe_doc_version);
				}

				xccdf_policy_model_add_cpe_autodetect(policy_model, full_cpe_filename);
				free(full_cpe_filename);
			}
		}

		oscap_string_iterator_free(cpe_it);
	}

	/* Register callbacks */
	xccdf_policy_model_register_start_callback(policy_model, callback_scr_rule, (void *) policy);
	xccdf_policy_model_register_output_callback(policy_model, callback_scr_result, NULL);

	/* xccdf_policy_model_register_output_callback(policy_model, callback_syslog_result, NULL); */

	/* Use OVAL files from policy model */
	if (action->f_ovals == NULL) {
		char * pathcopy =  strdup(xccdf_file);
		char * path = dirname(pathcopy);

		contents = xccdf_policy_get_oval_resources(policy_model, action->remote_resources, path, &temp_dir);
		free(pathcopy);

		if (contents == NULL)
			goto cleanup;
	} else
		contents = command_line_get_oval_resources(action->f_ovals);


	/* Validate OVAL files */
	// we will only validate if the file doesn't come from a datastream
	// or if full validation was explicitly requested
	if (action->validate && (!sds_likely || full_validation)) {
		for (idx=0; contents[idx]; idx++) {
			char *doc_version;

			doc_version = oval_determine_document_schema_version((const char *) contents[idx]->filename, OSCAP_DOCUMENT_OVAL_DEFINITIONS);
			if ((ret=oscap_validate_document(contents[idx]->filename, OSCAP_DOCUMENT_OVAL_DEFINITIONS,
				(const char *) doc_version, reporter, (void *) action))) {

				if (ret==1)
					validation_failed(contents[idx]->filename, OSCAP_DOCUMENT_OVAL_DEFINITIONS, doc_version);
				free(doc_version);
				goto cleanup;
			}
			free(doc_version);
		}
	}

	/* Register checking engines */
	for (idx=0; contents[idx]; idx++) {
		/* file -> def_model */
		struct oval_definition_model *tmp_def_model = oval_definition_model_import(contents[idx]->filename);
		if (tmp_def_model == NULL) {
			fprintf(stderr, "Failed to create OVAL definition model from: '%s'.\n", contents[idx]->filename);
			goto cleanup;
		}

		/* def_model -> session */
                struct oval_agent_session *tmp_sess = oval_agent_new_session(tmp_def_model, contents[idx]->href);
		if (tmp_sess == NULL) {
			fprintf(stderr, "Failed to create new OVAL agent session for: '%s'.\n", contents[idx]->href);
			goto cleanup;
		}

		/* store our name in the generated documents */
		oval_agent_set_product_name(tmp_sess, OSCAP_PRODUCTNAME);

		/* remember def_models */
		def_models = realloc(def_models, (idx + 2) * sizeof(struct oval_definition_model *));
		def_models[idx] = tmp_def_model;
		def_models[idx+1] = NULL;

		/* remember sessions */
		sessions = realloc(sessions, (idx + 2) * sizeof(struct oval_agent_session *));
		sessions[idx] = tmp_sess;
		sessions[idx+1] = NULL;

		/* register session */
	        xccdf_policy_model_register_engine_oval(policy_model, tmp_sess);
	}
	// -1 because we are counting the last NULL too, +1 because of conversion
	// from indices to array lengths
	unsigned int oval_session_count = (idx - 1) + 1;

	// register sce system
	xccdf_pathcopy =  strdup(xccdf_file);

#ifdef ENABLE_SCE
	sce_parameters = sce_parameters_new();
	sce_parameters_set_xccdf_directory(sce_parameters, dirname(xccdf_pathcopy));
	sce_parameters_allocate_session(sce_parameters);

	xccdf_policy_model_register_engine_sce(policy_model, sce_parameters);
#endif

	/* Perform evaluation */
	struct xccdf_result *ritem = xccdf_policy_evaluate(policy);
        if (ritem == NULL) {
		goto cleanup;
	}

	/* Write results into XCCDF Test Result model */
	xccdf_result_set_benchmark_uri(ritem, action->f_xccdf);
	struct oscap_text *title = oscap_text_new();
	oscap_text_set_text(title, "OSCAP Scan Result");
	xccdf_result_add_title(ritem, title);
	if (policy != NULL) {
		const char *id = xccdf_policy_get_id(policy);
		if (id != NULL)
			xccdf_result_set_profile(ritem, id);
	}

	xccdf_result_fill_sysinfo(ritem);

	struct xccdf_model_iterator *model_it = xccdf_benchmark_get_models(benchmark);
	while (xccdf_model_iterator_has_more(model_it)) {
		struct xccdf_model *model = xccdf_model_iterator_next(model_it);
		const char *score_model = xccdf_model_get_system(model);
		struct xccdf_score *score = xccdf_policy_get_score(policy, ritem, score_model);
		xccdf_result_add_score(ritem, score);

		/* record default base score for later use */
		if (!strcmp(score_model, "urn:xccdf:scoring:default"))
			base_score = xccdf_score_get_score(score);
	}
	xccdf_model_iterator_free(model_it);

	oval_result_files = malloc((oval_session_count + 1) * sizeof(char*));
	oval_result_files[0] = NULL;

	/* Export OVAL results */
	if ((action->oval_results == true || action->f_results_arf) && sessions) {
		int i;
		for (i=0; sessions[i]; i++) {
			/* get result model and session name*/
			struct oval_results_model *res_model = oval_agent_get_results_model(sessions[i]);
			const char* oval_results_directory = NULL;
			char *name = NULL;

			if (action->oval_results == true)
			{
				oval_results_directory = ".";
			}
			else
			{
				if (!temp_dir)
					temp_dir = oscap_acquire_temp_dir();
				if (temp_dir == NULL)
					goto cleanup;

				oval_results_directory = temp_dir;
			}

			char *escaped_url = NULL;
			const char *filename = oval_agent_get_filename(sessions[i]);
			if (oscap_acquire_url_is_supported(filename)) {
				escaped_url = oscap_acquire_url_to_filename(filename);
				if (escaped_url == NULL)
					goto cleanup;
			}

			name = malloc(PATH_MAX * sizeof(char));
			snprintf(name, PATH_MAX, "%s/%s.result.xml", oval_results_directory, escaped_url != NULL ? escaped_url : filename);
			if (escaped_url != NULL)
				free(escaped_url);

			/* export result model to XML */
			if (oval_results_model_export(res_model, NULL, name) == -1) {
				free(name);
				goto cleanup;
			}

			/* validate OVAL Results */
			if (action->validate && full_validation) {
				char *doc_version;

				doc_version = oval_determine_document_schema_version((const char *) name, OSCAP_DOCUMENT_OVAL_RESULTS);
				if (oscap_validate_document(name, OSCAP_DOCUMENT_OVAL_RESULTS, (const char *) doc_version,
							    reporter, (void*)action)) {
					validation_failed(name, OSCAP_DOCUMENT_OVAL_RESULTS, doc_version);
					free(name);
					free(doc_version);
					goto cleanup;
				}
				free(doc_version);
				fprintf(stdout, "OVAL Results are exported correctly.\n");
			}

			oval_result_files[i] = strdup(name);

			free(name);
		}

		oval_result_files[i] = NULL;
	}

#ifdef ENABLE_SCE
	/* Export SCE results */
	if (action->sce_results == true) {
		struct sce_check_result_iterator * it = sce_session_get_check_results(sce_parameters_get_session(sce_parameters));

		while(sce_check_result_iterator_has_more(it))
		{
			struct sce_check_result * check_result = sce_check_result_iterator_next(it);
			char target[2 + strlen(sce_check_result_get_basename(check_result)) + 11 + 1];
			snprintf(target, sizeof(target), "./%s.result.xml", sce_check_result_get_basename(check_result));
			sce_check_result_export(check_result, target);

			if (action->validate && full_validation) {
				if (oscap_validate_document(target, OSCAP_DOCUMENT_SCE_RESULT, "1.0", reporter, (void*)action)) {
					validation_failed(target, OSCAP_DOCUMENT_SCE_RESULT, "1.0");
					sce_check_result_iterator_free(it);
					goto cleanup;
				}
			}
		}

		sce_check_result_iterator_free(it);
	}
#endif

	f_results = action->f_results ? strdup(action->f_results) : NULL;
	if (!f_results && (action->f_report != NULL || action->f_results_arf != NULL))
	{
		if (!temp_dir)
			temp_dir = oscap_acquire_temp_dir();
		if (temp_dir == NULL)
			goto cleanup;

		f_results = malloc(PATH_MAX * sizeof(char));
		snprintf(f_results, PATH_MAX, "%s/xccdf-result.xml", temp_dir);
	}

	/* Export results */
	if (f_results != NULL) {
		xccdf_benchmark_add_result(benchmark, xccdf_result_clone(ritem));
		xccdf_benchmark_export(benchmark, f_results);

		/* validate XCCDF Results */
		if (action->validate && full_validation) {
			/* we assume there is a same xccdf doc_version on input and output */
			if (oscap_validate_document(f_results, OSCAP_DOCUMENT_XCCDF, xccdf_doc_version, reporter, (void*) action)) {
				validation_failed(f_results, OSCAP_DOCUMENT_XCCDF, xccdf_doc_version);
				goto cleanup;
			}
			fprintf(stdout, "XCCDF Results are exported correctly.\n");
		}

		/* generate report */
		if (action->f_report != NULL)
			xccdf_gen_report(f_results,
			                 xccdf_result_get_id(ritem),
			                 action->f_report,
			                 "",
			                 (action->oval_results ? "%.result.xml" : ""),
#ifdef ENABLE_SCE
			                 (action->sce_results  ? "%.result.xml" : ""),
#else
			                 "",
#endif
			                 action->profile == NULL ? "" : action->profile
			);
	}

	/* Export variables */
	if (action->export_variables && sessions) {
		int i;

		for (i = 0; sessions[i]; ++i) {
			int j;
			char *sname;
			struct oval_results_model *resmod;
			struct oval_definition_model *defmod;
			struct oval_variable_model_iterator *varmod_itr;
			char *escaped_url = NULL;

			sname = (char *) oval_agent_get_filename(sessions[i]);
			if (oscap_acquire_url_is_supported(sname)) {
				escaped_url = oscap_acquire_url_to_filename(sname);
				if (escaped_url == NULL)
					goto cleanup;
				sname = escaped_url;
			}
			resmod = oval_agent_get_results_model(sessions[i]);
			defmod = oval_results_model_get_definition_model(resmod);

			j = 0;
			varmod_itr = oval_definition_model_get_variable_models(defmod);
			while (oval_variable_model_iterator_has_more(varmod_itr)) {
				char fname[strlen(sname) + 32];
				struct oval_variable_model *varmod;

				varmod = oval_variable_model_iterator_next(varmod_itr);
				snprintf(fname, sizeof(fname), "%s-%d.variables-%d.xml", sname, i, j++);
				oval_variable_model_export(varmod, fname);
			}
			if (escaped_url != NULL)
				free(escaped_url);
			oval_variable_model_iterator_free(varmod_itr);
		}
	}

	if (action->f_results_arf != NULL)
	{
		char* sds_path = 0;

		if (sds_likely)
		{
			sds_path = strdup(action->f_xccdf);
		}
		else
		{
			if (!temp_dir)
				temp_dir = oscap_acquire_temp_dir();
			if (temp_dir == NULL)
				goto cleanup;

			sds_path =  malloc(PATH_MAX * sizeof(char));
			snprintf(sds_path, PATH_MAX, "%s/sds.xml", temp_dir);
			ds_sds_compose_from_xccdf(action->f_xccdf, sds_path);
		}

		ds_rds_create(sds_path, f_results, (const char**)oval_result_files, action->f_results_arf);
		free(sds_path);

		if (full_validation)
		{
			if (oscap_validate_document(action->f_results_arf, OSCAP_DOCUMENT_ARF, "1.1", reporter, (void*)action))
			{
				validation_failed(action->f_results_arf, OSCAP_DOCUMENT_ARF, "1.1");
				ret = OSCAP_ERROR;
				goto cleanup;
			}
			fprintf(stdout, "Result DataStream exported correctly.\n");
		}
	}

	/* Get the result from TestResult model and decide if end with error or with correct return code */
	result = OSCAP_OK;
	struct xccdf_rule_result_iterator *res_it = xccdf_result_get_rule_results(ritem);
	while (xccdf_rule_result_iterator_has_more(res_it)) {
		struct xccdf_rule_result *res = xccdf_rule_result_iterator_next(res_it);
		xccdf_test_result_type_t rule_result = xccdf_rule_result_get_result(res);
		if ((rule_result == XCCDF_RESULT_FAIL) || (rule_result == XCCDF_RESULT_UNKNOWN))
			result = OSCAP_FAIL;
	}
	xccdf_rule_result_iterator_free(res_it);


cleanup:
	if (oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	/* syslog message */
	syslog(priority, "Evaluation finnished. Return code: %d, Base score %f.", result, base_score);

	if (xccdf_doc_version)
		free(xccdf_doc_version);

#ifdef ENABLE_SCE
	sce_parameters_free(sce_parameters);
#endif
	free(xccdf_pathcopy);

	/* Definition Models */
	if (def_models) {
		for (int i=0; def_models[i]; i++)
			oval_definition_model_free(def_models[i]);
		free(def_models);
	}

	/* Sessions */
	if (sessions) {
		for (int i=0; sessions[i]; i++)
			oval_agent_destroy_session(sessions[i]);
		free(sessions);
	}

	/* OVAL and SCE files */
	oscap_content_resources_free(contents);

	if (policy_model)
		xccdf_policy_model_free(policy_model);

	if (sds_idx != NULL)
		ds_sds_index_free(sds_idx);

	oscap_acquire_cleanup_dir(&temp_dir);

	if (oval_result_files)
	{
		for(idx = 0; oval_result_files[idx] != NULL; idx++)
		{
			free(oval_result_files[idx]);
		}

		free(oval_result_files);
	}

	free(f_results);
	free(xccdf_file);

	return result;
}

static xccdf_test_result_type_t resolve_variables_wrapper(struct xccdf_policy *policy, const char *rule_id,
	const char *id, const char *href, struct xccdf_value_binding_iterator *bnd_itr,
	struct xccdf_check_import_iterator *check_import_it, void *usr)
{
	if (0 != oval_agent_resolve_variables((struct oval_agent_session *) usr, bnd_itr))
		return XCCDF_RESULT_UNKNOWN;

	return XCCDF_RESULT_PASS;
}

// todo: consolidate with app_evaluate_xccdf()
static int app_xccdf_export_oval_variables(const struct oscap_action *action)
{
	struct xccdf_benchmark *benchmark;
	struct xccdf_policy_model *policy_model = NULL;
	struct xccdf_policy *policy = NULL;
	struct xccdf_policy_iterator *policy_itr;
	struct oval_definition_model **def_mod_lst = NULL;
	struct oval_agent_session **ag_ses_lst = NULL;
	struct xccdf_result *xres;
	struct oscap_content_resource **oval_resources = NULL;
	int of_cnt = 0, i, ret;
	int result = OSCAP_ERROR;
	char *doc_version = NULL;
	char *temp_dir = NULL;

	/* validate the XCCDF document */
	if (action->validate) {
		doc_version = xccdf_detect_version(action->f_xccdf);
	        if (!doc_version)
        	        goto cleanup;

		if ((ret=oscap_validate_document(action->f_xccdf, OSCAP_DOCUMENT_XCCDF, doc_version, reporter, (void*)action))) {
			if (ret==1)
				validation_failed(action->f_xccdf, OSCAP_DOCUMENT_XCCDF, doc_version);
			goto cleanup;
		}
	}

	/* import the XCCDF document */
	benchmark = xccdf_benchmark_import(action->f_xccdf);
	if (benchmark == NULL) {
		fprintf(stderr, "Failed to import the XCCDF document from '%s'.\n", action->f_xccdf);
		goto cleanup;
	}

	/* create the policy model */
	policy_model = xccdf_policy_model_new(benchmark);

	/* select a profile */
	if (action->profile != NULL) {
		policy = xccdf_policy_model_get_policy_by_id(policy_model, action->profile);
		if (policy == NULL) {
			fprintf(stderr, "Unable to find profile '%s'.\n", action->profile);
			goto cleanup;
		}
	} else {
		/* use the first one if none specified */
		policy_itr = xccdf_policy_model_get_policies(policy_model);
		if (xccdf_policy_iterator_has_more(policy_itr))
			policy = xccdf_policy_iterator_next(policy_itr);
		xccdf_policy_iterator_free(policy_itr);
		if (policy == NULL) {
			fprintf(stderr, "No profile to evaluate.\n");
			goto cleanup;
		}
	}

	if (action->f_ovals != NULL) {
		oval_resources = command_line_get_oval_resources(action->f_ovals);
	} else {
		char *xccdf_path_cpy, *dir_path;

		xccdf_path_cpy = strdup(action->f_xccdf);
		dir_path = dirname(xccdf_path_cpy);

		oval_resources = xccdf_policy_get_oval_resources(policy_model, action->remote_resources, dir_path, &temp_dir);
		free(xccdf_path_cpy);

		if (oval_resources == NULL)
			goto cleanup;
	}
	for (of_cnt = 0; oval_resources[of_cnt]; of_cnt++);

	if (oval_resources[0] == NULL) {
		fprintf(stderr, "No OVAL definition files present, aborting.\n");
		goto cleanup;
	}

	def_mod_lst = calloc(of_cnt, sizeof(struct oval_definition_model *));
	ag_ses_lst = calloc(of_cnt, sizeof(struct oval_agent_session *));

	for (i = 0; i < of_cnt; i++) {
		def_mod_lst[i] = oval_definition_model_import(oval_resources[i]->filename);
		if (def_mod_lst[i] == NULL) {
			fprintf(stderr, "Failed to import definitions model from '%s'.\n", oval_resources[i]->filename);
			goto cleanup;
		}

		ag_ses_lst[i] = oval_agent_new_session(def_mod_lst[i], oval_resources[i]->href);
		if (ag_ses_lst[i] == NULL) {
			fprintf(stderr, "Failed to create new agent session for '%s'.\n", oval_resources[i]->href);
			goto cleanup;
		}

		xccdf_policy_model_register_engine_and_query_callback(policy_model,
			"http://oval.mitre.org/XMLSchema/oval-definitions-5",
			resolve_variables_wrapper, ag_ses_lst[i], NULL);
	}

	/* perform evaluation */
	xres = xccdf_policy_evaluate(policy);
	if (xres == NULL)
		goto cleanup;

	for (i = 0; i < of_cnt; i++) {
		int j;
		char *ses_name;
		struct oval_variable_model_iterator *var_mod_itr;

		j = 0;
		char *escaped_url = NULL;
		ses_name = (char *) oval_agent_get_filename(ag_ses_lst[i]);
		if (oscap_acquire_url_is_supported(ses_name)) {
			escaped_url = oscap_acquire_url_to_filename(ses_name);
			if (escaped_url == NULL)
				goto cleanup;
			ses_name = escaped_url;
		}

		var_mod_itr = oval_definition_model_get_variable_models(def_mod_lst[i]);
		while (oval_variable_model_iterator_has_more(var_mod_itr)) {
			struct oval_variable_model *var_mod;
			char fname[strlen(ses_name) + 32];

			var_mod = oval_variable_model_iterator_next(var_mod_itr);
			snprintf(fname, sizeof(fname), "%s-%d.variables-%d.xml", ses_name, i, j++);
			oval_variable_model_export(var_mod, fname);
		}
		oval_variable_model_iterator_free(var_mod_itr);
		if (escaped_url != NULL)
			free(escaped_url);
	}

	result = OSCAP_OK;

 cleanup:
	if (oscap_err())
		fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

	if (doc_version)
		free(doc_version);

	if (def_mod_lst != NULL) {
		for (i = 0; i < of_cnt; i++) {
			oval_agent_destroy_session(ag_ses_lst[i]);
			oval_definition_model_free(def_mod_lst[i]);
		}
		free(ag_ses_lst);
		free(def_mod_lst);
	}

	oscap_content_resources_free(oval_resources);

	if (policy_model)
		xccdf_policy_model_free(policy_model);

	oscap_acquire_cleanup_dir(&temp_dir);

	return result;
}

int app_xccdf_resolve(const struct oscap_action *action)
{
	char *doc_version = NULL;
	int ret = OSCAP_ERROR;
	struct xccdf_benchmark *bench = NULL;

	if (!action->f_xccdf) {
		fprintf(stderr, "No input document specified!\n");
		return OSCAP_ERROR;
	}
	if (!action->f_results) {
		fprintf(stderr, "No output document filename specified!\n");
		return OSCAP_ERROR;
	}

	/* validate input */
	if (action->validate) {
		doc_version = xccdf_detect_version(action->f_xccdf);
		if (!doc_version) {
			return OSCAP_ERROR;
		}

		if (oscap_validate_document(action->f_xccdf, OSCAP_DOCUMENT_XCCDF, doc_version, reporter, (void*) action) != 0) {
			validation_failed(action->f_xccdf, OSCAP_DOCUMENT_XCCDF, doc_version);
			goto cleanup;
		}
	}

	bench = xccdf_benchmark_import(action->f_xccdf);
	if (!bench)
		goto cleanup;

	if (action->force)
		xccdf_benchmark_set_resolved(bench, false);

	if (xccdf_benchmark_get_resolved(bench))
		fprintf(stderr, "Benchmark is already resolved!\n");
	else {
		if (!xccdf_benchmark_resolve(bench))
			fprintf(stderr, "Benchmark resolving failure (probably a dependency loop)!\n");

		{
			if (xccdf_benchmark_export(bench, action->f_results)) {
				ret = OSCAP_OK;

				/* validate exported results */
				const char* full_validation = getenv("OSCAP_FULL_VALIDATION");
				if (action->validate && full_validation) {
					/* reuse doc_version from unresolved document
					   it should be same in resolved one */
					if (oscap_validate_document(action->f_results, OSCAP_DOCUMENT_XCCDF, doc_version, reporter, (void*)action)) {
						validation_failed(action->f_results, OSCAP_DOCUMENT_XCCDF, doc_version);
						ret = OSCAP_ERROR;
					}
					else
						fprintf(stdout, "Resolved XCCDF has been exported correctly.\n");
				}
			}
		}
	}

cleanup:
	if (oscap_err())
		fprintf(stderr, "Error: %s\n", oscap_err_desc());
	if (bench)
		xccdf_benchmark_free(bench);
	if (doc_version)
		free(doc_version);

	return ret;
}

static int xccdf_gen_report(const char *infile, const char *id, const char *outfile, const char *show, const char *oval_template, const char *sce_template, const char* profile)
{
	const char *params[] = {
		"result-id",         id,
		"show",              show,
		"profile",           profile,
		"oval-template",     oval_template,
		"sce-template",      sce_template,
		"verbosity",         "",
		"hide-profile-info", NULL,
		NULL };

	return app_xslt(infile, "xccdf-report.xsl", outfile, params);
}

int app_xccdf_xslt(const struct oscap_action *action)
{
	const char *oval_template = action->oval_template;

	/* If generating the report and the option is missing -> use defaults */
	if (action->module == &XCCDF_GEN_REPORT && oval_template == NULL) {
		oval_template = "%.result.xml";
	}

	if (action->module == &XCCDF_GEN_CUSTOM) {
	        action->module->user = (void*)action->stylesheet;
	}

	const char *params[] = {
		"result-id",         action->id,
		"show",              action->show,
		"profile",           action->profile,
		"template",          action->tmpl,
		"format",            action->format,
		"oval-template",     oval_template,
#ifdef ENABLE_SCE
		"sce-template",      action->sce_template,
#endif
		"verbosity",         "",
		"hide-profile-info", action->hide_profile_info ? "yes" : NULL,
		NULL
	};

	int ret = app_xslt(action->f_xccdf, action->module->user, action->f_results, params);
	return ret;
}

bool getopt_generate(int argc, char **argv, struct oscap_action *action)
{
	static const struct option long_options[] = {
		{"profile", 1, 0, 3},
		{"format", 1, 0, 4},
		{0, 0, 0, 0}
	};

	int c;
	while ((c = getopt_long(argc, argv, "+", long_options, NULL)) != -1) {
		switch (c) {
		case 3: action->profile = optarg; break;
		case 4: action->format = optarg; break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}
    return true;
}

enum oval_opt {
    XCCDF_OPT_RESULT_FILE = 1,
    XCCDF_OPT_RESULT_FILE_ARF,
    XCCDF_OPT_DATASTREAM_ID,
    XCCDF_OPT_XCCDF_ID,
    XCCDF_OPT_PROFILE,
    XCCDF_OPT_REPORT_FILE,
    XCCDF_OPT_SHOW,
    XCCDF_OPT_TEMPLATE,
    XCCDF_OPT_FORMAT,
    XCCDF_OPT_OVAL_TEMPLATE,
    XCCDF_OPT_STYLESHEET_FILE,
#ifdef ENABLE_SCE
    XCCDF_OPT_SCE_TEMPLATE,
#endif
    XCCDF_OPT_FILE_VERSION,
    XCCDF_OPT_CPE,
    XCCDF_OPT_CPE_DICT,
    XCCDF_OPT_OUTPUT = 'o',
    XCCDF_OPT_RESULT_ID = 'i'
};

bool getopt_xccdf(int argc, char **argv, struct oscap_action *action)
{
	assert(action != NULL);

	action->doctype = OSCAP_DOCUMENT_XCCDF;

	/* Command-options */
	const struct option long_options[] = {
	// options
		{"output",		required_argument, NULL, XCCDF_OPT_OUTPUT},
		{"results", 		required_argument, NULL, XCCDF_OPT_RESULT_FILE},
		{"results-arf",		required_argument, NULL, XCCDF_OPT_RESULT_FILE_ARF},
		{"datastream-id",		required_argument, NULL, XCCDF_OPT_DATASTREAM_ID},
		{"xccdf-id",		required_argument, NULL, XCCDF_OPT_XCCDF_ID},
		{"profile", 		required_argument, NULL, XCCDF_OPT_PROFILE},
		{"result-id",		required_argument, NULL, XCCDF_OPT_RESULT_ID},
		{"report", 		required_argument, NULL, XCCDF_OPT_REPORT_FILE},
		{"show", 		required_argument, NULL, XCCDF_OPT_SHOW},
		{"template", 		required_argument, NULL, XCCDF_OPT_TEMPLATE},
		{"format", 		required_argument, NULL, XCCDF_OPT_FORMAT},
		{"oval-template", 	required_argument, NULL, XCCDF_OPT_OVAL_TEMPLATE},
		{"stylesheet",	required_argument, NULL, XCCDF_OPT_STYLESHEET_FILE},
		{"cpe",	required_argument, NULL, XCCDF_OPT_CPE},
		{"cpe-dict",	required_argument, NULL, XCCDF_OPT_CPE_DICT}, // DEPRECATED!
#ifdef ENABLE_SCE
		{"sce-template", 	required_argument, NULL, XCCDF_OPT_SCE_TEMPLATE},
#endif
	// flags
		{"force",		no_argument, &action->force, 1},
		{"oval-results",	no_argument, &action->oval_results, 1},
#ifdef ENABLE_SCE
		{"sce-results",	no_argument, &action->sce_results, 1},
#endif
		{"skip-valid",		no_argument, &action->validate, 0},
		{"fetch-remote-resources", no_argument, &action->remote_resources, 1},
		{"hide-profile-info",	no_argument, &action->hide_profile_info, 1},
		{"export-variables",	no_argument, &action->export_variables, 1},
	// end
		{0, 0, 0, 0}
	};

	int c;
	while ((c = getopt_long(argc, argv, "o:i:", long_options, NULL)) != -1) {

		switch (c) {
		case XCCDF_OPT_OUTPUT:
		case XCCDF_OPT_RESULT_FILE:	action->f_results = optarg;	break;
		case XCCDF_OPT_RESULT_FILE_ARF:	action->f_results_arf = optarg;	break;
		case XCCDF_OPT_DATASTREAM_ID:	action->f_datastream_id = optarg;	break;
		case XCCDF_OPT_XCCDF_ID:	action->f_xccdf_id = optarg; break;
		case XCCDF_OPT_PROFILE:		action->profile = optarg;	break;
		case XCCDF_OPT_RESULT_ID:	action->id = optarg;		break;
		case XCCDF_OPT_REPORT_FILE:	action->f_report = optarg; 	break;
		case XCCDF_OPT_SHOW:		action->show = optarg;		break;
		case XCCDF_OPT_TEMPLATE:	action->tmpl = optarg;		break;
		case XCCDF_OPT_FORMAT:		action->format = optarg;	break;
		case XCCDF_OPT_OVAL_TEMPLATE:	action->oval_template = optarg; break;
		/* we use realpath to get an absolute path to given XSLT to prevent openscap from looking
		   into /usr/share/openscap/xsl instead of CWD */
		case XCCDF_OPT_STYLESHEET_FILE: realpath(optarg, custom_stylesheet_path); action->stylesheet = custom_stylesheet_path; break;
		case XCCDF_OPT_CPE:			action->cpe = optarg; break;
		case XCCDF_OPT_CPE_DICT:
			{
				fprintf(stdout, "Warning: --cpe-dict is a deprecated option. Please use --cpe instead!\n\n");
				action->cpe = optarg; break;
			}
#ifdef ENABLE_SCE
		case XCCDF_OPT_SCE_TEMPLATE:	action->sce_template = optarg; break;
#endif
		case 0: break;
		default: return oscap_module_usage(action->module, stderr, NULL);
		}
	}

	if (action->module == &XCCDF_EVAL) {
		/* We should have XCCDF file here */
		if (optind >= argc) {
			/* TODO */
			return oscap_module_usage(action->module, stderr, "XCCDF file need to be specified!");
		}

                action->f_xccdf = argv[optind];
                if (argc > (optind+1)) {
                    action->f_ovals = malloc((argc-(optind+1)+1) * sizeof(char *));
                    int i = 1;
                    while (argc > (optind+i)) {
                        action->f_ovals[i-1] = argv[optind + i];
                        i++;
                    }
                    action->f_ovals[i-1] = NULL;
                } else {
                    action->f_ovals = NULL;
                }
	} else if (action->module == &XCCDF_GEN_CUSTOM) {
		if (!action->stylesheet) {
			return oscap_module_usage(action->module, stderr, "XSLT Stylesheet needs to be specified!");
		}

		if (optind >= argc)
			return oscap_module_usage(action->module, stderr, "XCCDF file needs to be specified!");
		action->f_xccdf = argv[optind];
	} else {
		if (optind >= argc)
			return oscap_module_usage(action->module, stderr, "XCCDF file needs to be specified!");
		action->f_xccdf = argv[optind];
	}

	return true;
}

int app_xccdf_validate(const struct oscap_action *action) {
	int ret;
	char *doc_version;
	int result;


	doc_version = xccdf_detect_version(action->f_xccdf);
        if (!doc_version) {
                result = OSCAP_ERROR;
                goto cleanup;
        }

        ret=oscap_validate_document(action->f_xccdf, action->doctype, doc_version, reporter, (void*)action);
        if (ret==-1) {
                result=OSCAP_ERROR;
                goto cleanup;
        }
        else if (ret==1) {
                result=OSCAP_FAIL;
        }
        else
                result=OSCAP_OK;

        if (result==OSCAP_FAIL)
		validation_failed(action->f_xccdf, OSCAP_DOCUMENT_XCCDF, doc_version);

cleanup:
        if (oscap_err())
                fprintf(stderr, "%s %s\n", OSCAP_ERR_MSG, oscap_err_desc());

        if (doc_version)
		free(doc_version);

        return result;

}
