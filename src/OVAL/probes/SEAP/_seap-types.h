#ifndef __STUB_PROBE
#pragma once
#ifndef _SEAP_TYPES_H
#define _SEAP_TYPES_H

#include <stdint.h>
#include "public/seap-types.h"
#include "_sexp-types.h"
#include "_seap-command.h"
#include "seap-descriptor.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

/* SEAP context */
struct SEAP_CTX {
        SEXP_t * (*parser) (SEXP_psetup_t *, const char *, size_t, SEXP_pstate_t **);
        SEXP_pflags_t pflags;
        SEXP_format_t fmt_in;
        SEXP_format_t fmt_out;
        SEAP_desctable_t  sd_table;
        SEAP_cmdtbl_t *cmd_c_table;
        SEAP_cflags_t  cflags;

        uint16_t recv_timeout;
        uint16_t send_timeout;
};

OSCAP_HIDDEN_END;

#endif /* _SEAP_TYPES_H */
#endif
