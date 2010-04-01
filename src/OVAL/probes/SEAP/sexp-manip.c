/**
 * @file
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 */

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
 *      Daniel Kopecek <dkopecek@redhat.com>
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "_sexp-types.h"
#include "_sexp-value.h"
#include "_sexp-manip.h"
#include "public/sm_alloc.h"

static void SEXP_free_lmemb (SEXP_t *s_exp);

/*
 * Numbers
 */

SEXP_t *SEXP_number_new (SEXP_numtype_t t, const void *n)
{
        _LOGCALL_;

        if (t <= SEXP_NUM_UINT16) {
                if (t <= SEXP_NUM_INT8) {
                        switch (t) {
                        case SEXP_NUM_BOOL:  return SEXP_number_newb   (*(bool    *)(n));
                        case SEXP_NUM_INT8:  return SEXP_number_newi_8 (*(int8_t  *)(n));
                        case SEXP_NUM_UINT8: return SEXP_number_newu_8 (*(uint8_t *)(n));
                        }
                } else {
                        switch (t) {
                        case SEXP_NUM_INT16:  return SEXP_number_newi_16 (*(int16_t  *)(n));
                        case SEXP_NUM_UINT16: return SEXP_number_newu_16 (*(uint16_t *)(n));
                        }
                }
        } else {
                if (t <= SEXP_NUM_INT64) {
                        switch (t) {
                        case SEXP_NUM_INT32:  return SEXP_number_newi_32 (*(int32_t  *)(n));
                        case SEXP_NUM_UINT32: return SEXP_number_newu_32 (*(uint32_t *)(n));
                        case SEXP_NUM_INT64:  return SEXP_number_newi_64 (*(int64_t  *)(n));
                        }
                } else {
                        switch (t) {
                        case SEXP_NUM_UINT64: return SEXP_number_newu_64 (*(uint64_t *)(n));
                        case SEXP_NUM_DOUBLE: return SEXP_number_newf    (*(double   *)(n));
                        }
                }
        }
        
        errno = EINVAL;
        return  (NULL);
}

SEXP_t *SEXP_number_newb (bool n)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;

        _LOGCALL_;
        
        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (bool),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        SEXP_NCASTP(b,v_dsc.mem)->t = SEXP_NUM_BOOL;
        SEXP_NCASTP(b,v_dsc.mem)->n = n;
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

