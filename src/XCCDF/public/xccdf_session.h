/**
 * @addtogroup XCCDF
 * @{
 * @addtogroup Session
 * @{
 *
 * @file xccdf_session.h
 * High Level API for OpenSCAP XCCDF operations.
 * @author Simon Lukasik <slukasik@redhat.com>
 */

/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 */

#ifndef XCCDF_SESSION_H_
#define XCCDF_SESSION_H_

#include "xccdf_policy.h"
#include "oscap_download_cb.h"
#include "oscap_export.h"

/**
 * @struct xccdf_session
 * A structure encapsulating the context of XCCDF operations.
 */
struct xccdf_session;

/**
 * Loading flags for XCCDF session
 * @memberof xccdf_session
 * The flags set which components will be loaded in XCCDF session.
 */
typedef enum {
	XCCDF_SESSION_LOAD_NONE = 0,
	XCCDF_SESSION_LOAD_XCCDF = 1 << 0,
	XCCDF_SESSION_LOAD_CPE = 1 << 1,
	XCCDF_SESSION_LOAD_OVAL = 1 << 2,
	XCCDF_SESSION_LOAD_CHECK_ENGINE_PLUGINS = 1 << 3,
	XCCDF_SESSION_LOAD_ALL = XCCDF_SESSION_LOAD_XCCDF | XCCDF_SESSION_LOAD_CPE | XCCDF_SESSION_LOAD_OVAL | XCCDF_SESSION_LOAD_CHECK_ENGINE_PLUGINS
} xccdf_session_loading_flags_t;

/**
 * Costructor of xccdf_session. It attempts to recognize type of the filename.
 * @memberof xccdf_session
 * @param filename path to XCCDF or DS file.
 * @returns newly created \ref xccdf_session.
 * @retval NULL is returned in case of error. Details might be found through \ref oscap_err_desc()
 */
OSCAP_API struct xccdf_session *xccdf_session_new(const char *filename);

/**
 * Costructor of xccdf_session. It creates a new xccdf_session from an oscap_source structure.
 * @memberof xccdf_session
 * @param source oscap_source which can represent a DS or XCCDF file.
 * @returns newly created \ref xccdf_session.
 * @retval NULL is returned in case of error. Details might be found through \ref oscap_err_desc()
 */
OSCAP_API struct xccdf_session *xccdf_session_new_from_source(struct oscap_source *source);

/**
 * Destructor of xccdf_session.
 * @memberof xccdf_session
 * @param session to destroy.
 */
OSCAP_API void xccdf_session_free(struct xccdf_session *session);

/**
 * Reset xccdf_session results.
 * This function resets XCCDF policies, session rules, skipped rules, OVAL system characteristics and OVAL results.
 * @memberof xccdf_session
 * @param session to reset results from.
 */
OSCAP_API void xccdf_session_result_reset(struct xccdf_session *session);

/**
 * Retrieves the filename the session was created with
 * @memberof xccdf_session
 */
OSCAP_API const char *xccdf_session_get_filename(const struct xccdf_session *session);

/**
 * Query if the session is based on Source DataStream.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns true if the session is based on Source Datastream
 */
OSCAP_API bool xccdf_session_is_sds(const struct xccdf_session *session);

/**
 * Set rule for session - if rule is not NULL, session will use only this
 * one rule.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param rule If not NULL, session will use only this rule
 */
OSCAP_DEPRECATED(OSCAP_API void xccdf_session_set_rule(struct xccdf_session *session, const char *rule));

/**
 * Add specific rule for session - if at least one rule is added to the session,
 * only the added rules will be evaluated
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param rule rule ID
 */
OSCAP_API void xccdf_session_add_rule(struct xccdf_session *session, const char *rule);

/**
 * Skip rule during evaluation of the session
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param rule rule ID
 */
OSCAP_API void xccdf_session_skip_rule(struct xccdf_session *session, const char *rule);

/**
 * Set XSD validation level to one of three possibilities:
 *	- None: 	All XSD validations will be skipped.
 *	- Default:	Partial (input) XSD validations will be done.
 *	- Full Valid.:	Every possible (input & output) XSD validation will be done.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param validate False value indicates to skip any XSD validation.
 * @param full_validation True value indicates that every possible step will be validated by XSD.
 */
