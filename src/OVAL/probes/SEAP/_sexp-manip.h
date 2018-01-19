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
#ifndef _SEXP_MANIP_H
#define _SEXP_MANIP_H

#include "public/sexp-manip.h"
#include "../../../common/util.h"


#define SEXP_LSTACK_INIT_SIZE      8
#define SEXP_LSTACK_GROWFAST_TRESH 64
#define SEXP_LSTACK_GROWSLOW_DIFF  8

typedef struct {
        SEXP_t   *p_list;
        size_t    l_size;
        size_t    l_real;
        SEXP_t  **l_sref; /* stack of soft refs to lists */
} SEXP_lstack_t;

SEXP_lstack_t *SEXP_lstack_new (void);
void SEXP_lstack_init (SEXP_lstack_t *stack);
void SEXP_lstack_free (SEXP_lstack_t *stack);
void SEXP_lstack_destroy (SEXP_lstack_t *stack);

SEXP_t *SEXP_lstack_push (SEXP_lstack_t *stack, SEXP_t *ref);
SEXP_t *SEXP_lstack_pop (SEXP_lstack_t *stack);
SEXP_t *SEXP_lstack_top (const SEXP_lstack_t *stack);
SEXP_t *SEXP_lstack_list (SEXP_lstack_t *stack);
size_t  SEXP_lstack_depth (SEXP_lstack_t *stack);


#endif /* _SEXP_MANIP_H */
