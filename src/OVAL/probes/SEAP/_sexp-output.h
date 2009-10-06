#pragma once
#ifndef _SEXP_OUTPUT_H
#define _SEXP_OUTPUT_H

#include <stddef.h>
#include <stdint.h>
#include "public/sexp-types.h"
#include "public/sexp-output.h"
#include "_sexp-manip.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

struct SEXP_ostate {
        SEXP_t        *s_exp;   /* ref to the S-exp that we are writing */
        SEXP_lstack_t *l_stack; /* list stack */
        uint32_t      *p_stack; /* list position stack */
        uint8_t       *s_part;  /* S-exp part */
        size_t        *s_pos;   /* S-exp position */
        
#if 0
        SEXP_t      *sexp;
        LIST_stack_t lstack;
        uint32_t    *list_pos;  /* stack of lpositions */
        uint8_t      sexp_part; /* 0 - type, 1 - data  */
        size_t       sexp_pos;
#endif
};

OSCAP_HIDDEN_END;

#endif /* _SEXP_OUTPUT_H */