OSCAP_API void xccdf_session_set_validation(struct xccdf_session *session, bool validate, bool full_validation);

/**
 * Set XML signature validation
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param validate False value indicates to skip any XML signature validation.
 */
OSCAP_API void xccdf_session_set_signature_validation(struct xccdf_session *session, bool validate);

/**
 * Set XML signature enforcement
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param enforce True value renders all unsigned XMLs invalid.
 */
OSCAP_API void xccdf_session_set_signature_enforcement(struct xccdf_session *session, bool enforce);

/**
 * Set whether the thin results override is enabled.
 * If true the OVAL results put in ARF or separate files will have thin results.
 * Thin results do not contain details about the evaluated criteria, only
 * minimal OVAL results.
 * @memberof xccdf_session
 * @param thin_results true to enable thin_results, default is false
 */
OSCAP_API void xccdf_session_set_thin_results(struct xccdf_session *session, bool thin_result);

/**
 * Set requested datastream_id for this session. This datastream_id is later
 * passed down to @ref ds_sds_index_select_checklist to determine target component.
 * This function is applicable only for sessions based on a DataStream.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param datastream_id requested datastream_id for this session.
 */
OSCAP_API void xccdf_session_set_datastream_id(struct xccdf_session *session, const char *datastream_id);

/**
 * Retrieves the datastream id
 * @see xccdf_session_set_datastream_id
 * @memberof xccdf_session
 */
OSCAP_API const char *xccdf_session_get_datastream_id(struct xccdf_session *session);

/**
 * Set requested component_id for this session. This component_id is later
 * pased down to @ref ds_sds_index_select_checklist to determine target component.
 * This function is applicable only for sessions based on a DataStream.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param component_id requested component_id for this session.
 */
OSCAP_API void xccdf_session_set_component_id(struct xccdf_session *session, const char *component_id);

/**
 * Retrieves the component id
 * @see xccdf_session_set_component_id
 * @memberof xccdf_session
 */
OSCAP_API const char *xccdf_session_get_component_id(struct xccdf_session *session);

/**
 * Sets requested benchmark_id for this session. It is only used when no component_id
 * and no datastream_id are requested. Benchmark ID selects the @id attribute of Benchmark
 * element inside a component that is referenced with a checklist component-ref.
 * @memberof xccdf_session
 */
OSCAP_API void xccdf_session_set_benchmark_id(struct xccdf_session *session, const char *benchmark_id);

/**
 * Retrieves the benchmark_id
 * @see xccdf_session_set_benchmark_id
 * @memberof xccdf_session
 */
OSCAP_API const char *xccdf_session_get_benchmark_id(struct xccdf_session *session);

/**
 * Retrieves the result id
 * @memberof xccdf_session
 */
OSCAP_API const char *xccdf_session_get_result_id(struct xccdf_session *session);

/**
 * Set path to custom CPE dictionary for the session. This function is applicable
 * only before session loads. It has no effect if run afterwards.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param user_cpe File path to user defined cpe dictionary.
 */
OSCAP_API void xccdf_session_set_user_cpe(struct xccdf_session *session, const char *user_cpe);

/**
 * Set path to custom Tailoring file for the session. This function is applicable
 * only before session loads. It has no effect if run afterwards.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param user_tailoring_file File path to user defined tailoring file.
 */
OSCAP_API void xccdf_session_set_user_tailoring_file(struct xccdf_session *session, const char *user_tailoring_file);

OSCAP_API struct oscap_source *xccdf_session_get_user_tailoring_file(struct xccdf_session *session);

/**
 * Set ID of Tailoring component for the session. This function is applicable
 * only before session loads. It has no effect if run afterwards.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param user_tailoring_cid ID of component with a tailoring file.
 */
OSCAP_API void xccdf_session_set_user_tailoring_cid(struct xccdf_session *session, const char *user_tailoring_cid);