SEXP_t *SEXP_number_newi_8  (int8_t n)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;

        _LOGCALL_;
        
        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (int8_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        SEXP_NCASTP(i8,v_dsc.mem)->t = SEXP_NUM_INT8;
        SEXP_NCASTP(i8,v_dsc.mem)->n = n;
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

SEXP_t *SEXP_number_newu_8  (uint8_t n)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;

        _LOGCALL_;
        
        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (uint8_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        SEXP_NCASTP(u8,v_dsc.mem)->t = SEXP_NUM_UINT8;
        SEXP_NCASTP(u8,v_dsc.mem)->n = n;
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

uint8_t SEXP_number_getu_8 (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
        SEXP_numtype_t t;
        
        if (s_exp == NULL) {
                errno = EFAULT;
                return (UINT8_MAX);
        }

        SEXP_VALIDATE(s_exp);
                
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        t = SEXP_rawval_number_type (&v_dsc);
        
        if (t > SEXP_NUM_UINT8) {
                errno = EDOM;
                return (UINT8_MAX);
        }
        
        switch (t) {
        case SEXP_NUM_UINT8:
        case SEXP_NUM_INT8:
        case SEXP_NUM_BOOL:
                return (SEXP_NCASTP(u8,v_dsc.mem)->n);
        default:
                abort ();
        }
        
        return (UINT8_MAX);
}

SEXP_t *SEXP_number_newi_16 (int16_t n)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;

        _LOGCALL_;
        
        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (int16_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        SEXP_NCASTP(i16,v_dsc.mem)->t = SEXP_NUM_INT16;
        SEXP_NCASTP(i16,v_dsc.mem)->n = n;
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

SEXP_t *SEXP_number_newu_16 (uint16_t n)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;

        _LOGCALL_;
        
        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (uint16_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        SEXP_NCASTP(u16,v_dsc.mem)->t = SEXP_NUM_UINT16;
        SEXP_NCASTP(u16,v_dsc.mem)->n = n;
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

uint16_t SEXP_number_getu_16 (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
        SEXP_numtype_t t;
        
        if (s_exp == NULL) {
                errno = EFAULT;
                return (UINT16_MAX);
        }
        
        SEXP_VALIDATE(s_exp);
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        t = SEXP_rawval_number_type (&v_dsc);
        
        if (t > SEXP_NUM_UINT16) {
                errno = EDOM;
                return (UINT16_MAX);
        }
        
        switch (t) {
        case SEXP_NUM_UINT16:
        case SEXP_NUM_INT16:
                return (SEXP_NCASTP(u16,v_dsc.mem)->n);
        case SEXP_NUM_UINT8:
        case SEXP_NUM_INT8:
        case SEXP_NUM_BOOL:
                return ((uint16_t)SEXP_NCASTP(u8,v_dsc.mem)->n);
        default:
                abort ();
        }
        
        return (UINT16_MAX);
}

SEXP_t *SEXP_number_newi_32 (int32_t n)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;
        
        _LOGCALL_;

        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (int32_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        SEXP_NCASTP(i32,v_dsc.mem)->t = SEXP_NUM_INT32;
        SEXP_NCASTP(i32,v_dsc.mem)->n = n;
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

int32_t SEXP_number_geti_32 (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
        SEXP_numtype_t t;
        
        _LOGCALL_;
        
        if (s_exp == NULL) {
                errno = EFAULT;
                return (INT32_MAX);
        }
        
        SEXP_VALIDATE(s_exp);
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        t = SEXP_rawval_number_type (&v_dsc);
        
        if (t > SEXP_NUM_UINT32) {
                errno = EDOM;
                return (INT32_MAX);
        }
        
        switch (t) {
        case SEXP_NUM_INT32:
                return (SEXP_NCASTP(i32,v_dsc.mem)->n);
        case SEXP_NUM_UINT32:
                return ((int32_t)SEXP_NCASTP(u32,v_dsc.mem)->n);
        case SEXP_NUM_UINT16:
        case SEXP_NUM_INT16:
                return ((int32_t)SEXP_NCASTP(u16,v_dsc.mem)->n);
        case SEXP_NUM_UINT8:
        case SEXP_NUM_INT8:
        case SEXP_NUM_BOOL:
                return ((int32_t)SEXP_NCASTP(u8,v_dsc.mem)->n);
        default:
                abort ();
        }
        
        return (INT32_MAX);
}

SEXP_t *SEXP_number_newu_32 (uint32_t n)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;
        
        _LOGCALL_;

        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (uint32_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        SEXP_NCASTP(u32,v_dsc.mem)->t = SEXP_NUM_UINT32;
        SEXP_NCASTP(u32,v_dsc.mem)->n = n;
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

uint32_t SEXP_number_getu_32 (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
        SEXP_numtype_t t;
        
        _LOGCALL_;
        
        if (s_exp == NULL) {
                errno = EFAULT;
                return (UINT32_MAX);
        }
        
        SEXP_VALIDATE(s_exp);
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        t = SEXP_rawval_number_type (&v_dsc);
        
        if (t > SEXP_NUM_UINT32) {
                errno = EDOM;
                return (UINT32_MAX);
        }
        
        switch (t) {
        case SEXP_NUM_UINT32:
                return (SEXP_NCASTP(u32,v_dsc.mem)->n);
        case SEXP_NUM_INT32:
                return ((uint32_t)SEXP_NCASTP(i32,v_dsc.mem)->n);
        case SEXP_NUM_UINT16:
        case SEXP_NUM_INT16:
                return ((uint32_t)SEXP_NCASTP(u16,v_dsc.mem)->n);
        case SEXP_NUM_UINT8:
        case SEXP_NUM_INT8:
        case SEXP_NUM_BOOL:
                return ((uint32_t)SEXP_NCASTP(u8,v_dsc.mem)->n);
        default:
                abort ();
        }
        
        return (UINT32_MAX);
}

SEXP_t *SEXP_number_newi_64 (int64_t n)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;
        
        _LOGCALL_;

        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (int64_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        SEXP_NCASTP(i64,v_dsc.mem)->t = SEXP_NUM_INT64;
        SEXP_NCASTP(i64,v_dsc.mem)->n = n;
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

int64_t SEXP_number_geti_64 (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
        SEXP_numtype_t t;
        
        _LOGCALL_;
        
        if (s_exp == NULL) {
                errno = EFAULT;
                return (INT64_MAX);
        }
        
        SEXP_VALIDATE(s_exp);
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        t = SEXP_rawval_number_type (&v_dsc);
        
        if (t > SEXP_NUM_UINT64) {
                errno = EDOM;
                return (INT64_MAX);
        }
        
        switch (t) {
        case SEXP_NUM_INT64:
                return (SEXP_NCASTP(i64,v_dsc.mem)->n);
        case SEXP_NUM_UINT64:
                return ((int64_t)SEXP_NCASTP(u64,v_dsc.mem)->n);
        case SEXP_NUM_UINT32:
                return ((int64_t)SEXP_NCASTP(u32,v_dsc.mem)->n);
        case SEXP_NUM_INT32:
                return ((int64_t)SEXP_NCASTP(i32,v_dsc.mem)->n);
        case SEXP_NUM_UINT16:
        case SEXP_NUM_INT16:
                return ((int64_t)SEXP_NCASTP(u16,v_dsc.mem)->n);
        case SEXP_NUM_UINT8:
        case SEXP_NUM_INT8:
        case SEXP_NUM_BOOL:
                return ((int64_t)SEXP_NCASTP(u8,v_dsc.mem)->n);
        default:
                abort ();
        }
        
        return (INT64_MAX);
}

SEXP_t *SEXP_number_newu_64 (uint64_t n)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;

        _LOGCALL_;
        
        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (uint64_t),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        SEXP_NCASTP(u64,v_dsc.mem)->t = SEXP_NUM_UINT64;
        SEXP_NCASTP(u64,v_dsc.mem)->n = n;
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

uint64_t SEXP_number_getu_64 (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
        SEXP_numtype_t t;
        
        _LOGCALL_;
        
        if (s_exp == NULL) {
                errno = EFAULT;
                return (UINT64_MAX);
        }
        
        SEXP_VALIDATE(s_exp);
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        t = SEXP_rawval_number_type (&v_dsc);
        
        if (t > SEXP_NUM_UINT64) {
                errno = EDOM;
                return (UINT64_MAX);
        }
        
        switch (t) {
        case SEXP_NUM_UINT64:
                return (SEXP_NCASTP(u64,v_dsc.mem)->n);
        case SEXP_NUM_INT64:
                return ((uint64_t)SEXP_NCASTP(i64,v_dsc.mem)->n);
        case SEXP_NUM_UINT32:
                return ((uint64_t)SEXP_NCASTP(u32,v_dsc.mem)->n);
        case SEXP_NUM_INT32:
                return ((uint64_t)SEXP_NCASTP(i32,v_dsc.mem)->n);
        case SEXP_NUM_UINT16:
        case SEXP_NUM_INT16:
                return ((uint64_t)SEXP_NCASTP(u16,v_dsc.mem)->n);
        case SEXP_NUM_UINT8:
        case SEXP_NUM_INT8:
        case SEXP_NUM_BOOL:
                return ((uint64_t)SEXP_NCASTP(u8,v_dsc.mem)->n);
        default:
                abort ();
        }
        
        return (UINT64_MAX);
}

bool SEXP_number_getb (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
        SEXP_numtype_t t;
        
        _LOGCALL_;
        
        if (s_exp == NULL) {
                errno = EFAULT;
                return (false);
        }
        
        SEXP_VALIDATE(s_exp);
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        t = SEXP_rawval_number_type (&v_dsc);
        
        if (t > SEXP_NUM_UINT64) {
                errno = EDOM;
                return (false);
        }
        
        switch (t) {
        case SEXP_NUM_UINT64:
                return (SEXP_NCASTP(u64,v_dsc.mem)->n ? true : false);
        case SEXP_NUM_INT64:
                return (SEXP_NCASTP(i64,v_dsc.mem)->n ? true : false);
        case SEXP_NUM_UINT32:
                return (SEXP_NCASTP(u32,v_dsc.mem)->n ? true : false);
        case SEXP_NUM_INT32:
                return (SEXP_NCASTP(i32,v_dsc.mem)->n ? true : false);
        case SEXP_NUM_UINT16:
        case SEXP_NUM_INT16:
                return (SEXP_NCASTP(u16,v_dsc.mem)->n ? true : false);
        case SEXP_NUM_UINT8:
        case SEXP_NUM_INT8:
        case SEXP_NUM_BOOL:
                return (SEXP_NCASTP(u8,v_dsc.mem)->n ? true : false);
        default:
                abort ();
        }
        
        return (false);
}

SEXP_t *SEXP_number_newf (double n)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;

        _LOGCALL_;
        
        if (SEXP_val_new (&v_dsc, sizeof (SEXP_numtype_t) + sizeof (double),
                          SEXP_VALTYPE_NUMBER) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        SEXP_NCASTP(f,v_dsc.mem)->t = SEXP_NUM_DOUBLE;
        SEXP_NCASTP(f,v_dsc.mem)->n = n;
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

double SEXP_number_getf (const SEXP_t *s_exp)
{
        _LOGCALL_;
        abort ();
        SEXP_VALIDATE(s_exp);
        return (0);
}

int SEXP_number_get (const SEXP_t *s_exp, void *dst, SEXP_numtype_t type)
{
        _LOGCALL_;
        abort ();
        SEXP_VALIDATE(s_exp);
        return (-1);
}

void SEXP_number_free (SEXP_t *s_exp)
{
        _LOGCALL_;
        SEXP_VALIDATE(s_exp);
        SEXP_free (s_exp);
}

bool SEXP_numberp (const SEXP_t *s_exp)
{
        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (false);
        }
        
        SEXP_VALIDATE(s_exp);

        return ((s_exp->s_valp & SEXP_VALT_MASK) == SEXP_VALTYPE_NUMBER);
}

SEXP_numtype_t SEXP_number_type (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;

        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (SEXP_NUM_NONE);
        }

        SEXP_VALIDATE(s_exp);

        if (!SEXP_numberp (s_exp)) {
                errno = EINVAL;
                return (SEXP_NUM_NONE);
        }
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        
        return (*(SEXP_numtype_t *)((uint8_t *)(v_dsc.mem) + v_dsc.hdr->size - sizeof (SEXP_numtype_t)));
}

/*
 * Strings
 */

SEXP_t *SEXP_string_new  (const void *string, size_t length)
{
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;

        _LOGCALL_;

        if (SEXP_val_new (&v_dsc, sizeof (char) * length,
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        memcpy (v_dsc.mem, string, sizeof (char) * length);

        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

SEXP_t *SEXP_string_newf (const char *format, ...)
{
        va_list    alist;
        SEXP_t    *s_exp;
        SEXP_val_t v_dsc;
        char      *v_string;
        int        v_strlen;
        
        _A(format != NULL);
        _LOGCALL_;

        va_start (alist, format);
        v_strlen = vasprintf (&v_string, format, alist);

        _D("v_strlen = %d\n", v_strlen);
        
        if (v_strlen < 0) {
                /* TODO: handle this */
                return (NULL);
        }

        va_end (alist);
        
        if (SEXP_val_new (&v_dsc, sizeof (char) * v_strlen,
                          SEXP_VALTYPE_STRING) != 0)
        {
                /* TODO: handle this */
                return (NULL);
        }
        
        memcpy  (v_dsc.mem, v_string, sizeof (char) * v_strlen);
        sm_free (v_string);
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;
        
        return (s_exp);
}

void SEXP_string_free (SEXP_t *s_exp)
{
        _LOGCALL_;
        SEXP_VALIDATE(s_exp);
        SEXP_free (s_exp);
}

bool SEXP_stringp (const SEXP_t *s_exp)
{
        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (false);
        }

        SEXP_VALIDATE(s_exp);
        
        return ((s_exp->s_valp & SEXP_VALT_MASK) == SEXP_VALTYPE_STRING);
}

size_t SEXP_string_length (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;

        _LOGCALL_;
        
        if (s_exp == NULL) {
                errno = EFAULT;
                return ((size_t)(-1));
        }

        SEXP_VALIDATE(s_exp);

        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        
        if (v_dsc.type != SEXP_VALTYPE_STRING) {
                errno = EINVAL;
                return ((size_t)(-1));
        }
        
        return (v_dsc.hdr->size / sizeof (char));
}

int SEXP_strcmp (const SEXP_t *s_exp, const char *str)
{
        SEXP_val_t v_dsc;
        size_t     i;
        int        c;

        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (-1);
        }
        
        SEXP_VALIDATE(s_exp);

        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        
        if (v_dsc.type != SEXP_VALTYPE_STRING) {
                errno = EINVAL;
                return (-1);
        }
        
        for (i = 0; i < (v_dsc.hdr->size / sizeof (char)); ++i) {
                if (str[i] == '\0')
                        return (((unsigned char *)v_dsc.mem)[i] - '\0');
                
                c = ((unsigned char *)v_dsc.mem)[i] - ((unsigned char *)str)[i];

                if (c != 0)
                        return (c);
        }
        
        return ('\0' - str[i]);
}

int SEXP_strncmp (const SEXP_t *s_exp, const char *str, size_t n)
{
        SEXP_val_t v_dsc;
        size_t     i;
        int        c;
        
        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (-1);
        }
        
        SEXP_VALIDATE(s_exp);

        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        
        if (v_dsc.type != SEXP_VALTYPE_STRING) {
                errno = EINVAL;
                return (-1);
        }
        
        for (i = 0; i < (v_dsc.hdr->size / sizeof (char)); ++i) {
                if (str[i] == '\0')
                        return (((unsigned char *)v_dsc.mem)[i] - '\0');
                
                _D("%c ?= %c\n",  ((unsigned char *)v_dsc.mem)[i], ((unsigned char *)str)[i]);
                
                c = ((unsigned char *)v_dsc.mem)[i] - ((unsigned char *)str)[i];

                if (c != 0 || (i + 1) >= n) {
                        _D("=> %d\n", c);
                        return (c);
                }
        }
        
        return ('\0' - str[i]);
}

int SEXP_string_nth (const SEXP_t *s_exp, size_t n)
{
        SEXP_val_t v_dsc;

        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (-1);
        }
        
        SEXP_VALIDATE(s_exp);

        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        
        if (v_dsc.type != SEXP_VALTYPE_STRING || n < 1) {
                errno = EINVAL;
                return (-1);
        }
        
        if (n <= (v_dsc.hdr->size / sizeof (char)))
                return (((char *)v_dsc.mem)[n - 1]);
        else {
                errno = ERANGE;
                return (-1);
        }
}

char *SEXP_string_cstr (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
        char      *str;
        size_t     len;

        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(s_exp);
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_STRING) {
                errno = EINVAL;
                return (NULL);
        }

        len = v_dsc.hdr->size / sizeof (char);
        str = sm_alloc (sizeof (char) * (len + 1));
        
        memcpy (str, v_dsc.mem, sizeof (char) * len);
        
        str[len] = '\0';
        
        return (str);
}

size_t SEXP_string_cstr_r (const SEXP_t *s_exp, char *buf, size_t len)
{
        SEXP_val_t v_dsc;

        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return ((size_t)-1);
        }

        SEXP_VALIDATE(s_exp);
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_STRING) {
                errno = EINVAL;
                return ((size_t)-1);
        }

        if (v_dsc.hdr->size / sizeof (char) <= len - 1) {
                memcpy (buf, v_dsc.mem, v_dsc.hdr->size);
                buf[v_dsc.hdr->size / sizeof (char)] = '\0';

                return (v_dsc.hdr->size / sizeof (char));
        }
        
        errno = ERANGE;
        return ((size_t)-1);
}

char *SEXP_string_subcstr (const SEXP_t *s_exp, size_t beg, size_t len)
{
        SEXP_val_t v_dsc;
        char      *s_str;
        size_t     s_len;

        _LOGCALL_;
        
        if (s_exp == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(s_exp);
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_STRING) {
                errno = EINVAL;
                return (NULL);
        }
        
        s_len = v_dsc.hdr->size / sizeof (char);
        
        if (beg > s_len) {
                errno = EINVAL;
                return (NULL);
        }
        
        s_len -= beg;
        
        if (s_len > len)
                s_len = len;
        
        if (s_len > 0) {
                s_len = v_dsc.hdr->size / sizeof (char);
                s_str = sm_alloc (sizeof (char) * (s_len + 1));
                
                memcpy (s_str, ((char *) v_dsc.mem) + beg, sizeof (char) * s_len);
                
                s_str[s_len] = '\0';
                
                return (s_str);
        }
        
        return (NULL);
}

char *SEXP_string_cstrp (const SEXP_t *s_exp)
{
        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(s_exp);
        abort (); /* obsolete */
        return (NULL);
}

int SEXP_string_cmp (const SEXP_t *str_a, const SEXP_t *str_b)
{
        char *a, *b;
        int   c;
        
        _LOGCALL_;

        if (str_a == NULL || str_b == NULL) {
                errno = EFAULT;
                return (-1);
        }

        SEXP_VALIDATE(str_a);
        SEXP_VALIDATE(str_b);

        a = SEXP_string_cstr (str_a);
        b = SEXP_string_cstr (str_b);

        c = strcmp (a, b);

        sm_free (a);
        sm_free (b);

        return (c);
}

/*
 * Lists
 */

SEXP_t *SEXP_list_new (SEXP_t *memb, ...)
{
        va_list    alist;
        SEXP_val_t v_dsc;
        SEXP_t    *s_ptr[32];
        size_t     s_cur;
        uint8_t    b_exp;
        SEXP_t    *s_exp;
        
        _LOGCALL_;

        va_start (alist, memb);

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
        
        s_exp = SEXP_new ();
        s_exp->s_type = NULL;
        s_exp->s_valp = v_dsc.ptr;

        SEXP_VALIDATE(s_exp);
        
        return (s_exp);
}

void SEXP_list_free (SEXP_t *s_exp)
{
        _LOGCALL_;
        SEXP_VALIDATE(s_exp);
        SEXP_free (s_exp);
}

bool SEXP_listp (const SEXP_t *s_exp)
{
        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (false);
        }
        
        SEXP_VALIDATE(s_exp);

        return ((s_exp->s_valp & SEXP_VALT_MASK) == SEXP_VALTYPE_LIST);
}

size_t SEXP_list_length (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;

        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (0);
        }

        SEXP_VALIDATE(s_exp);

        SEXP_val_dsc (&v_dsc, s_exp->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return ((size_t)(-1));
        }
        
        return (SEXP_rawval_list_length ((struct SEXP_val_list *)v_dsc.mem));
}

