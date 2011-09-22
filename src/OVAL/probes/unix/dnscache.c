#include <string.h>
#include "probe-api.h"
#include "common/debug_priv.h"
#include "common/assume.h"

int probe_main(probe_ctx *ctx, void *unused)
{
        SEXP_t *item;

        (void)unused;

        item = probe_item_create(OVAL_UNIX_DNSCACHE, NULL, NULL);

        probe_item_setstatus(item, SYSCHAR_STATUS_NOT_COLLECTED);
        probe_item_collect(ctx, item);

        return (0);
}
