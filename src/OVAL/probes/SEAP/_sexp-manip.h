#pragma once
#ifndef _SEXP_MANIP_H
#define _SEXP_MANIP_H

#include "public/sexp-manip.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

#define SEXP_LSTACK_INIT_SIZE      32
#define SEXP_LSTACK_GROWFAST_TRESH 512
#define SEXP_LSTACK_GROWSLOW_DIFF  32

typedef struct {
        size_t   l_size;
        size_t   l_real;
        SEXP_t **l_sref;
} SEXP_lstack_t;

SEXP_lstack_t *SEXP_lstack_new (void);
SEXP_t *SEXP_lstack_push (SEXP_lstack_t *stack, SEXP_t *ref);
SEXP_t *SEXP_lstack_pop (SEXP_lstack_t *stack);
SEXP_t *SEXP_lstack_top (SEXP_lstack_t *stack);

struct SEXP_it {
        SEXP_t *s_ref;
        
};

OSCAP_HIDDEN_END;

#endif /* _SEXP_MANIP_H */