SEXP_t *SEXP_list_first (const SEXP_t *list)
{
        SEXP_val_t v_dsc;
        SEXP_t    *s_exp;

        _LOGCALL_;
        
        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        s_exp = SEXP_rawval_lblk_nth ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                      SEXP_LCASTP(v_dsc.mem)->offset + 1);

        return (s_exp == NULL ? NULL : SEXP_ref (s_exp));
}

SEXP_t *SEXP_listref_first (SEXP_t *list)
{
        SEXP_val_t v_dsc;
        SEXP_t    *s_exp;

        _LOGCALL_;
        
        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        /* FIXME: check refs */

        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        s_exp = SEXP_rawval_lblk_nth ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                      SEXP_LCASTP(v_dsc.mem)->offset + 1);

        return (s_exp == NULL ? NULL : SEXP_softref (s_exp));        
}

SEXP_t *SEXP_list_rest  (const SEXP_t *list)
{
        SEXP_t    *rest;
        SEXP_val_t v_dsc_o, v_dsc_r;
        struct SEXP_val_lblk *lblk;
        
        _LOGCALL_;

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
                        SEXP_rawval_lblk_incref ((uintptr_t) SEXP_LCASTP(v_dsc_r.mem)->b_addr);
        }
        
        rest = SEXP_new ();
        rest->s_valp = SEXP_val_ptr (&v_dsc_r);
        
        SEXP_VALIDATE(rest);
        
        return (rest);
}

