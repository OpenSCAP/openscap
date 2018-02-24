/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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

#include <stddef.h>
#include <errno.h>

#include "public/sm_alloc.h"
#include "_seap-command.h"
#include "generic/rbt/rbt.h"
#include "seap-command-backendT.h"

typedef rbt_t Ttable_t;

static Ttable_t *Ttable_new (size_t capacity)
{
        Ttable_t *t;

        t = rbt_i32_new();

        return (t);
}

static int Ttable_add (Ttable_t *t, SEAP_cmdrec_t *r)
{
        return rbt_i32_add(t, (int32_t) r->code, (void *)r, NULL);
}

static SEAP_cmdrec_t *Ttable_get (Ttable_t *t, SEAP_cmdcode_t c)
{
        SEAP_cmdrec_t *r = NULL;

        rbt_i32_get(t, (int32_t)c, (void *)&r);

        return (r);
}

int  SEAP_cmdtbl_backendT_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        if (t->table == NULL)
                t->table = Ttable_new(-1);

	if (t->table == NULL) {
		return -1;
	}

        return Ttable_add ((Ttable_t *)t->table, r);
}

int  SEAP_cmdtbl_backendT_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        errno = EOPNOTSUPP;
        return (-1);
}

int  SEAP_cmdtbl_backendT_del (SEAP_cmdtbl_t *t, SEAP_cmdcode_t c)
{
        SEAP_cmdrec_t *tr = NULL;

        if (rbt_i32_del((Ttable_t*)t->table, (int32_t)c, (void *)&tr) != 0)
                return (-1);

        SEAP_cmdrec_free(tr);

        return(0);
}

SEAP_cmdrec_t *SEAP_cmdtbl_backendT_get (SEAP_cmdtbl_t *t, SEAP_cmdcode_t c)
{
        return (t->table == NULL ? NULL : Ttable_get ((Ttable_t *)t->table, c));
}

static void backendT_free_callback(struct rbt_i32_node *n)
{
        SEAP_cmdrec_free(n->data);
}

void SEAP_cmdtbl_backendT_free (SEAP_cmdtbl_t *t)
{
        if (t->table != NULL)
                rbt_i32_free_cb((Ttable_t *)(t->table), &backendT_free_callback);
}

int SEAP_cmdtbl_backendT_apply (SEAP_cmdtbl_t *t, int (*func) (SEAP_cmdrec_t *r, void *), void *arg)
{
        /*
          size_t i, l;
          int ret;

        Ttable_t *St;

        St = (Ttable_t *)(t->table);
        */

        return (-1);
}
