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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef XCCDF_SESSION_H_
#define XCCDF_SESSION_H_

#include "xccdf_policy.h"
#include "oscap_download_cb.h"

/**
 * @struct xccdf_session
 * A structure encapsulating the context of XCCDF operations.
 */
struct xccdf_session;

/**
 * Costructor of xccdf_session. It attempts to recognize type of the filename.
 * @memberof xccdf_session
 * @param filename path to XCCDF or DS file.
 * @returns newly created \ref xccdf_session.
 * @retval NULL is returned in case of error. Details might be found through \ref oscap_err_desc()
 */
struct xccdf_session *xccdf_session_new(const char *filename);

/**
 * Destructor of xccdf_session.
 * @memberof xccdf_session
 * @param session to destroy.
 */
void xccdf_session_free(struct xccdf_session *session);

/**
 * Retrieves the filename the session was created with
 * @memberof xccdf_session
 */
const char *xccdf_session_get_filename(const struct xccdf_session *session);

/**
 * Query if the session is based on Source DataStream.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns true if the session is based on Source Datastream
 */
bool xccdf_session_is_sds(const struct xccdf_session *session);

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
void xccdf_session_set_validation(struct xccdf_session *session, bool validate, bool full_validation);

/**
 * Set whether the thin results override is enabled.
 * If true the OVAL results put in ARF or separate files will have thin results.
 * Thin results do not contain details about the evaluated criteria, only
 * minimal OVAL results.
 * @memberof xccdf_session
 * @param thin_results true to enable thin_results, default is false
 */
void xccdf_session_set_thin_results(struct xccdf_session *session, bool thin_result);

/**
 * Set requested datastream_id for this session. This datastream_id is later
 * passed down to @ref ds_sds_index_select_checklist to determine target component.
 * This function is applicable only for sessions based on a DataStream.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param datastream_id requested datastream_id for this session.
 */
void xccdf_session_set_datastream_id(struct xccdf_session *session, const char *datastream_id);

/**
 * Retrieves the datastream id
 * @see xccdf_session_set_datastream_id
 * @memberof xccdf_session
 */
const char *xccdf_session_get_datastream_id(struct xccdf_session *session);

/**
 * Set requested component_id for this session. This component_id is later
 * pased down to @ref ds_sds_index_select_checklist to determine target component.
 * This function is applicable only for sessions based on a DataStream.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param component_id requested component_id for this session.
 */
void xccdf_session_set_component_id(struct xccdf_session *session, const char *component_id);

/**
 * Retrieves the component id
 * @see xccdf_session_set_component_id
 * @memberof xccdf_session
 */
const char *xccdf_session_get_component_id(struct xccdf_session *session);

/**
 * Sets requested benchmark_id for this session. It is only used when no component_id
 * and no datastream_id are requested. Benchmark ID selects the @id attribute of Benchmark
 * element inside a component that is referenced with a checklist component-ref.
 * @memberof xccdf_session
 */
void xccdf_session_set_benchmark_id(struct xccdf_session *session, const char *benchmark_id);

/**
 * Retrieves the benchmark_id
 * @see xccdf_session_set_benchmark_id
 * @memberof xccdf_session
 */
const char *xccdf_session_get_benchmark_id(struct xccdf_session *session);

/**
 * Set path to custom CPE dictionary for the session. This function is applicable
 * only before session loads. It has no effect if run afterwards.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param user_cpe File path to user defined cpe dictionary.
 */
void xccdf_session_set_user_cpe(struct xccdf_session *session, const char *user_cpe);

/**
 * Set path to custom Tailoring file for the session. This function is applicable
 * only before session loads. It has no effect if run afterwards.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param user_tailoring_file File path to user defined tailoring file.
 */
void xccdf_session_set_user_tailoring_file(struct xccdf_session *session, const char *user_tailoring_file);

/**
 * Set ID of Tailoring component for the session. This function is applicable
 * only before session loads. It has no effect if run afterwards.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param user_tailoring_cid ID of component with a tailoring file.
 */
void xccdf_session_set_user_tailoring_cid(struct xccdf_session *session, const char *user_tailoring_cid);

/**
 * Set properties of remote content.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param allowed Whether is download od remote resources allowed in this session (defaults to false)
 * @param callback used to notify user about download proceeds. This might be safely set
 * to NULL -- ignoring user notification.
 */
