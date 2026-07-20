/*
 * libFuzzer harness for the OVAL routingtable probe's line parsers.
 *
 * The routingtable probe parses the kernel's /proc/net/route and
 * /proc/net/ipv6_route tables: each line is tokenised and the hex-encoded
 * address/flag fields are decoded (process_line_ip4 / process_line_ip6, with
 * hexstring2bin / proc_ip{4,6}_to_string). That input comes straight from the
 * scanned host's procfs, so we fuzz it directly.
 *
 * The line parsers are static, so we #include the probe .c. Its SEAP entry
 * point routingtable_probe_main (and the static collect_item it reaches, which
 * calls the non-exported probe_entobj_cmp) is renamed to a static, unused
 * function so --gc-sections drops it -- we only drive the line parsers, which
 * need nothing beyond the strto_uint*_hex helpers compiled in alongside.
 *
 * Build with -DENABLE_FUZZING=ON -DENABLE_PROBES=ON.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "probe_fuzz_common.h"

#define routingtable_probe_main static __attribute__((unused)) fuzz_rt_probe_main
#include "../src/OVAL/probes/unix/routingtable_probe.c"
#undef routingtable_probe_main

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	PROBE_FUZZ_INIT();

	char *buf = probe_fuzz_cstr(data, size);
	if (buf == NULL)
		return 0;

	/* Feed each line to both the IPv4 and IPv6 parsers. They tokenise the
	 * line in place (strtok_r), so hand each parser its own copy. route_info
	 * holds only fixed-size buffers and static-string flag pointers, so
	 * nothing it fills needs to be freed. */
	char *save = NULL;
	for (const char *line = strtok_r(buf, "\n", &save); line != NULL;
	     line = strtok_r(NULL, "\n", &save)) {
		struct route_info rt;

		char *l4 = strdup(line);
		if (l4 != NULL) {
			memset(&rt, 0, sizeof(rt));
			process_line_ip4(l4, &rt);
			free(l4);
		}
		char *l6 = strdup(line);
		if (l6 != NULL) {
			memset(&rt, 0, sizeof(rt));
			process_line_ip6(l6, &rt);
			free(l6);
		}
	}

	free(buf);
	return 0;
}
