#include <string.h>
#include "probe-api.h"
#include "common/debug_priv.h"
#include "common/assume.h"

int probe_main(SEXP_t *probe_in, SEXP_t *probe_out, void *unused, SEXP_t *filters)
{
        SEXP_t *item;

        item = probe_item_create(OVAL_UNIX_DNSCACHE, NULL, NULL);
        probe_item_setstatus(item, OVAL_STATUS_NOTCOLLECTED);
        probe_cobj_add_item(probe_out, item);
        SEXP_free(item);

        return (0);
}
