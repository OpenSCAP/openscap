/*
 * libFuzzer harness for OpenSCAP SCAP file parsing.
 *
 * This harness feeds arbitrary bytes into the OpenSCAP parsing pipeline the
 * same way an application would when it loads a SCAP file from disk:
 *
 *   1. wrap the bytes in an oscap_source (the library's single entry point for
 *      "here is a SCAP document"),
 *   2. let the library sniff the document type, and
 *   3. dispatch to the matching importer (data stream, XCCDF, OVAL, CPE, ...).
 *
 * Every importer ultimately drives the XML reader and the type-specific
 * deserialization code, which is where parser crashes / segfaults live.
 *
 * Build it with the project's ENABLE_FUZZING CMake option (see fuzz/README.md),
 * which compiles the whole library with the libFuzzer + AddressSanitizer
 * instrumentation and links this file with -fsanitize=fuzzer.
 */

#include <stddef.h>
#include <stdint.h>

#include "fuzz_common.h"
#include "oscap_source.h"
#include "scap_ds.h"
#include "ds_sds_session.h"
#include "ds_rds_session.h"
#include "xccdf_benchmark.h"
#include "cpe_dict.h"
#include "cpe_lang.h"
#include "oval_definitions.h"
#include "oval_variables.h"
#include "oval_system_characteristics.h"
#include "oval_results.h"
#include "oval_directives.h"

/* Exercise the source data stream / result data stream code paths. */
static void fuzz_datastream(struct oscap_source *source)
{
	struct ds_sds_session *session = ds_sds_session_new_from_source(source);
	if (session != NULL) {
		/* NULL ids -> let the session guess; this walks the index,
		 * the catalogue and extracts/parses the selected components. */
		ds_sds_session_select_checklist(session, NULL, NULL, NULL);
		ds_sds_session_free(session);
	}
}

static void fuzz_arf(struct oscap_source *source)
{
	struct ds_rds_session *session = ds_rds_session_new_from_source(source);
	if (session != NULL) {
		ds_rds_session_get_rds_idx(session);
		ds_rds_session_free(session);
	}
}

static void fuzz_xccdf(struct oscap_source *source)
{
	struct xccdf_benchmark *benchmark = xccdf_benchmark_import_source(source);
	if (benchmark != NULL) {
		xccdf_benchmark_free(benchmark);
	}
}

static void fuzz_oval_definitions(struct oscap_source *source)
{
	struct oval_definition_model *model = oval_definition_model_import_source(source);
	if (model != NULL) {
		oval_definition_model_free(model);
	}
}

static void fuzz_oval_variables(struct oscap_source *source)
{
	struct oval_variable_model *model = oval_variable_model_import_source(source);
	if (model != NULL) {
		oval_variable_model_free(model);
	}
}

static void fuzz_oval_syschar(struct oscap_source *source)
{
	struct oval_definition_model *defs = oval_definition_model_new();
	struct oval_syschar_model *model = oval_syschar_model_new(defs);
	if (model != NULL) {
		oval_syschar_model_import_source(model, source);
		oval_syschar_model_free(model);
	}
	oval_definition_model_free(defs);
}

static void fuzz_oval_results(struct oscap_source *source)
{
	struct oval_definition_model *defs = oval_definition_model_new();
	struct oval_results_model *model = oval_results_model_new(defs, NULL);
	if (model != NULL) {
		oval_results_model_import_source(model, source);
		oval_results_model_free(model);
	}
	oval_definition_model_free(defs);
}

static void fuzz_oval_directives(struct oscap_source *source)
{
	struct oval_directives_model *model = oval_directives_model_new();
	if (model != NULL) {
		oval_directives_model_import_source(model, source);
		oval_directives_model_free(model);
	}
}

static void fuzz_cpe_dict(struct oscap_source *source)
{
	struct cpe_dict_model *model = cpe_dict_model_import_source(source);
	if (model != NULL) {
		cpe_dict_model_free(model);
	}
}

static void fuzz_cpe_lang(struct oscap_source *source)
{
	struct cpe_lang_model *model = cpe_lang_model_import_source(source);
	if (model != NULL) {
		cpe_lang_model_free(model);
	}
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	FUZZ_INIT();

	/* oscap_source copies the buffer, so the const input is never mutated. */
	struct oscap_source *source =
		oscap_source_new_from_memory((const char *)data, size, "fuzz.xml");
	if (source == NULL) {
		return 0;
	}

	/* Determining the type already parses the document far enough to read the
	 * root element and namespaces, so this alone exercises a lot of code. */
	oscap_document_type_t type = oscap_source_get_scap_type(source);

	switch (type) {
	case OSCAP_DOCUMENT_SDS:
		fuzz_datastream(source);
		break;
	case OSCAP_DOCUMENT_ARF:
		fuzz_arf(source);
		break;
	case OSCAP_DOCUMENT_XCCDF:
	case OSCAP_DOCUMENT_XCCDF_TAILORING:
		fuzz_xccdf(source);
		break;
	case OSCAP_DOCUMENT_OVAL_DEFINITIONS:
		fuzz_oval_definitions(source);
		break;
	case OSCAP_DOCUMENT_OVAL_VARIABLES:
		fuzz_oval_variables(source);
		break;
	case OSCAP_DOCUMENT_OVAL_SYSCHAR:
		fuzz_oval_syschar(source);
		break;
	case OSCAP_DOCUMENT_OVAL_RESULTS:
		fuzz_oval_results(source);
		break;
	case OSCAP_DOCUMENT_OVAL_DIRECTIVES:
		fuzz_oval_directives(source);
		break;
	case OSCAP_DOCUMENT_CPE_DICTIONARY:
		fuzz_cpe_dict(source);
		break;
	case OSCAP_DOCUMENT_CPE_LANGUAGE:
		fuzz_cpe_lang(source);
		break;
	case OSCAP_DOCUMENT_UNKNOWN:
	default:
		/* Unknown type: still try the data stream and XCCDF importers, which
		 * are the most complex parsers and do their own validation. This keeps
		 * coverage high even when type detection bails out early. */
		fuzz_datastream(source);
		break;
	}

	oscap_source_free(source);
	return 0;
}
