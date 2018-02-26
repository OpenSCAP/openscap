#ifndef OSCAP_PROBE_OPTION_H
#define OSCAP_PROBE_OPTION_H

#define PROBEOPT_VARREF_HANDLING 0
#define PROBEOPT_RESULT_CACHING  1
#define PROBEOPT_OFFLINE_MODE_SUPPORTED 2

#define PROBE_OPTION_SET 0
#define PROBE_OPTION_GET 1

#include <stddef.h>
#include <stdarg.h>
#include "common/util.h"

typedef struct {
  int option;
  int (*handler)(int, int, va_list);
} probe_option_t;

extern size_t OSCAP_GSYM(probe_optdef_count);
extern probe_option_t *OSCAP_GSYM(probe_optdef);

OSCAP_API int probe_setoption(int option, ...);
OSCAP_API int probe_getoption(int option, ...);

#endif /* OSCAP_PROBE_OPTION_H */
