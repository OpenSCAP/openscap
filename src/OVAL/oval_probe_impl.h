#pragma once
#ifndef OVAL_PROBE_IMPL_H
#define OVAL_PROBE_IMPL_H

#include <seap-types.h>
#include <config.h>

#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"
#include "public/oval_system_characteristics.h"
#include "../common/util.h"

#include "public/oval_probe.h"

OSCAP_HIDDEN_START;

#define OVAL_PROBE_SCHEME "pipe"
#ifndef OVAL_PROBE_DIR
# define OVAL_PROBE_DIR    "/usr/libexec/openscap"
#endif

typedef struct {
        oval_subtype_t typenum;
        char          *typestr;
        char          *filename;
} oval_probe_t;

typedef struct {
        oval_subtype_t typenum;
        int   sd;
        char *uri;
} ovalp_sd_t;

typedef struct {
        ovalp_sd_t *memb;
        size_t      count;
        SEAP_CTX_t *ctx;
        uint8_t     flags;
} ovalp_sdtbl_t;

#define OVALP_SDTBL_CMDDONE 0x01
#define OVALP_SDTBL_INITIALIZER { NULL, 0, NULL, 0 }

#define OVAL_PROBE_MAXRETRY 3

const oval_probe_t *ovalp_lookup (oval_subtype_t typenum);
oval_subtype_t      ovalp_lookup_type (const char *name);

struct oval_pctx {
        oval_probe_t  *p_table;
        ovalp_sdtbl_t *s_table;
        char          *p_dir;
        struct oval_definition_model *model;
};

OSCAP_HIDDEN_END;

#endif /* OVAL_PROBE_IMPL_H */
