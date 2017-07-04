/*
 * Copyright 2013--2014 Red Hat Inc., Durham, North Carolina.
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
 */
#include <string.h>
#include <limits.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>

#include <oscap.h>
#include "oscap_source.h"
#include <cpe_lang.h>
#include <OVAL/public/oval_agent_api.h>
#include <OVAL/public/oval_agent_xccdf_api.h>
#include "common/oscap_acquire.h"
#include <common/alloc.h>
#include "common/util.h"
#include "common/list.h"
#include "common/oscapxml.h"
#include "common/_error.h"
#include "common/debug_priv.h"
#include "CPE/cpe_session_priv.h"
#include "DS/public/scap_ds.h"
#include "DS/public/ds_sds_session.h"
#include "DS/ds_sds_session_priv.h"
#include "DS/rds_priv.h"
#include "OVAL/results/oval_results_impl.h"
#include "source/xslt_priv.h"
#include "XCCDF/xccdf_impl.h"
#include "XCCDF_POLICY/public/xccdf_policy.h"
#include "XCCDF_POLICY/xccdf_policy_priv.h"
#include "XCCDF_POLICY/xccdf_policy_model_priv.h"
#include "item.h"
#include "public/xccdf_session.h"
#include "XCCDF_POLICY/public/check_engine_plugin.h"

struct oval_content_resource {
	char *href;					///< Coresponds with xccdf:check-content-ref/\@href.
	struct oscap_source *source;                    ///< The oscap_source representing the href resource
	bool source_owned;                              ///< Indicates whether we need to dispose source property
};

struct xccdf_session {
	const char *filename;				///< File name of SCAP (SDS or XCCDF) file for this session.
	const char *rule;				///< Single-rule feature: if not NULL, the session will work only with this one rule.
	struct oscap_source *source;                    ///< Main source assigned with the main file (SDS or XCCDF)
	char *temp_dir;					///< Temp directory used for decomposed component files.
	struct {
		struct oscap_source *source;            ///< oscap_source representing the XCCDF file
		struct xccdf_policy_model *policy_model;///< Active policy model.
		char *profile_id;			///< Last selected profile.
		struct xccdf_result *result;		///< XCCDF Result model.
		float base_score;			///< Basec score of the latest evaluation.
		struct oscap_source *result_source;     ///< oscap_source for the exported XCCDF result
	} xccdf;
	struct {
		struct ds_sds_session *session;         ///< SDS Registry abstract structure
		char *user_datastream_id;		///< Datastream id requested by user (only applicable for sds).
		char *user_component_id;		///< Component id requested by user (only applicable for sds).
		char *user_benchmark_id;		///< Benchmark id requested by user (only applicable for sds).
	} ds;
	struct {
		bool fetch_remote_resources;		///< Allows download of remote resources (not applicable when user sets custom oval files)
		download_progress_calllback_t progress;	///< Callback to report progress of download.
		struct oval_content_resource **custom_resources;///< OVAL files required by user
		struct oval_content_resource **resources;///< OVAL files referenced from XCCDF
		struct oval_agent_session **agents;	///< OVAL Agent Session
		xccdf_policy_engine_eval_fn user_eval_fn;///< Custom OVAL engine callback
		char *product_cpe;			///< CPE of scanner product.
		struct oscap_source* arf_report;	///< ARF report
		struct oscap_htable *result_sources;    ///< mapping 'filepath' to oscap_source for OVAL results
		struct oscap_htable *results_mapping;    ///< mapping OVAL filename to filepath for OVAL results
		struct oscap_htable *arf_report_mapping;    ///< mapping OVAL filename to ARF report ID for OVAL results
	} oval;
	struct {
		char *arf_file;				///< Path to ARF file to export
		char *xccdf_file;			///< Path to XCCDF file to export
		char *report_file;			///< Path to HTML file to eport
		bool oval_results;			///< Shall be the OVAL results files exported?
		bool oval_variables;			///< Shall be the OVAL variable files exported?
		bool check_engine_plugins_results;	///< Shall the check engine plugins results be exported?
		bool without_sys_chars;			///< Shall system characteristics be exported?
		bool thin_results;			///< Shall OVAL/ARF results be exported as THIN? Default is FULL
	} export;					///< Settings of Session export
	char *user_cpe;					///< Path to CPE dictionary required by user
	struct {
		struct oscap_source *user_file; ///< Tailoring file requested by the user
		char *user_component_id;    ///< Component ID of the Tailoring requested by the user
	} tailoring;
	bool validate;					///< False value indicates to skip any XSD validation.
	bool full_validation;				///< True value indicates that every possible step will be validated by XSD.

	struct oscap_list *check_engine_plugins; ///< Extra non-OVAL check engines that may or may not have been loaded
};

static int _xccdf_session_autonegotiate_tailoring_file(struct xccdf_session *session, const char *original_path);
static void _oval_content_resources_free(struct oval_content_resource **resources);
static void _xccdf_session_free_oval_agents(struct xccdf_session *session);
static void _xccdf_session_free_oval_result_sources(struct xccdf_session *session);

static const char *oscap_productname = "cpe:/a:open-scap:oscap";
static const char *oval_sysname = "http://oval.mitre.org/XMLSchema/oval-definitions-5";

struct xccdf_session *xccdf_session_new_from_source(struct oscap_source *source)
{
	if (source == NULL) {
		return NULL;
	}
	const char *filename = oscap_source_get_filepath(source);
	struct xccdf_session *session = (struct xccdf_session *) oscap_calloc(1, sizeof(struct xccdf_session));
	session->source = source;
	oscap_document_type_t document_type = oscap_source_get_scap_type(session->source);
	if (document_type == OSCAP_DOCUMENT_UNKNOWN) {
		xccdf_session_free(session);
		return NULL;
	}
	if (document_type != OSCAP_DOCUMENT_XCCDF
			&& document_type != OSCAP_DOCUMENT_SDS
			&& document_type != OSCAP_DOCUMENT_XCCDF_TAILORING) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP,
			"Session input file was determined but it isn't an XCCDF file, "
			"a source datastream or an XCCDF tailoring file.");
		xccdf_session_free(session);
		return NULL;
	}
	session->validate = true;
	session->xccdf.base_score = 0;
	session->oval.progress = download_progress_empty_calllback;
	session->check_engine_plugins = oscap_list_new();

	// We now have to switch up the oscap_sources in case we were given XCCDF tailoring

	if (document_type == OSCAP_DOCUMENT_XCCDF_TAILORING) {
		if (_xccdf_session_autonegotiate_tailoring_file(session, filename) != 0) {
			xccdf_session_free(session);
			return NULL;
		}
	}

	dI("Created a new XCCDF session from a %s '%s'.",
		oscap_document_type_to_string(document_type), filename);
	return session;
}

struct xccdf_session *xccdf_session_new(const char *filename)
{
	struct oscap_source *source = oscap_source_new_from_file(filename);
	struct xccdf_session *session = xccdf_session_new_from_source(source);
	return session;
}

static int _xccdf_session_autonegotiate_tailoring_file(struct xccdf_session *session, const char *original_path)
{
	struct xccdf_tailoring* tailoring = xccdf_tailoring_import_source(session->source, NULL);

	if (tailoring == NULL) {
		return -1;
	}

	char *source_path = oscap_strdup(xccdf_tailoring_get_benchmark_ref(tailoring));
	xccdf_tailoring_free(tailoring);

	if (source_path == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP,
			"Session input file was determined to be XCCDF tailoring file, "
			"but it contained no benchmark reference!");
		return -1;
	}

	char *original_path_cpy = oscap_strdup(original_path);
	char *base_dir = dirname(original_path_cpy);

	char *real_source_path = source_path[0] == '/' ?
		oscap_strdup(source_path) : oscap_sprintf("%s/%s", base_dir, source_path);

	oscap_free(original_path_cpy);
	oscap_free(source_path);

	struct oscap_source *real_source = oscap_source_new_from_file(real_source_path);
	oscap_free(real_source_path);

	if (real_source == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP,
			"Session input file was determined to be XCCDF tailoring file, "
			"but the real source file could not be loaded.");
		return -1;
	}

	session->tailoring.user_file = session->source;
	session->source = real_source;

	return 0;
}