/**
 * Set properties of remote content.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param allowed Whether is download od remote resources allowed in this session (defaults to false)
 * @param callback used to notify user about download proceeds. This might be safely set
 * to NULL -- ignoring user notification.
 */
OSCAP_API OSCAP_DEPRECATED(void xccdf_session_set_remote_resources(struct xccdf_session *session, bool allowed, download_progress_calllback_t callback));

/**
 * Set properties of remote content.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param allowed Whether is download od remote resources allowed in this
 * session (defaults to false)
 * @param local_files Allows to use a locally downloaded copy of the remote
 * resources. Contains a path to a directory where the files are stored
 * (defaults to NULL).
 * @param callback used to notify user about download proceeds. This might be
 * safely set to NULL -- ignoring user notification.
 */
OSCAP_API void xccdf_session_configure_remote_resources(struct xccdf_session *session, bool allowed, const char *local_files, download_progress_calllback_t callback);

/**
 * Disable or allow loading of depending content (OVAL, SCE, CPE)
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param flags Bit mask that sets loading of other content in the session.
 */
OSCAP_API void xccdf_session_set_loading_flags(struct xccdf_session *session, xccdf_session_loading_flags_t flags);

/**
 * Set custom oval files for this session
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param oval_filenames - Array of paths to custom OVAL files. If the array is empty
 * no OVAL file will be used for the session. If this parameter is NULL then OVAL
 * files will be find automatically, as defined in XCCDF (which is default).
 */
OSCAP_API void xccdf_session_set_custom_oval_files(struct xccdf_session *session, char **oval_filenames);

/**
 * Set custom OVAL eval function to register with each OVAL session. This function shall
 * be called before OVAL files are parsed.
 * @memberof xccdf_session
 * @param session XCCDF Session.
 * @param eval_fn Callback - pointer to function called by XCCDF Policy for each evaluated rule.
 */
OSCAP_API void xccdf_session_set_custom_oval_eval_fn(struct xccdf_session *session, xccdf_policy_engine_eval_fn eval_fn);

/**
 * Set custom product CPE name.
 * @memberof xccdf_session
 * @param session XCCDF Session.
 * @param product_cpe Name of the scanner product.
 * @returns true on success
 */
OSCAP_API bool xccdf_session_set_product_cpe(struct xccdf_session *session, const char *product_cpe);

/**
 * Set whether the System Characteristics shall be exported in result files.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param without_sys_chars whether to export System Characteristics or not.
 */
OSCAP_API void xccdf_session_set_without_sys_chars_export(struct xccdf_session *session, bool without_sys_chars);

/**
 * Set whether the OVAL result files shall be exported.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param to_export_oval_results whether to export results or not.
 */
OSCAP_API void xccdf_session_set_oval_results_export(struct xccdf_session *session, bool to_export_oval_results);

/**
 * Set that check engine plugin's result files shall be exported.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param to_export_results whether to export results from check engine plugins or not.
 */
OSCAP_API void xccdf_session_set_check_engine_plugins_results_export(struct xccdf_session *session, bool to_export_results);

/**
 * Set whether the OVAL variables files shall be exported.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param to_export_oval_variables whether to export results or not.
 */
OSCAP_API void xccdf_session_set_oval_variables_export(struct xccdf_session *session, bool to_export_oval_variables);

/**
 * Set where to export XCCDF file. NULL value means to not export at all.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param xccdf_file path to XCCDF file
 * @returns true on success
 */
OSCAP_API bool xccdf_session_set_xccdf_export(struct xccdf_session *session, const char *xccdf_file);

/**
 * Set where to export STIG Viewer XCCDF file. NULL value means to not export at all.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param xccdf_file path to STIG Viewer file
 * @returns true on success
 */
OSCAP_API bool xccdf_session_set_xccdf_stig_viewer_export(struct xccdf_session *session, const char *xccdf_stig_viewer_file);

/**
 * Set where to export ARF file. NULL value means to not export at all.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param arf_file path to ARF file
 * @returns true on success
 */
OSCAP_API bool xccdf_session_set_arf_export(struct xccdf_session *session, const char *arf_file);

