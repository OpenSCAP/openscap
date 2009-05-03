#pragma once
#ifndef SEAP_TYPES_H
#define SEAP_TYPES_H

#include <stdint.h>
#include "sexp.h"

typedef uint8_t SEAP_scheme_t;

typedef struct {
        uint64_t      next_id;
        SEXP_t       *sexpbuf; /* S-exp buffer */
        SEXP_ostate_t *ostate;
        SEXP_pstate_t *pstate;  /* Parser state */
        SEAP_scheme_t  scheme;  /* Scheme used for this descriptor */
        void          *scheme_data; /* Scheme related data */
} SEAP_desc_t;

#define DESC_FDIN  0x00000001
#define DESC_FDOUT 0x00000002

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

/* SEAP context */
typedef struct __SEAP_CTX_t {
        SEXP_t * (*parser) (struct __SEAP_CTX_t *, const char *, size_t, SEXP_pstate_t **);
        SEXP_pflags_t pflags;
        SEXP_format_t fmt_in;
        SEXP_format_t fmt_out;
        SEAP_desctable_t sd_table;
} SEAP_CTX_t;

#define SEAP_CTX_INITIALIZER { NULL, 0, 0, 0, SEAP_DESCTBL_INITIALIZER }

typedef struct {
        char   *name;
        SEXP_t *value;
} SEAP_attr_t;

typedef struct {
        uint64_t     id;
        SEAP_attr_t *attrs;
        uint16_t     attrs_cnt;
        SEXP_t      *sexp;
} SEAP_msg_t;

/* SEAP errors */
#define SERR_REM_UNFIN 1 /* peer received an incomplete expression */
#define SERR_LOC_UNFIN 2 /* peer sent an incomplete expression */
#define SERR_REM_PARSE 3 /* peer received an invalid expression */
#define SERR_LOC_PARSE 4 /* peer sent an invalid expression */
#define SERR_REM_CLOSE 5 /* peer closed the connection */

#endif /* SEAP_TYPES_H */