static void xccdf_session_unload_check_engine_plugins(struct xccdf_session *session);

static struct oscap_source* xccdf_session_create_arf_source(struct xccdf_session *session)
{
	if (session->oval.arf_report != NULL) {
		return session->oval.arf_report;
	}

	struct oscap_source *sds_source = NULL;

	if (xccdf_session_is_sds(session)) {
		sds_source = session->source;
	}
	else {
		if (!session->temp_dir)
			session->temp_dir = oscap_acquire_temp_dir();
		if (session->temp_dir == NULL)
			return NULL;

		char *sds_path = malloc(PATH_MAX * sizeof(char));
		snprintf(sds_path, PATH_MAX, "%s/sds.xml", session->temp_dir);
		ds_sds_compose_from_xccdf(oscap_source_readable_origin(session->source), sds_path);
		sds_source = oscap_source_new_from_file(sds_path);
		free(sds_path);
	}

	session->oval.arf_report = ds_rds_create_source(sds_source, session->xccdf.result_source, session->oval.result_sources, session->oval.results_mapping, session->oval.arf_report_mapping, session->export.arf_file);
	if (!xccdf_session_is_sds(session)) {
		oscap_source_free(sds_source);
	}
	return session->oval.arf_report;
}

void xccdf_session_free(struct xccdf_session *session)
{
	if (session == NULL)
		return;
	oscap_free(session->xccdf.profile_id);
	oscap_free(session->export.xccdf_file);
	oscap_free(session->export.report_file);
	oscap_free(session->export.arf_file);
	_xccdf_session_free_oval_result_sources(session);
	xccdf_session_unload_check_engine_plugins(session);
	oscap_list_free0(session->check_engine_plugins);
	oscap_free(session->user_cpe);
	oscap_free(session->oval.product_cpe);
	_xccdf_session_free_oval_agents(session);
	_oval_content_resources_free(session->oval.custom_resources);
	_oval_content_resources_free(session->oval.resources);
	oscap_source_free(session->oval.arf_report);
	oscap_source_free(session->xccdf.result_source);
	if (session->xccdf.policy_model != NULL)
		xccdf_policy_model_free(session->xccdf.policy_model);
	oscap_free(session->ds.user_datastream_id);
	oscap_free(session->ds.user_component_id);
	oscap_free(session->ds.user_benchmark_id);
	ds_sds_session_free(session->ds.session);
	if (session->temp_dir != NULL)
		oscap_acquire_cleanup_dir((char **) &(session->temp_dir));
	oscap_source_free(session->source);
	oscap_source_free(session->tailoring.user_file);
	oscap_free(session->tailoring.user_component_id);
	oscap_htable_free(session->oval.results_mapping, (oscap_destruct_func) oscap_free);
	oscap_htable_free(session->oval.arf_report_mapping, (oscap_destruct_func) oscap_free);
	oscap_free(session);
}

const char *xccdf_session_get_filename(const struct xccdf_session *session)
{
	return oscap_source_readable_origin(session->source);
}

bool xccdf_session_is_sds(const struct xccdf_session *session)
{
	return oscap_source_get_scap_type(session->source) == OSCAP_DOCUMENT_SDS;
}

void xccdf_session_set_rule(struct xccdf_session *session, const char *rule)
{
	session->rule = rule;
}

void xccdf_session_set_validation(struct xccdf_session *session, bool validate, bool full_validation)
{
	session->validate = validate;
	session->full_validation = full_validation;
}

void xccdf_session_set_thin_results(struct xccdf_session *session, bool thin_results)
{
	session->export.thin_results = thin_results;
}

void xccdf_session_set_datastream_id(struct xccdf_session *session, const char *datastream_id)
{
	oscap_free(session->ds.user_datastream_id);
	session->ds.user_datastream_id = oscap_strdup(datastream_id);
}

const char *xccdf_session_get_datastream_id(struct xccdf_session *session)
{
	if (session->ds.session != NULL) {
		return ds_sds_session_get_datastream_id(session->ds.session);
	}
	return session->ds.user_datastream_id;
}

void xccdf_session_set_component_id(struct xccdf_session *session, const char *component_id)
{
	oscap_free(session->ds.user_component_id);
	session->ds.user_component_id = oscap_strdup(component_id);
}

const char *xccdf_session_get_component_id(struct xccdf_session *session)
{
	if (session->ds.session != NULL) {
		return ds_sds_session_get_checklist_id(session->ds.session);
	}
	return session->ds.user_component_id;
}

void xccdf_session_set_benchmark_id(struct xccdf_session *session, const char *benchmark_id)
{
	oscap_free(session->ds.user_benchmark_id);
	session->ds.user_benchmark_id = oscap_strdup(benchmark_id);
}

const char *xccdf_session_get_benchmark_id(struct xccdf_session *session)
{
	return session->ds.user_benchmark_id;
}

const char *xccdf_session_get_result_id(struct xccdf_session *session)
{
	return xccdf_result_get_id(session->xccdf.result);
}

void xccdf_session_set_user_cpe(struct xccdf_session *session, const char *user_cpe)
{
	oscap_free(session->user_cpe);
	session->user_cpe = oscap_strdup(user_cpe);
}

void xccdf_session_set_user_tailoring_file(struct xccdf_session *session, const char *user_tailoring_file)
{
	oscap_source_free(session->tailoring.user_file);
	session->tailoring.user_file = user_tailoring_file != NULL ?
		oscap_source_new_from_file(user_tailoring_file) : NULL;
}

void xccdf_session_set_user_tailoring_cid(struct xccdf_session *session, const char *user_tailoring_cid)
{
	oscap_free(session->tailoring.user_component_id);
	session->tailoring.user_component_id = oscap_strdup(user_tailoring_cid);
}

void xccdf_session_set_custom_oval_eval_fn(struct xccdf_session *session, xccdf_policy_engine_eval_fn eval_fn)
{
	session->oval.user_eval_fn = eval_fn;
}

bool xccdf_session_set_product_cpe(struct xccdf_session *session, const char *product_cpe)
{
	oscap_free(session->oval.product_cpe);
	session->oval.product_cpe = oscap_strdup(product_cpe);
	return true;
}

void xccdf_session_set_without_sys_chars_export(struct xccdf_session *session, bool without_sys_chars)
{
	session->export.without_sys_chars = without_sys_chars;
}

void xccdf_session_set_oval_results_export(struct xccdf_session *session, bool to_export_oval_results)
{
	session->export.oval_results = to_export_oval_results;
}

void xccdf_session_set_oval_variables_export(struct xccdf_session *session, bool to_export_oval_variables)
{
	session->export.oval_variables = to_export_oval_variables;
}

void xccdf_session_set_check_engine_plugins_results_export(struct xccdf_session *session, bool to_export_results)
{
	session->export.check_engine_plugins_results = to_export_results;
}

void xccdf_session_set_sce_results_export(struct xccdf_session *session, bool to_export_sce_results)
{
	xccdf_session_set_check_engine_plugins_results_export(session, to_export_sce_results);
}

bool xccdf_session_set_arf_export(struct xccdf_session *session, const char *arf_file)
{
	oscap_free(session->export.arf_file);
	session->export.arf_file = oscap_strdup(arf_file);
	return true;
}

bool xccdf_session_set_xccdf_export(struct xccdf_session *session, const char *xccdf_file)
{
	oscap_free(session->export.xccdf_file);
	session->export.xccdf_file = oscap_strdup(xccdf_file);
	return true;
}

