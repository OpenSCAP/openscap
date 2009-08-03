#pragma once
#ifndef SEXP_DATATYPE
#define SEXP_DATATYPE

#include <stdarg.h>

typedef void (*SEXP_datatype_opfn_t) (const SEXP_t *, void *, va_list);

typedef struct {
        uint8_t opnum;
        uint8_t arity;
        SEXP_datatype_opfn_t fn_string;
        SEXP_datatype_opfn_t fn_number;
        SEXP_datatype_opfn_t fn_list;
} SEXP_datatype_op_t;

const char *SEXP_datatype (const SEXP_t *sexp);

int SEXP_datatype_register (const char *datatype, SEXP_datype_op_t op[], uint8_t opnummax);
int SEXP_datatype_op (uint8_t op, const SEXP_t *sexp, void *res, ...);
int SEXP_datatype_op_safe (const char *datatype, uint8_t op, const SEXP_t *sexp, void *res, ...);

#endif /* SEXP_DATATYPE */
