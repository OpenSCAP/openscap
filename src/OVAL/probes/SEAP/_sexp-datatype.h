/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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

#include "generic/rbt/rbt.h"
#include "../../../common/util.h"
#include "_sexp-types.h"


struct SEXP_datatype {
        uint16_t          dt_flg;
        uint8_t          _dt[];
};
typedef struct SEXP_datatype SEXP_datatype_t;

#define SEXP_DTFLG_LOCALDATA 0x01 /* whether a extended pointer should be used */
#define SEXP_DTFLG_HAVEDTOPS 0x02 /* whether there are defined some operations */

typedef void (*SEXP_datatypeOP_t) (SEXP_t *, const SEXP_t *, void *);

struct SEXP_datatype_ops {
        uint16_t           dt_opcnt;
        SEXP_datatypeOP_t *dt_op;
};

struct SEXP_datatypeTbl {
        rbt_t *tree;
};
typedef struct SEXP_datatypeTbl SEXP_datatypeTbl_t;

/*
 * Datatype pointer
 */
typedef struct rbt_str_node SEXP_datatypePtr_t;

/*
 * Extended datatype pointer
 */
struct SEXP_datatype_extptr {
        struct rbt_str_node *n; /* datatype tree node */
        void                *l; /* local data related to an S-exp reference */
};

typedef struct SEXP_datatype_extptr SEXP_datatypeExtptr_t;


const char *SEXP_datatype_name(SEXP_datatypePtr_t *p);

void SEXP_datatype_once(void);
void SEXP_datatypeGlobalTbl_init(void);
void SEXP_datatypeGlobalTbl_free(void);

SEXP_datatypeTbl_t *SEXP_datatypeTbl_new (void);
int SEXP_datatypeTbl_init(SEXP_datatypeTbl_t *t);
void SEXP_datatypeTbl_free(SEXP_datatypeTbl_t *t);

SEXP_datatypePtr_t *SEXP_datatype_get(SEXP_datatypeTbl_t *t, const char *name);
SEXP_datatypePtr_t *SEXP_datatype_add(SEXP_datatypeTbl_t *t, char *name);
int SEXP_datatype_del(SEXP_datatypeTbl_t *t, const char *name);

#define SEXP_OP_CONTINUE 0 /* continue with the default handler */
#define SEXP_OP_ABORT    1 /* abort with an error status */
#define SEXP_OP_RETURN   2 /* return success, don't execute default handler */

#define SEXP_DATATYPEPTR_MASK  (UINTPTR_MAX << 2)
#define SEXP_DATATYPEPTR_ALIGN (4 > sizeof(void *) ? 4 : sizeof (void *))

extern SEXP_datatypeTbl_t g_datatypes;

int SEXP_datatype_register(SEXP_datatypeTbl_t *t, const char *datatype);

int SEXP_datatype_op_safe(const char *datatype, uint8_t op, const SEXP_t *sexp, void *res, ...);

SEXP_datatype_t *SEXP_datatype_new(void);
int SEXP_datatype_setflag(SEXP_datatype_t **dp, uint16_t flag, ...);
int SEXP_datatype_unsetflag(SEXP_datatype_t **dp, uint16_t flag);
int SEXP_datatype_addop(SEXP_datatype_t **dp, int opnum, SEXP_datatypeOP_t *op);
int SEXP_datatype_delop(SEXP_datatype_t **dp, int opnum);

#endif /* _SEXP_DATATYPE */
