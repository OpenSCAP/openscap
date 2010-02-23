#pragma once
#ifndef SEXP_DATATYPE
#define SEXP_DATATYPE

#include <stdarg.h>
#include <stdint.h>
#include <sexp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SEXP_datatype_opfn_t) (const SEXP_t *, void *, ...);

typedef struct {
        uint8_t opnum;
        uint8_t arity;
        SEXP_datatype_opfn_t fn_string;
        SEXP_datatype_opfn_t fn_number;
        SEXP_datatype_opfn_t fn_list;
} SEXP_datatype_op_t;

typedef struct SEXP_datatype    SEXP_datatype_t;
typedef struct SEXP_datatypetbl SEXP_datatypetbl_t;

extern SEXP_datatypetbl_t g_datatypes;

/* const char *SEXP_datatype (const SEXP_t *sexp); */

int SEXP_datatype_register (SEXP_datatypetbl_t *t, const char *datatype, SEXP_datatype_op_t *op, uint8_t opnummax);
/* int SEXP_datatype_unregister (void); */

int SEXP_datatype_op (uint8_t op, const SEXP_t *sexp, void *res, ...);
int SEXP_datatype_op_safe (const char *datatype, uint8_t op, const SEXP_t *sexp, void *res, ...);

#ifdef __cplusplus
}
#endif

#endif /* SEXP_DATATYPE */
