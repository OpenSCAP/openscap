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

SEXP_t *probe_main(SEXP_t *probe_in, int *err, void *arg)
{
	SEXP_t *list, *item, *v_fm;

        (void)arg;
        
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

        item  = probe_item_creat ("family_item", NULL,
                                  /* entities */
                                  "family", NULL, v_fm = SEXP_string_newf (family),
                                  NULL);
        
	list = SEXP_list_new (item, NULL);
        
        SEXP_free (item);
        SEXP_free (v_fm);
        
	*err = 0;
	return (list);
}
#endif