bool xccdf_session_set_report_export(struct xccdf_session *session, const char *report_file)
{
	oscap_free(session->export.report_file);
	session->export.report_file = oscap_strdup(report_file);
	return true;
}

bool xccdf_session_set_profile_id(struct xccdf_session *session, const char *profile_id)
{
	if (xccdf_policy_model_get_policy_by_id(session->xccdf.policy_model, profile_id) == NULL)
		return false;
	oscap_free(session->xccdf.profile_id);
	session->xccdf.profile_id = oscap_strdup(profile_id);
	return true;
}

// 0 = no error, 1 = no matches, 2 = multiple matches
int xccdf_session_set_profile_id_by_suffix(struct xccdf_session *session, const char *profile_suffix)
{
	const char *full_profile_id = NULL;
	struct xccdf_benchmark *bench = xccdf_policy_model_get_benchmark(session->xccdf.policy_model);
	bool multiple = false;

	// Tailoring Profiles
	struct xccdf_tailoring *tailoring = xccdf_policy_model_get_tailoring(session->xccdf.policy_model);
	if (tailoring != NULL)   {
		struct xccdf_profile_iterator *profit_tailoring = xccdf_tailoring_get_profiles(tailoring);
		while (xccdf_profile_iterator_has_more(profit_tailoring)) {
			struct xccdf_profile *tailoring_profile = xccdf_profile_iterator_next(profit_tailoring);
			const char *tailoring_profile_id = xccdf_profile_get_id(tailoring_profile);

			if (oscap_str_endswith(tailoring_profile_id, profile_suffix)) {
				if (full_profile_id != NULL) {
					oscap_seterr(OSCAP_EFAMILY_OSCAP, "Multiple matches found:\n%s\n%s\n",
						full_profile_id, tailoring_profile_id);
					multiple = true;
					break;
				} else {
					full_profile_id = tailoring_profile_id;
				}
			}
		}
		xccdf_profile_iterator_free(profit_tailoring);
	}

	// Benchmark Profiles
	if (full_profile_id == NULL) {
		struct xccdf_profile_iterator *profit_bench = xccdf_benchmark_get_profiles(bench);
		while (xccdf_profile_iterator_has_more(profit_bench)) {
			struct xccdf_profile *bench_profile = xccdf_profile_iterator_next(profit_bench);
			const char *bench_profile_id = xccdf_profile_get_id(bench_profile);

			if(oscap_str_endswith(bench_profile_id, profile_suffix)) {
				if (full_profile_id != NULL) {
					oscap_seterr(OSCAP_EFAMILY_OSCAP, "Multiple matches found:\n%s\n%s\n",
						full_profile_id, bench_profile_id);
					multiple = true;
					break;
				} else {
					full_profile_id = bench_profile_id;
				}
			}
		}
		xccdf_profile_iterator_free(profit_bench);
	}

	if (full_profile_id == NULL) {
		return 1;
	} else if (multiple) {
		return 2;
	} else {
		const bool search_result = xccdf_session_set_profile_id(session, full_profile_id);
		assert(search_result);
		return 0;
	}
}

const char *xccdf_session_get_profile_id(struct xccdf_session *session)
{
	return session->xccdf.profile_id;
}

static struct ds_sds_session *xccdf_session_get_ds_sds_session(struct xccdf_session *session)
{
	if (!xccdf_session_is_sds(session))
		return NULL;
	if (session->ds.session == NULL) {
		session->ds.session = ds_sds_session_new_from_source(session->source);
	}
	return session->ds.session;
}

void xccdf_session_set_remote_resources(struct xccdf_session *session, bool allowed, download_progress_calllback_t callback)
{
	if (callback == NULL) {
		// With empty cb we don't have to check for NULL
		// when we want to use it
		callback = download_progress_empty_calllback;
	}

	session->oval.fetch_remote_resources = allowed;
	session->oval.progress = callback;

	if (xccdf_session_is_sds(session)) {
		// We have to propagate this option to allow loading
		// of external datastream components
		ds_sds_session_set_remote_resources(xccdf_session_get_ds_sds_session(session), allowed, callback);
	}
}

/**
 * Get Source DataStream index of the session.
 * @memberof xccdf_session
 * @warning This is applicable only on sessions which are SDS.
 * @return sds index
 */
struct ds_sds_index *xccdf_session_get_sds_idx(struct xccdf_session *session)
{
	if (!xccdf_session_is_sds(session))
		return NULL;

	return ds_sds_session_get_sds_idx(xccdf_session_get_ds_sds_session(session));
}

int xccdf_session_load(struct xccdf_session *session)
{
	int ret = 0;

	if (session->ds.session) {
		ds_sds_session_reset(session->ds.session);
	}

	if ((ret = xccdf_session_load_xccdf(session)) != 0)
		return ret;
	if ((ret = xccdf_session_load_cpe(session)) != 0)
		return ret;
	if ((ret = xccdf_session_load_oval(session)) != 0)
		return ret;
	if ((ret = xccdf_session_load_check_engine_plugins(session)) != 0)
		return ret;
	return xccdf_session_load_tailoring(session);
}

static int _reporter(const char *file, int line, const char *msg, void *arg)
{
	oscap_seterr(OSCAP_EFAMILY_OSCAP, "File '%s' line %d: %s", file, line, msg);
	return 0;
}

static inline int _xccdf_session_load_xccdf_benchmark(struct xccdf_session *session)
{
	if (session->xccdf.policy_model != NULL) {
		xccdf_policy_model_free(session->xccdf.policy_model);
		session->xccdf.policy_model = NULL;
	}

	/* Validate documents */
	if (session->validate && (!xccdf_session_is_sds(session) || session->full_validation)) {
		if (oscap_source_validate(session->xccdf.source, _reporter, NULL)) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid %s (%s) content in %s",
					oscap_document_type_to_string(oscap_source_get_scap_type(session->source)),
					oscap_source_get_schema_version(session->source),
					oscap_source_readable_origin(session->source));
			return 1;
		}
	}

	/* Load XCCDF model and XCCDF Policy model */
	struct xccdf_benchmark *benchmark = xccdf_benchmark_import_source(session->xccdf.source);
	if (benchmark == NULL) {
		return 1;
	}

	/* create the policy model */
	session->xccdf.policy_model = xccdf_policy_model_new(benchmark);
	if (session->xccdf.policy_model == NULL) {
		xccdf_benchmark_free(benchmark);
		return 1;
	}
	return 0;
}

static int _acquire_xccdf_checklist_from_tailoring(struct xccdf_session* session)
{
	struct ds_sds_session *ds_sds_session = xccdf_session_get_ds_sds_session(session);
	if (ds_sds_session == NULL) {
		return 1;
	}

	xmlDoc *tailoring_xmlDoc = xmlCopyDoc(oscap_source_get_xmlDoc(session->xccdf.source), true);
	struct oscap_source *tailoring_source = oscap_source_new_from_xmlDoc(tailoring_xmlDoc, NULL);
	struct xccdf_tailoring* tailoring = xccdf_tailoring_import_source(tailoring_source, NULL);
	if (tailoring == NULL) {
		/* Freeing tailoring_source also frees the tailoring_xmlDoc used to create the source */
		oscap_source_free(tailoring_source);
		return 1;
	}
	const char *benchmark_ref = oscap_strdup(xccdf_tailoring_get_benchmark_ref(tailoring));
	xccdf_tailoring_free(tailoring);
	if (benchmark_ref == NULL) {
		oscap_source_free(tailoring_source);
		return 1;
	}

	struct oscap_source *xccdf_source = NULL;
	if (oscap_str_startswith(benchmark_ref, "file:")) {
		char* sep = strchr(benchmark_ref, '#');
		const char *filename = benchmark_ref + strlen("file:");
		const char *component_ref = NULL;
		if (sep != NULL) {
			component_ref = sep + 1;
			*sep = '\0';
		}

		struct oscap_source *external_file = oscap_source_new_from_file(filename);
		if (oscap_source_get_scap_type(external_file) == OSCAP_DOCUMENT_SDS) {
			if (component_ref == NULL) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP,
						"The referenced component is a datastream, but no datastream component was specified.");
				oscap_source_free(external_file);
				oscap_source_free(tailoring_source);
				oscap_free(benchmark_ref);
				return 1;
			}
			ds_sds_session_free(ds_sds_session);
			ds_sds_session = ds_sds_session_new_from_source(external_file);
			session->ds.session = ds_sds_session;
			xccdf_source = ds_sds_session_select_checklist(ds_sds_session, NULL, component_ref, NULL);
			oscap_source_free(session->source);
			session->source = external_file;
		} else {
			xccdf_source = external_file;
		}
	} else {
		xccdf_source = ds_sds_session_select_checklist(ds_sds_session,
				NULL, benchmark_ref + 1, NULL);
	}
	if (xccdf_source == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP,
				"Could not find benchmark referenced from tailoring as '%s'.", benchmark_ref);
		oscap_source_free(tailoring_source);
		oscap_free(benchmark_ref);
		return 1;
	}

	session->xccdf.source = xccdf_source;
	session->tailoring.user_file = tailoring_source;
	oscap_free(benchmark_ref);
	return 0;
}

