#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <config.h>
#include <assert.h>
#include "common.h"
#include "xmalloc.h"
#include "sexp-types.h"
#include "sexp-output.h"

#ifndef _A
#define _A(x) assert(x)
#endif

static SEXP_ostate_t *__SEXP_ostate_new (void)
{
        SEXP_ostate_t *ost;

        ost = xmalloc (sizeof (SEXP_ostate_t));
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
                xfree ((void **)&(ost->lstack.LIST_stack));
        
        xfree ((void **)&ost);
}

ssize_t SEXP_st_dprintf (int fd, SEXP_format_t fmt, SEXP_t *sexp, SEXP_ostate_t **ost)
{
        _A(fd >= 0);
        _A(sexp != NULL);
        _A(ost != NULL);

        switch (fmt) {
        case FMT_CANONICAL:
                return SEXP_st_dprintc (fd, sexp, ost);
        case FMT_ADVANCED:
                return SEXP_st_dprinta (fd, sexp, ost);
        case FMT_TRANSPORT:
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
        case FMT_CANONICAL:
                return SEXP_st_dnprintc (fd, maxsz, sexp, ost);
        case FMT_ADVANCED:
                return SEXP_st_dnprinta (fd, maxsz, sexp, ost);
        case FMT_TRANSPORT:
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
        case FMT_CANONICAL:
                return SEXP_printfc (sexp);
        case FMT_ADVANCED:
                return SEXP_printfa (sexp);
        case FMT_TRANSPORT:
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
        case FMT_CANONICAL:
                return SEXP_fprintfc (fp, sexp);
        case FMT_ADVANCED:
                return SEXP_fprintfa (fp, sexp);
        case FMT_TRANSPORT:
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
        case FMT_CANONICAL:
                return SEXP_sprintfc (str, sexp);
        case FMT_ADVANCED:
                return SEXP_sprintfa (str, sexp);
        case FMT_TRANSPORT:
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
        case FMT_CANONICAL:
                return SEXP_snprintfc (str, size, sexp);
        case FMT_ADVANCED:
                return SEXP_snprintfa (str, size, sexp);
        case FMT_TRANSPORT:
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
        case FMT_CANONICAL:
                return SEXP_asprintfc (ret, sexp);
        case FMT_ADVANCED:
                return SEXP_asprintfa (ret, sexp);
        case FMT_TRANSPORT:
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
        case FMT_CANONICAL:
                return SEXP_asnprintfc (ret, maxsz, sexp);
        case FMT_ADVANCED:
                return SEXP_asnprintfa (ret, maxsz, sexp);
        case FMT_TRANSPORT:
                return SEXP_asnprintft (ret, maxsz, sexp);
        default:
                abort ();
        }
        
        /* NOTREACHED */
        return (-1);
}

/* canonical */

ssize_t SEXP_st_dprintc (int fd, SEXP_t *sexp, SEXP_ostate_t **ost)
{
        SEXP_t *psexp;

        char    *buffer, ch;
        uint32_t digits;
        ssize_t wret = -1;
        size_t  wlen =  0;
        int err;
        
        struct iovec vec[2];
        
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
                /* print datatype */
        }
        
        switch (SEXP_TYPE(psexp)) {
        case ATOM_LIST:
                ch = '(';
                if (write (fd, &ch, sizeof ch) != sizeof ch) {
                        /* write error */
                        return (-1);
                }
                
                wlen += sizeof ch;
                
                LIST_stack_push (&((*ost)->lstack), &(psexp->atom.list));
                (*ost)->list_pos = xrealloc ((*ost)->list_pos,
                                             sizeof (uint32_t) * SCNT);
                
                _A(SCNT > 0);
                
                (*ost)->list_pos[SCNT - 1] = 0;
                break;
        case ATOM_STRING:
                digits = xnumdigits (psexp->atom.string.len);
                buffer = xmalloc (sizeof (char) * (digits + 2));
                snprintf (buffer, sizeof (char) * (digits + 2), "%.*u:",
                          digits, psexp->atom.string.len);
                
                vec[0].iov_base = buffer;
                vec[0].iov_len  = digits + 1;
                vec[1].iov_base = psexp->atom.string.str;
                vec[1].iov_len  = psexp->atom.string.len;
                
                if ((wret = writev (fd, vec, 2)) == -1) {
                        /* write error */
                        err = errno;
                        return (-1);
                }
                
                xfree ((void **)&buffer);
                
                if ((wret - digits - 1 - psexp->atom.string.len) != 0) {
                        /* not everything was written to fd */
                        err = errno;
                        (*ost)->sexp_pos  = wret;
                        (*ost)->sexp_part = 1;

                        return (-1);
                }
                
                wlen += (size_t)wret;
                break;
        case ATOM_NUMBER:
                /* 
                 * Number is converted to string with datatype.
                 * e.g.: 123 -> 4[int8]3:123
                 */
                
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
                        if (write (fd, &ch, sizeof ch) != sizeof ch) {
                                /* write error */
                                return (-1);
                        }
                        
                        wlen += sizeof ch;
                        
                        LIST_stack_dec (&((*ost)->lstack));
                        (*ost)->list_pos = xrealloc ((*ost)->list_pos,
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

}

ssize_t SEXP_st_dnprinta (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost)
{

}

int SEXP_printfa (SEXP_t *sexp)
{
        return SEXP_fprintfa (stdout, sexp);
}

static int __SEXP_fprintfa (FILE *fp, SEXP_t *sexp, uint32_t indent)
{
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