SEXP_t *SEXP_listref_rest (SEXP_t *list)
{
        _LOGCALL_;
        SEXP_VALIDATE(list);
        return (NULL);
}

SEXP_t *SEXP_list_last (const SEXP_t *list)
{
        SEXP_val_t v_dsc;
        struct SEXP_val_lblk *l_blk;
        
        _LOGCALL_;

        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }
        
        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        l_blk = SEXP_VALP_LBLK(SEXP_rawval_lblk_last ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr));
        
        if (l_blk == NULL)
                return (NULL);
        
        return (SEXP_ref (l_blk->memb + (l_blk->real - 1)));
}

SEXP_t *SEXP_list_replace (SEXP_t *list, uint32_t n, const SEXP_t *n_val)
{
        SEXP_val_t v_dsc;
        SEXP_t    *o_val;

        _LOGCALL_;

        if (list == NULL || n_val == NULL || n < 1) {
                errno = EFAULT;
                return (NULL);
        }
        
        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        if (v_dsc.hdr->refs > 1) {
		uintptr_t uptr = SEXP_rawval_list_copy (list->s_valp);
                
		if (SEXP_rawval_decref (list->s_valp)) {
			/* TODO: handle this */
			abort();
		}
                
		list->s_valp = uptr;
		SEXP_val_dsc (&v_dsc, list->s_valp);
        }

        _A(n > 0);
        
        SEXP_LCASTP(v_dsc.mem)->b_addr = (void *) SEXP_rawval_lblk_replace ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                                                            SEXP_LCASTP(v_dsc.mem)->offset + n,
                                                                            n_val, &o_val);
        
        return (o_val);
}

