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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <pthread.h>

#include "generic/bitmap.h"
#include "seap-descriptor.h"
#include "_sexp-atomic.h"

SEAP_desctable_t *SEAP_desctable_new (void)
{
	SEAP_desctable_t *t = malloc(sizeof(SEAP_desctable_t));
        t->tree = NULL;
        t->bmap = NULL;

        return(t);
}

int SEAP_desc_add(SEAP_desctable_t *sd_table, SEAP_scheme_t scheme,
                   void *scheme_data)
{
        bitmap_bitn_t sd;
        pthread_mutexattr_t mutex_attr;
        SEAP_desc_t  *sd_dsc;

        if (sd_table->bmap == NULL)
                sd_table->bmap = bitmap_new(128); /* XXX: hardcoded size */
        if (sd_table->bmap == NULL)
                return (-1);

        sd = bitmap_setfree (sd_table->bmap);

        if (sd >= 0) {
                if (sd_table->tree == NULL)
                        sd_table->tree = rbt_i32_new();

		sd_dsc = malloc(sizeof(SEAP_desc_t));

                sd_dsc->next_id = 0;
                /* sd_dsc->sexpcnt = 0; */
                sd_dsc->scheme  = scheme;
                sd_dsc->scheme_data = scheme_data;
                sd_dsc->ostate  = NULL;
                sd_dsc->next_cid = 0;
                sd_dsc->cmd_c_table = SEAP_cmdtbl_new ();
                sd_dsc->cmd_w_table = SEAP_cmdtbl_new ();
		sd_dsc->msg_queue = NULL;
		sd_dsc->err_queue = rbt_i32_new();
		sd_dsc->cmd_queue = NULL;

		SEAP_packetq_init(&sd_dsc->pck_queue);

                pthread_mutexattr_init (&mutex_attr);
                pthread_mutexattr_settype (&mutex_attr, PTHREAD_MUTEX_RECURSIVE);

                pthread_mutex_init (&(sd_dsc->r_lock), &mutex_attr);
                pthread_mutex_init (&(sd_dsc->w_lock), &mutex_attr);

                pthread_mutexattr_destroy (&mutex_attr);

                if (rbt_i32_add(sd_table->tree, (int32_t)sd, (void *)sd_dsc, NULL) != 0) {
                        SEAP_desc_free(sd_dsc);
                        return (-1);
                }

                return ((int)sd);
        }

        return (-1);
}

int SEAP_desc_del (SEAP_desctable_t *sd_table, int sd)
{
        SEAP_desc_t *dsc = NULL;

        if (sd < 0) {
                errno = EINVAL;
                return (-1);
        }

        if (sd_table->tree == NULL)
                return (-1);

        if (rbt_i32_del(sd_table->tree, sd, (void *)&dsc) != 0)
                return (-1);

        bitmap_unset(sd_table->bmap, sd);

        if (dsc != NULL)
                SEAP_desc_free(dsc);

        return (0);
}

static void __SEAP_desc_errqueue_free_cb(struct rbt_i32_node *n)
{
    SEAP_error_free(n->data);
}

void SEAP_desc_free(SEAP_desc_t *dsc)
{
        SEAP_cmdtbl_free(dsc->cmd_c_table);
        SEAP_cmdtbl_free(dsc->cmd_w_table);
	SEAP_packetq_free(&dsc->pck_queue);
        pthread_mutex_destroy(&(dsc->r_lock));
        pthread_mutex_destroy(&(dsc->w_lock));
	rbt_i32_free_cb(dsc->err_queue, __SEAP_desc_errqueue_free_cb);
	free(dsc);
}

static void SEAP_desc_free_node(struct rbt_i32_node *n)
{
        SEAP_desc_free(n->data);
}

void SEAP_desctable_free(SEAP_desctable_t *sd_table)
{
        if (sd_table->tree != NULL)
                rbt_i32_free_cb(sd_table->tree, &SEAP_desc_free_node);
        if (sd_table->bmap != NULL)
                bitmap_free(sd_table->bmap);
	free(sd_table);
}

SEAP_desc_t *SEAP_desc_get (SEAP_desctable_t *sd_table, int sd)
{
        SEAP_desc_t *dsc = NULL;

        if (sd < 0 || sd_table->tree == NULL) {
                errno = EBADF;
                return (NULL);
        }

        if (rbt_i32_get(sd_table->tree, sd, (void *)&dsc) != 0) {
                errno = EBADF;
                return (NULL);
        }

        return(dsc);
}

SEAP_msgid_t SEAP_desc_genmsgid (SEAP_desctable_t *sd_table, int sd)
{
        SEAP_desc_t *dsc;
        SEAP_msgid_t  id;
        dsc = SEAP_desc_get (sd_table, sd);

        if (dsc == NULL) {
                errno = EINVAL;
                return (-1);
        }

#if SEAP_MSGID_BITS == 64
        id = SEXP_atomic_inc_u64 (&(dsc->next_id));
#else
        id = SEXP_atomic_inc_u32 (&(dsc->next_id));
#endif
        return (id);
}

SEAP_cmdid_t SEAP_desc_gencmdid (SEAP_desctable_t *sd_table, int sd)
{
        SEAP_desc_t *dsc;
        SEAP_cmdid_t  id;

        dsc = SEAP_desc_get (sd_table, sd);

        if (dsc == NULL) {
                errno = EINVAL;
                return (-1);
        }

        id = SEXP_atomic_inc_u16 (&(dsc->next_cid));

        return (id);
}
