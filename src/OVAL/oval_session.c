/*
 * Copyright 2015 Red Hat Inc., Durham, North Carolina.
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
 * Author:
 * 		Michal Šrubař <msrubar@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libgen.h>
#include <unistd.h>
#include <string.h>
#if defined(__linux__)
#include <linux/limits.h>
#endif

#include "common/alloc.h"
#include "common/debug_priv.h"
#include "common/util.h"
#include "common/_error.h"
#include "common/oscapxml.h"
#include "source/xslt_priv.h"
#include "public/oval_agent_api.h"
#include "public/oval_session.h"
#include "../DS/public/ds_sds_session.h"
#include "oscap_source.h"

static const char *oscap_productname = "cpe:/a:open-scap:oscap";
static const char *oval_results_report = "oval-results-report.xsl";

struct oval_session {
	/* Main source assigned with the main file (SDS or OVAL) */
	struct oscap_source *source;
	struct oval_definition_model *def_model;
	struct oval_variable_model *var_model;
	struct oval_results_model *res_model;

	oval_agent_session_t *sess;
	struct ds_sds_session *sds_session;

	struct {
		struct oscap_source *definitions;
		struct oscap_source *variables;
		struct oscap_source *directives;
	} oval;

	char *datastream_id;
	/* particular OVAL component if there are two OVALs in one datastream */
	char *component_id;

	struct {
		char *results;
		char *report;
	} export;

	struct {
		/* it's called when there is something invalid in input/output files */
		xml_reporter xml_fn;
	} reporter;

	bool validation;
	bool export_sys_chars;
	bool full_validation;
	bool fetch_remote_resources;
	download_progress_calllback_t progress;
};

struct oval_session *oval_session_new(const char *filename)
{
	oscap_document_type_t scap_type;
	struct oval_session *session;

	session = (struct oval_session *) calloc(1, sizeof(struct oval_session));

	session->source = oscap_source_new_from_file(filename);
	if ((scap_type = oscap_source_get_scap_type(session->source)) == OSCAP_DOCUMENT_UNKNOWN) {
		oval_session_free(session);
		return NULL;
	}

	if (scap_type != OSCAP_DOCUMENT_OVAL_DEFINITIONS && scap_type != OSCAP_DOCUMENT_SDS) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Session input file was determined but it"
				" isn't an OVAL file nor a source datastream file.");
		oval_session_free(session);
		return NULL;
	}

	session->export_sys_chars = true;

	dI("Created a new OVAL session from input file '%s'.", filename);
	return session;
}

void oval_session_set_variables(struct oval_session *session, const char *filename)
{
	__attribute__nonnull__(session);

	oscap_source_free(session->oval.variables);

	if (filename != NULL)
		session->oval.variables = oscap_source_new_from_file(filename);
	else
		session->oval.variables = NULL;	/* reset */
}

void oval_session_set_directives(struct oval_session *session, const char *filename)
{
	__attribute__nonnull__(session);

	oscap_source_free(session->oval.directives);

	if (filename != NULL)
		session->oval.directives = oscap_source_new_from_file(filename);
	else
		session->oval.directives = NULL;
}

void oval_session_set_validation(struct oval_session *session, bool validate, bool full_validation)
{
	__attribute__nonnull__(session);

	session->validation = validate;
	session->full_validation = full_validation;
}

void oval_session_set_datastream_id(struct oval_session *session, const char *id)
{
	__attribute__nonnull__(session);

	free(session->datastream_id);
	session->datastream_id = oscap_strdup(id);
}

void oval_session_set_component_id(struct oval_session *session, const char *id)
{
	__attribute__nonnull__(session);

	free(session->component_id);
	session->component_id = oscap_strdup(id);
}

void oval_session_set_results_export(struct oval_session *session, const char *filename)
{
	__attribute__nonnull__(session);

	free(session->export.results);
	session->export.results = oscap_strdup(filename);
}

void oval_session_set_report_export(struct oval_session *session, const char *filename)
{
	__attribute__nonnull__(session);

	free(session->export.report);
	session->export.report = oscap_strdup(filename);
}

void oval_session_set_xml_reporter(struct oval_session *session, xml_reporter fn)
{
	__attribute__nonnull__(session);

	session->reporter.xml_fn = fn;
}

static bool oval_session_validate(struct oval_session *session, struct oscap_source *source, oscap_document_type_t type)
{
	if (oscap_source_get_scap_type(source) == type) {
		if (oscap_source_validate(source, session->reporter.xml_fn, NULL))
			return false;
	}
	else {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Type mismatch: %s. Expecting %s "
			"but found %s.", oscap_source_readable_origin(source),
			oscap_document_type_to_string(type),
			oscap_document_type_to_string(oscap_source_get_scap_type(source)));
		return false;
	}

	return true;
}

