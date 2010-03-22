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
#ifndef SEXP_PARSER_H
#define SEXP_PARSER_H

#include <stddef.h>
#include <stdint.h>
#include "public/sexp-parser.h"
#include "_sexp-manip.h"
#include "generic/spb.h"

OSCAP_HIDDEN_START;

/* Number classes */
#define SEXP_NUMCLASS_INV  0
#define SEXP_NUMCLASS_INT  1
#define SEXP_NUMCLASS_UINT 2
#define SEXP_NUMCLASS_FLT  3
#define SEXP_NUMCLASS_EXP  4
#define SEXP_NUMCLASS_FRA  5
#define SEXP_NUMCLASS_PRE  6

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

        uint8_t       p_numclass;
        uint8_t       p_numbase;
        uint8_t       p_numstage;
        int8_t        p_numsign;
        
        /*
         * Output data
         */
        SEXP_lstack_t l_stack;
};

struct SEXP_psetup {
        SEXP_format_t p_format; /* expected or required format (depends on p_flags) */
        SEXP_pflags_t p_flags;  /* initial parser flags */
};

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

OSCAP_HIDDEN_END;

#endif /* SEXP_PARSER_H */
