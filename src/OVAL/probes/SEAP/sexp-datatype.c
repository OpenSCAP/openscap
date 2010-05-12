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

#include <string.h>
#include <errno.h>
#include "common/assume.h"
#include "public/sm_alloc.h"
#include "_sexp-datatype.h"

#if defined(SEAP_THREAD_SAFE)
# include<pthread.h>
static pthread_once_t __SEXP_datatype_once = PTHREAD_ONCE_INIT;
#endif

SEXP_datatypeTbl_t g_datatypes = { NULL };

SEXP_datatypeTbl_t *SEXP_datatypeTbl_new ()
{
        SEXP_datatypeTbl_t *t;

        t = sm_talloc (SEXP_datatypeTbl_t);
        SEXP_datatypeTbl_init(t);

        return(t);
}

void SEXP_datatypeTbl_free(SEXP_datatypeTbl_t *t)
{
        rbt_str_free(t->tree);
        sm_free(t);
        return;
}

int SEXP_datatypeTbl_init (SEXP_datatypeTbl_t *t)
{
        assume_d(t != NULL, -1);
        t->tree = rbt_str_new();
        return(0);
}

void SEXP_datatype_once(void)
{
#if defined(SEAP_THREAD_SAFE)
        if (pthread_once (&__SEXP_datatype_once, &SEXP_datatypeGlobalTbl_init) != 0)
                abort ();
#else
        SEXP_datatypeGlobalTbl_init();
#endif
        return;
}

void SEXP_datatypeGlobalTbl_init(void)
{
        if (g_datatypes.tree == NULL) {
                SEXP_datatypeTbl_init(&g_datatypes);
                atexit(&SEXP_datatypeGlobalTbl_free);
        }
}

void SEXP_datatypeGlobalTbl_free(void)
{
        if (g_datatypes.tree != NULL) {
                rbt_str_free(g_datatypes.tree);
        }
}

SEXP_datatypePtr_t *SEXP_datatype_get (SEXP_datatypeTbl_t *t, const char *k)
{
        void *r;
        struct rbt_str_node *n = NULL;
        SEXP_datatype_t *d;

        assume_d(t != NULL, NULL);
        assume_d(k != NULL, NULL);

        SEXP_datatype_once();

        if (rbt_str_getnode(t->tree, k, &n) != 0)
                return(NULL);

        d = (SEXP_datatype_t *)(n->data);

        if (d != NULL) {
                if (d->dt_flg & SEXP_DTFLG_LOCALDATA) {
                        struct SEXP_datatype_extptr *eptr = NULL;

                        /* See comment in SEXP_datatype_add */
                        if (posix_memalign((void **)(void *)(&eptr), sizeof(void *),
                                           sizeof(struct SEXP_datatype_extptr)) != 0)
                        {
                                return(NULL);
                        }

                        eptr->n = n;
                        eptr->l = NULL;

                        return(SEXP_datatypePtr_t *)((uintptr_t)(eptr)|1);
                }
        }

        return((SEXP_datatypePtr_t *)n);
}

SEXP_datatypePtr_t *SEXP_datatype_add(SEXP_datatypeTbl_t *t, char *n, SEXP_datatype_t *d, void *l)
{
        void *r;
        struct rbt_str_node *node = NULL;

        assume_d(t != NULL, NULL);
        assume_d(n != NULL, NULL);

        SEXP_datatype_once();

        /*
         * Check whether flags & passed values are meaningful
         */
        if (l != NULL && (d->dt_flg & SEXP_DTFLG_LOCALDATA) == 0) {
                errno = EINVAL;
                return(NULL);
        }

        if (rbt_str_add(t->tree, n, d) != 0)
                return(NULL);
        /*
         * XXX: consider adding a version of rbt_str_add that returns
         *      the node pointer, so that we don't have perform the
         *      following search operation.
         */
        if (rbt_str_getnode(t->tree, n, &node) != 0)
                return(NULL);

        if (d != NULL) {
                /*
                 * If DTFLG_LOCALDATA is set, allocate a new extended pointer
                 * and return it with `l' set as the local data pointer.
                 */
                if (d->dt_flg & SEXP_DTFLG_LOCALDATA) {
                        struct SEXP_datatype_extptr *eptr = NULL;

                        /*
                         * Ensure that we can use the lowest bit for the extended
                         * pointer flag. In the case we are returning the "normal"
                         * pointer, the memory is already aligned because it's a
                         * pointer into a red-black tree node + 2*sizeof(void *)
                         * offset. The red-black tree implmentation allocates nodes
                         * aligned to sizeof(void *) bytes.
                         */
                        if (posix_memalign((void **)(void *)(&eptr), sizeof(void *),
                                           sizeof(struct SEXP_datatype_extptr)) != 0)
                        {
                                return(NULL);
                        }

                        eptr->n = node;
                        eptr->l = l;

                        r = (void *)((uintptr_t)(eptr)|1);
                } else
                        r = node;
        } else
                r = node;

        return((SEXP_datatypePtr_t *)r);
}

int SEXP_datatype_del (SEXP_datatypeTbl_t *t, const char *name)
{
        errno = EOPNOTSUPP;
        return (-1);
}

const char *SEXP_datatype_name(SEXP_datatypePtr_t *p)
{
        char *name = NULL;

        if ((uintptr_t)p & 1) {
                SEXP_datatypeExtptr_t *ep = (SEXP_datatypeExtptr_t *)p;
                name = ep->n->key;
        } else {
                struct rbt_str_node *node = (struct rbt_str_node *)p;
                name = node->key;
        }

        return(name);
}
