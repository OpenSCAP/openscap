#pragma once
#ifndef SEXP_HANDLER_H
#define SEXP_HANDLER_H

#if defined(SEAP_THREAD_SAFE)
# include <pthread.h>
#endif

#include <stdio.h>

typedef struct {
        char    *typestr;
        uint16_t typelen;
        
        int (*fprint) (FILE *, SEXP_t *);
        SEXP_t * (*fread) (FILE *, size_t);
        int (*dprint) (int, SEXP_t *);
        SEXP_t * (*dread) (int, size_t);
        int (*mem2sexp) (void *, size_t *, SEXP_t *);
        int (*sexp2mem) (SEXP_t *, void *, size_t);
} SEXP_handler_t;

#include "generic/redblack.h"
DEFRBTREE(handlers, SEXP_handler_t handler);

typedef struct {
#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_t rwlock;
#endif
        TREETYPE(handlers) tree;
        uint8_t      init;
} SEXP_handlertbl_t;

extern SEXP_handlertbl_t gSEXP_handlers;

void SEXP_handlertbl_init (SEXP_handlertbl_t *htbl);
SEXP_handler_t *SEXP_gethandler (SEXP_handlertbl_t *htbl, const char *typestr, size_t typelen);
SEXP_handler_t *SEXP_reghandler (SEXP_handlertbl_t *htbl, SEXP_handler_t *handler);
int SEXP_delhandler (SEXP_handlertbl_t *htbl, const char *typestr, size_t typelen);

#define SEXP_gethandler_g(s, l) SEXP_gethandler (&gSEXP_handlers, s, l)
#define SEXP_reghandler_g(h)    SEXP_reghandler (&gSEXP_handlers, h)
#define SEXP_delhandler_g(s, l) SEXP_delhandler (&gSEXP_handlers, s, l)

#endif /* SEXP_HANDLER_H */