SEXP_t *SEXP_listref_last (SEXP_t *list)
{
        _LOGCALL_;
        SEXP_VALIDATE(list);
        return (NULL);
}

SEXP_t *SEXP_list_nth (const SEXP_t *list, uint32_t n)
{
        SEXP_val_t v_dsc;
        SEXP_t    *s_exp;

        _LOGCALL_;

        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }
        
        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST || n < 1) {
                errno = EINVAL;
                return (NULL);
        }
        
        s_exp = SEXP_rawval_lblk_nth ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                      SEXP_LCASTP(v_dsc.mem)->offset + n);
        
#if !defined(NDEBUG)
        if (s_exp != NULL)
                SEXP_VALIDATE(s_exp);
#endif
        return (s_exp == NULL ? NULL : SEXP_ref (s_exp));
}

SEXP_t *SEXP_listref_nth (SEXP_t *list, uint32_t n)
{
        SEXP_val_t v_dsc;
        SEXP_t    *s_exp;

        _LOGCALL_;

        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        /* FIXME: check refs */
        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST || n < 1) {
                errno = EINVAL;
                return (NULL);
        }
        
        s_exp = SEXP_rawval_lblk_nth ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                      SEXP_LCASTP(v_dsc.mem)->offset + n);

#if !defined(NDEBUG)        
        if (s_exp != NULL)
                SEXP_VALIDATE(s_exp);
#endif
        return (s_exp == NULL ? NULL : SEXP_softref (s_exp));
}

SEXP_t *SEXP_list_add (SEXP_t *list, const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
                        
        _LOGCALL_;

        if (list == NULL || s_exp == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        _A(list->s_valp != 0);
        SEXP_VALIDATE(list);
        SEXP_VALIDATE(s_exp);

        SEXP_val_dsc (&v_dsc, list->s_valp);
        
        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }
        
        if (v_dsc.hdr->refs > 1) {
                /*
                 * Create a private copy of the value and
                 * decrement the reference counter in the
                 * original value.
                 */
                uintptr_t uptr = SEXP_rawval_list_copy (list->s_valp);
                
                if (SEXP_rawval_decref (list->s_valp)) {
                        /* TODO: handle this */
                        abort ();
                }
                
                list->s_valp = uptr;
                SEXP_val_dsc (&v_dsc, list->s_valp);

                uptr = SEXP_rawval_lblk_last ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr);
                SEXP_rawval_lblk_add1 (uptr, s_exp);
        } else {
                /*
                 * Only one reference exists to the value.
                 * However, list blocks have their own
                 * reference counter and some blocks can
                 * be shared. This case is handled by the
                 * function SEXP_rawval_list_add.
                 */
                SEXP_LCASTP(v_dsc.mem)->b_addr = (void *)SEXP_rawval_lblk_add ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr, s_exp);
        }
        
        return (list);
}

SEXP_t *SEXP_list_join (const SEXP_t *list_a, const SEXP_t *list_b)
{
        SEXP_val_t v_dsc_j;
        SEXP_t    *list_j, *memb;
        
        _LOGCALL_;

        if (list_a == NULL || list_b == NULL) {
                errno = EFAULT;
                return (NULL);
        }
        
        SEXP_VALIDATE(list_a);
        SEXP_VALIDATE(list_b);

        if (!SEXP_listp (list_a) || !SEXP_listp (list_b)) {
                errno = EINVAL;
                return (NULL);
        }
        
        list_j = SEXP_new ();
        list_j->s_valp = SEXP_rawval_list_copy (list_a->s_valp);
        
        SEXP_list_foreach (memb, list_b)
                SEXP_list_add (list_j, memb);
        
        return (list_j);
}

SEXP_t *SEXP_list_push (SEXP_t *list, const SEXP_t *s_exp)
{
        _LOGCALL_;
        SEXP_VALIDATE(list);
        SEXP_VALIDATE(s_exp);
        errno = EOPNOTSUPP;
        return (NULL);
}

