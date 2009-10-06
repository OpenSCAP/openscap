#pragma once
#ifndef OVAL_PROBE_H
#define OVAL_PROBE_H

#include <seap-types.h>
#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"
#include "api/oval_system_characteristics.h"
#include "config.h"
#include "../common/util.h"

OSCAP_HIDDEN_START;

#define OVAL_PROBE_SCHEME "pipe://"
#ifndef OVAL_PROBE_DIR
# define OVAL_PROBE_DIR    "/usr/libexec/openscap/oval"
#endif

typedef struct {
        oval_subtype_t typenum;
        char             *typestr;
        char             *filename;
} oval_probe_t;

typedef struct {
        oval_subtype_t typenum;
        int sd;
} probe_sd_t;

typedef struct {
        probe_sd_t *memb;
        size_t      count;
        SEAP_CTX_t *ctx;
        uint8_t     flags;
} probe_sdtbl_t;

#define PROBE_SDTBL_CMDDONE 0x01

#define PROBE_SDTBL_INITIALIZER { NULL, 0, NULL, 0 }

SEXP_t *oval_object_to_sexp (const char *typestr, struct oval_object *object);
struct oval_syschar *sexp_to_oval_state (SEXP_t *sexp, struct oval_object *object);

int oval_syschar_apply_sexp(struct oval_syschar *syschar, SEXP_t *sexp, struct oval_object *object);

OSCAP_HIDDEN_END;

#endif /* OVAL_PROBE_H */
