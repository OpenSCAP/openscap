#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <config.h>
#include "common.h"
#include "xmalloc.h"
#include "seap.h"
#include "sexp-printf.h"

/* general */
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

/* canonical */
int SEXP_printfc (SEXP_t *sexp)
{
        return SEXP_fprintfc (stdout, sexp);
}

int SEXP_fprintfc (FILE *fp, SEXP_t *sexp)
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


/* advanced */
int SEXP_printfa (SEXP_t *sexp)
{
        return SEXP_fprintfa (stdout, sexp);
}

static int __SEXP_fprintfa (FILE *fp, SEXP_t *sexp, uint32_t indent)
{
        switch (sexp->type) {
        case ATOM_STRING:
                return fprintf (fp, "\"%.*s\" ", sexp->atom.string.len, sexp->atom.string.str);
        case ATOM_NUMBER:
                abort ();
                break;
        case ATOM_LIST: {
                uint32_t i;
                
                if (indent > 0)
                        putc ('\n', fp);
                for (i = 0; i < indent; ++i)
                        putc (' ', fp);
                putc ('(', fp);

                for (i = 0; i < sexp->atom.list.count; ++i) {
                        if (__SEXP_fprintfa (fp, SEXP(sexp->atom.list.memb) + i, indent + 1) < 0)
                                return (-1);
                }
                putc ('\b', fp);
                putc (')', fp);
                putc (' ', fp);
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

/* transport */
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
