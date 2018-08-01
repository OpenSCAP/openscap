
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sexp.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "_sexp-manip.h"

int main (void)
{
        SEXP_t *list;
        SEXP_t *i1, *i2;
        
        setbuf (stdout, NULL);

        i1   = SEXP_list_new (NULL);
        i2   = SEXP_softref (i1);
	SEXP_free(i1);
	SEXP_free(i2);
        
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
                
		SEXP_free(r0);
		SEXP_free(r1);
		SEXP_free(list);
		SEXP_free(j1);
		SEXP_free(j2);
                
                r0 = SEXP_list_new (r1 = SEXP_string_newf ("a"),
                                    r2 = SEXP_string_newf ("b"),
                                    NULL);
                
                r3 = SEXP_list_new (r4 = SEXP_string_newf ("c"),
                                    NULL);
                
		SEXP_free(r1);
		SEXP_free(r2);
		SEXP_free(r4);

                j1 = SEXP_list_join (r0, r3);

                SEXP_fprintfa (stdout, j1);
                fputc ('\n', stdout);
                
                j2 = SEXP_list_join (r3, r0);

                SEXP_fprintfa (stdout, j2);
                fputc ('\n', stdout);
                
		SEXP_free(j1);
		SEXP_free(j2);
		SEXP_free(r3);
		SEXP_free(r0);
        }

	{
		/* test SEXP_list_replace(), SEXP_softref() */
		SEXP_t *l1, *l2, *l3, *l4, *r0, *r1, *r2, *r3;

		l1 = SEXP_list_new(r0 = SEXP_string_newf ("a"),
				   r1 = SEXP_string_newf ("b"),
				   r2 = SEXP_string_newf ("x"),
				   r3 = SEXP_string_newf ("d"),
				   NULL);
		SEXP_free(r0);
		SEXP_free(r1);
		SEXP_free(r2);
		SEXP_free(r3);

		SEXP_fprintfa (stdout, l1);
		fputc('\n', stdout);

		r0 = SEXP_list_replace (l1, 3, r1 = SEXP_string_newf ("c"));
		SEXP_free(r0);
		SEXP_free(r1);

		SEXP_fprintfa (stdout, l1);
		fputc('\n', stdout);

		l2 = SEXP_ref(l1);
		r0 = SEXP_list_replace (l1, 2, r1 = SEXP_string_newf ("B"));
		SEXP_free(r0);
		SEXP_free(r1);

		SEXP_fprintfa (stdout, l1);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l2);
		fputs("\n---\n", stdout);

		SEXP_free(l1);
		SEXP_free(l2);

		l1 = SEXP_list_new(r0 = SEXP_string_newf ("x"),
				   r1 = SEXP_string_newf ("y"),
				   r2 = SEXP_string_newf ("z"),
				   NULL);
		SEXP_free(r0);
		SEXP_free(r1);
		SEXP_free(r2);
		l2 = SEXP_softref(l1);
		l3 = SEXP_ref(l1);

		SEXP_fprintfa (stdout, l1);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l2);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l3);
		fputs("\n\n", stdout);

		r0 = SEXP_list_replace (l3, 3, r1 = SEXP_string_newf ("Z"));
		SEXP_free(r0);
		SEXP_free(r1);

		SEXP_fprintfa (stdout, l1);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l2);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l3);
		fputs("\n\n", stdout);

		r0 = SEXP_list_replace (l2, 2, r1 = SEXP_string_newf ("Y"));
		SEXP_free(r0);
		SEXP_free(r1);

		SEXP_fprintfa (stdout, l1);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l2);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l3);
		fputs("\n---\n", stdout);

		SEXP_free(l1);
		SEXP_free(l2);
		SEXP_free(l3);

		l1 = SEXP_list_new(r0 = SEXP_string_newf ("d"),
				   r1 = SEXP_string_newf ("e"),
				   r2 = SEXP_string_newf ("f"),
				   r3 = SEXP_string_newf ("g"),
				   NULL);
		SEXP_free(r0);
		SEXP_free(r1);
		SEXP_free(r3);
		l2 = SEXP_ref(l1);
		l3 = SEXP_ref(l1);
		l4 = SEXP_softref(l3);

		SEXP_fprintfa (stdout, l1);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l2);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l3);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l4);
		fputs("\n\n", stdout);

		r0 = SEXP_list_replace (l4, 4, r1 = SEXP_string_newf ("G"));
		SEXP_free(r0);
		SEXP_free(r1);

		SEXP_fprintfa (stdout, l1);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l2);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l3);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l4);
		fputs("\n\n", stdout);

		r0 = SEXP_list_replace (l1, 1, r1 = SEXP_string_newf ("D"));
		SEXP_free(r0);
		SEXP_free(r1);

		SEXP_fprintfa (stdout, l1);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l2);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l3);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l4);
		fputs("\n\n", stdout);

		r0 = SEXP_list_replace (l2, 2, r1 = SEXP_string_newf ("E"));
		SEXP_free(r0);
		SEXP_free(r1);

		SEXP_fprintfa (stdout, l1);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l2);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l3);
		fputc('\n', stdout);
		SEXP_fprintfa (stdout, l4);
		fputs("\n---\n", stdout);

		SEXP_free(l1);
		SEXP_free(l2);
		SEXP_free(l3);
	}

        {
                SEXP_t *l1, *r0, *r1, *r2, *r3;
                SEXP_t *r4, *r5, *r6, *r7;
                SEXP_t *r8, *r9, *r10, *r11;
                SEXP_t *r12, *r13, *r14, *r15;

                r0  = SEXP_string_newf ("a");
                r1  = SEXP_string_newf ("b");
                r2  = SEXP_string_newf ("c");
                r3  = SEXP_string_newf ("d");
                r4  = SEXP_string_newf ("e");
                r5  = SEXP_string_newf ("f");
                r6  = SEXP_string_newf ("g");
                r7  = SEXP_string_newf ("h");
                r8  = SEXP_string_newf ("i");
                r9  = SEXP_string_newf ("j");
                r10 = SEXP_string_newf ("k");
                r11 = SEXP_string_newf ("l");
                r12 = SEXP_string_newf ("m");
                r13 = SEXP_string_newf ("n");
                r14 = SEXP_string_newf ("o");
                r15 = SEXP_string_newf ("p");

                printf("a = %"PRIuPTR"\n"
                       "b = %"PRIuPTR"\n"
                       "c = %"PRIuPTR"\n"
                       "d = %"PRIuPTR"\n"
                       "e = %"PRIuPTR"\n"
                       "f = %"PRIuPTR"\n"
                       "g = %"PRIuPTR"\n"
                       "h = %"PRIuPTR"\n"
                       "i = %"PRIuPTR"\n"
                       "j = %"PRIuPTR"\n"
                       "k = %"PRIuPTR"\n"
                       "l = %"PRIuPTR"\n"
                       "m = %"PRIuPTR"\n"
                       "n = %"PRIuPTR"\n"
                       "o = %"PRIuPTR"\n"
                       "p = %"PRIuPTR"\n",
                       r0->s_valp, r1->s_valp, r2->s_valp, r3->s_valp,
                       r4->s_valp, r5->s_valp, r6->s_valp, r7->s_valp,
                       r8->s_valp, r9->s_valp, r10->s_valp, r11->s_valp,
                       r12->s_valp, r13->s_valp, r14->s_valp, r15->s_valp);

                /* 16 */
                printf("//16\n");
                l1 = SEXP_list_new(r3, NULL);

                SEXP_list_add(l1, r4);
                SEXP_list_add(l1, r6);
                SEXP_list_add(l1, r5);
                SEXP_list_add(l1, r2);
                SEXP_list_add(l1, r11);
                SEXP_list_add(l1, r10);
                SEXP_list_add(l1, r15);
                SEXP_list_add(l1, r8);
                SEXP_list_add(l1, r1);
                SEXP_list_add(l1, r0);
                SEXP_list_add(l1, r12);
                SEXP_list_add(l1, r7);
                SEXP_list_add(l1, r9);
                SEXP_list_add(l1, r13);
                SEXP_list_add(l1, r14);

                l1 = SEXP_list_sort(l1, SEXP_refcmp);

                SEXP_fprintfa(stdout, l1);
                fputc('\n', stdout);
                SEXP_free(l1);

                /* 4 */
                printf("//4\n");
                l1 = SEXP_list_new(r3, r1, r0, r2, NULL);
                l1 = SEXP_list_sort(l1, SEXP_refcmp);

                SEXP_fprintfa(stdout, l1);
                fputc('\n', stdout);
                SEXP_free(l1);

                /* 3 */
                printf("//3\n");
                l1 = SEXP_list_new(r3, r1, r0, NULL);
                l1 = SEXP_list_sort(l1, SEXP_refcmp);

                SEXP_fprintfa(stdout, l1);
                fputc('\n', stdout);
                SEXP_free(l1);

                /* 2 */
                printf("//2\n");
                l1 = SEXP_list_new(r3, r1, NULL);
                l1 = SEXP_list_sort(l1, SEXP_refcmp);

                SEXP_fprintfa(stdout, l1);
                fputc('\n', stdout);
                SEXP_free(l1);

                /* 1 */
                printf("//1\n");
                l1 = SEXP_list_new(r3, NULL);
                l1 = SEXP_list_sort(l1, SEXP_refcmp);

                SEXP_fprintfa(stdout, l1);
                fputc('\n', stdout);
                SEXP_free(l1);

                /* 0 */
                printf("//0\n");
                l1 = SEXP_list_new(NULL);
                l1 = SEXP_list_sort(l1, SEXP_refcmp);

                SEXP_fprintfa(stdout, l1);
                fputc('\n', stdout);
                SEXP_free(l1);

                /* NULL */
                printf("//NULL\n");
                l1 = SEXP_list_sort(NULL, SEXP_refcmp);

                if (l1 != NULL) {
                        printf("!= NULL\n");
                        SEXP_fprintfa(stdout, l1);
                        fputc('\n', stdout);
                        SEXP_free(l1);
                }

		SEXP_free(r0);
		SEXP_free(r1);
		SEXP_free(r3);
        }

        return (0);
}
