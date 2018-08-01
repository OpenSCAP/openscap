/**
 * @file   sexp-ID.c
 * @brief  S-expression object ID functions - implementation
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * @addtogroup SEXPRESSIONS
 * @{
 */

/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *      Daniel Kopecek <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "_sexp-types.h"
#include "_sexp-value.h"
#include "_sexp-manip.h"
#include "_sexp-rawptr.h"
#include "_sexp-ID.h"

#include "MurmurHash3.h"

static SEXP_ID_t SEXP_ID_hash(void *buf, size_t len, SEXP_ID_t seed, int part)
{
        uint64_t resbuf[2];

	if (part != 0 && part != 1) {
		return -1;
	}

        MurmurHash3_x86_128(buf, (int)len, (uint32_t)((0x7C0FFEE7 ^ seed) ^ (seed >> 32)), resbuf);

        return (resbuf[part]);
}

static int SEXP_ID_v_callback(const SEXP_t *sexp, __IDres_pair *pair)
{
        SEXP_val_t v_dsc;

	if (sexp == NULL || pair == NULL) {
		return -1;
	}

        /*
         * Fill v_dsc with metainformation
         */
        SEXP_val_dsc(&v_dsc, sexp->s_valp);

        switch (v_dsc.type) {
        case SEXP_VALTYPE_NUMBER:
        case SEXP_VALTYPE_STRING:
                pair->hash = SEXP_ID_hash(v_dsc.mem, v_dsc.hdr->size, pair->hash, pair->part);
                break;
        case SEXP_VALTYPE_LIST:
        {
                SEXP_rawval_lblk_cb ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                     (int (*)(SEXP_t *, void *)) SEXP_ID_v_callback,
                                     (void *) pair,
                                     SEXP_LCASTP(v_dsc.mem)->offset + 1);

                pair->hash = SEXP_ID_hash(&pair->hash, 1,
					  pair->hash + SEXP_LCASTP(v_dsc.mem)->offset, pair->part);
                break;
        }
        case SEXP_VALTYPE_EMPTY:
                pair->hash = SEXP_ID_hash(&pair->hash, 1, pair->hash, pair->part);
                break;
        default:
                /* Unknown S-exp value type */
                abort ();
        }

        return (0);
}

SEXP_ID_t SEXP_ID_v(const SEXP_t *s)
{
	__IDres_pair pair;

        pair.hash = 0xAD30917100C0FFEE;
	pair.part = 0;

        SEXP_ID_v_callback(s, &pair);

        return (pair.hash);
}

/// @}
