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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <config.h>
#include <assert.h>
#include "generic/common.h"
#include "public/strbuf.h"
#include "public/sm_alloc.h"
#include "_sexp-types.h"
#include "_sexp-output.h"
#include "_sexp-value.h"

#define SEXP_SBPRINTF_BUFSZ 1024

#if 0
static int SEXP_sbprintf_t_cb (SEXP_t *s_exp, strbuf_t *sb)
{
        if (SEXP_sbprintf_t (s_exp, sb) != 0)
                return (-1);
        /*if (strbuf_add (sb, " ", 1) != 0)
          return (-1); */
        else
                return (0);
}
#endif

int SEXP_sbprintf_t (SEXP_t *s_exp, strbuf_t *sb)
{
        SEXP_val_t v_dsc;
        int buflen;

        if (s_exp->s_type != NULL) {
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

        if (s_exp->s_type != NULL)
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

#if 0
static SEXP_ostate_t *__SEXP_ostate_new (void)
{
        SEXP_ostate_t *ost;

        ost = sm_talloc (SEXP_ostate_t);
        ost->sexp = NULL;
        ost->list_pos = 0;
        ost->sexp_pos = 0;
        ost->sexp_part = 0;
        LIST_stack_init (&(ost->lstack));

        return (ost);
}

static void __SEXP_ostate_free (SEXP_ostate_t *ost)
{
        _A(ost != NULL);

        if (ost->lstack.LIST_stack_cnt > 0)
                sm_free (ost->lstack.LIST_stack);

        sm_free (ost);
        return;
}

ssize_t SEXP_st_dprintf (int fd, SEXP_format_t fmt, SEXP_t *sexp, SEXP_ostate_t **ost)
{
        _A(fd >= 0);
        _A(sexp != NULL);
        _A(ost != NULL);

        switch (fmt) {
        case SEXP_FMT_CANONICAL:
                return SEXP_st_dprintc (fd, sexp, ost);
        case SEXP_FMT_ADVANCED:
                return SEXP_st_dprinta (fd, sexp, ost);
        case SEXP_FMT_TRANSPORT:
                return SEXP_st_dprintt (fd, sexp, ost);
        default:
                abort ();
        }
        /* NOTREACHED */
        return (-1);
}

ssize_t SEXP_st_dnprintf (int fd, size_t maxsz, SEXP_format_t fmt, SEXP_t *sexp, SEXP_ostate_t **ost)
{
        _A(fd >= 0);
        _A(maxsz > 0);
        _A(sexp != NULL);
        _A(ost != NULL);

        switch (fmt) {
        case SEXP_FMT_CANONICAL:
                return SEXP_st_dnprintc (fd, maxsz, sexp, ost);
        case SEXP_FMT_ADVANCED:
                return SEXP_st_dnprinta (fd, maxsz, sexp, ost);
        case SEXP_FMT_TRANSPORT:
                return SEXP_st_dnprintt (fd, maxsz, sexp, ost);
        default:
                abort ();
        }
        /* NOTREACHED */
        return (-1);
}

int SEXP_printf (SEXP_format_t fmt, SEXP_t *sexp)
{
        switch (fmt) {
        case SEXP_FMT_CANONICAL:
                return SEXP_printfc (sexp);
        case SEXP_FMT_ADVANCED:
                return SEXP_printfa (sexp);
        case SEXP_FMT_TRANSPORT:
                return SEXP_printft (sexp);
        default:
                abort ();
        }

        /* NOTREACHED */
        return (-1);
}

int SEXP_fprintf (FILE *fp, SEXP_format_t fmt, SEXP_t *sexp)
{
        switch (fmt) {
        case SEXP_FMT_CANONICAL:
                return SEXP_fprintfc (fp, sexp);
        case SEXP_FMT_ADVANCED:
                return SEXP_fprintfa (fp, sexp);
        case SEXP_FMT_TRANSPORT:
                return SEXP_fprintft (fp, sexp);
        default:
                abort ();
        }

        /* NOTREACHED */
        return (-1);
}

int SEXP_sprintf (char *str, SEXP_format_t fmt, SEXP_t *sexp)
{
        switch (fmt) {
        case SEXP_FMT_CANONICAL:
                return SEXP_sprintfc (str, sexp);
        case SEXP_FMT_ADVANCED:
                return SEXP_sprintfa (str, sexp);
        case SEXP_FMT_TRANSPORT:
                return SEXP_sprintft (str, sexp);
        default:
                abort ();
        }

        /* NOTREACHED */
        return (-1);
}

int SEXP_snprintf (char *str, size_t size, SEXP_format_t fmt, SEXP_t *sexp)
{
        switch (fmt) {
        case SEXP_FMT_CANONICAL:
                return SEXP_snprintfc (str, size, sexp);
        case SEXP_FMT_ADVANCED:
                return SEXP_snprintfa (str, size, sexp);
        case SEXP_FMT_TRANSPORT:
                return SEXP_snprintft (str, size, sexp);
        default:
                abort ();
        }

        /* NOTREACHED */
        return (-1);
}

int SEXP_asprintf (char **ret, SEXP_format_t fmt, SEXP_t *sexp)
{
        switch (fmt) {
        case SEXP_FMT_CANONICAL:
                return SEXP_asprintfc (ret, sexp);
        case SEXP_FMT_ADVANCED:
                return SEXP_asprintfa (ret, sexp);
        case SEXP_FMT_TRANSPORT:
                return SEXP_asprintft (ret, sexp);
        default:
                abort ();
        }

        /* NOTREACHED */
        return (-1);
}

int SEXP_asnprintf (char **ret, size_t maxsz, SEXP_format_t fmt, SEXP_t *sexp)
{
        switch (fmt) {
        case SEXP_FMT_CANONICAL:
                return SEXP_asnprintfc (ret, maxsz, sexp);
        case SEXP_FMT_ADVANCED:
                return SEXP_asnprintfa (ret, maxsz, sexp);
        case SEXP_FMT_TRANSPORT:
                return SEXP_asnprintft (ret, maxsz, sexp);
        default:
                abort ();
        }

        /* NOTREACHED */
        return (-1);
}

/* canonical */

size_t SEXP_st_dprintc (int fd, SEXP_t *s_exp, SEXP_ostate_t **ost)
{
        SEXP_t *s_cur;

        /* create output state */


        for (;;) {
                s_cur = SEXP_ostate_sexp (*ost);

                if (s_cur->s_type != NULL) {
                        /* print datatype */
                }

                switch (SEXP_typeof (
        }
}


ssize_t SEXP_st_dprintc (int fd, SEXP_t *sexp, SEXP_ostate_t **ost)
{
        SEXP_t *psexp;

        char    *buffer, ch;
        uint32_t digits;
        ssize_t wret = -1;
        size_t  wlen =  0;

        struct iovec vec[3];

        _A(fd >= 0);
        _A(sexp != NULL);
        _A(ost  != NULL);

#define SCNT (LIST_stack_cnt (&((*ost)->lstack)))

        if (*ost == NULL) {
                *ost = __SEXP_ostate_new ();
                (*ost)->sexp = psexp = sexp;
        } else {
                psexp = (*ost)->sexp;
                /* FIXME */
        }

print_loop:
        if (psexp->handler != NULL) {
                SEXP_datatype_t *h = psexp->handler;

                ch = ']';

                digits = xnumdigits (h->name_len);
                buffer = sm_alloc (sizeof (char) * (digits + 2));
                snprintf (buffer, sizeof (char) * (digits + 2), "%.*hu[",
                          digits, h->name_len);

                vec[0].iov_base = buffer;
                vec[0].iov_len  = digits + 1;
                vec[1].iov_base = h->name;
                vec[1].iov_len  = h->name_len;
                vec[2].iov_base = &ch;
                vec[2].iov_len  = 1;

                wret = writev (fd, vec, 3);

                _D("wrote: %.*s%.*s\n", digits + 1, buffer, h->name_len, h->name);

                if (wret == -1) {
                        protect_errno {
                                _D("errno=%u, %s.\n", errno, strerror (errno));
                        }

                        return (-1);
                }

                sm_free (buffer);

                wlen += (size_t)wret;
        }

        switch (SEXP_TYPE(psexp)) {
        case ATOM_LIST:
                ch = '(';
                if (write (fd, &ch, sizeof ch) != sizeof ch) {
                        /* write error */
                        protect_errno {
                                _D("errno=%u, %s.\n", errno, strerror (errno));
                        }

                        return (-1);
                }

                _D("wrote: (\n");

                wlen += sizeof ch;

                LIST_stack_push (&((*ost)->lstack), &(psexp->atom.list));
                (*ost)->list_pos = sm_realloc ((*ost)->list_pos,
                                               sizeof (uint32_t) * SCNT);

                _A(SCNT > 0);

                (*ost)->list_pos[SCNT - 1] = 0;
                break;
        case ATOM_STRING:
                digits = xnumdigits (psexp->atom.string.len);
                buffer = sm_alloc (sizeof (char) * (digits + 2));
                snprintf (buffer, sizeof (char) * (digits + 2), "%.*u:",
                          digits, psexp->atom.string.len);

                vec[0].iov_base = buffer;
                vec[0].iov_len  = digits + 1;
                vec[1].iov_base = psexp->atom.string.str;
                vec[1].iov_len  = psexp->atom.string.len;

                _D("wrote: %.*s%.*s\n", digits + 1, buffer,
                   psexp->atom.string.len, psexp->atom.string.str);

                if ((wret = writev (fd, vec, 2)) == -1) {
                        /* write error */
                        protect_errno {
                                _D("errno=%u, %s.\n", errno, strerror (errno));
                        }

                        return (-1);
                }

                sm_free (buffer);

                if ((wret - digits - 1 - psexp->atom.string.len) != 0) {
                        /* not everything was written to fd */
                        (*ost)->sexp_pos  = wret;
                        (*ost)->sexp_part = 1;

                        protect_errno {
                                _D("errno=%u, %s.\n", errno, strerror (errno));
                        }

                        return (-1);
                }

                wlen += (size_t)wret;
                break;
        case ATOM_NUMBER: {
                char numstr[64];
                int  numlen;
                /*
                 * Number is converted to string with datatype.
                 * e.g.: 123 -> 4[int8]3:123
                 *
                 * FIXME: not implemented...
                 */
#define CASE(__m, __t, __func, __fmt)                                   \
                case (__m): {                                           \
                        __t num;                                        \
                        num = __func (psexp);                           \
                        numlen = snprintf (numstr, sizeof numstr,       \
                                           __fmt, num);                 \
                }                                                       \
                        break

                switch (psexp->atom.number.type) {
                        CASE(NUM_INT8,     int8_t, SEXP_number_gethhd, "%hhd ");
                        CASE(NUM_UINT8,   uint8_t, SEXP_number_gethhu, "%hhu ");
                        CASE(NUM_INT16,   int16_t, SEXP_number_gethd,  "%hd ");
                        CASE(NUM_UINT16, uint16_t, SEXP_number_gethu,  "%hu ");
                        CASE(NUM_INT32,   int32_t, SEXP_number_getd,   "%d ");
                        CASE(NUM_UINT32, uint32_t, SEXP_number_getu,   "%u ");
                        CASE(NUM_INT64,   int64_t, SEXP_number_getlld, "%lld ");
                        CASE(NUM_UINT64, uint64_t, SEXP_number_getllu, "%llu ");
                        CASE(NUM_DOUBLE,   double, SEXP_number_getf,   "%f ");
                default:
                        abort ();
                }
#undef CASE
                if (write (fd, numstr, numlen) != numlen) {
                        /* write error */
                        protect_errno {
                                _D("errno=%u, %s.\n", errno, strerror (errno));
                        }

                        return (-1);
                }

                _D("wrote: %.*s\n", numlen, numstr);

                wlen += numlen;
        }
                break;
        default:
                abort ();
        }

        while (SCNT > 0) {
                LIST_t *ltop;

                ltop = LIST_stack_top (&((*ost)->lstack));

                if ((*ost)->list_pos[SCNT - 1] < ltop->count) {
                        psexp = &(SEXP(ltop->memb)[((*ost)->list_pos[SCNT - 1])]);
                        ++((*ost)->list_pos[SCNT - 1]);

                        goto print_loop;
                } else {
                        /* end of list */
                        ch = ')';

                        _D("wrote: )\n");

                        if (write (fd, &ch, sizeof ch) != sizeof ch) {
                                /* write error */
                                protect_errno {
                                        _D("errno=%u, %s.\n", errno, strerror (errno));
                                }

                                return (-1);
                        }

                        wlen += sizeof ch;

                        LIST_stack_dec (&((*ost)->lstack));
                        (*ost)->list_pos = sm_realloc ((*ost)->list_pos,
                                                       sizeof (uint32_t) * SCNT);
                }
        }

        /*
         *  If we got here, we can destroy
         *  the ostate because everything
         *  went fine.
         */
        (*ost) = NULL;

        return ((ssize_t)wlen);
}

ssize_t SEXP_st_dnprintc (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost)
{
        return (-1);
}

int SEXP_printfc (SEXP_t *sexp)
{
        return SEXP_fprintfc (stdout, sexp);
}

int SEXP_fprintfc (FILE *fp, SEXP_t *sexp)
{
        return (-1);
}

int SEXP_dprintfc (int fd, SEXP_t *sexp)
{

        return (-1);
}

int SEXP_sprintfc (char *str, SEXP_t *sexp)
{
        return SEXP_snprintfc (str, strlen (str) + 1, sexp);
}

int SEXP_snprintfc (char *str, size_t size, SEXP_t *sexp)
{
        return (-1);
}

int SEXP_asprintfc (char **ret, SEXP_t *sexp)
{
        return (-1);
}

int SEXP_asnprintfc (char **ret, size_t maxsz, SEXP_t *sexp)
{
        return (-1);
}

/* advanced */
ssize_t SEXP_st_dprinta (int fd, SEXP_t *sexp, SEXP_ostate_t **ost)
{
        return (-1);
}

ssize_t SEXP_st_dnprinta (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost)
{
        return (-1);
}

int SEXP_printfa (SEXP_t *sexp)
{
        return SEXP_fprintfa (stdout, sexp);
}

static int __SEXP_fprintfa (FILE *fp, SEXP_t *sexp, uint32_t indent)
{
        const char *datatype;

        datatype = SEXP_datatype (sexp);

        if (datatype != NULL)
                fprintf (fp, "[%s]", datatype);

        switch (SEXP_TYPE(sexp)) {
        case ATOM_STRING:
                return fprintf (fp, "\"%.*s\"", sexp->atom.string.len, sexp->atom.string.str);
        case ATOM_NUMBER:
                switch (sexp->atom.number.type) {
                case NUM_INT8:
                        return fprintf (fp, "%hhd", NUM(int8_t, sexp->atom.number.nptr));
                case NUM_UINT8:
                        return fprintf (fp, "%hhu", NUM(uint8_t, sexp->atom.number.nptr));
                case NUM_INT16:
                        return fprintf (fp, "%hd", NUM(int16_t, sexp->atom.number.nptr));
                case NUM_UINT16:
                        return fprintf (fp, "%hu", NUM(uint16_t, sexp->atom.number.nptr));
                case NUM_INT32:
                        return fprintf (fp, "%d", NUM(int32_t, sexp->atom.number.nptr));
                case NUM_UINT32:
                        return fprintf (fp, "%u", NUM(uint32_t, sexp->atom.number.nptr));
                case NUM_INT64:
                        return fprintf (fp, "%lld", NUM(int64_t, sexp->atom.number.nptr));
                case NUM_UINT64:
                        return fprintf (fp, "%llu", NUM(uint64_t, sexp->atom.number.nptr));
                case NUM_DOUBLE:
                        return fprintf (fp, "%f", NUM(double, sexp->atom.number.nptr));
                default:
                        _D("Unsupported number type: %d\n", sexp->atom.number.type);
                        abort ();
                }
                break;
        case ATOM_LIST: {
                uint32_t i;

                if (indent > 0)
                        putc ('\n', fp);
                for (i = 0; i < indent; ++i)
                        putc (' ', fp);
                putc ('(', fp);

                i = 0;
                if (i < sexp->atom.list.count) {
                        for (;;) {
                                if (__SEXP_fprintfa (fp, SEXP(sexp->atom.list.memb) + i, indent + 1) < 0)
                                        return (-1);
                                ++i;

                                if (i < sexp->atom.list.count) {
                                        putc (' ', fp);
                                } else {
                                        break;
                                }
                        }
                }

                putc (')', fp);
                return (0);
        }
        case ATOM_EMPTY:
        case ATOM_UNFIN:
        default:
                abort ();
        }

        /* NOTREACHED */
        return (-1);
}

int SEXP_fprintfa (FILE *fp, SEXP_t *sexp)
{
        return __SEXP_fprintfa (fp, sexp, 0);
}

int SEXP_sprintfa (char *str, SEXP_t *sexp)
{
        return SEXP_snprintfa (str, strlen (str) + 1, sexp);
}

int SEXP_snprintfa (char *str, size_t size, SEXP_t *sexp)
{
        return (-1);
}

int SEXP_asprintfa (char **ret, SEXP_t *sexp)
{
        return (-1);
}

int SEXP_asnprintfa (char **ret, size_t maxsz, SEXP_t *sexp)
{
        return (-1);
}

/* transport */
ssize_t SEXP_st_dprintt (int fd, SEXP_t *sexp, SEXP_ostate_t **ost)
{
        return SEXP_st_dprintc (fd, sexp, ost);
}

ssize_t SEXP_st_dnprintt (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost)
{
        return SEXP_st_dnprintc (fd, maxsz, sexp, ost);
}

int SEXP_printft (SEXP_t *sexp)
{
        return SEXP_fprintft (stdout, sexp);
}

int SEXP_fprintft (FILE *fp, SEXP_t *sexp)
{
        return SEXP_fprintfc (fp, sexp);
}

int SEXP_sprintft (char *str, SEXP_t *sexp)
{
        return SEXP_snprintft (str, strlen (str) + 1, sexp);
}

int SEXP_snprintft (char *str, size_t size, SEXP_t *sexp)
{
        return SEXP_snprintfc (str, size, sexp);
}

int SEXP_asprintft (char **ret, SEXP_t *sexp)
{
        return SEXP_asprintfc (ret, sexp);
}

int SEXP_asnprintft (char **ret, size_t n, SEXP_t *sexp)
{
        return (-1);
}
#endif
