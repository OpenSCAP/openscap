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

#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "_sexp-types.h"
#include "_sexp-value.h"
#include "_sexp-rawptr.h"
#include "_sexp-manip.h"
#include "_sexp-manip_r.h"
#include "debug_priv.h"
#include "_sexp-list_r.h"

SEXP_t *SEXP_list_new_rv (SEXP_t *sexp_mem, SEXP_t *memb, va_list alist)
{
        SEXP_val_t v_dsc;
        SEXP_t    *s_ptr[32];
        size_t     s_cur;
        uint8_t    b_exp;

        s_cur = 0;
        s_ptr[s_cur] = memb;

        while (s_ptr[s_cur] != NULL) {
                _A(s_cur < (sizeof s_ptr / sizeof (SEXP_t *)));
                SEXP_VALIDATE(s_ptr[s_cur]);

                s_ptr[++s_cur] = va_arg (alist, SEXP_t *);
        }

        if (SEXP_val_new (&v_dsc, sizeof (void *) + sizeof (uint16_t),
                          SEXP_VALTYPE_LIST) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }

        if (s_cur > 0) {
                for (b_exp = 0; (size_t)(1 << b_exp) < s_cur; ++b_exp);

                SEXP_LCASTP(v_dsc.mem)->offset = 0;
                SEXP_LCASTP(v_dsc.mem)->b_addr = (void *)SEXP_rawval_lblk_new (b_exp);

                if (SEXP_rawval_lblk_fill ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                           s_ptr, s_cur) != ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr))
                {
                        /* TODO: handle this */
                        return (NULL);
                }
        } else {
                SEXP_LCASTP(v_dsc.mem)->offset = 0;
                SEXP_LCASTP(v_dsc.mem)->b_addr = NULL;
        }

        SEXP_init(sexp_mem);
        sexp_mem->s_type = NULL;
        sexp_mem->s_valp = v_dsc.ptr;

        SEXP_VALIDATE(sexp_mem);

        return (sexp_mem);
}

SEXP_t *SEXP_list_new_r (SEXP_t *sexp_mem, SEXP_t *memb, ...)
{
        va_list ap;
        SEXP_t *ret;

        va_start(ap, memb);
        ret = SEXP_list_new_rv(sexp_mem, memb, ap);
        va_end(ap);

        return (ret);
}

SEXP_t *SEXP_list_rest_r (SEXP_t *rest, const SEXP_t *list)
{
        SEXP_val_t v_dsc_o, v_dsc_r;
        struct SEXP_val_lblk *lblk;

	if (rest == NULL) {
		errno = EINVAL;
		return (NULL);
	}

        if (list == NULL) {
                errno = EINVAL;
                return (NULL);
        }

        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc_o, list->s_valp);

        if (v_dsc_o.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        if (SEXP_val_new (&v_dsc_r, sizeof (void *) + sizeof (uint16_t),
                          SEXP_VALTYPE_LIST) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }

        SEXP_LCASTP(v_dsc_r.mem)->offset = SEXP_LCASTP(v_dsc_o.mem)->offset + 1;
        SEXP_LCASTP(v_dsc_r.mem)->b_addr = SEXP_LCASTP(v_dsc_o.mem)->b_addr;

        lblk = SEXP_VALP_LBLK(SEXP_LCASTP(v_dsc_r.mem)->b_addr);

        if (lblk != NULL) {
                if (SEXP_LCASTP(v_dsc_r.mem)->offset == lblk->real) {
                        SEXP_LCASTP(v_dsc_r.mem)->offset = 0;
                        SEXP_LCASTP(v_dsc_r.mem)->b_addr = SEXP_VALP_LBLK(lblk->nxsz);
                }

                if (SEXP_VALP_LBLK(SEXP_LCASTP(v_dsc_r.mem)->b_addr) != NULL)
                        SEXP_LCASTP(v_dsc_r.mem)->b_addr = (void *)SEXP_rawval_lblk_incref ((uintptr_t) SEXP_LCASTP(v_dsc_r.mem)->b_addr);
        }

        SEXP_init(rest);
        rest->s_type = NULL;
        rest->s_valp = SEXP_val_ptr (&v_dsc_r);

        SEXP_VALIDATE(rest);

        return (rest);
}

