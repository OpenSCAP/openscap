/*
 * libFuzzer harness for XCCDF tailoring parsing (src/XCCDF/tailoring.c).
 *
 * A tailoring file customizes an existing benchmark (overriding profiles,
 * selecting/deselecting rules, refining values). It is parsed *against* a
 * benchmark, so this harness imports one small fixed benchmark at startup and
 * then feeds every fuzzer input to xccdf_tailoring_import_source() as the
 * tailoring document. This reaches the tailoring parser and its profile /
 * selector / value-refinement handling, which plain benchmark parsing skips.
 */

#include <stddef.h>
#include <stdint.h>

#include "fuzz_common.h"
#include "oscap_source.h"
#include "xccdf_benchmark.h"

// A minimal but valid XCCDF 1.2 benchmark with a profile, a value and a rule so
// that tailoring documents have something to extend / select / refine.
static const char BASE_BENCHMARK[] =
	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	"<Benchmark xmlns=\"http://checklists.nist.gov/xccdf/1.2\""
	" id=\"xccdf_org.fuzz_benchmark_base\">\n"
	"  <status>draft</status>\n"
	"  <version>1.0</version>\n"
	"  <Profile id=\"xccdf_org.fuzz_profile_base\">\n"
	"    <title>base profile</title>\n"
	"    <select idref=\"xccdf_org.fuzz_rule_1\" selected=\"true\"/>\n"
	"  </Profile>\n"
	"  <Value id=\"xccdf_org.fuzz_value_1\" type=\"number\">\n"
	"    <title>v1</title>\n"
	"    <value>1</value>\n"
	"  </Value>\n"
	"  <Rule id=\"xccdf_org.fuzz_rule_1\" selected=\"false\">\n"
	"    <title>r1</title>\n"
	"  </Rule>\n"
	"</Benchmark>\n";

// Imported once and reused for every input. Never freed (process-lifetime).
static struct xccdf_benchmark *g_benchmark = NULL;

static struct xccdf_benchmark *get_base_benchmark(void)
{
	if (g_benchmark == NULL) {
		struct oscap_source *bsrc = oscap_source_new_from_memory(
			BASE_BENCHMARK, sizeof(BASE_BENCHMARK) - 1, "fuzz-base.xml");
		if (bsrc != NULL) {
			g_benchmark = xccdf_benchmark_import_source(bsrc);
			oscap_source_free(bsrc);
		}
	}
	return g_benchmark;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	FUZZ_INIT();

	struct xccdf_benchmark *benchmark = get_base_benchmark();
	if (benchmark == NULL) {
		return 0;
	}

	struct oscap_source *source =
		oscap_source_new_from_memory((const char *)data, size, "fuzz-tailoring.xml");
	if (source == NULL) {
		return 0;
	}

	// Tailoring does not take ownership of the benchmark, so we keep the shared
	// one alive and only free the parsed tailoring.
	struct xccdf_tailoring *tailoring = xccdf_tailoring_import_source(source, benchmark);
	if (tailoring != NULL) {
		xccdf_tailoring_free(tailoring);
	}

	oscap_source_free(source);
	return 0;
}
