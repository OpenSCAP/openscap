#ifndef __STUB_PROBE
/*
 * family probe:
 *
 *  family_object
 *
 *  family_item
 *    attrs
 *      id
 *      status_enum status
 *    [0..1] string family
 */

#include <config.h>
#include <seap.h>
#include <probe-api.h>

SEXP_t *probe_main(SEXP_t *probe_in, int *err)
{
	SEXP_t *lst, *item, *attrs;
	const char *family =
#       if defined PLATFORM_IOS
		"ios";
#       elif defined PLATFORM_MACOS
		"macos";
#       elif defined PLATFORM_UNIX
		"unix";
#       elif defined PLATFORM_WINDOWS
		"windows";
#       else
		"error";
#       endif

	if (probe_in == NULL) {
		*err = PROBE_EINVAL;
		return NULL;
	}

	lst = SEXP_list_new(NULL);
                
        attrs = probe_attr_creat ("id",     SEXP_string_newf ("-1"),
                                  "status", SEXP_number_newi_32 (OVAL_STATUS_EXISTS),
                                  NULL);        
        item  = probe_item_creat ("family_item", attrs,
                                  /* value */
                                  "family", SEXP_string_newf (family),
                                  NULL);
        probe_itement_setstatus (item, "family", 1, OVAL_STATUS_EXISTS); /* TODO: status = exists -> default ? */

        /*
        item = probe_item_build ("(family_item :id %s :status %s) ((family :status %s) %s)",
                                 SEXP_string_newf ("-1"),
                                 SEXP_number_newi_32 (OVAL_STATUS_EXISTS),
                                 SEXP_number_newi_32 (OVAL_STATUS_EXISTS),
                                 SEXP_string_newf (family));
        */

	SEXP_list_add(lst, item);
        
        SEXP_free (attrs);
        SEXP_free (item);
        
	*err = 0;
	return lst;
}
#endif
