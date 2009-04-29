#include <seap.h>
#include <sexp-printf.h>
#include <assert.h>
#include <xmalloc.h>
#include "probe.h"

#ifndef _A
#define _A(x) assert(x)
#endif

/*
 * Examples of S-exp OVAL object (XML) representation:
 *
 * 1) object without attributes
 *    (rpminfo_state (name "foo") (arch "i386") ...)
 * 
 * 2) object with attributes
 *    ((rpminfo_state :attr1 "attrval") (name "foo") (arch "i386") ...)
 */

SEXP_t *SEXP_OVALobj_create (const char *obj_name, OVALobj_attr_t obj_attrs[], ...)
{
        SEXP_t *sexp = NULL, *sexp_tmp;
        uint16_t i;
        
        _A(obj_name != NULL);
        
        sexp = SEXP_list_new ();
        
        if (obj_attrs == NULL) {
                SEXP_list_add (sexp, SEXP_string_new (obj_name));
        } else {
                
                sexp_tmp = SEXP_list_new ();
                SEXP_list_add (sexp_tmp, SEXP_string_new (obj_name));

                /* Add object attributes */
                for (i = 0; obj_attrs[i] != NULL; ++i) {
                        SEXP_list_add (sexp_tmp,
                                       SEXP_string_new (obj_attrs[i].name));
                
                        /*
                         * Value isn't mandatory. The convention here
                         * is that if an attribute has a value, then the
                         * attribute name must begin with colon.
                         */
                        if (obj_attrs[i].value != NULL) {
                                SEXP_list_add (sexp_tmp,
                                               SEXP_string_new(obj_attrs[i].value));
                        }
                }
                
                SEXP_list_add (sexp, sexp_tmp);
        }
        
        /* Add object elements */
        

        return (sexp);
}
