
/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

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
