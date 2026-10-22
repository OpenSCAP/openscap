/*
 * libFuzzer harness for the OVAL xinetd probe's configuration parser.
 *
 * The xinetd probe contains a ~1700-line hand-written parser for xinetd
 * configuration files (xiconf_parse / xiconf_read / xiconf_parse_section /
 * xiconf_parse_service). On a scanned host this input comes straight off the
 * target's filesystem (/etc/xinetd.conf and the /etc/xinetd.d directory), so it is
 * attacker-influenced and worth fuzzing directly.
 *
 * We reach the static parser exactly the way tests/probes/xinetd/
 * test_probe_xinetd.c does: #include the probe .c (with XINETD_TEST defined so
 * its SEAP-driven probe_main is excluded), then call xiconf_parse() on a file
 * holding the fuzz bytes. No probe_ctx is needed for this path.
 *
 * Build with -DENABLE_FUZZING=ON -DENABLE_PROBES=ON.
 */

#include <stddef.h>
#include <stdint.h>

#include "probe_fuzz_common.h"

#define XINETD_TEST
#include "../src/OVAL/probes/unix/xinetd_probe.c"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	PROBE_FUZZ_INIT();

	const char *path = probe_fuzz_write_tmpfile(data, size);
	if (path == NULL)
		return 0;

	xiconf_t *xcfg = xiconf_parse(path, 32);
	if (xcfg == NULL)
		return 0;

	/* Exercise the lookup path too: resolve a fixed service/protocol. This
	 * walks the service tree the parser built. The returned strans is a
	 * borrowed reference owned by xcfg's service-translation tree (it comes
	 * straight out of rbt_str_get), so it must NOT be freed here -- xiconf_free
	 * below frees it. */
	(void)xiconf_getservice(xcfg, "fuzz", "tcp");

	xiconf_free(xcfg);
	return 0;
}
