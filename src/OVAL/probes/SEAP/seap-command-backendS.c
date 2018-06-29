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

#include <stddef.h>
#include <errno.h>

#include "_seap-command.h"
#include "seap-command-backendS.h"
#include "seap-command-backendL.h"
#include "debug_priv.h"

typedef struct {
        SEAP_cmdrec_t **c_recs;
        size_t          c_size;
} Stable_rec_t;

typedef struct {
        Stable_rec_t *t_recs;
        size_t        t_size;
} Stable_t;

static const uint16_t Stable_primes[] = {
        7, 13, 19, 29, 37, 43, 53, 97, 193, 389, 769
};

static uint16_t Stable_prime_gt (uint16_t n) {
        size_t w, s;
        
        w = (sizeof Stable_primes) / sizeof (uint16_t);
        s = 0;
        
        while (w > 0) {
                if (n > Stable_primes[s + w/2]) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (n < Stable_primes[s + w/2]) {
                        w = w/2;
                } else
                        break;
        }
        
        _A(s + w/2 < (sizeof Stable_primes) / sizeof (uint16_t));
        
        return (Stable_primes[s + w/2]);
}

static Stable_t *Stable_new (size_t capacity)
{
        Stable_t *t;
        size_t    i;
        
        _A(capacity > 0 &&
           capacity < SEAP_COMMAND_BACKENDS_MAXCAPACITY);
        
	t = malloc(sizeof(Stable_t));
        t->t_size = Stable_prime_gt (capacity);
	t->t_recs = calloc(t->t_size, sizeof(Stable_rec_t));
        
        for (i = 0; i < t->t_size; ++i)
                t->t_recs[i].c_recs = NULL;
        
        return (t);
}

static int Stable_add (Stable_t *t, SEAP_cmdrec_t *r)
{
        Stable_rec_t *t_r;

        _A(t != NULL);
        _A(r != NULL);

        t_r = &(t->t_recs[r->code % t->t_size]);
        ++(t_r->c_size);
	t_r->c_recs = realloc(t_r->c_recs, sizeof(SEAP_cmdrec_t *) * t_r->c_size);
        t_r->c_recs[t_r->c_size - 1] = r;
        
        return (0);
}

static SEAP_cmdrec_t *Stable_get (Stable_t *t, SEAP_cmdcode_t c)
{
        Stable_rec_t *t_r;
        size_t i;

        _A(t != NULL);
        
        t_r = &(t->t_recs[c % t->t_size]);
        
        for (i = 0; i < t_r->c_size; ++i)
                if (t_r->c_recs[i]->code == c)
                        return (t_r->c_recs[i]);
        
        return (NULL);
}

static int Stable_conv (SEAP_cmdrec_t *r, void *arg)
{
        Stable_t *t = (Stable_t *)arg;
        
        _A(t != NULL);
        _A(r != NULL);

        return (Stable_add (t, r));
}

int  SEAP_cmdtbl_backendS_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        _A(r != NULL);
        
        if (t->flags & SEAP_CMDTBL_LARGE) {
                Stable_t *n;
                int       ret;
                
                n = Stable_new (SEAP_CMDTBL_LARGE_TRESHOLD);
                ret = SEAP_cmdtbl_backendL_apply (t, &Stable_conv, (void *)n);
                
                if (ret != 0) {
                        SEAP_cmdtbl_backendS_free (t);
                        return (ret);
                }
                
                SEAP_cmdtbl_backendL_free (t);
                t->table = n;
        } else if (t->table == NULL) {
                t->table = Stable_new (SEAP_CMDTBL_LARGE_TRESHOLD);
        }
        
        _A(t->table != NULL);
        
        return (Stable_add ((Stable_t *)t->table, r));
}

int  SEAP_cmdtbl_backendS_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        _A(r != NULL);
        errno = EOPNOTSUPP;
        return (-1);
}

int  SEAP_cmdtbl_backendS_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r)
{
        _A(t != NULL);
        _A(r != NULL);
        errno = EOPNOTSUPP;
        return (-1);
}

SEAP_cmdrec_t *SEAP_cmdtbl_backendS_get (SEAP_cmdtbl_t *t, SEAP_cmdcode_t c)
{
        _A(t != NULL);
        return (t->table == NULL ? NULL : Stable_get ((Stable_t *)t->table, c));
}

int  SEAP_cmdtbl_backendS_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b)
{
        return (a->code - b->code);
}

void SEAP_cmdtbl_backendS_free (SEAP_cmdtbl_t *t)
{
        size_t    i;
        Stable_t *St;
        
        St = (Stable_t *)(t->table);
        
        if (St != NULL) {
                for (i = 0; i < St->t_size; ++i)
                        if (St->t_recs[i].c_size > 0)
				free(St->t_recs[i].c_recs);
                
		free(St->t_recs);
		free(St);
                
                t->table = NULL;
        }
        return;
}

int SEAP_cmdtbl_backendS_apply (SEAP_cmdtbl_t *t, int (*func) (SEAP_cmdrec_t *r, void *), void *arg)
{
        size_t i, l;
        Stable_t *St;
        int ret;

        St = (Stable_t *)(t->table);
        
        for (i = 0; i < St->t_size; ++i) {
                for (l = 0; l < St->t_recs[i].c_size; ++l) {
                        
                        ret = func (St->t_recs[i].c_recs[l], arg);
                        if (ret != 0)
                                return (ret);
                }
        }
        
        return (0);
}
