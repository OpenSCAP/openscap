/*
 * libFuzzer harness for the XCCDF *policy* layer (src/XCCDF_POLICY).
 *
 * The base scap_parse_fuzzer only parses an XCCDF benchmark into its object
 * model. This harness goes one step further and drives the policy model, which
 * is the code that resolves profiles, applies selectors, binds values and
 * performs text substitution. None of that is reached by plain parsing, yet it
 * runs purely on parsed content (no OVAL probes / no system access), so it is a
 * good fuzzing target.
 *
 * Pipeline:
 *   xccdf_benchmark_import_source()           parse the benchmark
 *   xccdf_policy_model_new()                  build a policy model (takes
 *                                             ownership of the benchmark)
 *   xccdf_policy_model_build_all_useful_policies()
 *                                             instantiate a policy per profile,
 *                                             resolving selectors & refinements
 *   xccdf_policy_resolve() for each policy    resolve the selected items
 */

#include <stddef.h>
#include <stdint.h>

#include "fuzz_common.h"
#include "oscap_source.h"
#include "xccdf_benchmark.h"
#include "xccdf_policy.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	FUZZ_INIT();

	struct oscap_source *source =
		oscap_source_new_from_memory((const char *)data, size, "fuzz-xccdf.xml");
	if (source == NULL) {
		return 0;
	}

	struct xccdf_benchmark *benchmark = xccdf_benchmark_import_source(source);
	if (benchmark == NULL) {
		oscap_source_free(source);
		return 0;
	}

	// xccdf_policy_model_new takes ownership of the benchmark and frees it in
	// xccdf_policy_model_free, so we must not free the benchmark separately.
	struct xccdf_policy_model *model = xccdf_policy_model_new(benchmark);
	if (model != NULL) {
		xccdf_policy_model_build_all_useful_policies(model);

		struct xccdf_policy_iterator *it = xccdf_policy_model_get_policies(model);
		while (xccdf_policy_iterator_has_more(it)) {
			struct xccdf_policy *policy = xccdf_policy_iterator_next(it);
			xccdf_policy_resolve(policy);
		}
		xccdf_policy_iterator_free(it);

		xccdf_policy_model_free(model);
	} else {
		xccdf_benchmark_free(benchmark);
	}

	oscap_source_free(source);
	return 0;
}
