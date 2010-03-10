
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
#ifndef _SEXP_TYPES_H
#define _SEXP_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "public/sexp-types.h"
#include "_sexp-datatype.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

/* Magic numbers */
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
# define SEXP_MAGIC0     0xf3f3
# define SEXP_MAGIC0_INV 0xffff
# define SEXP_MAGIC1     0x6767
# define SEXP_MAGIC1_INV 0x0000
#endif

struct SEXP {
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        volatile uint16_t __magic0;
#endif

        SEXP_datatype_t *s_type;
        uintptr_t        s_valp;
        uint8_t          s_flgs;

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        volatile uint16_t __magic1;
#endif
};

#define SEXP_FLAG_SREF  0x01
#define SEXP_FLAG_INVAL 0x02
#define SEXP_FLAG_UNFIN 0x04

static inline void SEXP_flag_set (SEXP_t *s_exp, uint8_t flag)
{
        s_exp->s_flgs |= flag;
}

static inline void SEXP_flag_unset (SEXP_t *s_exp, uint8_t flag)
{
        s_exp->s_flgs &= ~flag;
}

static inline bool SEXP_flag_isset (SEXP_t *s_exp, uint8_t flag)
{
        return ((s_exp->s_flgs & flag) == flag);
}

OSCAP_HIDDEN_END;

#endif /* _SEXP_TYPES_H */
