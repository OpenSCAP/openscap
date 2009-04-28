#include <stdlib.h>
#include <config.h>
#include <errno.h>
#include <assert.h>
#include "common.h"
#include "xmalloc.h"
#include "sexp-handler.h"

#ifndef _A
#define _A(x) assert(x)
#endif

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
#if defined(THREAD_SAFE)
        .rwlock = PTHREAD_RWLOCK_INITIALIZER,
#endif
        .tree = { .root = NULL, .size = 0 },
        .init = 1
};

void SEXP_handlertbl_init (SEXP_handlertbl_t *htbl)
{
        _A(htbl != NULL);
#if defined(THREAD_SAFE)
        htbl->rwlock = PTHREAD_RWLOCK_INITIALIZER;
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
        
#if defined(THREAD_SAFE)
        pthread_rwlock_rdlock (&(htbl->rwlock));
#endif
        
        key.handler.typestr = (char *)typestr;
        key.handler.typelen = typelen;
        
        if ((nret = RB_SEARCH(handlers)(&(htbl->tree), &key)) != NULL) {
                _D("Found handler for: %.*s\n", key.handler.typelen, key.handler.typestr);
                ret = &(nret->handler);
        } else {
                _D("Handler for %.*s not found\n", key.handler.typelen, key.handler.typestr);
                ret = NULL;
        }
        
#if defined(THREAD_SAFE)
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

#if defined(THREAD_SAFE)
        pthread_rwlock_wrlock (&(htbl->rwlock));
#endif
        
        new = RB_NEWNODE(handlers)();
        memcpy (&(new->handler), handler, sizeof (SEXP_handler_t));
        
        if (RB_INSERT(handlers)(&(htbl->tree), new) == E_OK) {
                _D("Registered handler for: %.*s\n", handler->typelen, handler->typestr);
                ret = &(new->handler);
        } else {
                _D("Failed to register handler for: %.*s\n", handler->typelen, handler->typestr);
                xfree ((void **)&new);
                ret = NULL;
        }
        
#if defined(THREAD_SAFE)
        pthread_rwlock_unlock (&(htbl->rwlock));
#endif
        return (ret);
}

int SEXP_delhandler (SEXP_handlertbl_t *htbl, const char *typestr, size_t typelen)
{
        int ret;

        _A(htbl != NULL);
        _A(typestr != NULL);
        _A(typelen > 0);

#if defined(THREAD_SAFE)
        pthread_rwlock_wrlock (&(htbl->rwlock));
#endif

        /* TODO: implement rb tree delete :] */
        
#if defined(THREAD_SAFE)
        pthread_rwlock_unlock (&(htbl->rwlock));
#endif
        return (ret);
}
