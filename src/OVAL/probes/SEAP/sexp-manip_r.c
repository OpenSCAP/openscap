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

#include "config.h"

#include <string.h>
#include <errno.h>
#include "common/assume.h"
#include "public/sm_alloc.h"
#include "_sexp-types.h"
#include "_sexp-value.h"
#include "_sexp-rawptr.h"
#include "public/sexp-manip_r.h"

SEXP_t *SEXP_init(SEXP_t *sexp_mem)
{
        if (sexp_mem == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        sexp_mem->s_type = NULL;
        sexp_mem->s_valp = 0;

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        sexp_mem->__magic0 = SEXP_MAGIC0;
        sexp_mem->__magic1 = SEXP_MAGIC1;
#endif
        return (sexp_mem);
}

SEXP_t *SEXP_number_newi_32_r(SEXP_t *sexp_mem, int32_t n)
{
        SEXP_val_t v_dsc;

        if (sexp_mem == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (int32_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }

        SEXP_NCASTP(i32,v_dsc.mem)->t = SEXP_NUM_INT32;
        SEXP_NCASTP(i32,v_dsc.mem)->n = n;

        SEXP_init(sexp_mem);
        sexp_mem->s_type = NULL;
        sexp_mem->s_valp = v_dsc.ptr;

        return (sexp_mem);
}

SEXP_t *SEXP_number_newu_32_r(SEXP_t *sexp_mem, uint32_t n)
{
        SEXP_val_t v_dsc;

        if (sexp_mem == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (uint32_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }

        SEXP_NCASTP(u32,v_dsc.mem)->t = SEXP_NUM_UINT32;
        SEXP_NCASTP(u32,v_dsc.mem)->n = n;

        SEXP_init(sexp_mem);
        sexp_mem->s_type = NULL;
        sexp_mem->s_valp = v_dsc.ptr;

        return (sexp_mem);
}

SEXP_t *SEXP_number_newu_64_r(SEXP_t *sexp_mem, uint64_t n)
{
        SEXP_val_t v_dsc;

        if (sexp_mem == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (uint64_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }

        SEXP_NCASTP(u64,v_dsc.mem)->t = SEXP_NUM_UINT64;
        SEXP_NCASTP(u64,v_dsc.mem)->n = n;

        SEXP_init(sexp_mem);
        sexp_mem->s_type = NULL;
        sexp_mem->s_valp = v_dsc.ptr;

        return (sexp_mem);
}

SEXP_t *SEXP_number_newi_64_r(SEXP_t *sexp_mem, int64_t n)
{
        SEXP_val_t v_dsc;

        if (sexp_mem == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (int64_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }

        SEXP_NCASTP(i64,v_dsc.mem)->t = SEXP_NUM_INT64;
        SEXP_NCASTP(i64,v_dsc.mem)->n = n;

        SEXP_init(sexp_mem);
        sexp_mem->s_type = NULL;
        sexp_mem->s_valp = v_dsc.ptr;

        return (sexp_mem);
}

SEXP_t *SEXP_number_newf_r(SEXP_t *sexp_mem, double n)
{
        SEXP_val_t v_dsc;

        if (sexp_mem == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (double),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }

        SEXP_NCASTP(f,v_dsc.mem)->t = SEXP_NUM_DOUBLE;
        SEXP_NCASTP(f,v_dsc.mem)->n = n;

        SEXP_init(sexp_mem);
        sexp_mem->s_type = NULL;
        sexp_mem->s_valp = v_dsc.ptr;

        return (sexp_mem);
}

SEXP_t *SEXP_number_newb_r(SEXP_t *sexp_mem, bool n)
{
        SEXP_val_t v_dsc;

        if (sexp_mem == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (bool),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }

        SEXP_NCASTP(b,v_dsc.mem)->t = SEXP_NUM_BOOL;
        SEXP_NCASTP(b,v_dsc.mem)->n = n;

        SEXP_init(sexp_mem);
        sexp_mem->s_type = NULL;
        sexp_mem->s_valp = v_dsc.ptr;

        return (sexp_mem);
}

SEXP_t *SEXP_string_new_r  (SEXP_t *sexp_mem, const void *string, size_t length)
{
        SEXP_val_t v_dsc;

        if (sexp_mem == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        if (SEXP_val_new (&v_dsc, sizeof (char) * length,
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }

        memcpy (v_dsc.mem, string, sizeof (char) * length);

        SEXP_init(sexp_mem);
        sexp_mem->s_type = NULL;
        sexp_mem->s_valp = v_dsc.ptr;

        return (sexp_mem);
}

SEXP_t *SEXP_string_newf_r(SEXP_t *sexp_mem, const char *format, ...)
{
        va_list ap;
        SEXP_t *se;

        va_start(ap, format);
        se = SEXP_string_newf_rv(sexp_mem, format, ap);
        va_end(ap);

        return (se);
}

SEXP_t *SEXP_string_newf_rv(SEXP_t *sexp_mem, const char *format, va_list ap)
{
        SEXP_val_t v_dsc;
        char      *v_string;
        int        v_strlen;

        if (sexp_mem == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        v_strlen = vasprintf (&v_string, format, ap);

        if (v_strlen < 0) {
                /* TODO: handle this */
                return (NULL);
        }

        if (SEXP_val_new (&v_dsc, sizeof (char) * v_strlen,
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }

        memcpy  (v_dsc.mem, v_string, sizeof (char) * v_strlen);
        sm_free (v_string);

        SEXP_init(sexp_mem);
        sexp_mem->s_type = NULL;
        sexp_mem->s_valp = v_dsc.ptr;

        return (sexp_mem);
}

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

int SEXP_unref_r(SEXP_t *s_exp)
{
        if (SEXP_refs(s_exp) != 1)
                return (-1);
        else
                SEXP_rawval_decref(s_exp->s_valp);
        return (0);
}

#if defined(NDEBUG)
void SEXP_free_r (SEXP_t *s_exp)
#else
void __SEXP_free_r (SEXP_t *s_exp, const char *file, uint32_t line, const char *func)
#endif
{
#if !defined(NDEBUG) && defined(SEAP_VERBOSE_DEBUG)
        dI("s_exp=%p (%s:%u:%s)", s_exp, file, line, func);
#endif
        if (s_exp == NULL)
                return;

	if (!SEXP_softrefp(s_exp) && SEXP_typeof(s_exp) != SEXP_TYPE_EMPTY) {
                SEXP_val_t v_dsc;

                SEXP_VALIDATE(s_exp);
                SEXP_val_dsc (&v_dsc, s_exp->s_valp);

                if (SEXP_rawval_decref (s_exp->s_valp)) {
                        switch (v_dsc.type) {
                        case SEXP_VALTYPE_STRING:
                                sm_free (v_dsc.hdr);
                                break;
                        case SEXP_VALTYPE_NUMBER:
                                sm_free (v_dsc.hdr);
                                break;
                        case SEXP_VALTYPE_LIST:
                                if (SEXP_LCASTP(v_dsc.mem)->b_addr != NULL)
                                        SEXP_rawval_lblk_free ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr, SEXP_free_r);

                                sm_free (v_dsc.hdr);
                                break;
                        default:
                                abort ();
                        }
                }
        }

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        s_exp->s_valp = 0;
        s_exp->s_type = NULL;
        s_exp->__magic0 = SEXP_MAGIC0_INV;
        s_exp->__magic1 = SEXP_MAGIC1_INV;
#endif
        return;
}
