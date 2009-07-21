#pragma once
#ifndef _SEXP_PARSE_H
#define _SEXP_PARSE_H

#include <stddef.h>
#include "public/sexp-parse.h"
#include "_sexp-manip.h"

/* Number classes */
#define NUMCLASS_INV 0
#define NUMCLASS_INT 1
#define NUMCLASS_FLT 2
#define NUMCLASS_FRA 3
#define NUMCLASS_EXP 4

typedef uint8_t SEXP_pflags_t;

#define PF_EOFOK 0x01
#define PF_ALL   0xff

/* S-expression parsing state */
struct SEXP_pstate {
        char    *buffer;
        size_t   buffer_data_len;
        size_t   buffer_fail_off;

        LIST_stack_t  lstack;
        SEXP_pflags_t pflags;
};

struct SEXP_psetup {
        SEXP_format_t fmt;
        SEXP_pflags_t pflags;
};

void SEXP_psetup_init (SEXP_psetup_t *psetup);

#endif /* _SEXP_PARSE_H */