SEXP_t *SEXP_list_pop (SEXP_t *list)
{
        SEXP_t    *s_ref;
        SEXP_val_t v_dsc;
        struct SEXP_val_lblk *lblk;
        
        _LOGCALL_;

        if (list == NULL) {
                errno = EINVAL;
                return (NULL);
        }
        
        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);
        
        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }
        
        s_ref = SEXP_list_first (list);

        if (v_dsc.hdr->refs > 1) {
                abort ();
        }
        
        lblk = SEXP_VALP_LBLK(SEXP_LCASTP(v_dsc.mem)->b_addr);
        
        if (lblk != NULL) {
                if (++SEXP_LCASTP(v_dsc.mem)->offset == lblk->real) {
                        SEXP_LCASTP(v_dsc.mem)->offset = 0;
                        SEXP_LCASTP(v_dsc.mem)->b_addr = SEXP_VALP_LBLK(lblk->nxsz);
                }
                
                SEXP_rawval_lblk_free1 ((uintptr_t)lblk, SEXP_free_lmemb);
        }

#if !defined(NDEBUG)        
        if (s_ref != NULL)
                SEXP_VALIDATE(s_ref);
#endif
        return (s_ref);
}

void SEXP_lstack_init (SEXP_lstack_t *stack)
{
        stack->p_list = SEXP_list_new (NULL);
        stack->l_size = SEXP_LSTACK_INIT_SIZE;
        stack->l_real = 1;
        stack->l_sref = sm_alloc (sizeof (SEXP_t *) * SEXP_LSTACK_INIT_SIZE);
        stack->l_sref[0] = SEXP_softref (stack->p_list);

        return;
}

SEXP_lstack_t *SEXP_lstack_new (void)
{
        SEXP_lstack_t *stack;
        
        stack = sm_talloc (SEXP_lstack_t);
        SEXP_lstack_init (stack);
        
        return (stack);
}

void SEXP_lstack_destroy (SEXP_lstack_t *stack)
{
        size_t i;

        for (i = stack->l_real; i > 0; --i)
                SEXP_free (stack->l_sref[i - 1]);
        
        sm_free (stack->l_sref);
        SEXP_free (stack->p_list);
        return;
}

void SEXP_lstack_free (SEXP_lstack_t *stack)
{
        SEXP_lstack_destroy (stack);
        sm_free (stack);
        return;
}

SEXP_t *SEXP_lstack_push (SEXP_lstack_t *stack, SEXP_t *s_exp)
{
        if (stack->l_real == stack->l_size) {
                if (stack->l_size < SEXP_LSTACK_GROWFAST_TRESH)
                        stack->l_size <<= 1;
                else
                        stack->l_size += SEXP_LSTACK_GROWSLOW_DIFF;
                
                stack->l_sref = sm_realloc (stack->l_sref, sizeof (SEXP_t *) * stack->l_size);
        }
        
        stack->l_sref[stack->l_real++] = s_exp;
        
        return (s_exp);
}

SEXP_t *SEXP_lstack_pop (SEXP_lstack_t *stack)
{
        SEXP_t *ref;
        size_t  diff;
        
        ref  = stack->l_sref[--stack->l_real];
        diff = stack->l_size - stack->l_real;
        
        if (stack->l_size > SEXP_LSTACK_GROWFAST_TRESH) {
                if (diff >= SEXP_LSTACK_GROWSLOW_DIFF) {
                        stack->l_size -= SEXP_LSTACK_GROWSLOW_DIFF;
                        goto resize;
                }
        } else {
                if (diff >= 2 * stack->l_real) {
                        stack->l_size >>= 1;
                        goto resize;
                }
        }
        
        return (ref);
resize:
        stack->l_sref = sm_realloc (stack->l_sref, sizeof (SEXP_t *) * stack->l_size);
        return (ref);
}

SEXP_t *SEXP_lstack_top (const SEXP_lstack_t *stack)
{
        return (stack->l_sref[stack->l_real - 1]);
}

SEXP_t *SEXP_lstack_list (SEXP_lstack_t *stack)
{
        return SEXP_ref (stack->p_list);
}

size_t SEXP_lstack_depth (SEXP_lstack_t *stack)
{
        return (stack->l_real);
}

/*
 * Generic
 */

SEXP_t *SEXP_new (void)
{
        SEXP_t *s_exp;

        _LOGCALL_;

        s_exp = sm_talloc (SEXP_t);
        s_exp->s_type = NULL;
        s_exp->s_valp = 0;
        s_exp->s_flgs = 0;

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        s_exp->__magic0 = SEXP_MAGIC0;
        s_exp->__magic1 = SEXP_MAGIC1;
#endif        
        
        return (s_exp);
}

SEXP_t *SEXP_ref (const SEXP_t *s_exp_o)
{
        SEXP_t *s_exp_r;

        _LOGCALL_;

        SEXP_VALIDATE(s_exp_o);

        s_exp_r = SEXP_new ();
        s_exp_r->s_type = s_exp_o->s_type;
        s_exp_r->s_valp = SEXP_rawval_incref (s_exp_o->s_valp);
        
        SEXP_VALIDATE(s_exp_r);

        return (s_exp_r);
}

SEXP_t *SEXP_unref (SEXP_t *s_exp_o)
{
        _LOGCALL_;
        SEXP_VALIDATE(s_exp_o);
        
        if (!SEXP_flag_isset (s_exp_o, SEXP_FLAG_SREF)) {
                SEXP_val_t v_dsc;
                
                if (SEXP_rawval_decref (s_exp_o->s_valp)) {
                        
                        SEXP_val_dsc (&v_dsc, s_exp_o->s_valp);
                        
                        switch (v_dsc.type) {
                        case SEXP_VALTYPE_STRING:
                                sm_free (v_dsc.hdr);
                                break;
                        case SEXP_VALTYPE_NUMBER:
                                sm_free (v_dsc.hdr);
                                break;
                        case SEXP_VALTYPE_LIST:
                                if (SEXP_LCASTP(v_dsc.mem)->b_addr != NULL)
                                        SEXP_rawval_lblk_free ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr, SEXP_free_lmemb);
                                
                                sm_free (v_dsc.hdr);
                                break;
                        default:
                                abort ();
                        }
                        
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)        
                        s_exp_o->s_valp = 0;
                        s_exp_o->s_type = NULL;
                        s_exp_o->s_flgs = 0;
                        s_exp_o->__magic0 = SEXP_MAGIC0_INV;
                        s_exp_o->__magic1 = SEXP_MAGIC1_INV;        
#endif
                        sm_free (s_exp_o);
                        s_exp_o = NULL;
                }
                
                SEXP_flag_set (s_exp_o, SEXP_FLAG_SREF);
        }
        
        return (s_exp_o);
}

