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

typedef uint8_t SEAP_cmdclass_t;

#define SEAP_CMDCLASS_INT 1
#define SEAP_CMDCLASS_USR 2

#define SEAP_CMDFLAG_SYNC  0x01
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

#define SEAP_CMDTBL_LARGE 0x01

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
int SEAP_cmdtbl_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b);

/* Backends */
int  SEAP_cmdtbl_backendL_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendL_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendL_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendL_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b);
void SEAP_cmdtbl_backendL_free (SEAP_cmdtbl_t *t);

int  SEAP_cmdtbl_backendS_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendS_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendS_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendS_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b);
void SEAP_cmdtbl_backendS_free (SEAP_cmdtbl_t *t);

#define SEAP_DESC_SELF    -1

#endif /* _SEAP_COMMAND_H */
