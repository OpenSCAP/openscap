#pragma once
#ifndef _SEAP_DESCRIPTOR_H
#define _SEAP_DESCRIPTOR_H

#include <stdint.h>
#include "generic/bitmap.h"
#include "_sexp-types.h"
#include "_sexp-parse.h"
#include "_sexp-output.h"
#include "_seap-command.h"
#include "public/seap-scheme.h"

/*
 * Descriptor table + related stuff
 */
typedef struct {
        SEAP_msgid_t   next_id;
        SEXP_t        *sexpbuf; /* S-exp buffer */
        SEXP_ostate_t *ostate; /* Output state */
        SEXP_pstate_t *pstate; /* Parser state */
        SEAP_scheme_t  scheme; /* Protocol/Scheme used for this descriptor */
        void          *scheme_data; /* Protocol/Scheme related data */
        
        SEAP_cmdtbl_t *cmd_c_table; /* Local SEAP commands */
        SEAP_cmdtbl_t *cmd_w_table; /* Waiting SEAP commands */
} SEAP_desc_t;

#define SEAP_DESC_FDIN  0x00000001
#define SEAP_DESC_FDOUT 0x00000002
#define SEAP_DESC_SELF  -1

typedef struct {
        SEAP_desc_t *sd;
        uint16_t     sdsize;
        bitmap_t     bitmap;
} SEAP_desctable_t;

#define SEAP_DESCTBL_INITIALIZER { NULL, 0, BITMAP_INITIALIZER }

#define SEAP_BUFFER_SIZE 4096
#define SEAP_MAX_OPENDESC 128
#define SDTABLE_REALLOC_ADD 4

int          SEAP_desc_add (SEAP_desctable_t *sd_table, SEXP_pstate_t *pstate, SEAP_scheme_t scheme, void *scheme_data);
int          SEAP_desc_del (SEAP_desctable_t *sd_table, int sd);
SEAP_desc_t *SEAP_desc_get (SEAP_desctable_t *sd_table, int sd);

#endif /* _SEAP_DESCRIPTOR_H */