/**
 * Set where to export HTML Report file. NULL value means to not export at all.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param report_file
 * @returns true on success
 */
OSCAP_API bool xccdf_session_set_report_export(struct xccdf_session *session, const char *report_file);

/**
 * Select XCCDF Profile for evaluation.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param profile_id ID of profile to set
 * @returns true on success
 */
OSCAP_API bool xccdf_session_set_profile_id(struct xccdf_session *session, const char *profile_id);

/**
 *Select XCCDF Profile for evaluation with only profile suffix as input. Reports error
 *if multiple profiles match the suffix.
 *@memberof xccdf_session
 *@param session XCCDF Session
 *@param profile_suffix unique profile ID or suffix of the ID of the profile to set
 *@returns 0 on success, 1 if profile is not found, and 2 if multiple matches are found.
 */
OSCAP_API int xccdf_session_set_profile_id_by_suffix(struct xccdf_session *session, const char *profile_suffix);

/**
 * Retrieves ID of the profile that we will evaluate with, or NULL.
 * @memberof xccdf_session
 * @param session XCCDF Session
 */
OSCAP_API const char *xccdf_session_get_profile_id(struct xccdf_session *session);

/**
 * Get Source DataStream index of the session.
 * @memberof xccdf_session
 * @warning This is applicable only on sessions which are SDS. NULL is returned
 * otherwise.
 * @return sds index
 */
OSCAP_API struct ds_sds_index *xccdf_session_get_sds_idx(struct xccdf_session *session);

/**
 * Load and parse all XCCDF structures needed to evaluate this session. This is
 * only a placeholder for load_xccdf, load_cpe, load_oval and load_check_engine_plugins functions.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_load(struct xccdf_session *session);

/**
 * Load and parse XCCDF file. If the file upon which is based this session is
 * Source DataStream use functions @ref xccdf_session_set_datastream_id and
 * @ref xccdf_session_set_component_id to select particular component within
 * that DataStream to parse. This function is reentrant meaning that it allows
 * user to change i.e. component_id and load_xccdf again in the very same session.
 * However in such case, previous xccdf structures will be deallocated from session
 * and pointers to it become invalid.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_load_xccdf(struct xccdf_session *session);

/**
 * Load and parse CPE dictionaries. Function xccdf_session_set_user_cpe
 * might be called before this to set custom CPE dictionary.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_load_cpe(struct xccdf_session *session);

/**
 * Load and parse OVAL definitions files for the XCCDF session.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_load_oval(struct xccdf_session *session);

/**
 * Load extra check engine from a plugin of given name to the XCCDF session.
 *
 * Extra check engines are in loadable shared objects. This function is
 * designed to be called !after! xccdf_session_load has been called.
 * XCCDF has to have already been loaded for this to work because the callbacks
 * are registered as part of this function!
 *
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param plugin_name Name of the plugin to load
 * @param quiet If true we will not output errors if loading fails
 * @returns zero on success
 */
OSCAP_API int xccdf_session_load_check_engine_plugin2(struct xccdf_session *session, const char* plugin_name, bool quiet);
OSCAP_API int xccdf_session_load_check_engine_plugin(struct xccdf_session *session, const char* plugin_name);

/**
 * Load extra check engines (if any are available) to the XCCDF session.
 *
 * Extra check engines are in loadable shared objects and this function
 * searches if any such are available and loads them if they are.
 *
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_load_check_engine_plugins(struct xccdf_session *session);

/**
 * Load Tailoring file (if applicable) to the XCCDF session.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_load_tailoring(struct xccdf_session *session);

/**
 * Evaluate XCCDF Policy.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_evaluate(struct xccdf_session *session);

/**
 * Export XCCDF file.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_export_xccdf(struct xccdf_session *session);

/**
 * Export OVAL (result and variables) files.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_export_oval(struct xccdf_session *session);

/**
 * Export results (if any) from any check engine plugins that are loaded
 *
 * Only applicable if enabled by @ref xccdf_session_set_check_engine_plugins_results_export).
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_export_check_engine_plugins(struct xccdf_session *session);

/**
 * Export ARF (if enabled by @ref xccdf_session_set_arf_export).
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_export_arf(struct xccdf_session *session);

/**
 * Get policy_model of the session. The @ref xccdf_session_load_xccdf shall be run
 * before this to parse XCCDF file to the policy_model.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns XCCDF Policy Model or NULL in case of failure.
 */