void xccdf_session_set_remote_resources(struct xccdf_session *session, bool allowed, download_progress_calllback_t callback);

/**
 * Set custom oval files for this session
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param oval_filenames - Array of paths to custom OVAL files. If the array is empty
 * no OVAL file will be used for the session. If this parameter is NULL then OVAL
 * files will be find automatically, as defined in XCCDF (which is default).
 */
void xccdf_session_set_custom_oval_files(struct xccdf_session *session, char **oval_filenames);

/**
 * Set custom OVAL eval function to register with each OVAL session. This function shall
 * be called before OVAL files are parsed.
 * @memberof xccdf_session
 * @param session XCCDF Session.
 * @param eval_fn Callback - pointer to function called by XCCDF Policy for each evaluated rule.
 */
void xccdf_session_set_custom_oval_eval_fn(struct xccdf_session *session, xccdf_policy_engine_eval_fn eval_fn);

/**
 * Set custom product CPE name.
 * @memberof xccdf_session
 * @param session XCCDF Session.
 * @param product_cpe Name of the scanner product.
 * @returns true on success
 */
bool xccdf_session_set_product_cpe(struct xccdf_session *session, const char *product_cpe);

/**
 * Set whether the System Characteristics shall be exported in result files.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param without_sys_chars whether to export System Characteristics or not.
 */
void xccdf_session_set_without_sys_chars_export(struct xccdf_session *session, bool without_sys_chars);

/**
 * Set whether the OVAL result files shall be exported.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param to_export_oval_results whether to export results or not.
 */
void xccdf_session_set_oval_results_export(struct xccdf_session *session, bool to_export_oval_results);

/**
 * Set that check engine plugin's result files shall be exported.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param to_export_results whether to export results from check engine plugins or not.
 */
void xccdf_session_set_check_engine_plugins_results_export(struct xccdf_session *session, bool to_export_results);

/**
 * Set that SCE reult files shall be exported.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param to_export_sce_results whether to export SCE results or not.
 */
OSCAP_DEPRECATED(void xccdf_session_set_sce_results_export(struct xccdf_session *session, bool to_export_sce_results));

/**
 * Set whether the OVAL variables files shall be exported.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param to_export_oval_variables whether to export results or not.
 */
void xccdf_session_set_oval_variables_export(struct xccdf_session *session, bool to_export_oval_variables);

/**
 * Set where to export XCCDF file. NULL value means to not export at all.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param xccdf_file path to XCCDF file
 * @returns true on success
 */
bool xccdf_session_set_xccdf_export(struct xccdf_session *session, const char *xccdf_file);

/**
 * Set where to export ARF file. NULL value means to not export at all.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param arf_file path to ARF file
 * @returns true on success
 */
bool xccdf_session_set_arf_export(struct xccdf_session *session, const char *arf_file);

/**
 * Set where to export HTML Report file. NULL value means to not export at all.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param report_file
 * @returns true on success
 */
bool xccdf_session_set_report_export(struct xccdf_session *session, const char *report_file);

/**
 * Select XCCDF Profile for evaluation.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param profile_id ID of profile to set
 * @returns true on success
 */
bool xccdf_session_set_profile_id(struct xccdf_session *session, const char *profile_id);

/**
 * Retrieves ID of the profile that we will evaluate with, or NULL.
 * @memberof xccdf_session
 * @param session XCCDF Session
 */
const char *xccdf_session_get_profile_id(struct xccdf_session *session);

/**
 * Get Source DataStream index of the session.
 * @memberof xccdf_session
 * @warning This is applicable only on sessions which are SDS. NULL is returned
 * otherwise.
 * @return sds index
 */
struct ds_sds_index *xccdf_session_get_sds_idx(struct xccdf_session *session);

/**
 * Load and parse all XCCDF structures needed to evaluate this session. This is
 * only a placeholder for load_xccdf, load_cpe, load_oval and load_check_engine_plugins functions.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
int xccdf_session_load(struct xccdf_session *session);

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
int xccdf_session_load_xccdf(struct xccdf_session *session);

/**
 * Load and parse CPE dictionaries. Function xccdf_session_set_user_cpe
 * might be called before this to set custom CPE dictionary.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
int xccdf_session_load_cpe(struct xccdf_session *session);

/**
 * Load and parse OVAL definitions files for the XCCDF session.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
int xccdf_session_load_oval(struct xccdf_session *session);

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
 * @returns zero on success
 */
