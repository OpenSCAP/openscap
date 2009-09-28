#include <sexp.h>
#include <string.h>

int main (void)
{
        SEXP_t *list;
        SEXP_t *i1, *i2;
        
        setbuf (stdout, NULL);

        i1   = SEXP_number_newu_8 (123);
        i2   = SEXP_string_newf ("asdf");
        
        SEXP_fprintfa (stdout, i1);
        fputc ('\n', stdout);
        SEXP_fprintfa (stdout, i2);
        fputc ('\n', stdout);

        list = SEXP_list_new (i1, i2, NULL);
        
        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_free (list);
        
        list = SEXP_list_new (NULL);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);

        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_list_add (list, i1);
        SEXP_list_add (list, i2);
        
        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);
        
        {
                SEXP_t  *m;
                uint32_t i;

                for (i = 1, m = SEXP_list_nth (list, i); m != NULL; m = SEXP_list_nth (list, ++i)) {
                        SEXP_fprintfa (stdout, m);
                        fputc ('\n', stdout);
                        SEXP_free (m);
                }
        }

        SEXP_free (i1);
        SEXP_free (i2);

        printf ("l=%u\n", SEXP_list_length (list));
        
        SEXP_free (list);
        
        return (0);
}
