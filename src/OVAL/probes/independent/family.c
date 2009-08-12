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
#include <probe.h>

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

	lst = SEXP_list_new();
	attrs = SEXP_OVALattr_create("id", SEXP_string_newf("-1"), // todo: id
				     "status", SEXP_number_newd(OVAL_STATUS_EXISTS),
				     NULL);
	item = SEXP_OVALobj_create("family_item",
				   attrs,
				   "family", NULL,
				   SEXP_string_newf(family),
				   NULL);
	SEXP_OVALobj_setelmstatus(item, "family", 1, OVAL_STATUS_EXISTS);
	SEXP_list_add(lst, item);

	*err = 0;
	return lst;
}
