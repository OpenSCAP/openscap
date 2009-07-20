#pragma once
#ifndef _SEAP_TYPES_H
#define _SEAP_TYPES_H

#include <stdint.h>
#include "public/seap-types.h"
#include "_sexp-types.h"
#include "_seap-command.h"
#include "seap-descriptor.h"

/* SEAP context */
struct SEAP_CTX {
        SEXP_t * (*parser) (SEXP_psetup_t *, const char *, size_t, SEXP_pstate_t **);
        SEXP_pflags_t pflags;
        SEXP_format_t fmt_in;
        SEXP_format_t fmt_out;
        SEAP_desctable_t  sd_table;
        SEAP_cmdtbl_t *cmd_c_table;
};

typedef struct {
        char   *name;
        SEXP_t *value;
} SEAP_attr_t;

struct SEAP_msg {
        SEAP_msgid_t id;
        SEAP_attr_t *attrs;
        uint16_t     attrs_cnt;
        SEXP_t      *sexp;
};

struct SEAP_err {
        SEAP_msgid_t id;
        uint32_t     code;
        SEXP_t      *data;
};

#define SEAP_SYM_PREFIX "seap."
#define SEAP_SYM_MSG    SEAP_SYM_PREFIX"msg"
#define SEAP_SYM_CMD    SEAP_SYM_PREFIX"cmd"
#define SEAP_SYM_ERR    SEAP_SYM_PREFIX"err"

struct SEAP_packet {
        uint8_t type;
        union {
                SEAP_msg_t msg;
                SEAP_err_t err;
                SEAP_cmd_t cmd;
        } data;
};

#define SEAP_PACKET_INV 0x00 /* Invalid packet */
#define SEAP_PACKET_MSG 0x01 /* Message packet */
#define SEAP_PACKET_ERR 0x02 /* Error packet */
#define SEAP_PACKET_CMD 0x03 /* Command packet */
#define SEAP_PACKET_RAW 0x04 /* Raw packet */

#endif /* _SEAP_TYPES_H */
