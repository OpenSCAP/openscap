#include <stdarg.h>
#include "_sexp-types.h"
#include "_sexp-manip.h"
#include "_sexp-parser.h"

/*
 * SEXP_template_create("[%](foo (bar %n) (baz %s) %L)");
 */

/*
 * %s - string
 * %n - number
 * %l - list
 * %L - inline list
 */
#if defined(SEAP_THREAD_SAFE)
# include <pthread.h>
static pthread_once_t __SEXP_template_once = PTHREAD_ONCE_INIT;
#else
static volatile int   __SEXP_template_once = 0;
#endif

static void SEXP_template_init(void)
{
        SEXP_datatype_t *d;

        d = SEXP_datatype_new();

        if (d == NULL)
                abort();

        SEXP_datatype_setflag(&d, SEXP_DTFLG_LOCALDATA,
                              &SEXP_template_localfree);
        SEXP_datatype_register("%", d);
}

static void SEAP_template_once(void)
{
#if defined(SEAP_THREAD_SAFE)
        if (pthread_once(&_SEXP_template_once, &SEXP_template_init) != 0)
                abort();
#else
        if (__SEXP_template_once == 0) {
                SEXP_template_init(void);
                __SEXP_template_once = 1;
        }
#endif
}

static __PARSE_RT SEXP_template_pfunc(__PARSE_PT(dsc))
{
        return (SEXP_PRET_EUNDEF);
}

static __PARSE_RT SEXP_template_wrapper_pfunc(__PARSE_PT(dsc))
{
        if (dsc->p_explen == 0) {
                if (spb_octet(dsc->p_buffer, dsc->p_bufoff) == SEXP_TEMPLATE_FMTCHAR)
                        return SEXP_template_pfunc(dsc);
        }

        return SEXP_parse_ul_string_si(dsc);
}

SEXP_t *SEXP_template_new(const char *tplstr)
{
        SEXP_t *stpl, *sexp;
        SEXP_psetup_t *psetup;
        SEXP_pstate_t *pstate;

        psetup = SEXP_psetup_new();
        pstate = NULL;

        if (SEXP_psetup_setpfunc(psetup,
                                 SEXP_PFUNC_UL_STRING_SI,
                                 SEXP_template_wrapper_pfunc) != 0)
        {
                SEXP_psetup_free(psetup);
                return(NULL);
        }

        sexp = SEXP_parse(psetup, tplstr, strlen(tplstr), &pstate);

        if (sexp == NULL || pstate != NULL) {
                assume_d(pstate != NULL, NULL);

                SEXP_pstate_free(pstate);
                SEXP_psetup_free(psetup);

                if (sexp != NULL)
                        SEXP_free(sexp);

                errno = EINVAL;
                return(NULL);
        }

        if (SEXP_list_length(sexp) != 1) {
                SEXP_free(sexp);
                SEXP_psetup_free(psetup);
                errno = EINVAL;
                return(NULL);
        }

        stpl = SEXP_list_first(sexp);
        SEXP_free(sexp);
        SEXP_psetup_free(psetup);

        return (stpl);
}

void SEXP_template_free(SEXP_t *tpl)
{
        SEXP_free(tpl);
}

SEXP_t *SEXP_template_fill(SEXP_t *tpl, ...)
{
        return(NULL);
}

SEXP_t *SEXP_template_safefill(SEXP_t *tpl, int argc, ...)
{
        return(NULL);
}
