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
#ifndef SEXP_HANDLER_H
#define SEXP_HANDLER_H

#if defined(SEAP_THREAD_SAFE)
# include <pthread.h>
#endif

#include <stdint.h>
#include <stddef.h>
#include "_sexp-datatype.h"
#include "../../../common/util.h"


typedef struct {
        char    *typestr;
        uint16_t typelen;

        SEXP_dtype_op_t *op;
        size_t op_cnt;

} SEXP_handler_t;

#include "generic/redblack.h"
DEFRBTREE(handlers, SEXP_handler_t handler);

typedef struct {
#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_t rwlock;
#endif
        TREETYPE(handlers) tree;
        uint8_t      init;
} SEXP_handlertbl_t;

extern SEXP_handlertbl_t gSEXP_handlers;

void SEXP_handlertbl_init (SEXP_handlertbl_t *htbl);
SEXP_handler_t *SEXP_gethandler (SEXP_handlertbl_t *htbl, const char *typestr, size_t typelen);
SEXP_handler_t *SEXP_reghandler (SEXP_handlertbl_t *htbl, SEXP_handler_t *handler);
int SEXP_delhandler (SEXP_handlertbl_t *htbl, const char *typestr, size_t typelen);

#define SEXP_gethandler_g(s, l) SEXP_gethandler (&gSEXP_handlers, s, l)
#define SEXP_reghandler_g(h)    SEXP_reghandler (&gSEXP_handlers, h)
#define SEXP_delhandler_g(s, l) SEXP_delhandler (&gSEXP_handlers, s, l)


#endif /* SEXP_HANDLER_H */