OSCAP_API struct xccdf_policy_model *xccdf_session_get_policy_model(const struct xccdf_session *session);

/**
 * Get xccdf_policy of the session.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns XCCDF Policy or NULL in case of failure.
 */
OSCAP_API struct xccdf_policy *xccdf_session_get_xccdf_policy(const struct xccdf_session *session);

/**
 * Get the base score of the latest XCCDF evaluation in the session.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns the score
 */
OSCAP_API float xccdf_session_get_base_score(const struct xccdf_session *session);

/**
 * Get count of OVAL agent sessions not used for CPE in the xccdf_session.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns number of OVAL agents.
 */
OSCAP_API unsigned int xccdf_session_get_oval_agents_count(const struct xccdf_session *session);

/**
 * Get count of OVAL agent sessions for CPE in the xccdf_session.
 * CPE uses OVAL on demand, we do not count OVAL agents that were referenced
 * but weren't used.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns number of OVAL agents for CPE.
 */
OSCAP_API unsigned int xccdf_session_get_cpe_oval_agents_count(const struct xccdf_session *session);

/**
 * Query if the result of evaluation contains FAIL, ERROR, or UNKNOWN rule-result elements.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns Exists such rule-result r . r = FAIL | r = UNKNOWN | r = ERROR
 */
OSCAP_API bool xccdf_session_contains_fail_result(const struct xccdf_session *session);

/**
 * @struct xccdf_rule_result_iterator
 */
struct xccdf_rule_result_iterator;

/**
 * Get rule results.
 * @memberof xccdf_session
 * @param session XCCDF Session
 */
OSCAP_API struct xccdf_rule_result_iterator *xccdf_session_get_rule_results(const struct xccdf_session *session);

/**
 * Run XCCDF Remediation. It uses XCCDF Policy and XCCDF TestResult from the session
 * and modifies the TestResult. This also drops and recreate OVAL Agent Session, thus
 * users are advised to run @ref xccdf_session_export_oval first.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_remediate(struct xccdf_session *session);

/**
 * Load xccdf:TestResult to the session from file and prepare session for remediation.
 * This function assumes that the session internals has the policy_model prepared,
 * it calculates all the other internals which might be needed for remediation.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param testresult_id ID of the TestResult element in the file (the NULL value stands
 * for the last TestResult). Suffix match is attempted if exact match is not found.
 * @returns zero on success.
 */
OSCAP_API int xccdf_session_build_policy_from_testresult(struct xccdf_session *session, const char *testresult_id);

/**
 * Load xccdf:TestResult to the session from oscap_source
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param report_source Structure conataining oscap_source of the test results
 * @returns zero on success.
 */
OSCAP_API int xccdf_session_add_report_from_source(struct xccdf_session *session, struct oscap_source *report_source);

/**
 * Generate HTML guide form a loaded XCCDF session
 * @param session XCCDF Session
 * @param outfile path to the output file
 * @returns zero on success
 */
OSCAP_API int xccdf_session_generate_guide(struct xccdf_session *session, const char *outfile);

/**
 * Export XCCDF results, ARF results and HTML report from the given XCCDF
 * session based on values set in the XCCDF session. This is a destructive
 * operation that modifies the oscap_source structures, specifically the XML
 * trees. Callers must not perform any operation with the session after this
 * call and they must free the session immediately.
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_API int xccdf_session_export_all(struct xccdf_session *session);

/**
 * Set reference filter to the XCCDF session. If this filter is set,
 * the XCCDF session will evaluate only rules that conform to the filter.
 * @param session XCCDF session
 * @param reference_filter a string in a form "key:identifier"
 */
OSCAP_API void xccdf_session_set_reference_filter(struct xccdf_session *session, const char *reference_filter);
/// @}
/// @}
#endif
