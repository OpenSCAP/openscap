
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <strbuf.h>
#include <string.h>
#include <sexp.h>

#include "common/debug_priv.h"

static int print_sexp (SEXP_t *s_exp)
{
        strbuf_t *sb;

        /*
         * print the S-exp in advanced format
         */
        printf ("a> ");
        SEXP_fprintfa (stdout, s_exp);
        printf ("\n");
        
        /*
         * print the S-exp in transport format
         */
        printf ("t> ");
        sb = strbuf_new (8);
        SEXP_sbprintf_t (s_exp, sb);
        strbuf_fwrite (stdout, sb);
        strbuf_free (sb);
        printf ("\n");

        return (0);
}

int main (int argc, char *argv[])
{
        char  *input;
        size_t inlen;
        
        SEXP_psetup_t *psetup;
        SEXP_pstate_t *pstate;
        SEXP_t *s_exp;

        setbuf (stdout, NULL);
        setbuf (stdin,  NULL);
        
        psetup = SEXP_psetup_new ();
        pstate = NULL;

        if (argc == 1) {
                while (!feof (stdin)) {
                        input = NULL;
                        inlen = 0;

#if defined(__FreeBSD__)
                        input = fgetln (stdin, &inlen);
#elif defined(__linux__) || defined(__GLIBC__)
                        getline (&input, &inlen, stdin);
#elif defined(__SVR4) && defined(__sun)
                        getline (&input, &inlen, stdin);
#else
# error "FIXME"
#endif
                        s_exp = SEXP_parse (psetup, input, inlen, &pstate);
                        
                        if (s_exp != NULL) {
                                _A(pstate == NULL);
                                print_sexp (s_exp);
                                SEXP_free (s_exp);
                        }
                        
                        free(input);
                        input = NULL;
                        inlen = 0;
                }
        } else {
                int i;
                
                for (i = 0; i < (argc - 1); ++i) {
                        s_exp = SEXP_parse (psetup, argv[i + 1], strlen (argv[i + 1]), &pstate);
                        
                        if (s_exp != NULL) {
                                _A(pstate == NULL);
                                print_sexp (s_exp);
                                
                                SEXP_free (s_exp);
                        }
                }
                
                if (pstate != NULL)
                        return (1);
        }
        
        SEXP_psetup_free (psetup);

        return (0);
}
