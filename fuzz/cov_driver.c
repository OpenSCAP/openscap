/*
 * Corpus-replay driver for coverage measurement.
 *
 * Designed to be invoked once per corpus file via xargs -P so crashes in
 * one input don't abort the whole run.  Each invocation runs exactly one
 * input, writes its own profraw, and exits.  Merge everything afterwards:
 *
 *   ls fuzz/corpus | xargs -P8 -I{} sh -c \
 *     "LLVM_PROFILE_FILE=build-cov/profiles/{}.profraw \
 *      ASAN_OPTIONS=detect_leaks=0 \
 *      build-cov/scap_parse_cov fuzz/corpus/{} 2>/dev/null || true"
 *   find build-cov/profiles -name '*.profraw' -print0 \
 *     | xargs -0 llvm-profdata merge -sparse -o combined.profdata
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size);

int main(int argc, char **argv)
{
	if (argc < 2)
		return 0;

	FILE *f = fopen(argv[1], "rb");
	if (!f)
		return 1;
	fseek(f, 0, SEEK_END);
	long sz = ftell(f);
	rewind(f);
	if (sz <= 0) {
		fclose(f);
		return 0;
	}
	unsigned char *buf = malloc((size_t)sz);
	if (!buf) {
		fclose(f);
		return 1;
	}
	fread(buf, 1, (size_t)sz, f);
	fclose(f);
	LLVMFuzzerTestOneInput(buf, (size_t)sz);
	free(buf);
	return 0;
}