int xccdf_session_load_xccdf(struct xccdf_session *session)
{
	session->xccdf.source = NULL;

	if (xccdf_session_is_sds(session)) {
		if (session->validate) {
			if (oscap_source_validate(session->source, _reporter, NULL)) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid %s (%s) content in %s",
						oscap_document_type_to_string(oscap_source_get_scap_type(session->source)),
						oscap_source_get_schema_version(session->source),
						oscap_source_readable_origin(session->source));
				return 1;
			}
		}
		session->xccdf.source = ds_sds_session_select_checklist(xccdf_session_get_ds_sds_session(session), session->ds.user_datastream_id,
				session->ds.user_component_id, session->ds.user_benchmark_id);
		if (session->xccdf.source == NULL) {
			return 1;
		}
		if (oscap_source_get_scap_type(session->xccdf.source) == OSCAP_DOCUMENT_XCCDF_TAILORING) {
			if (_acquire_xccdf_checklist_from_tailoring(session)) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP,
						"Could not find appropriate checklist to tailor.");
				return 1;
			}
		}
		if (oscap_source_get_scap_type(session->xccdf.source) != OSCAP_DOCUMENT_XCCDF) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "The selected checklist document is not '%s', but '%s'.",
					oscap_document_type_to_string(OSCAP_DOCUMENT_XCCDF),
					oscap_document_type_to_string(oscap_source_get_scap_type(session->xccdf.source)));
			return 1;
		}
	}
	else {
		session->xccdf.source = session->source;
	}

	return _xccdf_session_load_xccdf_benchmark(session);
}
static inline void _connect_cpe_session_with_sds(struct xccdf_session *session)
{
	struct cpe_session *cpe_session = xccdf_policy_model_get_cpe_session(session->xccdf.policy_model);
	struct oscap_htable *sources_cache = ds_sds_session_get_component_sources(xccdf_session_get_ds_sds_session(session));
	cpe_session_set_cache(cpe_session, sources_cache);
}

int xccdf_session_load_cpe(struct xccdf_session *session)
{
	if (session == NULL || session->xccdf.policy_model == NULL)
		return 1;

	// The CPE session will load OVAL files for any CPE dicts that require it.
	// These OVAL files are outside of scope of XCCDF session but we still want
	// to apply the thin results settings to them.
	struct cpe_session *cpe_session = xccdf_policy_model_get_cpe_session(session->xccdf.policy_model);
	cpe_session_set_thin_results(cpe_session, session->export.thin_results);

	/* Use custom CPE dict if given */
	if (session->user_cpe != NULL) {
		struct oscap_source *source = oscap_source_new_from_file(session->user_cpe);
		if (oscap_source_validate(source, _reporter, NULL) != 0) {
			oscap_source_free(source);
			return 1;
		}
		if (!xccdf_policy_model_add_cpe_autodetect_source(session->xccdf.policy_model, source)) {
			oscap_source_free(source);
			return 1;
		}
		oscap_source_free(source);
	}

	if (xccdf_session_is_sds(session)) {
		struct ds_sds_index *sds_idx = xccdf_session_get_sds_idx(session);
		if (sds_idx == NULL) {
			return -1;
		}
		struct ds_stream_index* stream_idx = ds_sds_index_get_stream(sds_idx, xccdf_session_get_datastream_id(session));
		struct oscap_string_iterator* cpe_it = ds_stream_index_get_dictionaries(stream_idx);

		// This potentially allows us to skip yet another decompose if we are sure
		// there are no CPE dictionaries or language models inside the datastream.
		if (oscap_string_iterator_has_more(cpe_it)) {
			if (ds_sds_session_register_component_with_dependencies(xccdf_session_get_ds_sds_session(session),
					"dictionaries", NULL, NULL) != 0) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't decompose CPE dictionaries from datastream '%s' "
						"from file '%s'!\n", xccdf_session_get_datastream_id(session),
						oscap_source_readable_origin(session->source));
				oscap_string_iterator_free(cpe_it);
				return 1;
			}
			_connect_cpe_session_with_sds(session);
			while (oscap_string_iterator_has_more(cpe_it)) {
				const char* cpe_filename = oscap_string_iterator_next(cpe_it);

				struct oscap_source *source = ds_sds_session_get_component_by_href(xccdf_session_get_ds_sds_session(session), cpe_filename);

				if (session->full_validation) {
					if (oscap_source_validate(source, _reporter, NULL) != 0) {
						oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid %s (%s) content in %s",
							oscap_document_type_to_string(oscap_source_get_scap_type(source)),
							oscap_source_get_schema_version(source),
							oscap_source_readable_origin(source));
						oscap_string_iterator_free(cpe_it);
						return 1;
					}
				}
				if (!xccdf_policy_model_add_cpe_autodetect_source(session->xccdf.policy_model, source)) {
					oscap_string_iterator_free(cpe_it);
					return 1;
				}
			}
		}
		oscap_string_iterator_free(cpe_it);
	}
	return 0;
}

static void _oval_content_resources_free(struct oval_content_resource **resources)
{
	if (resources) {
		for (int i=0; resources[i]; i++) {
			free(resources[i]->href);
			if (resources[i]->source_owned) {
				oscap_source_free(resources[i]->source);
			}
			free(resources[i]);
		}
		free(resources);
	}
}

void xccdf_session_set_custom_oval_files(struct xccdf_session *session, char **oval_filenames)
{
	_oval_content_resources_free(session->oval.custom_resources);
	session->oval.custom_resources = NULL;
	if (oval_filenames == NULL)
		return;

	struct oval_content_resource **resources = malloc(sizeof(struct oval_content_resource *));
	resources[0] = NULL;

	for (int i = 0; oval_filenames[i];) {
		resources[i] = malloc(sizeof(struct oval_content_resource));
		resources[i]->href = strdup(basename(oval_filenames[i]));
		resources[i]->source = oscap_source_new_from_file(oval_filenames[i]);
		resources[i]->source_owned = true;
		i++;
		resources = realloc(resources, (i + 1) * sizeof(struct oval_content_resource *));
		resources[i] = NULL;
	}
	session->oval.custom_resources = resources;
}