SEXP_t *SEXP_softref (SEXP_t *s_exp_o)
{
        SEXP_t *s_exp_r;
        SEXP_val_t v_dsc;

        _LOGCALL_;

        SEXP_VALIDATE(s_exp_o);

        s_exp_r = SEXP_new ();
        s_exp_r->s_type = s_exp_o->s_type;
        s_exp_r->s_valp = s_exp_o->s_valp;

        SEXP_val_dsc (&v_dsc, s_exp_r->s_valp);
        
        if (v_dsc.hdr->refs > 1) {
		if (v_dsc.type == SEXP_VALTYPE_LIST) {
			uintptr_t uptr;

			uptr = SEXP_rawval_list_copy (s_exp_r->s_valp);

			if (SEXP_rawval_decref (s_exp_r->s_valp)) {
				/* TODO: handle this */
				abort();
			}

			s_exp_r->s_valp = uptr;
		} else {
                        /* TODO: handle types other than lists */
			abort();
		}
        }

        SEXP_flag_set (s_exp_r, SEXP_FLAG_SREF);
        
        SEXP_VALIDATE(s_exp_r);

        return (s_exp_r);
}

/*
 * SEXP_free for list members. The difference between
 * this function and SEXP_free is that SEXP_free frees
 * the memory used by SEXP_t structure. We can't do
 * this with a list member because the memory is part of
 * a list block.
 */
static void SEXP_free_lmemb (SEXP_t *s_exp)
{
        _A(s_exp != NULL);
        _LOGCALL_;

        SEXP_VALIDATE(s_exp);

        if (((s_exp->s_flgs &
              (SEXP_FLAG_SREF|SEXP_FLAG_INVAL|SEXP_FLAG_UNFIN)) == 0) &&
            SEXP_typeof (s_exp) != SEXP_TYPE_EMPTY)
        {
                SEXP_val_t v_dsc;

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
                                        SEXP_rawval_lblk_free ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr, SEXP_free_lmemb);
                                
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
        s_exp->s_flgs = 0;
        s_exp->__magic0 = SEXP_MAGIC0_INV;
        s_exp->__magic1 = SEXP_MAGIC1_INV;        
#endif
        
        return;
}

#if defined(NDEBUG)
void SEXP_free (SEXP_t *s_exp)
#else
void __SEXP_free (SEXP_t *s_exp, const char *file, uint32_t line, const char *func)
#endif
{
        _LOGCALL_;
        
#if !defined(NDEBUG)
        _D("s_exp=%p (%s:%u:%s)\n", s_exp, file, line, func);
#endif
        
        if (s_exp == NULL) {
                _D("WARN: s_exp = NULL\n");
                return;
        }
        
        if (((s_exp->s_flgs &
              (SEXP_FLAG_SREF|SEXP_FLAG_INVAL|SEXP_FLAG_UNFIN)) == 0) &&
            SEXP_typeof (s_exp) != SEXP_TYPE_EMPTY)
        {
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
                                        SEXP_rawval_lblk_free ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr, SEXP_free_lmemb);
                                
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
        s_exp->s_flgs = 0;
        s_exp->__magic0 = SEXP_MAGIC0_INV;
        s_exp->__magic1 = SEXP_MAGIC1_INV;        
#endif
        
        sm_free (s_exp);
        
        return;
}

#if defined(NDEBUG)
void SEXP_vfree (SEXP_t *s_exp, ...)
#else
void __SEXP_vfree (const char *file, uint32_t line, const char *func, SEXP_t *s_exp, ...)
#endif
{
        va_list ap;

#if !defined(NDEBUG)
        _D("s_exp=%p (%s:%u:%s)\n", s_exp, file, line, func);
#endif

        va_start (ap, s_exp);
        
        for (; s_exp != NULL; s_exp = va_arg (ap, SEXP_t *))
                SEXP_free (s_exp);
        
        va_end (ap);
}

const char *SEXP_datatype (const SEXP_t *s_exp)
{
        _LOGCALL_;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(s_exp);

        if (s_exp->s_type != NULL) {
                _A(s_exp->s_type->name != NULL);
                return ((const char *)s_exp->s_type->name);
        }

        return (NULL);
}

int SEXP_datatype_set (SEXP_t *s_exp, const char *name)
{
        SEXP_datatype_t *t;

        _A(s_exp != NULL);
        _A(name  != NULL);
        _LOGCALL_;

        SEXP_VALIDATE(s_exp);

        t = SEXP_datatype_get (&g_datatypes, name);
        
        if (t == NULL) {
                SEXP_datatype_t dt;

                dt.name     = strdup (name);
                dt.name_len = strlen (name);

                dt.op     = NULL;
                dt.op_cnt = 0;

                t = SEXP_datatype_add (&g_datatypes, &dt);
        }

        if (t == NULL)
                return (-1);
        else
                s_exp->s_type = t;
        
        return (0);
}

int SEXP_datatype_set_nth (SEXP_t *list, uint32_t n, const char *name)
{
        SEXP_datatype_t *t;

        _A(list != NULL);
        _A(name != NULL);
        _LOGCALL_;

        SEXP_VALIDATE(list);

        t = SEXP_datatype_get (&g_datatypes, name);
        
        if (t == NULL) {
                SEXP_datatype_t dt;

                dt.name     = strdup (name);
                dt.name_len = strlen (name);

                dt.op     = NULL;
                dt.op_cnt = 0;

                t = SEXP_datatype_add (&g_datatypes, &dt);
        }
        
        if (t == NULL)
                return (-1);
        else {
                SEXP_val_t v_dsc;
                SEXP_t    *s_nth;
                
                SEXP_val_dsc (&v_dsc, list->s_valp);
                
                if (v_dsc.type != SEXP_VALTYPE_LIST)
                        return (-1);
                
                s_nth = SEXP_rawval_lblk_nth (list->s_valp, n);
                
                if (s_nth == NULL)
                        return (-1);
                
                s_nth->s_type = t; /* XXX: atomic? */
        }

        return (0);
}

