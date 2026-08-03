/*
 * libFuzzer harness for the legacy (OVAL 5.3) textfilecontent probe's reader.
 *
 * Like textfilecontent54 but distinct code: this process_file() matches the
 * pattern line-by-line (the _54 variant slurps the whole file), so it is its
 * own parsing surface. We fix the pattern to ".*" and fuzz the file content.
 *
 * process_file() is static and calls non-exported probe helpers (it emits via
 * probe_item_collect), so this harness is built with add_fuzzer_probe_full()
 * (links the library object files). The probe's non-static *_probe_main is
 * renamed to avoid clashing with the copy in independent_probes_object.
 *
 * Build with -DENABLE_FUZZING=ON -DENABLE_PROBES=ON.
 */

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "probe_fuzz_common.h"

#define textfilecontent_probe_main                    fuzz_tfc_probe_main
#define textfilecontent_probe_offline_mode_supported  fuzz_tfc_offline_mode_supported
#include "../src/OVAL/probes/independent/textfilecontent_probe.c"
#undef textfilecontent_probe_main
#undef textfilecontent_probe_offline_mode_supported

#define FUZZ_PATTERN ".*"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	PROBE_FUZZ_INIT();

	const char *path = probe_fuzz_write_tmpfile(data, size);
	if (path == NULL)
		return 0;
	const char *slash = strrchr(path, '/');
	const char *file = slash ? slash + 1 : path;

	/* No item cache (ctx.icache == NULL): probe_item_collect() frees the item
	 * and returns -1, so we exercise the full parse without the icache worker
	 * thread (see textfilecontent54_probe_fuzzer.c). */
	probe_ctx ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.probe_out = probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, NULL, NULL, NULL);
	ctx.icache = NULL;
	ctx.max_mem_ratio = OSCAP_PROBE_MEMORY_USAGE_RATIO_DEFAULT;
	ctx.max_collected_items = OSCAP_PROBE_COLLECT_UNLIMITED;

	char pattern[] = FUZZ_PATTERN;      /* mutable copy; process_file compiles it itself */
	struct pfdata pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.pattern = pattern;
	pfd.filename_ent = NULL;            /* unused by process_file */
	pfd.ctx = &ctx;

	oval_schema_version_t over = OVAL_SCHEMA_VERSION(5.11);
	process_file(NULL, PROBE_FUZZ_TMPDIR, file, &pfd, over, ctx.blocked_paths);

	SEXP_free(ctx.probe_out);
	return 0;
}