static int _xccdf_session_get_oval_from_model(struct xccdf_session *session)
{
	struct oval_content_resource **resources = NULL;
	struct oscap_file_entry_list *files = NULL;
	struct oscap_file_entry_iterator *files_it = NULL;
	int idx = 0;
	char *tmp_path;
	char *printable_path;
	bool fetch_option_suggested = false;
	char *xccdf_path_cpy = NULL;
	char *dir_path = NULL;

	_oval_content_resources_free(session->oval.resources);

	xccdf_path_cpy = strdup(oscap_source_readable_origin(session->xccdf.source));
	dir_path = dirname(xccdf_path_cpy);

	resources = malloc(sizeof(struct oval_content_resource *));
	resources[idx] = NULL;

	files = xccdf_policy_model_get_systems_and_files(session->xccdf.policy_model);
	files_it = oscap_file_entry_list_get_files(files);
	while (oscap_file_entry_iterator_has_more(files_it)) {
		struct oscap_file_entry *file_entry;
		struct stat sb;

		file_entry = (struct oscap_file_entry *) oscap_file_entry_iterator_next(files_it);

		// we only care about OVAL referenced files
		if (strcmp(oscap_file_entry_get_system(file_entry), oval_sysname))
			continue;

		const char *file_path = oscap_file_entry_get_file(file_entry);
		struct oscap_source *source = NULL;
		if (xccdf_session_get_ds_sds_session(session) != NULL) {
			source = ds_sds_session_get_component_by_href(xccdf_session_get_ds_sds_session(session), file_path);
		}

		tmp_path = malloc(PATH_MAX * sizeof(char));
		if (file_path[0] == '/') { // it's a simple absolute path
			snprintf(tmp_path, PATH_MAX, "%s", file_path);
		}
		else if (oscap_str_startswith(file_path, "file://")) { // it's an absolute path prefixed with file://
			snprintf(tmp_path, PATH_MAX, "%s", file_path + 7);
		}
		else { // assume it's a relative path
			snprintf(tmp_path, PATH_MAX, "%s/%s", dir_path, file_path);
		}

		if (source != NULL || stat(tmp_path, &sb) == 0) {
			resources[idx] = malloc(sizeof(struct oval_content_resource));
			resources[idx]->href = strdup(oscap_file_entry_get_file(file_entry));
			if (source == NULL) {
				source = oscap_source_new_from_file(tmp_path);
				resources[idx]->source_owned = true;
			}
			else {
				resources[idx]->source_owned = false;
			}
			resources[idx]->source = source;
			idx++;
			resources = realloc(resources, (idx + 1) * sizeof(struct oval_content_resource *));
			resources[idx] = NULL;
		}
		else {
			if (oscap_acquire_url_is_supported(oscap_file_entry_get_file(file_entry))) {
				// Strip out the 'path' for printing the url.
				printable_path = (char *) oscap_file_entry_get_file(file_entry);

				if (session->oval.fetch_remote_resources) {
					session->oval.progress(false, "Downloading: %s ... ", printable_path);

					size_t data_size;
					char *data = oscap_acquire_url_download(printable_path, &data_size);
					if (data == NULL) {
						session->oval.progress(false, "error\n");
					} else {
						session->oval.progress(false, "ok\n");

						resources[idx] = malloc(sizeof(struct oval_content_resource));
						resources[idx]->href = strdup(printable_path);
						resources[idx]->source = oscap_source_new_take_memory(data, data_size, printable_path);
						resources[idx]->source_owned = true;
						idx++;
						resources = realloc(resources, (idx + 1) * sizeof(struct oval_content_resource *));
						resources[idx] = NULL;
						free(tmp_path);
						continue;
					}
				}
				else if (!fetch_option_suggested) {
					session->oval.progress(true, "WARNING: This content points out to the remote resources. "
								"Use `--fetch-remote-resources' option to download them.\n");
					fetch_option_suggested = true;
				}
			}
			else
				printable_path = tmp_path;
			session->oval.progress(true, "WARNING: Skipping %s file which is referenced from XCCDF content\n", printable_path);
		}
		free(tmp_path);
	}
	oscap_file_entry_iterator_free(files_it);
	oscap_file_entry_list_free(files);
	free(xccdf_path_cpy);
	session->oval.resources = resources;
	return 0;
}

static void _xccdf_session_free_oval_agents(struct xccdf_session *session)
{
	if (session->oval.agents != NULL) {
		for (int i=0; session->oval.agents[i]; i++) {
			struct oval_definition_model *def_model = oval_agent_get_definition_model(session->oval.agents[i]);
			oval_definition_model_free(def_model);
			oval_agent_destroy_session(session->oval.agents[i]);
		}
		free(session->oval.agents);
		session->oval.agents = NULL;
	}
}

int xccdf_session_load_oval(struct xccdf_session *session)
{
	struct oval_content_resource **contents = NULL;

	_xccdf_session_free_oval_agents(session);

	/* Locate all OVAL files */
	if (session->oval.custom_resources == NULL) {
		/* Use OVAL files from policy model */
		if (_xccdf_session_get_oval_from_model(session) != 0)
			return 1;
	}

	contents = session->oval.custom_resources != NULL ? session->oval.custom_resources : session->oval.resources;

	/* Validate OVAL files. Only validate if the file doesn't come from a datastream
	 * or if full validation was explicitly requested.
	 */
	if (session->validate && (!xccdf_session_is_sds(session) || session->full_validation)) {
		for (int idx=0; contents[idx]; idx++) {
			if (oscap_source_validate(contents[idx]->source, _reporter, NULL) != 0) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid %s (%s) content in %s",
						oscap_document_type_to_string(oscap_source_get_scap_type(session->source)),
						oscap_source_get_schema_version(session->source),
						contents[idx]->href);
				return 1;
			}
		}
	}

	for (int idx=0; contents[idx]; idx++) {
		/* file -> def_model */
		struct oval_definition_model *tmp_def_model = oval_definition_model_import_source(contents[idx]->source);
		if (tmp_def_model == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to create OVAL definition model from: '%s'.",
				oscap_source_readable_origin(contents[idx]->source));
			return 1;
		}

		/* def_model -> session */
		struct oval_agent_session *tmp_sess = oval_agent_new_session(tmp_def_model, contents[idx]->href);
		if (tmp_sess == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to create new OVAL agent session for: '%s'.", contents[idx]->href);
			oval_definition_model_free(tmp_def_model);
			return 2;
		}

		if (session->export.thin_results) {
			struct oval_results_model *res_model = oval_agent_get_results_model(tmp_sess);
			struct oval_directives_model *dir_model = oval_results_model_get_directives_model(res_model);
			// This is the worst function name in existence, despite its name,
			// it's getting the oval_result_directives of the oval_directives_model.
			// You would expect oval_directives_model_getresdirs at least, but no..
			struct oval_result_directives *dir = oval_directives_model_get_defdirs(dir_model);
			oval_result_directives_set_content(dir,  OVAL_RESULT_TRUE | OVAL_RESULT_FALSE |
							OVAL_RESULT_UNKNOWN | OVAL_RESULT_NOT_EVALUATED |
							OVAL_RESULT_NOT_APPLICABLE | OVAL_RESULT_ERROR,
							OVAL_DIRECTIVE_CONTENT_THIN);
		}

		/* store our name in the generated documents */
		oval_agent_set_product_name(tmp_sess, session->oval.product_cpe != NULL ?
				session->oval.product_cpe : (char *) oscap_productname);

		/* remember sessions */
		session->oval.agents = realloc(session->oval.agents, (idx + 2) * sizeof(struct oval_agent_session *));
		session->oval.agents[idx] = tmp_sess;
		session->oval.agents[idx+1] = NULL;

		/* register session */
		if (session->oval.user_eval_fn != NULL)
			xccdf_policy_model_register_engine_and_query_callback(
					session->xccdf.policy_model, (char *) oval_sysname,
					session->oval.user_eval_fn, tmp_sess, NULL);
		else
			xccdf_policy_model_register_engine_oval(session->xccdf.policy_model, tmp_sess);
	}
	return 0;
}

