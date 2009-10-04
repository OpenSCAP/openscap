#include <string.h>
#include <stdlib.h>
#include <seap.h>
#include <probe-api.h>

#define FAIL(ret, ...)                                        \
        do {                                                  \
                fprintf (stderr, "FAIL: " __VA_ARGS__);       \
                exit (ret);                                   \
        } while (0)

#define WARN(...) fprintf (stderr, "WARN: " __VA_ARGS__)

void print_asdf (SEXP_t *asdf)
{
        (void) SEXP_fprintfa (stdout, asdf);
        (void) fputc ('\n', stdout);
        return;
}

int main (void)
{
        SEXP_t *obj, *val, *ent;
        
        char  *string;
        char   buffer[1024];
        size_t buflen;

        /*
         *  (test_object)
         */
        obj = probe_obj_creat ("test_object", NULL, NULL);
        print_asdf (obj);
        
        string = probe_obj_getname (obj);
        
        if (string != NULL) {
                if (strcmp (string, "test_object") != 0)
                        FAIL(1, "getname: %s != %s\n", "test_object", string);
        
                free (string);
        } else
                FAIL(1, "getname: string == NULL\n");
        
        buflen = probe_obj_getname_r (obj, buffer, sizeof buffer);
        
        if (buflen != strlen ("test_object"))
                FAIL(1, "getname_r: buflen=%zu != %zu\n", buflen, strlen ("test_object"));
        
        if (strcmp (buffer, "test_object") != 0)
                FAIL(1, "getname_r: %s != %s\n", "test_object", buffer);

        ent = probe_obj_getent (obj, "nonexistent", 1);

        if (ent != NULL)
                FAIL(1, "obj_getent: != NULL\n");

        probe_free (ent);

        val = probe_obj_getentval (obj, "nonexistent", 1);
        
        if (val != NULL)
                FAIL(1, "obj_getentval: != NULL\n");
        
        probe_free (val);
        
        if (probe_obj_getentvals (obj, "nonexistent", 1, &val) != 0)
                FAIL(1, "obj_getentvals: != 0\n");
        
        if (probe_obj_attrexists (obj, "nonexistent"))
                FAIL(1, "obj_attrexists: != false");
        
        probe_free (obj);

        /*
         * (test_object (ent1 123))
         */
        {
                SEXP_t *ent1val = SEXP_number_newu_8 (123);
                
                obj = probe_obj_creat ("test_object", NULL,
                                       /* entities */
                                       "ent1", NULL, ent1val,
                                       NULL);
                SEXP_free (ent1val);
        }
        
        print_asdf (obj);
        
        ent = probe_obj_getent (obj, "ent1", 1);
        
        if (ent == NULL)
                FAIL(1, "obj_getent(%s): == NULL\n", "ent1");
        
        probe_free (ent);

        val = probe_obj_getentval (obj, "ent1", 1);
        
        if (val == NULL)
                FAIL(1, "obj_getentval(%s): == NULL\n", "ent1");
        else {
                uint8_t nval;
                
                nval = SEXP_number_getu_8 (val);
                
                if (nval != 123)
                        FAIL(1, "obj_getentval(%s): != 123 (== %u)\n", "ent1", nval);
                
        }
        
        probe_free (val);
        
        if (probe_obj_getentvals (obj, "ent1", 1, &val) != 1)
                FAIL(1, "obj_getentvals(%s): != 1\n", "ent1");
        
        if (val == NULL)
                FAIL(1, "obj_getentvals(%s): val == NULL\n", "ent1");
        else {
                if (!SEXP_listp (val))
                        FAIL(1, "obj_getentvals(%s): listp(val) == false\n", "ent1");
                else {
                        if (SEXP_list_length (val) != 1)
                                FAIL(1, "obj_getentvals(%s): list_length(val) != 1\n", "ent1");
                        else {
                                SEXP_t *lval;
                                uint8_t nval;
                                
                                lval = SEXP_list_first (val);
                                nval = SEXP_number_getu_8 (lval);

                                if (nval != 123)
                                        FAIL(1, "obj_getentvals(%s): val != 123 (== %u)\n", "ent1", nval);
                                
                                SEXP_free (lval);
                        }
                }
        }
        
        SEXP_free (val);
        probe_free (obj);
        
        /*
         * ((test_object :attr1 "abc"))
         */
        {
                SEXP_t *attrs, *str;
                
                str   = SEXP_string_newf ("abc");
                attrs = probe_attr_creat ("attr1", str, NULL);
                obj   = probe_obj_creat ("test_object", attrs, NULL);

                SEXP_free (str);
                SEXP_free (attrs);
        }

        print_asdf (obj);
        
        ent = probe_obj_getent (obj, "ent1", 1);
        
        if (ent != NULL)
                FAIL(1, "obj_getent(%s): != NULL\n", "ent1");
        
        if (probe_obj_getentvals (obj, "ent1", 1, &val) != 0)
                FAIL(1, "obj_getentvals(%s): != 0\n", "ent1");
        
        val = probe_obj_getattrval (obj, "attr1");
        
        if (val == NULL)
                FAIL(1, "obj_getattrval(%s): == NULL\n", "attr1");
        else {
                char *strval;

                strval = SEXP_string_cstr (val);
                
                if (strval == NULL)
                        FAIL(1, "obj_getattrval(%s): cstr == NULL\n", "attr1");
                else {
                        if (strcmp (strval, "abc") != 0)
                                FAIL(1, "obj_getattrval(%s): != \"abc\"\n", "attr1");
                        
                        free (strval);
                }

                if (SEXP_strcmp (val, "abc") != 0)
                        FAIL(1, "obj_getattrval(%s): sexp: != \"abc\"\n", "attr1");
                
                SEXP_free (val);
        }

        val = probe_obj_getattrval (obj, "nonexistent");

        if (val != NULL)
                FAIL(1, "obj_getattrval(%s): != NULL\n", "nonexistent");
        
        probe_free (obj);

        /*
         * ((test_object :attr1 123) (ent1 "abc"))
         */
        
        /*
         * ((test_object :attr1 123) (ent1 "abc") (ent2 123) (ent3 456) (ent4))
         */
        
        /*
         * ((test_object :attr1 123) ((ent1 :entattr1 456) "abc"))
         */
        
        
        return (0);
}
