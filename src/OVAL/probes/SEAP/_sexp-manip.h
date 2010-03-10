
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
#ifndef _SEXP_MANIP_H
#define _SEXP_MANIP_H

#include "public/sexp-manip.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

#define SEXP_LSTACK_INIT_SIZE      32
#define SEXP_LSTACK_GROWFAST_TRESH 512
#define SEXP_LSTACK_GROWSLOW_DIFF  32

typedef struct {
        size_t   l_size;
        size_t   l_real;
        SEXP_t **l_sref;
} SEXP_lstack_t;

SEXP_lstack_t *SEXP_lstack_new (void);
SEXP_t *SEXP_lstack_push (SEXP_lstack_t *stack, const SEXP_t *ref);
SEXP_t *SEXP_lstack_pop (SEXP_lstack_t *stack);
SEXP_t *SEXP_lstack_top (const SEXP_lstack_t *stack);

struct SEXP_it {
        SEXP_t *s_ref;
        
};

OSCAP_HIDDEN_END;

#endif /* _SEXP_MANIP_H */
