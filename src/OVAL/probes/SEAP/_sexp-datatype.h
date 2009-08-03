#pragma once
#ifndef _SEXP_DATATYPE
#define _SEXP_DATATYPE

#include "public/sexp-datatype.h"

typedef struct {
        uint8_t arity;
        SEXP_datatype_opfn_t fn[3];
} __SEXP_datatype_op_t;

#endif /* _SEXP_DATATYPE */