int xccdf_session_load_check_engine_plugin2(struct xccdf_session *session, const char *plugin_name, bool quiet)
{
	struct check_engine_plugin_def *plugin = check_engine_plugin_load2(plugin_name, quiet);

	if (!plugin)
		return -1; // error already set

	oscap_list_add(session->check_engine_plugins, plugin);

	if (xccdf_session_get_datastream_id(session) != NULL){
		return check_engine_plugin_register(plugin, session->xccdf.policy_model, ds_sds_session_get_target_dir(session->ds.session));
	} else {
		char* xccdf_filename = oscap_strdup(oscap_source_readable_origin(session->xccdf.source));
		int res = check_engine_plugin_register(plugin, session->xccdf.policy_model, dirname(xccdf_filename));
		oscap_free(xccdf_filename);
		return res;
	}
}

int xccdf_session_load_check_engine_plugin(struct xccdf_session *session, const char *plugin_name)
{
	return xccdf_session_load_check_engine_plugin2(session, plugin_name, false);
}

int xccdf_session_load_check_engine_plugins(struct xccdf_session *session)
{
	xccdf_session_unload_check_engine_plugins(session);

	const char * const *known_plugins = check_engine_plugin_get_known_plugins();

	while (*known_plugins) {
		// We do not report failure when a known plugin doesn't load properly, that's because they
		// are optional and we don't know if it's not there or if it just failed to load.
		if (xccdf_session_load_check_engine_plugin2(session, *known_plugins, true) != 0)
			oscap_clearerr();

		known_plugins++;
	}

	return 0;
}

static void xccdf_session_unload_check_engine_plugins(struct xccdf_session *session)
{
	struct oscap_iterator *it = oscap_iterator_new(session->check_engine_plugins);

	while (oscap_iterator_has_more(it)) {
		struct check_engine_plugin_def *plugin = (struct check_engine_plugin_def *)oscap_iterator_next(it);

		check_engine_plugin_cleanup(plugin, session->xccdf.policy_model);
		check_engine_plugin_unload(plugin);
	}

	oscap_iterator_free(it);

	oscap_list_free0(session->check_engine_plugins);
	session->check_engine_plugins = oscap_list_new();
}

int xccdf_session_load_sce(struct xccdf_session *session)
{
	return xccdf_session_load_check_engine_plugins(session);
}

int xccdf_session_load_tailoring(struct xccdf_session *session)
{
	bool from_sds = false;
	struct oscap_source *tailoring_source = NULL;

	if (session->tailoring.user_file != NULL) {
		tailoring_source = session->tailoring.user_file;
	}
	else if (session->tailoring.user_component_id != NULL) {
		if (!xccdf_session_is_sds(session)) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't use given tailoring component ID because file isn't a source datastream.");
			return 1;
		}

		tailoring_source = ds_sds_session_select_tailoring(xccdf_session_get_ds_sds_session(session), session->tailoring.user_component_id);
		if (tailoring_source == NULL) {
			return 1;
		}

		from_sds = true;
	}

	// TODO: We should warn if has a XCCDF tailoring-file hint and user isn't
	//       using that particular tailoring.

	if (tailoring_source == NULL)
		return 0; // nothing to do

	if (session->validate && (!from_sds || session->full_validation)) {
		if (oscap_source_validate(tailoring_source, _reporter, NULL) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid %s (%s) content in %s",
					oscap_document_type_to_string(oscap_source_get_scap_type(tailoring_source)),
					oscap_source_get_schema_version(tailoring_source),
					oscap_source_readable_origin(tailoring_source));
			oscap_source_free(tailoring_source);
			session->tailoring.user_file = NULL;
			return 1;
		}
	}

	struct xccdf_benchmark *benchmark = xccdf_policy_model_get_benchmark(session->xccdf.policy_model);
	struct xccdf_tailoring *tailoring = xccdf_tailoring_import_source(tailoring_source, benchmark);

	if (tailoring == NULL)
		return 1;

	return xccdf_policy_model_set_tailoring(session->xccdf.policy_model, tailoring) ? 0 : 1;
}

int xccdf_session_evaluate(struct xccdf_session *session)
{
	struct xccdf_policy *policy = xccdf_session_get_xccdf_policy(session);
	if (policy == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Cannot build xccdf_policy.");
		return 1;
	}
	policy->rule = session->rule;

	session->xccdf.result = xccdf_policy_evaluate(policy);
	if (session->xccdf.result == NULL)
		return 1;

	/* Write results into XCCDF Test Result model */
	xccdf_result_set_benchmark_uri(session->xccdf.result, oscap_source_readable_origin(session->source));
	struct oscap_text *title = oscap_text_new();
	oscap_text_set_text(title, "OSCAP Scan Result");
	xccdf_result_add_title(session->xccdf.result, title);
	struct xccdf_benchmark *benchmark = xccdf_policy_get_benchmark(policy);
	xccdf_result_set_version(session->xccdf.result,
			benchmark != NULL ? xccdf_benchmark_get_version(benchmark) : NULL);

	xccdf_result_fill_sysinfo(session->xccdf.result);

	struct xccdf_model_iterator *model_it = xccdf_benchmark_get_models(xccdf_policy_model_get_benchmark(session->xccdf.policy_model));
	while (xccdf_model_iterator_has_more(model_it)) {
		struct xccdf_model *model = xccdf_model_iterator_next(model_it);
		const char *score_model = xccdf_model_get_system(model);
		struct xccdf_score *score = xccdf_policy_get_score(policy, session->xccdf.result, score_model);
		xccdf_result_add_score(session->xccdf.result, score);

		/* record default base score for later use */
		if (!strcmp(score_model, "urn:xccdf:scoring:default"))
			session->xccdf.base_score = xccdf_score_get_score(score);
	}
	xccdf_model_iterator_free(model_it);
	return 0;
}

static size_t _paramlist_size(const char **p) { size_t s = 0; if (!p) return s; while (p[s]) s += 2; return s; }

static size_t _paramlist_cpy(const char **to, const char **p) {
	size_t s = 0;
	if (!p) return s;
	for (;p && p[s]; s += 2) to[s] = p[s], to[s+1] = p[s+1];
	to[s] = p[s];
	return s;
}

static int _app_xslt(struct oscap_source *infile, const char *xsltfile, const char *outfile, const char **params)
{
	char pwd[PATH_MAX];

	if (getcwd(pwd, sizeof(pwd)) == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Getcwd() failed: %s", strerror(errno));
		return 1;
	}

	/* add params oscap-version & pwd */
	const char *stdparams[] = { "oscap-version", oscap_get_version(), "pwd", pwd, NULL };
	const char *par[_paramlist_size(params) + _paramlist_size(stdparams) + 1];
	size_t s = _paramlist_cpy(par, params);
	s += _paramlist_cpy(par + s, stdparams);

	return oscap_source_apply_xslt_path(infile, xsltfile, outfile, par, oscap_path_to_xslt()) == -1;
}

static inline int _xccdf_gen_report(struct oscap_source *infile, const char *id, const char *outfile, const char *show, const char* sce_template, const char* profile)
{
	const char *params[] = {
		"result-id",		id,
		"show",			show,
		"profile",		profile,
		"oval-template",	"",
		"sce-template",		sce_template,
		"verbosity",		"",
		"hide-profile-info",	NULL,
		NULL};

	return _app_xslt(infile, "xccdf-report.xsl", outfile, params);
}

