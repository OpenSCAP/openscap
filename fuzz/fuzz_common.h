/*
 * Shared setup for the OpenSCAP fuzz harnesses.
 */
#ifndef OPENSCAP_FUZZ_COMMON_H
#define OPENSCAP_FUZZ_COMMON_H

#include <libxml/parser.h>
#include <libxml/xmlerror.h>

#include "oscap.h"

/*
 * One-time process initialization. Silences libxml2's error reporting (it would
 * otherwise print a parse error to stderr for every malformed input, which both
 * slows fuzzing down and buries real sanitizer reports) and initializes the
 * library. Call from the top of LLVMFuzzerTestOneInput guarded by a static flag.
 */
static inline void fuzz_init_once(void)
{
	xmlSetGenericErrorFunc(NULL, NULL);
	xmlSetStructuredErrorFunc(NULL, NULL);
	oscap_init();
}

#define FUZZ_INIT()                          \
	do {                                 \
		static int _fuzz_inited = 0; \
		if (!_fuzz_inited) {         \
			fuzz_init_once();    \
			_fuzz_inited = 1;    \
		}                            \
	} while (0)

#endif /* OPENSCAP_FUZZ_COMMON_H */
