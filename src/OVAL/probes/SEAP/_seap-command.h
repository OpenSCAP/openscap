#ifndef __STUB_PROBE
#pragma once
#ifndef _SEAP_COMMAND_H
#define _SEAP_COMMAND_H

#include <stdint.h>
#include <stddef.h>
#if defined(SEAP_THREAD_SAFE)
# include <pthread.h>
#endif

#include "public/seap-command.h"
#include "_sexp-types.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

typedef uint8_t SEAP_cmdclass_t;

#define SEAP_CMDCLASS_INT 1
#define SEAP_CMDCLASS_USR 2

#define SEAP_CMDFLAG_SYNC  0x01
#define SEAP_CMDFLAG_ASYNC 0x00
#define SEAP_CMDFLAG_REPLY 0x02
#define SEAP_CMDFLAG_MASK  0xff

struct SEAP_cmd {
        SEAP_cmdid_t    id;
        SEAP_cmdid_t    rid;
        uint8_t         flags;
        SEAP_cmdclass_t class;
        SEAP_cmdcode_t  code;
        SEXP_t         *args;
};

struct SEAP_synchelper {
        SEXP_t         *args;
        pthread_cond_t  cond;
        pthread_mutex_t mtx;
};

#define SEAP_CMDTBL_LARGE 0x01
#define SEAP_CMDTBL_LARGE_TRESHOLD 32

typedef struct {
        uint8_t  flags;
        void    *table;
        size_t   maxcnt;
#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_t lock;
#endif
} SEAP_cmdtbl_t;

typedef struct {
        SEAP_cmdcode_t code;
        SEAP_cmdfn_t   func;
        void          *arg;
} SEAP_cmdrec_t;

SEAP_cmdtbl_t *SEAP_cmdtbl_new (void);
void SEAP_cmdtbl_free (SEAP_cmdtbl_t *t);

int SEAP_cmdtbl_setsize (SEAP_cmdtbl_t *t, size_t maxsz);
int SEAP_cmdtbl_setfl (SEAP_cmdtbl_t *t, uint8_t f);
int SEAP_cmdtbl_unsetfl (SEAP_cmdtbl_t *t, uint8_t f);

/* Generic operations */
int SEAP_cmdtbl_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int SEAP_cmdtbl_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int SEAP_cmdtbl_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
SEAP_cmdrec_t *SEAP_cmdtbl_get (SEAP_cmdtbl_t *t, SEAP_cmdcode_t c);
int SEAP_cmdtbl_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b);

#define SEAP_CMDTBL_ECOLL 1

SEAP_cmdrec_t *SEAP_cmdrec_new (void);
void SEAP_cmdrec_free (SEAP_cmdrec_t *r);

#define SEAP_EXEC_LOCAL  0x01
#define SEAP_EXEC_LONLY  0x02
#define SEAP_EXEC_GFIRST 0x04
#define SEAP_EXEC_THREAD 0x08
#define SEAP_EXEC_WQUEUE 0x10

typedef uint8_t SEAP_cflags_t;

#define SEAP_CFLG_THREAD 0x01
#define SEAP_CFLG_WATCH  0x02

/* Backends */
#include "seap-command-backendL.h"
#include "seap-command-backendS.h"

typedef struct {
        SEAP_CTX_t *ctx;
        int         sd;
        SEAP_cmd_t *cmd;
} SEAP_cmdjob_t;

SEAP_cmdjob_t *SEAP_cmdjob_new (void);
void SEAP_cmdjob_free (SEAP_cmdjob_t *j);

SEXP_t *SEAP_cmd2sexp (SEAP_cmd_t *cmd);

OSCAP_HIDDEN_END;

#endif /* _SEAP_COMMAND_H */
#endif