static int oval_session_load_definitions(struct oval_session *session)
{
	__attribute__nonnull__(session);
	__attribute__nonnull__(session->source);

	oscap_document_type_t type = oscap_source_get_scap_type(session->source);
	if (type != OSCAP_DOCUMENT_OVAL_DEFINITIONS && type != OSCAP_DOCUMENT_SDS) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Type mismatch: %s. Expecting %s "
			"or %s but found %s.", oscap_source_readable_origin(session->source),
			oscap_document_type_to_string(OSCAP_DOCUMENT_OVAL_DEFINITIONS),
			oscap_document_type_to_string(OSCAP_DOCUMENT_SDS),
			oscap_document_type_to_string(type));
		return 1;
	}
	else {
		if (session->validation && !oval_session_validate(session, session->source, type))
			return 1;
	}

	if (oscap_source_get_scap_type(session->source) == OSCAP_DOCUMENT_SDS) {
		if ((session->sds_session = ds_sds_session_new_from_source(session->source)) == NULL) {
			return 1;
		}
		ds_sds_session_set_remote_resources(session->sds_session,session->fetch_remote_resources ,session->progress);
		ds_sds_session_set_datastream_id(session->sds_session, session->datastream_id);
		if (ds_sds_session_register_component_with_dependencies(session->sds_session,
					"checks", session->component_id, "oval.xml") != 0) {
			return 1;
		}

		session->oval.definitions = ds_sds_session_get_component_by_href(session->sds_session, "oval.xml");
		if (session->oval.definitions == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "Internal error: OVAL file was not found in "
					"Source DataStream session cache!");
			return 1;
		}
	}
	else {
		session->oval.definitions = session->source;
	}

	/* import OVAL Definitions */
	if (session->def_model) oval_definition_model_free(session->def_model);
	session->def_model = oval_definition_model_import_source(session->oval.definitions);
	if (session->def_model == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Failed to import the OVAL Definitions from '%s'.",
				oscap_source_readable_origin(session->oval.definitions));
		return 1;
	}

	return 0;
}

static int oval_session_load_variables(struct oval_session *session)
{
	__attribute__nonnull__(session);

	if (session->oval.variables != NULL) {

		if (session->def_model == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "No OVAL Definitions to bind the variables to.");
			return 1;
		}
		if (session->validation && !oval_session_validate(session, session->oval.variables, OSCAP_DOCUMENT_OVAL_VARIABLES)) {
			return 1;
		}

		/* bind external variables */
		if (session->oval.variables) {
			session->var_model = oval_variable_model_import_source(session->oval.variables);

			if (session->var_model == NULL) {
				oscap_seterr(OSCAP_EFAMILY_OVAL, "Failed to import the OVAL Variables "
						"from '%s'.", session->oval.variables);
				return 1;
			}

			if (oval_definition_model_bind_variable_model(session->def_model, session->var_model)) {
				oscap_seterr(OSCAP_EFAMILY_OVAL, "Failed to bind Variables to Definitions.");
				return 1;
			}
		}
		dI("Loaded OVAL variables.");
	} else {
		dI("No external OVAL variables provided.");
	}

	return 0;
}

int oval_session_load(struct oval_session *session)
{
	__attribute__nonnull__(session);

	int ret = 0;

	if ((ret = oval_session_load_definitions(session)) != 0) {
		return ret;
	}
	if ((ret = oval_session_load_variables(session)) != 0) {
		return ret;
	}

	return ret;
}

static int oval_session_setup_agent(struct oval_session *session)
{
	__attribute__nonnull__(session);

	char *path_clone;

	path_clone = oscap_strdup(oscap_source_readable_origin(session->oval.definitions));
	if (path_clone == NULL) {
		return 1;
	}

	/* free the previous Agent session if this function was already called */
	if (session->sess)
		oval_agent_destroy_session(session->sess);

	session->sess = oval_agent_new_session(session->def_model, basename(path_clone));
	if (session->sess == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Failed to create a new agent session.");
		free(path_clone);
		return 1;
	}
	free(path_clone);

	oval_agent_set_product_name(session->sess, (char *)oscap_productname);
	return 0;
}