int xccdf_session_load_check_engine_plugin(struct xccdf_session *session, const char* plugin_name);

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
int xccdf_session_load_check_engine_plugins(struct xccdf_session *session);

/**
 * @deprecated
 * SCE is no longer part of the main openscap library,
 * use xccdf_session_load_check_engine_plugins instead.
 */
OSCAP_DEPRECATED(int xccdf_session_load_sce(struct xccdf_session *session));

/**
 * Load Tailoring file (if applicable) to the XCCDF session.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
int xccdf_session_load_tailoring(struct xccdf_session *session);

/**
 * Evaluate XCCDF Policy.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
int xccdf_session_evaluate(struct xccdf_session *session);

/**
 * Export XCCDF file.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
int xccdf_session_export_xccdf(struct xccdf_session *session);

/**
 * Export OVAL (result and variables) files.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
int xccdf_session_export_oval(struct xccdf_session *session);

/**
 * Export results (if any) from any check engine plugins that are loaded
 *
 * Only applicable if enabled by @ref xccdf_session_set_check_engine_plugins_results_export).
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
int xccdf_session_export_check_engine_plugins(struct xccdf_session *session);

/**
 * Export SCE files (if enabled by @ref xccdf_session_set_sce_results_export).
 *
 * @deprecated Please use xccdf_session_export_check_engine_plugins instead.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
OSCAP_DEPRECATED(int xccdf_session_export_sce(struct xccdf_session *session));

/**
 * Export ARF (if enabled by @ref xccdf_session_set_arf_export).
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
int xccdf_session_export_arf(struct xccdf_session *session);

/**
 * Get policy_model of the session. The @ref xccdf_session_load_xccdf shall be run
 * before this to parse XCCDF file to the policy_model.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns XCCDF Policy Model or NULL in case of failure.
 */
struct xccdf_policy_model *xccdf_session_get_policy_model(const struct xccdf_session *session);

/**
 * Get xccdf_policy of the session.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns XCCDF Policy or NULL in case of failure.
 */
struct xccdf_policy *xccdf_session_get_xccdf_policy(const struct xccdf_session *session);

/**
 * Get the base score of the latest XCCDF evaluation in the session.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns the score
 */
float xccdf_session_get_base_score(const struct xccdf_session *session);

/**
 * Get count of OVAL agent sessions not used for CPE in the xccdf_session.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns number of OVAL agents.
 */
unsigned int xccdf_session_get_oval_agents_count(const struct xccdf_session *session);

/**
 * Get count of OVAL agent sessions for CPE in the xccdf_session.
 * CPE uses OVAL on demand, we do not count OVAL agents that were referenced
 * but weren't used.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns number of OVAL agents for CPE.
 */
unsigned int xccdf_session_get_cpe_oval_agents_count(const struct xccdf_session *session);

/**
 * Query if the result of evaluation contains FAIL, ERROR, or UNKNOWN rule-result elements.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns Exists such rule-result r . r = FAIL | r = UNKNOWN | r = ERROR
 */
bool xccdf_session_contains_fail_result(const struct xccdf_session *session);

/**
 * Run XCCDF Remediation. It uses XCCDF Policy and XCCDF TestResult from the session
 * and modifies the TestResult. This also drops and recreate OVAL Agent Session, thus
 * users are advised to run @ref xccdf_session_export_oval first.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @returns zero on success
 */
int xccdf_session_remediate(struct xccdf_session *session);

/**
 * Load xccdf:TestResult to the session from file and prepare session for remediation.
 * This function assumes that the session internals has the policy_model prepared,
 * it calculates all the other internals which might be needed for remediation.
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param testresult_id ID of the TestResult element in the file (the NULL value stands
 * for the last TestResult).
 * @returns zero on success.
 */
int xccdf_session_build_policy_from_testresult(struct xccdf_session *session, const char *testresult_id);

/**
 * Load xccdf:TestResult to the session from oscap_source
 * @memberof xccdf_session
 * @param session XCCDF Session
 * @param report_source Structure conataining oscap_source of the test results
 * @returns zero on success.
 */
int xccdf_session_add_report_from_source(struct xccdf_session *session, struct oscap_source *report_source);

/// @}
/// @}
#endif
