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

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include "generic/common.h"
#include "generic/redblack.h"
#include "public/sm_alloc.h"
#include "_sexp-types.h"
#include "sexp-handler.h"

RBNODECMP(handlers)
{
        return xstrncoll (a->handler.typestr, a->handler.typelen,
                          b->handler.typestr, b->handler.typelen);
}

RBNODEJOIN(handlers)
{
        (void)b;
        return (a);
}

/*
 * Generate code for
 * rb tree handling.
 */
RBTREECODE(handlers);

SEXP_handlertbl_t gSEXP_handlers = {
#if defined(SEAP_THREAD_SAFE)
        .rwlock = PTHREAD_RWLOCK_INITIALIZER,
#endif
        .tree = { .root = NULL, .size = 0 },
        .init = 1
};

void SEXP_handlertbl_init (SEXP_handlertbl_t *htbl)
{
        _A(htbl != NULL);
#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_init (&(htbl->rwlock), NULL);
#endif
        htbl->tree.root = NULL;
        htbl->tree.size = 0;
        htbl->init = 1;
}

SEXP_handler_t *SEXP_gethandler (SEXP_handlertbl_t *htbl, const char *typestr, size_t typelen)
{
        SEXP_handler_t *ret = NULL;
        NODETYPE(handlers) *nret, key;

        _A(htbl != NULL);
        _A(typestr != NULL);
        _A(typelen > 0);

#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_rdlock (&(htbl->rwlock));
#endif

        key.handler.typestr = (char *)typestr;
        key.handler.typelen = typelen;

        if ((nret = RB_SEARCH(handlers)(&(htbl->tree), &key)) != NULL) {
                dI("Found handler for: %.*s", key.handler.typelen, key.handler.typestr);
                ret = &(nret->handler);
        } else {
                dI("Handler for %.*s not found", key.handler.typelen, key.handler.typestr);
                ret = NULL;
        }

#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_unlock (&(htbl->rwlock));
#endif
        return (ret);
}

SEXP_handler_t *SEXP_reghandler (SEXP_handlertbl_t *htbl, SEXP_handler_t *handler)
{
        SEXP_handler_t *ret = NULL;
        NODETYPE(handlers) *new;

        _A(htbl != NULL);
        _A(handler != NULL);

#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_wrlock (&(htbl->rwlock));
#endif

        new = RB_NEWNODE(handlers)();
        memcpy (&(new->handler), handler, sizeof (SEXP_handler_t));

        if (RB_INSERT(handlers)(&(htbl->tree), new) == E_OK) {
                dI("Registered handler for: %.*s", handler->typelen, handler->typestr);
                ret = &(new->handler);
        } else {
                dI("Failed to register handler for: %.*s", handler->typelen, handler->typestr);
                sm_free (new);
                ret = NULL;
        }

#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_unlock (&(htbl->rwlock));
#endif
        return (ret);
}

int SEXP_delhandler (SEXP_handlertbl_t *htbl, const char *typestr, size_t typelen)
{
        int ret = 0;

        _A(htbl != NULL);
        _A(typestr != NULL);
        _A(typelen > 0);

#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_wrlock (&(htbl->rwlock));
#endif

        /* TODO: implement rb tree delete :] */

#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_unlock (&(htbl->rwlock));
#endif
        return (ret);
}

