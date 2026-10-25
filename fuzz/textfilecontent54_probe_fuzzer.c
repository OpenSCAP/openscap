/*
 * libFuzzer harness for the OVAL textfilecontent54 probe's file reader.
 *
 * process_file() calls non-exported probe helpers (probe_entobj_cmp -> the OVAL
 * comparison code, plus the item cache), so this harness is built with
 * add_fuzzer_probe_full() in fuzz/CMakeLists.txt, which links the library's
 * object files directly (those internal symbols are hidden in the .so).
 *
 * textfilecontent54 reads an arbitrary file off the scanned host and runs a
 * PCRE pattern over its contents (process_file(): read the whole file into a
 * grown buffer, then oscap_pcre_get_substrings() in a loop, building an item
 * per match). We fix the pattern to ".*" and fuzz the *file content*, which is
 * the attacker-influenced part on a scanned system.
 *
 * process_file() is static, so we #include the probe .c to reach it. The probe
 * also exposes two non-static symbols that already live in libopenscap.so
 * (textfilecontent54_probe_main / ..._offline_mode_supported); we rename them
 * via macros so this translation unit does not clash with the library at link
 * time.
 *
 * process_file() emits matches through probe_item_collect(), which needs a
 * probe_ctx with a live item cache. We build a minimal one: a real icache (its
 * worker thread clears a barrier initialised for a single participant) plus a
 * fresh collected-object per iteration. The OVAL "instance" entity the reader
 * consults is built once with probe_obj_creat().
 *
 * Build with -DENABLE_FUZZING=ON -DENABLE_PROBES=ON.
 */

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "probe_fuzz_common.h"

#define textfilecontent54_probe_main          fuzz_tfc54_probe_main
#define textfilecontent54_probe_offline_mode_supported fuzz_tfc54_offline_mode_supported
#include "../src/OVAL/probes/independent/textfilecontent54_probe.c"
#undef textfilecontent54_probe_main
#undef textfilecontent54_probe_offline_mode_supported

/* Fixed pattern: match everything, so the regex loop and item-building code
 * run on whatever bytes the fuzzer produced. */
#define FUZZ_PATTERN ".*"

static SEXP_t       *g_instance_ent;
static oscap_pcre_t *g_regex;
static int           g_ready;

static void tfc54_setup(void)
{
	/* Build a fixed "instance" entity (value 1) the same way the probe's
	 * object would carry it, then extract the entity process_file() reads. */
	SEXP_t *inst_val = SEXP_number_newi_32(1);
	SEXP_t *obj = probe_obj_creat("textfilecontent54_object", NULL,
	                              "instance", NULL, inst_val,
	                              NULL);
	g_instance_ent = probe_obj_getent(obj, "instance", 1);
	SEXP_free(inst_val);
	SEXP_free(obj);

	char *err = NULL;
	int erroff = -1;
	g_regex = oscap_pcre_compile(FUZZ_PATTERN, OSCAP_PCRE_OPTS_UTF8, &err, &erroff);
	if (err != NULL)
		oscap_pcre_err_free(err);

	g_ready = (g_instance_ent != NULL && g_regex != NULL);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	PROBE_FUZZ_INIT();
	if (!g_ready) {
		tfc54_setup();
		if (!g_ready)
			return 0;
	}

	const char *path = probe_fuzz_write_tmpfile(data, size);
	if (path == NULL)
		return 0;
	const char *slash = strrchr(path, '/');
	const char *file = slash ? slash + 1 : path;

	/* No item cache: with ctx.icache == NULL, probe_item_collect() frees the
	 * item and returns -1 (the parser handles that), so we exercise the full
	 * parse without the icache worker thread. That thread otherwise dominates
	 * runtime (~1 exec/s) and is unsafe under libFuzzer -fork. probe_out must
	 * stay non-NULL or probe_item_collect() would leak the item on its early
	 * return. */
	probe_ctx ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.probe_in = NULL;
	ctx.probe_out = probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, NULL, NULL, NULL);
	ctx.icache = NULL;
	ctx.filters = NULL;
	ctx.offline_mode = 0;
	ctx.max_mem_ratio = OSCAP_PROBE_MEMORY_USAGE_RATIO_DEFAULT;
	ctx.collected_items = 0;
	ctx.max_collected_items = OSCAP_PROBE_COLLECT_UNLIMITED;
	ctx.blocked_paths = NULL;

	char pattern[] = FUZZ_PATTERN;        /* mutable copy; read-only inside process_file */
	struct pfdata pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.pattern = pattern;
	pfd.re_opts = OSCAP_PCRE_OPTS_UTF8;
	pfd.instance_ent = g_instance_ent;
	pfd.ctx = &ctx;
	pfd.compiled_regex = g_regex;

	oval_schema_version_t over = OVAL_SCHEMA_VERSION(5.11);
	process_file(NULL, PROBE_FUZZ_TMPDIR, file, &pfd, over, ctx.blocked_paths);

	SEXP_free(ctx.probe_out);
	return 0;
}
