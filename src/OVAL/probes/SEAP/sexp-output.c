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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <assert.h>

#include "generic/common.h"
#include "public/strbuf.h"
#include "_sexp-types.h"
#include "_sexp-output.h"
#include "_sexp-value.h"
#include "_sexp-datatype.h"
#include "_sexp-rawptr.h"
#include "debug_priv.h"

#define SEXP_SBPRINTF_BUFSZ 1024

int SEXP_sbprintf_t (SEXP_t *s_exp, strbuf_t *sb)
{
        SEXP_val_t v_dsc;
        int buflen;

        if (SEXP_rawptr_mask(s_exp->s_type, SEXP_DATATYPEPTR_MASK) != NULL) {
                const char *name;
                char  buffer[64+1];

                name   = SEXP_datatype_name(s_exp->s_type);
                buflen = snprintf (buffer, sizeof buffer,
                                   "#d%zu[%s]", strlen (name), name);

                _A((size_t)buflen < sizeof buffer);

                if (strbuf_add (sb, buffer, buflen) != 0)
                        return (-1);
        }

        SEXP_val_dsc (&v_dsc, s_exp->s_valp);

        switch (v_dsc.type) {
        case SEXP_VALTYPE_NUMBER:
        {
                SEXP_numtype_t t;

                t = SEXP_NTYPEP(v_dsc.hdr->size, v_dsc.mem);

                if (t <= SEXP_NUM_UINT16) {
                        if (t <= SEXP_NUM_UINT8) {
                                char buffer[6+1];

                                switch (t) {
                                case SEXP_NUM_BOOL:
                                        buflen = snprintf (buffer, sizeof buffer,
                                                           "#%c", SEXP_NCASTP(b ,v_dsc.mem)->n ? 'T':'F');
                                        break;
                                case SEXP_NUM_INT8:
                                        buflen = snprintf (buffer, sizeof buffer,
                                                           "#d%hhd", SEXP_NCASTP(i8,v_dsc.mem)->n);
                                        break;
                                case SEXP_NUM_UINT8:
                                        buflen = snprintf (buffer, sizeof buffer,
                                                           "#d%hhu", SEXP_NCASTP(u8,v_dsc.mem)->n);
                                        break;
                                default:
                                        abort ();
                                }

                                _A(buflen >= 0);
                                _A((size_t)buflen < sizeof buffer);

                                if (strbuf_add (sb, (const char *)buffer, (size_t)buflen) != 0)
                                        return (-1);

                        } else {
                                char buffer[8+1];

                                switch (t) {
                                case SEXP_NUM_INT16:
                                        buflen = snprintf (buffer, sizeof buffer,
                                                           "#d%hd", SEXP_NCASTP(i16,v_dsc.mem)->n);
                                        break;
                                case SEXP_NUM_UINT16:
                                        buflen = snprintf (buffer, sizeof buffer,
                                                           "#d%hu", SEXP_NCASTP(u16,v_dsc.mem)->n);
                                        break;
                                default:
                                        abort ();
                                }

                                _A(buflen >= 0);
                                _A((size_t)buflen < sizeof buffer);

                                if (strbuf_add (sb, (const char *)buffer, (size_t)buflen) != 0)
                                        return (-1);

                        }
                } else {
                        if (t <= SEXP_NUM_INT64) {
                                char buffer[22+1];

                                switch (t) {
                                case SEXP_NUM_INT32:
                                        buflen = snprintf (buffer, sizeof buffer,
                                                           "#d%" PRId32, SEXP_NCASTP(i32,v_dsc.mem)->n);
                                        break;
                                case SEXP_NUM_UINT32:
                                        buflen = snprintf (buffer, sizeof buffer,
                                                           "#d%" PRIu32, SEXP_NCASTP(u32,v_dsc.mem)->n);
                                        break;
                                case SEXP_NUM_INT64:
                                        buflen = snprintf (buffer, sizeof buffer,
                                                           "#d%" PRId64, SEXP_NCASTP(i64,v_dsc.mem)->n);
                                        break;
                                default:
                                        abort ();
                                }

                                _A(buflen >= 0);
                                _A((size_t)buflen < sizeof buffer);

                                if (strbuf_add (sb, (const char *)buffer, (size_t)buflen) != 0)
                                        return (-1);

                        } else {
                                char buffer[66+1];

                                switch (t) {
                                case SEXP_NUM_UINT64:
                                        buflen = snprintf (buffer, sizeof buffer,
                                                           "#d%" PRIu64, SEXP_NCASTP(u64,v_dsc.mem)->n);
                                        break;
                                case SEXP_NUM_DOUBLE:
                                        buflen = snprintf (buffer, sizeof buffer,
                                                           "#d%g", SEXP_NCASTP(f  ,v_dsc.mem)->n);
                                        break;
                                default:
                                        abort ();
                                }

                                _A(buflen >= 0);
                                _A((size_t)buflen < sizeof buffer);

                                if (strbuf_add (sb, (const char *)buffer, (size_t)buflen) != 0)
                                        return (-1);

                        }
                }
                break;
        }
        case SEXP_VALTYPE_STRING:
        {
                char buffer[20+1];

                buflen = snprintf (buffer, sizeof buffer, "#d%zu:", v_dsc.hdr->size / sizeof (char));

                _A(buflen >= 0);
                _A((size_t)buflen < sizeof buffer);

                if (strbuf_add (sb, (const char *)buffer, (size_t)buflen) != 0)
                        return (-1);
                if (strbuf_add (sb, (const char *)v_dsc.mem, v_dsc.hdr->size / sizeof (char)) != 0)
                        return (-1);

                break;
        }
        case SEXP_VALTYPE_LIST:

                if (strbuf_add (sb, "(", 1) != 0)
                        return (-1);
                if (SEXP_rawval_lblk_cb ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr, (int (*)(SEXP_t *, void *)) SEXP_sbprintf_t, (void *)sb,
                                         SEXP_LCASTP(v_dsc.mem)->offset + 1) != 0)
                        return (-1);
                if (strbuf_trunc (sb, 1) != 0)
                        return (-1);
                if (strbuf_add (sb, ")", 1) != 0)
                        return (-1);

                break;
        default:
                abort ();
        }

        return (0);
}

