/*
 * libFuzzer harness for SCAP document *validation* (src/source/schematron.c,
 * src/source/xslt.c and the libxml2 XSD validation path).
 *
 * `oscap ds sds-validate` and friends are a very common entry point and a
 * distinct chunk of code from the object-model importers: XSD schema validation
 * and Schematron (implemented as an XSLT transform). Both are driven here.
 *
 * Validation needs the bundled XML schemas. At runtime point the harness at
 * them with the OSCAP_SCHEMA_PATH environment variable, e.g.
 *
 *     OSCAP_SCHEMA_PATH=<srcdir>/schemas ./validate_fuzzer corpus
 *
 * Without it, schema lookups simply fail early (still exercises the dispatch /
 * type-detection and error handling, just not the deep schema code).
 */

#include <stddef.h>
#include <stdint.h>

#include "fuzz_common.h"
#include "oscap_source.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	FUZZ_INIT();

	struct oscap_source *source =
		oscap_source_new_from_memory((const char *)data, size, "fuzz-validate.xml");
	if (source == NULL) {
		return 0;
	}

	// Determining the type selects which schema(s) the validator will use.
	if (oscap_source_get_scap_type(source) != OSCAP_DOCUMENT_UNKNOWN) {
		// XSD validation: parses the document and walks the schema grammar.
		oscap_source_validate(source, NULL, NULL);
		// Schematron validation: compiles and applies the Schematron XSLT.
		oscap_source_validate_schematron(source);
	}

	oscap_source_free(source);
	return 0;
}
