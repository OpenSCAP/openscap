#ifndef OVAL_PROBE_META_H
#define OVAL_PROBE_META_H

#include <stdint.h>
#include "public/oval_types.h"
#include "_oval_probe_handler.h"

typedef struct {
        oval_subtype_t otype;
        const char    *stype;
        oval_probe_handler_t *handler;
        uint32_t       flags;
        const char    *pname;
} oval_probe_meta_t;

extern oval_probe_meta_t OSCAP_GSYM(__probe_meta)[];
extern size_t OSCAP_GSYM(__probe_meta_count);
extern oval_subtypedsc_t OSCAP_GSYM(__s2n_tbl)[];
extern oval_subtypedsc_t OSCAP_GSYM(__n2s_tbl)[];

#define OVAL_PROBEMETA_EXTERNAL 0x00000001 /**< pass the `pext' structure to the handler oval_probe_session_new */

/**
 * Convenience macro for defining a probe handled by the external probe handler.
 */
#define OVAL_PROBE_EXTERNAL(subtype, strtype) {(subtype), (strtype), &oval_probe_ext_handler, OVAL_PROBEMETA_EXTERNAL, "probe_"strtype}

#endif /* OVAL_PROBE_META_H */
