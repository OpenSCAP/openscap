
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <seap.h>
#include <probe-api.h>
#include "common/memusage.h"

#define FAIL(ret, ...)                                        \
        do {                                                  \
                fprintf (stderr, "FAIL: " __VA_ARGS__);       \
                exit (ret);                                   \
        } while (0)

#define WARN(...) fprintf (stderr, "WARN: " __VA_ARGS__)

static void print_asdf (SEXP_t *asdf)
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
        {
                SEXP_t *attrs, *str, *num;

                str = SEXP_string_newf ("abc");
                num = SEXP_number_newu_8 (123);

                attrs = probe_attr_creat ("attr1", num, NULL);
                obj   = probe_obj_creat ("test_object", attrs,
                                         "ent1", NULL, str,
                                         NULL);

                SEXP_free (str);
                SEXP_free (num);
                SEXP_free (attrs);
        }

        val = probe_obj_getattrval (obj, "attr1");
        
        if (val == NULL)
                FAIL(1, "obj_getattrval(%s): == NULL\n", "attr1");
        else {
                uint8_t nval;

                nval = SEXP_number_getu_8 (val);
                
                if (nval != 123)
                        FAIL(1, "obj_getattrval(%s): != 123 (= %hhu)\n", "attr1", nval);
                
                SEXP_free (val);
        }

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
                                
                                lval = SEXP_list_first (val);

                                if (SEXP_strcmp (lval, "abc") != 0)
                                        FAIL(1, "obj_getentvals(%s): val != \"abc\"\n", "ent1");
                                
                                SEXP_free (lval);
                        }
                }
        }
        
        SEXP_free (val);
        probe_free (obj);

        /*
         * ((test_object :attr1 123) (ent1 "abc") (ent2 123) (ent3 456) (ent4))
         */
        
        {
                SEXP_t *a1val, *e1val, *e3val, *attrs;

                a1val = SEXP_number_newu_8 (123);
                e1val = SEXP_string_newf ("abc");
                e3val = SEXP_number_newu_16 (456);

                attrs = probe_attr_creat ("attr1", a1val, NULL);
                obj   = probe_obj_creat ("test_object", attrs,
                                         "ent1", NULL, e1val,
                                         "ent2", NULL, a1val,
                                         "ent3", NULL, e3val,
                                         "ent4", NULL, NULL,
                                         NULL);
                
                SEXP_free (attrs);
                SEXP_free (a1val);
                SEXP_free (e1val);
                SEXP_free (e3val);
        }

        print_asdf (obj);

        val = probe_obj_getattrval (obj, "attr1");
        
        if (val == NULL)
                FAIL(1, "obj_getattrval(%s): == NULL\n", "attr1");
        else {
                uint8_t nval;

                nval = SEXP_number_getu_8 (val);
                
                if (nval != 123)
                        FAIL(1, "obj_getattrval(%s): != 123 (= %hhu)\n", "attr1", nval);
                
                SEXP_free (val);
        }
        
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
                                
                                lval = SEXP_list_first (val);

                                if (SEXP_strcmp (lval, "abc") != 0)
                                        FAIL(1, "obj_getentvals(%s): val != \"abc\"\n", "ent1");
                                
                                SEXP_free (lval);
                        }
                }
        }

        SEXP_free (val);
        
        if (probe_obj_getentvals (obj, "ent2", 1, &val) != 1)
                FAIL(1, "obj_getentvals(%s): != 1\n", "ent2");
        
        if (val == NULL)
                FAIL(1, "obj_getentvals(%s): val == NULL\n", "ent2");
        else {
                if (!SEXP_listp (val))
                        FAIL(1, "obj_getentvals(%s): listp(val) == false\n", "ent2");
                else {
                        if (SEXP_list_length (val) != 1)
                                FAIL(1, "obj_getentvals(%s): list_length(val) != 1\n", "ent2");
                        else {
                                SEXP_t *lval;
                                uint8_t nval;
                                
                                lval = SEXP_list_first (val);
                                nval = SEXP_number_getu_8 (lval);

                                if (nval != 123)
                                        FAIL(1, "obj_getentvals(%s): val != 123 (= %hhu)\n", "ent2", nval);
                                
                                SEXP_free (lval);
                        }
                }
        }

        SEXP_free (val);
        
        if (probe_obj_getentvals (obj, "ent3", 1, &val) != 1)
                FAIL(1, "obj_getentvals(%s): != 1\n", "ent3");
        
        if (val == NULL)
                FAIL(1, "obj_getentvals(%s): val == NULL\n", "ent3");
        else {
                if (!SEXP_listp (val))
                        FAIL(1, "obj_getentvals(%s): listp(val) == false\n", "ent3");
                else {
                        if (SEXP_list_length (val) != 1)
                                FAIL(1, "obj_getentvals(%s): list_length(val) != 1\n", "ent3");
                        else {
                                SEXP_t *lval;
                                uint16_t nval;
                                
                                lval = SEXP_list_first (val);
                                nval = SEXP_number_getu_16 (lval);

                                if (nval != 456)
                                        FAIL(1, "obj_getentvals(%s): val != 456 (= %hu)\n", "ent3", nval);
                                
                                SEXP_free (lval);
                        }
                }
        }
        
        SEXP_free (val);
        
        if (probe_obj_getentvals (obj, "ent4", 1, &val) != 0)
                FAIL(1, "obj_getentvals(%s): != 0\n", "ent4");
                
        probe_free (obj);


        /*
         * ((test_object :attr1 123) ((ent1 :entattr1 456) "abc"))
         */

        {
                SEXP_t *a1val, *e1a1val, *e1val, *oatt, *e1att;

                a1val = SEXP_number_newu_8 (123);
                e1a1val = SEXP_number_newu_16 (456);
                e1val = SEXP_string_newf ("abc");
                
                oatt  = probe_attr_creat ("attr1", a1val, NULL);
                e1att = probe_attr_creat ("entattr1", e1a1val, NULL);

                obj = probe_obj_creat ("test_object", oatt,
                                       "ent1", e1att, e1val,
                                       NULL);

                SEXP_free (a1val);
                SEXP_free (e1a1val);
                SEXP_free (e1val);
                SEXP_free (oatt);
                SEXP_free (e1att);
        }

        print_asdf (obj);

        val = probe_obj_getattrval (obj, "attr1");
        
        if (val == NULL)
                FAIL(1, "obj_getattrval(%s): == NULL\n", "attr1");
        else {
                uint8_t nval;

                nval = SEXP_number_getu_8 (val);
                
                if (nval != 123)
                        FAIL(1, "obj_getattrval(%s): != 123 (= %hhu)\n", "attr1", nval);
                
                SEXP_free (val);
        }
        
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
                                
                                lval = SEXP_list_first (val);

                                if (SEXP_strcmp (lval, "abc") != 0)
                                        FAIL(1, "obj_getentvals(%s): val != \"abc\"\n", "ent1");
                                
                                SEXP_free (lval);
                        }
                }
        }

        SEXP_free (val);
        
        {
                SEXP_t *ent2;

                ent2 = probe_obj_getent (obj, "ent1", 1);

                if (ent2 == NULL)
                        FAIL(1, "obj_getent(%s): == NULL\n", "ent1");
                
                val = probe_ent_getattrval (ent2, "entattr1");
                
                if (val == NULL)
                        FAIL(1, "ent_getattrval(%s): == NULL\n", "entattr1");
                else {
                        uint16_t nval;
                        
                        nval = SEXP_number_getu_16 (val);
                        
                        if (nval != 456)
                                FAIL(1, "ent_getattrval(%s): val != 456 (= %hu)\n", "entattr1", nval);
                }
                
                SEXP_free (ent2);
                SEXP_free (val);
        }

        SEXP_free (obj);

        {
	        SEXP_t *ent;

	        struct test {
		        oval_datatype_t t;
		        const char     *v;
	        } __test_array[] = {
		        { OVAL_DATATYPE_FLOAT,   "1.2345" },
		        { OVAL_DATATYPE_INTEGER, "12345" },
		        { OVAL_DATATYPE_BOOLEAN, "1" },
		        { OVAL_DATATYPE_BOOLEAN, "0" },
		        { OVAL_DATATYPE_BOOLEAN, "true" },
		        { OVAL_DATATYPE_BOOLEAN, "false" },
		        { OVAL_DATATYPE_IPV4ADDR, "192.168.0.1" },
		        { OVAL_DATATYPE_IPV6ADDR, "fe80::f2de:f1ff:fe6a:13fd" },
		        { OVAL_DATATYPE_EVR_STRING, "1.7.2p1" },
		        { OVAL_DATATYPE_FILESET_REVISION, "12345" },
		        { OVAL_DATATYPE_IOS_VERSION, "1234" },
		        { OVAL_DATATYPE_STRING, "Hello world!" },
		        { OVAL_DATATYPE_VERSION, "1.7.2p1" }
	        };

	        for (unsigned int i = 0;
	             i < sizeof __test_array/(sizeof(struct test)); ++i)
		        {
			        ent = probe_ent_from_cstr("test",
			                                  __test_array[i].t,
			                                  __test_array[i].v,
			                                  strlen(__test_array[i].v));
			        if (ent == NULL)
				        FAIL(1, "probe_ent_from_cstr(%s, %d, %s, %zu) == NULL\n",
				             "test", __test_array[i].t,
				             __test_array[i].v,
				             strlen(__test_array[i].v));
			        print_asdf(ent);
			        SEXP_free(ent);
		        }
        }

        {
	        struct sys_memusage mu_sys;
	        struct proc_memusage mu_proc;

	        if (oscap_sys_memusage(&mu_sys) != 0)
		        FAIL(1, "oscap_sys_memusage != 0\n");
	        if (oscap_proc_memusage(&mu_proc) != 0)
		        FAIL(1, "oscap_proc_memusage != 0\n");

	        printf("mu_sys:\n"
	               "   mu_active: %zu\n"
	               "  mu_buffers: %zu\n"
	               "   mu_cached: %zu\n"
	               " mu_inactive: %zu\n"
	               "     mu_free: %zu\n"
	               "    mu_total: %zu\n"
	               " mu_realfree: %zu\n",
	               mu_sys.mu_active,
	               mu_sys.mu_buffers,
	               mu_sys.mu_cached,
	               mu_sys.mu_inactive,
	               mu_sys.mu_free,
	               mu_sys.mu_total,
	               mu_sys.mu_realfree);

	        printf("mu_proc:\n"
	               "  mu_data: %zu\n"
	               "  mu_text: %zu\n"
	               "   mu_hwm: %zu\n"
	               "  mu_lock: %zu\n"
	               "   mu_lib: %zu\n"
	               "   mu_rss: %zu\n"
	               " mu_stack: %zu\n",
	               mu_proc.mu_data,
	               mu_proc.mu_text,
	               mu_proc.mu_hwm,
	               mu_proc.mu_lock,
	               mu_proc.mu_lib,
	               mu_proc.mu_rss,
	               mu_proc.mu_stack);
        }

        /*
         * ((test_object :attr1 123) ((ent1 :entattr1 456) "abc"))
         */

        {
            SEXP_t *a1val, *e1a1val, *e1val, *oatt, *e1att;

            a1val = SEXP_number_newu_8 (123);
            e1a1val = SEXP_number_newu_16 (456);
            e1val = SEXP_string_newf ("abc");

            oatt  = probe_attr_creat ("attr1", a1val, NULL);
            e1att = probe_attr_creat ("entattr1", e1a1val, NULL);

            obj = probe_obj_creat ("test_object", oatt,
                                   "ent1", e1att, e1val,
                                   NULL);

            SEXP_free (a1val);
            SEXP_free (e1a1val);
            SEXP_free (e1val);
            SEXP_free (oatt);
            SEXP_free (e1att);
        }

        print_asdf (obj);

        {
            ent = probe_obj_getent(obj, "ent1", 1);
            if (ent == NULL)
               FAIL(1, "`ent1' not found!\n");
            if (probe_ent_attrexists(ent, "mask"))
                FAIL(1, "`mask' attribute found in obj!");
            SEXP_free(ent);
        }
        SEXP_free(obj);

        /*
         * ((test_object :attr1 123) ((ent1 :entattr1 456 mask :attr2 345) "abc"))
         */

        {
            SEXP_t *a1val, *e1a1val, *e1val, *oatt, *e1att;

            a1val = SEXP_number_newu_8 (123);
            e1a1val = SEXP_number_newu_16 (456);
            e1val = SEXP_string_newf ("abc");

            oatt  = probe_attr_creat ("attr1", a1val, NULL);
            e1att = probe_attr_creat ("entattr1", e1a1val, "mask", NULL, "attr2", e1a1val, NULL);

            obj = probe_obj_creat ("test_object", oatt,
                                   "ent1", e1att, e1val,
                                   NULL);

            SEXP_free (a1val);
            SEXP_free (e1a1val);
            SEXP_free (e1val);
            SEXP_free (oatt);
            SEXP_free (e1att);
        }

        print_asdf (obj);

        {
            ent = probe_obj_getent(obj, "ent1", 1);
            if (ent == NULL)
                FAIL(1, "`ent1' not found!\n");
            if (!probe_ent_attrexists(ent, "mask"))
                FAIL(1, "`mask' attribute not found in obj!\n");
            SEXP_free(ent);
        }
        SEXP_free(obj);

        return (0);
}
