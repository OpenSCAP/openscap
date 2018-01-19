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
#ifndef _SEXP_VALUE_H
#define _SEXP_VALUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "_sexp-types.h"
#include "../../../common/util.h"


typedef uint8_t SEXP_valtype_t;

#define SEXP_VALTYPE_EMPTY  0
#define SEXP_VALTYPE_STRING 1
#define SEXP_VALTYPE_NUMBER 2
#define SEXP_VALTYPE_LIST   3

typedef struct {
        uint32_t refs;
        size_t   size;
} __attribute__ ((packed)) SEXP_valhdr_t;

typedef struct {
        uintptr_t      ptr;
        SEXP_valhdr_t *hdr;
        void          *mem;
        SEXP_valtype_t type;
} SEXP_val_t;

#define SEXP_VALP_ALIGN (4 > sizeof(void *) ? 4 : sizeof(void *))
#define SEXP_VALP_MASK  (UINTPTR_MAX << 2)
#define SEXP_VALT_MASK  3
#define SEXP_VALP_HDR(p) ((SEXP_valhdr_t *)(((uintptr_t)(p)) & SEXP_VALP_MASK))

int       SEXP_val_new (SEXP_val_t *dst, size_t vmemsize, SEXP_valtype_t type);
void      SEXP_val_dsc (SEXP_val_t *dst, uintptr_t ptr);
uintptr_t SEXP_val_ptr (SEXP_val_t *dsc);

uintptr_t SEXP_rawval_incref (uintptr_t valp);
int       SEXP_rawval_decref (uintptr_t valp);

#define SEXP_DEFNUM(s,T)   struct SEXP_val_num_##s { T n; SEXP_numtype_t t; } __attribute__ ((packed))
#define SEXP_NCASTP(s,p) ((struct SEXP_val_num_##s *)(p))
#define SEXP_NTYPEP(sz,p) *((SEXP_numtype_t *)(((uint8_t *)(p)) + (sz) - sizeof (SEXP_numtype_t)))

SEXP_numtype_t SEXP_rawval_number_type (SEXP_val_t *dsc);

SEXP_DEFNUM(b, bool);
SEXP_DEFNUM(f, double);
SEXP_DEFNUM(i8,   int8_t);
SEXP_DEFNUM(u8,  uint8_t);
SEXP_DEFNUM(i16,  int16_t);
SEXP_DEFNUM(u16, uint16_t);
SEXP_DEFNUM(i32,  int32_t);
SEXP_DEFNUM(u32, uint32_t);
SEXP_DEFNUM(i64,  int64_t);
SEXP_DEFNUM(u64, uint64_t);

/*
 * List
 */

struct SEXP_val_list {
        void    *b_addr;
        uint16_t offset;
} __attribute__ ((packed));

#define SEXP_LCASTP(p) ((struct SEXP_val_list *)(p))

struct SEXP_val_lblk {
        uintptr_t nxsz;
        uint16_t  real;
        uint16_t  refs;
        SEXP_t    memb[];
} __attribute__ ((packed));

size_t    SEXP_rawval_list_length (struct SEXP_val_list *list);
uintptr_t SEXP_rawval_list_copy (uintptr_t s_valp);

uintptr_t SEXP_rawval_lblk_copy (uintptr_t lblkp, uint16_t n_skip);
uintptr_t SEXP_rawval_lblk_new  (uint8_t sz);
uintptr_t SEXP_rawval_lblk_incref (uintptr_t lblkp);
int       SEXP_rawval_lblk_decref (uintptr_t lblkp);

uintptr_t SEXP_rawval_lblk_fill (uintptr_t lblkp, SEXP_t *s_exp[], uint16_t s_exp_count);
uintptr_t SEXP_rawval_lblk_add  (uintptr_t lblkp, const SEXP_t *s_exp);
uintptr_t SEXP_rawval_lblk_add1 (uintptr_t lblkp, const SEXP_t *s_exp);
uintptr_t SEXP_rawval_lblk_last (uintptr_t lblkp);
SEXP_t   *SEXP_rawval_lblk_nth  (uintptr_t lblkp, uint32_t n);
uintptr_t SEXP_rawval_lblk_replace (uintptr_t lblkp, uint32_t n, const SEXP_t *n_val, SEXP_t **o_val);
int       SEXP_rawval_lblk_cb   (uintptr_t lblkp, int  (*func) (SEXP_t *, void *), void *arg, uint32_t n);
void      SEXP_rawval_lblk_free (uintptr_t lblkp, void (*func) (SEXP_t *));
void      SEXP_rawval_lblk_free1 (uintptr_t lblkp, void (*func) (SEXP_t *));

#define SEXP_LBLK_ALIGN (16 > sizeof(void *) ? 16 : sizeof(void *))
#define SEXP_LBLKP_MASK (UINTPTR_MAX << 4)
#define SEXP_LBLKS_MASK 0x0f

#define SEXP_VALP_LBLK(valp) ((struct SEXP_val_lblk *)((uintptr_t)(valp) & SEXP_LBLKP_MASK))

uintptr_t SEXP_rawval_copy(uintptr_t s_valp);


#endif /* _SEXP_VALUE_H */
