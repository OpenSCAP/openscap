/*
 * libFuzzer harness for the OVAL inetlisteningservers probe's /proc/net parsers.
 *
 * read_tcp/read_udp/read_raw read /proc/net/{tcp,tcp6,udp,udp6,raw,raw6} and
 * sscanf() each line into fields, then hex-decode the addresses (addr_convert).
 * That procfs content comes from the scanned host, so we fuzz the line parsing
 * directly.
 *
 * These functions are static, so we #include the probe .c. Because this harness
 * is linked with the library's object files (add_fuzzer_probe_full), the .c's
 * non-static SEAP entry points would clash with the copies in
 * linux_probes_object; we rename them to harmless static, unused functions.
 *
 * read_*() take a path and open it, so the fuzz bytes are written to a tmpfs
 * temp file. We skip collect_process_info() (it walks the real /proc and would
 * make the run non-deterministic) and drive the parsers with an empty process
 * list -- the line parsing/decoding we care about runs regardless.
 *
 * Build with -DENABLE_FUZZING=ON -DENABLE_PROBES=ON.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "probe_fuzz_common.h"

/* struct probe_ctx, probe_icache_t / probe_icache_*, OSCAP_PROBE_* defaults --
 * inetlisteningservers_probe.c itself only pulls in the public probe-api.h. */
#include "probe/probe.h"

#define inetlisteningservers_probe_main                   fuzz_ils_probe_main
#define inetlisteningservers_probe_offline_mode_supported fuzz_ils_offline_mode_supported
#include "../src/OVAL/probes/unix/linux/inetlisteningservers_probe.c"
#undef inetlisteningservers_probe_main
#undef inetlisteningservers_probe_offline_mode_supported

static struct server_info g_req;
static int g_ready;

static void ils_setup(void)
{
	/* Build the three entities read_*() consult (protocol / local_address /
	 * local_port). Their exact values only affect the match filter, not the
	 * line parsing we are exercising. */
	SEXP_t *vproto = SEXP_string_newf("tcp");
	SEXP_t *vaddr  = SEXP_string_newf("0.0.0.0");
	SEXP_t *vport  = SEXP_number_newi_32(0);
	SEXP_t *obj = probe_obj_creat("inetlisteningservers_object", NULL,
	                              "protocol",      NULL, vproto,
	                              "local_address", NULL, vaddr,
	                              "local_port",    NULL, vport,
	                              NULL);
	g_req.protocol_ent      = probe_obj_getent(obj, "protocol", 1);
	g_req.local_address_ent = probe_obj_getent(obj, "local_address", 1);
	g_req.local_port_ent    = probe_obj_getent(obj, "local_port", 1);
	SEXP_free(vproto);
	SEXP_free(vaddr);
	SEXP_free(vport);
	SEXP_free(obj);

	g_ready = (g_req.protocol_ent != NULL &&
	           g_req.local_address_ent != NULL && g_req.local_port_ent != NULL);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	PROBE_FUZZ_INIT();
	if (!g_ready) {
		ils_setup();
		if (!g_ready)
			return 0;
	}

	const char *path = probe_fuzz_write_tmpfile(data, size);
	if (path == NULL)
		return 0;

	/* No item cache (ctx.icache == NULL): probe_item_collect() frees the item
	 * and returns -1, so we exercise the full parse without the icache worker
	 * thread (see textfilecontent54_probe_fuzzer.c). */
	probe_ctx ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.probe_out = probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, NULL, NULL, NULL);
	ctx.icache = NULL;
	ctx.max_mem_ratio = OSCAP_PROBE_MEMORY_USAGE_RATIO_DEFAULT;
	ctx.max_collected_items = OSCAP_PROBE_COLLECT_UNLIMITED;

	/* Empty process list: we skip collect_process_info()'s real /proc walk;
	 * the inode lookups simply miss and report_finding() runs without a pid. */
	llist ll;
	list_create(&ll);

	/* Feed the same fuzz file through each parser (their sscanf formats differ). */
	read_tcp(path, "tcp", &ll, &ctx, &g_req);
	read_udp(path, "udp", &ll, &ctx, &g_req);
	read_raw(path, "udp", &ll, &ctx, &g_req);

	list_clear(&ll);
	SEXP_free(ctx.probe_out);
	return 0;
}
