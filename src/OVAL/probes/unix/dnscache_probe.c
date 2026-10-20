
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include "probe-api.h"
#include "common/debug_priv.h"
#include "dnscache_probe.h"

int dnscache_probe_main(probe_ctx *ctx, void *unused)
{
        (void)unused;

	probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_NOT_APPLICABLE);

	return (0);
}
