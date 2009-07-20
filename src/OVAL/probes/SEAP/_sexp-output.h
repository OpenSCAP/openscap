#pragma once
#ifndef _SEXP_OUTPUT_H
#define _SEXP_OUTPUT_H

#include <stddef.h>
#include <stdint.h>
#include "public/sexp-types.h"
#include "public/sexp-output.h"
#include "_sexp-manip.h"

struct SEXP_ostate {
        SEXP_t      *sexp;
        LIST_stack_t lstack;      
        uint32_t    *list_pos;  /* stack of lpositions */
        uint8_t      sexp_part; /* 0 - type, 1 - data  */
        size_t       sexp_pos;
};

#endif /* _SEXP_OUTPUT_H */
