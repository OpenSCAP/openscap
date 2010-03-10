
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
#ifndef _SEXP_PARSE_H
#define _SEXP_PARSE_H

#include <stddef.h>
#include <stdint.h>
#include "public/sexp-parse.h"
#include "_sexp-manip.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

/* Number classes */
#define NUMCLASS_INV 0
#define NUMCLASS_INT 1
#define NUMCLASS_FLT 2
#define NUMCLASS_FRA 3
#define NUMCLASS_EXP 4

typedef uint8_t SEXP_pflags_t;

#define PF_EOFOK 0x01
#define PF_ALL   0xff

/* S-expression parsing state */
struct SEXP_pstate {
        char    *buffer;
        size_t   buffer_data_len;
        size_t   buffer_fail_off;

        SEXP_pflags_t pflags;
        
        SEXP_t   *p_list;
        size_t    l_size;
        size_t    l_real;
        SEXP_t  **l_sref; /* stack of soft refs to lists */
};

struct SEXP_psetup {
        SEXP_format_t fmt;
        SEXP_pflags_t pflags;
};

void SEXP_psetup_init (SEXP_psetup_t *psetup);

struct SEXP_pext_dsc {
        SEXP_t     *s_exp;
        const char *t_beg;
        size_t      t_len;
        size_t      b_len;
        uint8_t     flags;
};

#define SEXP_EXT_SUCCESS 0
#define SEXP_EXT_EUNFIN  1
#define SEXP_EXT_EINVAL  2
#define SEXP_EXT_EUNDEF  255

OSCAP_HIDDEN_END;

#endif /* _SEXP_PARSE_H */