static int _build_xccdf_result_source(struct xccdf_session *session)
{
	if (session->xccdf.result_source != NULL) {
		return 0;
	}

	/* Build oscap_source of XCCDF TestResult only when needed */
	if (session->export.xccdf_file != NULL || session->export.report_file != NULL || session->export.arf_file != NULL) {
		if (session->xccdf.result == NULL) {
			// Attempt to export session before evaluation
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "No XCCDF results to export.");
			return 1;
		}
		xccdf_benchmark_add_result(xccdf_policy_model_get_benchmark(session->xccdf.policy_model),
				xccdf_result_clone(session->xccdf.result));
		session->xccdf.result_source = xccdf_benchmark_export_source(
				xccdf_policy_model_get_benchmark(session->xccdf.policy_model), session->export.xccdf_file);

		if (session->export.xccdf_file != NULL) {
			// Export XCCDF result file only when explicitly requested
			if (oscap_source_save_as(session->xccdf.result_source, NULL) != 0) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not save file: %s",
						oscap_source_readable_origin(session->xccdf.result_source));
				return -1;
			}
		}

		/* validate XCCDF Results */
		if (session->validate && session->full_validation) {
			if (oscap_source_validate(session->xccdf.result_source, _reporter, NULL)) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not export XCCDF Results correctly to %s",
					oscap_source_readable_origin(session->xccdf.result_source));
				return 1;
			}
		}
	}
	return 0;
}

int xccdf_session_export_xccdf(struct xccdf_session *session)
{
	if (_build_xccdf_result_source(session)) {
		return 1;
	}

	if (session->export.report_file == NULL)
		return 0;

	struct oscap_source* results = session->xccdf.result_source;
	struct oscap_source* arf = NULL;
	if (session->export.oval_results) {
		arf = xccdf_session_create_arf_source(session);
		if (arf == NULL) {
			return 1;
		}
		results = arf;
	}

	/* generate report */
	_xccdf_gen_report(results,
			xccdf_result_get_id(session->xccdf.result),
			session->export.report_file,
			"",
			(session->export.check_engine_plugins_results ? "%.result.xml" : ""),
			session->xccdf.profile_id == NULL ? "" : session->xccdf.profile_id
	);

	return 0;
}

static void _xccdf_session_free_oval_result_sources(struct xccdf_session *session)
{
	if (session->oval.result_sources != NULL) {
		oscap_htable_free(session->oval.result_sources, (oscap_destruct_func) oscap_source_free);
		session->oval.result_sources = NULL;
	}
}

static char *_xccdf_session_get_unique_oval_result_filename(struct xccdf_session *session, struct oval_agent_session *oval_session, const char *oval_results_directory)
{
	char *escaped_url = NULL;
	const char *filename = oval_agent_get_filename(oval_session);
	if (!filename) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Given oval_session doesn't have a valid filename. "
			"Can't deduce destination where we should be exporting it!");
		return NULL;
	}

	if (oscap_acquire_url_is_supported(filename) || strchr(filename, '/')) {
		// We need escaping if:
		// - filename is a URL
		// - filename is an absolute path

		escaped_url = oscap_acquire_url_to_filename(filename);
		if (escaped_url == NULL)
			return NULL;
	}
	else if (!session->export.oval_results && strchr(filename, '/')) {
		// We need recursive mkdir_p if:
		// - filename is not a basename and we are exporting to a temp dir (ARF)
		// (we are assuming that the dir structure exists if we are exporting to '.')
		char *filename_cpy = oscap_sprintf("%s/%s", oval_results_directory, filename);
		if (oscap_acquire_ensure_parent_dir(filename_cpy) != 0) {
			oscap_free(filename_cpy);
			return NULL;
		}
		oscap_free(filename_cpy);
	}

	char *name = NULL;
	unsigned int suffix = 1;
	while (suffix < UINT_MAX)
	{
		name = malloc(PATH_MAX * sizeof(char));
		if (suffix == 1)
			snprintf(name, PATH_MAX, "%s/%s.result.xml", oval_results_directory, escaped_url != NULL ? escaped_url : filename);
		else
			snprintf(name, PATH_MAX, "%s/%s.result%i.xml", oval_results_directory, escaped_url != NULL ? escaped_url : filename, suffix);

		// Try to guess how the real path will look like. This should avoid us rewriting
		// the results files if the OVAL happens to have the same name. We allow users
		// to shoot themselves to the foot, but it is not easy. They need to set-up
		// file->file symlink before scanning.
		char *final_name = oscap_acquire_guess_realpath(name);
		free(name);
		name = final_name;
		if (name == NULL) {
			oscap_free(escaped_url);
			return NULL;
		}
		if (oscap_htable_get(session->oval.result_sources, name) == NULL) {
			// Check if this export name conflicts with any other exported OVAL result.
			//
			// One example where a conflict can easily happen is if we have the
			// same OVAL file used for CPE platform evaluation and check evaluation.
			//
			// openscap will create 2 OVAL sessions for the same file and will try
			// to export 2 different OVAL result files to the same path.
			break;
		}

		free(name);
		name = NULL;
		++suffix;
	}

	if (escaped_url != NULL)
		free(escaped_url);

	return name;
}

static char *_xccdf_session_export_oval_result_file(struct xccdf_session *session, struct oval_agent_session *oval_session)
{
	/* get result model and session name */
	struct oval_results_model *res_model = oval_agent_get_results_model(oval_session);
	/* Import XCCDF session without_sys_chars flag to res_model */
	oval_results_model_set_export_system_characteristics(res_model, !session->export.without_sys_chars);
	const char* oval_results_directory = NULL;

	if (session->export.oval_results == true) {
		oval_results_directory = ".";
	}
	else {
		if (!session->temp_dir)
			session->temp_dir = oscap_acquire_temp_dir();
		if (session->temp_dir == NULL)
			return NULL;

		oval_results_directory = session->temp_dir;
	}

	char *name = _xccdf_session_get_unique_oval_result_filename(session, oval_session, oval_results_directory);

	if (name == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't figure out the right filename for OVAL result file. Can't export that file!");
		return NULL;
	}

	struct oscap_source *source = oval_results_model_export_source(res_model, NULL, name);
	if (source == NULL) {
		free(name);
		return NULL;
	}
	if (oscap_htable_add(session->oval.result_sources, name, source) == false) {
		// The source is already there, but it shouldn't be
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Internal error: attempted to export file %s twice", name);
		oscap_source_free(source);
		free(name);
		abort(); // Let's make this visible in debug mode
		return NULL;
	}

	static int counter = 0;
	char *report_id = oscap_sprintf("oval%d", counter++);
	const char *original_name = oval_agent_get_filename(oval_session);
	char *results_file_name = oscap_strdup(name);
	oscap_htable_add(session->oval.results_mapping, original_name, results_file_name);
	oscap_htable_add(session->oval.arf_report_mapping, original_name, report_id);

	/* validate OVAL Results */
	if (session->validate && session->full_validation) {
		if (oscap_source_validate(source, _reporter, NULL)) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not export OVAL Results correctly to %s",
				oscap_source_readable_origin(source));
			free(name);
			return NULL;
		}
	}
	return name;
}

static int _build_oval_result_sources(struct xccdf_session *session)
{
	if (session->oval.result_sources != NULL) {
		return 0;
	}

	/* Export OVAL results */
	session->oval.result_sources = oscap_htable_new();
	session->oval.results_mapping = oscap_htable_new();
	session->oval.arf_report_mapping = oscap_htable_new();
	if (session->oval.agents) {
		for (int i = 0; session->oval.agents[i]; i++) {
			char *filename = _xccdf_session_export_oval_result_file(session, session->oval.agents[i]);
			if (filename == NULL) {
				_xccdf_session_free_oval_result_sources(session);
				return 1;
			}
			oscap_free(filename);
		}
	}

	struct oscap_htable_iterator *cpe_it = xccdf_policy_model_get_cpe_oval_sessions(session->xccdf.policy_model);
	while (oscap_htable_iterator_has_more(cpe_it)) {
		const char *key = NULL;
		struct oval_agent_session *value = NULL;
		oscap_htable_iterator_next_kv(cpe_it, &key, (void*)&value);

		char *filename = _xccdf_session_export_oval_result_file(session, value);
		if (filename == NULL) {
			_xccdf_session_free_oval_result_sources(session);
			oscap_htable_iterator_free(cpe_it);
			return 1;
		}
		oscap_free(filename);
	}
	oscap_htable_iterator_free(cpe_it);
	return 0;
}

