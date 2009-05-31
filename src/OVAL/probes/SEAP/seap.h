#pragma once
#ifndef SEAP_H
#define SEAP_H

#include <stddef.h>
#include <stdint.h>
#include "sexp.h"
/*
#include "sexp-types.h"
#include "sexp-parse.h"
#include "seap-types.h"
*/
#include "seap-scheme.h"

int SEAP_desc_add (SEAP_desctable_t *sd_table, SEXP_pstate_t *pstate, SEAP_scheme_t scheme, void *scheme_data);
int SEAP_desc_del (SEAP_desctable_t *sd_table, int sd);
SEAP_desc_t *SEAP_desc_get (SEAP_desctable_t *sd_table, int sd);

SEAP_CTX_t *SEAP_CTX_new  (void);
void        SEAP_CTX_init (SEAP_CTX_t *ctx);
void        SEAP_CTX_free (SEAP_CTX_t *ctx);

int     SEAP_connect (SEAP_CTX_t *ctx, const char *uri, uint32_t flags);
int     SEAP_listen (SEAP_CTX_t *ctx, int sd, uint32_t maxcli);

/* int     SEAP_accept (SEAP_t *ctx, int sd); */

int     SEAP_open  (SEAP_CTX_t *ctx, const char *path, uint32_t flags);
SEXP_t *SEAP_read  (SEAP_CTX_t *ctx, int sd);
int     SEAP_write (SEAP_CTX_t *ctx, int sd, SEXP_t *sexp);
int     SEAP_close (SEAP_CTX_t *ctx, int sd);

int SEAP_openfd (SEAP_CTX_t *ctx, int fd, uint32_t flags);
int SEAP_openfd2 (SEAP_CTX_t *ctx, int ifd, int ofd, uint32_t flags);

#if 0
#include <stdio.h>
int SEAP_openfp (SEAP_CTX_t *ctx, FILE *fp, uint32_t flags);
#endif /* 0 */

SEAP_msg_t *SEAP_msg_new (void);
void        SEAP_msg_free (SEAP_msg_t *msg);
int         SEAP_msg_set (SEAP_msg_t *msg, SEXP_t *sexp);
SEXP_t     *SEAP_msg_get (SEAP_msg_t *msg);

int     SEAP_msgattr_set (SEAP_msg_t *msg, const char *attr, SEXP_t *value);
SEXP_t *SEAP_msgattr_get (SEAP_msg_t *msg, const char *name);

int SEAP_recvsexp (SEAP_CTX_t *ctx, int sd, SEXP_t **sexp);
int SEAP_recvmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t **seap_msg);

int SEAP_sendsexp (SEAP_CTX_t *ctx, int sd, SEXP_t *sexp);
int SEAP_sendmsg (SEAP_CTX_t *ctx, int sd, SEAP_msg_t *seap_msg);
int SEAP_reply (SEAP_CTX_t *ctx, int sd, SEAP_msg_t *rep_msg, SEAP_msg_t *req_msg);
int SEAP_senderr (SEAP_CTX_t *ctx, int sd, int e, SEAP_msg_t *msg);

#endif /* SEAP_H */
