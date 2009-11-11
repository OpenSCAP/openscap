#pragma once
#ifndef _SEXP_DATATYPE
#define _SEXP_DATATYPE

#include "public/sexp-datatype.h"
#include "generic/redblack.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

typedef struct {
        uint8_t              arity;
        SEXP_datatype_opfn_t fn[3];
} SEXP_dtype_op_t;

struct SEXP_datatype {
        char            *name;
        uint16_t         name_len;
        uint16_t         op_cnt;
        SEXP_dtype_op_t *op;
};

DEFRBTREE(datatype, SEXP_datatype_t datatype);

struct SEXP_datatypetbl {
        TREETYPE(datatype) tree;
};

SEXP_datatypetbl_t *SEXP_datatypetbl_new (void);
int SEXP_datatypetbl_init (SEXP_datatypetbl_t *t);

SEXP_datatype_t *SEXP_datatype_get (SEXP_datatypetbl_t *t, const char *name);
SEXP_datatype_t *SEXP_datatype_add (SEXP_datatypetbl_t *t, SEXP_datatype_t *datatype);
int              SEXP_datatype_del (SEXP_datatypetbl_t *t, const char *name);

OSCAP_HIDDEN_END;

#endif /* _SEXP_DATATYPE */
