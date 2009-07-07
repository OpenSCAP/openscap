#pragma once
#ifndef SEAP_TYPES_H
#define SEAP_TYPES_H

#include <stdint.h>
#include "sexp.h"

typedef uint8_t SEAP_scheme_t;
typedef uint64_t msg_id_t;
typedef uint16_t cmd_id_t;
typedef uint16_t cmd_t;

/*
 * Descriptor table + related stuff
 */
typedef struct {
        msg_id_t      next_id;
        SEXP_t       *sexpbuf; /* S-exp buffer */
        SEXP_ostate_t *ostate; /* Output state */
        SEXP_pstate_t *pstate; /* Parser state */
        SEAP_scheme_t  scheme; /* Protocol/Scheme used for this descriptor */
        void          *scheme_data; /* Protocol/Scheme related data */
} SEAP_desc_t;

#define SEAP_DESC_FDIN  0x00000001
#define SEAP_DESC_FDOUT 0x00000002
#define SEAP_DESC_SELF  -1

#include "bitmap.h"

typedef struct {
        SEAP_desc_t *sd;
        uint16_t     sdsize;
        bitmap_t     bitmap;
} SEAP_desctable_t;

#define SEAP_DESCTBL_INITIALIZER { NULL, 0, BITMAP_INITIALIZER }

#define SEAP_BUFFER_SIZE 4096
#define SEAP_MAX_OPENDESC 128
#define SDTABLE_REALLOC_ADD 4

/*
 * SEAP commands
 */

typedef SEXP_t * (*cmd_fn_t) (SEXP_t *, void *);

#include "redblack.h"

DEFRBTREE(command, cmd_t  code; cmd_fn_t func; void *arg);
DEFRBTREE(waiting, cmd_id_t id; cmd_fn_t func; void *arg);

typedef struct {
        cmd_id_t next_id;
        /* pthread_rwlock_t clock; */
        TREETYPE(command) cmds;
        /* pthread_rwlock_t wlock; */
        TREETYPE(waiting) wait;
} SEAP_cmdtable_t;

#define SEAP_CMDTABLE_INITIALIZER { 0, { NULL, 0}, { NULL, 0}}

/* SEAP context */
typedef struct {
        SEXP_t * (*parser) (SEXP_psetup_t *, const char *, size_t, SEXP_pstate_t **);
        SEXP_pflags_t pflags;
        SEXP_format_t fmt_in;
        SEXP_format_t fmt_out;
        SEAP_desctable_t sd_table;
        SEAP_cmdtable_t cmd_table;
} SEAP_CTX_t;

#define SEAP_CTX_INITIALIZER { NULL, 0, 0, 0, SEAP_DESCTBL_INITIALIZER, SEAP_CMDTABLE_INITIALIZER }

typedef struct {
        char   *name;
        SEXP_t *value;
} SEAP_attr_t;

typedef struct {
        msg_id_t     id;
        SEAP_attr_t *attrs;
        uint16_t     attrs_cnt;
        SEXP_t      *sexp;
} SEAP_msg_t;

typedef struct {
        msg_id_t id;
        uint32_t code;
        SEXP_t  *data;
} SEAP_err_t;

typedef uint8_t  cmd_type_t;
typedef uint8_t  cmd_class_t;

#define SEAP_CMDCLASS_INT 1
#define SEAP_CMDCLASS_USR 2

#define SEAP_CMDFLAG_SYNC  0x01
#define SEAP_CMDFLAG_REPLY 0x02
#define SEAP_CMDFLAG_MASK  0xff

typedef struct {
        cmd_id_t       id;
        cmd_id_t      rid;
        uint8_t     flags;
        cmd_class_t class;  /* usr/int */
        cmd_t        code;
        SEXP_t      *args;
} SEAP_cmd_t;

#define SEAP_SYM_PREFIX "seap."
#define SEAP_SYM_MSG    SEAP_SYM_PREFIX"msg"
#define SEAP_SYM_CMD    SEAP_SYM_PREFIX"cmd"
#define SEAP_SYM_ERR    SEAP_SYM_PREFIX"err"

typedef struct {
        uint8_t type;
        union {
                SEAP_msg_t msg;
                SEAP_err_t err;
                SEAP_cmd_t cmd;
        } data;
} SEAP_packet_t;

#define SEAP_PACKET_INV 0x00 /* Invalid packet */
#define SEAP_PACKET_MSG 0x01 /* Message packet */
#define SEAP_PACKET_ERR 0x02 /* Error packet */
#define SEAP_PACKET_CMD 0x03 /* Command packet */
#define SEAP_PACKET_RAW 0x04 /* Raw packet */

/* SEAP errors */
#define SEAP_ERRTYPE_INT  0 /* Internal error */
#define SEAP_ERRTYPE_USER 1 /* User-defined error */

#define SEAP_ERR_UNFIN 1  /* Can't finish parsing */
#define SEAP_ERR_PARSE 2  /* Parsing error */
#define SEAP_ERR_CLOSE 3  /* Connection close */
#define SEAP_ERR_INVAL 4  /* Invalid argument */
#define SEAP_ERR_NOMEM 5  /* Cannot allocate memory */
#define SEAP_ERR_MSEXP 6  /* Missing required S-exp/value */
#define SEAP_ERR_MATTR 7  /* Missing required attribute */
#define SEAP_ERR_UNEXP 8  /* Unexpected error */
#define SEAP_ERR_USER  9  /* User-defined error */
#define SEAP_ERR_NOCMD 10 /* Unknown cmd */
#define SEAP_ERR_QFULL 11 /* Queue full */

/* SEAP I/O flags */
#define SEAP_IOFLG_RECONN   0x00000001 /* Try to reconnect */
#define SEAP_IOFLG_NONBLOCK 0x00000002 /* Non-blocking mode */

#endif /* SEAP_TYPES_H */
