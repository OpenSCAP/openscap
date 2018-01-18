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
#ifndef SEXP_DATATYPE
#define SEXP_DATATYPE

#include <stdarg.h>
#include <stdint.h>
#include <sexp.h>
#include "oscap_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SEXP_datatypeOP_t) (SEXP_t *, const SEXP_t *, void *);

typedef struct SEXP_datatype    SEXP_datatype_t;
typedef struct SEXP_datatypeTbl SEXP_datatypeTbl_t;

extern SEXP_datatypeTbl_t g_datatypes;

OSCAP_API /* const char *SEXP_datatype (const SEXP_t *sexp); */

OSCAP_API int SEXP_datatype_register (SEXP_datatypeTbl_t *t, const char *datatype);
OSCAP_API /* int SEXP_datatype_unregister (void); */

OSCAP_API int SEXP_datatype_op (uint8_t op, const SEXP_t *sexp, void *res, ...);
OSCAP_API int SEXP_datatype_op_safe (const char *datatype, uint8_t op, const SEXP_t *sexp, void *res, ...);

OSCAP_API SEXP_datatype_t *SEXP_datatype_new(void);
OSCAP_API int SEXP_datatype_setflag(SEXP_datatype_t **dp, uint16_t flag, ...);
OSCAP_API int SEXP_datatype_unsetflag(SEXP_datatype_t **dp, uint16_t flag);
OSCAP_API int SEXP_datatype_addop(SEXP_datatype_t **dp, int opnum, SEXP_datatypeOP_t *op);
OSCAP_API int SEXP_datatype_delop(SEXP_datatype_t **dp, int opnum);

#ifdef __cplusplus
}
#endif

#endif /* SEXP_DATATYPE */
