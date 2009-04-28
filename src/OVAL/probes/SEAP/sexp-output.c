#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <config.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "common.h"
#include "xmalloc.h"
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

/* advanced */
ssize_t SEXP_st_dprinta (int fd, SEXP_t *sexp, SEXP_ostate_t **ost)
{

}

ssize_t SEXP_st_dnprinta (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost)
{

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
