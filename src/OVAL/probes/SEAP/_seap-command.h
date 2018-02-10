/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#pragma once
#ifndef _SEAP_COMMAND_H
#define _SEAP_COMMAND_H

#include <stdint.h>
#include <stddef.h>
#include <pthread.h>

#include "public/seap-command.h"
#include "_sexp-types.h"
#include "../../../common/util.h"


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
        int signaled;
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

typedef uint8_t SEAP_cflags_t;

#define SEAP_CFLG_THREAD 0x01
#define SEAP_CFLG_WATCH  0x02

/* Backends */
#include "seap-command-backendT.h"

typedef struct {
        SEAP_CTX_t *ctx;
        int         sd;
        SEAP_cmd_t *cmd;
} SEAP_cmdjob_t;

SEAP_cmdjob_t *SEAP_cmdjob_new (void);
void SEAP_cmdjob_free (SEAP_cmdjob_t *j);

SEXP_t *SEAP_cmd2sexp (SEAP_cmd_t *cmd);


#endif /* _SEAP_COMMAND_H */