int oval_session_evaluate_id(struct oval_session *session, char *probe_root, const char *id, oval_result_t *result)
{
	__attribute__nonnull__(session);

	if (probe_root) {
		if (setenv("OSCAP_PROBE_ROOT", probe_root, 1) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to set the OSCAP_PROBE_ROOT environment variable.");
			return 1;
		}
	}

	if (id == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "No OVAL Definion id set.");
		return 1;
	}

	if (oval_session_setup_agent(session) != 0) {
		return 1;
	}

	oval_agent_eval_definition(session->sess, id);
	*result = OVAL_RESULT_NOT_EVALUATED;
	oval_agent_get_definition_result(session->sess, id, result);
	if (oscap_err()) {
		return 1;
	}

	session->res_model = oval_agent_get_results_model(session->sess);

	return 0;
}

int oval_session_evaluate(struct oval_session *session, char *probe_root, agent_reporter fn, void *arg)
{
	__attribute__nonnull__(session);

	if (probe_root) {
		if (setenv("OSCAP_PROBE_ROOT", probe_root, 1) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to set the OSCAP_PROBE_ROOT environment variable.");
			return 1;
		}
		dI("OSCAP_PROBE_ROOT environment variable set to '%s'.", probe_root);
	}

	if (oval_session_setup_agent(session) != 0) {
		return 1;
	}

	oval_agent_eval_system(session->sess, fn, arg);
	if (oscap_err()) {
		return 1;
	}

	session->res_model = oval_agent_get_results_model(session->sess);

	dI("OVAL evaluation successfully finished.");
	return 0;
}

int oval_session_export(struct oval_session *session)
{
	__attribute__nonnull__(session);

	struct oval_directives_model *dir_model = NULL;
	struct oscap_source *result = NULL;		/* OVAL Results */
	const char *filename = NULL;
	int ret = 0;

	/* Import OVAL Directives if any */
	if (session->oval.directives && session->res_model) {
		/* OVAL Directives can be used only if there are OVAL Resutls and these are
		 * only available if there is a Results model which mean that either
		 * evaluation or analyse was performed. */
		dir_model = oval_directives_model_new();
		if (oval_directives_model_import_source(dir_model, session->oval.directives) != 0)
			goto cleanup;
	}

	/* Get OVAL Results if evaluation or analyse has been done and apply
	 * directives to them */
	if (session->res_model && (session->export.results || session->export.report)) {
		oval_results_model_set_export_system_characteristics(session->res_model, session->export_sys_chars);
		result = oval_results_model_export_source(session->res_model, dir_model, NULL);
		filename = session->export.results;
	}

	/* Validate OVAL Results. The 'result' in condition will make sure that there is
	 * something to validate */
	if (result && session->validation && session->full_validation) {
		if (!oval_session_validate(session, result, OSCAP_DOCUMENT_OVAL_RESULTS))
			goto cleanup;
	}

	if (session->export.results && result) {	/* export to XML */
		if (oscap_source_save_as(result, filename) != 0)
			goto cleanup;
	}

	if (session->export.report && result) {	/* export to HTML */
		char pwd[PATH_MAX];

		if (getcwd(pwd, sizeof(pwd)) == NULL) {
			oscap_seterr(OSCAP_EFAMILY_GLIBC, "ERROR: %s", strerror(errno));
			goto cleanup;
		}

		/* add params oscap-version & pwd */
		const char *stdparams[] = { "oscap-version", oscap_get_version(), "pwd", pwd, NULL };

		/* TODO: let the user set the xsl by oval_session_set_report_xsl? */
		if (oscap_source_apply_xslt_path(result, oval_results_report,
				session->export.report, stdparams, oscap_path_to_xslt()) == -1) {
			goto cleanup;
		}
	}

	ret = 0; 	/* successfull export */

cleanup:
	if (result)
		oscap_source_free(result);
	if (dir_model)
		oval_directives_model_free(dir_model);
	return ret;
}

void oval_session_set_export_system_characteristics(struct oval_session *session, bool export)
{
	session->export_sys_chars = export;
}

void oval_session_set_remote_resources(struct oval_session *session, bool allowed, download_progress_calllback_t callback)
{
	session->fetch_remote_resources = allowed;
	session->progress = callback;
}

void oval_session_free(struct oval_session *session)
{
	if (session == NULL)
		return;

	oscap_source_free(session->oval.directives);
	oscap_source_free(session->oval.variables);
	oscap_source_free(session->source);
	free(session->datastream_id);
	free(session->component_id);
	free(session->export.results);
	free(session->export.report);
	if (session->sess)
		oval_agent_destroy_session(session->sess);
	if (session->def_model)
		oval_definition_model_free(session->def_model);
	ds_sds_session_free(session->sds_session);
	free(session);
}