int xccdf_session_export_oval(struct xccdf_session *session)
{
	if (session->export.oval_results || session->export.arf_file != NULL) {
		if (_build_oval_result_sources(session) != 0) {
			return 1;
		}
		struct oscap_htable_iterator *hit = oscap_htable_iterator_new(session->oval.result_sources);
		while (oscap_htable_iterator_has_more(hit)) {
			struct oscap_source *source = oscap_htable_iterator_next_value(hit);
			if (oscap_source_save_as(source, NULL) != 0) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not save file: %s", oscap_source_readable_origin(source));
				oscap_htable_iterator_free(hit);
				return 1;
			}
		}
		oscap_htable_iterator_free(hit);
	}

	/* Export variables */
	if (session->export.oval_variables && session->oval.agents != NULL) {
		for (int i = 0; session->oval.agents[i]; ++i) {
			int j;
			char *sname;
			struct oval_definition_model *defmod;
			struct oval_variable_model_iterator *varmod_itr;
			char *escaped_url = NULL;

			sname = (char *) oval_agent_get_filename(session->oval.agents[i]);
			if (oscap_acquire_url_is_supported(sname)) {
				escaped_url = oscap_acquire_url_to_filename(sname);
				if (escaped_url == NULL)
					return 1;
				sname = escaped_url;
			}
			defmod = oval_agent_get_definition_model(session->oval.agents[i]);
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
	return 0;
}

int xccdf_session_export_check_engine_plugins(struct xccdf_session *session)
{
	if (!session->export.check_engine_plugins_results)
		return 0;

	struct oscap_iterator *it = oscap_iterator_new(session->check_engine_plugins);

	int ret = 0;
	while (oscap_iterator_has_more(it)) {
		struct check_engine_plugin_def *plugin = (struct check_engine_plugin_def *)oscap_iterator_next(it);

		if (check_engine_plugin_export_results(
				plugin,
				session->xccdf.policy_model,
				session->validate && session->full_validation,
				oscap_source_readable_origin(session->xccdf.source)) != 0)
			ret = 1;
	}

	oscap_iterator_free(it);

	return ret;
}

int xccdf_session_export_sce(struct xccdf_session *session)
{
	return xccdf_session_export_check_engine_plugins(session);
}

int xccdf_session_export_arf(struct xccdf_session *session)
{
	if (session->export.arf_file != NULL) {
		struct oscap_source* arf_source = xccdf_session_create_arf_source(session);
		if (arf_source == NULL) {
			return 1;
		}

		if (oscap_source_save_as(arf_source, NULL) != 0) {
			oscap_source_free(arf_source);
			session->oval.arf_report = NULL;
			return 1;
		}
		if (session->full_validation) {
			if (oscap_source_validate(arf_source, _reporter, NULL) != 0) {
				oscap_source_free(arf_source);
				return 1;
			}
		}
	}
	return 0;
}

OSCAP_GENERIC_GETTER(struct xccdf_policy_model *, xccdf_session, policy_model, xccdf.policy_model)
OSCAP_GENERIC_GETTER(float, xccdf_session, base_score, xccdf.base_score);

struct xccdf_policy *xccdf_session_get_xccdf_policy(const struct xccdf_session *session)
{
	if (session->xccdf.policy_model == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Cannot build xccdf_policy.");
		return NULL;
	}
	return xccdf_policy_model_get_policy_by_id(session->xccdf.policy_model, session->xccdf.profile_id);
}

unsigned int xccdf_session_get_oval_agents_count(const struct xccdf_session *session)
{
	unsigned int i = 0;
	if (session->oval.agents != NULL)
		while (session->oval.agents[i])
			i++;
	return i;
}

unsigned int xccdf_session_get_cpe_oval_agents_count(const struct xccdf_session *session)
{
	if (session->xccdf.policy_model == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Cannot build xccdf_policy.");
		return 0;
	}

	unsigned int i = 0;
	struct oscap_htable_iterator *it = xccdf_policy_model_get_cpe_oval_sessions(session->xccdf.policy_model);
	while (oscap_htable_iterator_has_more(it)) {
		oscap_htable_iterator_next(it);
		i++;
	}
	oscap_htable_iterator_free(it);

	return i;
}

bool xccdf_session_contains_fail_result(const struct xccdf_session *session)
{
	struct xccdf_rule_result_iterator *res_it = xccdf_result_get_rule_results(session->xccdf.result);
	while (xccdf_rule_result_iterator_has_more(res_it)) {
		struct xccdf_rule_result *res = xccdf_rule_result_iterator_next(res_it);
		xccdf_test_result_type_t rule_result = xccdf_rule_result_get_result(res);
		if (rule_result == XCCDF_RESULT_FAIL ||
				rule_result == XCCDF_RESULT_UNKNOWN ||
				rule_result == XCCDF_RESULT_ERROR) {
			xccdf_rule_result_iterator_free(res_it);
			return true;
		}
	}
	xccdf_rule_result_iterator_free(res_it);
	return false;
}

int xccdf_session_remediate(struct xccdf_session *session)
{
	int res = 0;
	if (session == NULL || session->xccdf.policy_model == NULL ||
			xccdf_session_get_xccdf_policy(session) == NULL ||
			session->xccdf.result == NULL)
		return 1;
	if(getenv("OSCAP_PROBE_ROOT") != NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Can't perform remediation in offline mode: not implemented");
		return 1;
	}
	xccdf_policy_model_unregister_engines(session->xccdf.policy_model, oval_sysname);
	if ((res = xccdf_session_load_oval(session)) != 0)
		return res;
	struct xccdf_benchmark *benchmark = xccdf_policy_get_benchmark(xccdf_session_get_xccdf_policy(session));
	xccdf_result_set_version(session->xccdf.result,
			benchmark != NULL ? xccdf_benchmark_get_version(benchmark) : NULL);
	xccdf_result_fill_sysinfo(session->xccdf.result);

	if ((res = xccdf_policy_remediate(xccdf_session_get_xccdf_policy(session), session->xccdf.result)) != 0)
		return res;

	return xccdf_policy_recalculate_score(xccdf_session_get_xccdf_policy(session), session->xccdf.result);
}

int xccdf_session_build_policy_from_testresult(struct xccdf_session *session, const char *testresult_id)
{
	if (session->xccdf.result_source == NULL) {
		session->xccdf.result = NULL;
		struct xccdf_benchmark *benchmark = xccdf_policy_model_get_benchmark(session->xccdf.policy_model);
		struct xccdf_result *result = xccdf_benchmark_get_result_by_id_suffix(benchmark, testresult_id);
		if (result == NULL) {
			if (testresult_id == NULL)
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not find latest TestResult element.");
			else
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not find TestResult/@id=\"%s\"", testresult_id);
			return 1;
		}
		session->xccdf.result = xccdf_result_clone(result);
	}

	const char *profile_id = xccdf_result_get_profile(session->xccdf.result);
	if (xccdf_session_set_profile_id(session, profile_id) == false) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP,
			"Could not find Profile/@id=\"%s\" to build policy from TestResult/@id=\"%s\"",
			profile_id, testresult_id);
		return 1;
	}
	struct xccdf_policy *xccdf_policy = xccdf_session_get_xccdf_policy(session);
	if (xccdf_policy == NULL)
		return 1;
	xccdf_result_set_start_time_current(session->xccdf.result);
	xccdf_policy_add_result(xccdf_policy, session->xccdf.result);
	return 0;
}

int xccdf_session_add_report_from_source(struct xccdf_session *session, struct oscap_source *report_source)
{
	struct xccdf_result *result = xccdf_result_import_source(report_source);
	if (result == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not find TestResult element.");
		return 1;
	}
	session->xccdf.result_source = report_source;
	session->xccdf.result = result;
	return 0;
}