SEXP_type_t SEXP_typeof (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;

        _LOGCALL_;
        SEXP_VALIDATE(s_exp);
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        
        return ((SEXP_type_t) v_dsc.type);
}

static const char *__sexp_strtype[] = {
        "empty",
        "string",
        "number",
        "list"
};

const char *SEXP_strtype (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
        
        _LOGCALL_;

        if (s_exp == NULL)
                return ("(null)");
        
        SEXP_VALIDATE(s_exp);

        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        
        return (__sexp_strtype[v_dsc.type]);
}

SEXP_t *SEXP_build (const char *s_str, ...)
{
        _LOGCALL_;
        /* (1 2 3 "asdf" %s %s) */

        /* TBI */
        return (NULL);
}

static int __SEXP_sizeof_lmemb (const SEXP_t *s_exp, size_t *sz)
{
        SEXP_val_t v_dsc;
        int ret = 0;
        
        SEXP_VALIDATE(s_exp);
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        
        switch (v_dsc.type) {
        case SEXP_VALTYPE_LIST:
        {
                struct SEXP_val_lblk *lblk;
                
                lblk = SEXP_VALP_LBLK(SEXP_LCASTP(v_dsc.mem)->b_addr);
                
                while (lblk != NULL) {
                        (*sz) += sizeof (uintptr_t) + (2 * sizeof (uint16_t)) + sizeof (SEXP_t) * (1 << (lblk->nxsz & SEXP_LBLKS_MASK));
                        lblk = SEXP_VALP_LBLK(lblk->nxsz);
                }
                  
                ret = SEXP_rawval_lblk_cb ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr, (int(*)(SEXP_t *, void *))__SEXP_sizeof_lmemb, sz, 1);
        }
        case SEXP_VALTYPE_NUMBER:
        case SEXP_VALTYPE_STRING:
                (*sz) += sizeof (SEXP_valhdr_t) + v_dsc.hdr->size;
                break;
        default:
                abort ();
        }
        
        return (ret);
}

size_t SEXP_sizeof (const SEXP_t *s_exp)
{
        size_t sz;
        
        if (s_exp == NULL)
                return (0);
        
        SEXP_VALIDATE(s_exp);
        
        sz = sizeof (SEXP_t);
        
        if (__SEXP_sizeof_lmemb (s_exp, &sz) != 0)
                return (0);
        
        return (sz);
}

#if 0
int SEXP_structprint (FILE *fp, const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);
        
        /*
         *  print S-exp flags, type, value pointer
         */
        
        fprintf (fp,
                 "=> s-exp@%p ... type: %s (%x)\n"
                 "            ... flgs: %c%c%c (%x)\n"
                 "            ...  ptr: %p\n",
                 SEXP_strtype (s_exp),
                 (s_exp->s_flgs & SEXP_FLAG_SREF  ? 's' : '-'),
                 (s_exp->s_flgs & SEXP_FLAG_INVAL ? 'i' : '-'),
                 (s_exp->s_flgs & SEXP_FLAG_UNFIN ? 'u' : '-'),
                 (void *)s_exp->s_valp);
        
        switch (v_dsc.type) {
        case SEXP_VALTYPE_LIST:
        {
                struct SEXP_val_lblk *lblk;
                
                /* print list blocks */
                
                /* recursion */
                
        }       break;
        case SEXP_VALTYPE_NUMBER:
                
                
                break;
        case SEXP_VALTYPE_STRING:
                
                break;
        default:
                abort ();
        }
        
        return (0);
}
#endif /* 0 */

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
typedef struct {
        const char *file;
        uint32_t    line;
        const char *func;
} __dbginf_t;

static int __SEXP_VALIDATE_cb (SEXP_t *s_exp, __dbginf_t *info)
{
        __SEXP_VALIDATE(s_exp, info->file, info->line, info->func);
        return (0);
}

void __SEXP_VALIDATE(const SEXP_t *s_exp, const char *file, uint32_t line, const char *func)
{
        SEXP_val_t v_dsc;

#ifdef SEXP_VALIDATE_DEBUG
        _D("VALIDATE: s_exp=%p (%s:%u:%s)\n", s_exp, file, line, func);
#endif        

        if (s_exp == NULL) abort ();
        if (s_exp->__magic0 != SEXP_MAGIC0) abort ();
        if (s_exp->__magic1 != SEXP_MAGIC1) abort ();
        
        SEXP_val_dsc (&v_dsc, s_exp->s_valp);

        if (s_exp->s_flgs & (SEXP_FLAG_INVAL | SEXP_FLAG_UNFIN) || v_dsc.type == SEXP_VALTYPE_EMPTY)
                return;
        else {
                if (v_dsc.hdr == NULL) abort ();
        }

        switch (v_dsc.type) {
        case SEXP_VALTYPE_NUMBER:
                switch (SEXP_NTYPEP(v_dsc.hdr->size, v_dsc.mem))
                {
                case SEXP_NUM_NONE:
                case SEXP_NUM_BOOL:
                case SEXP_NUM_INT8:
                case SEXP_NUM_UINT8:
                case SEXP_NUM_INT16:
                case SEXP_NUM_UINT16:
                case SEXP_NUM_INT32:
                case SEXP_NUM_UINT32:
                case SEXP_NUM_INT64:
                case SEXP_NUM_UINT64:
                case SEXP_NUM_DOUBLE:
                        break;
                default:
                        /* unknown number type */ abort ();
                }
                break;
        case SEXP_VALTYPE_STRING:
                break;
        case SEXP_VALTYPE_LIST:
        {
                __dbginf_t info;

                info.file = file;
                info.line = line;
                info.func = func;
                
                SEXP_rawval_lblk_cb ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                     (int (*)(SEXP_t *, void *)) __SEXP_VALIDATE_cb, (void *) &info,
                                     SEXP_LCASTP(v_dsc.mem)->offset + 1);
                
                break;
        }
        case SEXP_VALTYPE_EMPTY:
                break;
        default:
                /* unknown S-exp value type */ abort ();
        }
        
        return;
}
#endif /* !NDEBUG || VALIDATE_SEXP */
