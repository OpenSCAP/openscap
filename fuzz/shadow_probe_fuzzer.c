/*
 * libFuzzer harness for the OVAL shadow probe's password-hash parser.
 *
 * parse_enc_mth() inspects an /etc/shadow password field and classifies the
 * hashing method from its prefix ($1$, $5$, $6$, _, ...). The shadow field is
 * read off the scanned host, so we fuzz that classification directly.
 *
 * parse_enc_mth() is static, so we #include the probe .c. Its SEAP entry points
 * (shadow_probe_main / shadow_probe_offline_mode_supported) and the static
 * helpers only they reach are renamed to static, unused functions so that
 * --gc-sections drops them; parse_enc_mth itself only uses exported SEXP API.
 *
 * Build with -DENABLE_FUZZING=ON -DENABLE_PROBES=ON.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "probe_fuzz_common.h"

#define shadow_probe_main                    static __attribute__((unused)) fuzz_shadow_probe_main
#define shadow_probe_offline_mode_supported  static __attribute__((unused)) fuzz_shadow_offline
#include "../src/OVAL/probes/unix/shadow_probe.c"
#undef shadow_probe_main
#undef shadow_probe_offline_mode_supported

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	PROBE_FUZZ_INIT();

	char *pwd = probe_fuzz_cstr(data, size);
	if (pwd == NULL)
		return 0;

	SEXP_t *enc_mth = parse_enc_mth(pwd);
	SEXP_free(enc_mth);

	free(pwd);
	return 0;
}