typedef struct {
        size_t sz;
        FILE  *fp;
} __fprintfa_t;

#define AS(t,p) ((t)(p))

static int __SEXP_fprintfa_lmemb (const SEXP_t *s_exp, void *arg)
{
        AS(__fprintfa_t *, arg)->sz += SEXP_fprintfa (AS(__fprintfa_t *, arg)->fp, s_exp);
        fputc (' ', AS(__fprintfa_t *, arg)->fp);
        return (0);
}

size_t SEXP_fprintfa (FILE *fp, const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;

        if (SEXP_rawptr_mask(s_exp->s_type, SEXP_DATATYPEPTR_MASK) != NULL)
                fprintf (fp, "[%s]", SEXP_datatype_name(s_exp->s_type));

        SEXP_val_dsc (&v_dsc, s_exp->s_valp);

        switch (v_dsc.type) {
        case SEXP_VALTYPE_NUMBER:
        {
                SEXP_numtype_t t;

                t = SEXP_NTYPEP(v_dsc.hdr->size, v_dsc.mem);

                if (t <= SEXP_NUM_UINT16) {
                        if (t <= SEXP_NUM_UINT8) {
                                switch (t) {
                                case SEXP_NUM_BOOL:  return fprintf (fp, "#%c", SEXP_NCASTP(b ,v_dsc.mem)->n ? 'T':'F');
                                case SEXP_NUM_INT8:  return fprintf (fp, "%hhd", SEXP_NCASTP(i8,v_dsc.mem)->n);
                                case SEXP_NUM_UINT8: return fprintf (fp, "%hhu", SEXP_NCASTP(u8,v_dsc.mem)->n);
                                }
                        } else {
                                switch (t) {
                                case SEXP_NUM_INT16:  return fprintf (fp, "%hd", SEXP_NCASTP(i16,v_dsc.mem)->n);
                                case SEXP_NUM_UINT16: return fprintf (fp, "%hu", SEXP_NCASTP(u16,v_dsc.mem)->n);
                                }
                        }
                } else {
                        if (t <= SEXP_NUM_INT64) {
                                switch (t) {
                                case SEXP_NUM_INT32:  return fprintf (fp, "%" PRId32, SEXP_NCASTP(i32,v_dsc.mem)->n);
                                case SEXP_NUM_UINT32: return fprintf (fp, "%" PRIu32, SEXP_NCASTP(u32,v_dsc.mem)->n);
                                case SEXP_NUM_INT64:  return fprintf (fp, "%" PRId64, SEXP_NCASTP(i64,v_dsc.mem)->n);
                                }
                        } else {
                                switch (t) {
                                case SEXP_NUM_UINT64: return fprintf (fp, "%" PRIu64, SEXP_NCASTP(u64,v_dsc.mem)->n);
                                case SEXP_NUM_DOUBLE: return fprintf (fp,   "%g", SEXP_NCASTP(f  ,v_dsc.mem)->n);
                                }
                        }
                }

                abort ();
        }       break;
        case SEXP_VALTYPE_STRING:
                return fprintf (fp, "\"%.*s\"", (int)(v_dsc.hdr->size / sizeof (char)), (char *)v_dsc.mem);
        case SEXP_VALTYPE_LIST:
        {
                __fprintfa_t fpa;

                fpa.sz = 2;
                fpa.fp = fp;
                fputc ('(', fp);

                SEXP_rawval_lblk_cb ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                     (int(*)(SEXP_t *, void *))__SEXP_fprintfa_lmemb, &fpa,
                                     SEXP_LCASTP(v_dsc.mem)->offset + 1);

                fputc (')', fp);

                return (fpa.sz);
        }
        default:
                abort ();
        }

        return (0);
}
