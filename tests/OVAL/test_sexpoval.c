#include <seap.h>
#include <probe.h>
#include <assert.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef _A
#define _A(x) assert(x)
#endif

#undef _E
#undef _I

#define _E(...) do {                                \
                fprintf(stderr, "tst#%u: ", tst);   \
                fprintf(stderr, __VA_ARGS__);       \
        } while (0)

#define _I(...) do {                                \
                fprintf(stdout, "tst#%u: ", tst);   \
                fprintf(stdout, __VA_ARGS__);       \
        } while (0)

int main (void)
{
        int         tst = 0;
        
        { /* random tests */
                int     integer = 123;
                double floating = 789.34;
                
                SEXP_t *num;
                SEXP_t *flt;
                SEXP_t *str;
                SEXP_t *list;
                
                SEXP_t *attr_list, *attr, *obj, *elm_list, *elm;
                
                setbuf (stdout, NULL);
                
                num = SEXP_number_new (&integer, NUM_INT32);
                flt = SEXP_number_new (&floating, NUM_DOUBLE);
                str = SEXP_string_newf ("Hello, %s!", "world");

                list = SEXP_list_new ();
                puts("-----");
                SEXP_printfa (list);
                puts("\n-----");

                /* one attribute */
                attr_list = SEXP_OVALattr_create ("attr1", num, NULL);
                if (attr_list == NULL) {
                        _E("Can't create attribute list\n");
                        return (EXIT_FAILURE);
                }

                puts("-----");
                SEXP_printfa (attr_list);
                puts("\n-----");

                SEXP_free (attr_list);
                
                ++tst;
                
                num = SEXP_number_new (&integer, NUM_INT32);
                str = SEXP_string_newf ("Hello, %s!", "world");
                
                /* two attributes */
                attr_list = SEXP_OVALattr_create ("attr2", num,
                                                  "attr3", str, NULL);
                
                if (attr_list == NULL) {
                        _E("Can't create attribute list\n");
                        return (EXIT_FAILURE);
                }

                puts("-----");
                SEXP_printfa (attr_list);
                puts("\n-----");

                SEXP_free (attr_list);
        }
        
        tst = 10;

        { /* OVAL element list:
           *  ((test "123"))
           */
                SEXP_t *elm_list, *num, *elm, *val;
                int integer = 123, inttest = 0;
                
                num = SEXP_number_new (&integer, NUM_INT32);
                elm_list = SEXP_OVALelm_create ("test", NULL, num, 
                                                NULL);
                
                SEXP_VALIDATE(elm_list);

                puts("-----");
                SEXP_printfa (elm_list);
                puts("\n-----");

                elm = SEXP_list_first (elm_list);
                if (elm == NULL) {
                        _E("No element in list\n");
                        return (EXIT_FAILURE);
                }

                SEXP_VALIDATE(elm);
                
                ++tst;
                
                val = SEXP_OVALelm_getval (elm, 1);
                if (val == NULL) {
                        _E("No value in element\n");
                        return (EXIT_FAILURE);
                }
                
                SEXP_VALIDATE(val);
                
                ++tst;
                
                SEXP_number_get (val, &inttest, NUM_INT32);
                if (integer != inttest) {
                        _E("%d != %d\n", integer, inttest);
                        return (EXIT_FAILURE);
                }
                
                SEXP_free (elm_list);
        }
        
        tst = 20;
        
        { /* OVAL element:
           *  ((test :attr1 "Hello") 123)
           */
                int integer = 123, inttest;
                SEXP_t *elm_list, *elm, *val;
                
                elm_list = SEXP_OVALelm_create ("test",
                                                SEXP_OVALattr_create ("attr1", SEXP_string_newf ("Hello"),
                                                                      NULL),
                                                SEXP_number_new (&integer, NUM_INT32),
                                                NULL /* end */);
                
                SEXP_VALIDATE(elm_list);
                
                puts("-----");
                SEXP_printfa (elm_list);
                puts("\n-----");

                elm = SEXP_list_first (elm_list);
                if (elm == NULL) {
                        _E("No element in list\n");
                        return (EXIT_FAILURE);
                }

                SEXP_VALIDATE(elm);
                
                ++tst;
                
                val = SEXP_OVALelm_getval (elm, 1);
                if (val == NULL) {
                        _E("No value in element\n");
                        return (EXIT_FAILURE);
                }
                
                SEXP_VALIDATE(val);
                
                ++tst;
                
                SEXP_number_get (val, &inttest, NUM_INT32);
             
                if (integer != inttest) {
                        _E("%d != %d\n", integer, inttest);
                        return (EXIT_FAILURE);
                }
        
                ++tst;
        
                if (!SEXP_OVALelm_hasattr (elm, "attr1")) {
                        _E("hasattr(attr1) = FALSE\n");
                        return (EXIT_FAILURE);
                }
                
                ++tst;

                val = SEXP_OVALelm_getattrval (elm, "attr1");
                if (val == NULL) {
                        _E("No value in attribute\n");
                        return (EXIT_FAILURE);
                }
                
                SEXP_VALIDATE(val);

                ++tst;
                
                if (SEXP_strcmp (val, "Hello") != 0) {
                        _E("attrval != \"Hello\"\n");
                        return (EXIT_FAILURE);
                }
                
                SEXP_free (elm_list);
        }
        
        tst = 30;

        { /* OVAL element:
           *  ((test noarg :attr1 "Hello") "123")
           */
                        int integer = 123, inttest;
                SEXP_t *elm_list, *elm, *val;
                
                elm_list = SEXP_OVALelm_create ("test",
                                                SEXP_OVALattr_create ("attr1", SEXP_string_newf ("Hello"),
                                                                      "noarg", NULL,
                                                                      NULL),
                                                SEXP_number_new (&integer, NUM_INT32),
                                                NULL /* end */);
                
                SEXP_VALIDATE(elm_list);

                puts("-----");
                SEXP_printfa (elm_list);
                puts("\n-----");

                elm = SEXP_list_first (elm_list);
                if (elm == NULL) {
                        _E("No element in list\n");
                        return (EXIT_FAILURE);
                }

                SEXP_VALIDATE(elm);
                
                ++tst;
                
                val = SEXP_OVALelm_getval (elm, 1);
                if (val == NULL) {
                        _E("No value in element\n");
                        return (EXIT_FAILURE);
                }
                
                SEXP_VALIDATE(val);
                
                ++tst;
                
                SEXP_number_get (val, &inttest, NUM_INT32);
                if (integer != inttest) {
                        _E("%d != %d\n", integer, inttest);
                        return (EXIT_FAILURE);
                }
        
                ++tst;

                if (!SEXP_OVALelm_hasattr (elm, "noarg")) {
                        _E("hasattr(noarg) = FALSE\n");
                        return (EXIT_FAILURE);
                }
                
                ++tst;
                
                val = SEXP_OVALelm_getattrval (elm, "noarg");
                if (val != NULL) {
                        _E("non-NULL value in attribute \"noarg\"\n");
                        return (EXIT_FAILURE);
                }
                
                ++tst;
                        
                if (!SEXP_OVALelm_hasattr (elm, "attr1")) {
                        _E("hasattr(attr1) = FALSE\n");
                        return (EXIT_FAILURE);
                }
                
                ++tst;

                val = SEXP_OVALelm_getattrval (elm, "attr1");
                if (val == NULL) {
                        _E("No value in attribute\n");
                        return (EXIT_FAILURE);
                }
                
                SEXP_VALIDATE(val);

                ++tst;
                
                if (SEXP_strcmp (val, "Hello") != 0) {
                        _E("attrval != \"Hello\"\n");
                        return (EXIT_FAILURE);
                }
                
                SEXP_free (elm_list);
        }
        
        tst = 40;

        { /* OVAL element:
           *  ((test noarg) "123")
           */
                
        }

        tst = 50;

        { /* OVAL element:
           *  ((test :arg1 3 :arg2 2 :arg 1) 2^24)
           */
                SEXP_t *elm_list, *elm, *val;
                int a, b, c, d, test;

                a = 3;
                b = 2;
                c = 1;
                d = 1<<24;
                
                elm_list = SEXP_OVALelm_create ("test",
                                                SEXP_OVALattr_create ("arg1", SEXP_number_new (&a, NUM_INT32),
                                                                      "arg2", SEXP_number_new (&b, NUM_INT32),
                                                                      "arg" , SEXP_number_new (&c, NUM_INT32),
                                                                      NULL),
                                                SEXP_number_new (&d, NUM_INT32),
                                                NULL);
                
                elm = SEXP_list_first (elm_list);

                SEXP_VALIDATE(elm);
                
                puts("-----");
                SEXP_printfa (elm);
                puts("\n-----");

                val = SEXP_OVALelm_getattrval (elm, "arg1");

                SEXP_number_get (val, &test, NUM_INT32);
                if (test != a) {
                        _E("attrval(arg1=%d) != %d\n", test, a);
                        return (EXIT_FAILURE);
                }

                ++tst;

                val = SEXP_OVALelm_getattrval (elm, "arg2");
                
                SEXP_number_get (val, &test, NUM_INT32);
                if (test != b) {
                        _E("attrval(arg1) != %d\n", b);
                        return (EXIT_FAILURE);
                }

                ++tst;

                val = SEXP_OVALelm_getattrval (elm, "arg");
                
                SEXP_number_get (val, &test, NUM_INT32);
                if (test != c) {
                        _E("attrval(arg1) != %d\n", c);
                        return (EXIT_FAILURE);
                }

                ++tst;
                
                val = SEXP_OVALelm_getval (elm, 1);
                
                SEXP_number_get (val, &test, NUM_INT32);
                if (test != d) {
                        _E("attrval(arg1) != %d\n", d);
                        return (EXIT_FAILURE);
                }

                SEXP_free (elm_list);
        }
        
        tst = 60;

        { /* OVAL object:
           *  (rpminfo_object (name "sshd") (epoch "(none)") (version "5.0"))
           */
                
                /* create */
                
                /* validate */
                
                /* search element */
                
                /* free */
        }

        tst = 70;

        { /* OVAL object:
           *  ((rpminfo_object :id 1 :scheme "url://foo.bar") (name "sshd") (epoch "(none)") (version "5.0"))
           */

                /* create */
                
                /* validate */
                
                /* search element */
                
                /* free */
        }
        
        tst = 80;

        { /* OVAL object:
           *  ((rpminfo_object :id 1 :scheme "url://foo.bar") (name "sshd") ((epoch :attr1 "123") "(none)") (version "5.0"))
           */
                
                /* create */
                
                /* validate */
                
                /* search element */
                
                /* free */
                
        }
        
        tst = 90;

        { /* OVAL object:
           *  ((rpminfo_object :id 1 noarg :scheme "url://foo.bar") (name "sshd") (epoch "(none)") (version "5.0"))
           */

                /* create */
                
                /* validate */
                
                /* search element */
                
                /* free */
        }
        
        return (EXIT_SUCCESS);
}
