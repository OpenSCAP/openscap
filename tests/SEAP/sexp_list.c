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

        printf ("l=%zu\n", SEXP_list_length (list));
        
        SEXP_free (list);

        i1   = SEXP_string_newf ("abc");
        i2   = SEXP_string_newf ("def");
        list = SEXP_list_new (i1, i2, NULL);
        
        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);
        
        {
                SEXP_t *r;

                r = SEXP_list_rest (list);
                
                SEXP_fprintfa (stdout, r);
                fputc ('\n', stdout);
                
                SEXP_free (r);
        }
        
        SEXP_free (i1);
        SEXP_free (i2);
        SEXP_free (list);
        
        i1   = SEXP_list_new (NULL);
        i2   = SEXP_list_new (NULL);
        list = SEXP_list_join (i1, i2);
        
        SEXP_free (i1);
        SEXP_free (i2);
        
        SEXP_fprintfa (stdout, list);
        fputc ('\n', stdout);

        SEXP_free (list);

        {
                SEXP_t *r0, *r1, *r2, *r3, *r4, *j1, *j2;

                list = SEXP_list_new (r0 = SEXP_string_newf ("asdf"),
                                      NULL);

                r1 = SEXP_list_new (NULL);
                j1 = SEXP_list_join (list, r1);
                
                SEXP_fprintfa (stdout, j1);
                fputc ('\n', stdout);
        
                j2 = SEXP_list_join (r1, list);

                SEXP_fprintfa (stdout, j2);
                fputc ('\n', stdout);
                
                SEXP_vfree (r0, r1, list, j1, j2, NULL);
                
                r0 = SEXP_list_new (r1 = SEXP_string_newf ("a"),
                                    r2 = SEXP_string_newf ("b"),
                                    NULL);
                
                r3 = SEXP_list_new (r4 = SEXP_string_newf ("c"),
                                    NULL);
                
                SEXP_vfree (r1, r2, r4, NULL);

                j1 = SEXP_list_join (r0, r3);

                SEXP_fprintfa (stdout, j1);
                fputc ('\n', stdout);
                
                j2 = SEXP_list_join (r3, r0);

                SEXP_fprintfa (stdout, j2);
                fputc ('\n', stdout);
                
                SEXP_vfree (j1, j2, r3, r0, NULL);
        }

        return (0);
}
