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
#ifndef SEXP_PARSER_H
#define SEXP_PARSER_H

#include <stddef.h>
#include <stdint.h>
#include "public/sexp-parser.h"
#include "_sexp-manip.h"
#include "generic/spb.h"


/* Number classes */
#define SEXP_NUMCLASS_INV  0
#define SEXP_NUMCLASS_INT  1
#define SEXP_NUMCLASS_UINT 2
#define SEXP_NUMCLASS_FLT  3
#define SEXP_NUMCLASS_EXP  4
#define SEXP_NUMCLASS_FRA  5
#define SEXP_NUMCLASS_PRE  6

#define SEXP_PFUNC_COUNT 8

/*
 * Parser state
 */
struct SEXP_pstate {
        /*
         * Input data
         */
        spb_t        *p_buffer; /* input buffer */
        spb_size_t    p_bufoff; /* start index - everything before this index in the buffer is already parsed */
        spb_size_t    p_explen; /* length of already parsed/checked part of the expression */
        SEXP_pflags_t p_flags;  /* current parser flags */
        SEXP_t       *p_sexp;   /* last S-exp object */
        SEXP_pflags_t p_flags0; /* initial parser flags */

        void         *sp_data;          /* subparser data */
        void        (*sp_free)(void *); /* function for freeing the subparser data */

        void         *sp_shptr[SEXP_PFUNC_COUNT]; /* subparser shared pointer */
        void        (*sp_shfree[SEXP_PFUNC_COUNT])(void *); /* functions for freeing subparser shared pointer */

        uint8_t       p_label;  /* where to jump if p_explen > 0 */

        uint8_t       p_numclass; /* number class */
        uint8_t       p_numbase;  /* number base */
        uint8_t       p_numstage; /* number parsing stage */

        /*
         * Output data
         */
        SEXP_lstack_t l_stack; /* output list stack */

        /*
         * Value cache
         */
        uintptr_t v_bool[2]; /* true, false pre-allocated values */
	uint8_t   p_error;
};

struct SEXP_pext_dsc {
        spb_t        *p_buffer;
        spb_size_t    p_bufoff;
        spb_size_t    p_explen;
        SEXP_pflags_t p_flags;
        SEXP_t       *s_exp;
        void         *sp_data;          /* subparser data */
        void        (*sp_free)(void *); /* function for freeing the subparser data */
        void         *sp_shptr[SEXP_PFUNC_COUNT];
        void        (*sp_shfree[SEXP_PFUNC_COUNT])(void *);

        uint8_t       p_label;
        uint8_t       p_numclass;
        uint8_t       p_numbase;
        uint8_t       p_numstage;

        uintptr_t    *v_bool;
};

#define PEXT_DSC_INITIALIZER { NULL, 0, 0, NULL }

#define __PARSE_RT int
#define __PARSE_PT(n1) struct SEXP_pext_dsc *n1

typedef __PARSE_RT (SEXP_pfunc_t)(__PARSE_PT());


struct SEXP_psetup {
        SEXP_format_t  p_format; /* expected or required format (depends on p_flags) */
        SEXP_pflags_t  p_flags;  /* initial parser flags */
        SEXP_pfunc_t  *p_funcp[SEXP_PFUNC_COUNT];
};

#define SEXP_PFUNC_UL_STRING_SI  0
#define SEXP_PFUNC_UL_STRING_DQ  1
#define SEXP_PFUNC_UL_STRING_SQ  2
#define SEXP_PFUNC_KL_STRING     3
#define SEXP_PFUNC_UL_STRING_B64 4
#define SEXP_PFUNC_KL_STRING_B64 5
#define SEXP_PFUNC_UL_DATATYPE   6
#define SEXP_PFUNC_KL_DATATYPE   7
#define SEXP_PFUNC_BOOL          8

__PARSE_RT SEXP_parse_ul_string_si  (__PARSE_PT(dsc));
__PARSE_RT SEXP_parse_ul_string_dq  (__PARSE_PT(dsc));
__PARSE_RT SEXP_parse_ul_string_sq  (__PARSE_PT(dsc));
__PARSE_RT SEXP_parse_kl_string     (__PARSE_PT(dsc));
__PARSE_RT SEXP_parse_ul_string_b64 (__PARSE_PT(dsc));
__PARSE_RT SEXP_parse_kl_string_b64 (__PARSE_PT(dsc));
__PARSE_RT SEXP_parse_ul_datatype   (__PARSE_PT(dsc));
__PARSE_RT SEXP_parse_kl_datatype   (__PARSE_PT(dsc));
__PARSE_RT SEXP_parse_bool          (__PARSE_PT(dsc), bool val);

#define SEXP_PSLOT_MAX 1024

#define SEXP_PRET_SUCCESS 0
#define SEXP_PRET_EUNFIN  1   /* incomplete token */
#define SEXP_PRET_EINVAL  2   /* syntax error, invalid token */
#define SEXP_PRET_EUNDEF  255 /* unknown, unexpected error */

#ifdef __GNUC__
# define __predict(expr, v) __builtin_expect (expr, v)
#else
# define __predict(expr, v) expr
#endif /* __GNUC__ */

/*
 * TODO: make this function public in the future
 */
int SEXP_psetup_setpfunc(SEXP_psetup_t *psetup, int pfunctype, SEXP_pfunc_t *pfunc);


#endif /* SEXP_PARSER_H */
