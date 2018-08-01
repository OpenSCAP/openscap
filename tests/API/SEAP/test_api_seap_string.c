
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sexp.h>
#include <string.h>
#include <stdio.h>
#include "_sexp-manip.h"
#include "_sexp-output.h"

int main (void)
{
        SEXP_t *s_exp;
        char   *s1, *s2;

        s1 = "Hello, world!";
        s2 = "abcdefghijklmnopqrstuvwxyz";

        setbuf (stdout, NULL);
        
        s_exp = SEXP_string_new (s1, strlen (s1));
        SEXP_fprintfa (stdout, s_exp);
        putc ('\n', stdout);

        SEXP_free (s_exp);
        
        s_exp = SEXP_string_newf ("s2=%s", s2);
        SEXP_fprintfa (stdout, s_exp);
        putc ('\n', stdout);
        
        SEXP_free (s_exp);
        
        return (0);
}
