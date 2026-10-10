/*
 * libFuzzer harness for the OVAL iflisteners probe's /proc/net/packet parser.
 *
 * read_packet() reads /proc/net/packet and sscanf()s each line (packet-socket
 * table from net/packet/af_packet.c). That procfs content comes off the
 * scanned host, so we fuzz the line parsing directly.
 *
 * read_packet() hard-coded the "/proc/net/packet" path; it was refactored to
 * take the path as a parameter (mirroring read_tcp() in the inetlisteningservers
 * probe), so the harness can point it at a tmpfs file holding the fuzz bytes.
 *
 * read_packet() is static, so we #include the probe .c and build with
 * add_fuzzer_probe_full() (it reaches non-exported helpers: probe_entobj_cmp,
 * the item cache, oscap_enum_to_string). The non-static iflisteners_probe_main
 * is renamed to avoid clashing with the copy in linux_probes_object.
 *
 * We skip collect_process_info() (it walks the real /proc and would make the
 * run non-deterministic) and pass an empty inode list. The per-line sscanf
 * parsing runs regardless of the list; the inode-matched enrichment path
 * (get_interface/report_finding) is simply not taken.
 *
 * Build with -DENABLE_FUZZING=ON -DENABLE_PROBES=ON.
 */

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "probe_fuzz_common.h"
#include "probe/probe.h"

#define iflisteners_probe_main fuzz_ifl_probe_main
#include "../src/OVAL/probes/unix/linux/iflisteners_probe.c"
#undef iflisteners_probe_main

static SEXP_t *g_ifname_ent;
static int g_ready;

static void ifl_setup(void)
{
	SEXP_t *val = SEXP_string_newf("any");
	SEXP_t *obj = probe_obj_creat("iflisteners_object", NULL,
	                              "interface_name", NULL, val,
	                              NULL);
	g_ifname_ent = probe_obj_getent(obj, "interface_name", 1);
	SEXP_free(val);
	SEXP_free(obj);
	g_ready = (g_ifname_ent != NULL);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	PROBE_FUZZ_INIT();
	if (!g_ready) {
		ifl_setup();
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

	llist ll;
	list_create(&ll);

	oval_schema_version_t over = OVAL_SCHEMA_VERSION(5.11);
	read_packet(path, &ll, &ctx, over, g_ifname_ent);

	list_clear(&ll);
	SEXP_free(ctx.probe_out);
	return 0;
}
