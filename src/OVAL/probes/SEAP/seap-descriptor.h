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
#ifndef _SEAP_DESCRIPTOR_H
#define _SEAP_DESCRIPTOR_H

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include "generic/bitmap.h"
#include "generic/rbt/rbt.h"
#include "_sexp-types.h"
#include "_seap-packetq.h"
#include "_sexp-parser.h"
#include "_sexp-output.h"
#include "_seap-command.h"
#include "public/seap-scheme.h"
#include "public/seap-message.h"
#include "public/seap-command.h"
#include "public/seap-error.h"
#include "../../../common/util.h"


/*
 * Descriptor table + related stuff
 */
typedef struct {
        SEAP_msgid_t   next_id;
        SEXP_ostate_t *ostate; /* Output state */
        SEXP_pstate_t *pstate; /* Parser state */
        SEAP_scheme_t  scheme; /* Protocol/Scheme used for this descriptor */
        void          *scheme_data; /* Protocol/Scheme related data */

        SEXP_t *msg_queue;
	rbt_t  *err_queue;
        SEXP_t *cmd_queue;

        SEAP_packetq_t pck_queue;

        pthread_mutex_t w_lock;
        pthread_mutex_t r_lock;

        SEAP_cmdid_t   next_cid;
        SEAP_cmdtbl_t *cmd_c_table; /* Local SEAP commands */
        SEAP_cmdtbl_t *cmd_w_table; /* Waiting SEAP commands */
} SEAP_desc_t;

#define SEAP_DESC_FDIN  0x00000001
#define SEAP_DESC_FDOUT 0x00000002
#define SEAP_DESC_SELF  -1

typedef struct {
        rbt_t       *tree;
        bitmap_t    *bmap;
} SEAP_desctable_t;

#define SEAP_DESCTBL_INITIALIZER { NULL, NULL }

#define SEAP_BUFFER_SIZE 2*4096
#define SEAP_MAX_OPENDESC 128
#define SDTABLE_REALLOC_ADD 4

int          SEAP_desc_add (SEAP_desctable_t *sd_table, SEXP_pstate_t *pstate, SEAP_scheme_t scheme, void *scheme_data);
int          SEAP_desc_del (SEAP_desctable_t *sd_table, int sd);
SEAP_desc_t *SEAP_desc_get (SEAP_desctable_t *sd_table, int sd);

SEAP_desctable_t *SEAP_desctable_new (void);
void SEAP_desctable_free(SEAP_desctable_t *sd_table);
void SEAP_desc_free(SEAP_desc_t *dsc);

static inline int SEAP_desc_trylock (pthread_mutex_t *m)
{
        switch (pthread_mutex_trylock (m)) {
        case 0:
                return (1);
        case EBUSY:
                return (0);
        case EINVAL:
                errno = EINVAL;
                /* FALLTHROUGH */
        default:
                return (-1);
        }
}

static inline int SEAP_desc_lock (pthread_mutex_t *m)
{
        switch (pthread_mutex_lock (m)) {
        case 0:
                return (1);
        default:
                return (-1);
        }
}

static inline int SEAP_desc_unlock (pthread_mutex_t *m)
{
        switch (pthread_mutex_unlock (m)) {
        case 0:
                return (1);
        default:
                return (-1);
        }
}

#define DESC_TRYRLOCK(d) SEAP_desc_trylock (&((d)->r_lock))
#define DESC_RLOCK(d)    SEAP_desc_lock (&((d)->r_lock))
#define DESC_RUNLOCK(d)  SEAP_desc_unlock (&((d)->r_lock))

#define DESC_TRYWLOCK(d) SEAP_desc_trylock (&((d)->w_lock))
#define DESC_WLOCK(d)    SEAP_desc_lock (&((d)->w_lock))
#define DESC_WUNLOCK(d)  SEAP_desc_unlock (&((d)->w_lock))

SEAP_msgid_t SEAP_desc_genmsgid (SEAP_desctable_t *sd_table, int sd);
SEAP_cmdid_t SEAP_desc_gencmdid (SEAP_desctable_t *sd_table, int sd);


#endif /* _SEAP_DESCRIPTOR_H */
